#ifndef HMR_MOTOR_INC
#define HMR_MOTOR_INC 101
#

/*=====hmrMotor======
hmrMotor v1_01/130602 iwahori
	キーボード・パッド用のslot_setDataMode()を追加(引数がvoid(void)になっている)
hmrMotor v1_00/130427 iwahori
	cMotorMsgAgentのsignal-slot,inquiry-contactへの対応完了
*/
#include "hmLibVer.hpp"
#include<boost/signals2.hpp>
#include<hmLib_v3_05/signals.hpp>
#include <hmLib_v3_05/inquiries.hpp>
#include "hmrItfMessage.hpp"
#include "hmrFlagirl.hpp"

namespace hmr{
	class cMotorMsgAgent:public itfMessageAgent{
	private:
		bool BackMode;
		char StickL;
		bool BrakeL;
		char StickR;
		bool BrakeR;
		bool SendMotorCom;			//モーター出力送信フラグ
	public:
		cMotorMsgAgent():
			BackMode(false),
			StickL(0),
			BrakeL(false),
			StickR(0),
			BrakeR(false){
		}
		bool listen(datum::time_point _time,bool _errFlag,const std::string& _data)override{
			return true;
		}
		void setup_talk(void)override{
			SendMotorCom = true;
		}
		bool talk(std::string& Str)override{
			if(SendMotorCom){
				SendMotorCom = false;
				Str="";
				Str.push_back(static_cast<unsigned char>(StickL));
				Str.push_back(static_cast<unsigned char>(BrakeL));
				Str.push_back(static_cast<unsigned char>(StickR));
				Str.push_back(static_cast<unsigned char>(BrakeR));
				return false;
			}else{
				return true;
			}
		}
	private:
		hmLib::signals::unique_connections SignalConnections;
		hmLib::inquiries::unique_connections InquiryConnections;
		void setBackMode(bool Mode_){BackMode=Mode_;}
		void setStickL(char Val_){
			if(!BackMode)StickL=Val_;
			else StickR=-(static_cast<signed char>(Val_));
		}
		void setStickL(Pint point){
			signed int Val=static_cast<signed int>(point.y/10.);	//Padは-1000から1000
			if(!BackMode)StickL=static_cast<signed char>(Val);
			else StickR=-static_cast<signed char>(Val);
		}
		void setBrakeL(bool Mode_){
			if(!BackMode)BrakeL=Mode_;
			else BrakeR=Mode_;
		}
		void setStickR(char Val_){
			if(!BackMode)StickR=Val_;
			else StickL=-(static_cast<signed char>(Val_));
		}
		void setStickR(Pint point){
			signed int Val=static_cast<signed int>(point.y/10.);	//Padは-1000から1000
			if(!BackMode)StickR=static_cast<signed char>(Val);
			else StickL=-(static_cast<signed char>(Val));
		}
		void setBrakeR(bool Mode_){
			if(!BackMode)BrakeR=Mode_;
			else BrakeL=Mode_;
		}
		bool getBackMode()const{return BackMode;}
	public:
		void slot_setBackMode(boost::signals2::signal<void(bool)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](bool Flag)->void{this->setBackMode(Flag);}));	
		}
		void slot_setBackMode(boost::signals2::signal<void(void)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](void)->void{this->setBackMode(!BackMode);}));	
		}
		void slot_setBackModeON(boost::signals2::signal<void(void)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](void)->void{this->setBackMode(true);}));	
		}
		void slot_setBackModeOFF(boost::signals2::signal<void(void)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](void)->void{this->setBackMode(false);}));	
		}

		void slot_setStickL(boost::signals2::signal<void(char)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](char Val_)->void{this->setStickL(Val_);}));	
		}
		void slot_setStickL(boost::signals2::signal<void(Pint)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](Pint point_)->void{this->setStickL(point_);}));	
		}
		void slot_setBrakeL(boost::signals2::signal<void(bool)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](bool Flag)->void{this->setBrakeL(Flag);}));	
		}
		void slot_setBrakeLON(boost::signals2::signal<void(void)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](void)->void{this->setBrakeL(true);}));	
		}
		void slot_setBrakeLOFF(boost::signals2::signal<void(void)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](void)->void{this->setBrakeL(false);}));
		}

		void slot_setStickR(boost::signals2::signal<void(char)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](char Val_)->void{this->setStickR(Val_);}));	
		}
		void slot_setStickR(boost::signals2::signal<void(Pint)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](Pint point_)->void{this->setStickR(point_);}));	
		}
		void slot_setBrakeR(boost::signals2::signal<void(bool)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](bool Flag)->void{this->setBrakeR(Flag);}));	
		}
		void slot_setBrakeRON(boost::signals2::signal<void(void)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](void)->void{this->setBrakeR(true);}));	
		}
		void slot_setBrakeROFF(boost::signals2::signal<void(void)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](void)->void{this->setBrakeR(false);}));	
		}

		void contact_BackMode(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->bool{return this->getBackMode();}));
		}
		void contact_StickL(hmLib::inquiries::inquiry<char>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,StickL));
		}
		void contact_BrakeL(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,BrakeL));		
		}
		void contact_StickR(hmLib::inquiries::inquiry<char>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,StickR));
		}
		void contact_BrakeR(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,BrakeR));		
		}

	};
}

#
#endif
