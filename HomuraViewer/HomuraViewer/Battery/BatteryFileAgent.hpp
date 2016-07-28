#ifndef HMR_BATTERY_FILEAGENT_INC
#define HMR_BATTERY_FILEAGENT_INC 100
#

/*===hmrBatteryFile===
Battery ファイル関係のクラス
　ログはスロットを通して行うことにする

hmrBatteryFile v1_01/130907 amby
　　これまで、バッテリ数のシグナルとスロットをつなぐところで、スロットとして同じ関数を引数の値だけバッテリによって変化させることで対応していた。　
  　　このような形：SignalConnections(hmLib::signals::connect(Signal_,[&](clock::time_point time_, double data_)->void{this->logData(time_, data_, num_);}));	

	しかし、結局関数はポインタでは一つしか用意されないので、ポインタではこれらの引数の違いを区別することができないのかな？？
	よって、無理やりテンプレート関数に変更

hmrBatteryFile v1_00/130720 amby
	作成開始
*/

#include <array>
#include <string>
#include <fstream>
#include <boost/signals2.hpp>
#include <hmLib/signals.hpp>
#include <HomuraViewer/File.hpp>
#include <HomuraViewer/chrono.hpp>
#include "BatteryData.hpp"

namespace hmr{
	namespace viewer{
		namespace battery{
			template<unsigned int BatteryNum_>
			class cFileAgent :public itfFileAgent{
				using this_data_t = data<BatteryNum_>;
			private:
				std::ofstream ofs;
				hmLib::signals::unique_connections SignalConnections;
			public:
				void activate(const std::string& Path_){
					if(!ofs.is_open()){
						ofs.open(Path_ + "Battery.txt",std::ios_base::out|std::ios_base::app);
					}
				}
				bool is_active() const{ return ofs.is_open(); }
				void inactivate(){
					if(is_active()){
						ofs.close();
					}
				}
				void log(this_data_t& Data){
					date Date = date::from_time(Data.time());
					ofs << Date.Year << "\t" << Date.Month << "\t" << Date.Day << "\t" << Date.Hour << "\t" << Date.Min << "\t" << Date.Sec;
					for(auto& val : Data){
						ofs << "\t" << val;
					}
					ofs << std::endl;
				}
				// Log 用の signal slot
			public:
				// Logとして書き込む時のslot
				void slot_logData(boost::signals2::signal<void(this_data_t)>& Signal_){
					SignalConnections(hmLib::signals::connect(Signal_, [&](this_data_t Data_)->void{this->log(Data_); }));
				}
			};
		}
	}
}
#
#endif
