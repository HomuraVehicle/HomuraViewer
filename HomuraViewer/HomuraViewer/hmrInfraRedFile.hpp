
#ifndef HMR_INFRAREDFILE_INC
#define HMR_INFRAREDFILE_INC 100
#

/*===hmrinfraredFile===
H2S ファイル関係のクラス
　ログはスロットを通して行うことにする

hmrInfraRedFile v1_00/130720 amby
	作成開始

*/

#include <string>
#include <fstream>
#include <boost/signals2.hpp>
#include <hmLib/signals.hpp>
#include <HomuraViewer/File.hpp>
#include <HomuraViewer/Data.hpp>


namespace hmr{
	class cInfraRedFileAgent:public itfFileAgent{
	private:
		std::ofstream ofs;
		hmLib::signals::unique_connections SignalConnections;

		// log and write 関数
		void log_writeData(clock::time_point time_, double rTemp_, double oTemp_, double iTemp_){
			if(is_active()){
				ofs << hmr::time_to_hms(time_) << FILE_DEVIDE_CHARS << rTemp_
					<< FILE_DEVIDE_CHARS << oTemp_ << FILE_DEVIDE_CHARS << iTemp_
					<< std::endl;
			}
		}

	public:
		cInfraRedFileAgent(){}
		void activate(const std::string& Path_){
			if(!is_active()){
				ofs.open(Path_ + "InfThermo.txt");
				ofs << FILE_COMMENTOUT_CHARS << " This is the InfraRed data log file" << std::endl;
				ofs << FILE_COMMENTOUT_CHARS << " Time" << FILE_DEVIDE_CHARS << "resultTemp" 
					<< FILE_DEVIDE_CHARS << "outTemp"  << FILE_DEVIDE_CHARS << "inTemp" << std::endl;
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
		void slot_log_writeData(boost::signals2::signal<void(double, double, double, clock::time_point)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](double data_, double oTemp, double iTemp, clock::time_point time_)->void{this->log_writeData(time_, data_, oTemp, iTemp);}));	
		}
	
	};
}
#endif









