#ifndef HMR_DEVICEMANAGE_INC
#define HMR_DEVICEMANAGE_INC 100
#
/*==========hmrDeviceManage============
hmrDeviceManage v1_00/130928 amby
	初期構築開始

*/

/*
===DeviceManager===
'D'
スリープ設定コマンド（実行はされない）
>> 0x10,SleepLength[2],WakeupLength[2]
	寝る時間とその後起きる時間を送信
	寝る時間を0にすると、ECOモード
		クロック低速のまま、センサ系の利用が可能
	起きる時間を0にすると、エラー
<< 0x10,SleepLength[2],WakeupLength[2],Code
	設定値と実行用コードが返ってくる
<< 0xF1
	設定値が異常な場合は、0xF1が戻る

スリープ実行コマンド
>> 0x11,Code
	実行用コードを送信して、実行を確認
		直前のスリープ設定コマンドで取得した実行用コードのみ有効
<< なし（入眠）


ローミング設定コマンド（実行はされない）
>> 0x20 ModuleID
	ModuleIDの通信モジュールを有効にする
		ModuleID 0:RF, 1:MP
	1分間待機した後、元のモジュールに戻る(正確には一分ごとに探す相手を変える)
<< 0x20,ModuleID,Code
	設定値と実行用コードが返ってくる
<< 0xF2
	設定値が異常な場合は、0xF1が戻る

ローミング実行コマンド
>> 0x21,Code
	実行用コードを送信して、実行を確認
		直前のスリープ設定コマンドで取得した実行用コードのみ有効
<< なし（ローミング）


ターボモード実行コマンド
>> 0x30
	ターボモード実行
<< 0x30

モードリセット（ノーマルモード実行）コマンド
>> 0x40
	ノーマルモード
<< 0x40


*/

#include "hmLibVer.hpp"
#include<boost/signals2.hpp>
#include<hmLib_v3_05/signals.hpp>
#include <hmLib_v3_05/inquiries.hpp>
#include "hmrItfMessage.hpp"
#include "hmrFlagirl.hpp"


namespace hmr{
//	class cDeviceMessageStrategy:public cMessage:iMessageStrategy{
//		typedef cMessage::iMessageStrategy base_type;
	class cDevMngMsgAgent:public itfMessageAgent{
	public:
		typedef enum{NORMAL = 0, SLEEP=1, ROAM=2, UNKNOWN=3} DevMODE;
		typedef enum{NORMALCLOCK = 0, LOWCLOCK = 1, HIGHCLOCK = 2, UNKNOWNCLOCK = 3}ClockMODE;

	private:
		//status
		DevMODE devMODE;
		//DevMODE previousMODE;

		// sleep 系列の変数
		int SleepLength;
		int WakeupLength;
		unsigned char SleepCode;
		
		bool SetSleepFlag; // Sleepコマンドを送るフラグ
		bool SendSleepFlag;   // talk関数内で送るかどうかのフラグ
		bool ReadySleepFlag; // Sleep実行コマンドを送れる状態かどうか
		bool ExeSleepFlag;  // SLEEPを実行	

		// ROAM　系列の変数
		bool UseMP;
		int RoamLength;
		unsigned char RoamCode;

		bool SetRoamFlag;
		bool SendRoamFlag;
		bool ReadyRoamFlag;
		bool ExeRoamFlag;

		// Turbo　系列の変数
		//bool SetTurboFlag;
		//bool SendTurboFlag;

		// Reset (Normal)　系列
		bool SetNormalFlag;
		bool SendNormalFlag;

		// KILL 系列
		unsigned char KillCode;
		bool SetKillFlag;
		bool SendKillFlag;
		bool ReadyKillFlag;
		bool ExeKillFlag;

		// clock 変更系列
		ClockMODE clockMODE; // 現在のクロックモード
		ClockMODE commandClockMODE;  // コマンドとして送るクロックモード
		bool SetClockFlag;
		bool SendClockFlag;

		// Info 系列
		bool SetInfoFlag;
		bool SendInfoFlag;

	public:
		cDevMngMsgAgent():SetSleepFlag(false),ReadySleepFlag(false),ExeSleepFlag(false),SetRoamFlag(false),ReadyRoamFlag(false),
			ExeRoamFlag(false),SetNormalFlag(false), SetKillFlag(false), ReadyKillFlag(false),  ExeKillFlag(false), 
			SetClockFlag(false), SendClockFlag(false), SetInfoFlag(false), SendInfoFlag(false){
			devMODE = UNKNOWN;
			clockMODE = UNKNOWNCLOCK;
			SleepLength = 10;
			WakeupLength = 10;
			RoamLength = 10;
			UseMP = true;
			SleepCode = 0x00;
			RoamCode = 0x00;
			KillCode = 0x00;
		}

		bool listen(datum::time_point Time_, bool Err_,const std::string& Str_)override{
			if(Str_.size()==0)return true;

			// Info 情報
			if(static_cast<unsigned char>(Str_.at(0))==0xA0){
				if(Str_.size() != 4)return true;
				// DevMngMODE
				unsigned char mode = Str_.at(1);
				if(mode == 0x00) devMODE = NORMAL;
				else if(mode == 0x10) devMODE = SLEEP;
				else if(mode == 0x20) devMODE = ROAM;
				// CLOCKMODE
				unsigned char clock = Str_.at(2);
				if(clock == 0x00) clockMODE = NORMALCLOCK;
				else if(clock == 0x01) clockMODE = LOWCLOCK;
				else if(clock == 0x02) clockMODE = HIGHCLOCK;
				// RF or MP
				unsigned char comMode = Str_.at(3);
				if(comMode == 0x00) UseMP = false;
				else UseMP = true;

				// Flag 処理
				SetInfoFlag = false;
				return false;
			}

			// Device MODE　情報
			else if(static_cast<unsigned char>(Str_.at(0))==0x10){
				if(Str_.size() == 1) return true;

				// SLEEP MODE
				if(Str_.at(1) == 0x10){
					if(Str_.size()!=7)return true;
				
					//SleepLength認証
					/*
					if(static_cast<int>(Str_.at(2))
						+static_cast<int>(0x100*static_cast<unsigned char>(Str_.at(3)))
						!=SleepLength
					) return true;
					//WakeupLength認証
					if(static_cast<int>(Str_.at(4))
						+ static_cast<int>(0x100*static_cast<unsigned char>(Str_.at(5)))
						!=WakeupLength
					)return true;
					*/

					//Code取得
					SleepCode=static_cast<unsigned char>(Str_.at(6));

					// Flag 処理
					SetSleepFlag = false;
					ReadySleepFlag = true;

					return false;
				}

				// Sleep設定失敗
				else if(Str_.at(1) == 0xF1){
					return true;
				}

				// Roaming 設定
				else if(Str_.at(1) == 0x20){
					if(Str_.size()!=6)return true;
				
					// Roam Length認証
					if(static_cast<int>(Str_.at(2)) + static_cast<int>(0x100*static_cast<unsigned char>(Str_.at(3)))
						!=RoamLength)return true;
					
					//Code取得
					RoamCode=static_cast<unsigned char>(Str_.at(4));

					// 現在のUseMP を更新　(変更先の情報が来るので逆の状態をとっておく)
					// RF or MP
					unsigned char comMode = Str_.at(5);
					if(comMode == 0x00) UseMP = true; // 次のモードがRFの場合は現状はMPに
					else UseMP = false;// 次のモードがMPの場合は現状はRFに

					// Flag 処理
					SetRoamFlag = false;
					ReadyRoamFlag = true;

					return false;
				}

				// Roaming 設定失敗
				else if(Str_.at(1) ==0xF2){
					return true;
				} 

			/*
			// Turboモード設定
			else if(static_cast<unsigned char>(Str_[0])==0x30){
				if(Str_.size()!=1)return true;
				
				// Flag 処理
				SetTurboFlag = false;
				devMODE = TURBO;
				
				return false;
			}
			*/

				// Normal 設定
				else if(Str_.at(1) == 0x00){
					if(Str_.size()!=2)return true;
				
					// Flag 処理
					SetNormalFlag = false;
					devMODE = NORMAL;

					return false;
				}

			}

			// KILL モードコマンド
			else if(static_cast<unsigned char>(Str_.at(0))==0x20){
				if(Str_.size()!=2)return true;
				
				//Code取得
				KillCode=static_cast<unsigned char>(Str_.at(1));

				// Flag 処理
				SetKillFlag = false;
				ReadyKillFlag = true;

				return false;
			}

			// clock 設定コマンド
			else if(static_cast<unsigned char>(Str_.at(0))==0x30){
				if(Str_.size()!=2)return true;
				
				// CLOCKMODE
				unsigned char clock = Str_.at(1);
				if(clock == 0x00) clockMODE = NORMALCLOCK;
				else if(clock == 0x01) clockMODE = LOWCLOCK;
				else if(clock == 0x02) clockMODE = HIGHCLOCK;

				// Flag 処理
				SetClockFlag = false;

				return false;
			}

			return true;
		
		}
		bool talk(std::string& Str)override{
			Str="";
			if(SendInfoFlag){
				Str.push_back(static_cast<unsigned char>(0xA0));
				SendInfoFlag = false;
				return false;
			}
			else if(SendSleepFlag){
				Str.push_back(static_cast<unsigned char>(0x10));
				Str.push_back(static_cast<unsigned char>(0x10));
				Str.push_back(static_cast<unsigned char>(SleepLength));
				Str.push_back(static_cast<unsigned char>((SleepLength)>>8));
				Str.push_back(static_cast<unsigned char>(WakeupLength));
				Str.push_back(static_cast<unsigned char>(WakeupLength>>8));
				SendSleepFlag = false;
				ReadySleepFlag = false;
				return false;

			}else if(ExeSleepFlag){
				Str.push_back(static_cast<unsigned char>(0x10));
				Str.push_back(static_cast<unsigned char>(0x11));
				Str.push_back(static_cast<unsigned char>(SleepCode));
				ReadySleepFlag = false;
				ExeSleepFlag = false;
				devMODE = SLEEP;
				return false;

			}else if(SendRoamFlag){
				Str.push_back(static_cast<unsigned char>(0x10));
				Str.push_back(static_cast<unsigned char>(0x20));
				//if(UseMP)Str.push_back(static_cast<unsigned char>(0x01));
				//else Str.push_back(static_cast<unsigned char>(0x00));
				Str.push_back(static_cast<unsigned char>(RoamLength));
				Str.push_back(static_cast<unsigned char>(RoamLength>>8));
				SendRoamFlag = false;
				ReadyRoamFlag = false;
				return false;

			}else if(ExeRoamFlag){
				Str.push_back(static_cast<unsigned char>(0x10));
				Str.push_back(static_cast<unsigned char>(0x21));
				Str.push_back(static_cast<unsigned char>(RoamCode));
				ReadyRoamFlag = false;
				ExeRoamFlag = false;
				devMODE = ROAM;
				UseMP = !UseMP;
				return false;
			
			/*
			}else if(SendTurboFlag){
				Str.push_back(static_cast<unsigned char>(0x30));
				SendTurboFlag = false;
				return false;
			*/
			}else if(SendNormalFlag){
				Str.push_back(static_cast<unsigned char>(0x10));
				Str.push_back(static_cast<unsigned char>(0x00));
				SendNormalFlag = false;
				return false;
			}

			else if(SendKillFlag){
				Str.push_back(static_cast<unsigned char>(0x20));
				SendKillFlag = false;
				return false;
			}

			else if(ExeKillFlag){
				Str.push_back(static_cast<unsigned char>(0x21));
				Str.push_back(static_cast<unsigned char>(KillCode));
				ReadyKillFlag = false;
				ExeKillFlag = false;
				//devMODE = DEAD;
				return false;
			}

			else if(SendClockFlag){
				Str.push_back(static_cast<unsigned char>(0x30));
				unsigned char clock;
				if(commandClockMODE == NORMALCLOCK){
					clock = 0x00;
				}else if(commandClockMODE == LOWCLOCK){
					clock = 0x01;
				}else if(commandClockMODE == HIGHCLOCK){
					clock = 0x02;
				}
				Str.push_back(clock);

				SendClockFlag = false;
				return false;
			}

			return true;
		}
		void setup_talk(void)override{
			SendInfoFlag = SetInfoFlag;
			SendSleepFlag = SetSleepFlag;
			SendRoamFlag = SetRoamFlag;
			SendNormalFlag = SetNormalFlag;
			SendKillFlag = SetKillFlag;
			SendClockFlag = SetClockFlag;
			//SendTurboFlag = SetTurboFlag;
			return;
		}

	private:
		hmLib::signals::unique_connections SignalConnections;
		hmLib::inquiries::unique_connections InquiryConnections;

		void set_SleepMode(bool flag_, int sleepLength_, int wakeupLength_){
			SetSleepFlag = flag_;
			WakeupLength = wakeupLength_;
			SleepLength = sleepLength_;
		}
		void exe_SleepMode(bool flag){
			ExeSleepFlag = flag;
			if(!flag){
				ReadySleepFlag = false;
				SetSleepFlag = false;
				SleepCode = 0x00;
			}
		}

		void set_RoamMode(bool flag_, int roamLength_){
			SetRoamFlag = flag_;
			//UseMP = useMP_;
			RoamLength = roamLength_;
		}
		void exe_RoamMode(bool flag){
			ExeRoamFlag = flag;
			if(!flag){
				ReadyRoamFlag = false;
				SetRoamFlag = false;
				RoamCode = 0x00;
			}
		}

		/*
		void set_TurboMode(bool flag_){
			SetTurboFlag = flag_;
		}
		*/

		void set_NormalMode(bool flag_){
			SetNormalFlag = flag_;
		}
		void set_Kill(bool flag_){
			SetKillFlag = flag_;
		}
		void exe_KillMode(bool flag){
			ExeKillFlag = flag;
			if(!flag){
				ReadyKillFlag = false;
				SetKillFlag = false;
				KillCode = 0x00;
			}
		}
		void set_Info(bool flag_){
			SetInfoFlag = flag_;
		}
		void set_ClockMode(bool flag_, ClockMODE clock_){
			commandClockMODE = clock_;
			SetClockFlag = flag_;
		}
		
		DevMODE get_devMODE(void){
			return devMODE;
		}

	public:
		// slot 
		void slot_setSleepMode(boost::signals2::signal<void(bool, int, int)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](bool flag, int slpLength, int wakeLength)->void{set_SleepMode(flag, slpLength, wakeLength);}));				
		}
		void slot_setRoamMode(boost::signals2::signal<void(bool, int)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](bool flag, int roamLength)->void{set_RoamMode(flag, roamLength);}));				
		}
		void slot_exeSleepMode(boost::signals2::signal<void(bool)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](bool flag)->void{exe_SleepMode(flag);}));				
		}
		void slot_exeRoamMode(boost::signals2::signal<void(bool)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](bool flag)->void{exe_RoamMode(flag);}));				
		}
		/*
		void slot_setTurboMode(boost::signals2::signal<void(bool)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](bool flag)->void{set_TurboMode(flag);}));				
		}
		*/
		void slot_setNormalMode(boost::signals2::signal<void(bool)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](bool flag)->void{set_NormalMode(flag);}));				
		}
		void slot_setKillMode(boost::signals2::signal<void(bool)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](bool flag)->void{set_Kill(flag);}));				
		}
		void slot_exeKillMode(boost::signals2::signal<void(bool)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](bool flag)->void{exe_KillMode(flag);}));				
		}
		void slot_setInfo(boost::signals2::signal<void(bool)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](bool flag)->void{set_Info(flag);}));				
		}
		void slot_setClockMode(boost::signals2::signal<void(bool, ClockMODE)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](bool flag, ClockMODE clock)->void{set_ClockMode(flag, clock);}));				
		}


		/*
		void slot_getdevMODE(boost::signals2::signal<DevMODE(void)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](void)->DevMODE{get_devMODE();}));				
		}
		*/

		// inq
		void contact_getDevMODE(hmLib::inquiries::inquiry<DevMODE>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->DevMODE{return devMODE;}));
		}
		void contact_getClockMODE(hmLib::inquiries::inquiry<ClockMODE>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->ClockMODE{return clockMODE;}));
		}

		void contact_getSendingSleepCommand(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->bool{return SetSleepFlag;}));
		}
		void contact_getSendingRoamCommand(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->bool{return SetRoamFlag;}));
		}
		void contact_IsReadySleepExe(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->bool{return ReadySleepFlag;}));
		}
		void contact_IsReadyRoamExe(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->bool{return ReadyRoamFlag;}));
		}

		void contact_getUseMP(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->bool{return UseMP;}));
		}
		

		void contact_getSendingNormalCommand(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->bool{return SetNormalFlag;}));
		}
		void contact_isNormal(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->bool{return devMODE==DevMODE::NORMAL;}));
		}
		void contact_getSendingKillCommand(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->bool{return SetKillFlag;}));
		}
		void contact_IsReadyKillExe(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->bool{return ReadyKillFlag;}));
		}

		void contact_getSendingInfoCommand(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->bool{return SetInfoFlag;}));
		}
		void contact_getSendingClockCommand(hmLib::inquiries::inquiry<bool>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->bool{return SetClockFlag;}));
		}


		
	};
}
#
#endif
