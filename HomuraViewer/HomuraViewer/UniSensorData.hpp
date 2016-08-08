#ifndef HMR_VIEWER_UNISENSORDATA_INC
#define HMR_VIEWER_UNISENSORDATA_INC 100
#
#include<HomuraViewer/chrono.hpp>
namespace hmr{
	namespace viewer{
		struct cUniSensorData{
			clock::time_point Time;
			uint16_t RawValue;
			double Value;
		public:
			cUniSensorData()
				: Time()
				, RawValue(0)
				, Value(0.0){
			}
			clock::time_point time()const{ return Time; }
			uint16_t raw_value()const{ return RawValue; }
			double value()const{ return Value; }
		};
	}
}
#
#endif
