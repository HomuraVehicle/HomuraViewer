/*===========hmrV2500.cpp===========
hmrV2500 v1_06/160601
	a
		Git�ړ��㏉�߂ăR���p�C���ɐ���
	c
		HomuraViewer���� Data, File, VMC1���ړ�
	d
		�f�B���N�g���\����ύX�@Data�f�B���N�g�������ɂ��ׂẴf�[�^���o��悤�ɏC��
		���킹�āA���x�v�̎��Ԗ��X�V�o�O���X�V
		�o�̓t�@�C����tsv�`���ɍX�V
		Date���o�͂���悤�ɂ���
	e
		AcceleLogger�̊p�x���ߕq�ɔ�����������C��
	f
		FullADC�@�\������
	g
		Add foma settingh function
	h
		�V�����f�B���N�g���\���Ɉڍs
hmrV2500 v1_05/130914
	p
		�\���ʒu���C��
	n
		�R���p�X�ɕ����␳�@�\��ǉ�
	h
		Operator���f�o�b�O
	g
		�t�@�C���@�\����x�O��
		����d�ʐM�ւ̑Ή�
	�t�@�C���@�\����������
hmrV2500 v1_04/130803
	�t�@�C���@�\�������J�n

hmrV2500 v1_03/130713 
	�����ł��Ă��邱��
	�@�E��{�I�ȋ@�\�̎���
	�@
*/

#include"hmLibVer.hpp"
#include <hmLib/config_vc.h> // std::min, std::max�@�́@windows�}�N���ł���`����Ă���A�����I�ɕ������Ă��Ȃ��Ƃ���
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

#include"hmrDxKeyboard.hpp"
#include"hmrDxPad.hpp"

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

#include"hmrGPS.hpp"
#include"hmrDxGPSMUI.hpp"
#include"hmrDxGPSMap.hpp"
#include"hmrGPSKashmir.hpp"

#include"Battery.hpp"

#include"UniSensor.hpp"

#include"hmrH2S.hpp"
#include"hmrDxH2SMUI.hpp"

#include"hmrInfraRed.hpp"
#include"hmrDxInfraRedMUI.hpp"

#include"hmrMotor.hpp"
#include"hmrDxMotorMUI.hpp"

#include"hmrCompass.hpp"
#include"hmrCompassData.hpp"

#include"hmrDxCompassMUI.hpp"

#include"hmrAccele.hpp"
#include"hmrAcceleLogger.hpp"
#include"hmrDxAcceleMUI.hpp"

#include"hmrGyro.hpp"
#include"hmrGyroCompass.hpp"
#include"hmrDxGyroMUI.hpp"


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
#include "hmrGPSFile.hpp"
#include "hmrSpriteFile.hpp"

#include "Resource.hpp"


#include<hmLib_v2/hmLib.cpp>

#define HMR_MAIN_INC_END

#include"hmrConnectDx.hpp"
#include"hmrConnectSUI.hpp"
#include"hmrConnectMUI.hpp"
#include"hmrConnectModule.hpp"
#include"hmrConnectCore.hpp"
#include"hmrConnectKeyboard.hpp"
#include"hmrConnectPad.hpp"
#include"hmrConnectFile.hpp"

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow){
	namespace hmrv = hmr::viewer;

	SetWindowIconID(ID_ICON);

	//hmLib_dx���W���[����������
	dx::ini("hmrV2500_v1_06g", 960,720);


	try{
		//Com, Message, Operator��錾
		hmrv::cCom Com;
		hmrv::cCom::VMC1Creater<1> ComVMC(&Com);
		hmrv::cComLog ComLog;
		hmrv::connect(ComLog,Com);

		hmrv::cFHdxIO IO(ComVMC);
		hmrv::bufgate Bufgate;

		//ioLogBuf��錾
		iologgate<fdx_crlf_timeout_iologger<system_clock_iologtype>> ioLogGate;
		typedef fdx_vector_iologbuf<system_clock_iologtype> iologbuf;
		iologbuf ioLogBuf;
		hmrv::connect(ioLogBuf,ioLogGate);

		//GateSwitcher��錾
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

		//�e���W���[���錾		
		hmrv::cBattery Battery;
		hmrv::cFullADC FullADC;

		hmrv::cMotorMsgAgent MotorMA;
		Message.regist('m',&MotorMA);
		Message.regist('b',&(Battery.MsgAgent));
		
		hmrv::cAcceleMsgAgent AcceleMA;
		Message.regist('a',&AcceleMA);
		hmrv::cAcceleLogger AcceleLogger;
		AcceleLogger.slot_addLog(AcceleMA.signal_newData);

		hmrv::cCompassMsgAgent CompassMA;
		hmrv::cCompass CompassDat;
		Message.regist('c',&CompassMA);
		hmrv::connect(CompassDat, CompassMA);

		hmrv::cGyroMsgAgent GyroMA;
		hmrv::cGyroLogger GyroLogger;
		hmrv::connect(GyroLogger,GyroMA);
		Message.regist('G',&GyroMA);
		hmrv::cGyroCompass GyroCompass;
		hmrv::connect(GyroCompass,GyroMA);

		hmrv::cGPSMsgAgent GPSMA;
		hmrv::cGPSKashmir GPSKashmir;
		hmrv::connect(GPSKashmir,GPSMA);
		Message.regist('g',&GPSMA);

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



		//����n�f�o�C�X
		hmrv::cDxKeyboard Keyboard;
		hmrv::cDxPad1 Pad1;

		hmrv::connect_Pad(MotorMA,Pad1);
		hmrv::connect_Pad(Battery.MsgAgent,Pad1);
		hmrv::connect_Pad(AcceleMA,Pad1);
		hmrv::connect_Pad(CompassMA,Pad1);
		hmrv::connect_Pad(GyroMA,Pad1);
		hmrv::connect_Pad(GPSMA,Pad1);
		hmrv::connect_Pad(SpriteMA,Pad1);
		hmrv::connect_Pad(ThermoMA,Pad1);
		hmrv::connect_Pad(CO2MA,Pad1);


		//�e�f�B���N�g��
		hmrv::cConstNameDirectoryFile DirectoryFile("Data");
		DirectoryFile.regist(&(Battery.FileAgent));
		DirectoryFile.regist(&(FullADC.FileAgent));

		// Thermo �f�[�^��ۑ�
		hmrv::cConstNameFileAgent<std::pair<double, std::uint16_t>> ThermoFileAgent("Thermo.txt",'\t');
		ThermoFileAgent.slot_log_writeData(ThermoMA.signal_newRawData);
		DirectoryFile.regist(&ThermoFileAgent);

		// CO2 �f�[�^��ۑ�
		hmrv::cConstNameFileAgent<std::pair<double, std::uint16_t>> CO2FileAgent("CO2.txt", '\t');
		CO2FileAgent.slot_log_writeData(CO2MA.signal_newRawData);
		DirectoryFile.regist(&CO2FileAgent);

		//GPS�f�[�^��ۑ�
		hmrv::cGPSFileAgent GPSFileAgent;
		hmrv::connect(GPSFileAgent, GPSMA);
		DirectoryFile.regist(&GPSFileAgent);

		//GPGGA�f�[�^��ۑ�
		hmrv::cGPGGAFileAgent GPGGAFileAgent;
		hmrv::connect(GPGGAFileAgent, GPSMA);
		DirectoryFile.regist(&GPGGAFileAgent);

		//�J�����f�[�^��ۑ�
		hmrv::cSpriteFileAgent SpriteFileAgent("Sprite");
		hmrv::connect(SpriteFileAgent, SpriteMA,false);
		DirectoryFile.regist(&SpriteFileAgent);

		// SUI �n��
		hmrv::dxosBUIBoxSideDisplay SystemSideDisp;
		SystemSideDisp.ClrSet.Background=CLR::DarkSoftBlue;

		hmrv::dxosChronoSUI ChronoSUI;
		hmrv::connect(ChronoSUI, Chrono);
		SystemSideDisp.insert(&ChronoSUI);

		hmrv::dxosDevMngSUI DevMngSUI;
		hmrv::connect(DevMngSUI, DevMngMA);
		SystemSideDisp.insert(&DevMngSUI);

		hmrv::dxosLoggerMngSUI LogMngSUI;
		hmrv::connect(LogMngSUI, LogMngMA);
		SystemSideDisp.insert(&LogMngSUI);

		hmrv::dxosGateSwitcherSUI GateSwSUI;
		hmrv::connect(GateSwSUI, GateSW);
		SystemSideDisp.insert(&GateSwSUI);

		hmrv::dxosBufGateSUI BufGateSUI;
		hmrv::connect(BufGateSUI, Bufgate);
		SystemSideDisp.insert(&BufGateSUI);

		hmrv::dxosIOLogGateSUI LogSUI;
		hmrv::connect(LogSUI, ioLogGate, ioLogBuf);
		SystemSideDisp.insert(&LogSUI);

		hmrv::dxosIOSUI IOSUI;
		hmrv::connect(IOSUI, IO);
		SystemSideDisp.insert(&IOSUI);

		hmrv::dxosVMCSUI VMCSUI;
		hmrv::connect(VMCSUI, IO);
		SystemSideDisp.insert(&VMCSUI);


		hmrv::dxosComSUI ComBUI;
		hmrv::connect(ComBUI,Com);
		SystemSideDisp.insert(&ComBUI);

		hmrv::dxosOperatorSUI OpSUI;
		hmrv::connect(OpSUI, Operator);
		SystemSideDisp.insert(&OpSUI);

		hmrv::dxosFileSUI FileSUI;
		hmrv::connect(FileSUI, DirectoryFile);
		SystemSideDisp.insert(&FileSUI);


		// IO View Display �̒�`
		hmrv::dxosIO2<iologbuf::iterator> IOMainDisp(Pint(720,720), CLR::DarkDullGreen,CLR::SoftGreen,CLR::LightSoftOrenge,CLR::LightSoftSkyblue);
		hmrv::connect(IOMainDisp,IO,ioLogBuf,ioLogGate.Logger);

		//���c�pMainDisplaay
		hmrv::dxosControlMainDisplay ControlMainDisp;
		hmrv::connect(ControlMainDisp.Navigator,AcceleLogger,CompassDat,GyroLogger,GyroCompass);
		hmrv::connect(ControlMainDisp.Sprite,SpriteMA);
		hmrv::connect(ControlMainDisp.Infomation,GPSKashmir,Battery);
		hmrv::connect(ControlMainDisp.GPSMap, GPSMA,CompassDat);
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

		//MUI�p�T�C�h�o�[�錾
		hmrv::dxosBUIBoxSideDisplay MUISideDisp;
		MUISideDisp.ClrSet.Background=CLR::DarkSoftYellow;

		hmrv::dxosMotorMUI MotorMUI;
		hmrv::connect(MotorMUI,MotorMA);
		MUISideDisp.insert(&MotorMUI);

		hmrv::battery::dxosMUI<hmrv::cBattery::BatteryNum> BatteryMUI;
		Battery.connect(BatteryMUI);
		MUISideDisp.insert(&BatteryMUI);

		hmrv::dxosAcceleMUI AcceleMUI;
		hmrv::connect(AcceleMUI,AcceleMA);
		MUISideDisp.insert(&AcceleMUI);

		hmrv::dxosCompassMUI CompassMUI;
		hmrv::connect(CompassMUI,CompassMA);
		MUISideDisp.insert(&CompassMUI);

		hmrv::dxosGyroMUI GyroMUI;
		hmrv::connect(GyroMUI,GyroMA);
		MUISideDisp.insert(&GyroMUI);

		hmrv::dxosGPSMUI GPSMUI;
		hmrv::connect(GPSMUI,GPSMA);
		MUISideDisp.insert(&GPSMUI);

		hmrv::dxosSpriteMUI SpriteMUI;
		hmrv::connect(SpriteMUI,SpriteMA);
		MUISideDisp.insert(&SpriteMUI);

		hmrv::dxosThermoMUI ThermoMUI;
		hmrv::connect(ThermoMUI,ThermoMA);
		MUISideDisp.insert(&ThermoMUI);

		hmrv::dxosCO2MUI CO2MUI;
		hmrv::connect(CO2MUI,CO2MA);
		MUISideDisp.insert(&CO2MUI);

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
		MUISideDisp.insert(&FullADCMUI);
		ControlMainDisp.Infomation.slot_logData(FullADC.MsgAgent.signal_newData);

		hmrv::dxosDisplay Display(Pint(720,720),Pint(240,720));
		Display.registMain(&IOMainDisp);
		Display.registMain(&PacketMainDisp);
		Display.registMain(&ControlMainDisp);
		Display.registSide(&SystemSideDisp);
		Display.registSide(&MUISideDisp);	
	
		Display.slot_setMainNo0(Keyboard.signal(hmLib::predicate_and(hmrv::is_key_pushed(KEY::NUM1),hmLib::predicate_not(hmrv::have_key_pushed(KEY::SHIFT)))));
		Display.slot_setMainNo1(Keyboard.signal(hmLib::predicate_and(hmrv::is_key_pushed(KEY::NUM2),hmLib::predicate_not(hmrv::have_key_pushed(KEY::SHIFT)))));
		Display.slot_setMainNo2(Keyboard.signal(hmLib::predicate_and(hmrv::is_key_pushed(KEY::NUM3),hmLib::predicate_not(hmrv::have_key_pushed(KEY::SHIFT)))));
		Display.slot_setSideNo0(Keyboard.signal(hmLib::predicate_and(hmrv::is_key_pushed(KEY::NUM1),hmrv::have_key_pushed(KEY::SHIFT))));
		Display.slot_setSideNo1(Keyboard.signal(hmLib::predicate_and(hmrv::is_key_pushed(KEY::NUM2),hmrv::have_key_pushed(KEY::SHIFT))));
		Display.slot_setSideNo2(Keyboard.signal(hmLib::predicate_and(hmrv::is_key_pushed(KEY::NUM3),hmrv::have_key_pushed(KEY::SHIFT))));

		Display.slot_plusMainNo(Pad1.signal(hmLib::predicate_and(hmrv::is_pad1_cross_key_pulled(PAD::CROSS_KEY::RIGHT),hmLib::predicate_not(hmrv::have_pad1_pushed(PAD::But7)))));
		Display.slot_minusMainNo(Pad1.signal(hmLib::predicate_and(hmrv::is_pad1_cross_key_pulled(PAD::CROSS_KEY::LEFT),hmLib::predicate_not(hmrv::have_pad1_pushed(PAD::But7)))));
		Display.slot_plusSideNo(Pad1.signal(hmLib::predicate_and(hmrv::is_pad1_cross_key_pulled(PAD::CROSS_KEY::RIGHT),hmrv::have_pad1_pushed(PAD::But7))));
		Display.slot_minusSideNo(Pad1.signal(hmLib::predicate_and(hmrv::is_pad1_cross_key_pulled(PAD::CROSS_KEY::LEFT),hmrv::have_pad1_pushed(PAD::But7))));

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
