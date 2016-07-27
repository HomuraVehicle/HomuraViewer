#ifndef HMR_BATTERY_DATASET_INC
#define HMR_BATTERY_DATASET_INC 100
#

#include "hmLibVer.hpp"
#include <HomuraViewer/chrono.hpp>
#include <array>

namespace hmr{
	namespace viewer{
		namespace battery{
			template<unsigned int  data_size_>
			class BatteryDataSet{
				static constexpr unsigned int data_size = data_size_;
			public:
				static constexpr unsigned int getBatteryNum(){ return data_size; }
			private:
				std::array<double, data_size> BatteryVoltage;
			public:
				double getVoltage(unsigned char data_pos)const{
					if(data_pos >= data_size)return -1;
					else return BatteryVoltage[data_pos];
				}
				std::array<double, data_size>::iterator begin(){ return BatteryVoltage.begin(); }
				std::array<double, data_size>::iterator end(){ return BatteryVoltage.end(); }
			};
		}
	}
}

#
#endif