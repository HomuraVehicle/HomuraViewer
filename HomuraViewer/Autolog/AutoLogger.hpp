#ifndef HMR_AUTOLOG_AUTOLOGGER_INC
#define HMR_AUTOLOG_AUTOLOGGER_INC 100
#
#include<vector>
#include<fstream>
#include<hmLib/signals.hpp>
#include<hmLib/inquiries.hpp>
#include"Chrono.hpp"
#include"AutoLoggerClient.hpp"
#include<boost/filesystem.hpp>
namespace hmr {
	namespace autolog {
		class cAutoLogger {
			friend class test_cAutoLogger;
		private://User Config
			hmr::clock::time_point ObserveStartPoint;	//観測時間始点
			hmr::clock::duration ObserveInteraval;		//観測間隔
			hmr::clock::duration PreObserveLength;		//観測前準備時間長
			double HomuraTimeRate;
		public:
			void setObserveStartPoint(unsigned int Min_) {
				ObserveStartPoint=hmr::date(2000, 1, 1, 0, 0, 0, 0).to_time()+std::chrono::minutes(Min_);
			}
			unsigned int getObserveStartPoint() const{
				return std::chrono::duration_cast<std::chrono::minutes>(ObserveStartPoint - hmr::date(2000, 1, 1, 0, 0, 0, 0).to_time()).count();
			}
			void setObserveInterval(unsigned int Min_) {
				if(Min_>0)ObserveInteraval=std::chrono::minutes(Min_);
			}
			unsigned int getObserveInterval()const {
				return std::chrono::duration_cast<std::chrono::minutes>(ObserveInteraval).count();
			}
			void setPreObserveLength(unsigned int Min_) {
				if(Min_>=0)PreObserveLength=std::chrono::minutes(Min_);
			}
			unsigned int getPreObserveLength()const {
				return std::chrono::duration_cast<std::chrono::minutes>(PreObserveLength).count();
			}

		private:
			std::vector<itfAutoLoggerClient*> Clients;
		public:
			//Client登録
			void regist(itfAutoLoggerClient& rClient_) { Clients.push_back(&rClient_); }
		private:
			void setup_observe_forAllClients() {
				for(itfAutoLoggerClient* pClient:Clients) {
					pClient->setup_observe();
				}
			}
			bool observe_forAllClients(hmr::clock::time_point ObserveTime_) {
				bool IsLock=false;
				for(itfAutoLoggerClient* pClient:Clients) {
					IsLock=(pClient->observe(ObserveTime_)||IsLock);
				}
				return IsLock;
			}

		public:
			enum class status {
				SetSleep,	//Sleepモード設定中
				ReqSleep,	//Sleepモードへ変更中
				Sleep,		//Sleep中
				Call,		//再接続トライ中
				ReqNormal,	//Normalモードへ変更中
				Observe		//観測中
			};
		private://Inner Status
			status Status;
			hmr::clock::time_point NextAwake;
			hmr::clock::time_point NextObserve;
			hmr::clock::time_point NextCommand;
			hmr::clock::time_point NextCall;
		private:
			//現在からのNext時間産出
			void setNextObserveTime() {
				NextObserve = ObserveStartPoint 
					+ ((std::chrono::duration_cast<std::chrono::seconds>(hmr::clock::now()-ObserveStartPoint).count()/std::chrono::duration_cast<std::chrono::seconds>(ObserveInteraval).count()) + 1)*ObserveInteraval;
				NextAwake = NextObserve - PreObserveLength;
			}
			//現在からのスリープ時間算出用
			std::pair<int, int> getSleepTime() {
				std::pair<int, int> SleepTimePair;

				//SleepTime
				SleepTimePair.first=static_cast<int>(std::chrono::duration_cast<std::chrono::seconds>(NextAwake-hmr::clock::now() - get_duration<duration_tag::sleep_command_lag>()).count()*HomuraTimeRate);
				SleepTimePair.second=static_cast<int>(std::chrono::duration_cast<std::chrono::seconds>(NextObserve-NextAwake + get_duration<duration_tag::sleep_command_lag>()).count()*HomuraTimeRate);
				return SleepTimePair;
			}
			//各種ラグ時間産出
			struct duration_tag {
				struct command_delay{};
				struct call_delay{};
				struct awake_wait_limit{};
				struct com_connect_limit{};
				struct com_disconnect_limit{};
				struct sleep_command_lag {};
				struct observe_limit {};
			};
			template<typename duration_tag_> hmr::clock::duration get_duration();
			template<> hmr::clock::duration get_duration<duration_tag::command_delay>(){return std::chrono::seconds(5); }
			template<> hmr::clock::duration get_duration<duration_tag::call_delay>() { return std::chrono::seconds(10); }//return std::chrono::minutes(2);}
			template<> hmr::clock::duration get_duration<duration_tag::awake_wait_limit>() { return std::chrono::seconds(45); }//return std::chrono::minutes(5);}
			template<> hmr::clock::duration get_duration<duration_tag::com_connect_limit>(){return std::chrono::seconds(5); }
			template<> hmr::clock::duration get_duration<duration_tag::com_disconnect_limit>(){return std::chrono::seconds(10); }
			template<> hmr::clock::duration get_duration<duration_tag::sleep_command_lag>(){return std::chrono::seconds(30); }
			template<> hmr::clock::duration get_duration<duration_tag::observe_limit>(){return std::chrono::minutes(10); }
		public:
			status getStatus()const { return Status; }
			std::string getStatusStr()const {
				if(IsReqStart==false)return "無効";
				switch(Status) {
				case status::SetSleep:
					return "休止準備";
				case status::ReqSleep:
					return "休止要求";
				case status::Sleep:
					return "休止中";
				case status::Call:
					return "接続";
				case status::ReqNormal:
					return "観測要求";
				case status::Observe:
					return "観測中";
				default:
					return "不明";
				}
			}
			hmr::clock::time_point getNextAwake()const {return NextAwake;}
			hmr::clock::time_point getNextObserve()const {return NextObserve;}
			hmr::clock::time_point getNextCommand()const {return NextCommand;}
			hmr::clock::time_point getNextCall()const {return NextCall;}
		public:
			//DeviceManagerコントロール
			boost::signals2::signal<void(bool, int, int)> signal_setSleepMode;
			hmLib::inquiries::inquiry<bool> inquiry_isReadySleep;
			boost::signals2::signal<void(bool)> signal_exeSleepMode;
			boost::signals2::signal<void(bool)> signal_exeNormalMode;
			hmLib::inquiries::inquiry<bool> inquiry_isNormalMode;

			//IOコントロール

			//Comコントロール
			hmLib::inquiries::inquiry<clock::time_point> inquiry_getLastRecvTime;
			boost::signals2::signal<void(void)> signal_clearSendBuf;

			//Operatorコントロール
			boost::signals2::signal<void(bool)> signal_setIsSendActive;

			//GateSwitcherコントロール
			boost::signals2::signal<void(void)> signal_openFoma;
			boost::signals2::signal<void(void)> signal_closeFoma;

		private:
			bool IsStart;		//現在システム稼働中か？
			bool IsReqStart;	//現在システムに対する要求は稼働状態か？

		private:
			std::ofstream flog;
				
		public:
			cAutoLogger()
				: ObserveStartPoint(hmr::date(2000, 1, 1, 0, 0, 0, 0).to_time())
				, ObserveInteraval(std::chrono::minutes(60))
				, PreObserveLength(std::chrono::minutes(10))
				, HomuraTimeRate(1.0)
				, Status(status::SetSleep)
				, IsStart(false)
				, IsReqStart(false) {
			}
			//自動ログ開始要求
			bool start() {
				IsReqStart=true;
				//実際のスタートはoperator()で行われる

				return false;
			}
			//自動ログ終了要求
			void stop() {
				IsReqStart=false;
				//実際のストップはoperator()で行われる
			}
			//自動ログ要求中か？
			bool is_start()const {
				return IsReqStart;
			}
			//自動ログが稼働している最中か？
			bool is_start_autolog()const {
				return IsStart;
			}
			//自動ログ機能駆動関数
			void operator()(void) {
				//新規実行要求が出ていた場合
				if(IsReqStart && !IsStart) {
					IsStart=true;

					// Passを指定してしまう				
					hmr::date Date;
					Date.from_time(hmr::clock::now());
					std::string Path = (boost::format("autolog_%02d%02d%02d_%02d%02d%02d")%(Date.Year%100)%Date.Month%Date.Day%Date.Hour%Date.Min%Date.Sec).str();

					boost::filesystem::create_directory(boost::filesystem::current_path()/Path);
				
//					std::string Path="Autolog";
					flog.open(Path+"\\"+"autolog.txt",std::ios_base::out|std::ios_base::app);
					
					flog<<hmr::time_to_ymdhms(hmr::clock::now())<<" "<<"start autolog"<<std::endl;

					for(auto Ptr:Clients){
						Ptr->start(Path+"\\");
					}


					setNextObserveTime();
					flog<<hmr::time_to_ymdhms(hmr::clock::now())<<" "<<"NextAwake="<<hmr::time_to_ymdhms(NextAwake)<<std::endl;
					flog<<hmr::time_to_ymdhms(hmr::clock::now())<<" "<<"NextObserve="<<hmr::time_to_ymdhms(NextObserve)<<std::endl;

					//次の起床時間まで5分以上あれば
					if(hmr::clock::now()+get_duration<duration_tag::awake_wait_limit>()>NextAwake) {
						setup_observe_forAllClients();
						Status=status::Observe;
						flog<<hmr::time_to_ymdhms(hmr::clock::now())<<" "<<"status Observe"<<std::endl;
					} else {
						Status=status::SetSleep;
						flog<<hmr::time_to_ymdhms(hmr::clock::now())<<" "<<"status SetSleep"<<std::endl;
					}
				}
				
				//status change
				while(IsStart) {
					//自動ログ停止要求が出ていた場合
					if(!IsReqStart && (Status==status::SetSleep || Status==status::Observe)) {
						IsStart=false;
						
						for(auto Ptr:Clients) {
							Ptr->stop();
						}

						flog<<hmr::time_to_ymd(hmr::clock::now())<<" "<<hmr::time_to_hms(hmr::clock::now())<<" "<<"stop autolog"<<std::endl;
						flog.close();

						return;
					}

					switch(Status) {
					case status::SetSleep:
						//すでにSleepへの遷移準備が整っている場合
						if(inquiry_isReadySleep()) {
							//ReqSleepにしてやり直し
							Status=status::ReqSleep;
							flog<<hmr::time_to_ymdhms(hmr::clock::now())<<" "<<"status ReqSleep"<<std::endl;
							continue;
						}
						//コマンドが送れるなら
						if(NextCommand < hmr::clock::now()) {
							auto SleepTime=getSleepTime();
	
							signal_setSleepMode(true, SleepTime.first, SleepTime.second);
							flog<<hmr::time_to_ymdhms(hmr::clock::now())<<" "<<"signal setSleepMode(true,"<<SleepTime.first<<","<<SleepTime.second<<")"<<std::endl;

							NextCommand = hmr::clock::now()+get_duration<duration_tag::command_delay>();
						}
						return;
					case status::ReqSleep:
						//通信途絶から10秒以上たっていれば
						if(clock::now()-inquiry_getLastRecvTime()>get_duration<duration_tag::com_disconnect_limit>()) {
							signal_setIsSendActive(false);
							signal_clearSendBuf();
							signal_closeFoma();
							flog<<hmr::time_to_ymdhms(hmr::clock::now())<<" "<<"signal setIsSendActive(false), clearSendBuf(), closeFoma()"<<std::endl;

							Status=status::Sleep;
							flog<<hmr::time_to_ymdhms(hmr::clock::now())<<" "<<"status Sleep"<<std::endl;
							continue;
						}
						//コマンドが送れるなら
						if(NextCommand < hmr::clock::now()) {
							signal_exeSleepMode(true);
							flog<<hmr::time_to_ymdhms(hmr::clock::now())<<" "<<"signal exeSleepMode(true)"<<std::endl;
							NextCommand = hmr::clock::now()+get_duration<duration_tag::command_delay>();
						}
						return;
					case status::Sleep:
						//起床時間を過ぎていれば
						if(NextAwake < hmr::clock::now()) {
							Status=status::Call;
							flog<<hmr::time_to_ymdhms(hmr::clock::now())<<" "<<"status Call"<<std::endl;
							continue;
						}
						return;
					case status::Call:
						//通信が5秒以内に存在すれば
						if(clock::now()-inquiry_getLastRecvTime()<get_duration<duration_tag::com_connect_limit>()) {
							signal_setIsSendActive(true);
							flog<<hmr::time_to_ymdhms(hmr::clock::now())<<" "<<"signal setIsSendActive(true)"<<std::endl;
							Status=status::ReqNormal;
							flog<<hmr::time_to_ymdhms(hmr::clock::now())<<" "<<"status ReqNormal"<<std::endl;
							continue;
						}
						//コールが送れるなら
						if(NextCall < hmr::clock::now()) {
							signal_openFoma();
							NextCall = hmr::clock::now()+get_duration<duration_tag::call_delay>();
							flog<<hmr::time_to_ymdhms(hmr::clock::now())<<" "<<"signal openFoma()"<<std::endl;
						}
						return;
					case status::ReqNormal:
						//モードがノーマルになっていれば
						if(inquiry_isNormalMode()) {
							//通知
							setup_observe_forAllClients();
							Status=status::Observe;
							flog<<hmr::time_to_ymdhms(hmr::clock::now())<<" "<<"status PreObserve"<<std::endl;

							continue;
						}
						//コマンドが送れるなら
						if(NextCommand < hmr::clock::now()) {
							signal_exeNormalMode(true);
							flog<<hmr::time_to_ymdhms(hmr::clock::now())<<" "<<"signal exeNormalMode(true)"<<std::endl;
							NextCommand = hmr::clock::now()+get_duration<duration_tag::command_delay>();
						}
						return;
					case status::Observe:
						//すべてのClientがロックを解除していれば あるいは、観測限界を超えていれば
						if(!observe_forAllClients(NextObserve) || NextObserve+get_duration<duration_tag::observe_limit>()<clock::now()) {
							setNextObserveTime();
							flog<<hmr::time_to_ymdhms(hmr::clock::now())<<" "<<"NextAwake="<<hmr::time_to_ymdhms(NextAwake)<<std::endl;
							flog<<hmr::time_to_ymdhms(hmr::clock::now())<<" "<<"NextObserve="<<hmr::time_to_ymdhms(NextObserve)<<std::endl;
							Status=status::SetSleep;
							flog<<hmr::time_to_ymdhms(hmr::clock::now())<<" "<<"status SetSleep"<<std::endl;
							continue;
						}
						return;
					}
				}
			}
		};
	}
}
#
#endif
