#ifndef HMR_INFRARED_INC
#define HMR_INFRARED_INC 102
#
/*=============hmrInfraRed===============
hmrInfraRed v1_02/130602 iwahori
	キーボード・パッド用のslot_setDataMode()を追加(引数がvoid(void)になっている)
hmrInfraRed v1_01/130427 iwahori
	Flagirlへのinquiryを作成
hmrInfraRed v1_00/130420 iwahori
	cInfraRedMsgAgentのsignal-slot,inquiry-contactへの対応完了
*/

#include "hmLibVer.hpp"
#include<boost/signals2.hpp>
#include<hmLib/signals.hpp>
#include <hmLib/inquiries.hpp>
#include <HomuraViewer/Message/ItfMessageAgent.hpp>
#include "hmrFlagirl.hpp"
#include "hmrADC.h"

namespace hmr{
	namespace viewer{
		class cInfraRedMsgAgent :public itfMessageAgent{
		private:
			flagirl DataModeFlagirl;
			double InTemperature;
			double OutTemperature;
			double Temperature;
			//		static const double D_ADMaxValue;
			clock::time_point Time;
			static double toInfraRedIn(unsigned int Value_){ return (Value_*2.*4096. / D_ADMaxValue - 1084.) / 39.905; }
			static double toInfraRedOut(unsigned int Value_){ return (Value_*2.*4096. / D_ADMaxValue - 1645.); }
			static double toInfraRed(double Out_, double In_){ return (Out_*0.2043 + In_) / (1 + 0.00069*Out_); }
		public:
			cInfraRedMsgAgent()
				:DataModeFlagirl()
				, InTemperature(0.)
				, OutTemperature(0.)
				, Temperature(0.)
				, Time(){}
			bool listen(datum::time_point Time_, bool IsErr_, const std::string& Data_)override{
				if(Data_.size() == 0)return true;

				if(static_cast<unsigned char>(Data_[0]) == 0x00){
					if(Data_.size() != 5)return true;
					unsigned int OutValue = static_cast<unsigned char>(Data_[1]) + 256 * static_cast<unsigned char>(Data_[2]);
					unsigned int InValue = static_cast<unsigned char>(Data_[3]) + 256 * static_cast<unsigned char>(Data_[4]);
					InTemperature = toInfraRedIn(InValue);
					OutTemperature = toInfraRedOut(OutValue);
					Temperature = toInfraRed(OutTemperature, InTemperature);
					Time = Time_;
					signal_newData(Temperature, OutTemperature, InTemperature, Time);
					return false;
				} else if(static_cast<unsigned char>(Data_[0]) == 0x10){
					if(Data_.size() != 1)return true;
					DataModeFlagirl.set_pic(true);
					return false;
				} else if(static_cast<unsigned char>(Data_[0]) == 0x11){
					if(Data_.size() != 1)return true;
					DataModeFlagirl.set_pic(false);
					return false;
				}
				return true;
			}
			bool talk(std::string& Str)override{
				if(DataModeFlagirl.talk()){
					if(DataModeFlagirl.request())Str.push_back(static_cast<unsigned char>(0x10));
					else Str.push_back(static_cast<unsigned char>(0x11));
					return false;
				}
				return true;
			}
			void setup_talk(void)override{
				DataModeFlagirl.setup_talk();
			}
		private:
			hmLib::signals::unique_connections SignalConnections;
			hmLib::inquiries::unique_connections InquiryConnections;

			void setDataMode(bool Val_){ DataModeFlagirl.set_request(Val_); }
			std::pair<double, double> getInOutTemperature()const{
				return std::make_pair(InTemperature, OutTemperature);
			}
		public:
			boost::signals2::signal<void(double rTemp, double outTemp, double inTemp, clock::time_point time)> signal_newData;
			//boost::signals2::signal<void(double rTemp, clock::time_point time)> signal_newTempTimeData;

			void slot_setDataMode(boost::signals2::signal<void(bool)>& Signal_){
				SignalConnections(hmLib::signals::connect(Signal_, [&](bool b)->void{this->setDataMode(b); }));
			}
			void slot_setDataMode(boost::signals2::signal<void(void)>& Signal_){
				SignalConnections(hmLib::signals::connect(Signal_, [&](void)->void{this->setDataMode(!DataModeFlagirl.request()); }));
			}
			void contact_getInOutTemperature(hmLib::inquiries::inquiry<std::pair<double, double>>& Inquiry_){
				InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->std::pair<double, double>{return this->getInOutTemperature(); }));
			}
			void contact_getTemperature(hmLib::inquiries::inquiry<double>& Inquiry_){
				InquiryConnections(hmLib::inquiries::connect(Inquiry_, Temperature));
			}
			void contact_getTime(hmLib::inquiries::inquiry<clock::time_point>& Inquiry_){
				InquiryConnections(hmLib::inquiries::connect(Inquiry_, Time));
			}
			void contact_getPicDataMode(hmLib::inquiries::inquiry<bool>& Inquiry_){
				InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->bool{return DataModeFlagirl.pic(); }));
			}
			void contact_getRequestDataMode(hmLib::inquiries::inquiry<bool>& Inquiry_){
				InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->bool{return DataModeFlagirl.request(); }));
			}
		};
	}
}

#
#endif