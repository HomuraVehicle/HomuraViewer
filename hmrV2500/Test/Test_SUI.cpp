#include"hmLibVer.hpp"
#include <hmLib_v3_06/config_vc.h> // std::min, std::max　は　windowsマクロでも定義されており、明示的に分離してやらないとだめ
#include<hmLib_v3_06/bufgate.hpp>
#include<hmLib_v3_06/any_iterator.hpp>
#include <hmLib_v2/dxColorSet.hpp>
#include"dxSignalBut.hpp"
#include"iologgate.hpp"
#include"predicate.hpp"
#include"hmrBufGate.hpp"
#include"hmrGateSwitcher.hpp"
#include"hmrIO.hpp"
#include"hmrCom.hpp"
#include"hmrComLog.hpp"
#include"hmrDXCom.hpp"
#include"hmrOperator.hpp"
#include"hmrMessage.hpp"
#include"hmrDxKeyboard.hpp"
#include"hmrDxPad.hpp"
#include"hmrDxIODisplay.hpp"
#include"hmrDxControlMainDisplay.hpp"
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

#include"hmrDxBUIBoxSideDisp.hpp"

#include "hmrDxComSUI.hpp"
#include "hmrDxBufGateSUI.hpp"
#include "hmrDxGateSwitcherSUI.hpp"
#include "hmrDxIOLogGateSUI.hpp"
#include "hmrDxIOSUI.hpp"
#include "hmrDxOperatorSUI.hpp"
#include "hmrDxVMCSUI.hpp"


#include<hmLib_v2/hmLib.cpp>

#define HMR_MAIN_INC_END

#include"hmrConnectDx.hpp"
#include"hmrConnectModule.hpp"
#include"hmrConnectCore.hpp"
#include"hmrConnectKeyboard.hpp"
#include"hmrConnectPad.hpp"

//#include<hmLib_v3_06/virtual_com/virtual_comgate.hpp>
//#include<hmLib_v3_06/virtual_com/fdx_virtual_com.hpp>
//#include"DummyPulg_v1.hpp"

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow){
	//hmLib_dxモジュールを初期化
	dx::ini("hmrV2500_v0_06", 960,720);

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
		hmr::cFullRecvTimeSendOperator Operator(&Message,&Com,std::chrono::seconds(2),std::chrono::seconds(10),false,false);
		hmr::connect(Operator,IO,Com);

		//各モジュール宣言		
		hmr::cMotorMsgAgent MotorMA;
		hmr::connect_Pad(MotorMA,Pad1);
		Message.regist('m',&MotorMA);
		hmr::cBatteryMsgAgent<3> BatteryMA;
		Message.regist('b',&BatteryMA);
		hmr::connect_Keyboard(BatteryMA,Keyboard);		
		hmr::cAcceleMsgAgent AcceleMA;
		Message.regist('a',&AcceleMA);
		hmr::connect_Keyboard(AcceleMA,Keyboard);
		hmr::cCompassMsgAgent CompassMA;
		Message.regist('c',&CompassMA);
		hmr::cGyroMsgAgent GyroMA;
		hmr::cGyroLogger GyroLogger;
		hmr::connect(GyroLogger,GyroMA);
		Message.regist('G',&GyroMA);
		hmr::cGPSMsgAgent GPSMA;
		Message.regist('g',&GPSMA);
		hmr::cSpriteMsgAgent SpriteMA;
		Message.regist('j',&SpriteMA);
		hmr::cThermoMsgAgent ThermoMA;
		Message.regist('t',&ThermoMA);
		hmr::cInfraRedMsgAgent InfraRedMA;
		Message.regist('T',&InfraRedMA);
		hmr::cCO2MsgAgent CO2MA;
		Message.regist('C',&CO2MA);
		hmr::cH2SMsgAgent H2SMA;
		Message.regist('S',&H2SMA);
		hmr::cHumidMsgAgent HumidMA;
		Message.regist('h',&HumidMA);

		// IO View Display の定義
		hmr::dxosIOMainPage<iologbuf::iterator> IOMainDisp(Pint(720,720), CLR::DarkDullGreen);
		hmr::dxosIOSubPage IOSideDisp(Pint(240,720), CLR::DarkDullGreen);
		hmr::connect(IOMainDisp,IO,ioLogBuf);
		hmr::connect(IOSideDisp, GateSW,Operator,Bufgate);

		//操縦用MainDisplaay
		hmr::dxosControlMainDisplay ControlMainDisp;
		hmr::connect(ControlMainDisp.Navigator,AcceleMA,CompassMA,GyroLogger);
		hmr::dxosPacketList_withView<hmr::cComLog::iterator> PacketMainDisp(Pint(720,720),360,CLR::DullOrenge);
		hmr::connect(PacketMainDisp,Com,ComLog);

		//MUI用サイドバー宣言
		hmr::dxosMUI::DefaultClr.set(CLR::DarkSoftRed,CLR::SoftRed,CLR::LightSoftBlue,CLR::DeepSoftGreen,CLR::SoftRed,CLR::White,CLR::White,CLR::LightSoftRed,CLR::White,CLR::LightGray,CLR::Gray);
		hmr::dxosMUISideDisplay MUISideDisp;
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
		hmr::dxosInfraRedMUI InfraRedMUI;
		hmr::connect(InfraRedMUI,InfraRedMA);
		MUISideDisp.insert(&InfraRedMUI);
		hmr::dxosCO2MUI CO2MUI;
		hmr::connect(CO2MUI,CO2MA);
		MUISideDisp.insert(&CO2MUI);
		hmr::dxosH2SMUI H2SMUI;
		hmr::connect(H2SMUI,H2SMA);
		MUISideDisp.insert(&H2SMUI);
		hmr::dxosHumidMUI HumidMUI;
		hmr::connect(HumidMUI,HumidMA);
		MUISideDisp.insert(&HumidMUI);


		// SUI 系列
		hmr::dxosBUIBoxSideDisplay SystemSideDisp;

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

		hmr::dxosDisplay Display(Pint(720,720),Pint(240,720));
		Display.registMain(&IOMainDisp);
		Display.registMain(&PacketMainDisp);
		Display.registMain(&ControlMainDisp);
		Display.registSide(&IOSideDisp);
		Display.registSide(&MUISideDisp);
		Display.registSide(&SystemSideDisp);		


		Display.slot_setMainNo0(Keyboard.signal(hmLib::predicate_and(hmr::is_key_pushed(KEY::NUM1),hmLib::predicate_not(hmr::have_key_pushed(KEY::SHIFT)))));
		Display.slot_setMainNo1(Keyboard.signal(hmLib::predicate_and(hmr::is_key_pushed(KEY::NUM2),hmLib::predicate_not(hmr::have_key_pushed(KEY::SHIFT)))));
		Display.slot_setMainNo2(Keyboard.signal(hmLib::predicate_and(hmr::is_key_pushed(KEY::NUM3),hmLib::predicate_not(hmr::have_key_pushed(KEY::SHIFT)))));
		Display.slot_setSideNo0(Keyboard.signal(hmLib::predicate_and(hmr::is_key_pushed(KEY::NUM1),hmr::have_key_pushed(KEY::SHIFT))));
		Display.slot_setSideNo1(Keyboard.signal(hmLib::predicate_and(hmr::is_key_pushed(KEY::NUM2),hmr::have_key_pushed(KEY::SHIFT))));
		Display.slot_setSideNo2(Keyboard.signal(hmLib::predicate_and(hmr::is_key_pushed(KEY::NUM3),hmr::have_key_pushed(KEY::SHIFT))));

		while(!dx::work(30)){
			Keyboard.work();
			Pad1.work();

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
