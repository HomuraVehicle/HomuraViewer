#ifndef HMR_GYRO_INC
#define HMR_GYRO_INC 101
#
/*=============hmrGyro==============
hmrGyro v1_01/130602 iwahori
	キーボード・パッド用のslot_setDataMode()を追加(引数がvoid(void)になっている)
hmrGyro v1_00/130511 iwahori
	cAcceleMsAgentのsignal-slot,inquiry-contactへの対応完了
*/
#include "hmLibVer.hpp"
#include<boost/signals2.hpp>
#include<hmLib_v3_06/signals.hpp>
#include <hmLib_v3_06/inquiries.hpp>
#include "hmrItfMessage.hpp"
#include "hmrFlagirl.hpp"
#include "coordinates.hpp"

namespace hmr{
	class cGyroMsgAgent:public itfMessageAgent{
		typedef hmLib::coordinates3D::angle angle;
	private:
		angle GyroData;
		double dt;							//測定時間 (角速度に掛けることで回転角度になる)
		clock::time_point Time;
		angle CorrectionValue;				//補正値
		bool CorrectionFlag;				//true:補正値取得中
		angle TmpCorrectionValue;
		double TmpCorrectionDt;
		flagirl DataModeFlagirl;
		void toData(const std::string& Str_){
			angle Agl;
			if(Str_.size()!=16){
				GyroData=Agl;
				return;
			}

			unsigned int Num=(static_cast<unsigned char>(Str_[2])+0x100*static_cast<unsigned char>(Str_[3]));
			Agl.roll=(static_cast<unsigned char>(Str_[4])+0x100*static_cast<unsigned char>(Str_[5])+0x10000*static_cast<unsigned char>(Str_[6])+0x1000000*static_cast<unsigned char>(Str_[7]))*250./0x10000/Num*2;
			Agl.pitch=(static_cast<unsigned char>(Str_[8])+0x100*static_cast<unsigned char>(Str_[9])+0x10000*static_cast<unsigned char>(Str_[10])+0x1000000*static_cast<unsigned char>(Str_[11]))*250./0x10000/Num*2;
			Agl.yaw=(static_cast<unsigned char>(Str_[12])+0x100*static_cast<unsigned char>(Str_[13])+0x10000*static_cast<unsigned char>(Str_[14])+0x1000000*static_cast<unsigned char>(Str_[15]))*250./0x10000/Num*2;

			dt=Num/100.;
			GyroData=Agl/360*2*hmLib::pi()-CorrectionValue*dt;
			signal_newdata(GyroData, dt, Time);
		}
	public:
		cGyroMsgAgent()
			:GyroData()
			,dt(0.)
			,Time()
			,CorrectionValue()
			,CorrectionFlag(false)
			,TmpCorrectionValue()
			,TmpCorrectionDt(0.)
			,DataModeFlagirl(){
		}
		bool listen(datum::time_point Time_, bool Err_,const std::string& Str_)override{
			if(Str_.size()==0)return true;

			if(static_cast<unsigned char>(Str_[0])==0x00){
				if(Str_.size()!=16)return true;
				Time = Time_;
				toData(Str_);
				if(CorrectionFlag){
					TmpCorrectionValue+=GyroData;
					TmpCorrectionDt+=dt;
				}
				return false;
			}else if(static_cast<unsigned char>(Str_[0])==0x10){
				if(Str_.size()!=1)return true;
				DataModeFlagirl.set_pic(true);
				return false;
			}else if(static_cast<unsigned char>(Str_[0])==0x11){
				if(Str_.size()!=1)return true;
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
		void setCorrectionFlag(void){
			if(!CorrectionFlag){
				CorrectionValue=angle();
				TmpCorrectionValue=angle();
				TmpCorrectionDt=0.;
				CorrectionFlag=true;
			}else{
				CorrectionValue=TmpCorrectionValue/TmpCorrectionDt;
				CorrectionFlag=false;
			}
		}
		void clearCorrection(void){
			CorrectionValue.roll=0;
			CorrectionValue.pitch=0;
			CorrectionValue.yaw=0;
		}
	public:
		boost::signals2::signal<void(angle, double, clock::time_point)> signal_newdata;
		
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

		void contact_getGyroData(hmLib::inquiries::inquiry<angle>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,GyroData));
		}
		void contact_getTime(hmLib::inquiries::inquiry<clock::time_point>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,Time));
		}
		void contact_getGyroMeasureTime(hmLib::inquiries::inquiry<double>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,dt));
		}
	
		void slot_setCorrectionFlag(boost::signals2::signal<void(void)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](void)->void{this->setCorrectionFlag();}));		
		}
		void slot_clearCorrection(boost::signals2::signal<void(void)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](void)->void{this->clearCorrection();}));		
		}
		void contact_getCorrectionFlag(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,CorrectionFlag));
		}
		void contact_getGyroCorrectionData(hmLib::inquiries::inquiry<angle>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,CorrectionValue));
		}
	};
}
#
#endif
