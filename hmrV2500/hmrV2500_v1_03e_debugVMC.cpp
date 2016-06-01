
 
/*===========hmrV2500.cpp===========

hmrV2500 v1_04/130803
	�t�@�C���@�\�������J�n

hmrV2500 v1_03/130713 
	�����ł��Ă��邱��
	�@�E��{�I�ȋ@�\�̎���
	�@�E


*/

#include"hmLibVer.hpp"
#include <hmLib_v3_06/config_vc.h> // std::min, std::max�@�́@windows�}�N���ł���`����Ă���A�����I�ɕ������Ă��Ȃ��Ƃ���
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
#include"hmrDXCom_v2.hpp"
#include"hmrOperator.hpp"
#include"hmrMessage.hpp"
#include"hmrDxKeyboard.hpp"
#include"hmrDxPad.hpp"
//#include"hmrDxIODisplay.hpp"
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
#include"hmrDxGPSMap.hpp"
#include"hmrGPSKashmir.hpp"

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

#include"hmrCompassMA.hpp"
#include"hmrCompassData.hpp"
#include"hmrDxCompassMUI.hpp"

#include"hmrAccele.hpp"
#include"hmrDxAcceleMUI.hpp"

#include"hmrGyro.hpp"
#include"hmrDxGyroMUI.hpp"

#include"hmrDxComSUI.hpp"
#include "hmrDxBufGateSUI.hpp"
#include "hmrDxGateSwitcherSUI.hpp"
#include "hmrDxIOLogGateSUI.hpp"
#include "hmrDxIOSUI.hpp"
#include "hmrDxIO_v2.hpp"
#include "hmrDxOperatorSUI.hpp"
#include "hmrDxVMCSUI.hpp"
#include"hmrDxBUIBoxSideDisp.hpp"

// start test 
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
// end test

#include<hmLib_v2/hmLib.cpp>

#define HMR_MAIN_INC_END

#include"hmrConnectDx.hpp"
#include"hmrConnectModule.hpp"
#include"hmrConnectCore.hpp"
#include"hmrConnectKeyboard.hpp"
#include"hmrConnectPad.hpp"
#include "hmrConnectFile.hpp"


//#include<hmLib_v3_06/lab/virtual_com/virtual_comgate.hpp>
//#include<hmLib_v3_06/lab/virtual_com/fdx_virtual_com.hpp>
//#include"DummyPulg_v1.hpp"

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow){
	//hmLib_dx���W���[����������
	dx::ini("hmrV2500_v1_03", 960,720);

	try{
		//hmLib::vcom::fdx_virtual_com VCom;
		//VCom.start(9600);

		//hmr::dummy_plug_v1 DummyPlug;
		//std::thread Thread(std::ref(DummyPlug));
		//DummyPlug.VComGate.open(VCom);

		//hmLib::vcom::virtual_comgate vcomgate;
		//vcomgate.open(VCom);

		//����n�f�o�C�X
		hmr::cDxKeyboard Keyboard;
		hmr::cDxPad1 Pad1;

		//Com, Message, Operator��錾
		hmr::cCom Com;
		hmr::cCom::VMC1Creater<1> ComVMC(&Com);
		hmr::cComLog ComLog;
		hmr::connect(ComLog,Com);

		//IO(VMC), ioLogGate, GateSW��錾
		//typedef std::pair<bool,system_clock_iologtype> io_iologtype;
		hmr::cFHdxIO IO(ComVMC);
		hmr::bufgate Bufgate;

		//ioLogBuf��錾
		iologgate<fdx_crlf_timeout_iologger<system_clock_iologtype>> ioLogGate;
		typedef fdx_vector_iologbuf<system_clock_iologtype> iologbuf;
		iologbuf ioLogBuf;
		hmr::connect(ioLogBuf,ioLogGate);

		//GateSwitcher��錾
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
		hmr::cFullRecvTimeSendOperator Operator(&Message,&Com,std::chrono::milliseconds(250),std::chrono::seconds(1),true,false);
		hmr::connect(Operator,IO,Com);

		//�e���W���[���錾		
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

		hmr::cInfraRedMsgAgent InfraRedMA;
		hmr::connect_Pad(InfraRedMA,Pad1);
		Message.regist('T',&InfraRedMA);

		hmr::cCO2MsgAgent CO2MA;
		hmr::connect_Pad(CO2MA,Pad1);
		Message.regist('C',&CO2MA);

		hmr::cH2SMsgAgent H2SMA;
		hmr::connect_Pad(H2SMA,Pad1);
		Message.regist('S',&H2SMA);

		hmr::cHumidMsgAgent HumidMA;
		hmr::connect_Pad(HumidMA,Pad1);
		Message.regist('h',&HumidMA);


		// SUI �n��
		hmr::dxosBUIBoxSideDisplay SystemSideDisp;
		SystemSideDisp.ClrSet.Background=CLR::DarkSoftBlue;

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
	

		// IO View Display �̒�`
		hmr::dxosIO2<iologbuf::iterator> IOMainDisp(Pint(720,720), CLR::DarkDullGreen,CLR::SoftGreen,CLR::LightSoftOrenge,CLR::LightSoftSkyblue);
		//hmr::dxosIOSubPage IOSideDisp(Pint(240,720), CLR::DarkDullGreen);
		hmr::connect(IOMainDisp,IO,ioLogBuf,ioLogGate.Logger);
		//hmr::connect(IOSideDisp, GateSW,Operator,Bufgate);

		//���c�pMainDisplaay
		hmr::dxosControlMainDisplay ControlMainDisp;
		hmr::connect(ControlMainDisp.Navigator,AcceleMA,CompassDat,GyroLogger);
		hmr::connect(ControlMainDisp.Sprite,SpriteMA);
		hmr::connect(ControlMainDisp.Infomation,GPSKashmir,BatteryMA);
		hmr::connect(ControlMainDisp.GPSMap, GPSMA,CompassDat);
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

		//MUI�p�T�C�h�o�[�錾

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

		while(!dx::work(30)){
			Keyboard.work();
			Pad1.work();

			Bufgate();
			IO.work();
			Operator();

			Debug();
			DebugSignal(hmr::clock::now());

			dx::draw(Pint(0,0),Display);

			printfDx("\r%d",hmr::cCom::VMCConnection<1>::DebugNum);

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