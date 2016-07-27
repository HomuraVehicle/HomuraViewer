#ifndef HMR_CHRONO_INC
#define HMR_CHRONO_INC 100
#
/*========== hmrChrono ============
Chrono
*/

#include "hmLibVer.hpp"
#include <boost/signals2.hpp>
#include <hmLib/signals.hpp>
#include <hmLib/inquiries.hpp>
#include <hmLib/type.h>
#include <HomuraViewer/Message/ItfMessageAgent.hpp>
#include "vmstatus.hpp"
#include <HomuraViewer/Data.hpp>

namespace hmr{
	//Chrono関連
	class cChrono :public itfMessageAgent {
	public:
		typedef std::chrono::milliseconds milliseconds;
	private:
		//アラームの実行間隔
		vmstatus<unsigned int> AutoGetInterval;
		//自動補正機能を実行する
		milliseconds RequestCorrectMachineTime;
		//機体の時間のずれを記録しておく
		milliseconds MachineDifTime;
	private:
		//時刻補正機能
		class ping {
			//Pingの回数記録
			unsigned int Cnt;
			//PIC の内部時間を受け取るコマンドを送った時間と返信の時間
			clock::time_point SendTime;
			clock::time_point RecvTime;
			//累積用
			clock::duration WaitTimeAccumulation;
			clock::duration  DifTimeAccumulation;
		public:
			void reset() {
				Cnt=0;
				WaitTimeAccumulation= std::chrono::milliseconds(0);
				DifTimeAccumulation= std::chrono::milliseconds(0);
			}
			void start() { SendTime = clock::now(); }
			void stop(const clock::time_point& MachineTime_) {
				RecvTime = clock::now();
				WaitTimeAccumulation+=RecvTime-SendTime;
				DifTimeAccumulation+= MachineTime_ - RecvTime + (RecvTime-SendTime)/2;
				++Cnt;
			}
			unsigned int getCnt()const { return Cnt; }
			std::chrono::milliseconds getAverageWaitTime() const {
				return std::chrono::duration_cast<std::chrono::milliseconds>(WaitTimeAccumulation/Cnt);
			}
			std::chrono::milliseconds getAverageDifTime() const {
				return std::chrono::duration_cast<std::chrono::milliseconds>(DifTimeAccumulation/Cnt);
			}
		};
		ping Ping;
		//Pingを打つ回数
		unsigned int RequestPingNum;
	private:
		bool SendNowTime;
		bool SendCorrection;
		bool SendAlarm;
	public:
		//現在時刻の取得を要求する
		void requestMachineTime(unsigned int Num_) {
			if(RequestPingNum)return;
			RequestPingNum =Num_;
			Ping.reset();
		}
		//現在時刻を取得中か？
		bool is_requestMachineTime()const { return RequestPingNum>0; }
		//現在時刻が補正可能か?
		bool can_correctMachineTime()const { return Ping.getCnt()>0 && RequestPingNum==0; }
		//時刻補正を要求
		void correctMachineTime() {
			if(!can_correctMachineTime())return;
			RequestCorrectMachineTime = -Ping.getAverageDifTime();
		}
		//現在時刻が補正可能か?
		bool can_getMachineDifTime()const { return Ping.getCnt()>0 && RequestPingNum==0; }
		//PC時間から見た機体時間のずれを取得
		std::chrono::milliseconds getMachineDifTime()const { return  Ping.getAverageDifTime(); }
		//時報のインターバル
		void setArarmInterval(unsigned int AutoGetIntervalSec_) { AutoGetInterval.setv(AutoGetIntervalSec_); }
	public:
		void setup_talk(void)override {
			if(AutoGetInterval()) {
				SendAlarm=true;
			}

			if(RequestPingNum) {
				SendNowTime=true;
			}

			if(RequestCorrectMachineTime!=milliseconds(0)) {
				SendCorrection=true;
			}

			return;
		}
		bool talk(std::string& Str)override {
			// chrono :: get now time
			if(SendNowTime) {
				Str.push_back(static_cast<unsigned char>(0x00));
				// コマンドを送った時の時間
				Ping.start();
				--RequestPingNum;

				SendNowTime = false;
				return false;
			}
			// chrono :: set auto get interval
			if(SendAlarm) {
				// interval の設定
				hmLib_sint32 t_int = static_cast<hmLib_sint32>(AutoGetInterval.getv());

				//送信
				Str.push_back(static_cast<unsigned char>(0x01));
				Str.push_back(static_cast<unsigned char>(t_int & 0xFF));
				Str.push_back(static_cast<unsigned char>((t_int >> 8) & 0xFF));
				Str.push_back(static_cast<unsigned char>((t_int >> 16) & 0xFF));
				Str.push_back(static_cast<unsigned char>((t_int >> 24) & 0xFF));

				// 1っ回の送信で終わり
				SendAlarm = false;

				return false;
			}

			// chrono :: modify time 
			if(SendCorrection) {
				// 時間を望ましい形に変更する
				auto TimeSec = std::chrono::duration_cast< std::chrono::seconds >(RequestCorrectMachineTime);
				std::chrono::milliseconds TimeMSec = RequestCorrectMachineTime - TimeSec;

				// ms が負の時はこれが負にならないように変更する
				if(TimeMSec.count() < 0) {
					TimeSec -= std::chrono::seconds(1);
					TimeMSec += std::chrono::seconds(1);
				}

				// 送信用に変更
				hmLib_sint32 t_s = static_cast<hmLib_sint32>(TimeSec.count());
				hmLib_uint16 t_ms = static_cast<hmLib_uint16>(TimeMSec.count());

				Str.push_back(static_cast<unsigned char>(0x02));
				Str.push_back(static_cast<unsigned char>(t_s & 0xFF));
				Str.push_back(static_cast<unsigned char>((t_s >> 8) & 0xFF));
				Str.push_back(static_cast<unsigned char>((t_s >> 16) & 0xFF));
				Str.push_back(static_cast<unsigned char>((t_s >> 24) & 0xFF));
				Str.push_back(static_cast<unsigned char>(t_ms & 0xFF));
				Str.push_back(static_cast<unsigned char>((t_ms >> 8) & 0xFF));

				SendCorrection = false;
				RequestCorrectMachineTime = std::chrono::milliseconds::zero();
				return false;
			}

			return true;
		}
		bool listen(datum::time_point Time_, bool Err_, const std::string& Str_)override {
			if(Str_.size() == 0)return true;

			if(static_cast<unsigned char>(Str_.at(0)) == 0x00) {
				if(Str_.size() != 7)return true;

				// 現在時刻取得
				hmLib_sint32 MachineTimeSec = static_cast<hmLib_uint8>(Str_.at(1)) + static_cast<hmLib_uint8>(Str_.at(2)) * 256
					+ static_cast<hmLib_uint8>(Str_.at(3)) * 256 * 256 + static_cast<hmLib_uint8>(Str_.at(4)) * 256 * 256 * 256;
				hmLib_uint16 MachineTimeMSec = static_cast<hmLib_uint16>((hmLib_uint8)Str_.at(5)) + static_cast<hmLib_uint16>((hmLib_uint8)Str_.at(6)) * 256;

				// time_point に変換
				clock::time_point MachineTimePoint = std::chrono::system_clock::from_time_t(MachineTimeSec) + std::chrono::milliseconds(MachineTimeMSec);

				// データを受信した時間を記録
				Ping.stop(MachineTimePoint);

				//時刻計測が終了したら、シグナル発報
				if(can_getMachineDifTime()) {
					MachineDifTime=getMachineDifTime();
					signal_getMachineDifTime(MachineDifTime);
				}

				return false;
			}
			if(static_cast<unsigned char>(Str_.at(0)) == 0x01) {
				if(Str_.size() != 3)return true;

				// 現在時刻取得
				hmLib_uint16 Interval = static_cast<hmLib_uint16>(static_cast<hmLib_uint8>(Str_.at(1))) + static_cast<hmLib_uint16>(static_cast<hmLib_uint8>(Str_.at(1))) * 256;
				AutoGetInterval.setm(Interval);

				return false;
			}
			// chrono :: 時刻修正に対する返事
			else if(static_cast<unsigned char>(Str_.at(0)) == 0x02) {
				if(Str_.size() != 7)return true;

				// 現在時刻取得
				hmLib_sint32 t_s = (hmLib_sint32)((hmLib_uint32)(static_cast<hmLib_uint8>(Str_.at(1)) + static_cast<hmLib_uint8>(Str_.at(2)) * 256
					+ static_cast<hmLib_uint8>(Str_.at(3)) * 256 * 256 + static_cast<hmLib_uint8>(Str_.at(4)) * 256 * 256 * 256));
				hmLib_uint16 t_ms = static_cast<hmLib_uint16>((hmLib_uint8)Str_.at(5)) + static_cast<hmLib_uint16>((hmLib_uint8)Str_.at(6)) * 256;

				// millisec time に変換
				std::chrono::milliseconds time = std::chrono::seconds(t_s) + std::chrono::milliseconds(t_ms);

				//補正された時間を差し引く
				RequestCorrectMachineTime = std::chrono::milliseconds::zero();

				//MachineTimeを更新しておく
				MachineDifTime += time;
				signal_getMachineDifTime(MachineDifTime);

				return false;
			}

			// chrono :: 時報
			else if(static_cast<unsigned char>(Str_.at(0)) == 0x0A) {
				if(Str_.size() != 5)return true;

				// 現在時刻取得
				hmLib_sint32 t_s = (hmLib_sint32)((hmLib_uint32)(static_cast<hmLib_uint8>(Str_.at(1)) + static_cast<hmLib_uint8>(Str_.at(2)) * 256
					+ static_cast<hmLib_uint8>(Str_.at(3)) * 256 * 256 + static_cast<hmLib_uint8>(Str_.at(4)) * 256 * 256 * 256));

				// time_point に変換
				clock::time_point tp = std::chrono::system_clock::from_time_t(t_s);


				//本当は、受け取った時刻をファイルに記録したりしたい。
				//	元々の目的は、他のセンサーデータのうち時刻情報を提供しないデータのための、
				//	補正用のタイムスタンプ。正直、多くの観測データがUnixTimeを持つ以上いらない。

				return false;
			}

			return true;
		}
	private:
		hmLib::signals::unique_connections SignalConnections;
		hmLib::inquiries::unique_connections InquiryConnections;
	public:
		//機体時刻取得要求
		void slot_requestMachineTime(boost::signals2::signal<void(void)>& Signal_) {
			SignalConnections(hmLib::signals::connect(Signal_, [&](void)->void {requestMachineTime(3); }));
		}

		//機体時刻取得時に発報
		boost::signals2::signal<void(std::chrono::milliseconds)> signal_getMachineDifTime;

		//時刻補正が実行可能か？
		void contact_can_correctMachineTime(hmLib::inquiries::inquiry<bool>& Inquiry_) {
			InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&]()->bool {return can_correctMachineTime(); }));
		}

		//時刻補正要求
		void slot_correctMachineTime(boost::signals2::signal<void(void)>& Signal_) {
			SignalConnections(hmLib::signals::connect(Signal_, [&](void)->void {correctMachineTime(); }));
		}
	};
}
#
#endif
