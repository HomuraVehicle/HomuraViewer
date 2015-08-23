#ifndef HMR_AUTOLOGGER_AUTOLOGGERCLIENT_INC
#define HMR_AUTOLOGGER_AUTOLOGGERCLIENT_INC 100
#
#include<hmrChrono.hpp>
namespace hmr {
	namespace autolog {
		class itfAutoLoggerClient {
		private:
			bool IsActive;
			bool IsStart;
			std::string Path;
		protected:
			virtual void autolog_start(const std::string& Path_)=0;
			virtual void autolog_stop()=0;
			virtual void autolog_setup_observe()=0;
			virtual bool autolog_observe(hmr::clock::time_point ObserveTime_)=0;
		public:
			itfAutoLoggerClient() :IsActive(false), IsStart(false) {}
			void activate() {
				if(is_active())return;
				IsActive=true; 
				if(IsStart)autolog_start(Path);
			}
			void inactivate() {
				if(!is_active())return;
				IsActive=false;
				if(IsStart)autolog_stop();
			}
			bool is_active()const {
				return IsActive;
			}
			//ログパスとともに、自動ログを通知
			void start(const std::string& Path_) {
				if(is_start())return;

				IsStart=true;
				Path=Path_;

				if(IsActive)autolog_start(Path_);
			}
			//自動ログ終了を通知
			void stop() {
				if(!is_start())return;
				IsStart=false;
				
				if(IsActive)autolog_stop();
			}
			//スタートしているかどうか
			bool is_start()const {
				return IsStart;
			}
			//observeタイムを事前通知する
			void setup_observe() {
				autolog_setup_observe();
			}
			//observeタイム以降、マイステップごとに実行される
			//	自身の観測が終わるまではtrue、終わったらfalseを返す
			//	この戻り値を見て、Host側はObserveを終了していいかどうかを見ている
			bool observe(hmr::clock::time_point ObserveTime_) {
				if(!IsActive)return false;
				return autolog_observe(ObserveTime_);
			}
		};
	}
}
#
#endif
