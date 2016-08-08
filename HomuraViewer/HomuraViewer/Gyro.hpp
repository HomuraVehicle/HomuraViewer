#ifndef HMR_VIEWER_GYRO_INC
#define HMR_VIEWER_GYRO_INC 100
#
#include "Gyro/GyroMsgAgent.hpp"
#include "Gyro/GyroCompass.hpp"
#include "Gyro/GyroLogger.hpp"
#include "Gyro/DxGyroMUI.hpp"
namespace hmr{
	namespace viewer{
		struct cGyro{
		public:
			gyro::cMsgAgent MsgAgent;
			gyro::cGyroCompass Compass;
			gyro::cGyroLogger Logger;
			gyro::dxosMUI MUI;
		public:
			cGyro(){
				//gyro
				Compass.slot_addAngle(MsgAgent.signal_newData);
				Logger.slot_addLog(MsgAgent.signal_newData);

				MUI.slot_getData(MsgAgent.signal_newData);
				MUI.IsDataModeMUIBut.connect(MsgAgent.DataMode);

				MsgAgent.slot_setCorrectionFlag(MUI.CorrectionDataBut.Signal);
				MsgAgent.contact_getCorrectionFlag(MUI.CorrectionDataBut.Req);
				MsgAgent.contact_getGyroCorrectionData(MUI.inquiry_getCorrectionValue);
			}
		};
	}
}
#
#endif
