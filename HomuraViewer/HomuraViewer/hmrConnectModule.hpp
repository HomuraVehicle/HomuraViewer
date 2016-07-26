#ifdef HMR_MAIN_INC_END
#ifndef HMR_CONNECTMODULE_INC
#define HMR_CONNECTMODULE_INC 100
#

//#include "hmrDxGateSwitcher.hpp"
//#include "hmrGateSwitcher.hpp"

/*===hmrConnect===
接続補助関数群を定義する
*/
namespace hmr{
#if defined(HMR_GYROLOGGER_INC) && defined(HMR_GYRO_INC)
	void connect(cGyroLogger& GyroLog_,cGyroMsgAgent& Gyro_){
		GyroLog_.slot_addLog(Gyro_.signal_newdata);
	}
#endif

#if defined(HMR_GYROCOMPASS_INC) && defined(HMR_GYRO_INC)
	void connect(cGyroCompass& GyroCompass_,cGyroMsgAgent& Gyro_){
		GyroCompass_.slot_addAngle(Gyro_.signal_newdata);
	}
#endif

#if defined(HMR_GPSKASHMIR_INC) && defined(HMR_GPS_INC)
	void connect(cGPSKashmir& GPSKashmir_,cGPSMsgAgent& GPS_){
		GPSKashmir_.slot_sendGPGGA(GPS_.signal_newdata);
	}
#endif


// Data と　MAgent　のコネクト
#if defined(HMR_COMPASS_DATA_INC) && defined(HMR_COMPASS_INC)
	void connect(cCompass& CompassDat_,cCompassMsgAgent& Agent_){
		CompassDat_.slot_addCompassData(Agent_.signal_inform_compassData_and_time);
	}
#endif
}
#
#endif
#endif