#ifndef HMR_H2S_INC
#define HMR_H2S_INC 101
#

/*==========hmrH2S===========
hmrH2S v1_01/130602 iwahori
	キーボード・パッド用のslot_setDataMode()を追加(引数がvoid(void)になっている)
hmrGyro v1_00/130511 iwahori
	cH2SMsAgentのsignal-slot,inquiry-contactへの対応完了
*/

#include "hmLibVer.hpp"
#include<boost/signals2.hpp>
#include<hmLib_v3_05/signals.hpp>
#include <hmLib_v3_05/inquiries.hpp>
#include "hmrItfMessage.hpp"
#include "hmrFlagirl.hpp"
#include "hmrADC.h"

namespace hmr{
	class cH2SMsgAgent:public itfMessageAgent{
	private:
		flagirl DataModeFlagirl;
		double Value;
		clock::time_point Time;
//		static const double D_ADMaxValue;
	public:
		static double toValue(unsigned char LowByte,unsigned char HighByte){
			unsigned int tmp=static_cast<unsigned int>(LowByte+HighByte*256);
			return (static_cast<double>(tmp)/D_ADMaxValue*2.5)*1000.;
		}
		bool listen(datum::time_point Time_, bool Err_,const std::string& Data_)override{
				if(Data_.size()==0)return true;

				if(static_cast<unsigned char>(Data_[0])==0x00){
					if(Data_.size()!=3)return true;
					//データを温度に変換
					Value=toValue(static_cast<unsigned char>(Data_[1]),static_cast<unsigned char>(Data_[2]));
					Time=Time_;
					signal_newData(Value, Time);
					return false;
				}else if(static_cast<unsigned char>(Data_[0])==0x10){
					if(Data_.size()!=1)return true;
					DataModeFlagirl.set_pic(true);
					return false;
				}else if(static_cast<unsigned char>(Data_[0])==0x11){
					if(Data_.size()!=1)return true;
					DataModeFlagirl.set_pic(false);
					return false;
				}
				return true;
		}
		bool talk(std::string& Str)override{
			Str="";
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
		void setDataMode(bool Val_){DataModeFlagirl.set_request(Val_);}
	public:
		boost::signals2::signal<void(double data, clock::time_point time)> signal_newData;

		void slot_setDataMode(boost::signals2::signal<void(bool)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](bool Flag)->void{this->setDataMode(Flag);}));	
		}
		void slot_setDataMode(boost::signals2::signal<void(void)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](void)->void{this->setDataMode(!DataModeFlagirl.request());}));	
		}
		void contact_getPicDataMode(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->bool{return this->DataModeFlagirl.pic();}));
		}
		void contact_getRequestDataMode(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->bool{return this->DataModeFlagirl.request();}));
		}

		void contact_getValue(hmLib::inquiries::inquiry<double>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,Value));
		}
		void contact_getTime(hmLib::inquiries::inquiry<clock::time_point>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,Time));
		}
	public:
		cH2SMsgAgent():
			Value(0.),
			Time(),
			DataModeFlagirl(){
		}
	};
}
#
#endif