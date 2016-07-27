#ifdef HMR_MAIN_INC_END
#ifndef HMR_CONNECTKEYBOARD_INC
#define HMR_CONNECTKEYBOARD_INC 100
#

//#include "hmrDxGateSwitcher.hpp"
//#include "hmrGateSwitcher.hpp"

/*===hmrConnect===
ê⁄ë±ï‚èïä÷êîåQÇíËã`Ç∑ÇÈ
*/
namespace hmr{
	namespace viewer{
	#if defined(HMR_THERMO_INC)
		void connect_Keyboard(cThermoMsgAgent& Thermo_, cDxKeyboard& Keyboard_){
			Thermo_.slot_setDataMode(Keyboard_.signal(is_key_pushed(KEY::T)));
		}
	#endif

	#if defined(HMR_SPRITE_INC)
		void connect_Keyboard(cSpriteMsgAgent& Agent_, cDxKeyboard& Keyboard_){
			//		Agent_.slot_setInfoMode(MUI_.InfoModeMUIHead.Signal);

			//		Agent_.slot_setAutoTakePicture(MUI_.AutoTakePictureMUIBut.Signal);

			Agent_.slot_setTakePicture(Keyboard_.signal(is_key_pushed(KEY::S)));

			//		Agent_.slot_setPictureSize(MUI_.signal_setPictureSize);

			//		Agent_.slot_setAutoLight(MUI_.AutoLightMUIBut.Signal);

			//		Agent_.slot_setLight(MUI_.LightMUIBut.Signal);

			//		Agent_.slot_setAutoPowerReset(MUI_.AutoPowerResetMUIBut.Signal);

			Agent_.slot_setCommandReset(Keyboard_.signal(is_key_pushed(KEY::C)));

			Agent_.slot_setPowerReset(Keyboard_.signal(is_key_pushed(KEY::P)));

			//		Agent_.slot_setMiniPacketMode(MUI_.MiniPacketModeMUIBut.Signal);
		}
	#endif

	#if defined(HMR_CO2_INC)
		void connect_Keyboard(cCO2MsgAgent& Agent_, cDxKeyboard& Keyboard_){
			Agent_.slot_setDataMode(Keyboard_.signal(is_key_pushed(KEY::C)));
			Agent_.slot_setPumpPW(Keyboard_.signal(is_key_pushed(KEY::C)));
			Agent_.slot_setSensorPW(Keyboard_.signal(is_key_pushed(KEY::C)));
		}
	#endif

	#if defined(HMR_ACCELE_INC)
		void connect_Keyboard(cAcceleMsgAgent& Agent_, cDxKeyboard& Keyboard_){
			Agent_.slot_setDataMode(Keyboard_.signal(is_key_pushed(KEY::A)));
			Agent_.slot_setCorrection(Keyboard_.signal(is_key_pushed(KEY::A)));
		}
	#endif

	#if defined(HMR_COMPASS_INC)
		void connect_Keyboard(cCompassMsgAgent& Agent_, cDxKeyboard& Keyboard_){
			Agent_.slot_setDataMode(Keyboard_.signal(is_key_pushed(KEY::C)));
			Agent_.slot_setCorrectionMode(Keyboard_.signal(is_key_pushed(KEY::A)));
		}
	#endif

	#if defined(HMR_GYRO_INC)
		void connect_Keyboard(cGyroMsgAgent& Agent_, cDxKeyboard& Keyboard_){
			Agent_.slot_setDataMode(Keyboard_.signal(is_key_pushed(KEY::G)));
			Agent_.slot_setCorrectionFlag(Keyboard_.signal(is_key_pushed(KEY::G)));
		}
	#endif

	#if defined(HMR_GPS_INC)
		void connect_Keyboard(cGPSMsgAgent& Agent_, cDxKeyboard& Keyboard_){
			Agent_.slot_setDataMode(Keyboard_.signal(is_key_pushed(KEY::G)));
		}
	#endif

	#if defined(HMR_INFRARED_INC)
		void connect_Keyboard(cInfraRedMsgAgent& Agent_, cDxKeyboard& Keyboard_){
			Agent_.slot_setDataMode(Keyboard_.signal(is_key_pushed(KEY::I)));
		}
	#endif

	#if defined(HMR_BATTERY_INC)
		template<unsigned int Num_>
		void connect_Keyboard(battery::cMsgAgent<Num_>& Agent_, cDxKeyboard& Keyboard_){
			Agent_.slot_setDataMode(Keyboard_.signal(is_key_pushed(KEY::B)));
		}
	#endif

	#if defined(HMR_HUMID_INC)
		void connect_Keyboard(cHumidMsgAgent& Agent_, cDxKeyboard& Keyboard_){
			Agent_.slot_setDataMode(Keyboard_.signal(is_key_pushed(KEY::H)));
		}
	#endif

	#if defined(HMR_H2S_INC)
		void connect_Keyboard(cH2SMsgAgent& Agent_, cDxKeyboard& Keyboard_){
			Agent_.slot_setDataMode(Keyboard_.signal(is_key_pushed(KEY::H)));
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
