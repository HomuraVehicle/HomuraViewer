/*===========hmrV2500.cpp===========
hmrV2500 v1_06/160601
	a
		Git移動後初めてコンパイルに成功
	c
		HomuraViewer導入 Data, File, VMC1を移動
	d
		ディレクトリ構造を変更　Dataディレクトリ直下にすべてのデータが出るように修正
		合わせて、温度計の時間未更新バグを更新
		出力ファイルもtsv形式に更新
		Dateも出力するようにした
	e
		AcceleLoggerの角度が過敏に反応する問題を修正
	f
		FullADC機能を実装
	g
		Add foma settingh function
	h
		新しいディレクトリ構成に移行
hmrV2500 v1_05/130914
	p
		表示位置等修正
	n
		コンパスに複数補正機能を追加
	h
		Operatorをデバッグ
	g
		ファイル機能を一度外す
		半二重通信への対応
	ファイル機能を実装完了
hmrV2500 v1_04/130803
	ファイル機能を実装開始

hmrV2500 v1_03/130713 
	実現できていること
	　・基本的な機能の実装
	　
*/

#include"hmLibVer.hpp"
#include <hmLib/config_vc.h> // std::min, std::max　は　windowsマクロでも定義されており、明示的に分離してやらないとだめ
#include <hmLib/bufgate.hpp>
#include <hmLib/any_iterator.hpp>
#include <hmLib_v2/dxColorSet.hpp>
#include"iologgate.hpp"
#include"predicate.hpp"
#include"hmrBufGate.hpp"
#include"hmrGateSwitcher.hpp"
#include"hmrIO.hpp"
#include"hmrCom.hpp"
#include"hmrComLog.hpp"
#include"hmrDXCom_v2.hpp"
#include"hmrOperator.hpp"

#include"Message.hpp"



#include"hmrDxControlMainDisplay.hpp"
#include"hmrDxDisplay.hpp"

#include"hmrThermo.hpp"
#include"hmrDxThermoMUI.hpp"

#include"hmrSHT75.hpp"
#include"hmrDxSHT75MUI.hpp"

#include"hmrSprite.hpp"
#include"hmrDxSpriteMUI.hpp"

#include"hmrCO2.hpp"
#include"hmrDxCO2MUI.hpp"

#include"hmrH2S.hpp"
#include"hmrDxH2SMUI.hpp"

#include"hmrInfraRed.hpp"
#include"hmrDxInfraRedMUI.hpp"

#include"hmrMotor.hpp"
#include"hmrDxMotorMUI.hpp"

#include "Accele.hpp"
#include "Compass.hpp"
#include "Gyro.hpp"

#include"GPS.hpp"
#include"Battery.hpp"
#include"UniSensor.hpp"

#include "FullADC.hpp"

#include "hmrDeviceManage.hpp"
#include "hmrDxDeviceManageSUI.hpp"

#include "hmrChrono.hpp"
#include "hmrLoggerManage.hpp"
#include "hmrDxLoggerManageSUI.hpp"

#include "hmrDxComSUI.hpp"
#include "hmrDxBufGateSUI.hpp"
#include "hmrDxGateSwitcherSUI.hpp"
#include "hmrDxIOLogGateSUI.hpp"
#include "hmrDxIOSUI.hpp"
#include "hmrDxIO_v2.hpp"
#include "hmrDxOperatorSUI.hpp"
#include "hmrDxVMCSUI.hpp"
#include "hmrDXFileSUI.hpp"
#include "hmrDxChrono.hpp"
#include "hmrDxBUIBoxSideDisp.hpp"

#include <HomuraViewer/File.hpp>
#include "hmrSpriteFile.hpp"

#include "Resource.hpp"


#include<hmLib_v2/hmLib.cpp>

#define HMR_MAIN_INC_END

#include"hmrConnectDx.hpp"
#include"hmrConnectSUI.hpp"
#include"hmrConnectMUI.hpp"
#include"hmrConnectModule.hpp"
#include"hmrConnectCore.hpp"
#include"hmrConnectFile.hpp"
#include"Controller.hpp"

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow){
	namespace hmrv = hmr::viewer;

	SetWindowIconID(ID_ICON);

	//hmLib_dxモジュールを初期化
	dx::ini("hmrV2500_v1_06g", 960,720);


	try{
		//Com, Message, Operatorを宣言
		hmrv::cCom Com;
		hmrv::cCom::VMC1Creater<1> ComVMC(&Com);
		hmrv::cComLog ComLog;
		hmrv::connect(ComLog,Com);

		hmrv::cFHdxIO IO(ComVMC);
		hmrv::bufgate Bufgate;

		//ioLogBufを宣言
		iologgate<fdx_crlf_timeout_iologger<system_clock_iologtype>> ioLogGate;
		typedef fdx_vector_iologbuf<system_clock_iologtype> iologbuf;
		iologbuf ioLogBuf;
		hmrv::connect(ioLogBuf,ioLogGate);

		//GateSwitcherを宣言
		hmrv::cGateSwitcher GateSW;
		GateSW.readFomaSetting("hmr\\phone_config.txt");

		IO.open(&ioLogGate);
		iologgate<fdx_crlf_timeout_iologger<system_clock_iologtype>> ioLogGate2;

		ioLogGate.open(ioLogGate2);
		ioLogGate2.open(Bufgate);
		Bufgate.open(GateSW);

		//Message, Operator
		hmrv::cMessage Message;
		hmrv::cFHDxOperator Operator(&Message,&Com,true,std::chrono::milliseconds(250),std::chrono::seconds(1));
		hmrv::connect(Operator,IO,Com);

		//各モジュール宣言		
		hmrv::cBattery Battery;
		hmrv::cFullADC FullADC;

		hmrv::cAccele Accele;
		hmrv::cCompass Compass;
		hmrv::cGyro Gyro;
		hmrv::cGPS GPS;

		hmrv::cMotorMsgAgent MotorMA;
		Message.regist('m',&MotorMA);
		Message.regist('b',&(Battery.MsgAgent));
		Message.regist('a', &(Accele.MsgAgent));
		Message.regist('c', &(Compass.MsgAgent));
		Message.regist('G', &(Gyro.MsgAgent));
		Message.regist('g', &(GPS.MsgAgent));

		hmrv::cSpriteMsgAgent SpriteMA;
		Message.regist('j',&SpriteMA);

		hmrv::cThermoMsgAgent ThermoMA;
		Message.regist('t',&ThermoMA);

		hmrv::cCO2MsgAgent CO2MA;
		Message.regist('C',&CO2MA);

		hmrv::cChrono Chrono;
		Message.regist('$', &Chrono);

		hmrv::cDevMngMsgAgent DevMngMA;
		Message.regist('D', &DevMngMA);

		hmrv::cLoggerMngMsgAgent LogMngMA;
		Message.regist('L', &LogMngMA);

		Message.regist('f', &FullADC.MsgAgent);

//		hmrv::cSHT75MsgAgent SHT75MA;
//		hmrv::connect_Pad(SHT75MA,Pad1);
//		Message.regist('7',&SHT75MA);

//		hmrv::cInfraRedMsgAgent InfraRedMA;
//		hmrv::connect_Pad(InfraRedMA,Pad1);
//		Message.regist('T',&InfraRedMA);

//		hmrv::cH2SMsgAgent H2SMA;
//		hmrv::connect_Pad(H2SMA,Pad1);
//		Message.regist('S',&H2SMA);

//		hmrv::cHumidMsgAgent HumidMA;
//		hmrv::connect_Pad(HumidMA,Pad1);
//		Message.regist('h',&HumidMA);



		//制御系デバイス
		hmrv::cController Controller;
		Controller.connect_Pad(MotorMA);
		Controller.connect_Pad(Battery.MsgAgent);
		Controller.connect_Pad(Accele.MsgAgent);
		Controller.connect_Pad(Compass.MsgAgent);
		Controller.connect_Pad(Gyro.MsgAgent);
		Controller.connect_Pad(GPS.MsgAgent);
		Controller.connect_Pad(SpriteMA);
		Controller.connect_Pad(ThermoMA);
		Controller.connect_Pad(CO2MA);


		//親ディレクトリ
		hmrv::cConstNameDirectoryFile DirectoryFile("Data");
		DirectoryFile.regist(&(Battery.FileAgent));
		DirectoryFile.regist(&(FullADC.FileAgent));

		// Thermo データを保存
		hmrv::cConstNameFileAgent<std::pair<double, std::uint16_t>> ThermoFileAgent("Thermo.txt",'\t');
		ThermoFileAgent.slot_log_writeData(ThermoMA.signal_newRawData);
		DirectoryFile.regist(&ThermoFileAgent);

		// CO2 データを保存
		hmrv::cConstNameFileAgent<std::pair<double, std::uint16_t>> CO2FileAgent("CO2.txt", '\t');
		CO2FileAgent.slot_log_writeData(CO2MA.signal_newRawData);
		DirectoryFile.regist(&CO2FileAgent);

		//GPSデータを保存
		DirectoryFile.regist(&(GPS.FileAgent));

		//GPGGAデータを保存
		DirectoryFile.regist(&(GPS.GPGGAFileAgent));

		//カメラデータを保存
		hmrv::cSpriteFileAgent SpriteFileAgent("Sprite");
		hmrv::connect(SpriteFileAgent, SpriteMA,false);
		DirectoryFile.regist(&SpriteFileAgent);

		// SUI 系列
		hmrv::dxosBUIBoxSideDisplay SystemSideDisp;
		SystemSideDisp.ClrSet.Background=CLR::DarkSoftBlue;

		hmrv::dxosChronoSUI ChronoSUI;
		hmrv::connect(ChronoSUI, Chrono);
		SystemSideDisp.regist(&ChronoSUI);

		hmrv::dxosDevMngSUI DevMngSUI;
		hmrv::connect(DevMngSUI, DevMngMA);
		SystemSideDisp.regist(&DevMngSUI);

		hmrv::dxosLoggerMngSUI LogMngSUI;
		hmrv::connect(LogMngSUI, LogMngMA);
		SystemSideDisp.regist(&LogMngSUI);

		hmrv::dxosGateSwitcherSUI GateSwSUI;
		hmrv::connect(GateSwSUI, GateSW);
		SystemSideDisp.regist(&GateSwSUI);

		hmrv::dxosBufGateSUI BufGateSUI;
		hmrv::connect(BufGateSUI, Bufgate);
		SystemSideDisp.regist(&BufGateSUI);

		hmrv::dxosIOLogGateSUI LogSUI;
		hmrv::connect(LogSUI, ioLogGate, ioLogBuf);
		SystemSideDisp.regist(&LogSUI);

		hmrv::dxosIOSUI IOSUI;
		hmrv::connect(IOSUI, IO);
		SystemSideDisp.regist(&IOSUI);

		hmrv::dxosVMCSUI VMCSUI;
		hmrv::connect(VMCSUI, IO);
		SystemSideDisp.regist(&VMCSUI);


		hmrv::dxosComSUI ComBUI;
		hmrv::connect(ComBUI,Com);
		SystemSideDisp.regist(&ComBUI);

		hmrv::dxosOperatorSUI OpSUI;
		hmrv::connect(OpSUI, Operator);
		SystemSideDisp.regist(&OpSUI);

		hmrv::dxosFileSUI FileSUI;
		hmrv::connect(FileSUI, DirectoryFile);
		SystemSideDisp.regist(&FileSUI);


		// IO View Display の定義
		hmrv::dxosIO2<iologbuf::iterator> IOMainDisp(Pint(720,720), CLR::DarkDullGreen,CLR::SoftGreen,CLR::LightSoftOrenge,CLR::LightSoftSkyblue);
		hmrv::connect(IOMainDisp,IO,ioLogBuf,ioLogGate.Logger);

		//操縦用MainDisplaay
		hmrv::dxosControlMainDisplay ControlMainDisp;
		hmrv::connect(ControlMainDisp.Navigator,Accele.Logger,Compass.Compass,Gyro.Logger,Gyro.Compass);
		hmrv::connect(ControlMainDisp.Sprite,SpriteMA);
		hmrv::connect(ControlMainDisp.Infomation,GPS.GPSKashmir,Battery);
		hmrv::connect(ControlMainDisp.GPSMap, GPS.MsgAgent, Compass.Compass);
		std::vector<hmrv::message::datum::id_type> SwIDList;
		SwIDList.push_back('a');
		SwIDList.push_back('b');
		SwIDList.push_back('c');
		SwIDList.push_back('C');
		SwIDList.push_back('g');
		SwIDList.push_back('G');
		SwIDList.push_back('j');
		SwIDList.push_back('m');
		SwIDList.push_back('t');
		hmrv::dxosPacketList_withView<hmrv::cComLog::iterator> PacketMainDisp(Pint(720,720),30,360,SwIDList,CLR::DullOrenge);
		hmrv::connect(PacketMainDisp,Com,ComLog);

		//MUI用サイドバー宣言
		hmrv::dxosBUIBoxSideDisplay MUISideDisp;
		MUISideDisp.ClrSet.Background=CLR::DarkSoftYellow;

		hmrv::dxosMotorMUI MotorMUI;
		hmrv::connect(MotorMUI,MotorMA);
		MUISideDisp.regist(&MotorMUI);

		hmrv::battery::dxosMUI<hmrv::cBattery::BatteryNum> BatteryMUI;
		Battery.connect(BatteryMUI);
		MUISideDisp.regist(&BatteryMUI);

		MUISideDisp.regist(&(Accele.MUI));
		MUISideDisp.regist(&(Compass.MUI));
		MUISideDisp.regist(&(Gyro.MUI));
		MUISideDisp.regist(&(GPS.MUI));

		hmrv::dxosSpriteMUI SpriteMUI;
		hmrv::connect(SpriteMUI,SpriteMA);
		MUISideDisp.regist(&SpriteMUI);

		hmrv::dxosThermoMUI ThermoMUI;
		hmrv::connect(ThermoMUI,ThermoMA);
		MUISideDisp.regist(&ThermoMUI);

		hmrv::dxosCO2MUI CO2MUI;
		hmrv::connect(CO2MUI,CO2MA);
		MUISideDisp.regist(&CO2MUI);

//		hmrv::dxosSHT75MUI SHT75MUI;
//		hmrv::connect(SHT75MUI,SHT75MA);
//		MUISideDisp.insert(&SHT75MUI);

//		hmrv::dxosInfraRedMUI InfraRedMUI;
//		hmrv::connect(InfraRedMUI,InfraRedMA);
//		MUISideDisp.insert(&InfraRedMUI);

//		hmrv::dxosH2SMUI H2SMUI;
//		hmrv::connect(H2SMUI,H2SMA);
//		MUISideDisp.insert(&H2SMUI);

//		hmrv::dxosHumidMUI HumidMUI;
//		hmrv::connect(HumidMUI,HumidMA);
//		MUISideDisp.insert(&HumidMUI);

		hmrv::cFullADC::dxosMUI FullADCMUI;
		FullADC.connect(FullADCMUI);
		MUISideDisp.regist(&FullADCMUI);
		ControlMainDisp.Infomation.slot_logData(FullADC.MsgAgent.signal_newData);

		hmrv::dxosDisplay Display(Pint(720,720),Pint(240,720));
		Display.registMain(&IOMainDisp);
		Display.registMain(&PacketMainDisp);
		Display.registMain(&ControlMainDisp);
		Display.registSide(&SystemSideDisp);
		Display.registSide(&MUISideDisp);	
	
		Display.slot_setMainNo0(Controller.Keyboard.signal(hmLib::predicate_and(hmrv::dxmodule::is_key_pushed(KEY::NUM1),hmLib::predicate_not(hmrv::dxmodule::have_key_pushed(KEY::SHIFT)))));
		Display.slot_setMainNo1(Controller.Keyboard.signal(hmLib::predicate_and(hmrv::dxmodule::is_key_pushed(KEY::NUM2),hmLib::predicate_not(hmrv::dxmodule::have_key_pushed(KEY::SHIFT)))));
		Display.slot_setMainNo2(Controller.Keyboard.signal(hmLib::predicate_and(hmrv::dxmodule::is_key_pushed(KEY::NUM3),hmLib::predicate_not(hmrv::dxmodule::have_key_pushed(KEY::SHIFT)))));
		Display.slot_setSideNo0(Controller.Keyboard.signal(hmLib::predicate_and(hmrv::dxmodule::is_key_pushed(KEY::NUM1),hmrv::dxmodule::have_key_pushed(KEY::SHIFT))));
		Display.slot_setSideNo1(Controller.Keyboard.signal(hmLib::predicate_and(hmrv::dxmodule::is_key_pushed(KEY::NUM2),hmrv::dxmodule::have_key_pushed(KEY::SHIFT))));
		Display.slot_setSideNo2(Controller.Keyboard.signal(hmLib::predicate_and(hmrv::dxmodule::is_key_pushed(KEY::NUM3),hmrv::dxmodule::have_key_pushed(KEY::SHIFT))));

		Display.slot_plusMainNo(Controller.Pad.signal(hmLib::predicate_and(hmrv::dxmodule::is_pad1_cross_key_pulled(PAD::CROSS_KEY::RIGHT),hmLib::predicate_not(hmrv::dxmodule::have_pad1_pushed(PAD::But7)))));
		Display.slot_minusMainNo(Controller.Pad.signal(hmLib::predicate_and(hmrv::dxmodule::is_pad1_cross_key_pulled(PAD::CROSS_KEY::LEFT),hmLib::predicate_not(hmrv::dxmodule::have_pad1_pushed(PAD::But7)))));
		Display.slot_plusSideNo(Controller.Pad.signal(hmLib::predicate_and(hmrv::dxmodule::is_pad1_cross_key_pulled(PAD::CROSS_KEY::RIGHT),hmrv::dxmodule::have_pad1_pushed(PAD::But7))));
		Display.slot_minusSideNo(Controller.Pad.signal(hmLib::predicate_and(hmrv::dxmodule::is_pad1_cross_key_pulled(PAD::CROSS_KEY::LEFT),hmrv::dxmodule::have_pad1_pushed(PAD::But7))));

		while(!dx::work(30)){
			Controller();

			Bufgate();
			IO.work();
			Operator();

			dx::draw(Pint(0,0),Display);

			if(dx::getKey(KEY::ESC)<0)break;
		}
	}catch(const std::exception& Escp){
		printfDx("===ERROR===\n");
		printfDx("%s\n",Escp.what());
		while(!dx::work(30)){
			if(dx::getKey(KEY::ESC)<0)break;
		}
	}catch(const std::string& Escp){
		printfDx("===ERROR===\n");
		printfDx("%s\n",Escp.c_str());
		while(!dx::work(30)){
			if(dx::getKey(KEY::ESC)<0)break;
		}
	}

	dx::end();
	
	return 0;

}
