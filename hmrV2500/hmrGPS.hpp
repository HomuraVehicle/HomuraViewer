#ifndef HMR_GPS_INC
#define HMR_GPS_INC 104
#

/*===========hmrGPS===========
hmrGPS v1_04/141004 hmIto
	Power制御機能を追加
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
#include<hmLib_v3_05/signals.hpp>
#include <hmLib_v3_05/inquiries.hpp>
#include "hmrItfMessage.hpp"
#include "hmrFlagirl.hpp"
#include "hmrGPSDataSet.hpp"

namespace hmr{
	class cGPSMsgAgent
		
		:public itfMessageAgent{
	private:
		flagirl DataModeFlagirl;
		flagirl SwapStateFlagirl;
		flagirl PowerFlagirl;
		GPSDataSet GPSData;
		unsigned char gpsCh;
		static GPSDataSet toGPSDataSet(const clock::time_point& Time_,bool errFlag_,const std::string& data_){
			if(data_.size()==21 && !errFlag_){
				int UseGPS;
				GPSDataSet::pdouble	pos;
				double  height;
				bool	failFlag;
				int UTC_H=(int)((unsigned char)data_[1]);
				int UTC_M=(int)((unsigned char)data_[2]);
				int UTC_S=(int)((unsigned char)data_[3]);
				int UTC_MS=(int)((unsigned char)data_[4])*10+(int)(((unsigned char)data_[5])/10);

				//時間補正	
//				date cDate = hm::time()-hmTime(9*60*60,0);	
				date cDate;
				cDate.from_time(clock::now()-std::chrono::hours(9));
				if(cDate.Hour>=0 && cDate.Hour<1){
					if(UTC_H < 24 && UTC_H >= 23){
						cDate.Day -= 1;
					}
				}else if(cDate.Hour<24 && cDate.Hour>=23){
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
				pos.y =(unsigned char)data_[6]+(((unsigned char)data_[7])/1.+((unsigned char)data_[8])/100.+((unsigned char)data_[9])/10000.)/60.;
				if(data_[10]=='S') pos.y*=-1.;
				pos.x =(unsigned char)data_[11]+(((unsigned char)data_[12])/1.+((unsigned char)data_[13])/100.+((unsigned char)data_[14])/10000.)/60.;
				if(data_[15]=='W')pos.x*=-1.;
				//failフラグ
				failFlag=(data_[16]==0);
				//高さ
				height=data_[17]*1000.+data_[18]*10.+data_[19]*0.1;
				//使用GPS
				UseGPS=data_[20];

				return GPSDataSet(pos,height,Time_, cDate.to_time(), failFlag, errFlag_,UseGPS);
			}else{
				date Date;
				return GPSDataSet(GPSDataSet::pdouble(0.,0.),0.,Time_,Date.to_time(),false,true,0);
			}			
		}
	public:
		bool listen(datum::time_point Time_,bool errFlag_,const std::string& Data_)override{
			if(Data_.size()==0)return true;

			if(static_cast<unsigned char>(Data_[0])==0x00){
				if(Data_.size()!=21)return true;
				GPSData=toGPSDataSet(Time_,errFlag_,Data_);
				signal_newdata(GPSData, Time_);
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
				if(Data_.size()!=2)return true;
				gpsCh = static_cast<unsigned char>(Data_[1]);
				if(gpsCh == 0x00){ SwapStateFlagirl.set_pic(false); // 更新する 
				}else{SwapStateFlagirl.set_pic(true);}
				return false;
			}else if(static_cast<unsigned char>(Data_[0])==0x30){
				if(Data_.size()!=1)return true;
				PowerFlagirl.set_pic(true);
				return false;
			}else if(static_cast<unsigned char>(Data_[0])==0x31){
				if(Data_.size()!=1)return true;
				PowerFlagirl.set_pic(false);
				return false;
			}
			return true;		
		}
		bool talk(std::string& Str)override{
			Str="";
			if(DataModeFlagirl.talk()){
				if(DataModeFlagirl.request())Str=static_cast<unsigned char>(0x10);
				else Str=static_cast<unsigned char>(0x11);
				return false;
			}else if(PowerFlagirl.talk()){
				if(PowerFlagirl.request())Str=static_cast<unsigned char>(0x30);
				else Str=static_cast<unsigned char>(0x31);
				return false;
			}else if (SwapStateFlagirl.talk()){
				unsigned char char_;
				if(SwapStateFlagirl.request()){ char_ = static_cast<unsigned char>(0x01);}
				else{ char_ = static_cast<unsigned char>(0x00);}//
				
				Str=static_cast<unsigned char>(0x20);
				Str.push_back(char_);
				return false;
			}
			return true;
		}
		void setup_talk(void)override{
			DataModeFlagirl.setup_talk();
			PowerFlagirl.setup_talk();
			SwapStateFlagirl.setup_talk();
		}
	private:
		hmLib::signals::unique_connections SignalConnections;
		hmLib::inquiries::unique_connections InquiryConnections;

		void setDataMode(bool Mode_){DataModeFlagirl.set_request(Mode_);}
		void setPower(bool Mode_){PowerFlagirl.set_request(Mode_);}
		void setSwapState(bool Mode_){SwapStateFlagirl.set_request(Mode_);}

		GPSDataSet getGPSData()const{return GPSData;}

		bool getPicDataMode(){return DataModeFlagirl.pic();}
		bool getRequestDataMode(){return DataModeFlagirl.request();}

		bool getPicPower(){return PowerFlagirl.pic();}
		bool getRequestPower(){return PowerFlagirl.request();}

		bool getPicSwapState(){return SwapStateFlagirl.pic();}
		bool getRequestSwapState(){return SwapStateFlagirl.request();}

	public:
		boost::signals2::signal<void(const GPSDataSet& Data_, clock::time_point time)> signal_newdata;
		void contact_getGPSData(hmLib::inquiries::inquiry<GPSDataSet>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->GPSDataSet{return this->getGPSData();}));
		}

		void contact_getPicDataMode(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->bool{return this->getPicDataMode();}));
		}
		void contact_getRequestDataMode(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->bool{return this->getRequestDataMode();}));
		}
		void slot_setDataMode(boost::signals2::signal<void(bool)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](bool Flag)->void{this->setDataMode(Flag);}));	
		}
		void slot_setDataMode(boost::signals2::signal<void(void)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](void)->void{this->setDataMode(!DataModeFlagirl.request());}));	
		}

		void contact_getPicPower(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->bool{return this->getPicPower();}));
		}
		void contact_getRequestPower(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->bool{return this->getRequestPower();}));
		}
		void slot_setPower(boost::signals2::signal<void(bool)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](bool Flag)->void{this->setPower(Flag);}));	
		}
		void slot_setPower(boost::signals2::signal<void(void)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](void)->void{this->setPower(!PowerFlagirl.request());}));	
		}

		void contact_getPicSwapState(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->bool{return this->getPicSwapState();}));
		}
		void contact_getRequestSwapState(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->bool{return this->getRequestSwapState();}));
		}
		void slot_setSwapState(boost::signals2::signal<void(bool)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](bool Flag)->void{this->setSwapState(Flag);}));	
		}
		void slot_setSwapState(boost::signals2::signal<void(void)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](void)->void{this->setSwapState(!SwapStateFlagirl.request());}));	
		}
		void contact_getGPSCh(hmLib::inquiries::inquiry<unsigned char>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_, gpsCh));
		}



	public:
		cGPSMsgAgent():
			DataModeFlagirl(), SwapStateFlagirl(), PowerFlagirl(),
			GPSData(), gpsCh(0){
			PowerFlagirl.set_pic(true);
			PowerFlagirl.set_request(true);
		}
	};
}
#
#endif
