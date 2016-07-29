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

#include <HomuraViewer/Message.hpp>

#include"hmrDxKeyboard.hpp"
#include"hmrDxPad.hpp"
//#include"hmrDxIODisplay.hpp"
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

//include"hmrHumid.hpp"
//#include"hmrDxHumidMUI.hpp"

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

//#include<hmLib/virtual_com/virtual_comgate.hpp>
//#include<hmLib/virtual_com/fdx_virtual_com.hpp>
//#include"DummyPulg_v1.hpp"

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow){
	namespace hmrv = hmr::viewer;

	SetWindowIconID(ID_ICON);

	//hmLib_dx���W���[����������
	dx::ini("hmrV2500_v1_06g", 960,720);


	try{
		//hmLib::vcom::fdx_virtual_com VCom;
		//VCom.start(9600);

		//hmrv::dummy_plug_v1 DummyPlug;
		//std::thread Thread(std::ref(DummyPlug));
		//DummyPlug.VComGate.open(VCom);

		//hmLib::vcom::virtual_comgate vcomgate;
		//vcomgate.open(VCom);

		//����n�f�o�C�X
		hmrv::cDxKeyboard Keyboard;
		hmrv::cDxPad1 Pad1;

		//Com, Message, Operator��錾
		hmrv::cCom Com;
		hmrv::cCom::VMC1Creater<1> ComVMC(&Com);
		hmrv::cComLog ComLog;
		hmrv::connect(ComLog,Com);

		//IO(VMC), ioLogGate, GateSW��錾
		//typedef std::pair<bool,system_clock_iologtype> io_iologtype;
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
		hmrv::cMessage Message;
		hmrv::cFHDxOperator Operator(&Message,&Com,true,std::chrono::milliseconds(250),std::chrono::seconds(1));
		hmrv::connect(Operator,IO,Com);

		//�e���W���[���錾		
		hmrv::cMotorMsgAgent MotorMA;
		hmrv::connect_Pad(MotorMA,Pad1);
		Message.regist('m',&MotorMA);

		hmrv::cBattery Battery;
		Message.regist('b',&(Battery.MsgAgent));
		hmrv::connect_Pad(Battery.MsgAgent,Pad1);
		
//		hmrv::connect_Keyboard(BatteryMA,Keyboard);	

		hmrv::cAcceleMsgAgent AcceleMA;
		Message.regist('a',&AcceleMA);
		hmrv::connect_Pad(AcceleMA,Pad1);
		hmrv::cAcceleLogger AcceleLogger;
		AcceleLogger.slot_addLog(AcceleMA.signal_newData);
//		hmrv::connect_Keyboard(AcceleMA,Keyboard);

		hmrv::cCompassMsgAgent CompassMA;
		hmrv::cCompass CompassDat;
		Message.regist('c',&CompassMA);
		hmrv::connect_Pad(CompassMA,Pad1);
		hmrv::connect(CompassDat, CompassMA);

		hmrv::cGyroMsgAgent GyroMA;
		hmrv::cGyroLogger GyroLogger;
		hmrv::connect_Pad(GyroMA,Pad1);
		hmrv::connect(GyroLogger,GyroMA);
		Message.regist('G',&GyroMA);
		hmrv::cGyroCompass GyroCompass;
		hmrv::connect(GyroCompass,GyroMA);

		hmrv::cGPSMsgAgent GPSMA;
		hmrv::cGPSKashmir GPSKashmir;
		hmrv::connect_Pad(GPSMA,Pad1);
		hmrv::connect(GPSKashmir,GPSMA);
		Message.regist('g',&GPSMA);

		hmrv::cSpriteMsgAgent SpriteMA;
		hmrv::connect_Pad(SpriteMA,Pad1);
		Message.regist('j',&SpriteMA);

		hmrv::cThermoMsgAgent ThermoMA;
		hmrv::connect_Pad(ThermoMA,Pad1);
		Message.regist('t',&ThermoMA);

//		hmrv::cSHT75MsgAgent SHT75MA;
//		hmrv::connect_Pad(SHT75MA,Pad1);
//		Message.regist('7',&SHT75MA);

//		hmrv::cInfraRedMsgAgent InfraRedMA;
//		hmrv::connect_Pad(InfraRedMA,Pad1);
//		Message.regist('T',&InfraRedMA);

		hmrv::cCO2MsgAgent CO2MA;
		hmrv::connect_Pad(CO2MA,Pad1);
		Message.regist('C',&CO2MA);

//		hmrv::cH2SMsgAgent H2SMA;
//		hmrv::connect_Pad(H2SMA,Pad1);
//		Message.regist('S',&H2SMA);

//		hmrv::cHumidMsgAgent HumidMA;
//		hmrv::connect_Pad(HumidMA,Pad1);
//		Message.regist('h',&HumidMA);

		hmrv::cChrono Chrono;
		Message.regist('$', &Chrono);

		hmrv::cDevMngMsgAgent DevMngMA;
		Message.regist('D', &DevMngMA);

		hmrv::cLoggerMngMsgAgent LogMngMA;
		Message.regist('L', &LogMngMA);

		//�p�P�b�g�P�ʂŃZ���T�[�f�[�^��ۑ�
		//hmrv::cCSVFileAgent PacketFileAgent("Packet");
		//PacketFileAgent.slot_write(Com.signal_finRecvPacket);

		/*
		hmrv::cCSVFileAgent::cCell<hmrv::clock::time_point> TimePointCell("Time");
		Com.contactLastRecvPacTime(TimePointCell.Inquiry);
		PacketFileAgent.regist(TimePointCell);

		hmrv::cCSVFileAgent::cCell<double> ThermoCell("Thermo");
		ThermoMA.contact_getTemperature(ThermoCell.Inquiry);
		PacketFileAgent.regist(ThermoCell);

		hmrv::cCSVFileAgent::cCell<double> CO2Cell("CO2");
		CO2MA.contact_getValue(CO2Cell.Inquiry);
		PacketFileAgent.regist(CO2Cell);
*/

		//�Z���T�[�f�[�^�̂ݕۑ�
//		hmrv::cCSVFileAgent SenserFileAgent("Sensor");

		/*
		// log ����Ă������x�f�[�^�ۑ�
		hmrv::cCSVFileAgent LogThermoFileAgent("loggedThermoDat");
		LogThermoFileAgent.slot_write(ThermoMA.signal_newLogData);
		hmrv::cCSVFileAgent::cCell<hmrv::clock::time_point> logThermo_timeCell("time");
		ThermoMA.contact_getlogTime(logThermo_timeCell.Inquiry);
		LogThermoFileAgent.regist(logThermo_timeCell);
		hmrv::cCSVFileAgent::cCell<double> logThermo_dataCell("Thermo");
		ThermoMA.contact_getLogTemperature(logThermo_dataCell.Inquiry);
		LogThermoFileAgent.regist(logThermo_dataCell);
		hmrv::cCSVFileAgent::cCell<hmLib_uint16> logThermo_rawDataCell("rawThermo");
		ThermoMA.contact_getLogRawTemperature(logThermo_rawDataCell.Inquiry);
		LogThermoFileAgent.regist(logThermo_rawDataCell);

		DirectoryFile.regist(&LogThermoFileAgent);

		// �ʏ�̉��x�f�[�^�ۑ�
		hmrv::cCSVFileAgent ThermoFileAgent("ThermoDat");
		ThermoFileAgent.slot_write(ThermoMA.signal_newLogData);
		hmrv::cCSVFileAgent::cCell<hmrv::clock::time_point> thermo_timeCell("time");
		ThermoMA.contact_getTime(thermo_timeCell.Inquiry);
		ThermoFileAgent.regist(thermo_timeCell);
		hmrv::cCSVFileAgent::cCell<double> thermo_dataCell("Thermo");
		ThermoMA.contact_getTemperature(thermo_dataCell.Inquiry);
		ThermoFileAgent.regist(thermo_dataCell);
		hmrv::cCSVFileAgent::cCell<hmLib_uint16> thermo_rawDataCell("rawThermo");
		ThermoMA.contact_getRawTemperature(thermo_rawDataCell.Inquiry);
		ThermoFileAgent.regist(thermo_rawDataCell);

		DirectoryFile.regist(&ThermoFileAgent);
		*/

		//FULL ADC
		hmrv::cFullADC FullADC;
		Message.regist('f', &FullADC.MsgAgent);


		//�e�f�B���N�g��
		hmrv::cConstNameDirectoryFile DirectoryFile("Data");
		DirectoryFile.regist(&(Battery.FileAgent));
		DirectoryFile.regist(&(FullADC.FileAgent));

		// log Thermo �f�[�^��ۑ�
//		hmrv::cConstNameFileAgent<std::pair<double,std::uint16_t>> logThermoFileAgent("Thermo.tsv","\t");
//		logThermoFileAgent.slot_log_writeData(ThermoMA.signal_newLogRawData);
//		DirectoryFile.regist(&logThermoFileAgent);
		// Thermo �f�[�^��ۑ�
		hmrv::cConstNameFileAgent<std::pair<double, std::uint16_t>> ThermoFileAgent("Thermo.txt",'\t');
		ThermoFileAgent.slot_log_writeData(ThermoMA.signal_newRawData);
		DirectoryFile.regist(&ThermoFileAgent);

		// log CO2
//		hmrv::cConstNameFileAgent<std::pair<double, std::uint16_t>> logCO2FileAgent("CO2_log");
//		logCO2FileAgent.slot_log_writeData(CO2MA.signal_newLogRawData);
//		DirectoryFile.regist(&logCO2FileAgent);
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
		//hmrv::dxosIOSubPage IOSideDisp(Pint(240,720), CLR::DarkDullGreen);
		hmrv::connect(IOMainDisp,IO,ioLogBuf,ioLogGate.Logger);
		//hmrv::connect(IOSideDisp, GateSW,Operator,Bufgate);

		//���c�pMainDisplaay
		hmrv::dxosControlMainDisplay ControlMainDisp;
//		hmrv::connect(ControlMainDisp.Navigator,AcceleMA,CompassDat,GyroLogger);
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

//		hmrv::dxosBUI::DefaultClr.set(CLR::DarkSoftRed,CLR::SoftRed,CLR::LightSoftBlue,CLR::DeepSoftGreen,CLR::SoftRed,CLR::White,CLR::White,CLR::LightSoftRed,CLR::White,CLR::LightGray,CLR::Gray);
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

//		hmrv::dxosSHT75MUI SHT75MUI;
//		hmrv::connect(SHT75MUI,SHT75MA);
//		MUISideDisp.insert(&SHT75MUI);

//		hmrv::dxosInfraRedMUI InfraRedMUI;
//		hmrv::connect(InfraRedMUI,InfraRedMA);
//		MUISideDisp.insert(&InfraRedMUI);

		hmrv::dxosCO2MUI CO2MUI;
		hmrv::connect(CO2MUI,CO2MA);
		MUISideDisp.insert(&CO2MUI);

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
		//Display.registSide(&IOSideDisp);
	
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

/*
		struct cDebug{
			bool IsTimeout;
			hmrv::clock::time_point Timeout;
			bool IsNullData;
			hmrv::clock::time_point NullData;
			cDebug():IsTimeout(false),IsNullData(false){}
			void operator()(void){
				auto Time=hmrv::clock::now();
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
			void slot_timeout(boost::signals2::signal<void(hmrv::clock::time_point)>& Signal_){
				hmLib::signals::connect(Signal_,[&](hmrv::clock::time_point Time_)->void{this->Timeout=Time_;});
			}
			void slot_nulldata(boost::signals2::signal<void(hmrv::clock::time_point)>& Signal_){
				hmLib::signals::connect(Signal_,[&](hmrv::clock::time_point Time_)->void{this->NullData=Time_;});
			}
		}Debug;
		boost::signals2::signal<void(hmrv::clock::time_point)> DebugSignal;
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
//			DebugSignal(hmrv::clock::now());

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
