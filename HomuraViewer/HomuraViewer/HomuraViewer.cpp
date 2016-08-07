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



#include"hmrDxControlMainDisplay.hpp"
#include"hmrDxDisplay.hpp"

#include"Thermo.hpp"

#include"hmrSHT75.hpp"
#include"hmrDxSHT75MUI.hpp"

#include"Camera.hpp"

#include"CO2.hpp"

#include"hmrH2S.hpp"
#include"hmrDxH2SMUI.hpp"

#include"hmrInfraRed.hpp"
#include"hmrDxInfraRedMUI.hpp"

#include"Motor.hpp"
#include"Accele.hpp"
#include"Compass.hpp"
#include"Gyro.hpp"

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

#include "Resource.hpp"


#include<hmLib_v2/hmLib.cpp>
#include"Controller.hpp"
#define HMR_MAIN_INC_END

#include"hmrConnectDx.hpp"
#include"hmrConnectSUI.hpp"
#include"hmrConnectMUI.hpp"
#include"hmrConnectModule.hpp"
#include"hmrConnectCore.hpp"
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
		hmrv::cAccele Accele;
		hmrv::cCompass Compass;
		hmrv::cGyro Gyro;
		hmrv::cGPS GPS;
		hmrv::cMotor Motor;
		hmrv::cThermo Thermo;
		hmrv::cCO2 CO2;
		hmrv::cCamera Camera;

		Message.regist('b', &(Battery.MsgAgent));
		Message.regist('f', &FullADC.MsgAgent);
		Message.regist('a', &(Accele.MsgAgent));
		Message.regist('c', &(Compass.MsgAgent));
		Message.regist('G', &(Gyro.MsgAgent));
		Message.regist('g', &(GPS.MsgAgent));
		Message.regist('m', &(Motor.MsgAgent));

		Message.regist('t', &(Thermo.MsgAgent));
		Message.regist('C', &(CO2.MsgAgent));
		Message.regist('j',&(Camera.MsgAgent));

		hmrv::cChrono Chrono;
		Message.regist('$', &Chrono);

		hmrv::cDevMngMsgAgent DevMngMA;
		Message.regist('D', &DevMngMA);

		hmrv::cLoggerMngMsgAgent LogMngMA;
		Message.regist('L', &LogMngMA);


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


		//�e�f�B���N�g��
		hmrv::cConstNameDirectoryFile DirectoryFile("Data");
		DirectoryFile.regist(&(Battery.FileAgent));
		DirectoryFile.regist(&(FullADC.FileAgent));

		// Thermo �f�[�^��ۑ�
		DirectoryFile.regist(&(Thermo.FileAgent));

		// CO2 �f�[�^��ۑ�
		DirectoryFile.regist(&(CO2.FileAgent));

		//GPS�f�[�^��ۑ�
		DirectoryFile.regist(&(GPS.FileAgent));

		//GPGGA�f�[�^��ۑ�
		DirectoryFile.regist(&(GPS.GPGGAFileAgent));

		//�J�����f�[�^��ۑ�
		DirectoryFile.regist(&(Camera.FileAgent));

		// SUI �n��
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


		// IO View Display �̒�`
		hmrv::dxosIO2<iologbuf::iterator> IOMainDisp(Pint(720,720), CLR::DarkDullGreen,CLR::SoftGreen,CLR::LightSoftOrenge,CLR::LightSoftSkyblue);
		hmrv::connect(IOMainDisp,IO,ioLogBuf,ioLogGate.Logger);

		//���c�pMainDisplaay
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

		hmrv::dxosPacketList_withView<hmrv::cComLog::iterator> PacketMainDisp(Pint(720,720),30,360,SwIDList,CLR::DullOrenge);
		hmrv::connect(PacketMainDisp,Com,ComLog);

		//MUI�p�T�C�h�o�[�錾
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
