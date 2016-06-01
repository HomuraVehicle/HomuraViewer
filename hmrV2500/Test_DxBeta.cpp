#ifndef HMR_TEST_COMCORE_INC
#define HMR_TEST_COMCORE_INC 100
#
/*===Test_ComCore===
�ʐM�n�I�u�W�F�N�g�̃e�X�g�p�t�@�C��
���Ӂ@���̃v���O�����́ACOM9���J���Ă���
���肪���Ȃ��̂ɊJ���ƁA����
*/
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

#include "hmrDxControlMainDisplay.hpp"

#include "hmrDxIODisplay.hpp"

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

#include"hmrConnectDx.hpp"
#include"hmrConnectCore.hpp"

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow){

	dx::ini("Test",960,720);

	try{
		//Com, Message, Operator��錾
		hmr::cCom Com;
		hmr::cCom::VMC1Creater<1> ComVMC(&Com);
		hmr::cMessage Message;
		hmr::cFullRecvTimeSendOperator Operator(&Message,&Com,std::chrono::seconds(2),std::chrono::seconds(10),false,false);
		hmr::cComLog ComLog;
		hmr::connect(ComLog,Com);

		//IO(VMC), ioLogGate, GateSW��錾
		//typedef std::pair<bool,system_clock_iologtype> io_iologtype;
		hmr::cFHdxIO IO(ComVMC);
		iologgate<fdx_crlf_timeout_iologger<system_clock_iologtype>> ioLogGate;
		hmr::cGateSwitcher GateSW;
		ioLogGate.open(GateSW);
		IO.open(&ioLogGate);

		//ioLogBuf��錾
		fdx_vector_iologbuf<system_clock_iologtype> ioLogBuf;
		hmr::connect(ioLogBuf,ioLogGate);

		//MUI�p�T�C�h�o�[�錾
		hmr::dxosMUI::DefaultClr.set(CLR::DarkSoftRed,CLR::SoftRed,CLR::LightSoftBlue,CLR::DeepSoftGreen,CLR::SoftRed,CLR::White,CLR::White,CLR::LightSoftRed,CLR::White,CLR::LightGray,CLR::Gray);
		hmr::dxosMUISideDisplay MUISide;


		//�e���W���[���錾
		hmr::cMotorMsgAgent MotorMA;
		Message.regist('m',&MotorMA);
		hmr::dxosMotorMUI MotorMUI;
		hmr::connect(MotorMUI,MotorMA);
		MUISide.insert(&MotorMUI);

		hmr::cBatteryMsgAgent<3> BatteryMA;
		Message.regist('b',&BatteryMA);
		hmr::dxosBatteryMUI<3> BatteryMUI;
		hmr::connect(BatteryMUI,BatteryMA);
		MUISide.insert(&BatteryMUI);
	
		hmr::cAcceleMsgAgent AcceleMA;
		Message.regist('a',&AcceleMA);
		hmr::dxosAcceleMUI AcceleMUI;
		hmr::connect(AcceleMUI,AcceleMA);
		MUISide.insert(&AcceleMUI);

		hmr::cCompassMsgAgent CompassMA;
		Message.regist('c',&CompassMA);
		hmr::dxosCompassMUI CompassMUI;
		hmr::connect(CompassMUI,CompassMA);
		MUISide.insert(&CompassMUI);

		hmr::cGyroMsgAgent GyroMA;
		Message.regist('g',&GyroMA);
		hmr::dxosGyroMUI GyroMUI;
		hmr::connect(GyroMUI,GyroMA);
		MUISide.insert(&GyroMUI);

		hmr::cGPSMsgAgent GPSMA;
		Message.regist('G',&GPSMA);
		hmr::dxosGPSMUI GPSMUI;
		hmr::connect(GPSMUI,GPSMA);
		MUISide.insert(&GPSMUI);

		hmr::cSpriteMsgAgent SpriteMA;
		Message.regist('j',&SpriteMA);
		hmr::dxosSpriteMUI SpriteMUI;
		hmr::connect(SpriteMUI,SpriteMA);
		MUISide.insert(&SpriteMUI);
	
		hmr::cThermoMsgAgent ThermoMA;
		Message.regist('t',&ThermoMA);
		hmr::dxosThermoMUI ThermoMUI;
		hmr::connect(ThermoMUI,ThermoMA);
		MUISide.insert(&ThermoMUI);
	
		hmr::cInfraRedMsgAgent InfraRedMA;
		Message.regist('T',&InfraRedMA);
		hmr::dxosInfraRedMUI InfraRedMUI;
		hmr::connect(InfraRedMUI,InfraRedMA);
		MUISide.insert(&InfraRedMUI);

		hmr::cCO2MsgAgent CO2MA;
		Message.regist('C',&CO2MA);
		hmr::dxosCO2MUI CO2MUI;
		hmr::connect(CO2MUI,CO2MA);
		MUISide.insert(&CO2MUI);

		hmr::cH2SMsgAgent H2SMA;
		Message.regist('S',&H2SMA);
		hmr::dxosH2SMUI H2SMUI;
		hmr::connect(H2SMUI,H2SMA);
		MUISide.insert(&H2SMUI);

		hmr::cHumidMsgAgent HumidMA;
		Message.regist('h',&HumidMA);
		hmr::dxosHumidMUI HumidMUI;
		hmr::connect(HumidMUI,HumidMA);
		MUISide.insert(&HumidMUI);

		// IO�f�[�^�^�C�v��typedef 
		typedef std::pair<bool,system_clock_iologtype> io_iologtype;
		typedef std::vector<io_iologtype> container;
		typedef container::const_iterator contanerItr;

		// IO View Display �̒�`
		hmr::dxosIOMainPage<contanerItr> dxIOMain(Pint(720,720), CLR::DarkDullGreen);
		hmr::dxosIOSubPage dxIOSub(Pint(240,720), CLR::DarkDullGreen);

		/*
		// IO View�@�N���X�̒�`
		hmr::dxosIO<contanerItr> DxIO(Pint(640, 600), CLR::DarkDullGreen);
		// IO send box �N���X�̒�`
		hmr::dxosIOSendBox DxSendBox(Pint(640, 30), CLR::DarkDullGreen);
		// IO setFlag �N���X�̒�`
		hmr::dxosSelectIOPaint DxSelectIOPaint(Pint(640, 20), CLR::DarkDullGreen);
		// IO Control �N���X�̒�`
		hmr::dxosStartControl DxSendStartButt(Pint(200, 30), CLR::DarkDullGreen, "Send Start");
		hmr::dxosBufControl DxReadBufCtr(Pint(200, 30), CLR::DarkDullGreen, "Read Buf Clear");
		hmr::dxosBufControl DxSendBufCtr(Pint(200, 30), CLR::DarkDullGreen, "Send Buf Clear");
		hmr::dxosTimeIntervalControl DxSendIntervalCtr(Pint(200, 30), CLR::DarkDullGreen, "Interval");
		hmr::dxosTimeOutControl DxSendTimeOutCtr(Pint(200, 30), CLR::DarkDullGreen, "TimeOut");
		hmr::dxosSyncControl DxSyncCheckBox(Pint(200, 20),CLR::DarkDullGreen); 
		hmr::dxosSelectDuplex DxSelectDuplex(Pint(250, 20),CLR::DarkDullGreen);
		// Gate Switcher �`��N���X�̒�`
		hmr::dxosGateSwitcher DxosGateSwitcher(CLR::Cyan);
		*/

		// DxSendBox ������
		dxIOMain.initialize();

		//�@�R�l�N�g���Ă���
		 // Dx
		hmr::connect(dxIOMain);
		hmr::connect(dxIOSub, GateSW);
		hmr::connect(dxIOMain, IO);
		hmr::connect(dxIOMain, ioLogBuf);
		hmr::connect(dxIOSub, Operator);

		 // Dx Draw
		/*
		hmr::connect(DxosGateSwitcher, GateSW);
		hmr::connect(DxSendBox, IO);
		hmr::connect(DxIO, ioLogBuf);
		hmr::connect(DxSelectIOPaint, DxIO); 

		hmr::connect(DxSendStartButt, Operator);
		hmr::connect(DxSelectDuplex, Operator);
		hmr::connect(DxSendIntervalCtr, Operator);
		hmr::connect(DxSendTimeOutCtr, Operator);
		hmr::connect(DxSyncCheckBox, Operator);
		*/

		 // Com-Ope-IO
		hmr::connect(IO, Operator);
		hmr::connect(Com, Operator);

		//
		hmr::dxosControlMainDisplay ControlMainDisplay;
		hmr::connect(ControlMainDisplay.Navigator,AcceleMA,CompassMA,GyroLogger);
		hmr::connect(ControlMainDisplay.Sprite,

		//ComLog�`��N���X
		hmr::dxosPacketList_withView<hmr::cComLog::iterator> PacketListView(Pint(720,720),360,CLR::SoftBlue);
		hmr::connect(PacketListView,Com,ComLog);

		//MainSide����p�N���X
		hmr::dxosDisplay Display(Pint(720,720),Pint(240,720));
		Display.registSide(&MUISide);
		Display.registMain(&PacketListView);
		Display.registMain(&PacketListView);

		while(!dx::work(30)){
			if(dx::getKey(KEY::ESC)<0)break;

			IO.work();
			Operator();

/*			dx::draw(Pint(300, 10), DxSendBox);
			dx::draw(Pint(300,90), DxIO);
			dx::draw(Pint(300,50), DxSelectIOPaint);
			dx::draw(Pint(20, 30), DxosGateSwitcher);

			dx::draw(Pint(20, 400), DxSelectDuplex);
		
			dx::draw(Pint(30, 430), DxSendIntervalCtr);

			dx::draw(Pint(30, 480), DxSendTimeOutCtr);
			dx::draw(Pint(30, 510), DxSyncCheckBox);
		
			dx::draw(Pint(20, 570), DxSendStartButt);
			dx::draw(Pint(30, 600), DxSendBufCtr);
			dx::draw(Pint(30, 630), DxReadBufCtr);	
*/
			dx::draw(Pint(0,0),MainSide);
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