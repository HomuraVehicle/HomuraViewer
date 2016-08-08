#ifndef HMR_VIEWER_DEVICEMANAGE_INC
#define HMR_VIEWER_DEVICEMANAGE_INC 100
#
#include "DeviceManage/DeviceManageMsgAgent.hpp"
#include "DeviceManage/DxDeviceManageSUI.hpp"
namespace hmr{
	namespace viewer{
		struct cDeviceManage{
			devmng::cMsgAgent MsgAgent;
			devmng::dxosSUI SUI;
		public:
			cDeviceManage(){
				MsgAgent.slot_setNormalMode(SUI.signal_setNormalMode);
				//MsgAgent.slot_setTurboMode(SUI.signal_setTurboMode);
				MsgAgent.slot_setSleepMode(SUI.signal_setSleepMode);
				MsgAgent.slot_exeSleepMode(SUI.signal_exeSleepMode);
				MsgAgent.slot_setRoamMode(SUI.signal_setRoamMode);
				MsgAgent.slot_exeRoamMode(SUI.signal_exeRoamMode);
				MsgAgent.slot_setKillMode(SUI.signal_setKillMode);
				MsgAgent.slot_exeKillMode(SUI.signal_exeKillMode);
				MsgAgent.slot_setInfo(SUI.signal_setGettingInfo);
				MsgAgent.slot_setClockMode(SUI.signal_setClockMode);

				MsgAgent.contact_getDevMODE(SUI.devMODEInq);
				MsgAgent.contact_getClockMODE(SUI.clockMODEInq);
				MsgAgent.contact_getSendingKillCommand(SUI.SendingKillCommandInq);
				MsgAgent.contact_getSendingNormalCommand(SUI.SendingNormalCommandInq);
				MsgAgent.contact_getUseMP(SUI.UseMPInq);
				MsgAgent.contact_getSendingRoamCommand(SUI.SendingRoamCommandInq);
				MsgAgent.contact_getSendingSleepCommand(SUI.SendingSleepCommandInq);
				MsgAgent.contact_getSendingClockCommand(SUI.SendingClockCommandInq);
				MsgAgent.contact_getSendingInfoCommand(SUI.SendingInfoCommandInq);
				//MsgAgent.contact_getSendingTurboCommand(SUI.SendingTurboCommandInq);
				MsgAgent.contact_IsReadyKillExe(SUI.IsReadyKillInq);
				MsgAgent.contact_IsReadyRoamExe(SUI.IsReadyRoamInq);
				MsgAgent.contact_IsReadySleepExe(SUI.IsReadySleepInq);
			}
		};
	}
}
#
#endif
