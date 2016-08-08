#ifndef HMR_VIEWER_THERMO_INC
#define HMR_VIEWER_THERMO_INC 100
#
#include"Thermo/ThermoData.hpp"
#include"Thermo/ThermoMsgAgent.hpp"
#include"Thermo/DxThermoMUI.hpp"
#include"File/UniSensorFileAgent.hpp"
namespace hmr{
	namespace viewer{
		struct cThermo{
			using data_t = thermo::data_t;
		public:
			thermo::cMsgAgent MsgAgent;
			thermo::dxosMUI MUI;
			file::cUniSensorFileAgent<data_t> FileAgent;
		public:
			cThermo():FileAgent("thermo.txt","\t"){
				MUI.slot_getData(MsgAgent.signal_newData);
				MUI.IsDataModeMUIBut.connect(MsgAgent.DataMode);

				FileAgent.slot_log_writeData(MsgAgent.signal_newData);
			}
		};
	}
}
#
#endif
