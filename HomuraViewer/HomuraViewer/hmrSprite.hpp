#ifndef HMR_SPRITE_INC
#define HMR_SPRITE_INC 103
#
/*=======hmrSprite=========
hmrSprite:v1_03/130706 hmito
	DataSizeを取得した段階で、DataPosも初期化するように修正
hmrSprite v1_02/130511 iwahori
	DoSetTakePicture,DoCommandReset,DoPowerResetへのcontact追加
	情報取得のlisten/talkを最新の通信規約に対応
	setCommandResetとsetPowerReset関数の引数を変更
hmrSprite v1_01/130427 iwahori
	hmLibのバージョン変更に伴う微修正
	FlagirlのPicとrequestへのinquiryを追加
hmrSprite v1_00/130420 iwahori
	cSpriteMsgAgentのsignal-slot,inquiry-contactへの対応完了
2013.01.27 by amby
  　新通信規約に対応させる。
*/
#include "hmLibVer.hpp"
#include <boost/signals2.hpp>
#include <hmLib/signals.hpp>
#include <hmLib/inquiries.hpp>
#include <hmrVLib/Message/ItfMessageAgent.hpp>
#include "hmrFlagirl.hpp"

namespace hmr{
	class cSpriteMsgAgent:public itfMessageAgent{
	private:
		//データ関連
		bool IsData;				//データ取得中か？
		std::string DataStr;		//データ自体
		unsigned int DataPos;		//データ位置
		unsigned int DataSize;		//データサイズ
		clock::time_point DataTime;		//データ時間

		//log データ関連
		bool log_IsData;				//データ取得中か？
		std::string log_DataStr;		//データ自体
		unsigned int log_DataPos;		//データ位置
		unsigned int log_DataSize;		//データサイズ
		clock::time_point log_DataTime;		//データ時間

		//状況
		unsigned char Status;		//カメラステータス
		bool IsErr;					//エラー状態か？
		unsigned char ErrCode;		//エラーコード
		clock::time_point StatusTime;//ステータス時間

		//写真撮影要求フラグ
		bool DoSetTakePicture;
		//指定写真サイズ
		unsigned char PictureSize;   //PC側の写真サイズ指定

		//コマンドリセット要求フラグ
		bool DoCommandReset;
		//パワーリセット要求時フラグ
		bool DoPowerReset;

		//オートパワーリセット
		flagirl AutoPowerResetFlagirl;
		//オートライト点灯機能
		flagirl AutoLightFlagirl;
		//ライトON/OFF
		flagirl LightFlagirl;
		//連続撮影モード
		flagirl AutoTakePicFlagirl;
		//ミニパケット送信モード
		flagirl MiniPacketModeFlagirl;
		//情報取得モード
		flagirl InfoModeFlagirl;
		//ログモード
		flagirl LogModeFlagirl;

	public:
		cSpriteMsgAgent()
			:DataStr()
			,DataPos(0)
			,DataSize(0)
			,PictureSize(0)
			,DoCommandReset(false)
			,DoPowerReset(false)
			,AutoPowerResetFlagirl()
			,AutoLightFlagirl()
			,LightFlagirl()
			,DoSetTakePicture(false)
			,AutoTakePicFlagirl()
			,MiniPacketModeFlagirl()
			, LogModeFlagirl()
			,Status(0x00)
			,IsErr(false)
			,ErrCode(0x00)
			,log_DataStr(), log_DataPos(0), log_DataSize(0){
		}
		bool listen(datum::time_point Time_,bool Err_,const std::string& Str_)override;
		bool talk(std::string& Str)override;
		void setup_listen(void)override;
		void setup_talk(void)override;
	private:
		hmLib::signals::unique_connections SignalConnections;
		hmLib::inquiries::unique_connections InquiryConnections;

		void setTakePicture(){
			InfoModeFlagirl.set_request(true);
			DoSetTakePicture=true;
		}
		void setTakePictureLarge(){
			PictureSize=2;
			setTakePicture();
		}

		void setCommandReset(){DoCommandReset=true;}
		void setPowerReset(){DoPowerReset=true;}

		void setInfoMode(bool Val_){InfoModeFlagirl.set_request(Val_);}
		bool getPicInfoMode()const{return InfoModeFlagirl.pic();}
		bool getRequestInfoMode()const{return InfoModeFlagirl.request();}

		void setAutoTakePicture(bool Val_){
			if(Val_)InfoModeFlagirl.set_request(Val_);
			AutoTakePicFlagirl.set_request(Val_);
		}
		bool getPicAutoTakePicture()const{return AutoTakePicFlagirl.pic();}
		bool getRequestAutoTakePicture()const{return AutoTakePicFlagirl.request();}

		void setAutoPowerReset(bool Val_){AutoPowerResetFlagirl.set_request(Val_);}
		bool getPicAutoPowerReset()const{return AutoPowerResetFlagirl.pic();}
		bool getRequestAutoPowerReset()const{return AutoPowerResetFlagirl.request();}

		void setAutoLight(bool Val_){AutoLightFlagirl.set_request(Val_);}
		bool getPicAutoLight()const{return AutoLightFlagirl.pic();}
		bool getRequestAutoLight()const{return AutoLightFlagirl.request();}

		void setLight(bool Val_){LightFlagirl.set_request(Val_);}
		bool getPicLight()const{return LightFlagirl.pic();}
		bool getRequestLight()const{return LightFlagirl.request();}

		void setMiniPacketMode(bool Val_){MiniPacketModeFlagirl.set_request(Val_);}
		bool getPicMiniPacketMode()const{return MiniPacketModeFlagirl.pic();}
		bool getRequestMiniPacketMode()const{return MiniPacketModeFlagirl.request();}

		void setLogMode(bool Val_) { LogModeFlagirl.set_request(Val_); }
		bool getPicLogMode()const { return LogModeFlagirl.pic(); }
		bool getRequestLogMode()const { return LogModeFlagirl.request(); }

		void setPictureSize(unsigned char Size_) {
			if(Size_>2)return;
			PictureSize=Size_;
		}
		unsigned char getPictureSize()const{return PictureSize;}

		bool getIsData()const{return IsData;}
		std::pair<unsigned int,unsigned int> getDataPosSize()const{
			return std::make_pair(DataPos,DataSize);
		}
		clock::time_point getDataTime()const{return DataTime;}

		unsigned char getStatus()const{return Status;}
		bool getIsErr()const{return IsErr;}
		unsigned char getErrCode()const{return ErrCode;}
		clock::time_point getStatusTime()const{return StatusTime;}
	public:
		boost::signals2::signal<void(const std::string&,clock::time_point)> signal_setPicture;
		boost::signals2::signal<void(const std::string&, clock::time_point)> signal_setLogPicture;

		void slot_setTakePicture(boost::signals2::signal<void(void)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](void)->void{this->setTakePicture();}));
		}
		void contact_getDoTakePictureFlag(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,DoSetTakePicture));
		}
		void slot_setTakePictureLarge(boost::signals2::signal<void(void)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](void)->void{this->setTakePictureLarge();}));
		}

		void slot_setCommandReset(boost::signals2::signal<void(void)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](void)->void{this->setCommandReset();}));
		}
		void contact_getDoCommandResetFlag(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,DoCommandReset));
		}

		void slot_setPowerReset(boost::signals2::signal<void(void)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](void)->void{this->setPowerReset();}));
		}
		void contact_getDoPowerResetFlag(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,DoPowerReset));
		}

		void slot_setInfoMode(boost::signals2::signal<void(bool)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](bool b)->void{this->setInfoMode(b);}));
		}
		void slot_setInfoMode(boost::signals2::signal<void(void)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](void)->void{this->setInfoMode(!(InfoModeFlagirl.request()));}));
		}
		void contact_getPicInfoMode(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->bool{return this->getPicInfoMode();}));
		}
		void contact_getRequestInfoMode(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->bool{return this->getRequestInfoMode();}));
		}

		void slot_setAutoTakePicture(boost::signals2::signal<void(bool)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](bool b)->void{this->setAutoTakePicture(b);}));
		}
		void slot_setAutoTakePicture(boost::signals2::signal<void(void)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](void)->void{this->setAutoTakePicture(!(AutoTakePicFlagirl.request()));}));
		}
		void contact_getPicAutoTakePicture(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->bool{return this->getPicAutoTakePicture();}));
		}
		void contact_getRequestAutoTakePicture(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->bool{return this->getRequestAutoTakePicture();}));
		}

		void slot_setAutoPowerReset(boost::signals2::signal<void(bool)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](bool b)->void{this->setAutoPowerReset(b);}));
		}
		void contact_getPicAutoPowerReset(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->bool{return this->getPicAutoPowerReset();}));
		}
		void contact_getRequestAutoPowerReset(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->bool{return this->getRequestAutoPowerReset();}));
		}

		void slot_setAutoLight(boost::signals2::signal<void(bool)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](bool b)->void{this->setAutoLight(b);}));
		}
		void contact_getPicAutoLight(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->bool{return this->getPicAutoLight();}));
		}
		void contact_getRequestAutoLight(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->bool{return this->getRequestAutoLight();}));
		}	

		void slot_setLight(boost::signals2::signal<void(bool)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](bool b)->void{this->setLight(b);}));
		}
		void contact_getPicLight(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->bool{return this->getPicLight();}));
		}
		void contact_getRequestLight(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->bool{return this->getRequestLight();}));
		}

		void slot_setMiniPacketMode(boost::signals2::signal<void(bool)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](bool b)->void{this->setMiniPacketMode(b);}));
		}
		void contact_getPicMiniPacketMode(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->bool{return this->getPicMiniPacketMode();}));
		}
		void contact_getRequestMiniPacketMode(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->bool{return this->getRequestMiniPacketMode();}));
		}

		void slot_setLogMode(boost::signals2::signal<void(bool)>& Signal_) {
			SignalConnections(hmLib::signals::connect(Signal_, [&](bool b)->void {this->setLogMode(b); }));
		}
		void contact_getPicLogMode(hmLib::inquiries::inquiry<bool>& Inquiry_) {
			InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->bool {return this->getPicLogMode(); }));
		}
		void contact_getRequestLogMode(hmLib::inquiries::inquiry<bool>& Inquiry_) {
			InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->bool {return this->getRequestLogMode(); }));
		}

		void slot_setPictureSize(boost::signals2::signal<void(unsigned char)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](unsigned char c)->void{this->setPictureSize(c);}));
		}
		void contact_getPictureSize(hmLib::inquiries::inquiry<unsigned char>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->unsigned char{return this->getPictureSize();}));
		}
		void slot_setPictureSizeLarge(boost::signals2::signal<void(void)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](void)->void{this->setPictureSize(2);}));
		}

		void contact_getIsData(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,IsData));
		}
		void contact_getDataPosSize(hmLib::inquiries::inquiry<std::pair<unsigned int,unsigned int>>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->std::pair<unsigned int,unsigned int>{return this->getDataPosSize();}));
		}
		void contact_getDataTime(hmLib::inquiries::inquiry<clock::time_point>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,DataTime));
		}

		void contact_getStatus(hmLib::inquiries::inquiry<unsigned char>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,Status));
		}
		void contact_getIsErr(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,IsErr));
		}
		void contact_getErrCode(hmLib::inquiries::inquiry<unsigned char>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,ErrCode));
		}
		void contact_getStatusTime(hmLib::inquiries::inquiry<clock::time_point>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,StatusTime));
		}

		void contact_getLogDataTime(hmLib::inquiries::inquiry<clock::time_point>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_, log_DataTime));
		}
		void contact_getLogData(hmLib::inquiries::inquiry<std::string>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_, log_DataStr));
		}

	};
}

#
#endif
