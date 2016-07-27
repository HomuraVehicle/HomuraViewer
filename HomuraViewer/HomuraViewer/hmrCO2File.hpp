
#ifndef HMR_CO2FILE_INC
#define HMR_CO2FILE_INC 100
#

/*===hmrCO2File===
CO2 ファイル関係のクラス
　ログはスロットを通して行うことにする

hmrCO2File v1_00/130720 amby
	作成開始

*/

#include <string>
#include <fstream>
#include <boost/signals2.hpp>
#include <hmLib/signals.hpp>
#include <HomuraViewer/File.hpp>
#include <HomuraViewer/Data.hpp>


namespace hmr{
	class cCO2FileAgent:public itfFileAgent{
	private:
		std::ofstream ofs;
		hmLib::signals::unique_connections SignalConnections;

		// log and write 関数
		void log_writeData(clock::time_point time_, double data_){
			if(is_active()){
				ofs << hmr::time_to_hms(time_) << FILE_DEVIDE_CHARS << data_ << std::endl;
			}
		}

	public:
		cCO2FileAgent(){}
		void activate(const std::string& Path_){
			if(!is_active()){
				ofs.open(Path_ + "CO2.txt");
				ofs << FILE_COMMENTOUT_CHARS << " This is the CO2 data log file" << std::endl;
				ofs << FILE_COMMENTOUT_CHARS << " Time" << FILE_DEVIDE_CHARS << "Data" << std::endl;
			}
		}
		bool is_active() const {return ofs.is_open();}
		void inactivate(){
			if(is_active()){
				ofs << FILE_COMMENTOUT_CHARS << "EOF" << std::endl;
				ofs.close();
			}
		}
	
	// Log 用の signal slot
	public:
		void slot_log_writeData(boost::signals2::signal<void(double, clock::time_point)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&]( double data_, clock::time_point time_)->void{this->log_writeData(time_, data_);}));	
		}
	
	};
}
#endif









