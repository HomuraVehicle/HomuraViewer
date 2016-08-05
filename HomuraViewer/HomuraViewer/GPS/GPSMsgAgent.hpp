#ifndef HMR_VIEWER_GPS_GPSMSGAGENT_INC
#define HMR_VIEWER_GPS_GPSMSGAGENT_INC 103
#

/*===========hmrGPS===========
hmrGPS v1_03/130713 amby
	GPSの切り替えコマンドの実装
hmrGPS v1_02/130629 hmIto
	データ受信シグナルを公開
hmrGPS v1_01/130602 iwahori
	キーボード・パッド用のslot_setDataMode()を追加(引数がvoid(void)になっている)
hmrGPS v1_00/130427 iwahori
	cGPSMsgAgentのsignal-slot,inquiry-contactへの対応完了
*/

#include "hmLibVer.hpp"
#include<boost/signals2.hpp>
#include<hmLib/signals.hpp>
#include <hmLib/inquiries.hpp>
#include <HomuraViewer/Message/ItfMessageAgent.hpp>
#include <HomuraViewer/modeflags.hpp>
#include "GPSData.hpp"

namespace hmr{
	namespace viewer{
		namespace gps{
			class cMsgAgent:public itfMessageAgent{
			private:
				data GPSData;
				unsigned char gpsCh;
				unsigned int MsgPos;
				static data toGPSDataSet(const clock::time_point& Time_, bool errFlag_, const std::string& data_){
					if(data_.size() == 21 && !errFlag_){
						int UseGPS;
						data::pdouble	pos;
						double  height;
						bool	failFlag;
						int UTC_H = (int)((unsigned char)data_[1]);
						int UTC_M = (int)((unsigned char)data_[2]);
						int UTC_S = (int)((unsigned char)data_[3]);
						int UTC_MS = (int)((unsigned char)data_[4]) * 10 + (int)(((unsigned char)data_[5]) / 10);

						//時間補正	
						//				date cDate = hm::time()-hmTime(9*60*60,0);	
						date cDate;
						cDate.from_time(clock::now() - std::chrono::hours(9));
						if(cDate.Hour >= 0 && cDate.Hour < 1){
							if(UTC_H < 24 && UTC_H >= 23){
								cDate.Day -= 1;
							}
						} else if(cDate.Hour < 24 && cDate.Hour >= 23){
							if(UTC_H < 1 && UTC_H >= 0){
								cDate.Day += 1;
							}
						}
						//時間挿入
						cDate.Hour = UTC_H;
						cDate.Min = UTC_M;
						cDate.Sec = UTC_S;
						cDate.mSec = UTC_MS;

						//位置の挿入
						pos.y = (unsigned char)data_[6] + (((unsigned char)data_[7]) / 1. + ((unsigned char)data_[8]) / 100. + ((unsigned char)data_[9]) / 10000.) / 60.;
						if(data_[10] == 'S') pos.y *= -1.;
						pos.x = (unsigned char)data_[11] + (((unsigned char)data_[12]) / 1. + ((unsigned char)data_[13]) / 100. + ((unsigned char)data_[14]) / 10000.) / 60.;
						if(data_[15] == 'W')pos.x *= -1.;
						//failフラグ
						failFlag = (data_[16] == 0);
						//高さ
						height = data_[17] * 1000. + data_[18] * 10. + data_[19] * 0.1;
						//使用GPS
						UseGPS = data_[20];

						return data(pos, height, Time_, cDate.to_time(), failFlag, errFlag_, UseGPS);
					} else{
						date Date;
						return data(data::pdouble(0., 0.), 0., Time_, Date.to_time(), false, true, 0);
					}
				}
			public:
				modeflags DataMode;
				modeflags SwapState;
			public:
				bool listen(datum::time_point Time_, bool errFlag_, const std::string& Data_)override{
					if(Data_.size() == 0)return true;

					if(static_cast<unsigned char>(Data_[0]) == 0x00){
						if(Data_.size() != 21)return true;
						GPSData = toGPSDataSet(Time_, errFlag_, Data_);
						signal_newData(GPSData, Time_);
						return false;
					} else if(static_cast<unsigned char>(Data_[0]) == 0x10){
						if(Data_.size() != 1)return true;
						DataMode.set_pic(true);
						return false;
					} else if(static_cast<unsigned char>(Data_[0]) == 0x11){
						if(Data_.size() != 1)return true;
						DataMode.set_pic(false);
						return false;
					} else if(static_cast<unsigned char>(Data_[0]) == 0x20){
						if(Data_.size() != 2)return true;
						gpsCh = static_cast<unsigned char>(Data_[1]);
						if(gpsCh == 0x00){
							SwapState.set_pic(false); // 更新する 
						} else{ SwapState.set_pic(true); }
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
							if(DataMode.get_req())Str = static_cast<unsigned char>(0x10);
							else Str = static_cast<unsigned char>(0x11);
							return false;
						}
					case 1:
						++MsgPos;
						if(SwapState){
							unsigned char char_;
							if(SwapState.get_req()){ char_ = static_cast<unsigned char>(0x01); } else{ char_ = static_cast<unsigned char>(0x00); }//

							Str = static_cast<unsigned char>(0x20);
							Str.push_back(char_);
							return false;
						}
					default:
						return true;
					}
				}
				void setup_talk(void)override{
					MsgPos = 0;
				}
			private:
				hmLib::signals::unique_connections SignalConnections;
				hmLib::inquiries::unique_connections InquiryConnections;
				data getGPSData()const{ return GPSData; }
			public:
				boost::signals2::signal<void(const data& Data_, clock::time_point time)> signal_newData;
				void contact_getGPSData(hmLib::inquiries::inquiry<data>& Inquiry_){
					InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->data{return this->getGPSData(); }));
				}
				void contact_getGPSCh(hmLib::inquiries::inquiry<unsigned char>& Inquiry_){
					InquiryConnections(hmLib::inquiries::connect(Inquiry_, gpsCh));
				}
			public:
				cMsgAgent() :gpsCh(0){}
			};
		}
	}
}
#
#endif
