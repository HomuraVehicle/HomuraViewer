#ifndef HMR_VIEWER_CONTROLLER_INC
#define HMR_VIEWER_CONTROLLER_INC 100
#
#include"Controller/Keyboard.hpp"
#include"Controller/Pad.hpp"
#include"View.hpp"
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
			controller::cDxKeyboard Keyboard;
			controller::cDxPad1 Pad;
		public:
			void operator()(void){
				Keyboard.work();
				Pad.work();
			}
		public:
			void connect_Pad(thermo::cMsgAgent& Agent_){
				Agent_.DataMode.slot_setReqSwap(Pad.signal(controller::is_pad1_pushed(SensorDataModeBut)));
			}
			void connect_Pad(camera::cMsgAgent& Agent_){
				Agent_.slot_setTakePictureLarge(Pad.signal(controller::is_pad1_pushed(LargeTakePicBut)));
				Agent_.AutoTakePic.slot_setReqSwap(Pad.signal(controller::is_pad1_pushed(AutoTakePicModeBut)));
			}
			void connect_Pad(co2::cMsgAgent& Agent_){
				Agent_.DataMode.slot_setReqSwap(Pad.signal(controller::is_pad1_pushed(SensorDataModeBut)));
				Agent_.PumpPW.slot_setReqSwap(Pad.signal(controller::is_pad1_pushed(SensorDataModeBut)));
				Agent_.SensorPW.slot_setReqSwap(Pad.signal(controller::is_pad1_pushed(SensorDataModeBut)));
			}
			void connect_Pad(accele::cMsgAgent& Agent_){
				Agent_.DataMode.slot_setReqSwap(Pad.signal(controller::is_pad1_pushed(CockpitDataModeBut)));
				//		Agent_.slot_setCorrection(Pad_.signal(is_pad1_pushed(PAD::But1)));
			}
			void connect_Pad(compass::cMsgAgent& Agent_){
				Agent_.DataMode.slot_setReqSwap(Pad.signal(controller::is_pad1_pushed(CockpitDataModeBut)));
				//		Agent_.slot_setCorrectionMode(Pad_.signal(is_pad1_pushed(PAD::But1)));
			}
			void connect_Pad(gyro::cMsgAgent& Agent_){
				Agent_.DataMode.slot_setReqSwap(Pad.signal(controller::is_pad1_pushed(CockpitDataModeBut)));
				//		Agent_.slot_setCorrectionFlag(Pad_.signal(is_pad1_pushed(PAD::But1)));
			}
			void connect_Pad(gps::cMsgAgent& Agent_){
				Agent_.DataMode.slot_setReqSwap(Pad.signal(controller::is_pad1_pushed(CockpitDataModeBut)));
			}
			template<unsigned int Num_>
			void connect_Pad(battery::cMsgAgent<Num_>& Agent_){
				static boost::signals2::signal<void(bool)> Signal;
				Pad.signal(controller::is_pad1_pushed(SensorDataModeBut)).connect([&](){Signal(true); });
				Agent_.DataMode.slot_setReq(Signal);
			}
			void connect_Pad(motor::cMsgAgent& Agent_){
				Agent_.slot_setStickL(Pad.signal_LStick);
				Agent_.slot_setBrakeLON(Pad.signal(controller::is_pad1_pushed(PAD::But11)));
				Agent_.slot_setBrakeLOFF(Pad.signal(controller::is_pad1_pulled(PAD::But11)));

				Agent_.slot_setStickR(Pad.signal_RStick);
				Agent_.slot_setBrakeRON(Pad.signal(controller::is_pad1_pushed(PAD::But12)));
				Agent_.slot_setBrakeROFF(Pad.signal(controller::is_pad1_pulled(PAD::But12)));

				Agent_.slot_setBackModeON(Pad.signal(controller::is_pad1_pushed(PAD::But9)));
				Agent_.slot_setBackModeOFF(Pad.signal(controller::is_pad1_pushed(PAD::But10)));
			}
		public:
			void connect_Keyboard(thermo::cMsgAgent& Thermo_){
				Thermo_.DataMode.slot_setReqSwap(Keyboard.signal(controller::is_key_pushed(KEY::T)));
			}
			void connect_Keyboard(camera::cMsgAgent& Agent_){
				Agent_.slot_setTakePicture(Keyboard.signal(controller::is_key_pushed(KEY::S)));
				Agent_.slot_setCommandReset(Keyboard.signal(controller::is_key_pushed(KEY::C)));
				Agent_.slot_setPowerReset(Keyboard.signal(controller::is_key_pushed(KEY::P)));
			}
			void connect_Keyboard(co2::cMsgAgent& Agent_){
				Agent_.DataMode.slot_setReqSwap(Keyboard.signal(controller::is_key_pushed(KEY::C)));
				Agent_.PumpPW.slot_setReqSwap(Keyboard.signal(controller::is_key_pushed(KEY::C)));
				Agent_.SensorPW.slot_setReqSwap(Keyboard.signal(controller::is_key_pushed(KEY::C)));
			}
			void connect_Keyboard(accele::cMsgAgent& Agent_){
				Agent_.DataMode.slot_setReqSwap(Keyboard.signal(controller::is_key_pushed(KEY::A)));
				Agent_.slot_setCorrection(Keyboard.signal(controller::is_key_pushed(KEY::A)));
			}
			void connect_Keyboard(compass::cMsgAgent& Agent_){
				Agent_.DataMode.slot_setReqSwap(Keyboard.signal(controller::is_key_pushed(KEY::C)));
				Agent_.slot_setCorrectionMode(Keyboard.signal(controller::is_key_pushed(KEY::A)));
			}
			void connect_Keyboard(gyro::cMsgAgent& Agent_){
				Agent_.DataMode.slot_setReqSwap(Keyboard.signal(controller::is_key_pushed(KEY::G)));
				Agent_.slot_setCorrectionFlag(Keyboard.signal(controller::is_key_pushed(KEY::G)));
			}
			void connect_Keyboard(gps::cMsgAgent& Agent_){
				Agent_.DataMode.slot_setReqSwap(Keyboard.signal(controller::is_key_pushed(KEY::G)));
			}
			template<unsigned int Num_>
			void connect_Keyboard(battery::cMsgAgent<Num_>& Agent_){
				Agent_.slot_setDataMode(Keyboard.signal(is_key_pushed(KEY::B)));
			}
			void connect_Keyboard(cView& View){
				View.Display.slot_setMainNo0(Keyboard.signal(hmLib::predicate_and(controller::is_key_pushed(KEY::NUM1), hmLib::predicate_not(controller::have_key_pushed(KEY::SHIFT)))));
				View.Display.slot_setMainNo1(Keyboard.signal(hmLib::predicate_and(controller::is_key_pushed(KEY::NUM2), hmLib::predicate_not(controller::have_key_pushed(KEY::SHIFT)))));
				View.Display.slot_setMainNo2(Keyboard.signal(hmLib::predicate_and(controller::is_key_pushed(KEY::NUM3), hmLib::predicate_not(controller::have_key_pushed(KEY::SHIFT)))));
				View.Display.slot_setSideNo0(Keyboard.signal(hmLib::predicate_and(controller::is_key_pushed(KEY::NUM1), controller::have_key_pushed(KEY::SHIFT))));
				View.Display.slot_setSideNo1(Keyboard.signal(hmLib::predicate_and(controller::is_key_pushed(KEY::NUM2), controller::have_key_pushed(KEY::SHIFT))));
				View.Display.slot_setSideNo2(Keyboard.signal(hmLib::predicate_and(controller::is_key_pushed(KEY::NUM3), controller::have_key_pushed(KEY::SHIFT))));

				View.Display.slot_plusMainNo(Pad.signal(hmLib::predicate_and(controller::is_pad1_cross_key_pulled(PAD::CROSS_KEY::RIGHT), hmLib::predicate_not(controller::have_pad1_pushed(PAD::But7)))));
				View.Display.slot_minusMainNo(Pad.signal(hmLib::predicate_and(controller::is_pad1_cross_key_pulled(PAD::CROSS_KEY::LEFT), hmLib::predicate_not(controller::have_pad1_pushed(PAD::But7)))));
				View.Display.slot_plusSideNo(Pad.signal(hmLib::predicate_and(controller::is_pad1_cross_key_pulled(PAD::CROSS_KEY::RIGHT), controller::have_pad1_pushed(PAD::But7))));
				View.Display.slot_minusSideNo(Pad.signal(hmLib::predicate_and(controller::is_pad1_cross_key_pulled(PAD::CROSS_KEY::LEFT), controller::have_pad1_pushed(PAD::But7))));
			}
		};
	}
}
#
#endif
