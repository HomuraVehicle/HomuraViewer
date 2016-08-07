#ifndef HMR_COM_INC
#define HMR_COM_INC 113
#
/*===Sample for vmc hmrCom ===

hmrCom:v1_13/130615 hmito
	packet/datumの送受信時に固有noを振るように変更
		時間では、複数のdatumが同一時間を示す場合があったため。
hmrCom v1_12/130525 amby
	受信完了時間、受信中かどうかの状態フラグのinquiryを実装
hmrCom:v1_11/130324 amby
	データのバッファ処理を大幅に変更
	１、送受信バッファは Deque<Datum>　という形で保持して、パケットの境界はダミーデータ(ID = 0x00)によって区別
	２、Signa　slot は、sync, flush 関数にて実装した
	VMCも対応済み
hmrCom v1_11/130410 hmIto
	送受信中のpacket取得用のinquiryを実装
hmrCom v1_10/130310 amby
	データの受け渡し型を、Packet から　Datum に変更
	それにあたりクラスも変更してく
hmrCom v1_02/130302 amby
	cComのＶＭＣ用関数を実装
	hmrOperator 用のインターフェース部分を実装
	abtCom には対応できていない
hmrCom v1_01/130227 hmIto
	VMC作成の実体が宣言されていたのを削除
	送受信用バッファなどのcComメンバ変数を整理
	cComのSignal/Slotをとりあえず実装
hmrCom v1_00/130223 amby
	VMC関連関数の受信まわりを製作。
*/

#include<hmLib/config_vc.h>
#include<deque>
#include<boost/signals2/signal.hpp>
#include<hmLib/signals.hpp>
#include<hmLib/inquiries/inquiry.hpp>
#include<hmLib/inquiries/unique_connections.hpp>
#include<hmLib/exceptions.hpp>
#include <HomuraViewer/VMC1.h>
#include <HomuraViewer/chrono.hpp>
#include"hmrItfCom.hpp"

namespace hmr{
	namespace viewer{
		class cCom :public itfCom{
			using datum = message::datum;
			using packet = message::packet;
		private:
			typedef std::deque<datum> datum_que;
			static datum getEndDatum(bool Error_ = false){
				datum Datum;
				Datum.No = 0;
				Datum.ID = 0x00;
				Datum.ErrFlag = Error_;
				Datum.Time = clock::now();
				return Datum;
			}
			static bool isEndDatum(const datum& Datum_){ return Datum_.ID == 0x00; }
		private://Debug
				//送受信の制御を行うクラス
			template<unsigned int VMC1_Connection_Ch_>
			class VMCConnection{
			private:
				static cCom* pCom;
				static datum TemporalSendDatum;
				static datum TemporalRecvDatum;
				static int SendedDatumCnt; // 送ったデータ数カウント
			private:
				//PacStrtを受信しようとしてよいかどうか
				static hmLib_boolian can_iniRecvPac(void){
					return true;
				}
				//PacStrt受信完了時に呼ばれる  
				static void iniRecvPac(){
					//*DEBUG*/printfDx("<");
					pCom->IsFinishedRecv = false;
				}
				//PacTrmn受信完了時に呼ばれる　引数はエラーの有無
				static void finRecvPac(unsigned char Err){
					//*DEBUG*/printfDx("/%02x>\n",static_cast<unsigned int>(Err));
					// 1 Packet 受信終了
					// 判別するためにバッファにダミーデータを挿入
					pCom->RecvBuf.push_back(cCom::getEndDatum(Err != 0));
					pCom->LastRecvPacTime = clock::now();
					pCom->IsFinishedRecv = true;
					//				printfDx("\n");

				}
				//iniRecvDatを実行して良いかの確認に呼ばれる
				static hmLib_boolian can_iniRecvDat(void){
					return true;
				}
				//Dat受信開始時に呼ばれる 引数は受信するデータサイズ
				static void iniRecvDat(vmc1_did_t ID, vmc1_dsize_t Size){
					//*DEBUG*/printfDx("[%c:%4d",ID,Size);
					// 初期化
					TemporalRecvDatum.No = datum::no_gen::no();
					TemporalRecvDatum.ID = ID;
					TemporalRecvDatum.Size = Size;
					TemporalRecvDatum.Str.clear();
					TemporalRecvDatum.Time = clock::now();
				}
				//Dat受信終了時に呼ばれる 引数はエラーの有無
				static void finRecvDat(unsigned char Err){
					//*DEBUG*/printfDx("/%02x]",static_cast<unsigned int>(Err));
					// Err の代入
					TemporalRecvDatum.ErrFlag = (Err != 0);

					// バッファにぶち込む
					pCom->RecvBuf.push_back(TemporalRecvDatum);
					//				printfDx("%c",TemporalRecvDatum.ID);
				}
				//recvを実行してよいかの確認に呼ばれる
				static hmLib_boolian can_recv(void){
					return true;
				}
				//Datの中身受信時に呼ばれる　引数が受信したデータ
				static void recv(unsigned char c){
					TemporalRecvDatum.Str.push_back(c);
					//				clock::time_point LastRecvDatTime;
					pCom->LastRecvDatTime = clock::now();
				}
				//PacStrt送信を開始してよいかどうか？
				static hmLib_boolian can_iniSendPac(void){
					// Packet が空でなければStrt送信
					if(!pCom->SendBuf.empty()){
						return true;
					} else{
						return false;
					}
				}
				//RacStrt送信完了時に呼ばれる
				static void iniSendPac(void){
					// 何もしない
					//pCom->PacSendedFlag = false;
				}
				//PacTrmn送信完了時に呼ばれる　引数はエラーの有無
				static void finSendPac(unsigned char Err){
					//末尾のダミーデータを落とす
					if(pCom->SendBuf.empty() || !cCom::isEndDatum(pCom->SendBuf.front()))return;

					pCom->SendBuf.pop_front();
					pCom->LastSendPacTime = clock::now();
					//pCom->PacSendedFlag = true;
				}
				//existSendDat関数が実行可能かを問い合わせる
				static hmLib_boolian can_existSendDat(void){
					// 次のデータが用意されていなければ、とりあえず待ってもらう
					if(pCom->SendBuf.empty()) return false;
					else return true;
				}
				//送信が必要なデータの有無の確認
				// なければターミネーた送る
				static hmLib_boolian existSendDat(void){
					// 念のためbuffチェック
					hmLib_assert(!pCom->SendBuf.empty(), hmLib::io_exceptions::end_of_file, " VMCConnection::hmLib_boolian existSendDat -> SendBuf is empty !!");

					//これが最後のダミーデータでないかをチェック
					if(cCom::isEndDatum(pCom->SendBuf.front())){
						// 最後のダミーデータならおしまい
						return false;
					} else return true;
				}
				//iniSendDatを実行して良いかの確認に呼ばれる
				static hmLib_boolian can_iniSendDat(void){
					return true;
				}
				//Dat送信確定時に呼ばれる　サイズを戻す
				// Dat送信1セットにつき始めに1回呼ばれる
				static void iniSendDat(vmc1_did_t* pID, vmc1_dsize_t* pSize){
					// Data カウントの初期化
					SendedDatumCnt = 0;

					//データ取得
					TemporalSendDatum = pCom->SendBuf.front();
					pCom->SendBuf.pop_front();

					// Data情報返却
					*pID = TemporalSendDatum.ID;
					*pSize = TemporalSendDatum.Size;
				}
				//Dat送信終了時に呼ばれる　引数はエラーの有無
				static void finSendDat(unsigned char Err){
					SendedDatumCnt = 0;
				}
				//sendを実行してよいかの確認に呼ばれる
				static hmLib_boolian can_send(void){
					return true;
				}
				//Datの中身送信時に呼ばれる
				static unsigned char send(void){
					// データ送信
					return TemporalSendDatum.Str[SendedDatumCnt++];
				}
			public:
				static VMC1* create_vmc1(cCom* pCom_){
					pCom = pCom_;
					return vmc1_create
					(can_iniRecvPac, iniRecvPac, finRecvPac
						, can_iniRecvDat, iniRecvDat, finRecvDat
						, can_recv, recv
						, can_iniSendPac, iniSendPac, finSendPac
						, can_existSendDat, existSendDat
						, can_iniSendDat, iniSendDat, finSendDat
						, can_send, send);
				}
			};
		public:
			//vmcの実態を確保して渡すクラス
			template<unsigned int VMC_Connection_Ch_>
			class VMC1Creater{
			private:
				cCom* pCom;
			public:
				VMC1Creater(cCom* pCom_) :pCom(pCom_){}
				VMC1* operator()(void){ return VMCConnection<VMC_Connection_Ch_>::create_vmc1(pCom); }
			};
		private:
			//現在受信中のデータ/パケットの記録
			packet RecvLog;
			//受信済datum用キュー
			datum_que RecvBuf;
			//現在送信中のデータ/パケットの記録
			packet SendLog;
			//送信待datum用キュー
			datum_que SendBuf;

			//受信終了時の時間を記録
			// 最新のPac受信終了時間
			clock::time_point LastRecvPacTime;
			// 最新のData受信終了時間
			clock::time_point LastRecvDatTime;

			// 現在受信中か否か
			bool IsFinishedRecv;

			//送信終了時間の記録
			clock::time_point LastSendPacTime;

		public:
			// Constructor
			cCom() :itfCom(), IsFinishedRecv(false){
				LastRecvDatTime = clock::now();
				LastSendPacTime = clock::now();
				LastRecvPacTime = clock::now();
			}

		private:
			/*
			// Packetの送信完了したフラグ
			bool PacSendedFlag;
			// 最後に受信した時間
			clock::time_point ReceivedTime;
			public:
			// Packetの送信完了したフラグ
			bool isPacSended(void){
			return PacSendedFlag;
			}
			void resetPacSendedFlag(void){
			PacSendedFlag = false;
			}
			*/

		private://signals/inquiries
			hmLib::inquiries::unique_connections InquiryConnections;
		public://signals/inquiry
			   //パケットを受信完了して、RecvBufに入れるタイミングで呼ばれる
			boost::signals2::signal<void(const packet& Pac_)> signal_finRecvPacket;
			//パケットを送信完了して、次の送信データがSendBufから呼ばれるタイミングで呼ばれる。
			boost::signals2::signal<void(const packet& Pac_)> signal_finSendPacket;
			//受信中のログを取得する
			void contactRecvLog(hmLib::inquiries::inquiry<const packet&>& Inq_){
				InquiryConnections(Inq_.connect([&]()->const packet&{return this->RecvLog; }));
			}
			//送信中のログを取得する
			void contactSendLog(hmLib::inquiries::inquiry<const packet&>& Inq_){
				InquiryConnections(Inq_.connect([&]()->const packet&{return this->SendLog; }));
			}
			//受信完了時間を取得する
			void contactLastRecvPacTime(hmLib::inquiries::inquiry<clock::time_point>& Inq_){
				InquiryConnections(hmLib::inquiries::connect(Inq_, LastRecvPacTime));
			}
			//送信完了時間を取得する
			void contactLastSendPacTime(hmLib::inquiries::inquiry<clock::time_point>& Inq_){
				InquiryConnections(hmLib::inquiries::connect(Inq_, LastSendPacTime));
			}
			//Data受信時間を確認する
			void contactLastRecvDatTime(hmLib::inquiries::inquiry<clock::time_point>& Inq_){
				InquiryConnections(hmLib::inquiries::connect(Inq_, LastRecvDatTime));
			}
			//受信終了してるかどうかの確認
			void contactIsFinishedRecv(hmLib::inquiries::inquiry<bool>& Inq_){
				InquiryConnections(hmLib::inquiries::connect(Inq_, IsFinishedRecv));
			}
			//受信中のログのサイズを取得する
			void contactRecvLogSize(hmLib::inquiries::inquiry<unsigned int>& Inq_){
				InquiryConnections(hmLib::inquiries::connect(Inq_, [&](void)->unsigned int{return this->RecvLog.Data.size(); }));
			}
			//送信中のログのサイズを取得する
			void contactSendLogSize(hmLib::inquiries::inquiry<unsigned int>& Inq_){
				InquiryConnections(hmLib::inquiries::connect(Inq_, [&](void)->unsigned int{return this->SendLog.Data.size(); }));
			}
			//受信バッファのサイズを取得する
			void contactRecvBufSize(hmLib::inquiries::inquiry<unsigned int>& Inq_){
				InquiryConnections(hmLib::inquiries::connect(Inq_, [&](void)->unsigned int{return this->RecvBuf.size(); }));
			}
			//送信バッファのサイズを取得する
			void contactSendBufSize(hmLib::inquiries::inquiry<unsigned int>& Inq_){
				InquiryConnections(hmLib::inquiries::connect(Inq_, [&](void)->unsigned int{return this->SendBuf.size(); }));
			}
		public://slots
			   // 受信バッファにたまったものをとりあえず消去する
			void clearRecvBuf(){ RecvBuf.clear(); }
			// 送信バッファにたまったものをとりあえず消去する
			void clearSendBuf(){ SendBuf.clear(); }

		private:
			//slot 
			hmLib::signals::unique_connections connections;
		public:
			//slot
			void slot_clearRecvBuf(boost::signals2::signal<void(void)> &sig_){
				connections(sig_.connect([&](void)->void{this->clearRecvBuf(); }));
			}
			void slot_clearSendBuf(boost::signals2::signal<void(void)> &sig_){
				connections(sig_.connect([&](void)->void{this->clearSendBuf(); }));
			}
		public:
			//受信バッファ内のデータの存在の有無を確認する
			bool empty()override{
				//空っぽか、あるいは先頭がEndDatumなら空
				return RecvBuf.empty() || isEndDatum(RecvBuf.front());
			}
			//受信バッファ内からデータを取得する
			datum recv()override{
				hmLib_assert(!empty(), hmLib::io_exceptions::end_of_file, "hmrCom::RecvBuf is empty");

				//バッファの先頭を取得
				datum Datum = RecvBuf.front();
				//バッファの先頭を破棄
				RecvBuf.pop_front();
				//ログに書き込み
				RecvLog.Data.push_back(Datum);

				return Datum;
			}
			//受信ストリームから更新する必要があるかを確認する 最新ならtrue
			bool can_sync()override{
				//空でなく、先頭がEndDatumのときに真
				return ((!RecvBuf.empty())) && isEndDatum(RecvBuf.front());
			}
			//受信ストリームから受信バッファを更新する
			void sync()override{
				hmLib_assert(can_sync(), hmLib::io_exceptions::end_of_file, "hmrCom::RecvBuf have already been syncd");

				//RecvLogをシグナル発信して初期化
				RecvLog.ErrFlag = RecvBuf.front().ErrFlag;
				RecvLog.Time = RecvBuf.front().Time;
				RecvLog.No = packet::no_gen::no();
				signal_finRecvPacket(RecvLog);
				RecvLog.Data.clear();
				RecvLog.Time = clock::time_point::max();
				//バッファの先頭のEndDatumを削除する
				RecvBuf.pop_front();
			}
			//送信バッファの空きを確認する関数
			bool full()override{ return SendBuf.size()>0 && isEndDatum(SendBuf.back()); }
			//送信バッファにデータを追加する
			void send(const datum& Datum)override{
				datum Dat(Datum);
				Dat.No = datum::no_gen::no();
				//バッファの末尾に追加する
				SendBuf.push_back(Dat);
				//ログに書き込み
				SendLog.Data.push_back(Dat);
			}
			//送信ストリームに送信バッファを反映する
			void flush()override{
				hmLib_assert(can_flush(), hmLib::io_exceptions::end_of_file, "hmrCom::SendBuf cannot flush");
				SendBuf.push_back(getEndDatum());

				//RecvLogをシグナル発信して初期化
				SendLog.No = packet::no_gen::no();
				SendLog.ErrFlag = false;
				SendLog.Time = clock::now();
				signal_finSendPacket(SendLog);
				SendLog.Data.clear();
				RecvLog.Time = clock::time_point::max();
			}
			//送信ストリームに反映可能かを確認する
			bool can_flush()override{ return true; }
		};
		template<unsigned int VMC1_Connection_Ch_>
		cCom* cCom::VMCConnection<VMC1_Connection_Ch_>::pCom;
		template<unsigned int VMC1_Connection_Ch_>
		message::datum cCom::VMCConnection<VMC1_Connection_Ch_>::TemporalSendDatum;
		template<unsigned int VMC1_Connection_Ch_>
		message::datum cCom::VMCConnection<VMC1_Connection_Ch_>::TemporalRecvDatum;
		template<unsigned int VMC1_Connection_Ch_>
		int cCom::VMCConnection<VMC1_Connection_Ch_>::SendedDatumCnt;
		//	template<unsigned int VMC1_Connection_Ch_>
		//	int cCom::VMCConnection<VMC1_Connection_Ch_>::DebugNum=0;
	}
}
#
#endif
