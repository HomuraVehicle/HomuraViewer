#ifndef HMR_VIEWER_COMPASS_INC
#define HMR_VIEWER_COMPASS_INC 100
#
#include"Compass/CompassMsgAgent.hpp"
#include"Compass/DxCompassMUI.hpp"
#include"Compass/MagneticCompass.hpp"
namespace hmr{
	namespace viewer{
		struct cCompass{
		public:
			compass::cMsgAgent MsgAgent;
			compass::cMagneticCompass Compass;
			compass::dxosMUI MUI;
		public:
			cCompass(){
				//compass
				Compass.slot_addCompassData(MsgAgent.signal_newData);

				MUI.IsDataModeMUIBut.connect(MsgAgent.DataMode);

				MsgAgent.contact_getCorrectionMode(MUI.inquiry_isCorrectionMode);
				MsgAgent.slot_setCorrectionMode(MUI.signal_setCorrectionMode);
				MsgAgent.slot_addCorrection(MUI.signal_addCorrection);
				MsgAgent.slot_setCorrectionFromFile(MUI.signal_setCorrectionFromFile);
				MsgAgent.contact_getCorrectionNum(MUI.inquiry_getCorrectionNum);
				MsgAgent.contact_getCompassCorrectionData(MUI.inquiry_getCorrectionValue);
			}
		};
	}
}
#
#endif
