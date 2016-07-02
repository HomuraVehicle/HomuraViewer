#ifndef HMR_FULLADC_DATA_INC
#define HMR_FULLADC_DATA_INC 100
#
#include<array>
#include<cstdint>
#include<hmrVLib/Data.hpp>
namespace hmr{
	namespace fulladc{
		template<unsigned int adc_size>
		struct data_t{
			clock::time_point Time;
			std::array<std::uint16_t, adc_size> Data;
		};
	}
}
#
#endif
