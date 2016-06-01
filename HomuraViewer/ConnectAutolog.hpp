#ifdef HMR_MAIN_INC_END
#ifndef HMR_CONNECTAUTOLOG_INC
#define HMR_CONNECTAUTOLOG_INC 100
#

//#include "hmrDxGateSwitcher.hpp"
//#include "hmrGateSwitcher.hpp"

/*===hmrConnect===
ê⁄ë±ï‚èïä÷êîåQÇíËã`Ç∑ÇÈ
*/
namespace hmr{
	namespace autolog {
#if defined(HMR_AUTOLOG_AUTOLOGGER_INC) && defined(HMR_DEVICEMANAGE_INC) && defined(HMR_COM_INC) && defined(HMR_OPERATOR_INC) && defined(HMR_GATEHAB_INC)
		void connect(autolog::cAutoLogger& Autolog, cDevMngMsgAgent& DeviceManage, cCom& Com, cFHDxOperator& Operator, cGateSwitcher& GateSwitcher) {
			DeviceManage.slot_setSleepMode(Autolog.signal_setSleepMode);
			DeviceManage.contact_IsReadySleepExe(Autolog.inquiry_isReadySleep);
			DeviceManage.slot_exeSleepMode(Autolog.signal_exeSleepMode);
			DeviceManage.slot_setNormalMode(Autolog.signal_exeNormalMode);
			DeviceManage.contact_isNormal(Autolog.inquiry_isNormalMode);

			Com.contactLastRecvPacTime(Autolog.inquiry_getLastRecvTime);
			Com.slot_clearSendBuf(Autolog.signal_clearSendBuf);

			Operator.slot_setIsSendActiveFlag(Autolog.signal_setIsSendActive);

			GateSwitcher.slot_openFoma(Autolog.signal_openFoma);
			GateSwitcher.slot_closePort(Autolog.signal_closeFoma);
		}
#endif

#if defined(HMR_AUTOLOG_THERMOAUTOLOGGERCLIENT_INC) && defined(HMR_THERMO_INC)
		void connect(autolog::cThermoAutoLoggerClient& Autolog, cThermoMsgAgent& MsgAgent) {
			MsgAgent.slot_setDataMode(Autolog.signal_setDataMode);
			MsgAgent.contact_getPicDataMode(Autolog.inquiry_isDataMode);
			Autolog.slot_newData(MsgAgent.signal_newData);
		}
#endif

#if defined(HMR_AUTOLOG_BATTERYAUTOLOGGERCLIENT_INC) && defined(HMR_BATTERY_INC)
		void connect(autolog::cBatteryAutoLoggerClient<3>& Autolog, cBatteryMsgAgent<3>& MsgAgent) {
			MsgAgent.slot_setDataMode(Autolog.signal_setDataMode);
			MsgAgent.contact_getPicDataMode(Autolog.inquiry_isDataMode);
			Autolog.slot_newData(MsgAgent.signal_newdata[0],0);
			Autolog.slot_newData(MsgAgent.signal_newdata[1],1);
			Autolog.slot_newData(MsgAgent.signal_newdata[2],2);
		}
#endif

#if defined(HMR_AUTOLOG_CO2AUTOLOGGERCLIENT_INC) && defined(HMR_CO2_INC)
		void connect(autolog::cCO2AutoLoggerClient& Autolog, cCO2MsgAgent& MsgAgent) {
			MsgAgent.slot_setDataMode(Autolog.signal_setDataMode);
			MsgAgent.contact_getPicDataMode(Autolog.inquiry_isDataMode);
			MsgAgent.slot_setPumpPW(Autolog.signal_setPumpPw);
			MsgAgent.contact_getPicPumpPW(Autolog.inquiry_isPumpPw);
			MsgAgent.slot_setSensorPW(Autolog.signal_setSensorPw);
			MsgAgent.contact_getPicSensorPW(Autolog.inquiry_isSensorPw);

			Autolog.slot_newData(MsgAgent.signal_newData);
		}
#endif

#if defined(HMR_AUTOLOG_SPRITEAUTOLOGGERCLIENT_INC) && defined(HMR_SPRITE_INC)
		void connect(autolog::cSpriteAutoLoggerClient& Autolog, cSpriteMsgAgent& MsgAgent) {
			MsgAgent.slot_setInfoMode(Autolog.signal_setStatusMode);
			MsgAgent.contact_getPicInfoMode(Autolog.inquiry_isStatusMode);
			MsgAgent.contact_getIsErr(Autolog.inquiry_isError);

			MsgAgent.slot_setTakePicture(Autolog.signal_request_takePic);
			MsgAgent.contact_getDoTakePictureFlag(Autolog.inquiry_is_takePic);

			MsgAgent.slot_setAutoLight(Autolog.signal_requestAutoLight);
			MsgAgent.contact_getPicAutoLight(Autolog.inquiry_isAutoLight);
			
			MsgAgent.slot_setAutoPowerReset(Autolog.signal_setAutoResetMode);
			MsgAgent.contact_getPicAutoPowerReset(Autolog.inquiry_isAutoResetMode);

			Autolog.slot_newData(MsgAgent.signal_setPicture);
		}
#endif
	}
}
#
#endif
#endif
