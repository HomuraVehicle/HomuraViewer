#ifndef HMR_VIEWER_ACCELE_LOGGER_INC
#define HMR_VIEWER_ACCELE_LOGGER_INC 101
#
/*===hmrAcceleLogger===
v1_01
	coordinatesの更新に伴い、coordinates::polarの割り算が角度を割ってくれなくなっていた問題を修正
Acceleのデータを一定時間蓄積し続ける
*/
#include<deque>
#include<boost/signals2.hpp>
#include<hmLib/signals.hpp>
#include<hmLib/inquiries.hpp>
#include <hmLib/coordinates.hpp>
#include <HomuraViewer/chrono.hpp>
namespace hmr{
	namespace viewer{
		namespace accele{
			class cLogger{
				typedef hmLib::coordinates3D::polar polar;
				typedef std::pair<clock::time_point, polar> pair_time_angle;
				typedef std::deque<pair_time_angle> containor;
			public:
				typedef containor::const_reverse_iterator iterator;
			private:
				containor Log;
				hmLib::signals::unique_connections SignalConnections;
				hmLib::inquiries::unique_connections InquiryConnections;
			public:
				void slot_addLog(boost::signals2::signal<void(clock::time_point time_, hmLib::coordinates3D::position pos_, polar por_)>& Signal_){
					SignalConnections(
						hmLib::signals::connect(
						Signal_,
						[&](clock::time_point time, hmLib::coordinates3D::position pos_, polar por_)->void{
						this->Log.push_back(std::make_pair(time, por_));
						if(this->Log.size()>1000)this->Log.pop_front();
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
				void contact_getAcceleMeanPolar(hmLib::inquiries::inquiry<polar>& Inquiry_){
					// Log dat size　の確認
					/*			int datSize;
					if(Log.size() > 10){
					datSize = 10;
					}else{
					datSize = Log.size();
					}
					// 平均をとる処理
					iterator itr = Log.end();
					for(int i = 0; i< datSize; i++){
					meanPolar += itr->second;
					}
					*/

					// return 関数として返す
					InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->polar{
						polar meanPolar(0, 0, 0);
						int datSize = std::min(20u, Log.size());
						for(auto itr = Log.rbegin(); itr != Log.rbegin() + datSize; ++itr)meanPolar += itr->second;

						//meanPolar/datSize では、rだけが短くなる
						//角度も割るためには、各要素への明示的な割り算が必要
						meanPolar.phi /= datSize;
						meanPolar.theta /= datSize;
						meanPolar.r /= datSize;
						return meanPolar;
					}));
				}
			};
		}
	}
}
#
#endif
