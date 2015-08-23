
#ifndef HMR_UNISENSORFILE_INC
#define HMR_UNISENSORFILE_INC 100
#

/*===hmrThermoFile===
Thermo ファイル関係のクラス
　ログはスロットを通して行うことにする

hmrUniSensorFile v1_00/140320 amby
	作成開始

	signal(double, timePoint)型に対応

*/

#include <string>
#include <fstream>
#include <boost/signals2.hpp>
#include <hmLib_v3_05/signals.hpp>
#include "hmrItfFile.hpp"
#include"hmrData.hpp"


namespace hmr{
	class cUniSensorFileAgent:public itfFileAgent{
	private:
		std::ofstream ofs;
		hmLib::signals::unique_connections SignalConnections;
		std::string name;

		// log and write 関数
		void log_writeData(clock::time_point time_, double data_){
			if(is_active()){
				ofs << hmr::time_to_hms(time_) << FILE_DEVIDE_CHARS << data_ << std::endl;
			}
		}

	public:
		cUniSensorFileAgent(const std::string& Name_){
			name = Name_;
		}

		void activate(const std::string& Path_){
			if(!is_active()){
				ofs.open(Path_ + name + ".txt");
				ofs << FILE_COMMENTOUT_CHARS << " This is the data log file   name:" << name <<  std::endl;
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
			SignalConnections(hmLib::signals::connect(Signal_,[&](double data_, clock::time_point time_)->void{this->log_writeData(time_, data_);}));	
		}
	
	};
}
#endif









