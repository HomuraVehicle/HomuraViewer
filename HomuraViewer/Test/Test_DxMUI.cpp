#ifndef HMR_TEST_COMCORE_INC
#define HMR_TEST_COMCORE_INC 100
#
/*===Test_ComCore===
通信系オブジェクトのテスト用ファイル
注意　このプログラムは、COM9を開いている
相手がいないのに開くと、しぬ
*/
#include<hmLib_v3_06/config_vc.h>
#include"hmLibVer.hpp"
#include<iostream>

#include"hmrThermo.hpp"
#include"hmrDxThermoMUI.hpp"

#include"hmrSprite.hpp"
#include"hmrDxSpriteMUI.hpp"

#include"hmrCO2.hpp"
#include"hmrDxCO2MUI.hpp"

#include"hmrGPS.hpp"
#include"hmrDxGPSMUI.hpp"

#include"hmrBattery.hpp"
#include"hmrDxBatteryMUI.hpp"

#include"hmrHumid.hpp"
#include"hmrDxHumidMUI.hpp"

#include"hmrH2S.hpp"
#include"hmrDxH2SMUI.hpp"

#include"hmrInfraRed.hpp"
#include"hmrDxInfraRedMUI.hpp"

#include"hmrMotor.hpp"
#include"hmrDxMotorMUI.hpp"

#include"hmrCompass.hpp"
#include"hmrDxCompassMUI.hpp"

#include"hmrAccele.hpp"
#include"hmrDxAcceleMUI.hpp"

#include"hmrGyro.hpp"
#include"hmrDxGyroMUI.hpp"

//#include"hmrDxAcceleMUI.hpp"
//#include"hmrAccele.hpp"
#include<hmLib_v2/hmLib.cpp>
#include"hmrConnect.hpp"
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow){

	dx::ini("Test",960,720);

	try{
		hmr::dxosMUI::DefaultClr.set(CLR::DarkSoftRed,CLR::SoftRed,CLR::LightSoftBlue,CLR::DeepSoftGreen,CLR::SoftRed,CLR::White,CLR::White,CLR::LightSoftRed,CLR::White,CLR::LightGray,CLR::Gray);

		hmr::dxosMUISide MUISide;

		hmr::cThermoMsgAgent ThermoMA;
	
		hmr::dxosThermoMUI ThermoMUI;
		hmr::connect(ThermoMUI,ThermoMA);
		MUISide.insert(&ThermoMUI);
	
		hmr::cSpriteMsgAgent SpriteMA;
		hmr::dxosSpriteMUI SpriteMUI;
		hmr::connect(SpriteMUI,SpriteMA);
		MUISide.insert(&SpriteMUI);
	
		hmr::cCO2MsgAgent CO2MA;
		hmr::dxosCO2MUI CO2MUI;
		hmr::connect(CO2MUI,CO2MA);
		MUISide.insert(&CO2MUI);

		hmr::cGPSMsgAgent GPSMA;
		hmr::dxosGPSMUI GPSMUI;
		hmr::connect(GPSMUI,GPSMA);
		MUISide.insert(&GPSMUI);

		hmr::cBatteryMsgAgent<3> BatteryMA;
		hmr::dxosBatteryMUI<3> BatteryMUI;
		hmr::connect(BatteryMUI,BatteryMA);
		MUISide.insert(&BatteryMUI);
	
		hmr::cHumidMsgAgent HumidMA;
		hmr::dxosHumidMUI HumidMUI;
		hmr::connect(HumidMUI,HumidMA);
		MUISide.insert(&HumidMUI);

		hmr::cH2SMsgAgent H2SMA;
		hmr::dxosH2SMUI H2SMUI;
		hmr::connect(H2SMUI,H2SMA);
		MUISide.insert(&H2SMUI);

		hmr::cMotorMsgAgent MotorMA;
		hmr::dxosMotorMUI MotorMUI;
		hmr::connect(MotorMUI,MotorMA);
		MUISide.insert(&MotorMUI);

		hmr::cCompassMsgAgent CompassMA;
		hmr::dxosCompassMUI CompassMUI;
		hmr::connect(CompassMUI,CompassMA);
		MUISide.insert(&CompassMUI);

		hmr::cAcceleMsgAgent AcceleMA;
		hmr::dxosAcceleMUI AcceleMUI;
		hmr::connect(AcceleMUI,AcceleMA);
		MUISide.insert(&AcceleMUI);

		hmr::cInfraRedMsgAgent InfraRedMA;
		hmr::dxosInfraRedMUI InfraRedMUI;
		hmr::connect(InfraRedMUI,InfraRedMA);
		MUISide.insert(&InfraRedMUI);

		hmr::cGyroMsgAgent GyroMA;
		hmr::dxosGyroMUI GyroMUI;
		hmr::connect(GyroMUI,GyroMA);
		MUISide.insert(&GyroMUI);

		while(!dx::work(30)){
			if(dx::getKey(KEY::ESC)<0)break;

			dx::draw(Pint(0,0),MUISide);
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
#
#endif
