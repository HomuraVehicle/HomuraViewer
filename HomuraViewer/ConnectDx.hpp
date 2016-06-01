#ifdef HMR_MAIN_INC_END
#ifndef HMR_CONNECTDX_INC
#define HMR_CONNECTDX_INC 100
#

//#include "hmrDxGateSwitcher.hpp"
//#include "hmrGateSwitcher.hpp"

/*===hmrConnect===
接続補助関数群を定義する
*/
//#include"hmrPublicSignal.hpp"
namespace hmr{
#if defined(HMR_DXCOM_INC) && defined(HMR_COM_INC) && defined(HMR_COMLOG_INC)
	void connect(dxosPacketList_withView<cComLog::iterator>& DxCom_,cCom& Com_,cComLog& ComLog_){
		Com_.contactRecvLog(DxCom_.inquiryNewInPacket);
		Com_.contactSendLog(DxCom_.inquiryNewOutPacket);
		ComLog_.contactLogBuf(DxCom_.inquiryPacketLog);
	}
#endif

// DXIO に関するコネクトすべてをつかさどる関数
 // DxIOmain関連
#if defined(HMR_DXIODISPLAY_INC) && defined(HMRIO_INC) && defined(IOLOGGATE_INC)
	typedef std::pair<bool,system_clock_iologtype> io_iologtype;
	typedef std::vector<io_iologtype> container;
	typedef container::const_iterator contanerItr;
	void connect(dxosIOMainPage<contanerItr>& IOmain_, cFHdxIO& cIO_, fdx_vector_iologbuf<system_clock_iologtype>& ioLog_){
		//自身の内部の接続
		IOmain_.DxIO.slot_setInPaintMode(IOmain_.DxSelectIOPaint.signal_require_paintInput);
		IOmain_.DxIO.slot_setOutPaintMode(IOmain_.DxSelectIOPaint.signal_require_paintOutput);
		IOmain_.DxIO.contact_InPaintFlag(IOmain_.DxSelectIOPaint.InPaintFlagInq);
		IOmain_.DxIO.contact_OutPaintFlag(IOmain_.DxSelectIOPaint.OutPaintFlagInq);	

		//対IO
		cIO_.slot_send_direct(IOmain_.DxSendBox.signal_poutString);

		//対ioLog
		ioLog_.contactLogBuf(IOmain_.DxIO.IoLogBufInq);		
	}
#endif
 // DxIOsub関連
#if defined(HMR_DXIODISPLAY_INC) && defined(HMR_GATEHAB_INC) && defined(HMR_OPERATOR_INC) && defined(HMR_BUFGATE_INC)
	void connect(dxosIOSubPage& IOsub_, cGateSwitcher& cGateSwitcher_, cFHDxOperator& operator_, bufgate& bufgate_){
		//対GateSwitcher
		cGateSwitcher_.slot_set_and_openRS(IOsub_.DxosGateSwitcher.signal_require_openRS);
		cGateSwitcher_.slot_set_and_openFOMA(IOsub_.DxosGateSwitcher.signal_require_openFOMA);
		cGateSwitcher_.slot_closePort(IOsub_.DxosGateSwitcher.signal_require_closeRS);
		cGateSwitcher_.slot_closePort(IOsub_.DxosGateSwitcher.signal_require_closeFOMA);
		dxosGateSwitcher_.slot_sync_isFOMAOpen(IOsub_.DxosGateSwitcher.signal_inform_isFOMAOpen);
		dxosGateSwitcher_.slot_sync_isRSOpen(IOsub_.DxosGateSwitcher.signal_inform_isRSOpen);

		//対Operator
		operator_.slot_setIsSendActiveFlag(IOsub_.DxSendStartButt.signal_require_setStartFlag);
		operator_.contact_IsSendActive(IOsub_.DxSendStartButt.StartFlagInq);
		operator_.slot_setIsFullDuplex(IOsub_.DxSelectDuplex.signal_require_isFullDuplex);
		operator_.contact_IsFullDuplex(IOsub_.DxSelectDuplex.IsFullDuplexFlagInq);
		operator_.slot_setTimeOut(IOsub_.DxSendTimeOutCtr.signal_require_setTimeOut);
		operator_.contact_TimeOut(IOsub_.DxSendTimeOutCtr.TimeOutInq);
		operator_.slot_setSendInterval(IOsub_.DxSendIntervalCtr.signal_require_setTimeInterval);
		operator_.contact_Interval(IOsub_.DxSendIntervalCtr.TimeIntervalInq);
		operator_.slot_setSendInterval(IOsub_.DxSyncCheckBox.signal_require_setTimeInterval);
		operator_.contact_IsFullDuplex(IOsub_.DxSyncCheckBox.IsFullDuplexInq);
		operator_.contact_Interval(IOsub_.DxSyncCheckBox.TimeIntervalInq);

		//対BufGate
		bufgate_.slot_clearSendBuf(IOsub_.DxSendBufCtr.signal_require_clearBuf);
		bufgate_.contact_sendBufSize(IOsub_.DxSendBufCtr.BufSizeInq);
		bufgate_.slot_clearRecvBuf(IOsub_.DxSendBufCtr.signal_require_clearBuf);
		bufgate_.contact_recvBufSize(IOsub_.DxSendBufCtr.BufSizeInq);		
	}

	void connect(dxosIOSubPage& IOsub_, cGateSwitcher& cGateSwitcher_, cFullRecvTimeSendOperator& operator_, bufgate& bufgate_){
		//対GateSwitcher
		cGateSwitcher_.slot_set_and_openRS(IOsub_.DxosGateSwitcher.signal_require_openRS);
		cGateSwitcher_.slot_set_and_openFOMA(IOsub_.DxosGateSwitcher.signal_require_openFOMA);
		cGateSwitcher_.slot_closePort(IOsub_.DxosGateSwitcher.signal_require_closeRS);
		cGateSwitcher_.slot_closePort(IOsub_.DxosGateSwitcher.signal_require_closeFOMA);
		dxosGateSwitcher_.slot_sync_isFOMAOpen(IOsub_.DxosGateSwitcher.signal_inform_isFOMAOpen);
		dxosGateSwitcher_.slot_sync_isRSOpen(IOsub_.DxosGateSwitcher.signal_inform_isRSOpen);

		//対Operator
		operator_.slot_setIsSendActiveFlag(IOsub_.DxSendStartButt.signal_require_setStartFlag);
		operator_.contact_IsSendActive(IOsub_.DxSendStartButt.StartFlagInq);
		operator_.slot_setIsFullDuplex(IOsub_.DxSelectDuplex.signal_require_isFullDuplex);
		operator_.contact_IsFullDuplex(IOsub_.DxSelectDuplex.IsFullDuplexFlagInq);
		operator_.slot_setTimeOut(IOsub_.DxSendTimeOutCtr.signal_require_setTimeOut);
		operator_.contact_TimeOut(IOsub_.DxSendTimeOutCtr.TimeOutInq);
		operator_.slot_setSendInterval(IOsub_.DxSendIntervalCtr.signal_require_setTimeInterval);
		operator_.contact_Interval(IOsub_.DxSendIntervalCtr.TimeIntervalInq);
		operator_.slot_setSendInterval(IOsub_.DxSyncCheckBox.signal_require_setTimeInterval);
		operator_.contact_IsFullDuplex(IOsub_.DxSyncCheckBox.IsFullDuplexInq);
		operator_.contact_Interval(IOsub_.DxSyncCheckBox.TimeIntervalInq);

		//対BufGate
		bufgate_.slot_clearSendBuf(IOsub_.DxSendBufCtr.signal_require_clearBuf);
		bufgate_.contact_sendBufSize(IOsub_.DxSendBufCtr.BufSizeInq);
		bufgate_.slot_clearRecvBuf(IOsub_.DxSendBufCtr.signal_require_clearBuf);
		bufgate_.contact_recvBufSize(IOsub_.DxSendBufCtr.BufSizeInq);		
	}
#endif

#if defined(HMR_DXIO2_INC) && defined(HMRIO_INC) && defined(IOLOGGATE_INC)
	void connect(dxosIO2<fdx_vector_iologbuf<system_clock_iologtype>::iterator>& IOmain_, cFHdxIO& IO_, fdx_vector_iologbuf<system_clock_iologtype>& ioLogBuf_,fdx_crlf_timeout_iologger<system_clock_iologtype>& ioLogger_){
		//DxIO & cIO
		IO_.slot_send_direct(IOmain_.signal_poutString);

		//DxIO & ioLogBuf
		ioLogBuf_.contactLogBuf(IOmain_.inquiry_LogBuf);

		//DxIO & ioLogger
		ioLogger_.contacInLog(IOmain_.inquiry_InStr);
		ioLogger_.contacOutLog(IOmain_.inquiry_OutStr);
	}
#endif

// SUI 関連のコネクト
#if defined(HMR_DXGATE_SUI_INC) &&  defined(HMR_GATEHAB_INC)
	void connect(dxosGateSwitcherSUI& dxosGateSwitcherSUI_, cGateSwitcher& cGateSwitcher_){
		// GateSwUI を　GateSwitcher　に　つなげる
		// Switch系の　require signal　をつなげる
		cGateSwitcher_.slot_set_and_openRS(dxosGateSwitcherSUI_.signal_require_openRS);
		cGateSwitcher_.slot_set_and_openFOMA(dxosGateSwitcherSUI_.signal_require_openFOMA);
		cGateSwitcher_.slot_closePort(dxosGateSwitcherSUI_.signal_require_closeRS);
		cGateSwitcher_.slot_closePort(dxosGateSwitcherSUI_.signal_require_closeFOMA);

		// Switch系の　データのシンクロ　を行う
		dxosGateSwitcherSUI_.slot_sync_isFOMAOpen(cGateSwitcher_.signal_inform_isFOMAOpen);
		dxosGateSwitcherSUI_.slot_sync_isRSOpen(cGateSwitcher_.signal_inform_isRSOpen);


		// Inquire 系列の接続
		cGateSwitcher_.contact_is_open(dxosGateSwitcherSUI_.isOpenInq);
		cGateSwitcher_.contact_can_put(dxosGateSwitcherSUI_.canSendInq);
		cGateSwitcher_.contact_can_get(dxosGateSwitcherSUI_.canGetInq);
	}
#endif

#if defined(HMR_DXBUFGATE_SUI_INC) &&  defined(HMR_BUFGATE_INC)
	void connect(dxosBufGateSUI& dxosBufGateSUI_, bufgate& bufgate_){
		// BufGate SUI 
		bufgate_.slot_clearSendBuf(dxosBufGateSUI_.signal_require_clearOutBuf);
		bufgate_.slot_clearRecvBuf(dxosBufGateSUI_.signal_require_clearInBuf);

		bufgate_.contact_sendBufSize(dxosBufGateSUI_.outBufSizeInq);
		bufgate_.contact_recvBufSize(dxosBufGateSUI_.inBufSizeInq);

		bufgate_.contact_is_open(dxosBufGateSUI_.isOpenInq);
		bufgate_.contact_can_get(dxosBufGateSUI_.canGetInq);
		bufgate_.contact_can_put(dxosBufGateSUI_.canSendInq);
	}
#endif

#if defined(HMR_DXIOLOGGATE_SUI_INC) &&  defined(IOLOGGATE_INC)
	typedef std::pair<bool,system_clock_iologtype> io_iologtype;
	typedef std::vector<io_iologtype> container;
	typedef container::const_iterator contanerItr;
	
	//Log Buf とのコネクト
	//template <typename iterator>
	void connect(dxosIOLogGateSUI& dxosLogSUI_, iologgate<fdx_crlf_timeout_iologger<system_clock_iologtype>>& logGate_, fdx_vector_iologbuf<system_clock_iologtype>& logBuf_){
		logBuf_.slot_clearLogBuf(dxosLogSUI_.signal_require_clearBuf);
		logBuf_.contactLogBufSize(dxosLogSUI_.BufSizeInq);

		logGate_.contact_is_open(dxosLogSUI_.isOpenInq);
		logGate_.contact_can_put(dxosLogSUI_.canSendInq);
		logGate_.contact_can_get(dxosLogSUI_.canGetInq);
	}
#endif

#if defined(HMR_DXIO_SUI_INC) && defined(HMRIO_INC)
	void connect(dxosIOSUI& IOSUI_, cFHdxIO& cIO_){
		cIO_.contact_is_open(IOSUI_.isOpenInq);
		cIO_.contact_can_get(IOSUI_.canGetInq);
		cIO_.contact_can_put(IOSUI_.canSendInq);
	}
#endif

#if defined(HMR_DXVMC_SUI_INC) && defined(HMRIO_INC) 
	void connect(dxosVMCSUI& VMCSUI_, cFHdxIO& cIO_){
		cIO_.contactRecvErr(VMCSUI_.recvErrInq);
		cIO_.contactSendErr(VMCSUI_.sendErrInq);
		cIO_.slot_VMC_clearRecvErr(VMCSUI_.signal_require_clear_recvErr);
		cIO_.slot_VMC_clearSendErr(VMCSUI_.signal_require_clear_sendErr);
		cIO_.slot_VMC_force_end_recv(VMCSUI_.signal_require_end_recv);
		cIO_.slot_VMC_force_end_send(VMCSUI_.signal_require_end_send);

		VMCSUI_.slot_set_forceResetRecvTime(cIO_.signal_inform_VMC_force_end_recv);
		VMCSUI_.slot_set_forceResetSendTime(cIO_.signal_inform_VMC_force_end_send);
	}
#endif

#if defined(HMR_DXCOMSUI_INC) && defined(HMR_COM_INC)
	void connect(dxosComSUI& ComSUI,cCom& Com){
		Com.contactRecvBufSize(ComSUI.RecvBufSizeInq);
		Com.contactSendBufSize(ComSUI.SendBufSizeInq);
		Com.contactLastRecvPacTime(ComSUI.RecvTimeInq);
		Com.contactLastSendPacTime(ComSUI.SendTimeInq);

		Com.slot_clearRecvBuf(ComSUI.signal_require_clearRecvBuf);
		Com.slot_clearSendBuf(ComSUI.signal_require_clearSendBuf);
	}
#endif


#if defined(HMR_DXOPERATOR_SUI_INC) && defined(HMR_OPERATOR_INC)
	void connect(dxosOperatorSUI& opSUI_, cFullRecvTimeSendOperator& op_){
		op_.slot_setIsSendActiveFlag(opSUI_.signal_require_setStartFlag);
		op_.slot_setIsFullDuplex(opSUI_.signal_require_isFullDuplex);
		op_.slot_setSendInterval(opSUI_.signal_require_setTimeInterval);
		op_.slot_setTimeOut(opSUI_.signal_require_setTimeOut);
		
		op_.contact_IsSendActive(opSUI_.startFlagInq);
		op_.contact_IsFullDuplex(opSUI_.isFullDuplexFlagInq);
		op_.contact_Interval(opSUI_.timeIntervalInq);
		op_.contact_TimeOut(opSUI_.timeOutInq);

		opSUI_.slot_setIsReceivedFlag(op_.signal_inform_Received);
		opSUI_.slot_setIsSendedFlag(op_.signal_inform_Sended);
		opSUI_.slot_setIsTimeOutedFlag(op_.signal_inform_TimeOut);
	}
	void connect(dxosOperatorSUI& opSUI_, cFHDxOperator& op_){
		op_.slot_setIsSendActiveFlag(opSUI_.signal_require_setStartFlag);
		op_.slot_setIsFullDuplex(opSUI_.signal_require_isFullDuplex);
		op_.slot_setInterval(opSUI_.signal_require_setTimeInterval);
		op_.slot_setTimeOut(opSUI_.signal_require_setTimeOut);
		
		op_.contactIsSendActive(opSUI_.startFlagInq);
		op_.contactIsFullDuplex(opSUI_.isFullDuplexFlagInq);
		op_.contactInterval(opSUI_.timeIntervalInq);
		op_.contactTimeOut(opSUI_.timeOutInq);

		opSUI_.slot_setIsReceivedFlag(op_.signal_informFinReceive);
		opSUI_.slot_setIsSendedFlag(op_.signal_informFinSend);
		opSUI_.slot_setIsTimeOutedFlag(op_.signal_informTimeOut);
	}
#endif

#if defined(HMR_DXFILE_SUI_INC) && defined(HMR_WHOLEFILE_INC)
	void connect(dxosFileSUI& fileSUI_, cWholeFileAgent& agt_){
		fileSUI_.slot_setIsActive(agt_.signal_isActivate);
		agt_.slot_activate(fileSUI_.signal_require_activate);
		agt_.slot_inactivate(fileSUI_.signal_require_inactivate);
	}
#endif
#if defined(HMR_DXFILE_SUI_INC) && defined(HMR_DIRECTORYFILE_INC)
	void connect(dxosFileSUI& fileSUI_, cDirectoryFile& agt_){
		fileSUI_.slot_setIsActive(agt_.signal_isActivate);
		agt_.slot_activate(fileSUI_.signal_require_activate);
		agt_.slot_inactivate(fileSUI_.signal_require_inactivate);
	}
#endif

#if defined(HMR_DXDEVICEMANAGESUI_INC) && defined(HMR_DEVICEMANAGE_INC)
	void connect(dxosDevMngSUI& devSUI_, cDevMngMsgAgent& agt_){
		agt_.slot_setNormalMode(devSUI_.signal_setNormalMode);
		//agt_.slot_setTurboMode(devSUI_.signal_setTurboMode);
		agt_.slot_setSleepMode(devSUI_.signal_setSleepMode);
		agt_.slot_exeSleepMode(devSUI_.signal_exeSleepMode);
		agt_.slot_setRoamMode(devSUI_.signal_setRoamMode);
		agt_.slot_exeRoamMode(devSUI_.signal_exeRoamMode);
		agt_.slot_setKillMode(devSUI_.signal_setKillMode);
		agt_.slot_exeKillMode(devSUI_.signal_exeKillMode);
		agt_.slot_setInfo(devSUI_.signal_setGettingInfo);
		agt_.slot_setClockMode(devSUI_.signal_setClockMode);
		
		agt_.contact_getDevMODE(devSUI_.devMODEInq);
		agt_.contact_getClockMODE(devSUI_.clockMODEInq);
		agt_.contact_getSendingKillCommand(devSUI_.SendingKillCommandInq);
		agt_.contact_getSendingNormalCommand(devSUI_.SendingNormalCommandInq);
		agt_.contact_getUseMP(devSUI_.UseMPInq);
		agt_.contact_getSendingRoamCommand(devSUI_.SendingRoamCommandInq);
		agt_.contact_getSendingSleepCommand(devSUI_.SendingSleepCommandInq);
		agt_.contact_getSendingClockCommand(devSUI_.SendingClockCommandInq);
		agt_.contact_getSendingInfoCommand(devSUI_.SendingInfoCommandInq);
		//agt_.contact_getSendingTurboCommand(devSUI_.SendingTurboCommandInq);
		agt_.contact_IsReadyKillExe(devSUI_.IsReadyKillInq);
		agt_.contact_IsReadyRoamExe(devSUI_.IsReadyRoamInq);
		agt_.contact_IsReadySleepExe(devSUI_.IsReadySleepInq);
	}
#endif


#if defined(HMR_DXTHERMOMUI_INC) && defined(HMR_THERMO_INC)
	void connect(dxosThermoMUI& ThermoMUI_, cThermoMsgAgent& Thermo_){
		//Thermo_.contact_getTemperature(ThermoMUI_.inquiry_getThermo);
		//Thermo_.contact_getTime(ThermoMUI_.inquiry_getTime);
		ThermoMUI_.slot_getData(Thermo_.signal_newData);
		Thermo_.contact_getPicDataMode(ThermoMUI_.IsDataModeMUIBut.Pic);
		Thermo_.contact_getRequestDataMode(ThermoMUI_.IsDataModeMUIBut.Req);
		Thermo_.slot_setDataMode(ThermoMUI_.IsDataModeMUIBut.Signal);
	}
#endif
	
#if defined(HMR_DXSHT75MUI_INC) && defined(HMR_SHT75_INC)
	void connect(dxosSHT75MUI& SHT75MUI_, cSHT75MsgAgent& SHT75_){
		//Thermo_.contact_getTemperature(ThermoMUI_.inquiry_getThermo);
		//Thermo_.contact_getTime(ThermoMUI_.inquiry_getTime);
		SHT75MUI_.slot_getData(SHT75_.signal_newData);
		SHT75_.contact_getPicDataMode(SHT75MUI_.IsDataModeMUIBut.Pic);
		SHT75_.contact_getRequestDataMode(SHT75MUI_.IsDataModeMUIBut.Req);
		SHT75_.slot_setDataMode(SHT75MUI_.IsDataModeMUIBut.Signal);
		SHT75_.contact_getPicLowAcc(SHT75MUI_.IsLowAccMUIBut.Pic);
		SHT75_.contact_getRequestLowAcc(SHT75MUI_.IsLowAccMUIBut.Req);
		SHT75_.slot_setLowAcc(SHT75MUI_.IsLowAccMUIBut.Signal);
		SHT75_.contact_getRequestReset(SHT75MUI_.RequestRestMUIBut.Req);
		SHT75_.slot_requestReset(SHT75MUI_.RequestRestMUIBut.Signal);
	}
#endif


#if defined(HMR_DXSPRITE_INC) && defined(HMR_SPRITE_INC)
	void connect(dxosSprite& Sprite_, cSpriteMsgAgent& Agent_){
		Sprite_.slot_setNewGraph(Agent_.signal_setPicture);
	}
#endif
#if defined(HMR_DXSPRITEMUI_INC) && defined(HMR_SPRITE_INC)
	void connect(dxosSpriteMUI& MUI_, cSpriteMsgAgent& Agent_){
		Agent_.slot_setInfoMode(MUI_.InfoModeMUIHead.Signal);
		Agent_.contact_getPicInfoMode(MUI_.InfoModeMUIHead.Pic);
		Agent_.contact_getRequestInfoMode(MUI_.InfoModeMUIHead.Req);

		Agent_.contact_getStatus(MUI_.inquiry_getStatus);
		Agent_.contact_getIsErr(MUI_.inquiry_getIsErr);
		Agent_.contact_getErrCode(MUI_.inquiry_getErrCode);
		Agent_.contact_getStatusTime(MUI_.inquiry_getStatusTime);
		Agent_.contact_getDataPosSize(MUI_.inquiry_getDataPosSize);
		Agent_.contact_getDataTime(MUI_.inquiry_getDataTime);

		Agent_.contact_getRequestAutoTakePicture(MUI_.AutoTakePictureMUIBut.Req);
		Agent_.contact_getPicAutoTakePicture(MUI_.AutoTakePictureMUIBut.Pic);
		Agent_.slot_setAutoTakePicture(MUI_.AutoTakePictureMUIBut.Signal);

		Agent_.slot_setTakePicture(MUI_.TakePictureMUIBut.Signal);
		Agent_.contact_getDoTakePictureFlag(MUI_.TakePictureMUIBut.Req);

		Agent_.contact_getPictureSize(MUI_.inquiry_getPictureSize);
		Agent_.slot_setPictureSize(MUI_.signal_setPictureSize);

		Agent_.contact_getRequestAutoLight(MUI_.AutoLightMUIBut.Req);
		Agent_.contact_getPicAutoLight(MUI_.AutoLightMUIBut.Pic);
		Agent_.slot_setAutoLight(MUI_.AutoLightMUIBut.Signal);

		Agent_.contact_getPicLight(MUI_.LightMUIBut.Pic);
		Agent_.contact_getRequestLight(MUI_.LightMUIBut.Req);
		Agent_.slot_setLight(MUI_.LightMUIBut.Signal);

		Agent_.contact_getRequestAutoPowerReset(MUI_.AutoPowerResetMUIBut.Req);
		Agent_.contact_getPicAutoPowerReset(MUI_.AutoPowerResetMUIBut.Pic);
		Agent_.slot_setAutoPowerReset(MUI_.AutoPowerResetMUIBut.Signal);

		Agent_.contact_getDoCommandResetFlag(MUI_.CommandResetMUIBut.Req);
		Agent_.slot_setCommandReset(MUI_.CommandResetMUIBut.Signal);

		Agent_.contact_getDoPowerResetFlag(MUI_.PowerResetMUIBut.Req);
		Agent_.slot_setPowerReset(MUI_.PowerResetMUIBut.Signal);

		Agent_.contact_getPicMiniPacketMode(MUI_.MiniPacketModeMUIBut.Pic);
		Agent_.contact_getRequestMiniPacketMode(MUI_.MiniPacketModeMUIBut.Req);
		Agent_.slot_setMiniPacketMode(MUI_.MiniPacketModeMUIBut.Signal);
	}
#endif

#if defined(HMR_DXCO2MUI_INC) && defined(HMR_CO2_INC)
	void connect(dxosCO2MUI& MUI_,cCO2MsgAgent& Agent_){
		//Agent_.contact_getValue(MUI_.inquiry_getValue);
		//Agent_.contact_getTime(MUI_.inquiry_getTime);
		MUI_.slot_getData(Agent_.signal_newData);

		Agent_.contact_getPicDataMode(MUI_.IsDataModeMUIBut.Pic);
		Agent_.contact_getRequestDataMode(MUI_.IsDataModeMUIBut.Req);
		Agent_.slot_setDataMode(MUI_.IsDataModeMUIBut.Signal);

		Agent_.contact_getPicPumpPW(MUI_.PumpPWMUIBut.Pic);
		Agent_.contact_getRequestPumpPW(MUI_.PumpPWMUIBut.Req);
		Agent_.slot_setPumpPW(MUI_.PumpPWMUIBut.Signal);

		Agent_.contact_getPicSensorPW(MUI_.SensorPWMUIBut.Pic);
		Agent_.contact_getRequestSensorPW(MUI_.SensorPWMUIBut.Req);
		Agent_.slot_setSensorPW(MUI_.SensorPWMUIBut.Signal);
	}
#endif

#if defined(HMR_DXACCELEMUI_INC) && defined(HMR_ACCELE_INC)
	void connect(dxosAcceleMUI& MUI_,cAcceleMsgAgent& Agent_){
		//Agent_.contact_getAccelePosition(MUI_.inquiry_getAccelePosition);
		//Agent_.contact_getAccelePolar(MUI_.inquiry_getAccelePolar);
		//Agent_.contact_getTime(MUI_.inquiry_getTime);

		MUI_.slot_getData(Agent_.signal_newData);

		Agent_.contact_getPicDataMode(MUI_.IsDataModeMUIBut.Pic);
		Agent_.contact_getRequestDataMode(MUI_.IsDataModeMUIBut.Req);
		Agent_.slot_setDataMode(MUI_.IsDataModeMUIBut.Signal);

		Agent_.slot_setCorrection(MUI_.CorrectionDataBut.Signal);
		Agent_.contact_getCorrectionFlag(MUI_.CorrectionDataBut.Req);
		Agent_.contact_getCorrectionValue(MUI_.inquiry_getCorrectionValue);
	}
#endif

#if defined(HMR_DXCOMPASSMUI_INC) && defined(HMR_COMPASS_INC)
	void connect(dxosCompassMUI& MUI_,cCompassMsgAgent& Agent_){
		//Agent_.contact_getCompassPositon(MUI_.inquiry_getCompassPossition);
		//Agent_.contact_getCompassPolar(MUI_.inquiry_getCompassPolar);
		//Agent_.contact_getTime(MUI_.inquiry_getTime);

		MUI_.slot_setCompassData_and_time(Agent_.signal_inform_compassData_and_time);

		Agent_.contact_getPicDataMode(MUI_.IsDataModeMUIBut.Pic);
		Agent_.contact_getRequestDataMode(MUI_.IsDataModeMUIBut.Req);
		Agent_.slot_setDataMode(MUI_.IsDataModeMUIBut.Signal);

		Agent_.contact_getCorrectionMode(MUI_.inquiry_isCorrectionMode);
		Agent_.slot_setCorrectionMode(MUI_.signal_setCorrectionMode);
		Agent_.slot_addCorrection(MUI_.signal_addCorrection);
		Agent_.slot_setCorrectionFromFile(MUI_.signal_setCorrectionFromFile);
		Agent_.contact_getCorrectionNum(MUI_.inquiry_getCorrectionNum);
		Agent_.contact_getCompassCorrectionData(MUI_.inquiry_getCorrectionValue);
	}
#endif

#if defined(HMR_DXGYROMUI_INC) && defined(HMR_GYRO_INC)
	void connect(dxosGyroMUI& MUI_,cGyroMsgAgent& Agent_){
		//Agent_.contact_getGyroData(MUI_.inquiry_getGyroData);
		//Agent_.contact_getTime(MUI_.inquiry_getTime);
		MUI_.slot_getData(Agent_.signal_newdata);

		Agent_.contact_getPicDataMode(MUI_.IsDataModeMUIBut.Pic);
		Agent_.contact_getRequestDataMode(MUI_.IsDataModeMUIBut.Req);
		Agent_.slot_setDataMode(MUI_.IsDataModeMUIBut.Signal);

		Agent_.slot_setCorrectionFlag(MUI_.CorrectionDataBut.Signal);
		Agent_.contact_getCorrectionFlag(MUI_.CorrectionDataBut.Req);
		Agent_.contact_getGyroCorrectionData(MUI_.inquiry_getCorrectionValue);
	}
#endif


#if defined(HMR_DXGPSMUI_INC) && defined(HMR_GPS_INC)
	void connect(dxosGPSMUI& MUI_,cGPSMsgAgent& Agent_){
		//Agent_.contact_getGPSData(MUI_.inquiry_getGPSData);
//		Agent_.contact_getTime(MUI_.inquiry_getTime);
		MUI_.slot_getData(Agent_.signal_newdata);

		Agent_.contact_getPicDataMode(MUI_.IsDataModeMUIBut.Pic);
		Agent_.contact_getRequestDataMode(MUI_.IsDataModeMUIBut.Req);
		Agent_.slot_setDataMode(MUI_.IsDataModeMUIBut.Signal);

		Agent_.contact_getPicPower(MUI_.IsPowerMUIBut.Pic);
		Agent_.contact_getRequestPower(MUI_.IsPowerMUIBut.Req);
		Agent_.slot_setPower(MUI_.IsPowerMUIBut.Signal);

		Agent_.slot_setSwapState(MUI_.signal_changeGPSCh);
		Agent_.contact_getPicSwapState(MUI_.inq_getPic_GPSCh);
		Agent_.contact_getRequestSwapState(MUI_.inq_getRequest_GPSCh);
		//Agent_.contact_getGPSCh(MUI_.inq_getGPSCh);
	}
#endif

#if defined(HMR_DXINFRAREDMUI_INC) && defined(HMR_INFRARED_INC)
	void connect(dxosInfraRedMUI& MUI_,cInfraRedMsgAgent& Agent_){
		//Agent_.contact_getTemperature(MUI_.inquiry_getTemperature);
		//Agent_.contact_getTime(MUI_.inquiry_getTime);
		MUI_.slot_getData(Agent_.signal_newData);

		Agent_.contact_getPicDataMode(MUI_.IsDataModeMUIBut.Pic);
		Agent_.contact_getRequestDataMode(MUI_.IsDataModeMUIBut.Req);
		Agent_.slot_setDataMode(MUI_.IsDataModeMUIBut.Signal);
	}
#endif

#if defined(HMR_DXHUMIDMUI_INC) && defined(HMR_HUMID_INC)
	void connect(dxosHumidMUI& MUI_,cHumidMsgAgent& Agent_){
		//Agent_.contact_getValue(MUI_.inquiry_getValue);
		//Agent_.contact_getTime(MUI_.inquiry_getTime);
		MUI_.slot_getData(Agent_.signal_newData);

		Agent_.contact_getPicDataMode(MUI_.IsDataModeMUIBut.Pic);
		Agent_.contact_getRequestDataMode(MUI_.IsDataModeMUIBut.Req);
		Agent_.slot_setDataMode(MUI_.IsDataModeMUIBut.Signal);
	}
#endif

#if defined(HMR_DXH2SMUI_INC) && defined(HMR_H2S_INC)
	void connect(dxosH2SMUI& MUI_,cH2SMsgAgent& Agent_){
		//Agent_.contact_getValue(MUI_.inquiry_getValue);
		//Agent_.contact_getTime(MUI_.inquiry_getTime);
		MUI_.slot_getData(Agent_.signal_newData);

		Agent_.contact_getPicDataMode(MUI_.IsDataModeMUIBut.Pic);
		Agent_.contact_getRequestDataMode(MUI_.IsDataModeMUIBut.Req);
		Agent_.slot_setDataMode(MUI_.IsDataModeMUIBut.Signal);
	}
#endif

#if defined(HMR_DXBATTERYMUI_INC) && defined(HMR_BATTERY_INC)
	template<unsigned int Num_>
	void connect(dxosBatteryMUI<Num_>& MUI_,cBatteryMsgAgent<Num_>& Agent_){
		for(unsigned int Cnt=0;Cnt<Num_;++Cnt){
			Agent_.contact_getBatteryData(Cnt,MUI_.inquiry_getBatteryData[Cnt]);
		}
		Agent_.contact_getTime(MUI_.inquiry_getTime);

		Agent_.contact_getPicDataMode(MUI_.IsDataModeMUIBut.Pic);
		Agent_.contact_getRequestDataMode(MUI_.IsDataModeMUIBut.Req);
		Agent_.slot_setDataMode(MUI_.IsDataModeMUIBut.Signal);
	}
#endif

#if defined(HMR_DXMOTORMUI_INC) && defined(HMR_MOTOR_INC)
	void connect(dxosMotorMUI& MUI_,cMotorMsgAgent& Agent_){
		Agent_.slot_setStickL(MUI_.Signal_MotorStickL);
		Agent_.contact_StickL(MUI_.inquiry_getMotorStickL);
		Agent_.contact_BrakeL(MUI_.inquiry_getMotorBrakeL);
		Agent_.slot_setStickR(MUI_.Signal_MotorStickR);
		Agent_.contact_StickR(MUI_.inquiry_getMotorStickR);
		Agent_.contact_BrakeR(MUI_.inquiry_getMotorBrakeR);

		Agent_.slot_setBackMode(MUI_.Signal_BackMode);
		Agent_.contact_BackMode(MUI_.inquiry_getBackMode);
	}
#endif
#if defined(HMR_DXNAVIGATOR_INC) && defined(HMR_ACCELE_INC) && defined(HMR_COMPASS_DATA_INC) && defined(HMR_GYROLOGGER_INC)
	void connect(dxosNavigator<hmr::cGyroLogger::iterator>& Navigator_,cAcceleMsgAgent& Accele_,cCompass& Compass_,cGyroLogger& GyroLogger_){
		Accele_.contact_getAccelePolar(Navigator_.inquiry_accele);
		Compass_.contact_getPolar(Navigator_.inquiry_compass);
		GyroLogger_.contact_getLogs(Navigator_.range_inquiry_gyro);
	}
	void connect(dxosNavigator<hmr::cGyroLogger::iterator>& Navigator_,cAcceleLogger& AcceleLogger_,cCompass& Compass_,cGyroLogger& GyroLogger_){
		AcceleLogger_.contact_getAcceleMeanPolar(Navigator_.inquiry_accele);
		Compass_.contact_getPolar(Navigator_.inquiry_compass);
		GyroLogger_.contact_getLogs(Navigator_.range_inquiry_gyro);
	}
#endif
#if defined(HMR_DXNAVIGATOR_INC) && defined(HMR_ACCELE_INC) && defined(HMR_COMPASS_DATA_INC) && defined(HMR_GYROLOGGER_INC) && defined(HMR_GYROCOMPASS_INC)
	void connect(dxosNavigator<hmr::cGyroLogger::iterator>& Navigator_,cAcceleLogger& AcceleLogger_,cCompass& Compass_,cGyroLogger& GyroLogger_,cGyroCompass& GyroCompass_){
		AcceleLogger_.contact_getAcceleMeanPolar(Navigator_.inquiry_accele);
		Compass_.contact_getPolar(Navigator_.inquiry_compass);
		GyroLogger_.contact_getLogs(Navigator_.range_inquiry_gyro);
		GyroCompass_.inquiry_getAngle(Navigator_.inquiry_gyrocompass);
		GyroCompass_.slot_clearAngle(Navigator_.signal_clearGyroCompassAngle);
	}
#endif
#if defined(HMR_DXINFORMATIRON_INC) && defined(HMR_GPSKASHMIR_INC) && defined(HMR_BATTERY_INC)
	void connect(dxosInformation& Information,cGPSKashmir& GPSKashmir,cBatteryMsgAgent<3>& Battery){
		GPSKashmir.slot_connect(Information.GPSKashmirBut.signal_connect);
		GPSKashmir.slot_disconnect(Information.GPSKashmirBut.signal_disconnect);
		GPSKashmir.contact_is_connect(Information.GPSKashmirBut.inquiry_is_connect);
		Information.Battery0.slot_newdata(Battery.signal_newdata[0]);
		Information.Battery1.slot_newdata(Battery.signal_newdata[1]);
		Information.Battery2.slot_newdata(Battery.signal_newdata[2]);
	}
#endif

#if defined(HMR_DXGPSMAP_INC) && defined(HMR_GPS_INC) && defined(HMR_COMPASS_DATA_INC)
	void connect(dxosGPSMap& GPSMap, cGPSMsgAgent& Agent, cCompass& Compass_){
		GPSMap.slot_addNewData(Agent.signal_newdata);
		Compass_.contact_getYawAngle(GPSMap.inquiry_getMagneticAngle);
	}
#endif


#if defined(HMR_DXAUTOLOG_INC) && defined(HMR_AUTOLOG_AUTOLOGGER_INC) && defined(HMR_AUTOLOG_THERMOAUTOLOGGERCLIENT_INC) && defined(HMR_AUTOLOG_BATTERYAUTOLOGGERCLIENT_INC) && defined(HMR_AUTOLOG_CO2AUTOLOGGERCLIENT_INC) && defined(HMR_AUTOLOG_SPRITEAUTOLOGGERCLIENT_INC)
	void connect(dxosAutolog_480_360& DxAutolog, autolog::cAutoLogger& AutoLogger, autolog::cThermoAutoLoggerClient& Thermo, autolog::cBatteryAutoLoggerClient<3>& Battery, autolog::cCO2AutoLoggerClient& CO2, autolog::cSpriteAutoLoggerClient& Sprite){
		DxAutolog.signal_setStart.connect([&](bool val)->void {
			if(val)AutoLogger.start(); 
			else AutoLogger.stop(); 
		});
		DxAutolog.inquiry_isStart.connect([&]()->bool {return AutoLogger.is_start(); });
		DxAutolog.inquiry_isStartAutolog.connect([&]()->bool {return AutoLogger.is_start_autolog(); });

		DxAutolog.signal_setObserveStartPoint.connect([&](unsigned int val)->void {AutoLogger.setObserveStartPoint(val); });
		DxAutolog.inquiry_getObserveStartPoint.connect([&]()->unsigned int {return AutoLogger.getObserveStartPoint(); });

		DxAutolog.signal_setObserveInteraval.connect([&](unsigned int val)->void {AutoLogger.setObserveInterval(val); });
		DxAutolog.inquiry_getObserveInterval.connect([&]()->unsigned int {return AutoLogger.getObserveInterval(); });

		DxAutolog.signal_setPreObserveLength.connect([&](unsigned int val)->void {AutoLogger.setPreObserveLength(val); });
		DxAutolog.inquiry_getPreObserveLength.connect([&]()->unsigned int {return AutoLogger.getPreObserveLength(); });

		DxAutolog.inquiry_getStatusStr.connect([&]()->std::string {return AutoLogger.getStatusStr(); });
		DxAutolog.inquiry_getNextAwake.connect([&]()->hmr::clock::time_point {return AutoLogger.getNextAwake(); });
		DxAutolog.inquiry_getNextObserve.connect([&]()->hmr::clock::time_point {return AutoLogger.getNextObserve(); });

		DxAutolog.signal_startThermo.connect([&](bool val)->void {val?Thermo.activate():Thermo.inactivate(); });
		DxAutolog.inquiry_is_startThermo.connect([&]()->bool {return Thermo.is_active(); });

		DxAutolog.signal_startBattery.connect([&](bool val)->void {val?Battery.activate():Battery.inactivate(); });
		DxAutolog.inquiry_is_startBattery.connect([&]()->bool {return Battery.is_active(); });

		DxAutolog.signal_startCO2.connect([&](bool val)->void {val?CO2.activate():CO2.inactivate(); });
		DxAutolog.inquiry_is_startCO2.connect([&]()->bool {return CO2.is_active(); });

		DxAutolog.signal_startSprite.connect([&](bool val)->void {val?Sprite.activate():Sprite.inactivate(); });
		DxAutolog.inquiry_is_startSprite.connect([&]()->bool {return Sprite.is_active(); });
	}
#endif



}
#
#endif
#endif
