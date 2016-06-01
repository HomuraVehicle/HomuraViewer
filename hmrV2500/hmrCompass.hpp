#ifndef HMR_COMPASS_INC
#define HMR_COMPASS_INC 103
#
/*==========hmrCompass============
hmrCompass v1_03/131111 hmIto
	CorrectionValue�𕡐���擾���Đ��x�����߂���悤�ɕύX
hmrCompass v1_02/130602 iwahori
	�L�[�{�[�h�E�p�b�h�p��slot_setDataMode()��ǉ�(������void(void)�ɂȂ��Ă���)
hmrCompass v1_01/130519 iwahori
	hmLib::coordinates3D::position�ɕύX
hmrCompass v1_00/130511 iwahori
	cAcceleMsAgent��signal-slot,inquiry-contact�ւ̑Ή�����

*/
#include<fstream>
#include "hmLibVer.hpp"
#include<boost/signals2.hpp>
#include<hmLib_v3_06/signals.hpp>
#include <hmLib_v3_06/inquiries.hpp>
#include "hmrItfMessage.hpp"
#include "hmrFlagirl.hpp"
#include "coordinates.hpp"

namespace hmr{
	class cCompassMsgAgent:public itfMessageAgent{
		typedef hmLib::coordinates3D::position position;
		typedef hmLib::coordinates3D::polar polar;
	private:
/*		class cCorrector{
			position CorrectionValue;
			position NextCorrectionValue;
		public:
			position operator()(const position& Val_){return Val_-CorrectionValue;}
		};*/
		position CompassData;				//�R���p�X�f�[�^
		bool CorrectionFlag;				//true:�␳�l�擾��
//		clock::time_point Time;
		flagirl DataModeFlagirl;
		//Correction�n��
		position CorrectionValue;		//�␳�l
		position NextCorrectionValue;	//���̕␳�l
		unsigned int CorrectionNum;
		//�f�[�^���擾����
		position toData(const std::string& Str_){
			double X,Y,Z;
			if(Str_.size()!=7)return position();
			X=static_cast<signed short>((static_cast<unsigned char>(Str_[1])+0x100*static_cast<unsigned char>(Str_[2])));
			Y=static_cast<signed short>((static_cast<unsigned char>(Str_[3])+0x100*static_cast<unsigned char>(Str_[4])));
			Z=static_cast<signed short>((static_cast<unsigned char>(Str_[5])+0x100*static_cast<unsigned char>(Str_[6])));
			return position(X-CorrectionValue.x,Y-CorrectionValue.y,Z-CorrectionValue.z);
		}
	public:
		cCompassMsgAgent():
			CompassData(),
			CorrectionValue(),
			CorrectionFlag(false),
			CorrectionNum(0),
//			CorrectionRecvCnt(0),
//			Time(),
			DataModeFlagirl(){
		}
		bool listen(datum::time_point Time_, bool Err_,const std::string& Str_)override{
			if(Str_.size()==0)return true;

			if(static_cast<unsigned char>(Str_[0])==0x00){
				if(Str_.size()!=7)return true;	
				CompassData=toData(Str_);
				signal_inform_compassData_and_time(CompassData, Time_);
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
		void setCorrectionMode(void){
			//�␳���[�h�ɓ��������́A�������u�Ԃ̃f�[�^��k�x�N�g���Ƃ��ċL�^
			if(!CorrectionFlag){
				CorrectionValue.x=0;
				CorrectionValue.y=0;
				CorrectionValue.z=0;
				CorrectionFlag=true;
				NextCorrectionValue.x=0;
				NextCorrectionValue.y=0;
				NextCorrectionValue.z=0;
				CorrectionNum=0;
//				NextCorrectionValue=CompassData;
			//�␳���[�h���̏ꍇ�́A���f�[�^���x�N�g���Ƃ��ĕ␳�l�m��
			}else{
//				position SouthValue(CompassData.x,CompassData.y,-CompassData.z);
//				CorrectionValue=position(261.5,887.5,26.0);
				if(CorrectionNum%2!=0)return;
				if(CorrectionNum!=0)CorrectionValue=NextCorrectionValue/CorrectionNum;
				CorrectionFlag=false;
			}
		}
		void addCorrection(void){
//			NextCorrectionValue+=position(CompassData.x,CompassData.y,(CorrectionNum%2==0?CompassData.z:-CompassData.z));
			NextCorrectionValue+=CompassData;
			++CorrectionNum;
		}
		void setCorrectionFromFile(void){
			std::ifstream fin("hmr/Compass/CorrectionValue.txt");
			if(!fin.is_open())return;

			fin>>CorrectionValue.x;
			fin>>CorrectionValue.y;
			fin>>CorrectionValue.z;
		}
		void clearCorrection(void){
			CorrectionValue.x=0;
			CorrectionValue.y=0;
			CorrectionValue.z=0;
		}
		polar getPolarData(void){
			return polar(position(CompassData.x,CompassData.y,CompassData.z));
		}
	public:
		void slot_setCorrectionMode(boost::signals2::signal<void(void)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](void)->void{setCorrectionMode();}));		
		}
		void slot_addCorrection(boost::signals2::signal<void(void)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](void)->void{addCorrection();}));		
		}
		void slot_clearCorrection(boost::signals2::signal<void(void)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](void)->void{clearCorrection();}));		
		}
		void slot_setCorrectionFromFile(boost::signals2::signal<void(void)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](void)->void{setCorrectionFromFile();}));		
		}
		void contact_getCorrectionMode(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,CorrectionFlag));
		}
		void contact_getCorrectionNum(hmLib::inquiries::inquiry<unsigned int>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,CorrectionNum));
		}

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
		void contact_getCompassCorrectionData(hmLib::inquiries::inquiry<position>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,CorrectionValue));
		}

		boost::signals2::signal<void(position,clock::time_point)> signal_inform_compassData_and_time;

		void contact_getCompassPositon(hmLib::inquiries::inquiry<position>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,CompassData));
		}
		void contact_getCompassPolar(hmLib::inquiries::inquiry<polar>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->polar{return this->getPolarData();}));
		}
/*		void contact_getTime(hmLib::inquiries::inquiry<clock::time_point>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,Time));
		}
*/	};
}
#
#endif
