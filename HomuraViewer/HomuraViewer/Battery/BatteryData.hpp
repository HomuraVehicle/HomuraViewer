#ifndef HMR_BATTERY_BATTERYDATA_INC
#define HMR_BATTERY_BATTERYDATA_INC 100
#

#include "hmLibVer.hpp"
#include <HomuraViewer/chrono.hpp>
#include <array>

namespace hmr{
	namespace viewer{
		namespace battery{
			template<unsigned int  data_size_>
			class data{
			public:
				using container = std::array<double, data_size_>;
				using iterator = container::iterator;
				static constexpr unsigned int data_size = data_size_;
			private:
				clock::time_point Time;
				std::array<double, data_size> BatteryVoltage;
			public:
				data() = default;
				data(clock::time_point Time_, std::array<double, data_size> BatteryVoltage_)
					: Time(Time_)
					, BatteryVoltage(BatteryVoltage_){
				}
				iterator begin(){ return BatteryVoltage.begin(); }
				iterator end(){ return BatteryVoltage.end(); }
				double& at(unsigned int pos_){ return BatteryVoltage[pos_]; }
				static constexpr unsigned int size(){ return data_size; }
				clock::time_point& time(){ return Time; }
			};
		}
	}
}

#
#endif