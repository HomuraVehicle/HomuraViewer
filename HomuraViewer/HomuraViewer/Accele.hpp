#ifndef HMR_VIEWER_ACCELE_INC
#define HMR_VIEWER_ACCELE_INC 100
#
#include"Accele/AcceleLogger.hpp"
#include"Accele/AcceleMsgAgent.hpp"
#include"Accele/DxAcceleMUI.hpp"

namespace hmr{
	namespace viewer{
		struct cAccele{
		public:
			accele::cMsgAgent MsgAgent;
			accele::cLogger Logger;
			accele::dxosMUI MUI;
		public:
			cAccele(){
				//accele
				Logger.slot_addLog(MsgAgent.signal_newData);

				MUI.slot_getData(MsgAgent.signal_newData);
				MUI.IsDataModeMUIBut.connect(MsgAgent.DataMode);

				MsgAgent.slot_setCorrection(MUI.CorrectionDataBut.Signal);
				MsgAgent.contact_getCorrectionFlag(MUI.CorrectionDataBut.Req);
				MsgAgent.contact_getCorrectionValue(MUI.inquiry_getCorrectionValue);
			}
		};
	}
}
#
#endif
