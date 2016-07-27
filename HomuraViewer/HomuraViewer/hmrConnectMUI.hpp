#ifdef HMR_MAIN_INC_END
#ifndef HMR_CONNECTMUI_INC
#define HMR_CONNECTMUI_INC 100
#
namespace hmr{
	namespace viewer{
	#if defined(HMR_DXTHERMOMUI_INC) && defined(HMR_THERMO_INC)
		void connect(dxosThermoMUI& ThermoMUI_, cThermoMsgAgent& Thermo_){
			//Thermo_.contact_getTemperature(ThermoMUI_.inquiry_getThermo);
			//Thermo_.contact_getTime(ThermoMUI_.inquiry_getTime);
			ThermoMUI_.slot_getRawData(Thermo_.signal_newRawData);
			ThermoMUI_.slot_getRawData(Thermo_.signal_newLogRawData);

			Thermo_.contact_getPicDataMode(ThermoMUI_.IsDataModeMUIBut.Pic);
			Thermo_.contact_getRequestDataMode(ThermoMUI_.IsDataModeMUIBut.Req);
			Thermo_.slot_setDataMode(ThermoMUI_.IsDataModeMUIBut.Signal);

			Thermo_.contact_getPicLogMode(ThermoMUI_.IsLogModeMUIBut.Pic);
			Thermo_.contact_getRequestLogMode(ThermoMUI_.IsLogModeMUIBut.Req);
			Thermo_.slot_setLogMode(ThermoMUI_.IsLogModeMUIBut.Signal);
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

			Agent_.contact_getPicLogMode(MUI_.LogModeMUIBut.Pic);
			Agent_.contact_getRequestLogMode(MUI_.LogModeMUIBut.Req);
			Agent_.slot_setLogMode(MUI_.LogModeMUIBut.Signal);
		}
	#endif

	#if defined(HMR_DXCO2MUI_INC) && defined(HMR_CO2_INC)
		void connect(dxosCO2MUI& MUI_, cCO2MsgAgent& Agent_){
			//Agent_.contact_getValue(MUI_.inquiry_getValue);
			//Agent_.contact_getTime(MUI_.inquiry_getTime);
			MUI_.slot_getRawData(Agent_.signal_newRawData);
			MUI_.slot_getRawData(Agent_.signal_newLogRawData);

			Agent_.contact_getPicDataMode(MUI_.IsDataModeMUIBut.Pic);
			Agent_.contact_getRequestDataMode(MUI_.IsDataModeMUIBut.Req);
			Agent_.slot_setDataMode(MUI_.IsDataModeMUIBut.Signal);

			Agent_.contact_getPicPumpPW(MUI_.PumpPWMUIBut.Pic);
			Agent_.contact_getRequestPumpPW(MUI_.PumpPWMUIBut.Req);
			Agent_.slot_setPumpPW(MUI_.PumpPWMUIBut.Signal);

			Agent_.contact_getPicSensorPW(MUI_.SensorPWMUIBut.Pic);
			Agent_.contact_getRequestSensorPW(MUI_.SensorPWMUIBut.Req);
			Agent_.slot_setSensorPW(MUI_.SensorPWMUIBut.Signal);

			Agent_.contact_getPicLogMode(MUI_.IsLogModeMUIBut.Pic);
			Agent_.contact_getRequestLogMode(MUI_.IsLogModeMUIBut.Req);
			Agent_.slot_setLogMode(MUI_.IsLogModeMUIBut.Signal);
		}
	#endif

	#if defined(HMR_DXACCELEMUI_INC) && defined(HMR_ACCELE_INC)
		void connect(dxosAcceleMUI& MUI_, cAcceleMsgAgent& Agent_){
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
		void connect(dxosCompassMUI& MUI_, cCompassMsgAgent& Agent_){
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
		void connect(dxosGyroMUI& MUI_, cGyroMsgAgent& Agent_){
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
		void connect(dxosGPSMUI& MUI_, cGPSMsgAgent& Agent_){
			//Agent_.contact_getGPSData(MUI_.inquiry_getGPSData);
			//		Agent_.contact_getTime(MUI_.inquiry_getTime);
			MUI_.slot_getData(Agent_.signal_newdata);

			Agent_.contact_getPicDataMode(MUI_.IsDataModeMUIBut.Pic);
			Agent_.contact_getRequestDataMode(MUI_.IsDataModeMUIBut.Req);
			Agent_.slot_setDataMode(MUI_.IsDataModeMUIBut.Signal);

			Agent_.slot_setSwapState(MUI_.signal_changeGPSCh);
			Agent_.contact_getPicSwapState(MUI_.inq_getPic_GPSCh);
			Agent_.contact_getRequestSwapState(MUI_.inq_getRequest_GPSCh);
			//Agent_.contact_getGPSCh(MUI_.inq_getGPSCh);
		}
	#endif

	#if defined(HMR_DXINFRAREDMUI_INC) && defined(HMR_INFRARED_INC)
		void connect(dxosInfraRedMUI& MUI_, cInfraRedMsgAgent& Agent_){
			//Agent_.contact_getTemperature(MUI_.inquiry_getTemperature);
			//Agent_.contact_getTime(MUI_.inquiry_getTime);
			MUI_.slot_getData(Agent_.signal_newData);

			Agent_.contact_getPicDataMode(MUI_.IsDataModeMUIBut.Pic);
			Agent_.contact_getRequestDataMode(MUI_.IsDataModeMUIBut.Req);
			Agent_.slot_setDataMode(MUI_.IsDataModeMUIBut.Signal);
		}
	#endif

	#if defined(HMR_DXHUMIDMUI_INC) && defined(HMR_HUMID_INC)
		void connect(dxosHumidMUI& MUI_, cHumidMsgAgent& Agent_){
			//Agent_.contact_getValue(MUI_.inquiry_getValue);
			//Agent_.contact_getTime(MUI_.inquiry_getTime);
			MUI_.slot_getData(Agent_.signal_newData);

			Agent_.contact_getPicDataMode(MUI_.IsDataModeMUIBut.Pic);
			Agent_.contact_getRequestDataMode(MUI_.IsDataModeMUIBut.Req);
			Agent_.slot_setDataMode(MUI_.IsDataModeMUIBut.Signal);
		}
	#endif

	#if defined(HMR_DXH2SMUI_INC) && defined(HMR_H2S_INC)
		void connect(dxosH2SMUI& MUI_, cH2SMsgAgent& Agent_){
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
		void connect(dxosBatteryMUI<Num_>& MUI_, battery::cMsgAgent<Num_>& Agent_){
			for(unsigned int Cnt = 0; Cnt < Num_; ++Cnt){
				Agent_.contact_getBatteryData(Cnt, MUI_.inquiry_getBatteryData[Cnt]);
			}
			Agent_.contact_getTime(MUI_.inquiry_getTime);

			Agent_.contact_getPicDataMode(MUI_.IsDataModeMUIBut.Pic);
			Agent_.contact_getRequestDataMode(MUI_.IsDataModeMUIBut.Req);
			Agent_.slot_setDataMode(MUI_.IsDataModeMUIBut.Signal);
		}
	#endif

	#if defined(HMR_DXMOTORMUI_INC) && defined(HMR_MOTOR_INC)
		void connect(dxosMotorMUI& MUI_, cMotorMsgAgent& Agent_){
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
	}
}
#
#endif
#endif