#ifndef HMR_VIEWER_CAMERA_INC
#define HMR_VIEWER_CAMERA_INC 100
#
#include"Camera/CameraFileAgent.hpp"
#include"Camera/CameraMsgAgent.hpp"
#include"Camera/DxCameraMUI.hpp"
namespace hmr{
	namespace viewer{
		struct cCamera{
		public:
			camera::cMsgAgent MsgAgent;
			camera::cFileAgent FileAgent;
			camera::dxosMUI MUI;
		public:
			cCamera() :FileAgent("Sprite") {
				FileAgent.slot_log_writeData(MsgAgent.signal_setPicture);

				MUI.InfoModeMUIBut.connect(MsgAgent.InfoMode);
				MUI.AutoLightMUIBut.connect(MsgAgent.AutoLight);
				MUI.LightMUIBut.connect(MsgAgent.Light);
				MUI.AutoPowerResetMUIBut.connect(MsgAgent.AutoPowerReset);
				MUI.AutoTakePictureMUIBut.connect(MsgAgent.AutoTakePic);
				MUI.MiniPacketModeMUIBut.connect(MsgAgent.MiniPacketMode);

				MsgAgent.slot_setTakePicture(MUI.TakePictureMUIBut.Signal);
				MsgAgent.contact_getDoTakePictureFlag(MUI.TakePictureMUIBut.Req);

				MsgAgent.slot_setPictureSize(MUI.signal_setPictureSize);
				MsgAgent.contact_getPictureSize(MUI.inquiry_getPictureSize);

				MsgAgent.slot_setCommandReset(MUI.CommandResetMUIBut.Signal);
				MsgAgent.contact_getDoCommandResetFlag(MUI.CommandResetMUIBut.Req);

				MsgAgent.slot_setPowerReset(MUI.PowerResetMUIBut.Signal);
				MsgAgent.contact_getDoPowerResetFlag(MUI.PowerResetMUIBut.Req);

				MsgAgent.contact_getStatus(MUI.inquiry_getStatus);
				MsgAgent.contact_getIsErr(MUI.inquiry_getIsErr);
				MsgAgent.contact_getErrCode(MUI.inquiry_getErrCode);
				MsgAgent.contact_getStatusTime(MUI.inquiry_getStatusTime);
				MsgAgent.contact_getDataPosSize(MUI.inquiry_getDataPosSize);
				MsgAgent.contact_getDataTime(MUI.inquiry_getDataTime);
			}
		};
	}
}
#
#endif
