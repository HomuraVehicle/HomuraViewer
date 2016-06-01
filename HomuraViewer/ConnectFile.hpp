#ifdef HMR_MAIN_INC_END
#ifndef HMR_CONNECTFILE_INC
#define HMR_CONNECTFILE_INC 101
#

/*===hmrConnect===
接続補助関数群を定義する
*/
namespace hmr{
#if defined(HMR_ACCELEFILE_INC) && defined(HMR_ACCELE_INC)
	void connect(cAcceleFileAgent& fAgent_, cAcceleMsgAgent& mAgent_){
		fAgent_.slot_log_writeData(mAgent_.signal_newData);
	}
#endif

#if defined(HMR_BATTERYFILE_INC) && defined(HMR_BATTERY_INC)
	//template<unsigned int Num_>
	void connect(cBatteryFileAgent<3>& fAgent_, cBatteryMsgAgent<3>& mAgent_){
		fAgent_.slot_logData<0>( mAgent_.signal_newdata[0]);
		fAgent_.slot_logData<1>( mAgent_.signal_newdata[1]);
		fAgent_.slot_logData<2>( mAgent_.signal_newdata[2]);
		// slot_writeData を呼び出す必要があるけど、今は保留
		//  一定時間ごとにスロットを呼び出して書いていく必要がある
	}

#endif

#if defined(HMR_COMPASSFILE_INC) && defined(HMR_COMPASS_INC)
	void connect(cCompassFileAgent& fAgent_, cCompassMsgAgent& mAgent_){
		fAgent_.slot_log_writeData(mAgent_.signal_inform_compassData_and_time);
	}
#endif

#if defined(HMR_GPSFILE_INC) && defined(HMR_GPS_INC)
	void connect(cGPSFileAgent& fAgent_, cGPSMsgAgent& mAgent_){
		fAgent_.slot_log_writeData(mAgent_.signal_newdata);
	}
#endif

#if defined(HMR_GYROFILE_INC) && defined(HMR_GYRO_INC)
	void connect(cGyroFileAgent& fAgent_, cGyroMsgAgent& mAgent_){
		fAgent_.slot_log_writeData(mAgent_.signal_newdata);
	}
#endif

// センサー系列のコネクト関数
// 一括システム

// Sensors File Agents
#if defined(HMR_SENSORSFILE_INC) && defined(HMR_CO2_INC) && defined(HMR_H2S_INC) && defined(HMR_HUMID_INC) && defined(HMR_INFRARED_INC) && defined(HMR_THERMO_INC)
	void connect(cSensorsFileAgent& fAgent_, cCO2MsgAgent& mCO2Agent_, cH2SMsgAgent& mH2SAgent_, cHumidMsgAgent& mHumidAgent_, cInfraRedMsgAgent& mInfAgent_, cThermoMsgAgent& mThermoAgent_){
		fAgent_.slot_logInfThermoData(mInfAgent_.signal_newData);
		fAgent_.slot_logData<cSensorsFileAgent::CO2>(mCO2Agent_.signal_newData);
		fAgent_.slot_logData<cSensorsFileAgent::H2S>(mH2SAgent_.signal_newData);
		fAgent_.slot_logData<cSensorsFileAgent::HUMID>(mHumidAgent_.signal_newData);
		fAgent_.slot_logData<cSensorsFileAgent::THERMO>(mThermoAgent_.signal_newData);
	}
#endif


// Each sensor -------->
#if defined(HMR_CO2FILE_INC) && defined(HMR_CO2_INC)
	void connect(cCO2FileAgent& fAgent_, cCO2MsgAgent& mAgent_){
		fAgent_.slot_log_writeData(mAgent_.signal_newData);
	}
#endif

#if defined(HMR_H2SFILE_INC) && defined(HMR_H2S_INC)
	void connect(cH2SFileAgent& fAgent_, cH2SMsgAgent& mAgent_){
		fAgent_.slot_log_writeData(mAgent_.signal_newData);
	}
#endif

#if defined(HMR_HUMIDFILE_INC) && defined(HMR_HUMID_INC)
	void connect(cHumidFileAgent& fAgent_, cHumidMsgAgent& mAgent_){
		fAgent_.slot_log_writeData(mAgent_.signal_newData);
	}
#endif

#if defined(HMR_INFRAREDFILE_INC) && defined(HMR_INFRARED_INC)
	void connect(cInfraRedFileAgent& fAgent_, cInfraRedMsgAgent& mAgent_){
		fAgent_.slot_log_writeData(mAgent_.signal_newData);
	}
#endif

#if defined(HMR_THERMOFILE_INC) && defined(HMR_THERMO_INC)
	void connect(cThermoFileAgent& fAgent_, cThermoMsgAgent& mAgent_){
		fAgent_.slot_log_writeData(mAgent_.signal_newData);
	}
#endif

#if defined(HMR_SPRITEFILE_INC) && defined(HMR_SPRITE_INC)
	void connect(cSpriteFileAgent& fAgent_, cSpriteMsgAgent& mAgent_){
		fAgent_.slot_log_writeData(mAgent_.signal_setPicture);
	}



#endif


}
#
#endif
#endif