#ifdef HMR_MAIN_INC_END
#ifndef HMR_CONNECTSUI_INC
#define HMR_CONNECTSUI_INC 100
#
namespace hmr{
	namespace viewer{
		// SUI 関連のコネクト

	#if defined(HMR_DXLOGGERMANAGESUI_INC) && defined(HMR_LOGGERMANAGE_INC)
		void connect(dxosLoggerMngSUI& logSUI_, cLoggerMngMsgAgent& agt_){
			agt_.slot_request_get_logDataSize(logSUI_.signal_request_get_logDataSize);
			agt_.slot_request_logData(logSUI_.signal_request_logData);
			agt_.slot_request_set_logMode(logSUI_.isLogModeSUIBut.Signal);

			agt_.contact_is_sending_get_logDataSize(logSUI_.is_sending_request_logDataSizeInq);
			agt_.contact_is_sending_request_logData(logSUI_.is_sending_request_logDataInq);
			agt_.contact_logDataSize(logSUI_.logDataSizeInq);
			agt_.contact_is_getting_logDataSet(logSUI_.is_getting_dataInq);

			agt_.contact_is_logMode(logSUI_.isLogModeSUIBut.Pic);
			agt_.contact_is_logMode_req(logSUI_.isLogModeSUIBut.Req);
		}
	#endif

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
		typedef std::pair<bool, system_clock_iologtype> io_iologtype;
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

	#if defined(HMR_DXFILE_SUI_INC) && defined(HMR_VIEWER_TIMEDIRECTORYFILE_INC)
		void connect(dxosFileSUI& fileSUI_, cTimeDirectoryFile& agt_){
			fileSUI_.slot_setIsActive(agt_.signal_isActivate);
			agt_.slot_activate(fileSUI_.signal_require_activate);
			agt_.slot_inactivate(fileSUI_.signal_require_inactivate);
		}
	#endif

	#if defined(HMR_DXFILE_SUI_INC) && defined(HMR_VIEWER_CONSTNAMEDIRECTORYFILE_INC)
		void connect(dxosFileSUI& fileSUI_, cConstNameDirectoryFile& agt_){
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

	#if defined(HMR_DXCHRONOSUI_INC) && defined(HMR_CHRONO_INC)
		void connect(dxosChronoSUI& devSUI_, cChrono& agt_){
			devSUI_.slot_getMachineDifTime(agt_.signal_getMachineDifTime);
			agt_.contact_can_correctMachineTime(devSUI_.inquiry_can_correctMachineTime);
			agt_.slot_correctMachineTime(devSUI_.signal_correctMachineTime);
			agt_.slot_requestMachineTime(devSUI_.signal_requestMachineTime);
		}
	#endif
	}
}
#
#endif
#endif