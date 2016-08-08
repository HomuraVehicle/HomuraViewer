#ifndef HMR_VIEWER_FULLADC_FULLADCMESSAGEAGENT_INC
#define HMR_VIEWER_FULLADC_FULLADCMESSAGEAGENT_INC 100
#
#include "hmLibVer.hpp"
#include <boost/signals2.hpp>
#include <hmLib/signals.hpp>
#include <hmLib/inquiries.hpp>
#include <HomuraViewer/Message/ItfMessageAgent.hpp>
#include <HomuraViewer/Message.hpp>
#include <HomuraViewer/chrono.hpp>
#include <HomuraViewer/modeflags.hpp>

namespace hmr{
	namespace viewer{
		namespace fulladc{
			template<unsigned int adc_size_>
			class cMsgAgent :public itfMessageAgent{
				constexpr static unsigned int adc_size = adc_size_;
				using this_data_t = fulladc::data_t<adc_size>;
			public:
				modeflags DataMode;
			private:
				unsigned int MsgPos;
			public:
				bool listen(datum::time_point Time_, bool Err_, const std::string& Data_)override{
					if(Data_.size() == 0)return true;

					if(static_cast<unsigned char>(Data_[0]) == 0x00){
						if(Data_.size() != 1 + adc_size * 2)return true;

						//データを記録
						this_data_t Data;
						Data.Time = Time_;
						for(unsigned int ch = 0; ch < adc_size; ++ch){
							Data.Data[ch] = (static_cast<std::uint16_t>(Data_.at(1)) & 0x00ff) + (static_cast<std::uint16_t>(Data_.at(2)) << 8);
						}

						// signal 発信
						signal_newData(Data);

						return false;
					} else if(static_cast<unsigned char>(Data_[0]) == 0x10){
						if(Data_.size() != 1)return true;
						DataMode.set_pic(true);
						return false;
					} else if(static_cast<unsigned char>(Data_[0]) == 0x11){
						if(Data_.size() != 1)return true;
						DataMode.set_pic(false);
						return false;
					}

					return true;
				}
				bool talk(std::string& Str)override{
					Str = "";
					switch(MsgPos){
					case 0:
						++MsgPos;
						if(DataMode){
							if(DataMode.get_req())Str.push_back(static_cast<unsigned char>(0x10));
							else Str.push_back(static_cast<unsigned char>(0x11));
							return false;
						}
					default:
						return true;
					}
				}
				void setup_talk(void)override{
					MsgPos = 0;
				}
			public:
				boost::signals2::signal<void(this_data_t)> signal_newData;
			};
		}
	}
}
#
#endif
