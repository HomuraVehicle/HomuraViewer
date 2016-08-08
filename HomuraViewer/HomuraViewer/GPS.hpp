#ifndef HMR_VIEWER_GPS_INC
#define HMR_VIEWER_GPS_INC 100
#
#include "GPS/GPSMsgAgent.hpp"
#include "GPS/GPSFileAgent.hpp"
#include "GPS/GPSKashmir.hpp"
#include "GPS/DxGPSMUI.hpp"
#include "GPS/DxGPSKashmir.hpp"
#include "GPS/DxGPSMap.hpp"
namespace hmr{
	namespace viewer{
		struct cGPS{
		public:
			gps::cMsgAgent MsgAgent;
			gps::cFileAgent FileAgent;
			gps::cGPGGAFileAgent GPGGAFileAgent;
			gps::cGPSKashmir GPSKashmir;
			gps::dxosMUI MUI;

			bool GPSCh;
			boost::signals2::signal<void(bool)> signal_changeGPSCh;
		public:
			cGPS():GPSCh(false){
				FileAgent.slot_log_writeData(MsgAgent.signal_newData);
				GPGGAFileAgent.slot_log_writeData(MsgAgent.signal_newData);

				GPSKashmir.slot_sendGPGGA(MsgAgent.signal_newData);

				MUI.slot_getData(MsgAgent.signal_newData);
				MUI.IsDataModeMUIBut.connect(MsgAgent.DataMode);
				MUI.signal_changeGPSCh.connect([&](void)->void{GPSCh = !GPSCh; signal_changeGPSCh(GPSCh); });

				MsgAgent.SwapState.slot_setReq(signal_changeGPSCh);
				MsgAgent.SwapState.contact_getPic(MUI.inq_getPic_GPSCh);
				MsgAgent.SwapState.contact_getReq(MUI.inq_getRequest_GPSCh);
			}
		};
	}
}
#
#endif
