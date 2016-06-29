#ifndef HMR_SHT75_INC
#define HMR_SHT75_INC 100
#
/*
===hmrSHT75===
v1_00/131026 hmIto
	基本機能を作成
*/
#include "hmLibVer.hpp"
#include<boost/signals2.hpp>
#include<hmLib/signals.hpp>
#include <hmLib/inquiries.hpp>
#include <hmrVLib/Message/ItfMessageAgent.hpp>
#include "hmrFlagirl.hpp"

namespace hmr{
	class cSHT75MsgAgent:public itfMessageAgent{
	private:
		flagirl DataModeFlagirl;
		flagirl LowAccFlagirl;
		bool ResetRequest;
		double Temperature;
		double Humidity;
		clock::time_point Time;
	public:
		static double toTemperature(unsigned char Low, unsigned char High,bool LowAcc){
			unsigned int Val=0;

			Val=High&(LowAcc?0x3F:0x0F);
			Val<<=8;
			Val=Low;

			return -39.7+Val*(LowAcc?0.04:0.01);
		}
		static double toHumidity(unsigned char Low, unsigned char High,bool LowAcc){
			unsigned int Val=0;

			Val=High&(LowAcc?0x0F:0x00);
			Val<<=8;
			Val=Low;

			return -2.0468+Val*(LowAcc?0.5872:0.0367)+Val*Val*(LowAcc?0.00040845:0.0000015955);
		}
		bool listen(datum::time_point Time_, bool Err_,const std::string& Data_)override{
				if(Data_.size()==0)return true;

				if(static_cast<unsigned char>(Data_[0])==0x00){
					if(Data_.size()!=3)return true;
					//データを温度に変換
					Temperature=toTemperature(static_cast<unsigned char>(Data_[1]),static_cast<unsigned char>(Data_[2]),false);
					Humidity=toHumidity(static_cast<unsigned char>(Data_[3]),static_cast<unsigned char>(Data_[4]),false);
					Time=Time_;
					// signal 発信
					signal_newData(Temperature, Humidity, Time);
					return false;
				}else if(static_cast<unsigned char>(Data_[0])==0x01){
					if(Data_.size()!=3)return true;
					//データを温度に変換
					Temperature=toTemperature(static_cast<unsigned char>(Data_[1]),static_cast<unsigned char>(Data_[2]),true);
					Humidity=toHumidity(static_cast<unsigned char>(Data_[3]),0,true);
					Time=Time_;
					// signal 発信
					signal_newData(Temperature, Humidity, Time);
					return false;
				}else if(static_cast<unsigned char>(Data_[0])==0x10){
					if(Data_.size()!=1)return true;
					DataModeFlagirl.set_pic(true);
					return false;
				}else if(static_cast<unsigned char>(Data_[0])==0x11){
					if(Data_.size()!=1)return true;
					DataModeFlagirl.set_pic(false);
					return false;
				}else if(static_cast<unsigned char>(Data_[0])==0x20){
					if(Data_.size()!=1)return true;
					LowAccFlagirl.set_pic(true);
					return false;
				}else if(static_cast<unsigned char>(Data_[0])==0x21){
					if(Data_.size()!=1)return true;
					LowAccFlagirl.set_pic(false);
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
			if(LowAccFlagirl.talk()){
				if(LowAccFlagirl.request())Str.push_back(static_cast<unsigned char>(0x20));
				else Str.push_back(static_cast<unsigned char>(0x21));
				return false;
			}
			if(ResetRequest){
				Str.push_back(static_cast<unsigned char>(0xE0));
				ResetRequest=false;
				return false;
			}
			return true;
		}
		void setup_talk(void)override{
			DataModeFlagirl.setup_talk();
			LowAccFlagirl.setup_talk();
		}
	private:
		hmLib::signals::unique_connections SignalConnections;
		hmLib::inquiries::unique_connections InquiryConnections;
		void setDataMode(bool Val_){DataModeFlagirl.set_request(Val_);}
		void setLowAcc(bool Val_){LowAccFlagirl.set_request(Val_);}
		void requestReset(void){ResetRequest=true;}
		double getHumidity()const{return Temperature;}
		double getTemperature()const{return Temperature;}
	public:
		boost::signals2::signal<void(double Thermo, double Humid, clock::time_point Time)> signal_newData;
		void contact_getTemperature(hmLib::inquiries::inquiry<double>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->double{return this->getTemperature();}));
		}
		void contact_getPicDataMode(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->bool{return this->DataModeFlagirl.pic();}));
		}
		void contact_getRequestDataMode(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->bool{return this->DataModeFlagirl.request();}));
		}
		void contact_getPicLowAcc(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->bool{return this->LowAccFlagirl.pic();}));
		}
		void contact_getRequestLowAcc(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->bool{return this->LowAccFlagirl.request();}));
		}
		void contact_getTime(hmLib::inquiries::inquiry<clock::time_point>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,Time));
		}
		void contact_getRequestReset(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,ResetRequest));
		}
		void slot_setDataMode(boost::signals2::signal<void(bool)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](bool Flag)->void{this->setDataMode(Flag);}));	
		}
		void slot_setDataMode(boost::signals2::signal<void(void)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](void)->void{this->setDataMode(!(DataModeFlagirl.request()));}));	
		}
		void slot_setLowAcc(boost::signals2::signal<void(bool)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](bool Flag)->void{this->setLowAcc(Flag);}));	
		}
		void slot_setLowAcc(boost::signals2::signal<void(void)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](void)->void{this->setLowAcc(!(LowAccFlagirl.request()));}));	
		}
		void slot_requestReset(boost::signals2::signal<void(void)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](void)->void{this->requestReset();}));	
		}
	public:
		cSHT75MsgAgent():
			Temperature(0.),
			Time(),
			DataModeFlagirl(),
			LowAccFlagirl(){
		}
	};
}

#
#endif
