/*===========hmrV2500.cpp===========

=== hmrV2500_OntakeL ===
v1_00/141029 hmIto
	長期観測用機能を追加したバージョン

=== hmrV2500===
v1_05/130914
	r
		GPSのPower制御を追加
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

v1_04/130803
	ファイル機能を実装開始

v1_03/130713 
	実現できていること
	　・基本的な機能の実装
	　


*/
#include<hmLibVer.hpp>
#include<hmLib/config_vc.h>
#include<hmLib/bufgate.hpp>
#include<hmLib/any_iterator.hpp>
#include<hmLib_v2/dxColorSet.hpp>
#include"iologgate.hpp"
#include"predicate.hpp"
#include"GateSwitcher.hpp"
#include"IO.hpp"
#include"BufGate.hpp"
#include"Com.hpp"
#include"ComLog.hpp"
#include"DXCom_v2.hpp"
#include"Operator.hpp"
#include"Message.hpp"
#include"DxKeyboard.hpp"
#include"DxPad.hpp"

//#include"hmrDxIODisplay.hpp"
#include"DxControlMainDisplay.hpp"
#include"DxDisplay.hpp"

#include"Thermo.hpp"
#include"DxThermoMUI.hpp"

#include"SHT75.hpp"
#include"DxSHT75MUI.hpp"

#include"Sprite.hpp"
#include"DxSpriteMUI.hpp"

#include"CO2.hpp"
#include"DxCO2MUI.hpp"

#include"GPS.hpp"
#include"DxGPSMUI.hpp"
#include"DxGPSMap.hpp"
#include"GPSKashmir.hpp"

#include"Battery.hpp"
#include"DxBatteryMUI.hpp"

#include"H2S.hpp"
#include"DxH2SMUI.hpp"

#include"InfraRed.hpp"
#include"DxInfraRedMUI.hpp"

#include"Motor.hpp"
#include"DxMotorMUI.hpp"

#include"Compass.hpp"
#include"CompassData.hpp"
#include"DxCompassMUI.hpp"

#include"Accele.hpp"
#include"AcceleLogger.hpp"
#include"DxAcceleMUI.hpp"

#include"Gyro.hpp"
#include"GyroCompass.hpp"
#include"DxGyroMUI.hpp"

#include"DeviceManage.hpp"
#include"DxDeviceManageSUI.hpp"


#include"DxComSUI.hpp"
#include"DxBufGateSUI.hpp"
#include"DxGateSwitcherSUI.hpp"
#include"DxIOLogGateSUI.hpp"
#include"DxIOSUI.hpp"
#include"DxIO_v2.hpp"
#include"DxOperatorSUI.hpp"
#include"DxVMCSUI.hpp"
#include"DXFileSUI.hpp"
#include"DxBUIBoxSideDisp.hpp"


#include "DirectoryFile.hpp"
#include "GPSFile.hpp"
#include "SpriteFile.hpp"
#include "CSVFile.hpp"
/*
#include "hmrWholeFile.hpp"
#include "hmrAcceleFile.hpp"
#include "hmrBatteryFile.hpp"
#include "hmrCO2File.hpp"
#include "hmrCompassFile.hpp"
#include "hmrGPSFile.hpp"
#include "hmrGyroFile.hpp"
#include "hmrH2SFile.hpp"
#include "hmrHumidFile.hpp"
#include "hmrInfraRedFile.hpp"
#include "hmrSensorsFile.hpp"
#include "hmrThermoFile.hpp"
#include "hmrSpriteFile.hpp"
*/

//test 

#include "Autolog/AutoLogger.hpp"
#include "Autolog/ThermoAutoLoggerClient.hpp"
#include "Autolog/BatteryAutoLoggerClient.hpp"
#include "Autolog/CO2AutoLoggerClient.hpp"
#include "Autolog/SpriteAutoLoggerClient.hpp"
#include "DxAutolog.hpp"


#include<hmLib_v2/hmLib.cpp>

#define HMR_MAIN_INC_END

#include"ConnectDx.hpp"
#include"ConnectModule.hpp"
#include"ConnectCore.hpp"
#include"ConnectKeyboard.hpp"
#include"ConnectPad.hpp"
#include"ConnectFile.hpp"
#include"ConnectAutolog.hpp"


//#include<hmLib/lab/virtual_com/virtual_comgate.hpp>
//#include<hmLib/lab/virtual_com/fdx_virtual_com.hpp>
//#include"DummyPulg_v1.hpp"

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow){
	//hmLib_dxモジュールを初期化
	dx::ini("hmrV2500_OntakeL_v1_00", 960,720);

	try{
		//hmLib::vcom::fdx_virtual_com VCom;
		//VCom.start(9600);

		//hmr::dummy_plug_v1 DummyPlug;
		//std::thread Thread(std::ref(DummyPlug));
		//DummyPlug.VComGate.open(VCom);

		//hmLib::vcom::virtual_comgate vcomgate;
		//vcomgate.open(VCom);

		//制御系デバイス
		hmr::cDxKeyboard Keyboard;
		hmr::cDxPad1 Pad1;

		//Com, Message, Operatorを宣言
		hmr::cCom Com;
		hmr::cCom::VMC1Creater<1> ComVMC(&Com);
		hmr::cComLog ComLog;
		hmr::connect(ComLog,Com);

		//IO(VMC), ioLogGate, GateSWを宣言
		//typedef std::pair<bool,system_clock_iologtype> io_iologtype;
		hmr::cFHdxIO IO(ComVMC);
		hmr::bufgate Bufgate;

		//ioLogBufを宣言
		iologgate<fdx_crlf_timeout_iologger<system_clock_iologtype>> ioLogGate;
		typedef fdx_vector_iologbuf<system_clock_iologtype> iologbuf;
		iologbuf ioLogBuf;
		hmr::connect(ioLogBuf,ioLogGate);

		//GateSwitcherを宣言
		hmr::cGateSwitcher GateSW;
		GateSW.FomaSetting.Baudrate = 38400;
		GateSW.FomaSetting.PhoneNo = "08024766917";

//		IO.open(&ioLogGate);
//		IO.open(&Bufgate);
//		Bufgate.open(ioLogGate);
//		ioLogGate.open(GateSW);
		IO.open(&ioLogGate);
		iologgate<fdx_crlf_timeout_iologger<system_clock_iologtype>> ioLogGate2;

		ioLogGate.open(ioLogGate2);
		ioLogGate2.open(Bufgate);
		Bufgate.open(GateSW);

		//Message, Operator
		hmr::cMessage Message;
		hmr::cFHDxOperator Operator(&Message,&Com,true,std::chrono::milliseconds(250),std::chrono::seconds(1));
		hmr::connect(Operator,IO,Com);

		//各モジュール宣言		
		hmr::cMotorMsgAgent MotorMA;
		hmr::connect_Pad(MotorMA,Pad1);
		Message.regist('m',&MotorMA);

		hmr::cBatteryMsgAgent<3> BatteryMA;
		Message.regist('b',&BatteryMA);
		hmr::connect_Pad(BatteryMA,Pad1);
		
//		hmr::connect_Keyboard(BatteryMA,Keyboard);	

		hmr::cAcceleMsgAgent AcceleMA;
		Message.regist('a',&AcceleMA);
		hmr::connect_Pad(AcceleMA,Pad1);
		hmr::cAcceleLogger AcceleLogger;
		AcceleLogger.slot_addLog(AcceleMA.signal_newData);
//		hmr::connect_Keyboard(AcceleMA,Keyboard);

		hmr::cCompassMsgAgent CompassMA;
		hmr::cCompass CompassDat;
		Message.regist('c',&CompassMA);
		hmr::connect_Pad(CompassMA,Pad1);
		hmr::connect(CompassDat, CompassMA);

		hmr::cGyroMsgAgent GyroMA;
		hmr::cGyroLogger GyroLogger;
		hmr::connect_Pad(GyroMA,Pad1);
		hmr::connect(GyroLogger,GyroMA);
		Message.regist('G',&GyroMA);
		hmr::cGyroCompass GyroCompass;
		hmr::connect(GyroCompass,GyroMA);

		hmr::cGPSMsgAgent GPSMA;
		hmr::cGPSKashmir GPSKashmir;
		hmr::connect_Pad(GPSMA,Pad1);
		hmr::connect(GPSKashmir,GPSMA);
		Message.regist('g',&GPSMA);

		hmr::cSpriteMsgAgent SpriteMA;
		hmr::connect_Pad(SpriteMA,Pad1);
		Message.regist('j',&SpriteMA);

		hmr::cThermoMsgAgent ThermoMA;
		hmr::connect_Pad(ThermoMA,Pad1);
		Message.regist('t',&ThermoMA);

//		hmr::cSHT75MsgAgent SHT75MA;
//		hmr::connect_Pad(SHT75MA,Pad1);
//		Message.regist('7',&SHT75MA);

//		hmr::cInfraRedMsgAgent InfraRedMA;
//		hmr::connect_Pad(InfraRedMA,Pad1);
//		Message.regist('T',&InfraRedMA);

		hmr::cCO2MsgAgent CO2MA;
		hmr::connect_Pad(CO2MA,Pad1);
		Message.regist('C',&CO2MA);

//		hmr::cH2SMsgAgent H2SMA;
//		hmr::connect_Pad(H2SMA,Pad1);
//		Message.regist('S',&H2SMA);

//		hmr::cHumidMsgAgent HumidMA;
//		hmr::connect_Pad(HumidMA,Pad1);
//		Message.regist('h',&HumidMA);

		hmr::cDevMngMsgAgent DevMngMA;
		Message.regist('D', &DevMngMA);

		hmr::cDirectoryFile DirectoryFile;
/*
		//パケット単位でセンサーデータを保存
		hmr::cCSVFileAgent PacketFileAgent("Packet");
		PacketFileAgent.slot_write(Com.signal_finRecvPacket);
		
		hmr::cCSVFileAgent::cCell<hmr::clock::time_point> TimePointCell("Time");
		Com.contactLastRecvPacTime(TimePointCell.Inquiry);
		PacketFileAgent.regist(TimePointCell);

		hmr::cCSVFileAgent::cCell<double> ThermoCell("Thermo");
		ThermoMA.contact_getTemperature(ThermoCell.Inquiry);
		PacketFileAgent.regist(ThermoCell);

		hmr::cCSVFileAgent::cCell<double> CO2Cell("CO2");
		CO2MA.contact_getValue(CO2Cell.Inquiry);
		PacketFileAgent.regist(CO2Cell);
*/

		//センサーデータのみ保存
//		hmr::cCSVFileAgent SenserFileAgent("Sensor");

		//GPGGAデータを保存
		hmr::cGPSFileAgent GPSFileAgent;
		hmr::connect(GPSFileAgent, GPSMA);
		DirectoryFile.regist(&GPSFileAgent);

		//カメラデータを保存
		hmr::cSpriteFileAgent SpriteFileAgent;
		hmr::connect(SpriteFileAgent, SpriteMA);
		DirectoryFile.regist(&SpriteFileAgent);


		//自動観測モジュール
		hmr::autolog::cAutoLogger AutoLogger;
		hmr::autolog::connect(AutoLogger, DevMngMA, Com, Operator, GateSW);

		hmr::autolog::cThermoAutoLoggerClient ThermoAutoLogger(3);
		hmr::autolog::connect(ThermoAutoLogger, ThermoMA);
		AutoLogger.regist(ThermoAutoLogger);

		hmr::autolog::cBatteryAutoLoggerClient<3> BatteryAutoLogger(3);
		hmr::autolog::connect(BatteryAutoLogger, BatteryMA);
		AutoLogger.regist(BatteryAutoLogger);

		hmr::autolog::cCO2AutoLoggerClient CO2AutoLogger(3);
		hmr::autolog::connect(CO2AutoLogger, CO2MA);
		AutoLogger.regist(CO2AutoLogger);

		hmr::autolog::cSpriteAutoLoggerClient SpriteAutoLogger(1,6,18);
		hmr::autolog::connect(SpriteAutoLogger, SpriteMA);
		AutoLogger.regist(SpriteAutoLogger);

/*
		// File 系列の宣言
		hmr::cWholeFileAgent WholeFA;
		
		hmr::cAcceleFileAgent AcceleFA;
		hmr::connect(AcceleFA,AcceleMA);
		WholeFA.regist(&AcceleFA);

		hmr::cBatteryFileAgent<3> BatteryFA;
		hmr::connect(BatteryFA, BatteryMA);
		WholeFA.regist(&BatteryFA);

		hmr::cCompassFileAgent CompassFA;
		hmr::connect(CompassFA, CompassMA);
		WholeFA.regist(&CompassFA);

		hmr::cGPSFileAgent GPSFA;
		hmr::connect(GPSFA, GPSMA);
		WholeFA.regist(&GPSFA);
		
		hmr::cGyroFileAgent GyroFA;
		hmr::connect(GyroFA, GyroMA);
		WholeFA.regist(&GyroFA);
		
//		hmr::cSensorsFileAgent SensorFA;
//		hmr::connect(SensorFA, CO2MA, H2SMA, HumidMA, InfraRedMA, ThermoMA);
//		WholeFA.regist(&SensorFA);
		
		hmr::cSpriteFileAgent SpriteFA;
		hmr::connect(SpriteFA, SpriteMA);
		WholeFA.regist(&SpriteFA);
		
*/
		// SUI 系列
		hmr::dxosBUIBoxSideDisplay SystemSideDisp;
		SystemSideDisp.ClrSet.Background=CLR::DarkSoftBlue;

		hmr::dxosDevMngSUI DevMngSUI;
		hmr::connect(DevMngSUI, DevMngMA);
		SystemSideDisp.insert(&DevMngSUI);

		hmr::dxosGateSwitcherSUI GateSwSUI;
		hmr::connect(GateSwSUI, GateSW);
		SystemSideDisp.insert(&GateSwSUI);

		hmr::dxosBufGateSUI BufGateSUI;
		hmr::connect(BufGateSUI, Bufgate);
		SystemSideDisp.insert(&BufGateSUI);

		hmr::dxosIOLogGateSUI LogSUI;
		hmr::connect(LogSUI, ioLogGate, ioLogBuf);
		SystemSideDisp.insert(&LogSUI);

		hmr::dxosIOSUI IOSUI;
		hmr::connect(IOSUI, IO);
		SystemSideDisp.insert(&IOSUI);

		hmr::dxosVMCSUI VMCSUI;
		hmr::connect(VMCSUI, IO);
		SystemSideDisp.insert(&VMCSUI);


		hmr::dxosComSUI ComBUI;
		hmr::connect(ComBUI,Com);
		SystemSideDisp.insert(&ComBUI);

		hmr::dxosOperatorSUI OpSUI;
		hmr::connect(OpSUI, Operator);
		SystemSideDisp.insert(&OpSUI);

		hmr::dxosFileSUI FileSUI;
		hmr::connect(FileSUI, DirectoryFile);
		SystemSideDisp.insert(&FileSUI);


		// IO View Display の定義
		hmr::dxosIO2<iologbuf::iterator> IOMainDisp(Pint(720,720), CLR::DarkDullGreen,CLR::SoftGreen,CLR::LightSoftOrenge,CLR::LightSoftSkyblue);
		//hmr::dxosIOSubPage IOSideDisp(Pint(240,720), CLR::DarkDullGreen);
		hmr::connect(IOMainDisp,IO,ioLogBuf,ioLogGate.Logger);
		//hmr::connect(IOSideDisp, GateSW,Operator,Bufgate);

		//操縦用MainDisplaay
		hmr::dxosControlMainDisplay ControlMainDisp;
//		hmr::connect(ControlMainDisp.Navigator,AcceleMA,CompassDat,GyroLogger);
		hmr::connect(ControlMainDisp.Navigator,AcceleLogger,CompassDat,GyroLogger,GyroCompass);
		hmr::connect(ControlMainDisp.Sprite,SpriteMA);
		hmr::connect(ControlMainDisp.Infomation,GPSKashmir,BatteryMA);
		hmr::connect(ControlMainDisp.Autolog, AutoLogger,ThermoAutoLogger,BatteryAutoLogger,CO2AutoLogger,SpriteAutoLogger);
		std::vector<hmr::datum::id_type> SwIDList;
		SwIDList.push_back('a');
		SwIDList.push_back('b');
		SwIDList.push_back('c');
		SwIDList.push_back('C');
		SwIDList.push_back('g');
		SwIDList.push_back('G');
		SwIDList.push_back('j');
		SwIDList.push_back('m');
		SwIDList.push_back('t');
		hmr::dxosPacketList_withView<hmr::cComLog::iterator> PacketMainDisp(Pint(720,720),30,360,SwIDList,CLR::DullOrenge);
		hmr::connect(PacketMainDisp,Com,ComLog);

		//MUI用サイドバー宣言

//		hmr::dxosBUI::DefaultClr.set(CLR::DarkSoftRed,CLR::SoftRed,CLR::LightSoftBlue,CLR::DeepSoftGreen,CLR::SoftRed,CLR::White,CLR::White,CLR::LightSoftRed,CLR::White,CLR::LightGray,CLR::Gray);
		hmr::dxosBUIBoxSideDisplay MUISideDisp;
		MUISideDisp.ClrSet.Background=CLR::DarkSoftYellow;

		hmr::dxosMotorMUI MotorMUI;
		hmr::connect(MotorMUI,MotorMA);
		MUISideDisp.insert(&MotorMUI);

		hmr::dxosBatteryMUI<3> BatteryMUI;
		hmr::connect(BatteryMUI,BatteryMA);
		MUISideDisp.insert(&BatteryMUI);

		hmr::dxosAcceleMUI AcceleMUI;
		hmr::connect(AcceleMUI,AcceleMA);
		MUISideDisp.insert(&AcceleMUI);

		hmr::dxosCompassMUI CompassMUI;
		hmr::connect(CompassMUI,CompassMA);
		MUISideDisp.insert(&CompassMUI);

		hmr::dxosGyroMUI GyroMUI;
		hmr::connect(GyroMUI,GyroMA);
		MUISideDisp.insert(&GyroMUI);

		hmr::dxosGPSMUI GPSMUI;
		hmr::connect(GPSMUI,GPSMA);
		MUISideDisp.insert(&GPSMUI);

		hmr::dxosSpriteMUI SpriteMUI;
		hmr::connect(SpriteMUI,SpriteMA);
		MUISideDisp.insert(&SpriteMUI);

		hmr::dxosThermoMUI ThermoMUI;
		hmr::connect(ThermoMUI,ThermoMA);
		MUISideDisp.insert(&ThermoMUI);

//		hmr::dxosSHT75MUI SHT75MUI;
//		hmr::connect(SHT75MUI,SHT75MA);
//		MUISideDisp.insert(&SHT75MUI);

//		hmr::dxosInfraRedMUI InfraRedMUI;
//		hmr::connect(InfraRedMUI,InfraRedMA);
//		MUISideDisp.insert(&InfraRedMUI);

		hmr::dxosCO2MUI CO2MUI;
		hmr::connect(CO2MUI,CO2MA);
		MUISideDisp.insert(&CO2MUI);

//		hmr::dxosH2SMUI H2SMUI;
//		hmr::connect(H2SMUI,H2SMA);
//		MUISideDisp.insert(&H2SMUI);

//		hmr::dxosHumidMUI HumidMUI;
//		hmr::connect(HumidMUI,HumidMA);
//		MUISideDisp.insert(&HumidMUI);

		hmr::dxosDisplay Display(Pint(720,720),Pint(240,720));
		Display.registMain(&IOMainDisp);
		Display.registMain(&PacketMainDisp);
		Display.registMain(&ControlMainDisp);
		Display.registSide(&SystemSideDisp);
		Display.registSide(&MUISideDisp);	
		//Display.registSide(&IOSideDisp);
	
		Display.slot_setMainNo0(Keyboard.signal(hmLib::predicate_and(hmr::is_key_pushed(KEY::NUM1),hmLib::predicate_not(hmr::have_key_pushed(KEY::SHIFT)))));
		Display.slot_setMainNo1(Keyboard.signal(hmLib::predicate_and(hmr::is_key_pushed(KEY::NUM2),hmLib::predicate_not(hmr::have_key_pushed(KEY::SHIFT)))));
		Display.slot_setMainNo2(Keyboard.signal(hmLib::predicate_and(hmr::is_key_pushed(KEY::NUM3),hmLib::predicate_not(hmr::have_key_pushed(KEY::SHIFT)))));
		Display.slot_setSideNo0(Keyboard.signal(hmLib::predicate_and(hmr::is_key_pushed(KEY::NUM1),hmr::have_key_pushed(KEY::SHIFT))));
		Display.slot_setSideNo1(Keyboard.signal(hmLib::predicate_and(hmr::is_key_pushed(KEY::NUM2),hmr::have_key_pushed(KEY::SHIFT))));
		Display.slot_setSideNo2(Keyboard.signal(hmLib::predicate_and(hmr::is_key_pushed(KEY::NUM3),hmr::have_key_pushed(KEY::SHIFT))));

		Display.slot_plusMainNo(Pad1.signal(hmLib::predicate_and(hmr::is_pad1_cross_key_pulled(PAD::CROSS_KEY::RIGHT),hmLib::predicate_not(hmr::have_pad1_pushed(PAD::But7)))));
		Display.slot_minusMainNo(Pad1.signal(hmLib::predicate_and(hmr::is_pad1_cross_key_pulled(PAD::CROSS_KEY::LEFT),hmLib::predicate_not(hmr::have_pad1_pushed(PAD::But7)))));
		Display.slot_plusSideNo(Pad1.signal(hmLib::predicate_and(hmr::is_pad1_cross_key_pulled(PAD::CROSS_KEY::RIGHT),hmr::have_pad1_pushed(PAD::But7))));
		Display.slot_minusSideNo(Pad1.signal(hmLib::predicate_and(hmr::is_pad1_cross_key_pulled(PAD::CROSS_KEY::LEFT),hmr::have_pad1_pushed(PAD::But7))));

/*
		struct cDebug{
			bool IsTimeout;
			hmr::clock::time_point Timeout;
			bool IsNullData;
			hmr::clock::time_point NullData;
			cDebug():IsTimeout(false),IsNullData(false){}
			void operator()(void){
				auto Time=hmr::clock::now();
				if(Time-Timeout>std::chrono::seconds(3)){
					if(!IsTimeout){
						IsTimeout=true;
						PlaySoundFile("hmr/sound/error1.wav",DX_PLAYTYPE_LOOP);
					}
				}else if(Time-NullData<std::chrono::seconds(3)){
					if(!IsNullData){
						IsNullData=true;
						PlaySoundFile("hmr/sound/error2.wav",DX_PLAYTYPE_LOOP);
					}
				}else{
					IsTimeout=false;
					IsNullData=false;
					StopSoundFile();
				}
			}
			void slot_timeout(boost::signals2::signal<void(hmr::clock::time_point)>& Signal_){
				hmLib::signals::connect(Signal_,[&](hmr::clock::time_point Time_)->void{this->Timeout=Time_;});
			}
			void slot_nulldata(boost::signals2::signal<void(hmr::clock::time_point)>& Signal_){
				hmLib::signals::connect(Signal_,[&](hmr::clock::time_point Time_)->void{this->NullData=Time_;});
			}
		}Debug;
		boost::signals2::signal<void(hmr::clock::time_point)> DebugSignal;
		Debug.slot_timeout(Operator.signal_inform_Received);
		Debug.slot_nulldata(BatteryMA.signal_nulldata);
*/
		while(!dx::work(30)){
			Keyboard.work();
			Pad1.work();

			Bufgate();
			IO.work();
			Operator();

//			Debug();
//			DebugSignal(hmr::clock::now());

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
