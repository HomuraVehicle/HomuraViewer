#ifndef HMR_VIEWER_CONTROL_INC
#define HMR_VIEWER_CONTROL_INC 100
#
#include"DxModule/Keyboard.hpp"
#include"DxModule/Pad.hpp"

namespace hmr{
	namespace viewer{
		struct cController{
		private:
			static constexpr PAD::PADNAME SensorDataModeBut = PAD::But4;
			static constexpr PAD::PADNAME CockpitDataModeBut = PAD::But3;
			static constexpr PAD::PADNAME AutoTakePicModeBut = PAD::But2;
			static constexpr PAD::PADNAME LargeTakePicBut = PAD::But1;
			static constexpr PAD::PADNAME PAD_SHIFT = PAD::But7;
		public:
			dxmodule::cDxKeyboard Keyboard;
			dxmodule::cDxPad1 Pad;
		public:
			void operator()(void){
				Keyboard.work();
				Pad.work();
			}
		public:
			void connect_Pad(thermo::cMsgAgent& Agent_){
				Agent_.DataMode.slot_setReqSwap(Pad.signal(dxmodule::is_pad1_pushed(SensorDataModeBut)));
			}
			void connect_Pad(cSpriteMsgAgent& Agent_){
				Agent_.slot_setTakePictureLarge(Pad.signal(dxmodule::is_pad1_pushed(LargeTakePicBut)));

				Agent_.slot_setAutoTakePicture(Pad.signal(dxmodule::is_pad1_pushed(AutoTakePicModeBut)));
			}
			void connect_Pad(cCO2MsgAgent& Agent_){
				Agent_.slot_setDataMode(Pad.signal(dxmodule::is_pad1_pushed(SensorDataModeBut)));
				Agent_.slot_setPumpPW(Pad.signal(dxmodule::is_pad1_pushed(SensorDataModeBut)));
				Agent_.slot_setSensorPW(Pad.signal(dxmodule::is_pad1_pushed(SensorDataModeBut)));
			}
			void connect_Pad(accele::cMsgAgent& Agent_){
				Agent_.DataMode.slot_setReqSwap(Pad.signal(dxmodule::is_pad1_pushed(CockpitDataModeBut)));
				//		Agent_.slot_setCorrection(Pad_.signal(is_pad1_pushed(PAD::But1)));
			}
			void connect_Pad(compass::cMsgAgent& Agent_){
				Agent_.DataMode.slot_setReqSwap(Pad.signal(dxmodule::is_pad1_pushed(CockpitDataModeBut)));
				//		Agent_.slot_setCorrectionMode(Pad_.signal(is_pad1_pushed(PAD::But1)));
			}
			void connect_Pad(gyro::cMsgAgent& Agent_){
				Agent_.DataMode.slot_setReqSwap(Pad.signal(dxmodule::is_pad1_pushed(CockpitDataModeBut)));
				//		Agent_.slot_setCorrectionFlag(Pad_.signal(is_pad1_pushed(PAD::But1)));
			}
			void connect_Pad(gps::cMsgAgent& Agent_){
				Agent_.DataMode.slot_setReqSwap(Pad.signal(dxmodule::is_pad1_pushed(CockpitDataModeBut)));
			}
			template<unsigned int Num_>
			void connect_Pad(battery::cMsgAgent<Num_>& Agent_){
				static boost::signals2::signal<void(bool)> Signal;
				Pad.signal(dxmodule::is_pad1_pushed(SensorDataModeBut)).connect([&](){Signal(true); });
				Agent_.DataMode.slot_setReq(Signal);
			}
			void connect_Pad(motor::cMsgAgent& Agent_){
				Agent_.slot_setStickL(Pad.signal_LStick);
				Agent_.slot_setBrakeLON(Pad.signal(dxmodule::is_pad1_pushed(PAD::But11)));
				Agent_.slot_setBrakeLOFF(Pad.signal(dxmodule::is_pad1_pulled(PAD::But11)));

				Agent_.slot_setStickR(Pad.signal_RStick);
				Agent_.slot_setBrakeRON(Pad.signal(dxmodule::is_pad1_pushed(PAD::But12)));
				Agent_.slot_setBrakeROFF(Pad.signal(dxmodule::is_pad1_pulled(PAD::But12)));

				Agent_.slot_setBackModeON(Pad.signal(dxmodule::is_pad1_pushed(PAD::But9)));
				Agent_.slot_setBackModeOFF(Pad.signal(dxmodule::is_pad1_pushed(PAD::But10)));
			}

			#if defined(HMR_H2S_INC)
			void connect_Pad(cH2SMsgAgent& Agent_){
				Agent_.slot_setDataMode(Pad.signal(dxmodule::is_pad1_pushed(SensorDataModeBut)));
			}
			#endif
			#if defined(HMR_INFRARED_INC)
			void connect_Pad(cInfraRedMsgAgent& Agent_){
				Agent_.slot_setDataMode(Pad.signal(dxmodule::is_pad1_pushed(SensorDataModeBut)));
			}
			#endif
			#if defined(HMR_SHT75_INC)
			void connect_Pad(cSHT75MsgAgent& SHT75MA_){
				SHT75MA_.slot_setDataMode(Pad.signal(dxmodule::is_pad1_pushed(SensorDataModeBut)));

			}
			#endif
		public:
			void connect_Keyboard(thermo::cMsgAgent& Thermo_){
				Thermo_.DataMode.slot_setReqSwap(Keyboard.signal(dxmodule::is_key_pushed(KEY::T)));
			}
			void connect_Keyboard(cSpriteMsgAgent& Agent_){
				Agent_.slot_setTakePicture(Keyboard.signal(dxmodule::is_key_pushed(KEY::S)));
				Agent_.slot_setCommandReset(Keyboard.signal(dxmodule::is_key_pushed(KEY::C)));
				Agent_.slot_setPowerReset(Keyboard.signal(dxmodule::is_key_pushed(KEY::P)));
			}
			void connect_Keyboard(cCO2MsgAgent& Agent_){
				Agent_.slot_setDataMode(Keyboard.signal(dxmodule::is_key_pushed(KEY::C)));
				Agent_.slot_setPumpPW(Keyboard.signal(dxmodule::is_key_pushed(KEY::C)));
				Agent_.slot_setSensorPW(Keyboard.signal(dxmodule::is_key_pushed(KEY::C)));
			}
			void connect_Keyboard(accele::cMsgAgent& Agent_){
				Agent_.DataMode.slot_setReqSwap(Keyboard.signal(dxmodule::is_key_pushed(KEY::A)));
				Agent_.slot_setCorrection(Keyboard.signal(dxmodule::is_key_pushed(KEY::A)));
			}
			void connect_Keyboard(compass::cMsgAgent& Agent_){
				Agent_.DataMode.slot_setReqSwap(Keyboard.signal(dxmodule::is_key_pushed(KEY::C)));
				Agent_.slot_setCorrectionMode(Keyboard.signal(dxmodule::is_key_pushed(KEY::A)));
			}
			void connect_Keyboard(gyro::cMsgAgent& Agent_){
				Agent_.DataMode.slot_setReqSwap(Keyboard.signal(dxmodule::is_key_pushed(KEY::G)));
				Agent_.slot_setCorrectionFlag(Keyboard.signal(dxmodule::is_key_pushed(KEY::G)));
			}
			void connect_Keyboard(gps::cMsgAgent& Agent_){
				Agent_.DataMode.slot_setReqSwap(Keyboard.signal(dxmodule::is_key_pushed(KEY::G)));
			}
			template<unsigned int Num_>
			void connect_Keyboard(battery::cMsgAgent<Num_>& Agent_){
				Agent_.slot_setDataMode(Keyboard.signal(is_key_pushed(KEY::B)));
			}

			#if defined(HMR_INFRARED_INC)
			void connect_Keyboard(cInfraRedMsgAgent& Agent_){
				Agent_.slot_setDataMode(Keyboard.signal(dxmodule::is_key_pushed(KEY::I)));
			}
			#endif

			#if defined(HMR_H2S_INC)
			void connect_Keyboard(cH2SMsgAgent& Agent_){
				Agent_.slot_setDataMode(Keyboard.signal(dxmodule::is_key_pushed(KEY::H)));
			}
			#endif
		};
	}
}
#
#endif
