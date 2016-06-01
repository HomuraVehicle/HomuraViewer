#ifndef HMR_THERMO_INC
#define HMR_THERMO_INC 103
#
/*===hmrThermo===
v1_03/130720 amby
	温度データの受け渡し方法をinquiryからsignalに変更
v1_02/130602 iwahori
	キーボード・パッド用のslot_setDataMode()を追加(引数がvoid(void)になっている)
v1_01/130414 iwahori
	温度データの受け渡し方法をsignal発報からinquiryに変更
v1_00/130414 iwahori
	cThermoMsgAgentのsignal-slotへの対応完了
*/
#include "hmLibVer.hpp"
#include<boost/signals2.hpp>
#include<hmLib/signals.hpp>
#include <hmLib/inquiries.hpp>
#include "hmrItfMessage.hpp"
#include "hmrFlagirl.hpp"

namespace hmr{
//#define D_ADMaxValue 4096
	class cThermoMsgAgent:public itfMessageAgent{
	private:
		flagirl DataModeFlagirl;
		double Temperature;
		clock::time_point Time;
	public:
		static double toTemperature(unsigned char LowByte,unsigned char HighByte){
			unsigned int tmp=static_cast<unsigned int>(LowByte+HighByte*256);
	//				return static_cast<double>(tmp);
	//		return (tmp*2*4096./4096-1069.3)/39.173;
			return (tmp*2*4096./4096-1171.7)/36.486;	//131104 気温＋ひと肌で補正
		}
		bool listen(datum::time_point Time_, bool Err_,const std::string& Data_)override{
				if(Data_.size()==0)return true;

				if(static_cast<unsigned char>(Data_[0])==0x00){
					if(Data_.size()!=3)return true;
					//データを温度に変換
					Temperature=toTemperature(static_cast<unsigned char>(Data_[1]),static_cast<unsigned char>(Data_[2]));
					Time=Time_;
					// signal 発信
					signal_newData(Temperature, Time);
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
		double getTemperature()const{return Temperature;}
	public:
		boost::signals2::signal<void(double data, clock::time_point time)> signal_newData;

		void contact_getTemperature(hmLib::inquiries::inquiry<double>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->double{return this->getTemperature();}));
		}
		void contact_getPicDataMode(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->bool{return this->DataModeFlagirl.pic();}));
		}
		void contact_getRequestDataMode(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->bool{return this->DataModeFlagirl.request();}));
		}
		void contact_getTime(hmLib::inquiries::inquiry<clock::time_point>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,Time));
		}
		void slot_setDataMode(boost::signals2::signal<void(bool)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](bool Flag)->void{this->setDataMode(Flag);}));	
		}
		void slot_setDataMode(boost::signals2::signal<void(void)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](void)->void{this->setDataMode(!(DataModeFlagirl.request()));}));	
		}
	public:
		cThermoMsgAgent():
			Temperature(0.),
			Time(),
			DataModeFlagirl(){
		}
	};
}

#
#endif