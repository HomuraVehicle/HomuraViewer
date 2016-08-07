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
#include<hmLib/config_vc.h> // std::min, std::max　は　windowsマクロでも定義されており、明示的に分離してやらないとだめ

#include<hmLib/bufgate.hpp>
#include<hmLib/any_iterator.hpp>
#include<hmLib_v2/dxColorSet.hpp>
#include"predicate.hpp"

#include"Message/DxCom.hpp"

#include"IO.hpp"

#include"Message.hpp"

#include"hmrDxControlMainDisplay.hpp"
#include"hmrDxDisplay.hpp"

#include"Thermo.hpp"
#include"Camera.hpp"
#include"CO2.hpp"
#include "FullADC.hpp"

#include"Motor.hpp"
#include"Accele.hpp"
#include"Compass.hpp"
#include"Gyro.hpp"

#include"GPS.hpp"
#include"Battery.hpp"
#include"UniSensor.hpp"
#include"DeviceManage.hpp"

#include "File.hpp"
#include "hmrDxBUIBoxSideDisp.hpp"

#include "Resource.hpp"

#include "IO/DxIO_v2.hpp"
#include "IO/DxGateSwitcher.hpp"
//#include "hmrDxIODisplay.hpp"

#include<hmLib_v2/hmLib.cpp>
#include"Controller.hpp"
#define HMR_MAIN_INC_END

#include"hmrConnectDx.hpp"
#include"hmrConnectSUI.hpp"


int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow){
	namespace hmrv = hmr::viewer;

	SetWindowIconID(ID_ICON);

	//hmLib_dxモジュールを初期化
	dx::ini("hmrV2500_v1_06g", 960,720);

	try{
		//Message
		hmrv::cMessage Message;


		//IO
		//	PCからの出口を管理する　
		hmrv::cIO IO(Message.ComVMC,Message.MessageDriver);

		//各モジュール宣言		
		hmrv::cBattery Battery;
		hmrv::cFullADC FullADC;
		hmrv::cAccele Accele;
		hmrv::cCompass Compass;
		hmrv::cGyro Gyro;
		hmrv::cGPS GPS;
		hmrv::cMotor Motor;
		hmrv::cThermo Thermo;
		hmrv::cCO2 CO2;
		hmrv::cCamera Camera;
		hmrv::cDeviceManage DeviceManage;

		Message.regist('b', Battery.MsgAgent);
		Message.regist('f', FullADC.MsgAgent);
		Message.regist('a', Accele.MsgAgent);
		Message.regist('c', Compass.MsgAgent);
		Message.regist('G', Gyro.MsgAgent);
		Message.regist('g', GPS.MsgAgent);
		Message.regist('m', Motor.MsgAgent);
		Message.regist('t', Thermo.MsgAgent);
		Message.regist('C', CO2.MsgAgent);
		Message.regist('j',Camera.MsgAgent);
		Message.regist('D', DeviceManage.MsgAgent);

		//制御系デバイス
		hmrv::cController Controller;
		Controller.connect_Pad(Motor.MsgAgent);
		Controller.connect_Pad(Battery.MsgAgent);
		Controller.connect_Pad(Accele.MsgAgent);
		Controller.connect_Pad(Compass.MsgAgent);
		Controller.connect_Pad(Gyro.MsgAgent);
		Controller.connect_Pad(GPS.MsgAgent);
		Controller.connect_Pad(Thermo.MsgAgent);
		Controller.connect_Pad(Camera.MsgAgent);
		Controller.connect_Pad(CO2.MsgAgent);

		//親ディレクトリ
		hmrv::cFile File;

		//Batteryデータ保存
		File.regist(Battery.FileAgent);

		//FullADCデータ保存
		File.regist(FullADC.FileAgent);

		// Thermo データを保存
		File.regist(Thermo.FileAgent);

		// CO2 データを保存
		File.regist(CO2.FileAgent);

		//GPSデータを保存
		File.regist(GPS.FileAgent);

		//GPGGAデータを保存
		File.regist(GPS.GPGGAFileAgent);

		//カメラデータを保存
		File.regist(Camera.FileAgent);


		// SUI 系列
		hmrv::dxosBUIBoxSideDisplay SystemSideDisp;
		SystemSideDisp.ClrSet.Background=CLR::DarkSoftBlue;

		SystemSideDisp.regist(&(DeviceManage.SUI));
		SystemSideDisp.regist(&(IO.GateSwSUI));
		SystemSideDisp.regist(&(IO.BufGateSUI));
		SystemSideDisp.regist(&(IO.ioLogGateSUI));
		SystemSideDisp.regist(&(IO.IODriverSUI));
		SystemSideDisp.regist(&(IO.VMCSUI));
		SystemSideDisp.regist(&(File.SUI));

		// IO View Display の定義
		hmrv::io::dxosIO2<hmrv::cIO::this_iologbuf::iterator> IOMainDisp(Pint(720,720), CLR::DarkDullGreen,CLR::SoftGreen,CLR::LightSoftOrenge,CLR::LightSoftSkyblue);
		hmrv::connect(IOMainDisp,IO.IODriver,IO.ioLogBuf,IO.ioLogGate.Logger);

		//操縦用MainDisplaay
		hmrv::dxosControlMainDisplay ControlMainDisp;
		hmrv::connect(ControlMainDisp.Navigator,Accele.Logger,Compass.Compass,Gyro.Logger,Gyro.Compass);
		hmrv::connect(ControlMainDisp.Camera,Camera.MsgAgent);
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

		hmrv::message::dxosPacketList_withView<hmrv::message::cComLog::iterator> PacketMainDisp(Pint(720,720),30,360,SwIDList,CLR::DullOrenge);
		hmrv::connect(PacketMainDisp,Message.Com, Message.ComLog);

		//MUI用サイドバー宣言
		hmrv::dxosBUIBoxSideDisplay MUISideDisp;
		MUISideDisp.ClrSet.Background=CLR::DarkSoftYellow;

		MUISideDisp.regist(&(Motor.MUI));
		MUISideDisp.regist(&(Battery.MUI));
		MUISideDisp.regist(&(Accele.MUI));
		MUISideDisp.regist(&(Compass.MUI));
		MUISideDisp.regist(&(Gyro.MUI));
		MUISideDisp.regist(&(GPS.MUI));
		MUISideDisp.regist(&(Thermo.MUI));
		MUISideDisp.regist(&(Camera.MUI));
		MUISideDisp.regist(&(CO2.MUI));
		MUISideDisp.regist(&(FullADC.MUI));

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
			IO();
			Message();

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
