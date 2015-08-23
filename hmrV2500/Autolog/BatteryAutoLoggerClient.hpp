#ifndef HMR_AUTOLOG_BATTERYAUTOLOGGERCLIENT_INC
#define HMR_AUTOLOG_BATTERYAUTOLOGGERCLIENT_INC 100
#
#include<fstream>
#include"AutoLoggerClient.hpp"
namespace hmr {
	namespace autolog {
		template<unsigned int BatteryNum_>
		class cBatteryAutoLoggerClient :public itfAutoLoggerClient{
			typedef cBatteryAutoLoggerClient<BatteryNum_> my_type;
		private:
			unsigned int DataCnt[BatteryNum_];
			unsigned int DataNum;
			bool IsObserve;
			std::ofstream fout;
			hmr::clock::time_point NextCommand;
		private:
			void newData(clock::time_point Time_,unsigned int No_,double Data_) {
				if(No_>=BatteryNum_)return;

				if(IsObserve && DataCnt[No_]<DataNum) {
					fout<<std::chrono::duration_cast<std::chrono::seconds>(Time_.time_since_epoch()).count()<<","<<hmr::time_to_ymd(Time_)+" "+hmr::time_to_hms(Time_)<<","<<No_<<","<<Data_<<std::endl;
					++DataCnt[No_];
				}
			}
		public:
			boost::signals2::signal<void(bool)> signal_setDataMode;
			hmLib::inquiries::inquiry<bool> inquiry_isDataMode;

			void slot_newData(boost::signals2::signal<void(clock::time_point,double)>& Signal_,unsigned int BatteryNo_) {
				class func{
					my_type& Ref;
					unsigned int No;
				public:
					func(my_type& Ref_, unsigned int No_) :Ref(Ref_), No(No_) {}
					void operator()(clock::time_point Time_,double Data_){
						Ref.newData(Time_,No,Data_); 
					}
				};

				if(BatteryNo_<BatteryNum_) {
					hmLib::signals::connect(Signal_, func(*this,BatteryNo_));
				}
			}
		public:
			cBatteryAutoLoggerClient(unsigned int DataSamplingNum_)
				: DataNum(DataSamplingNum_)
				, IsObserve(false) {
				for(unsigned int& i:DataCnt)i=0;
			}
		protected:
			//自動ログスタート
			void autolog_start(const std::string& Path_)override {
				fout.open(Path_+"autolog_data_battery.csv",std::ios_base::out|std::ios_base::app);
			}
			//自動ログストップ
			void autolog_stop() override{
				fout.close();
			}
			//observeタイムを事前通知する
			void autolog_setup_observe()override {
				for(int No_=0; No_<BatteryNum_; ++No_)DataCnt[No_]=0;
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
				for(int No_=0; No_<BatteryNum_; ++No_) {
					if(DataCnt[No_]<DataNum)return true;
				}

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
