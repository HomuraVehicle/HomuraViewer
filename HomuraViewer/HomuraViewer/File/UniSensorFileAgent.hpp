
#ifndef HMRVLIB_UNISENSORFILEAGENT_INC
#define HMRVLIB_UNISENSORFILEAGENT_INC 100
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
#include <hmLib/signals.hpp>
#include <hmrVLib/Data.hpp>
#include "ItfFile.hpp"

namespace hmr{
	template<typename T>
	class cUniSensorFileAgent:public itfFileAgent{
	private:
		std::ofstream ofs;
		hmLib::signals::unique_connections SignalConnections;
		std::string name;

		// log and write 関数
		void log_writeData(clock::time_point time_, T data_){
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
		void slot_log_writeData(boost::signals2::signal<void(T, clock::time_point)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](T data_, clock::time_point time_)->void{this->log_writeData(time_, data_);}));	
		}
	};
	template<typename T1,typename T2>
	class cUniSensorFileAgent< std::pair<T1,T2> > :public itfFileAgent{
	private:
		std::ofstream ofs;
		hmLib::signals::unique_connections SignalConnections;
		std::string name;

		// log and write 関数
		void log_writeData(clock::time_point time_, T1 data1_, T2 data2_){
			if(is_active()){
				ofs << hmr::time_to_hms(time_) << FILE_DEVIDE_CHARS << data1_ << FILE_DEVIDE_CHARS << data2_ << std::endl;
			}
		}

	public:
		cUniSensorFileAgent(const std::string& Name_){
			name = Name_;
		}

		void activate(const std::string& Path_){
			if(!is_active()){
				ofs.open(Path_ + name + ".txt");
				ofs << FILE_COMMENTOUT_CHARS << " This is the data log file   name:" << name << std::endl;
				ofs << FILE_COMMENTOUT_CHARS << " Time" << FILE_DEVIDE_CHARS << "Data1" << FILE_DEVIDE_CHARS << "Data2" << std::endl;
			}
		}
		bool is_active() const{ return ofs.is_open(); }
		void inactivate(){
			if(is_active()){
				ofs << FILE_COMMENTOUT_CHARS << "EOF" << std::endl;
				ofs.close();
			}
		}

		// Log 用の signal slot
	public:
		void slot_log_writeData(boost::signals2::signal<void(T1,T2, clock::time_point)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_, [&](T1 data1_, T2 data2_, clock::time_point time_)->void{this->log_writeData(time_, data1_,data2_); }));
		}
	};
}
#endif









