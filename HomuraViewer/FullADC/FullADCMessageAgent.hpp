#ifndef HMR_VIEWER_FULLADC_FULLADCMESSAGEAGENT_INC
#define HMR_VIEWER_FULLADC_FULLADCMESSAGEAGENT_INC 100
#
#include "hmLibVer.hpp"
#include <boost/signals2.hpp>
#include <hmLib/signals.hpp>
#include <hmLib/inquiries.hpp>
#include <hmrVLib/Message/ItfMessageAgent.hpp>
#include "../hmrFlagirl.hpp"
#include <hmrVLib/Message.hpp>
#include <hmrVLib/Data.hpp>
#include "Data.hpp"

namespace hmr{
	namespace viewer{
		template<unsigned int adc_size_>
		class cFullADCMsgAgent :public itfMessageAgent{
			constexpr static unsigned int adc_size = adc_size_;
			using this_data_t = fulladc::data_t<adc_size>;
		private:
			this_data_t Data;

			flagirl DataModeFlagirl;

			flagirl Ctr1Flagirl;
			flagirl Ctr2Flagirl;
			flagirl Ctr3Flagirl;
			flagirl Ctr4Flagirl;
		public:
			bool listen(datum::time_point Time_, bool Err_, const std::string& Data_)override{
				if(Data_.size() == 0)return true;

				if(static_cast<unsigned char>(Data_[0]) == 0x00){
					if(Data_.size() != 1 + adc_size * 2)return true;

					//データを記録
					Data.Time = Time_;
					for(unsigned int ch = 0; ch < adc_size; ++ch){
						Data.Data[ch] = (static_cast<std::uint16_t>(Data_.at(1)) & 0x00ff) + (static_cast<std::uint16_t>(Data_.at(2)) << 8);
					}

					// signal 発信
					signal_newData(Data);

					return false;
				} else if(static_cast<unsigned char>(Data_[0]) == 0x10){
					if(Data_.size() != 1)return true;
					DataModeFlagirl.set_pic(true);
					return false;
				} else if(static_cast<unsigned char>(Data_[0]) == 0x11){
					if(Data_.size() != 1)return true;
					DataModeFlagirl.set_pic(false);
					return false;
				} else if(static_cast<unsigned char>(Data_[0]) == 0xA0){
					if(Data_.size() != 1)return true;
					Ctr1Flagirl.set_pic(true);
					return false;
				} else if(static_cast<unsigned char>(Data_[0]) == 0xA1){
					if(Data_.size() != 1)return true;
					Ctr1Flagirl.set_pic(false);
					return false;
				} else if(static_cast<unsigned char>(Data_[0]) == 0xA2){
					if(Data_.size() != 1)return true;
					Ctr2Flagirl.set_pic(true);
					return false;
				} else if(static_cast<unsigned char>(Data_[0]) == 0xA3){
					if(Data_.size() != 1)return true;
					Ctr2Flagirl.set_pic(false);
					return false;
				} else if(static_cast<unsigned char>(Data_[0]) == 0xA4){
					if(Data_.size() != 1)return true;
					Ctr3Flagirl.set_pic(true);
					return false;
				} else if(static_cast<unsigned char>(Data_[0]) == 0xA5){
					if(Data_.size() != 1)return true;
					Ctr3Flagirl.set_pic(false);
					return false;
				} else if(static_cast<unsigned char>(Data_[0]) == 0xA6){
					if(Data_.size() != 1)return true;
					Ctr4Flagirl.set_pic(true);
					return false;
				} else if(static_cast<unsigned char>(Data_[0]) == 0xA7){
					if(Data_.size() != 1)return true;
					Ctr4Flagirl.set_pic(false);
					return false;
				}

				return true;
			}
			bool talk(std::string& Str)override{
				Str = "";

				if(DataModeFlagirl.talk()){
					if(DataModeFlagirl.request())Str.push_back(static_cast<unsigned char>(0x10));
					else Str.push_back(static_cast<unsigned char>(0x11));
					return false;
				} else if(Ctr1Flagirl.talk()){
					if(Ctr1Flagirl.request()) Str.push_back(static_cast<unsigned char>(0xA0));
					else Str.push_back(static_cast<unsigned char>(0xA1));
					return false;
				} else if(Ctr2Flagirl.talk()){
					if(Ctr2Flagirl.request()) Str.push_back(static_cast<unsigned char>(0xA2));
					else Str.push_back(static_cast<unsigned char>(0xA3));
					return false;
				} else if(Ctr3Flagirl.talk()){
					if(Ctr3Flagirl.request()) Str.push_back(static_cast<unsigned char>(0xA4));
					else Str.push_back(static_cast<unsigned char>(0xA5));
					return false;
				} else if(Ctr4Flagirl.talk()){
					if(Ctr4Flagirl.request()) Str.push_back(static_cast<unsigned char>(0xA6));
					else Str.push_back(static_cast<unsigned char>(0xA7));
					return false;
				}

				return true;
			}
			void setup_talk(void)override{
				DataModeFlagirl.setup_talk();
				Ctr1Flagirl.setup_talk();
				Ctr2Flagirl.setup_talk();
				Ctr3Flagirl.setup_talk();
				Ctr4Flagirl.setup_talk();
			}
		private:
			hmLib::signals::unique_connections SignalConnections;
			hmLib::inquiries::unique_connections InquiryConnections;
		public:
			boost::signals2::signal<void(this_data_t)> signal_newData;

			void contact_getData(hmLib::inquiries::inquiry<this_data_t>& Inquiry_){
				InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->this_data_t{return this->Data; }));
			}

			void contact_getPicDataMode(hmLib::inquiries::inquiry<bool>& Inquiry_){
				InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->bool{return this->DataModeFlagirl.pic(); }));
			}
			void contact_getReqDataMode(hmLib::inquiries::inquiry<bool>& Inquiry_){
				InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->bool{return this->DataModeFlagirl.request(); }));
			}
			void slot_setDataMode(boost::signals2::signal<void(bool)>& Signal_){
				SignalConnections(hmLib::signals::connect(Signal_, [&](bool Flag)->void{this->DataModeFlagirl.set_request(Flag); }));
			}

			void contact_getPicCtr1(hmLib::inquiries::inquiry<bool>& Inquiry_){
				InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->bool{return this->Ctr1Flagirl.pic(); }));
			}
			void contact_getReqCtr1(hmLib::inquiries::inquiry<bool>& Inquiry_){
				InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->bool{return this->Ctr1Flagirl.request(); }));
			}
			void slot_setCtr1(boost::signals2::signal<void(bool)>& Signal_){
				SignalConnections(hmLib::signals::connect(Signal_, [&](bool Flag)->void{this->Ctr1Flagirl.set_request(Flag); }));
			}

			void contact_getPicCtr2(hmLib::inquiries::inquiry<bool>& Inquiry_){
				InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->bool{return this->Ctr2Flagirl.pic(); }));
			}
			void contact_getReqCtr2(hmLib::inquiries::inquiry<bool>& Inquiry_){
				InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->bool{return this->Ctr2Flagirl.request(); }));
			}
			void slot_setCtr2(boost::signals2::signal<void(bool)>& Signal_){
				SignalConnections(hmLib::signals::connect(Signal_, [&](bool Flag)->void{this->Ctr2Flagirl.set_request(Flag); }));
			}

			void contact_getPicCtr3(hmLib::inquiries::inquiry<bool>& Inquiry_){
				InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->bool{return this->Ctr3Flagirl.pic(); }));
			}
			void contact_getReqCtr3(hmLib::inquiries::inquiry<bool>& Inquiry_){
				InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->bool{return this->Ctr3Flagirl.request(); }));
			}
			void slot_setCtr3(boost::signals2::signal<void(bool)>& Signal_){
				SignalConnections(hmLib::signals::connect(Signal_, [&](bool Flag)->void{this->Ctr3Flagirl.set_request(Flag); }));
			}

			void contact_getPicCtr4(hmLib::inquiries::inquiry<bool>& Inquiry_){
				InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->bool{return this->Ctr4Flagirl.pic(); }));
			}
			void contact_getReqCtr4(hmLib::inquiries::inquiry<bool>& Inquiry_){
				InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->bool{return this->Ctr4Flagirl.request(); }));
			}
			void slot_setCtr4(boost::signals2::signal<void(bool)>& Signal_){
				SignalConnections(hmLib::signals::connect(Signal_, [&](bool Flag)->void{this->Ctr4Flagirl.set_request(Flag); }));
			}
		};
	}
}
#
#endif
