/*===========hmrV2500.cpp===========
HomuraViewer v1_07/160808
	a
		ほむらのディレクトリ構造を一新

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
#include"IO/DxIO_v2.hpp"
#include"IO/DxGateSwitcher.hpp"

#include"IO.hpp"
#include"Message.hpp"

#include"Thermo.hpp"
#include"Camera.hpp"
#include"CO2.hpp"
#include"FullADC.hpp"

#include"Motor.hpp"
#include"Accele.hpp"
#include"Compass.hpp"
#include"Gyro.hpp"

#include"GPS.hpp"
#include"Battery.hpp"
#include"UniSensor.hpp"

#include"DeviceManage.hpp"
#include"File.hpp"

#include"View.hpp"
#include"Controller.hpp"

#include"Resource.hpp"

#include<hmLib_v2/hmLib.cpp>

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow){
	namespace hmrv = hmr::viewer;

	SetWindowIconID(ID_ICON);

	//hmLib_dxモジュールを初期化
	dx::ini("HomuraViewer_v1_07e", 960,720);

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
		Message.regist('j', Camera.MsgAgent);
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

		//画面表示モジュール
		hmrv::cView View(IO,Message, FullADC, Accele,Compass,Gyro,Camera,GPS,Battery);

		View.registSUI(DeviceManage.SUI);
		View.registSUI(IO.GateSwSUI);
		View.registSUI(IO.BufGateSUI);
		View.registSUI(IO.ioLogGateSUI);
		View.registSUI(IO.IODriverSUI);
		View.registSUI(IO.VMCSUI);
		View.registSUI(Message.ComSUI);
		View.registSUI(Message.MessageDriverSUI);
		View.registSUI(File.SUI);

		View.registMUI(Motor.MUI);
		View.registMUI(Battery.MUI);
		View.registMUI(Accele.MUI);
		View.registMUI(Compass.MUI);
		View.registMUI(Gyro.MUI);
		View.registMUI(GPS.MUI);
		View.registMUI(Thermo.MUI);
		View.registMUI(Camera.MUI);
		View.registMUI(CO2.MUI);
		View.registMUI(FullADC.MUI);

		Controller.connect_Keyboard(View);

		while(!dx::work(30)){
			Controller();
			IO();
			Message();
			View();

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
