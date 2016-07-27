#ifndef HMR_CO2_INC
#define HMR_CO2_INC 101
#
/*======hmrCO2=======
hmrCO2 v1_01/130602 iwahori
	キーボード・パッド用のslot_setDataMode()を追加(引数がvoid(void)になっている)
hmrCO2 v1_00/130427 iwahori
	cCO2MsgAgentのsignal-slot,inquiry-contactへの対応完了
*/

#include "hmLibVer.hpp"
#include<cstdint>
#include<boost/signals2.hpp>
#include<hmLib/signals.hpp>
#include <hmLib/inquiries.hpp>
#include <HomuraViewer/Message/ItfMessageAgent.hpp>
#include "hmrFlagirl.hpp"

//#include <hmLib/hmTime.hpp>
//#include <hmLib/dxArea.hpp>
//#include <cmath>
//#include "hmr.h"
//#include "hmrCom.h"
//#include "hmrBufCtr.hpp"
//#include "hmrComCtr.hpp"
//#include "hmrPad.hpp"
//#include "hmrUniFileType.hpp"
//#include "hmrUniDraw.h"
//#include "hmrDataBaseType.hpp"

namespace hmr{
	namespace viewer{
		class cCO2MsgAgent :public itfMessageAgent{
		private:
			double Value;
			clock::time_point Time;

			double LogValue;
			clock::time_point LogTime;

			flagirl DataModeFlagirl;
			flagirl PumpPWFlagirl;
			flagirl SensorPWFlagirl;
			flagirl LogModeFlagirl;
			static constexpr double D_ADMaxValue = 4096.;
		public:
			static double toCO2(std::uint16_t Bytes){
				std::uint16_t tmp = static_cast<std::uint16_t>(Bytes);
				return ((static_cast<double>(tmp) / D_ADMaxValue*2.5) - 0.5)*1000.;
			}
			bool listen(datum::time_point Time_, bool Err_, const std::string& Str_)override{

				// data getting mode
				if(static_cast<unsigned char>(Str_[0]) == 0x00){
					if(Str_.size() != 3)return true;

					std::uint16_t tmp = static_cast<std::uint16_t>(Str_[1]) + (static_cast<std::uint16_t>(Str_[2]) << 8);

					Value = toCO2(tmp);
					Time = Time_;
					signal_newData(Value, Time);
					signal_newRawData(Value, tmp, Time);
					//		Ref.insert(Datum(_time,_errFlag,static_cast<unsigned char>(Str_[2])*256+static_cast<unsigned char>(Str_[1])));
					return false;
				}

				// Change Data getting mode
				else if(static_cast<unsigned char>(Str_[0]) == 0x10){
					if(Str_.size() != 1)return true;
					DataModeFlagirl.set_pic(true);
					return false;
				} else if(static_cast<unsigned char>(Str_[0]) == 0x11){
					if(Str_.size() != 1)return true;
					DataModeFlagirl.set_pic(false);
					return false;
				}

				// Change sensor power status
				else if(static_cast<unsigned char>(Str_[0]) == 0x20){
					if(Str_.size() != 1)return true;
					SensorPWFlagirl.set_pic(true);
					return false;
				} else if(static_cast<unsigned char>(Str_[0]) == 0x21){
					if(Str_.size() != 1)return true;
					SensorPWFlagirl.set_pic(false);
					return false;
				}

				// Change pump power status
				else if(static_cast<unsigned char>(Str_[0]) == 0x30){
					if(Str_.size() != 1)return true;
					PumpPWFlagirl.set_pic(true);
					return false;
				} else if(static_cast<unsigned char>(Str_[0]) == 0x31){
					if(Str_.size() != 1)return true;
					PumpPWFlagirl.set_pic(false);
					return false;
				}

				// change Log Mode
				else if(static_cast<unsigned char>(Str_[0]) == 0xC0){
					if(Str_.size() != 1)return true;
					LogModeFlagirl.set_pic(true);
					return false;
				} else if(static_cast<unsigned char>(Str_[0]) == 0xC1){
					if(Str_.size() != 1)return true;
					LogModeFlagirl.set_pic(false);
					return false;
				}

				// getting log data
				if(static_cast<unsigned char>(Str_[0]) == 0xD0){
					if(Str_.size() != 7)return true;
					// data取得
					std::uint16_t tmp = static_cast<hmLib_uint32>(static_cast<unsigned char>(Str_.at(1)))
						+ static_cast<hmLib_uint32>(static_cast<unsigned char>(Str_.at(2))) * 256;
					double LogValue = toCO2(tmp);


					// 現在時刻取得
					hmLib_sint32 TimeSec = static_cast<hmLib_sint32>(
						static_cast<hmLib_uint32>(static_cast<unsigned char>(Str_.at(3)))
						+ static_cast<hmLib_uint32>(static_cast<unsigned char>(Str_.at(4))) * 256
						+ static_cast<hmLib_uint32>(static_cast<unsigned char>(Str_.at(5))) * 256 * 256
						+ static_cast<hmLib_uint32>(static_cast<unsigned char>(Str_.at(6))) * 256 * 256 * 256
						);
					// time_point に変換
					LogTime = std::chrono::system_clock::from_time_t(TimeSec);

					signal_newLogData(LogValue, LogTime);
					signal_newLogRawData(LogValue, tmp, Time);
					//		Ref.insert(Datum(_time,_errFlag,static_cast<unsigned char>(Str_[2])*256+static_cast<unsigned char>(Str_[1])));
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
				} else if(SensorPWFlagirl.talk()){
					if(SensorPWFlagirl.request())Str.push_back(static_cast<unsigned char>(0x20));
					else Str.push_back(static_cast<unsigned char>(0x21));
					return false;
				} else if(PumpPWFlagirl.talk()){
					if(PumpPWFlagirl.request())Str.push_back(static_cast<unsigned char>(0x30));
					else Str.push_back(static_cast<unsigned char>(0x31));
					return false;
				} else if(LogModeFlagirl.talk()){
					if(LogModeFlagirl.request())Str.push_back(static_cast<unsigned char>(0xC0));
					else Str.push_back(static_cast<unsigned char>(0xC1));
					return false;
				}

				return true;
			}
			void setup_talk(void)override{
				DataModeFlagirl.setup_talk();
				LogModeFlagirl.setup_talk();

				SensorPWFlagirl.setup_talk();
				PumpPWFlagirl.setup_talk();
				LogModeFlagirl.setup_talk();
			}
		private:
			hmLib::signals::unique_connections SignalConnections;
			hmLib::inquiries::unique_connections InquiryConnections;

			void setDataMode(bool Mode_){ DataModeFlagirl.set_request(Mode_); }
			void setPumpPW(bool Mode_){ PumpPWFlagirl.set_request(Mode_); }
			void setSensorPW(bool Mode_){ SensorPWFlagirl.set_request(Mode_); }
			double getValue(void)const{ return Value; }
		public:
			boost::signals2::signal<void(double data, clock::time_point time)> signal_newData;
			boost::signals2::signal<void(double data, clock::time_point time)> signal_newLogData;
			boost::signals2::signal<void(double data, std::uint16_t, clock::time_point time)> signal_newRawData;
			boost::signals2::signal<void(double data, std::uint16_t, clock::time_point time)> signal_newLogRawData;

			void slot_setDataMode(boost::signals2::signal<void(bool)>& Signal_){
				SignalConnections(hmLib::signals::connect(Signal_, [&](bool Flag)->void{this->setDataMode(Flag); }));
			}
			void slot_setDataMode(boost::signals2::signal<void(void)>& Signal_){
				SignalConnections(hmLib::signals::connect(Signal_, [&](void)->void{this->setDataMode(!DataModeFlagirl.request()); }));
			}

			void slot_setSensorPW(boost::signals2::signal<void(bool)>& Signal_){
				SignalConnections(hmLib::signals::connect(Signal_, [&](bool Flag)->void{this->setSensorPW(Flag); }));
			}
			void slot_setSensorPW(boost::signals2::signal<void(void)>& Signal_){
				SignalConnections(hmLib::signals::connect(Signal_, [&](void)->void{this->setSensorPW(!SensorPWFlagirl.request()); }));
			}

			void slot_setPumpPW(boost::signals2::signal<void(bool)>& Signal_){
				SignalConnections(hmLib::signals::connect(Signal_, [&](bool Flag)->void{this->setPumpPW(Flag); }));
			}
			void slot_setPumpPW(boost::signals2::signal<void(void)>& Signal_){
				SignalConnections(hmLib::signals::connect(Signal_, [&](void)->void{this->setPumpPW(!PumpPWFlagirl.request()); }));
			}

			void slot_setLogMode(boost::signals2::signal<void(bool)>& Signal_){
				SignalConnections(hmLib::signals::connect(Signal_, [&](bool flag)->void{this->LogModeFlagirl.set_request(flag); }));
			}

			void contact_getValue(hmLib::inquiries::inquiry<double>& Inquiry_){
				InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->double{return this->getValue(); }));
			}
			void contact_getTime(hmLib::inquiries::inquiry<clock::time_point>& Inquiry_){
				InquiryConnections(hmLib::inquiries::connect(Inquiry_, Time));
			}
			void contact_getLogValue(hmLib::inquiries::inquiry<double>& Inquiry_);
			void contact_getLogTime(hmLib::inquiries::inquiry<clock::time_point>& Inquiry_);

			void contact_getPicDataMode(hmLib::inquiries::inquiry<bool>& Inquiry_){
				InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->bool{return this->DataModeFlagirl.pic(); }));
			}
			void contact_getPicPumpPW(hmLib::inquiries::inquiry<bool>& Inquiry_){
				InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->bool{return this->PumpPWFlagirl.pic(); }));
			}
			void contact_getPicSensorPW(hmLib::inquiries::inquiry<bool>& Inquiry_){
				InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->bool{return this->SensorPWFlagirl.pic(); }));
			}
			void contact_getPicLogMode(hmLib::inquiries::inquiry<bool>& Inquiry_){
				InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->bool{return this->LogModeFlagirl.pic(); }));
			}

			void contact_getRequestDataMode(hmLib::inquiries::inquiry<bool>& Inquiry_){
				InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->bool{return this->DataModeFlagirl.request(); }));
			}
			void contact_getRequestPumpPW(hmLib::inquiries::inquiry<bool>& Inquiry_){
				InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->bool{return this->PumpPWFlagirl.request(); }));
			}
			void contact_getRequestSensorPW(hmLib::inquiries::inquiry<bool>& Inquiry_){
				InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->bool{return this->SensorPWFlagirl.request(); }));
			}
			void contact_getRequestLogMode(hmLib::inquiries::inquiry<bool>& Inquiry_){
				InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->bool{return this->LogModeFlagirl.request(); }));
			}


		public:
			cCO2MsgAgent() :
				Value(0.), LogValue(0.),
				Time(), LogTime(),
				DataModeFlagirl(),
				PumpPWFlagirl(),
				SensorPWFlagirl(),
				LogModeFlagirl(){}
		};

	}
}

#
#endif