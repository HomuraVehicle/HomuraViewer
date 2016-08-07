#ifdef HMR_MAIN_INC_END
#ifndef HMR_CONNECTSUI_INC
#define HMR_CONNECTSUI_INC 100
#
namespace hmr{
	namespace viewer{
		// SUI 関連のコネクト
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