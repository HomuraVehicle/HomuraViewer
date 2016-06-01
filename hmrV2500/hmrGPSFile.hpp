
#ifndef HMR_GPSFILE_INC
#define HMR_GPSFILE_INC 101
#

/*===hmrGPSFile===
GPS ファイル関係のクラス
　ログはスロットを通して行うことにする


 hmrGPSFile v1_01/130907 amby
	GPGGA形式に変化させた

 hmrGPSFile v1_00/130720 amby
	作成開始

*/

#include <string>
#include <fstream>
#include <boost/signals2.hpp>
#include <hmLib_v3_06/signals.hpp>
#include "hmrItfFile.hpp"
#include "hmrData.hpp"
#include "hmrGPSDataSet.hpp"

namespace hmr{
	class cGPSFileAgent:public itfFileAgent{
	private:
		std::ofstream ofs;
		hmLib::signals::unique_connections SignalConnections;

		// log and write 関数
		void log_writeData(clock::time_point time_, const GPSDataSet& data_){
			if(is_active()){
				ofs << data_.strGPGGA() << std::endl;
				/*
				ofs << hmr::time_to_hms(time_) << FILE_DEVIDE_CHARS << data_.getUseGPS()
					<< FILE_DEVIDE_CHARS << data_.getPos().x << FILE_DEVIDE_CHARS << data_.getPos().y
					<< FILE_DEVIDE_CHARS << data_.getHeight() << FILE_DEVIDE_CHARS << hmr::time_to_hms(data_.getUTC())
					<< FILE_DEVIDE_CHARS << data_.getFail() << FILE_DEVIDE_CHARS << data_.getErr()
					<< std::endl;
				*/
			}
		}

	public:
		cGPSFileAgent(){}
		void activate(const std::string& Path_){
			if(!is_active()){
				ofs.open(Path_ + "GPS.txt");
				/*
				ofs << FILE_COMMENTOUT_CHARS << " This is the GPS data log file " << std::endl;
				ofs << FILE_COMMENTOUT_CHARS << " 　単に緯度などのデータを出していくだけのファイル " << std::endl;
				ofs << FILE_COMMENTOUT_CHARS << " Time" << FILE_DEVIDE_CHARS << "UseGPS" 
					<< FILE_DEVIDE_CHARS << "Position-longitude" << FILE_DEVIDE_CHARS << "Position-latitude"
					<< FILE_DEVIDE_CHARS << "Height" << FILE_DEVIDE_CHARS << "UTC" 
					<< FILE_DEVIDE_CHARS << "failFlag" << FILE_DEVIDE_CHARS << "errFlag" << std::endl;
				*/
			}
		}
		bool is_active() const {return ofs.is_open();}
		void inactivate(){
			if(is_active()){
				//ofs << FILE_COMMENTOUT_CHARS << "EOF" << std::endl;
				ofs.close();
			}
		}
	
	// Log 用の signal slot
	public:
		void slot_log_writeData(boost::signals2::signal<void(const GPSDataSet&, clock::time_point)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](const GPSDataSet& data_, clock::time_point time_)->void{this->log_writeData(time_, data_);}));	
		}
	
	};
}
#endif









