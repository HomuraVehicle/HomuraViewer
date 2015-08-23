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
#include<hmLib_v2/dxObject.hpp>
#include<boost/signals2.hpp>
#include<hmLib_v3_06/signals.hpp>
#include<hmLib_v3_06/inquiries.hpp>
#include"hmrDxKeyboard.hpp"
#include"hmrDxPad.hpp"
#include<hmLib_v2/dxColorSet.hpp>
#include<iostream>
#include<hmLib_v2/hmLib.cpp>
#include"predicate.hpp"


//=====================================
#include<hmLib_v3_06/config_vc.h>
#include"hmLibVer.hpp"
#include<iostream>

#include<hmLib_v3_06/bufgate.hpp>
#include<hmLib_v3_06/any_iterator.hpp>

#include"dxSignalBut.hpp"
#include"iologgate.hpp"
#include"hmrGateSwitcher.hpp"
#include"hmrIO.hpp"
#include"hmrCom.hpp"
#include"hmrComLog.hpp"
#include"hmrDXCom.hpp"
#include"hmrOperator.hpp"
#include"hmrMessage.hpp"
#include "hmrDxIO.hpp"
#include "hmrDxGateSwitcher.hpp"

#include"hmrDxDisplay.hpp"

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

#include<hmLib_v2/hmLib.cpp>

#define HMR_MAIN_INC_END

#include"hmrConnectKeyboard.hpp"

#include"hmrConnectPad.hpp"
//=====================================


class Test_slot{
	typedef boost::signals2::signal<void(void)> signal;
	typedef hmLib::inquiries::inquiry<bool> inquiry;
	bool Flag;
public:
	void slot_setModeOn(signal& Signal_){hmLib::signals::connect(Signal_,[&](void)->void{this->Flag=true;});}
	void slot_setModeOff(signal& Signal_){hmLib::signals::connect(Signal_,[&](void)->void{this->Flag=false;});}
	void contact_flag(inquiry& Inquiry_){hmLib::inquiries::connect(Inquiry_,Flag);}
};

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow){

	dx::ini("Test",960,720);

	Test_slot Test;

	hmLib::inquiries::inquiry<bool> FlagInq;
	Test.contact_flag(FlagInq);

	hmr::cDxKeyboard Keyboard;
	hmr::cDxPad1 Pad1;
	
	Test.slot_setModeOn(Keyboard.signal(hmr::is_key_pushed(KEY::A)));

	Test.slot_setModeOn(Keyboard.signal(hmLib::predicate_and(hmr::is_key_pushed(KEY::NUM1),hmr::have_key_pushed(KEY::SHIFT))));
	Test.slot_setModeOff(Keyboard.signal(hmLib::predicate_and(hmr::is_key_pushed(KEY::NUM1),hmLib::predicate_not(hmr::have_key_pushed(KEY::SHIFT)))));
	Test.slot_setModeOn(Keyboard.signal(hmr::have_key_pushed(KEY::Q)));
	Test.slot_setModeOff(Keyboard.signal(hmr::is_key_pulled(KEY::Q)));


	Test.slot_setModeOff(Pad1.signal(hmr::is_pad1_pushed(PAD::But1)));
	Test.slot_setModeOn(Pad1.signal(hmr::is_pad1_pushed(PAD::But2)));

	try{
		while(!dx::work(30)){
			if(dx::getKey(KEY::ESC)<0)break;
			if(dx::getKey(KEY::SPACE)==1)hmr::cPad<DX_INPUT_PAD1>::connect();

			Keyboard.work();
			Pad1.work();
			dx::draw(Pint(0,0),dxoButIO(Pint(100,100),"",CLR::Red,FlagInq()));
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
