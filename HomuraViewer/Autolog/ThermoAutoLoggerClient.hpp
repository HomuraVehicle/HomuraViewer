#ifndef HMR_AUTOLOG_THERMOAUTOLOGGERCLIENT_INC
#define HMR_AUTOLOG_THERMOAUTOLOGGERCLIENT_INC 100
#
#include<fstream>
#include"AutoLoggerClient.hpp"
namespace hmr {
	namespace autolog {
		class cThermoAutoLoggerClient :public itfAutoLoggerClient{
		private:
			unsigned int DataCnt;
			unsigned int DataNum;
			bool IsObserve;
			std::ofstream fout;
			hmr::clock::time_point NextCommand;
		private:
			void newData(double Data_, clock::time_point Time_) {
				if(IsObserve && DataCnt<DataNum) {
					fout<<std::chrono::duration_cast<std::chrono::seconds>(Time_.time_since_epoch()).count()<<","<<hmr::time_to_ymd(Time_)+" "+hmr::time_to_hms(Time_)<<","<<Data_<<std::endl;
					++DataCnt;
				}
			}
		public:
			boost::signals2::signal<void(bool)> signal_setDataMode;
			hmLib::inquiries::inquiry<bool> inquiry_isDataMode;

			void slot_newData(boost::signals2::signal<void(double, clock::time_point)>& Signal_) {
				hmLib::signals::connect(Signal_, [&](double Data_, clock::time_point Time_)->void { newData(Data_, Time_); });
			}
		public:
			cThermoAutoLoggerClient(unsigned int DataSamplingNum_)
				: DataCnt(0)
				, DataNum(DataSamplingNum_)
				, IsObserve(false) {
			}
		protected:
			//自動ログスタート
			void autolog_start(const std::string& Path_)override {
				fout.open(Path_+"autolog_data_thermo.csv",std::ios_base::out|std::ios_base::app);
			}
			//自動ログストップ
			void autolog_stop() override{
				fout.close();
			}
			//observeタイムを事前通知する
			void autolog_setup_observe()override {
				DataCnt=0;
				IsObserve=false;
			}
			//observeタイム以降、マイステップごとに実行される
			//	自身の観測が終わるまではtrue、終わったらfalseを返す
			//	この戻り値を見て、Host側はObserveを終了していいかどうかを見ている
			bool autolog_observe(hmr::clock::time_point ObserveTime_)override {
				//データ観測がまだできない場合
				if(!IsObserve && !inquiry_isDataMode()) {
					if(NextCommand<hmr::clock::now()) {
						signal_setDataMode(true);
						NextCommand=hmr::clock::now()+std::chrono::seconds(5);
					}
					return true;
				}

				//データ観測時間になっていない場合
				if(hmr::clock::now()<ObserveTime_)return true;
				IsObserve=true;

				//データ観測中だが、必要データ数取得していない場合
				if(DataCnt<DataNum)return true;

				//データ観測モードが切れていない
				if(inquiry_isDataMode()) {
					if(NextCommand<hmr::clock::now()) {
						signal_setDataMode(false);
						NextCommand=hmr::clock::now()+std::chrono::seconds(5);
					}
					return true;
				}

				return false;
			}
		};
	}
}
#
#endif
