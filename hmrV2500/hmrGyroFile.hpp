
#ifndef HMR_GYROFILE_INC
#define HMR_GYROFILE_INC 100
#

/*===hmrGyroFile===
Gyro ファイル関係のクラス
　ログはスロットを通して行うことにする

hmrThermoFile v1_00/130720 amby
	作成開始

*/

#include <string>
#include <fstream>
#include <boost/signals2.hpp>
#include <hmLib_v3_06/signals.hpp>
#include "hmrItfFile.hpp"
#include"hmrData.hpp"
#include "coordinates.hpp"


namespace hmr{
	class cGyroFileAgent:public itfFileAgent{
		typedef hmLib::coordinates3D::angle angle;
	private:
		std::ofstream ofs;
		hmLib::signals::unique_connections SignalConnections;

		// log and write 関数
		void log_writeData(clock::time_point time_, angle data_){
			if(is_active()){
				ofs << hmr::time_to_hms(time_) << FILE_DEVIDE_CHARS << data_.roll
					<< FILE_DEVIDE_CHARS << data_.pitch << FILE_DEVIDE_CHARS << data_.yaw
					<< std::endl;
			}
		}

	public:
		cGyroFileAgent(){}
		void activate(const std::string& Path_){
			if(!is_active()){
				ofs.open(Path_ + "Gyro.txt");
				ofs << FILE_COMMENTOUT_CHARS << " This is the Gyro data log file" << std::endl;
				ofs << FILE_COMMENTOUT_CHARS << " Time" << FILE_DEVIDE_CHARS << "Angle[roll, pitch, yaw]" << std::endl;
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
		void slot_log_writeData(boost::signals2::signal<void(angle, double, clock::time_point)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](angle data_, double d, clock::time_point time_)->void{this->log_writeData(time_, data_);}));	
		}
	
	};
}
#endif









