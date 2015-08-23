#ifndef HMR_BATTERYDATASET_INC
#define HMR_BATTERYDATASET_INC 100
#

#include"hmLibVer.hpp"
#include"hmrData.hpp"
#include<array>

namespace hmr{
	template<std::size_t SIZE>
	class BatteryDataSet{
	private:
		std::array<double,SIZE> BatteryVoltage;
	public:
		double getVoltage(unsigned char Num_)const{
			if(Num>=SIZE)return -1;
			else return BatteryVoltage[Num];
		}
		std::size_t getBatteryNum()const{return BatteryVoltage.size();}
		std::array<double,SIZE>::iterator begin(){return BatteryVoltage.begin();}
		std::array<double,SIZE>::iterator end(){return BatteryVoltage.end();}
	};
}

#
#endif