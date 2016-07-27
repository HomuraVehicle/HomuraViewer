#ifndef HMR_DXOPERATOR_SUI_INC
#define HMR_DXOPERATOR_SUI_INC 100

#include <iostream>
#include <string>
#include "hmLibVer.hpp"
#include <hmLib_v2/hmBasic.hpp>
#include <hmLib_v2/dxObject.hpp>
#include <hmLib_v2/dxArea.hpp>
#include <hmLib/signals.hpp>
#include <boost/signals2.hpp>
#include "hmrDxBUI.hpp"
//#include "hmrBaseDX.hpp"

//#define DEBUG_MODE 1

#define FLAG_UPDATE_DURATION 100 // Flag の更新頻度(ms)

/* ---- hmrDxOperatorSUI-----

hmrDxOperator.hpp :v1_00/130615 amby
	DxOperatorSUIの製作開始
	実装
*/


namespace hmr{
	namespace viewer{
		// 描画クラス static
		class dxosOperatorSUI :public dxosBUI{
		private:
			bool startFlag;
			clock::duration timeOut;
			clock::duration timeInterval;
			bool isFullDuplexFlag;

			bool isTimeOutedFlag;
			clock::time_point timeOutedTime;

			bool isSendedFlag;
			clock::time_point sendedTime;

			bool isReceivedFlag;
			clock::time_point receivedTime;


			// 前回リセットしたデータの時間
			clock::time_point prvTime;

			//slot 
			hmLib::signals::unique_connections connections;

		public:
			// signal 系列の定義
			boost::signals2::signal<void(bool)> signal_require_setStartFlag;
			boost::signals2::signal<void(clock::duration)> signal_require_setTimeOut;
			boost::signals2::signal<void(clock::duration)> signal_require_setTimeInterval;
			boost::signals2::signal<void(bool)> signal_require_isFullDuplex;

			// inquire 系列の定義
			hmLib::inquiries::inquiry<bool> startFlagInq;
			hmLib::inquiries::inquiry<clock::duration> timeOutInq;
			hmLib::inquiries::inquiry<clock::duration> timeIntervalInq;
			hmLib::inquiries::inquiry<bool> isFullDuplexFlagInq;

			// Slot 系列の定義
			void setTimeOutedFlag(clock::time_point _time){
				isTimeOutedFlag = true;
				timeOutedTime = _time;
			}
			void setSendedFlag(clock::time_point _time){
				isSendedFlag = true;
				sendedTime = _time;
			}
			void setReceivedFlag(clock::time_point _time){
				isReceivedFlag = true;
				receivedTime = _time;
			}

		public:
			//connections
			void slot_setIsTimeOutedFlag(boost::signals2::signal<void(clock::time_point)> &sig_){
				connections(sig_.connect([&](clock::time_point _time)->void{this->setTimeOutedFlag(_time); }));
			}
			void slot_setIsSendedFlag(boost::signals2::signal<void(clock::time_point)> &sig_){
				connections(sig_.connect([&](clock::time_point _time)->void{this->setSendedFlag(_time); }));
			}
			void slot_setIsReceivedFlag(boost::signals2::signal<void(clock::time_point)> &sig_){
				connections(sig_.connect([&](clock::time_point _time)->void{this->setReceivedFlag(_time); }));
			}

		public:
			//virtual int fndraw(dxO& dxf);
			int normal_draw(dxO& dxo)override{
				//int ans;

				// Flag 処理を行う
				//  一定時間ごとにリセット処理を行う
				if(clock::now() - prvTime > std::chrono::milliseconds(FLAG_UPDATE_DURATION)){
					isTimeOutedFlag = false;
					isSendedFlag = false;
					isReceivedFlag = false;
				}

				// 自分自身を名乗って状態表示
				//dxo.draw(Pint(5,5),dxoStrP(Pint(70,20),"Operator",getClr(normal,strobj)));

				// is Sending 状態表示
				try{
					startFlag = startFlagInq();

					if(startFlag){
						dxo.draw(Pint(5, 5), dxoStrP(Pint(140, 20), "Sending mode", getClr(normal, strobj)));
					} else{
						dxo.draw(Pint(5, 5), dxoStrP(Pint(140, 20), "Not sending", getClr(normal, strobj)));
					}
				}
				catch(const hmLib::inquiries::unconnected_exception&){
					dxo.draw(Pint(5, 5), dxoStr("Inquiry::strtFlag 取得失敗"));
				}
				catch(const hmLib::exceptions::exception& Excp){
					dxo.draw(Pint(5, 5), dxoStr(Excp.what()));
				}

				// Sending start と　Duplex　の設定
				// send start
				if(dxo.draw(Pint(5, 30), dxoButIO(Pint(105, 20), "Send Start", getClr(normal, butobj), startFlag)) == 1){
					startFlag = !startFlag;
					signal_require_setStartFlag(startFlag);
				}
				// Duplex Mode
				try{
					isFullDuplexFlag = isFullDuplexFlagInq();
					std::string str;
					if(isFullDuplexFlag){
						str = "Full Duplex";
					} else{
						str = "Half Duplex";
					}
					if(dxo.draw(Pint(115, 30), dxoButIO(Pint(105, 20), str, getClr(normal, butobj), isFullDuplexFlag)) == 1){
						isFullDuplexFlag = !isFullDuplexFlag;
						signal_require_isFullDuplex(isFullDuplexFlag);
					}
				}
				catch(const hmLib::inquiries::unconnected_exception&){
					dxo.draw(Pint(115, 30), dxoStr("Inquiry::isDuplex 取得失敗"));
				}
				catch(const hmLib::exceptions::exception& Excp){
					dxo.draw(Pint(115, 30), dxoStr(Excp.what()));
				}

				// Interval と　timeout
				 //interval
				try{
					// データ更新
					timeInterval = timeIntervalInq();
					// 扱いやすくINTにしておく
					int Interval_ms = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(timeInterval).count());

					// Interval 設定
					dxRGB intClr;
					if(isSendedFlag){
						intClr = getClr(active, butobj);
					} else{ intClr = getClr(normal, butobj); }

					int ans = dxo.draw(Pint(5, 55), dxoButLRIO(Pint(105, 20), hmstd::fStr("Itvl %d", Interval_ms), intClr, true));

					if(ans == 1){
						if(Interval_ms == 0){
							Interval_ms = 250;
						} else{
							Interval_ms *= 2;
							if(Interval_ms > 4000){
								if(!isFullDuplexFlag) Interval_ms = 0;
								else  Interval_ms = 250;
							}
						}
					} else if(ans == 10){
						if(Interval_ms == 0){
							Interval_ms = 4000;
						} else{
							Interval_ms /= 2;
							if(Interval_ms < 250){
								if(!isFullDuplexFlag) Interval_ms = 0;
								else  Interval_ms = 4000;
							}
						}
					}

					// データ変換
					timeInterval = std::chrono::milliseconds(Interval_ms);
					// signal 発信
					signal_require_setTimeInterval(timeInterval);

				}
				catch(const hmLib::inquiries::unconnected_exception&){
					// Inquire にアクセスできなかった！！　でも、そんなのメイン関数には関係ないからもみ消す
					dxo.draw(Pint(5, 55), dxoStr("Inquiry::Intval 取得失敗"));
				}
				catch(const hmLib::exceptions::exception& Excp){
					dxo.draw(Pint(5, 55), dxoStr(Excp.what()));
				}

				// timeout
				try{
					// データ更新
					timeOut = timeOutInq();
					// 扱いやすくINTにしておく
					int TimeOut_ms = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(timeOut).count());

					// timeout 設定
					dxRGB timClr;
					if(isSendedFlag){
						timClr = getClr(active, butobj);
					} else{ timClr = getClr(normal, butobj); }

					int ans = dxo.draw(Pint(115, 55), dxoButLRIO(Pint(105, 20), hmstd::fStr("TOut %d", TimeOut_ms), timClr, true));

					if(ans == 1){
						TimeOut_ms *= 2;
						if(TimeOut_ms > 8000)TimeOut_ms = 1000;
					} else if(ans == 10){
						TimeOut_ms /= 2;
						if(TimeOut_ms < 1000)TimeOut_ms = 8000;
					}

					// データ変換
					timeOut = std::chrono::milliseconds(TimeOut_ms);
					// signal 発信
					signal_require_setTimeOut(timeOut);

				}
				catch(const hmLib::inquiries::unconnected_exception&){
					// Inquire にアクセスできなかった！！　でも、そんなのメイン関数には関係ないからもみ消す
					dxo.draw(Pint(115, 55), dxoStr("Inquiry::timeOut 取得失敗"));
				}
				catch(const hmLib::exceptions::exception& Excp){
					dxo.draw(Pint(115, 55), dxoStr(Excp.what()));
				}

				return 0;
			}

			int extend_draw(dxO& dxo)override{
				// draw
				normal_draw(dxo);

				// 各状態について描画する
				dxo.draw(Pint(5, 80), dxoStrP(Pint(105, 20), "Sended at", getClr(normal, strobj)));
				if(isSendedFlag){
					dxo.draw(Pint(115, 80), dxoStrP(Pint(105, 20), time_to_hms(sendedTime), getClr(active, strobj)));
				} else{
					dxo.draw(Pint(115, 80), dxoStrP(Pint(105, 20), time_to_hms(sendedTime), getClr(normal, strobj)));
				}

				dxo.draw(Pint(5, 105), dxoStrP(Pint(105, 20), "Recieved at", getClr(normal, strobj)));
				if(isReceivedFlag){
					dxo.draw(Pint(115, 105), dxoStrP(Pint(105, 20), time_to_hms(receivedTime), getClr(active, strobj)));
				} else{
					dxo.draw(Pint(115, 105), dxoStrP(Pint(105, 20), time_to_hms(receivedTime), getClr(normal, strobj)));
				}

				dxo.draw(Pint(5, 130), dxoStrP(Pint(105, 20), "TimeOut at", getClr(normal, strobj)));
				if(isTimeOutedFlag){
					dxo.draw(Pint(115, 130), dxoStrP(Pint(105, 20), time_to_hms(timeOutedTime), getClr(active, strobj)));
				} else{
					dxo.draw(Pint(115, 130), dxoStrP(Pint(105, 20), time_to_hms(timeOutedTime), getClr(normal, strobj)));
				}

				return 0;

			}

			status getStatus()const override{
				if(isTimeOutedFlag) return error;
				else return normal;
			}
		public:
			// constructor
			dxosOperatorSUI(void) :dxosBUI("Operator", 80, 155), startFlag(false), timeOut(std::chrono::milliseconds(250)), timeInterval(std::chrono::milliseconds(250)), isFullDuplexFlag(true),
				isTimeOutedFlag(false), isSendedFlag(false), isReceivedFlag(false){
				prvTime = clock::now();
				timeOutedTime = clock::now();
				sendedTime = clock::now();
				receivedTime = clock::now();
			}
		};
	}
}

#
#endif
