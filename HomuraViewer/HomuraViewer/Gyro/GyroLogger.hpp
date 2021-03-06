#ifndef HMR_VIEWER_GYRO_GYROLOGGER_INC
#define HMR_VIEWER_GYRO_GYROLOGGER_INC 100
#
/*===hmrGyroLogger===
Gyroのデータを一定時間蓄積し続ける
*/
#include<deque>
#include<boost/signals2.hpp>
#include<hmLib/signals.hpp>
#include<hmLib/inquiries.hpp>
#include <hmLib/coordinates.hpp>
#include <HomuraViewer/chrono.hpp>
namespace hmr{
	namespace viewer{
		namespace gyro{
			class cGyroLogger{
				typedef hmLib::coordinates3D::angle angle;
				typedef std::pair<clock::time_point, angle> pair_time_angle;
				typedef std::deque<pair_time_angle> containor;
			public:
				typedef containor::const_reverse_iterator iterator;
			private:
				containor Log;
				hmLib::signals::unique_connections SignalConnections;
				hmLib::inquiries::unique_connections InquiryConnections;
			public:
				void slot_addLog(boost::signals2::signal<void(angle, double, clock::time_point)>& Signal_){
					SignalConnections(
						hmLib::signals::connect(
						Signal_,
						[&](angle Angl_, double dt, clock::time_point time)->void{
						this->Log.push_back(std::make_pair(time, Angl_*dt));
						if(this->Log.size() > 127)this->Log.pop_front();
					}
					)
					);
				}
				void contact_getLogs(hmLib::inquiries::range_inquiry<iterator>& RangeInquiry_){
					InquiryConnections(
						hmLib::inquiries::connect(
						RangeInquiry_,
						[&](void)->iterator{return this->Log.rbegin(); },
						[&](void)->iterator{return this->Log.rend(); }
					)
					);

				}
			};
		}
	}
}
#
#endif
