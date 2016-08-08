#ifndef HMR_VIEWER_MESSAGE_MESSAGEDRIVER_INC
#define HMR_VIEWER_MESSAGE_MESSAGEDRIVER_INC 103
#
/*===hmrMessageDriver===
MesasgeとComを制御して送受信タイミングをコントロールするクラス

hmrMessageDriver:v1_03/130525 amby  
	Interval と　TimeOut の考え型を変更、それに伴って、実装しなおし

	以下に説明
　　　 VMCはsignalによって、パケットの受信終了を知らせ続ける。
　　　 その受信終了時刻を記録しておいて、

　　　全二重において
　　　　とりあえず、TimeIntervalで送信して、いつでも受信を行う。　　　　
　　　　最新の受信終了時刻からTimeOutを過ぎてしまったときに、VMCの受信フェーズを強制終了する。

　　　半二重において
　　　　最新の受信終了時刻から、TimeInterval待って送信する。(Intervalを0にするとsyncということ)
　　　　最新の受信終了時刻からTimeOutを過ぎてしまったときに、VMCの受信フェーズを強制終了する。

hmrMessageDriver:v1_02/130519 amby
	sendMode については使っていない
	全二重、半二重、タイムアウトとかはすべてここで管理している。

	最終的にデータが来た来てないだけで、TimeOut を判断している。
	ここまでデータが来ていなければ送信をもう一度行うという流れになっている

	送信制御フラグを追加

	半二重モードについて修正する必要あり、次回要確認
	VMCのリセットのタイミングについても同様　

hmrMessageDriver:v1_01/130511 amby
	Interval を制御するために　
	cFullRecvTimeSendMessageDriverに
	signal slot を追加する

	IOのsendMode について　だれが責任を持つか？？
	OPが持つべきなのだけれど、送信がいつ終わったかを知りようがない。
	VMCからSignalとかで知ったとしてやることはできるが、それが重要なのかどうか？？

	結局、OPで制御してしまえば、SendModeを制御するメリットはあまりないのでは？と感じる
	よって、SendModeについては保留にしておく.


hmrMessageDriver:v1_00/130310 hmito
	cFullRecvOneSendMessageDriverを追加

*/
#include <hmLib/signals.hpp>
#include <hmLib/inquiries/inquiry.hpp>
#include <boost/signals2.hpp>
#include <HomuraViewer/Message/ItfMessage.hpp>
#include <HomuraViewer/chrono.hpp>
#include"ItfCom.hpp"

namespace hmr{
	namespace viewer{
		namespace message{
			class cFullRecvOneSendMessageDriver{
				using datum = message::datum;
			private:
				itfMessageHost* pMessage;
				itfCom* pCom;
			public:
				cFullRecvOneSendMessageDriver(itfMessageHost* pMessage_, itfCom* pCom_)
					:pMessage(pMessage_)
					, pCom(pCom_){}
				void operator()(void){
					while(true){
						if(pCom->empty()){
							if(!pCom->can_sync())break;
							pCom->sync();
							pMessage->setup_listen();
							continue;
						}
						pMessage->listen(pCom->recv());
					}

					pMessage->setup_talk();
					while(true){
						if(pCom->full())break;

						datum Dat;
						if(pMessage->talk(Dat))break;

						pCom->send(Dat);
					}
					if(pCom->can_flush())pCom->flush();
				}
			};
			class cFullRecvTimeSendMessageDriver{
				using datum = message::datum;
			private:
				itfMessageHost* pMessage;
				itfCom* pCom;

				//　全二重、半二重の選択フラグ
				bool IsFullDuplex; // 全二重ならtrue 半二重ならfalse
								   // IO によって参照される

								   //  送信自体を行うかどうかのフラグ
				bool IsSendActiveFlag; // 送信する状態なら true    
									   // 送信しない状態なら false 

									   // 全二重, 半二重通信制御のための変数通信制御のための変数
				clock::duration Interval;// 送信間隔	
										 //bool SyncFlag;// Syncするかどうかのフラグ
				clock::duration TimeOut;// Sync時に返信が来なかったときの再送までの時間間隔
										// Sync していなければ、送信の時間間隔になる

										// 半二重通信のためのモード制御フラグ
				bool RecvMode;

				bool RecevedFlag;// 正常に受信したかどうかのフラグ  
				bool IsTimeoutedFlag;
				bool IsSendedFlag;

				// 前回受信送信した時間
				clock::time_point LastRecvTime;
				clock::time_point LastSendTime;

			private:
				// slot
				hmLib::signals::unique_connections connections;
			public:
				//signal 
				//boost::signals2::signal<void(bool)> signal_require_setSendMode;
				// VMC関連 signal
				boost::signals2::signal<void(void)> signal_require_VMC_force_end_recv;
				boost::signals2::signal<void(void)> signal_require_VMC_force_end_send;

				boost::signals2::signal<void(clock::time_point)> signal_inform_TimeOut;
				boost::signals2::signal<void(clock::time_point)> signal_inform_Sended;
				boost::signals2::signal<void(clock::time_point)> signal_inform_Received;


				// slot 
				void setSendInterval(clock::duration Interval_){
					Interval = Interval_;
				}
				// slot 
				void setTimeOut(clock::duration TimeOut_){
					TimeOut = TimeOut_;
				}
				// slot 
				void setIsSendActiveFlag(bool IsSendActive_){
					IsSendActiveFlag = IsSendActive_;
				}
				// slot 
				void setIsFullDuplex(bool IsFullDuplex_){
					IsFullDuplex = IsFullDuplex_;
				}


				// inquire 系列の定義
				hmLib::inquiries::inquiry<bool> IsFinishedRecvInq;


			public:
				// connection
				void slot_setSendInterval(boost::signals2::signal<void(clock::duration)> &sig_){
					connections(sig_.connect([&](clock::duration v)->void{this->setSendInterval(v); }));
				}
				void slot_setTimeOut(boost::signals2::signal<void(clock::duration)> &sig_){
					connections(sig_.connect([&](clock::duration v)->void{this->setTimeOut(v); }));
				}
				void slot_setIsSendActiveFlag(boost::signals2::signal<void(bool)> &sig_){
					connections(sig_.connect([&](bool v)->void{this->setIsSendActiveFlag(v); }));
				}
				void slot_setIsFullDuplex(boost::signals2::signal<void(bool)> &sig_){
					connections(sig_.connect([&](bool v)->void{this->setIsFullDuplex(v); }));
				}


				// Contact関数
				void contact_Interval(hmLib::inquiries::inquiry<clock::duration>& IntervalInq_){
					hmLib::inquiries::connect(IntervalInq_, Interval);
				}
				void contact_TimeOut(hmLib::inquiries::inquiry<clock::duration>& TimeOutInq_){
					hmLib::inquiries::connect(TimeOutInq_, TimeOut);
				}
				void contact_IsFullDuplex(hmLib::inquiries::inquiry<bool>& IsFullDuplexInq_){
					hmLib::inquiries::connect(IsFullDuplexInq_, IsFullDuplex);
				}
				void contact_IsSendActive(hmLib::inquiries::inquiry<bool>& IsSendActiveInq_){
					hmLib::inquiries::connect(IsSendActiveInq_, IsSendActiveFlag);
				}

			public:
				cFullRecvTimeSendMessageDriver(itfMessageHost* pMessage_, itfCom* pCom_, clock::duration Interval_, clock::duration TimeOut_, bool IsFullDuplex_, bool SyncFlag_)
					:pMessage(pMessage_)
					, pCom(pCom_)
					, IsFullDuplex(IsFullDuplex_)
					, Interval(Interval_)
					//,SyncFlag(SyncFlag_)
					, TimeOut(TimeOut_)
					, RecevedFlag(false)
					, IsSendActiveFlag(false)
					, RecvMode(true){
					LastRecvTime = clock::now();
					LastSendTime = clock::now();
				}
				void operator()(void){

					// 全二重モードの場合、受信できる限り全て受信＆一定時間間隔で送信
					if(IsFullDuplex){
						// 受信フェーズ
						while(true){
							if(pCom->empty()){
								if(!pCom->can_sync())break;
								pCom->sync();
								pMessage->setup_listen();
								// packet受信処理
								LastRecvTime = clock::now();
								signal_inform_Received(LastRecvTime);

								continue;
							}
							pMessage->listen(pCom->recv());
						}

						// 送信フェーズ
						if(IsSendActiveFlag && (LastSendTime + Interval < clock::now())){
							pMessage->setup_talk();
							while(true){
								if(pCom->full())break;

								datum Dat;
								if(pMessage->talk(Dat))break;

								pCom->send(Dat);
							}
							if(pCom->can_flush())pCom->flush();

							// 送信が終了したことを発信
							LastSendTime = clock::now();
							signal_inform_Sended(LastSendTime);

						}

					}
					// 半二重もーどの場合
					else{
						// 受信フェーズ
						if(RecvMode){
							while(true){
								// 受信が終了しているのかを確認
								if(IsFinishedRecvInq() && pCom->empty() && !pCom->can_sync()){
									RecvMode = false;
									// 受信が終了したことを通知
									LastRecvTime = clock::now();
									signal_inform_Received(LastRecvTime);
								}
								// 受信
								if(pCom->empty()){
									if(!pCom->can_sync())break;
									pCom->sync();
									pMessage->setup_listen();
									continue;
								}
								pMessage->listen(pCom->recv());

							}
						}

						// 送信フェーズ
						if(IsSendActiveFlag){
							// 受信モードは終了しており、かつインターバルをこえてる場合
							if(!RecvMode && ((clock::now() - LastSendTime) > Interval)){
								//送信処理開始
								pMessage->setup_talk();
								// 送信データ制作
								while(true){
									if(!pCom->full()){
										datum Dat;
										if(pMessage->talk(Dat))break;
										pCom->send(Dat);
									}
								}
								// 送信
								if(pCom->can_flush()){
									pCom->flush();
									RecvMode = true;

									// 送信が終了したことを通知
									LastSendTime = clock::now();
									signal_inform_Sended(LastSendTime);
								}
							}
							// 受信データがTimeOut内で来なかった場合
							//  VMCのリセットをかけてから、
							//  仕方ないからもう一回送ってみる
							else if(clock::now() - LastSendTime > TimeOut){
								// VMC リセットのコマンドを送る　
								signal_require_VMC_force_end_recv();
								signal_require_VMC_force_end_send();

								// Time Out したことを通知
								signal_inform_TimeOut(clock::now());

								//送信処理開始
								pMessage->setup_talk();
								// 送信データ制作
								while(true){
									if(!pCom->full()){
										datum Dat;
										if(pMessage->talk(Dat))break;
										pCom->send(Dat);
									}
								}
								// 送信
								if(pCom->can_flush()){
									pCom->flush();
									RecvMode = true;

									// 送信が終了したことを通知
									LastSendTime = clock::now();
									signal_inform_Sended(LastSendTime);
								}
							}
						}
					}


				}
			};
			class cFHDxMessageDriver{
				using datum = message::datum;
			private:
				itfCom* pCom;
				itfMessageHost* pMessage;
				//前回受信した時間
				clock::time_point LastRecvTime;
				//前回送信した時間
				clock::time_point LastSendTime;
				//パケットを待っているかどうか
				bool IsWaitPacket;
			private:
				//送信自体を行うかどうかのフラグ
				bool IsSendActiveFlag;
				//全二重、半二重の選択フラグ
				bool IsFullDuplex;
				//半二重/全二重専用、送信間隔
				clock::duration Interval;
				//半二重専用、受信待ちをあきらめて再送するまでの時間
				clock::duration TimeOut;
			private:
				void setIsSendActiveFlag(bool IsSendActive_){ IsSendActiveFlag = IsSendActive_; }
				void setIsFullDuplex(bool IsFullDuplex_){ IsFullDuplex = IsFullDuplex_; }
				void setInterval(clock::duration Interval_){ Interval = Interval_; }
				void setTimeOut(clock::duration TimeOut_){ TimeOut = TimeOut_; }
			private:
				hmLib::signals::unique_connections connections;
			public:
				// inquire 系列の定義
				hmLib::inquiries::inquiry<bool> IsFinishedRecvInq;
				// VMC関連 signal
				boost::signals2::signal<void(void)> signal_require_VMC_force_end_recv;
				boost::signals2::signal<void(void)> signal_require_VMC_force_end_send;
				boost::signals2::signal<void(clock::time_point)> signal_informTimeOut;
				boost::signals2::signal<void(clock::time_point)> signal_informFinSend;
				boost::signals2::signal<void(clock::time_point)> signal_informFinReceive;
			public:
				// connection
				void slot_setIsSendActiveFlag(boost::signals2::signal<void(bool)> &sig_){
					connections(sig_.connect([&](bool v)->void{this->setIsSendActiveFlag(v); }));
				}
				void slot_setIsFullDuplex(boost::signals2::signal<void(bool)> &sig_){
					connections(sig_.connect([&](bool v)->void{this->setIsFullDuplex(v); }));
				}
				void slot_setInterval(boost::signals2::signal<void(clock::duration)> &sig_){
					connections(sig_.connect([&](clock::duration v)->void{this->setInterval(v); }));
				}
				void slot_setTimeOut(boost::signals2::signal<void(clock::duration)> &sig_){
					connections(sig_.connect([&](clock::duration v)->void{this->setTimeOut(v); }));
				}

				// Contact関数
				void contactIsSendActive(hmLib::inquiries::inquiry<bool>& IsSendActiveInq_){
					hmLib::inquiries::connect(IsSendActiveInq_, IsSendActiveFlag);
				}
				void contactIsFullDuplex(hmLib::inquiries::inquiry<bool>& IsFullDuplexInq_){
					hmLib::inquiries::connect(IsFullDuplexInq_, IsFullDuplex);
				}
				void contactInterval(hmLib::inquiries::inquiry<clock::duration>& IntervalInq_){
					hmLib::inquiries::connect(IntervalInq_, Interval);
				}
				void contactTimeOut(hmLib::inquiries::inquiry<clock::duration>& TimeOutInq_){
					hmLib::inquiries::connect(TimeOutInq_, TimeOut);
				}

			public:
				cFHDxMessageDriver(itfMessageHost* pMessage_, itfCom* pCom_, bool IsFullDuplex_, clock::duration Interval_, clock::duration TimeOut_)
					:pMessage(pMessage_)
					, pCom(pCom_)
					, IsSendActiveFlag(false)
					, IsFullDuplex(IsFullDuplex_)
					, Interval(Interval_)
					, TimeOut(TimeOut_){
					LastRecvTime = clock::now();
					LastSendTime = clock::now();
				}
				void operator()(void){
					//受信処理
					while(true){
						//空になっても、syncできる限りsyncして読み込みを続ける
						if(pCom->empty()){
							if(!pCom->can_sync()){
								break;
							}

							pCom->sync();
							pMessage->setup_listen();
							// packet受信処理
							LastRecvTime = clock::now();
							signal_informFinReceive(LastRecvTime);

							continue;
						}
						pMessage->listen(pCom->recv());
						if(IsFinishedRecvInq())IsWaitPacket = false;
					}

					// 送信可能でなければ、終了
					if(!IsSendActiveFlag || pCom->full())return;

					// 全二重モードの場合
					if(IsFullDuplex){
						//一定時間経過していなければ、終了
						if(LastSendTime + Interval > clock::now())return;
					}//半二重モードの場合
					else{
						//一定時間経過していなければ、終了
						if(LastSendTime + Interval > clock::now())return;
						//パケット待ち中で、TimeOut時間経過していなければ、終了
						if(IsWaitPacket && (LastSendTime + TimeOut > clock::now()))return;
					}

					//送信処理
					pMessage->setup_talk();
					while(true){
						if(pCom->full())break;

						datum Dat;
						if(pMessage->talk(Dat))break;

						pCom->send(Dat);
					}
					if(pCom->can_flush())pCom->flush();

					IsWaitPacket = true;

					// 送信が終了したことを発信
					LastSendTime = clock::now();
					signal_informFinSend(LastSendTime);
				}
			};
		}
	}

}
#
#endif
