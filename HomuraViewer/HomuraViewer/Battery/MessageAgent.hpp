#ifndef HMR_BATTERY_INC
#define HMR_BATTERY_INC 102
#

/*=======hmrBattery=========
hmrBattery v1_02/130602 iwahori
	キーボード・パッド用のslot_setDataMode()を追加(引数がvoid(void)になっている)
hmrBattery v1_01/130519 iwahori
	BatteryDataSet廃止．std::arrayでデータを保持
hmrBattery v1_00/130427 iwahori
	cBatteryMsgAgentのsignal-slot,inquiry-contactへの対応完了
*/

#include "hmLibVer.hpp"
#include <array>
#include <iterator>
#include <boost/signals2.hpp>
#include <hmLib/signals.hpp>
#include <hmLib/inquiries.hpp>
#include <HomuraViewer/Message/ItfMessageAgent.hpp>
#include "../hmrFlagirl.hpp"
#include "../hmrADC.h"
//#include "hmrBatteryDataSet.hpp"
//#include "hmr.h"

//#include "hmrAnalog.hpp"
//#include <hmLib/dxArea.hpp>

//#define D_ADMaxValue 4096

namespace hmr{
	template<unsigned int BatteryNum_>
	class cBatteryMsgAgent:public itfMessageAgent{
	private:
//		static const double D_ADMaxValue;
//		static const unsigned char BatteryNum;
		std::array<double,BatteryNum_> BatteryData;
//		BatteryDataSet<HMR_BATTERY_NUM> BatteryData;
		clock::time_point Time;
		flagirl DataModeFlagirl;
		static double toVoltage(unsigned char LowByte,unsigned char HighByte){
			unsigned int tmp=static_cast<unsigned int>(LowByte+HighByte*256);
			return (static_cast<double>(tmp)/D_ADMaxValue)*10.7;
		}
	public:
		bool listen(datum::time_point Time_, bool Err_,const std::string& Str_)override{
			if(Str_.size() == 0) return true;

			//nulldataか確認する(debug用)
			for(auto itr=Str_.begin();itr!=Str_.end();++itr){
				if(*itr!=0x00)break;
				if(std::next(itr)==Str_.end())signal_nulldata(hmr::clock::now());
			}

			// Batt read ON
			if(static_cast<unsigned char>(Str_[0])==0x10){
				if(Str_.size() != 1) return true;
				DataModeFlagirl.set_pic(true);
				return false;
			// Batt read OFF
			}else if(static_cast<unsigned char>(Str_[0])==0x11){
				if(Str_.size() != 1) return true;
				DataModeFlagirl.set_pic(false);
				return false;
			// Batt data coming
			}else if(static_cast<unsigned char>(Str_[0])==0x00){
				if(Str_.size() != BatteryNum_ * 2 + 1) return true;
				Time=Time_;
				unsigned char i=0;
				for(auto Itr=BatteryData.begin();Itr!=BatteryData.end();++Itr){
					(*Itr)=toVoltage(static_cast<unsigned char>(Str_[2*i+1]),static_cast<unsigned char>(Str_[2*i+2]));
					signal_newdata[i](Time, *Itr);	//シグナル発信
					++i;
					if(i>=BatteryNum_)break;	//念のため
				}
//				Set.insert(toDatum(_time,_errFlag,Str_));
				return false;
			}
			return true;
		}
		bool talk(std::string& Str)override{
			Str="";
			if(DataModeFlagirl.talk()){
				if(DataModeFlagirl.request()) Str.push_back(static_cast<unsigned char>(0x10));
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
	public:
		boost::signals2::signal<void(hmr::clock::time_point)> signal_nulldata;
		boost::signals2::signal<void(clock::time_point, double)> signal_newdata[BatteryNum_];
		void slot_setDataMode(boost::signals2::signal<void(bool)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](bool Flag)->void{DataModeFlagirl.set_request(Flag);}));				
		}
		void slot_setDataMode(boost::signals2::signal<void(void)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](void)->void{DataModeFlagirl.set_request(!DataModeFlagirl.request());}));				
		}
		void contact_getPicDataMode(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->bool{return this->DataModeFlagirl.pic();}));
		}
		void contact_getRequestDataMode(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->bool{return this->DataModeFlagirl.request();}));
		}
		void contact_getBatteryData(unsigned int BatteryNo_,hmLib::inquiries::inquiry<double>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,BatteryData[BatteryNo_]));
		}
		void contact_getTime(hmLib::inquiries::inquiry<clock::time_point>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,Time));
		}
	public:
		cBatteryMsgAgent():
			BatteryData(),
			Time(),
			DataModeFlagirl(){
		}
	};
}


#
#endif
