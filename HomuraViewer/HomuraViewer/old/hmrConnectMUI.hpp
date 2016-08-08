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
	}
}
#
#endif
#endif