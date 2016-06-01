#ifndef HMR_THERMO_INC
#define HMR_THERMO_INC 104
#
/*===hmrThermo===
hmrThermo v1_04/140320 amby
	温度データのバッファ機能に対応
hmrThermo v1_03/130720 amby
	温度データの受け渡し方法をinquiryからsignalに変更
hmrThermo v1_02/130602 iwahori
	キーボード・パッド用のslot_setDataMode()を追加(引数がvoid(void)になっている)
hmrThermo v1_01/130414 iwahori
	温度データの受け渡し方法をsignal発報からinquiryに変更
hmrThermo v1_00/130414 iwahori
	cThermoMsgAgentのsignal-slotへの対応完了
*/
#include "hmLibVer.hpp"
#include<boost/signals2.hpp>
#include<hmLib_v3_06/signals.hpp>
#include <hmLib_v3_06/inquiries.hpp>
#include "hmrItfMessage.hpp"
#include "hmrFlagirl.hpp"

namespace hmr{
//#define D_ADMaxValue 4096
	class cThermoMsgAgent:public itfMessageAgent{
	private:
		flagirl DataModeFlagirl;
		flagirl LogModeFlagirl;
		double Temperature;
		double logTemperature;

		hmLib_uint16 logRawTemperature;
		hmLib_uint16 RawTemperature;

		clock::time_point Time;
		clock::time_point logTime;
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
					RawTemperature = static_cast<hmLib_uint16>(Data_.at(1)) + static_cast<hmLib_uint16>(Data_.at(2)) * 256;
					Time=Time_;

					// signal 発信
					signal_newData(Temperature, Time);
					signal_newRawData(static_cast<double>(RawTemperature), logTime);
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
				else if (static_cast<unsigned char>(Data_[0]) == 0xC0){
					if (Data_.size() != 1)return true;
					LogModeFlagirl.set_pic(true);
					return false;
				}
				else if (static_cast<unsigned char>(Data_[0]) == 0xC1){
					if (Data_.size() != 1)return true;
					LogModeFlagirl.set_pic(false);
					return false;
				}
				else if (static_cast<unsigned char>(Data_[0]) == 0xD0){
					if (Data_.size() != 7)return true;
					logTemperature = toTemperature(static_cast<unsigned char>(Data_.at(1)), static_cast<unsigned char>(Data_.at(2)));

					logRawTemperature = 
						static_cast<hmLib_uint16>((hmLib_uint8)Data_.at(1))
						+ static_cast<hmLib_uint16>((hmLib_uint8)Data_.at(2)) * 256;
					
					// 現在時刻取得
					hmLib_sint32 TimeSec = 
						static_cast<hmLib_uint32>((hmLib_uint8)Data_.at(3)) 
						+ static_cast<hmLib_uint32>((hmLib_uint8)Data_.at(4)) * 256
						+ static_cast<hmLib_uint32>((hmLib_uint8)Data_.at(5)) * 256 * 256 
						+ static_cast<hmLib_uint32>((hmLib_uint8)Data_.at(6)) * 256 * 256 * 256;
					// time_point に変換
					logTime = std::chrono::system_clock::from_time_t(TimeSec);
					
					signal_newLogData(logTemperature, logTime);
					signal_newLogRawData(static_cast<double>(logRawTemperature), logTime);
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
			else if (LogModeFlagirl.talk()){
				if (LogModeFlagirl.request()) Str.push_back(static_cast<unsigned char>(0xC0));
				else Str.push_back(static_cast<unsigned char>(0xC1));
				return false;
			}

			return true;
		}
		void setup_talk(void)override{
			DataModeFlagirl.setup_talk();
			LogModeFlagirl.setup_talk();
		}
	private:
		hmLib::signals::unique_connections SignalConnections;
		hmLib::inquiries::unique_connections InquiryConnections;
		void setDataMode(bool Val_){DataModeFlagirl.set_request(Val_);}
		double getTemperature()const{return Temperature;}
	public:
		boost::signals2::signal<void(double data, clock::time_point time)> signal_newData;
		boost::signals2::signal<void(double data, clock::time_point time)> signal_newLogData;
		boost::signals2::signal<void(double data, clock::time_point time)> signal_newRawData;
		boost::signals2::signal<void(double data, clock::time_point time)> signal_newLogRawData;

		void contact_getTemperature(hmLib::inquiries::inquiry<double>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->double{return this->getTemperature();}));
		}
		void contact_getLogTemperature(hmLib::inquiries::inquiry<double>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_, logTemperature));
		}

		void contact_getRawTemperature(hmLib::inquiries::inquiry<hmLib_uint16>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_, RawTemperature));
		}
		void contact_getLogRawTemperature(hmLib::inquiries::inquiry<hmLib_uint16>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_, logRawTemperature));
		}

		void contact_getPicDataMode(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->bool{return this->DataModeFlagirl.pic();}));
		}
		void contact_getRequestDataMode(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->bool{return this->DataModeFlagirl.request();}));
		}
		void contact_getPicLogMode(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->bool{return this->LogModeFlagirl.pic(); }));
		}
		void contact_getRequestLogMode(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->bool{return this->LogModeFlagirl.request(); }));
		}

		void contact_getTime(hmLib::inquiries::inquiry<clock::time_point>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,Time));
		}
		void contact_getlogTime(hmLib::inquiries::inquiry<clock::time_point>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_, logTime));
		}

		void slot_setDataMode(boost::signals2::signal<void(bool)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](bool Flag)->void{this->setDataMode(Flag);}));	
		}
		void slot_setLogMode(boost::signals2::signal<void(bool)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_, [&](bool Flag)->void{LogModeFlagirl.set_request(Flag); }));
		}

		void slot_setDataMode(boost::signals2::signal<void(void)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](void)->void{this->setDataMode(!(DataModeFlagirl.request()));}));	
		}
	public:
		cThermoMsgAgent():
			Temperature(0.),
			Time(),
			DataModeFlagirl(), 
			LogModeFlagirl(){
		}
	};
}

#
#endif
