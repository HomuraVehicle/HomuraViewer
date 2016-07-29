#ifndef HMR_VIEWER_BATTERY_INC
#define HMR_VIEWER_BATTERY_INC 100
#
#include<HomuraViewer/chrono.hpp>
#include"Battery/BatteryData.hpp"
#include"Battery/BatteryFileAgent.hpp"
#include"Battery/BatteryMessageAgent.hpp"
#include"Battery/DxBatteryMUI.hpp"
#include"Battery/DxBattery.hpp"
namespace hmr{
	namespace viewer{
		struct cBattery{
			static constexpr unsigned int BatteryNum = 3;
			using this_data_t = battery::data<BatteryNum>;
		private:
			this_data_t CurrentData;
		public:
			battery::cMsgAgent<BatteryNum> MsgAgent;
			battery::cFileAgent<BatteryNum> FileAgent;
			boost::signals2::signal<void(clock::time_point, double)> signal_newEachData[BatteryNum];
		public:
			cBattery(){
				MsgAgent.signal_newData.connect(
					[&](this_data_t Data_){
						CurrentData = Data_;
						for(unsigned int Cnt = 0; Cnt < BatteryNum; ++Cnt){
							signal_newEachData[Cnt](Data_.time(), Data_.at(Cnt));
						}
					}
				);
				FileAgent.slot_logData(MsgAgent.signal_newData);
			}
			void connect(battery::dxosMUI<BatteryNum>& MUI){
				hmLib::inquiries::connect(MUI.inquiry_getData, CurrentData);
				hmr::viewer::connect(MsgAgent.DataMode, MUI.IsDataModeMUIBut);
			}
		};
	}
}
#
#endif
