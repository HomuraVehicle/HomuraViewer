#ifndef HMR_VIEWER_MOTOR_INC
#define HMR_VIEWER_MOTOR_INC 100
#
#include"Motor/MotorMsgAgent.hpp"
#include"Motor/DxMotorMUI.hpp"
namespace hmr{
	namespace viewer{
		struct cMotor{
			motor::cMsgAgent MsgAgent;
			motor::dxosMUI MUI;
		public:
			cMotor(){
				MsgAgent.slot_setStickL(MUI.Signal_MotorStickL);
				MsgAgent.contact_StickL(MUI.inquiry_getMotorStickL);
				MsgAgent.contact_BrakeL(MUI.inquiry_getMotorBrakeL);
				MsgAgent.slot_setStickR(MUI.Signal_MotorStickR);
				MsgAgent.contact_StickR(MUI.inquiry_getMotorStickR);
				MsgAgent.contact_BrakeR(MUI.inquiry_getMotorBrakeR);

				MsgAgent.slot_setBackMode(MUI.Signal_BackMode);
				MsgAgent.contact_BackMode(MUI.inquiry_getBackMode);
			}
		};
	}
}
#
#endif
