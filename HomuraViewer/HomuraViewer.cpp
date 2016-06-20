/*===========hmrV2500.cpp===========
hmrV2500 v1_06/160601
	a
		Git移動後初めてコンパイルに成功
	b
		
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
#include <hmLib/config_vc.h> // std::min, std::max　は　windowsマクロでも定義されており、明示的に分離してやらないとだめ
#include<hmLib/bufgate.hpp>
#include<hmLib/any_iterator.hpp>
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

#include"hmrBattery.hpp"
#include"hmrDxBatteryMUI.hpp"

//include"hmrHumid.hpp"
//#include"hmrDxHumidMUI.hpp"

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

#include <hmrVLib/File.hpp>
#include "hmrGPSFile.hpp"
#include "hmrSpriteFile.hpp"
#include "hmrCSVFile.hpp"
#include "hmrUniSensorFile.hpp"

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
	//hmLib_dxモジュールを初期化
	dx::ini("hmrV2500_v1_06b", 960,720);


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
		hmr::cFHDxOperator Operator(&Message,&Com,true,std::chrono::milliseconds(250),std::chrono::seconds(1));
		hmr::connect(Operator,IO,Com);

		//各モジュール宣言		
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
		hmr::cAcceleLogger AcceleLogger;
		AcceleLogger.slot_addLog(AcceleMA.signal_newData);
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
		hmr::cGyroCompass GyroCompass;
		hmr::connect(GyroCompass,GyroMA);

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

//		hmr::cSHT75MsgAgent SHT75MA;
//		hmr::connect_Pad(SHT75MA,Pad1);
//		Message.regist('7',&SHT75MA);

//		hmr::cInfraRedMsgAgent InfraRedMA;
//		hmr::connect_Pad(InfraRedMA,Pad1);
//		Message.regist('T',&InfraRedMA);

		hmr::cCO2MsgAgent CO2MA;
		hmr::connect_Pad(CO2MA,Pad1);
		Message.regist('C',&CO2MA);

//		hmr::cH2SMsgAgent H2SMA;
//		hmr::connect_Pad(H2SMA,Pad1);
//		Message.regist('S',&H2SMA);

//		hmr::cHumidMsgAgent HumidMA;
//		hmr::connect_Pad(HumidMA,Pad1);
//		Message.regist('h',&HumidMA);

		hmr::cChrono Chrono;
		Message.regist('$', &Chrono);

		hmr::cDevMngMsgAgent DevMngMA;
		Message.regist('D', &DevMngMA);

		hmr::cLoggerMngMsgAgent LogMngMA;
		Message.regist('L', &LogMngMA);

		hmr::cDirectoryFile DirectoryFile;
		//パケット単位でセンサーデータを保存
		//hmr::cCSVFileAgent PacketFileAgent("Packet");
		//PacketFileAgent.slot_write(Com.signal_finRecvPacket);

		/*
		hmr::cCSVFileAgent::cCell<hmr::clock::time_point> TimePointCell("Time");
		Com.contactLastRecvPacTime(TimePointCell.Inquiry);
		PacketFileAgent.regist(TimePointCell);

		hmr::cCSVFileAgent::cCell<double> ThermoCell("Thermo");
		ThermoMA.contact_getTemperature(ThermoCell.Inquiry);
		PacketFileAgent.regist(ThermoCell);

		hmr::cCSVFileAgent::cCell<double> CO2Cell("CO2");
		CO2MA.contact_getValue(CO2Cell.Inquiry);
		PacketFileAgent.regist(CO2Cell);
*/

		//センサーデータのみ保存
//		hmr::cCSVFileAgent SenserFileAgent("Sensor");

		/*
		// log されていた温度データ保存
		hmr::cCSVFileAgent LogThermoFileAgent("loggedThermoDat");
		LogThermoFileAgent.slot_write(ThermoMA.signal_newLogData);
		hmr::cCSVFileAgent::cCell<hmr::clock::time_point> logThermo_timeCell("time");
		ThermoMA.contact_getlogTime(logThermo_timeCell.Inquiry);
		LogThermoFileAgent.regist(logThermo_timeCell);
		hmr::cCSVFileAgent::cCell<double> logThermo_dataCell("Thermo");
		ThermoMA.contact_getLogTemperature(logThermo_dataCell.Inquiry);
		LogThermoFileAgent.regist(logThermo_dataCell);
		hmr::cCSVFileAgent::cCell<hmLib_uint16> logThermo_rawDataCell("rawThermo");
		ThermoMA.contact_getLogRawTemperature(logThermo_rawDataCell.Inquiry);
		LogThermoFileAgent.regist(logThermo_rawDataCell);

		DirectoryFile.regist(&LogThermoFileAgent);

		// 通常の温度データ保存
		hmr::cCSVFileAgent ThermoFileAgent("ThermoDat");
		ThermoFileAgent.slot_write(ThermoMA.signal_newLogData);
		hmr::cCSVFileAgent::cCell<hmr::clock::time_point> thermo_timeCell("time");
		ThermoMA.contact_getTime(thermo_timeCell.Inquiry);
		ThermoFileAgent.regist(thermo_timeCell);
		hmr::cCSVFileAgent::cCell<double> thermo_dataCell("Thermo");
		ThermoMA.contact_getTemperature(thermo_dataCell.Inquiry);
		ThermoFileAgent.regist(thermo_dataCell);
		hmr::cCSVFileAgent::cCell<hmLib_uint16> thermo_rawDataCell("rawThermo");
		ThermoMA.contact_getRawTemperature(thermo_rawDataCell.Inquiry);
		ThermoFileAgent.regist(thermo_rawDataCell);

		DirectoryFile.regist(&ThermoFileAgent);
		*/

		// log Thermo データを保存
		hmr::cUniSensorFileAgent<std::pair<double,std::uint16_t>> logThermoFileAgent("Thermo_log");
		logThermoFileAgent.slot_log_writeData(ThermoMA.signal_newLogRawData);
		DirectoryFile.regist(&logThermoFileAgent);
		// Thermo データを保存
		hmr::cUniSensorFileAgent<std::pair<double, std::uint16_t>> ThermoFileAgent("Thermo_normal");
		ThermoFileAgent.slot_log_writeData(ThermoMA.signal_newRawData);
		DirectoryFile.regist(&ThermoFileAgent);

		// log CO2
		hmr::cUniSensorFileAgent<std::pair<double, std::uint16_t>> logCO2FileAgent("CO2_log");
		logCO2FileAgent.slot_log_writeData(CO2MA.signal_newLogRawData);
		DirectoryFile.regist(&logCO2FileAgent);
		// CO2 データを保存
		hmr::cUniSensorFileAgent<std::pair<double, std::uint16_t>> CO2FileAgent("CO2_normal");
		CO2FileAgent.slot_log_writeData(CO2MA.signal_newRawData);
		DirectoryFile.regist(&CO2FileAgent);

		//GPGGAデータを保存
		hmr::cGPSFileAgent GPSFileAgent;
		hmr::connect(GPSFileAgent, GPSMA);
		DirectoryFile.regist(&GPSFileAgent);

		//カメラデータを保存
		hmr::cSpriteFileAgent SpriteFileAgent("Sprite");
		hmr::connect(SpriteFileAgent, SpriteMA,false);
		DirectoryFile.regist(&SpriteFileAgent);

		//カメラログデータを保存
		hmr::cSpriteFileAgent SpriteLogFileAgent("SpriteLog");
		hmr::connect(SpriteLogFileAgent, SpriteMA, true);
		DirectoryFile.regist(&SpriteLogFileAgent);

/*
		// File 系列の宣言
		hmr::cWholeFileAgent WholeFA;
		
		hmr::cAcceleFileAgent AcceleFA;
		hmr::connect(AcceleFA,AcceleMA);
		WholeFA.regist(&AcceleFA);

		hmr::cBatteryFileAgent<3> BatteryFA;
		hmr::connect(BatteryFA, BatteryMA);
		WholeFA.regist(&BatteryFA);

		hmr::cCompassFileAgent CompassFA;
		hmr::connect(CompassFA, CompassMA);
		WholeFA.regist(&CompassFA);

		hmr::cGPSFileAgent GPSFA;
		hmr::connect(GPSFA, GPSMA);
		WholeFA.regist(&GPSFA);
		
		hmr::cGyroFileAgent GyroFA;
		hmr::connect(GyroFA, GyroMA);
		WholeFA.regist(&GyroFA);
		
//		hmr::cSensorsFileAgent SensorFA;
//		hmr::connect(SensorFA, CO2MA, H2SMA, HumidMA, InfraRedMA, ThermoMA);
//		WholeFA.regist(&SensorFA);
		
		hmr::cSpriteFileAgent SpriteFA;
		hmr::connect(SpriteFA, SpriteMA);
		WholeFA.regist(&SpriteFA);
		
*/
		// SUI 系列
		hmr::dxosBUIBoxSideDisplay SystemSideDisp;
		SystemSideDisp.ClrSet.Background=CLR::DarkSoftBlue;

		hmr::dxosChronoSUI ChronoSUI;
		hmr::connect(ChronoSUI, Chrono);
		SystemSideDisp.insert(&ChronoSUI);

		hmr::dxosDevMngSUI DevMngSUI;
		hmr::connect(DevMngSUI, DevMngMA);
		SystemSideDisp.insert(&DevMngSUI);

		hmr::dxosLoggerMngSUI LogMngSUI;
		hmr::connect(LogMngSUI, LogMngMA);
		SystemSideDisp.insert(&LogMngSUI);

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

		hmr::dxosFileSUI FileSUI;
		hmr::connect(FileSUI, DirectoryFile);
		SystemSideDisp.insert(&FileSUI);


		// IO View Display の定義
		hmr::dxosIO2<iologbuf::iterator> IOMainDisp(Pint(720,720), CLR::DarkDullGreen,CLR::SoftGreen,CLR::LightSoftOrenge,CLR::LightSoftSkyblue);
		//hmr::dxosIOSubPage IOSideDisp(Pint(240,720), CLR::DarkDullGreen);
		hmr::connect(IOMainDisp,IO,ioLogBuf,ioLogGate.Logger);
		//hmr::connect(IOSideDisp, GateSW,Operator,Bufgate);

		//操縦用MainDisplaay
		hmr::dxosControlMainDisplay ControlMainDisp;
//		hmr::connect(ControlMainDisp.Navigator,AcceleMA,CompassDat,GyroLogger);
		hmr::connect(ControlMainDisp.Navigator,AcceleLogger,CompassDat,GyroLogger,GyroCompass);
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

		//MUI用サイドバー宣言

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

//		hmr::dxosSHT75MUI SHT75MUI;
//		hmr::connect(SHT75MUI,SHT75MA);
//		MUISideDisp.insert(&SHT75MUI);

//		hmr::dxosInfraRedMUI InfraRedMUI;
//		hmr::connect(InfraRedMUI,InfraRedMA);
//		MUISideDisp.insert(&InfraRedMUI);

		hmr::dxosCO2MUI CO2MUI;
		hmr::connect(CO2MUI,CO2MA);
		MUISideDisp.insert(&CO2MUI);

//		hmr::dxosH2SMUI H2SMUI;
//		hmr::connect(H2SMUI,H2SMA);
//		MUISideDisp.insert(&H2SMUI);

//		hmr::dxosHumidMUI HumidMUI;
//		hmr::connect(HumidMUI,HumidMA);
//		MUISideDisp.insert(&HumidMUI);

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

/*
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
*/

		while(!dx::work(30)){
			Keyboard.work();
			Pad1.work();

			Bufgate();
			IO.work();
			Operator();

//			Debug();
//			DebugSignal(hmr::clock::now());

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
