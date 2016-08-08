#ifndef HMR_VIEWER_GPS_GPSFILEAGENT_INC
#define HMR_VIEWER_GPS_GPSFILEAGENT_INC 101
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
#include <hmLib/signals.hpp>
#include <HomuraViewer/File.hpp>
#include <HomuraViewer/chrono.hpp>
#include "GPSData.hpp"

namespace hmr{
	namespace viewer{
		namespace gps{
			class cFileAgent :public itfFileAgent{
			private:
				std::ofstream ofs;
				hmLib::signals::unique_connections SignalConnections;

				// log and write 関数
				void log_writeData(clock::time_point time_, const data& data_){
					if(is_active()){
						ofs << time_to_ymd(time_) << "\t" << time_to_hms(time_);
						ofs << "\t" << time_to_hms(data_.getUTC());
						ofs << "\t" << data_.getPos().y << "\t" << data_.getPos().x;
						ofs << "\t" << data_.getHeight();
						ofs << "\t" << data_.getUseGPS();
						ofs << "\t" << data_.getFail() << "\t" << data_.getErr();
						ofs << std::endl;
						/*
						ofs << time_to_hms(time_) << FILE_DEVIDE_CHARS << data_.getUseGPS()
						<< FILE_DEVIDE_CHARS << data_.getPos().x << FILE_DEVIDE_CHARS << data_.getPos().y
						<< FILE_DEVIDE_CHARS << data_.getHeight() << FILE_DEVIDE_CHARS << time_to_hms(data_.getUTC())
						<< FILE_DEVIDE_CHARS << data_.getFail() << FILE_DEVIDE_CHARS << data_.getErr()
						<< std::endl;
						*/
					}
				}

			public:
				cFileAgent(){}
				void activate(const std::string& Path_){
					if(!is_active()){
						std::ifstream fin(Path_ + "GPS.txt");

						if(!fin.is_open()){
							fin.close();
							ofs.open(Path_ + "GPS.txt");
							ofs << "Date\tTime\tUTC\tLatitude\tLongitude\tHeight\tCh\tIsFail\tIsError" << std::endl;
						} else{
							ofs.open(Path_ + "GPS.txt", std::ios_base::out | std::ios_base::app);
						}
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
				bool is_active() const{ return ofs.is_open(); }
				void inactivate(){
					if(is_active()){
						//ofs << FILE_COMMENTOUT_CHARS << "EOF" << std::endl;
						ofs.close();
					}
				}
				// Log 用の signal slot
			public:
				void slot_log_writeData(boost::signals2::signal<void(const data&, clock::time_point)>& Signal_){
					SignalConnections(hmLib::signals::connect(Signal_, [&](const data& data_, clock::time_point time_)->void{this->log_writeData(time_, data_); }));
				}
			};
			class cGPGGAFileAgent :public itfFileAgent{
			private:
				std::ofstream ofs;
				hmLib::signals::unique_connections SignalConnections;

				// log and write 関数
				void log_writeData(clock::time_point time_, const data& data_){
					if(is_active()){
						ofs << data_.strGPGGA() << std::endl;
						/*
						ofs << time_to_hms(time_) << FILE_DEVIDE_CHARS << data_.getUseGPS()
						<< FILE_DEVIDE_CHARS << data_.getPos().x << FILE_DEVIDE_CHARS << data_.getPos().y
						<< FILE_DEVIDE_CHARS << data_.getHeight() << FILE_DEVIDE_CHARS << time_to_hms(data_.getUTC())
						<< FILE_DEVIDE_CHARS << data_.getFail() << FILE_DEVIDE_CHARS << data_.getErr()
						<< std::endl;
						*/
					}
				}

			public:
				cGPGGAFileAgent(){}
				void activate(const std::string& Path_){
					if(!is_active()){
						ofs.open(Path_ + "GPGGA.txt", std::ios_base::out | std::ios_base::app);
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
				bool is_active() const{ return ofs.is_open(); }
				void inactivate(){
					if(is_active()){
						//ofs << FILE_COMMENTOUT_CHARS << "EOF" << std::endl;
						ofs.close();
					}
				}

				// Log 用の signal slot
			public:
				void slot_log_writeData(boost::signals2::signal<void(const data&, clock::time_point)>& Signal_){
					SignalConnections(hmLib::signals::connect(Signal_, [&](const data& data_, clock::time_point time_)->void{this->log_writeData(time_, data_); }));
				}

			};
		}
	}
}
#endif









