#ifndef HMR_LOGGERMANAGE_INC
#define HMR_LOGGERMANAGE_INC 101
#
/*==========hmrLoggerManage============

>> 0x00
保存されているデータサイズを要求

<< 0x00 LogSize[4]
データサイズを4byteで戻す

<> 0x01 RequestSize[2]
最低RequestSize分のデータを要求する/受諾する（超えた瞬間終わる、足りなくてももちろん終わる）

<< 0x02 LogSize[4]
ログデータの送信完了を通知するとともに、残りのログデータサイズを通知

<> 0xC0
ログモード開始（ただのフラグで、動作無　ただし、main側でこのフラグを見てロガーにぶち込む部分の処理を変更）
<> 0xC1
ログモード終了（ただのフラグで、動作無）


設計
<interface> 

//ログされているデータ数を返す
int get_logDataSize()

// 出力時のデータの大きさ(最小)を設定してデータをもらう
void request_logData(int size)

// データは全て得ることができたか？
bool is_finished_get_logDataSet

// ログモードの設定
void set_logMode

*/


#include "hmLibVer.hpp"
#include<boost/signals2.hpp>
#include <hmLib/signals.hpp>
#include <hmLib/inquiries.hpp>
#include <hmLib/type.h>
#include <hmrVLib/Message/ItfMessageAgent.hpp>
#include <hmrVLib/Data.hpp>
#include "hmrFlagirl.hpp"

namespace hmr{
	class cLoggerMngMsgAgent :public itfMessageAgent{
	private:
		// message送信用フラグ
		struct flags{
		public:
			bool get_dataSize;
			bool request_data;

			// constructor
			flags(){
				get_dataSize = false;
				request_data = false; 
			}

			// reset flags
			void reset(){
				get_dataSize = false;
				request_data = false;
			}
		};

		flags setFlags;
		flags sendFlags;
		flagirl logModeFlagirl;

		// 受信完了したかフラグ
		bool is_finished_get_data;
		// ロガーのデータを送信中フラグ
		bool is_getting_logData;

		//　LOGDataのデータ数
		hmLib_uint32 logDataSize;

		// 要求する最少のデータ数
		hmLib_uint16 logDataSetSize;

	public:
		// construtor
		cLoggerMngMsgAgent() :itfMessageAgent(), 
			is_finished_get_data(false), logDataSize(0), logDataSetSize(0), sendFlags(), setFlags(), is_getting_logData(false), logModeFlagirl(){
		}

		// message関数
		void setup_listen()override{
			return;
		}

		bool listen(datum::time_point Time_, bool Err_, const std::string& Str_)override{
			if (Str_.size() == 0)return true;

			if (static_cast<unsigned char>(Str_.at(0)) == 0x00){
				if (Str_.size() != 5)return true;
				logDataSize = static_cast<hmLib_uint32>((hmLib_uint8)Str_.at(1)) + (static_cast<hmLib_uint32>((hmLib_uint8)Str_.at(2)))*256
					+ (static_cast<hmLib_uint32>((hmLib_uint8)Str_.at(3)))*256*256 + (static_cast<hmLib_uint32>((hmLib_uint8)Str_.at(4)))*256*256*256;

				setFlags.get_dataSize = false;
				return false;
			}
			else if (static_cast<unsigned char>(Str_.at(0)) == 0x01){
				if (Str_.size() != 3)return true;
				logDataSetSize = static_cast<hmLib_uint16>((hmLib_uint8)Str_.at(1)) + (static_cast<hmLib_uint16>((hmLib_uint8)Str_.at(2)))*256;
				
				setFlags.request_data = false;
				is_finished_get_data = false;
				is_getting_logData = true;

				return false;
			}
			else if (static_cast<unsigned char>(Str_.at(0)) == 0x02){
				if (Str_.size() != 5)return true;
				logDataSize = static_cast<hmLib_uint32>((hmLib_uint8)Str_.at(1)) + (static_cast<hmLib_uint32>((hmLib_uint8)Str_.at(2)))*256
					+ (static_cast<hmLib_uint32>((hmLib_uint8)Str_.at(3)))*256*256 + (static_cast<hmLib_uint32>((hmLib_uint8)Str_.at(4)))*256*256*256;

				is_finished_get_data = true;
				is_getting_logData = false;
				return false;
			}
			else if (static_cast<unsigned char>(Str_.at(0)) == 0xC0){
				if (Str_.size() != 1)return true;
				logModeFlagirl.set_pic(true);
				return false;
			}
			else if (static_cast<unsigned char>(Str_.at(0)) == 0xC1){
				if (Str_.size() != 1)return true;
				logModeFlagirl.set_pic(false);
				return false;
			}
			return true;
		}

		void setup_talk(){
			sendFlags = setFlags;
			logModeFlagirl.setup_talk();
		}

		bool talk(std::string& Str)override{
			Str = "";
			// get data size
			if (sendFlags.get_dataSize){
				Str.push_back(static_cast<unsigned char>(0x00));
				sendFlags.get_dataSize = false;
				return false;
			}
			// request data
			else if (sendFlags.request_data){
				Str.push_back(static_cast<unsigned char>(0x01));
				Str.push_back(static_cast<unsigned char>(logDataSetSize & (0xFF)));
				Str.push_back(static_cast<unsigned char>((logDataSetSize >> 8) & (0xFF)));
				sendFlags.request_data = false;
				return false;
			}
			// change log mode
			else if (logModeFlagirl.talk()){
				// logmode start
				if (logModeFlagirl.request()) Str.push_back(static_cast<unsigned char>(0xC0));
				// logmode stop
				else Str.push_back(static_cast<unsigned char>(0xC1));

				return false;
			}
			return true;
		}


	public:
		// interface
		void request_get_logDataSize(bool y){
			if (!is_getting_logData){
				setFlags.get_dataSize = y;
			}
			return;
		}
		hmLib_uint32 get_logDataSize(){
			return logDataSize;
		}
		void request_logData(bool y, hmLib_uint16 min_logDataSetSize){
			if (!is_getting_logData){
				setFlags.request_data = y;
				logDataSetSize = min_logDataSetSize;
			}
			return;
		}
		bool is_finished_get_logDataSet(){
			return (!is_getting_logData && is_finished_get_data);
		}
		bool is_getting_logDataSet(){
			return is_getting_logData;
		}

		void set_logMode(bool y){
			logModeFlagirl.set_request(y);
		}

	private:
		hmLib::signals::unique_connections SignalConnections;
		hmLib::inquiries::unique_connections InquiryConnections;

	public:
		// slot 
		
		// request get Data size
		void slot_request_get_logDataSize(boost::signals2::signal<void(bool)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_, [&](bool flag)->void{this->request_get_logDataSize(flag); }));
		}
		// request logData
		void slot_request_logData(boost::signals2::signal<void(bool, hmLib_uint16)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_, [&](bool flag, hmLib_uint16 min_logDatSetSize)->void{this->request_logData(flag, min_logDatSetSize); }));
		}
		// request set logMode
		void slot_request_set_logMode(boost::signals2::signal<void(bool)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_, [&](bool flag)->void{this->set_logMode(flag); }));
		}

		// Inquiry
		// それぞれのコマンドの送信状態
		// ログデータサイズの取得コマンドを送信中？？
		void contact_is_sending_get_logDataSize(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->bool{return setFlags.get_dataSize; }));
		}
		// ログデータサイズを取得
		void contact_logDataSize(hmLib::inquiries::inquiry<hmLib_uint32>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->hmLib_uint32{return get_logDataSize(); }));
		}
		// ログデータの取得要求関数を送信中か？
		void contact_is_sending_request_logData(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->bool{return setFlags.request_data; }));
		}
		// logDataを受信し終わったか
		void contact_is_finished_get_logDataSet(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->bool{return is_finished_get_logDataSet(); }));
		}
		// logDataを受信している最中
		void contact_is_getting_logDataSet(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->bool{return is_getting_logDataSet(); }));
		}
		// logmode の状態
		void contact_is_logMode(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->bool{return logModeFlagirl.pic(); }));
		}
		// logmode のrequest 状態
		void contact_is_logMode_req(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->bool{return logModeFlagirl.request(); }));
		}
	};
}
#
#endif
