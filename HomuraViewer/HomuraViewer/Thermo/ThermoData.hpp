#ifndef HMR_VIEWER_THERMO_THERMODATA_INC
#define HMR_VIEWER_THERMO_THERMODATA_INC 100
#
#include<HomuraViewer/chrono.hpp>
namespace hmr{
	namespace viewer{
		namespace thermo{
			struct data_t{
				clock::time_point Time;
				uint16_t RawData;
				double Temperature;
			public:
				data_t()
					: Time()
					, RawData(0)
					, Temperature(0.0){
				}
				clock::time_point time()const{ return Time; }
				uint16_t raw_value()const{ return RawData; }
				double value()const{ return Temperature; }
			};
		}
	}
}
#
#endif
