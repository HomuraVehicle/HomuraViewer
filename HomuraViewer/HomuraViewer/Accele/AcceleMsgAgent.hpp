#ifndef HMR_VIEWER_ACCELE_MESSAGEAGENT_INC
#define HMR_VIEWER_ACCELE_MESSAGEAGENT_INC 101
#
/*==========hmrAccele============
hmrAccele v1_01/130602 iwahori
	キーボード・パッド用のslot_setDataMode()を追加(引数がvoid(void)になっている)
hmrAccele v1_00/130511 iwahori
	cAcceleMsAgentのsignal-slot,inquiry-contactへの対応完了
*/
#include"hmLibVer.hpp"
#include<boost/signals2.hpp>
#include<hmLib/signals.hpp>
#include<hmLib/inquiries.hpp>
#include<HomuraViewer/Message/ItfMessageAgent.hpp>
#include<HomuraViewer/modeflags.hpp>
#include<hmLib/coordinates.hpp>

namespace hmr{
	namespace viewer{
		namespace accele{
			class cMsgAgent :public itfMessageAgent{
				typedef hmLib::coordinates3D::position position;
				typedef hmLib::coordinates3D::polar polar;
			private:
				position AcceleData;
				position CorrectionValue;
				bool CorrectionFlag;
				clock::time_point Time;
				unsigned int MsgPos;
			public:
				modeflags DataMode;
			private:
				position toData(const std::string& Str_){
					double X, Y, Z;
					if(Str_.size() != 7)return position();
					X = static_cast<signed short>((static_cast<unsigned char>(Str_[1]) + 0x100 * static_cast<unsigned char>(Str_[2])));
					Y = static_cast<signed short>((static_cast<unsigned char>(Str_[3]) + 0x100 * static_cast<unsigned char>(Str_[4])));
					Z = static_cast<signed short>((static_cast<unsigned char>(Str_[5]) + 0x100 * static_cast<unsigned char>(Str_[6])));
					return position(X - CorrectionValue.x, Y - CorrectionValue.y, Z - CorrectionValue.z);
				}
			public:
				cMsgAgent() :AcceleData(), Time(), DataMode(), CorrectionValue(), CorrectionFlag(false){}
				bool listen(datum::time_point Time_, bool Err_, const std::string& Str_)override{
					if(Str_.size() == 0)return true;

					if(static_cast<unsigned char>(Str_[0]) == 0x00){
						if(Str_.size() != 7)return true;
						AcceleData = toData(Str_);
						Time = Time_;

						//signal
						signal_newData(Time, AcceleData, getPolarData());
						return false;

					} else if(static_cast<unsigned char>(Str_[0]) == 0x10){
						if(Str_.size() != 1)return true;
						DataMode.set_pic(true);
						return false;
					} else if(static_cast<unsigned char>(Str_[0]) == 0x11){
						if(Str_.size() != 1)return true;
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
			private:
				hmLib::signals::unique_connections SignalConnections;
				hmLib::inquiries::unique_connections InquiryConnections;
				void setCorrectionValue(void){
					static position StdValue1;
					if(!CorrectionFlag){
						StdValue1 = AcceleData;
						CorrectionFlag = true;
					} else{
						position StdValue2 = AcceleData;
						CorrectionValue = (StdValue1 + StdValue2) / 2;
						CorrectionFlag = false;
					}
				}
				void clearCorrectionValue(void){
					CorrectionValue.x = 0;
					CorrectionValue.y = 0;
					CorrectionValue.z = 0;
				}
				polar getPolarData(void){
					return polar(position(-AcceleData.z, -AcceleData.y, -AcceleData.x));
				}
			public:
				// signal 
				boost::signals2::signal<void(clock::time_point time_, position pos_, polar por_)> signal_newData;

				void contact_getAccelePosition(hmLib::inquiries::inquiry<position>& Inquiry_){
					InquiryConnections(hmLib::inquiries::connect(Inquiry_, AcceleData));
				}
				void contact_getAccelePolar(hmLib::inquiries::inquiry<polar>& Inquiry_){
					InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->polar{return this->getPolarData(); }));
				}
				void contact_getTime(hmLib::inquiries::inquiry<clock::time_point>& Inquiry_){
					InquiryConnections(hmLib::inquiries::connect(Inquiry_, Time));
				}
				void slot_setCorrection(boost::signals2::signal<void(void)>& Signal_){
					SignalConnections(hmLib::signals::connect(Signal_, [&](void)->void{setCorrectionValue(); }));
				}
				void slot_clearCorrection(boost::signals2::signal<void(void)>& Signal_){
					SignalConnections(hmLib::signals::connect(Signal_, [&](void)->void{clearCorrectionValue(); }));
				}
				void contact_getCorrectionFlag(hmLib::inquiries::inquiry<bool>& Inquiry_){
					InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->bool{return this->CorrectionFlag; }));
				}
				void contact_getCorrectionValue(hmLib::inquiries::inquiry<position>& Inquiry_){
					InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->position{return this->CorrectionValue; }));
				}
			};
		}
	}
}
#
#endif
