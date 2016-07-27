#ifdef HMR_MAIN_INC_END
#ifndef HMR_CONNECTPAD_INC
#define HMR_CONNECTPAD_INC 100
#

//#include "hmrDxGateSwitcher.hpp"
//#include "hmrGateSwitcher.hpp"

/*===hmrConnect===
ê⁄ë±ï‚èïä÷êîåQÇíËã`Ç∑ÇÈ
*/
namespace hmr{
	namespace viewer{
	#define SensorDataModeBut PAD::But4
	#define CockpitDataModeBut PAD::But3
	#define AutoTakePicModeBut PAD::But2
	#define LargeTakePicBut PAD::But1
	#define PAD_SHIFT PAD::But7

	#if defined(HMR_THERMO_INC)
		void connect_Pad(cThermoMsgAgent& Thermo_, cDxPad1& Pad_){
			Thermo_.slot_setDataMode(Pad_.signal(is_pad1_pushed(SensorDataModeBut)));

		}
	#endif
	#if defined(HMR_SHT75_INC)
		void connect_Pad(cSHT75MsgAgent& SHT75MA_, cDxPad1& Pad_){
			SHT75MA_.slot_setDataMode(Pad_.signal(is_pad1_pushed(SensorDataModeBut)));

		}
	#endif
	#if defined(HMR_SPRITE_INC)
		void connect_Pad(cSpriteMsgAgent& Agent_, cDxPad1& Pad_){
			Agent_.slot_setTakePictureLarge(Pad_.signal(is_pad1_pushed(LargeTakePicBut)));

			Agent_.slot_setAutoTakePicture(Pad_.signal(is_pad1_pushed(AutoTakePicModeBut)));
		}
	#endif

	#if defined(HMR_CO2_INC)
		void connect_Pad(cCO2MsgAgent& Agent_, cDxPad1& Pad_){
			Agent_.slot_setDataMode(Pad_.signal(is_pad1_pushed(SensorDataModeBut)));
			Agent_.slot_setPumpPW(Pad_.signal(is_pad1_pushed(SensorDataModeBut)));
			Agent_.slot_setSensorPW(Pad_.signal(is_pad1_pushed(SensorDataModeBut)));
		}
	#endif

	#if defined(HMR_ACCELE_INC)
		void connect_Pad(cAcceleMsgAgent& Agent_, cDxPad1& Pad_){
			Agent_.slot_setDataMode(Pad_.signal(is_pad1_pushed(CockpitDataModeBut)));
			//		Agent_.slot_setCorrection(Pad_.signal(is_pad1_pushed(PAD::But1)));
		}
	#endif

	#if defined(HMR_COMPASS_INC)
		void connect_Pad(cCompassMsgAgent& Agent_, cDxPad1& Pad_){
			Agent_.slot_setDataMode(Pad_.signal(is_pad1_pushed(CockpitDataModeBut)));
			//		Agent_.slot_setCorrectionMode(Pad_.signal(is_pad1_pushed(PAD::But1)));
		}
	#endif

	#if defined(HMR_GYRO_INC)
		void connect_Pad(cGyroMsgAgent& Agent_, cDxPad1& Pad_){
			Agent_.slot_setDataMode(Pad_.signal(is_pad1_pushed(CockpitDataModeBut)));
			//		Agent_.slot_setCorrectionFlag(Pad_.signal(is_pad1_pushed(PAD::But1)));
		}
	#endif

	#if defined(HMR_GPS_INC)
		void connect_Pad(cGPSMsgAgent& Agent_, cDxPad1& Pad_){
			Agent_.slot_setDataMode(Pad_.signal(is_pad1_pushed(CockpitDataModeBut)));
		}
	#endif

	#if defined(HMR_INFRARED_INC)
		void connect_Pad(cInfraRedMsgAgent& Agent_, cDxPad1& Pad_){
			Agent_.slot_setDataMode(Pad_.signal(is_pad1_pushed(SensorDataModeBut)));
		}
	#endif

	#if defined(HMR_BATTERY_INC)
		template<unsigned int Num_>
		void connect_Pad(battery::cMsgAgent<Num_>& Agent_, cDxPad1& Pad_){
			Agent_.slot_setDataMode(Pad_.signal(is_pad1_pushed(SensorDataModeBut)));
		}
	#endif

	#if defined(HMR_HUMID_INC)
		void connect_Pad(cHumidMsgAgent& Agent_, cDxPad1& Pad_){
			Agent_.slot_setDataMode(Pad_.signal(is_pad1_pushed(SensorDataModeBut)));
		}
	#endif

	#if defined(HMR_H2S_INC)
		void connect_Pad(cH2SMsgAgent& Agent_, cDxPad1& Pad_){
			Agent_.slot_setDataMode(Pad_.signal(is_pad1_pushed(SensorDataModeBut)));
		}
	#endif

	#if defined(HMR_MOTOR_INC)
		void connect_Pad(cMotorMsgAgent& Agent_, cDxPad1& Pad_){
			Agent_.slot_setStickL(Pad_.signal_LStick);
			Agent_.slot_setBrakeLON(Pad_.signal(is_pad1_pushed(PAD::But11)));
			Agent_.slot_setBrakeLOFF(Pad_.signal(is_pad1_pulled(PAD::But11)));

			Agent_.slot_setStickR(Pad_.signal_RStick);
			Agent_.slot_setBrakeRON(Pad_.signal(is_pad1_pushed(PAD::But12)));
			Agent_.slot_setBrakeROFF(Pad_.signal(is_pad1_pulled(PAD::But12)));

			Agent_.slot_setBackModeON(Pad_.signal(is_pad1_pushed(PAD::But9)));
			Agent_.slot_setBackModeOFF(Pad_.signal(is_pad1_pushed(PAD::But10)));
		}
	#endif
		/*
		#if defined(HMR_DXNAVIGATOR_INC) && defined(HMR_ACCELE_INC) && defined(HMR_COMPASS_INC) && defined(HMR_GYROLOGGER_INC)
			void connect(dxosNavigator<cGyroLogger::iterator>& Navigator_,cAcceleMsgAgent& Accele_,cCompassMsgAgent& Compass_,cGyroLogger& GyroLogger_){
				Accele_.contact_getAccelePolar(Navigator_.inquiry_accele);
				Compass_.contact_getCompassPolar(Navigator_.inquiry_compass);
				GyroLogger_.contact_getLogs(Navigator_.range_inquiry_gyro);
			}
		#endif
		*/
	}
}

#
#endif
#endif
