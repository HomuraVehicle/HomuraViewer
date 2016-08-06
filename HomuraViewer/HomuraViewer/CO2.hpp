#ifndef HMR_VIEWER_CO2_INC
#define HMR_VIEWER_CO2_INC 100
#
#include"CO2/CO2Data.hpp"
#include"CO2/CO2MsgAgent.hpp"
#include"CO2/DxCO2MUI.hpp"
#include"File/UniSensorFileAgent.hpp"
namespace hmr{
	namespace viewer{
		struct cCO2{
			using data_t = co2::data_t;
		public:
			co2::cMsgAgent MsgAgent;
			co2::dxosMUI MUI;
			file::cUniSensorFileAgent<data_t> FileAgent;
		public:
			cCO2() :FileAgent("co2.txt", "\t"){
				MUI.slot_getData(MsgAgent.signal_newData);
				MUI.DataModeMUIBut.connect(MsgAgent.DataMode);
				MUI.SensorPWMUIBut.connect(MsgAgent.SensorPW);
				MUI.PumpPWMUIBut.connect(MsgAgent.PumpPW);

				FileAgent.slot_log_writeData(MsgAgent.signal_newData);
			}
		};
	}
}
#
#endif
