
#ifndef HMR_COMPASSFILE_INC
#define HMR_COMPASSFILE_INC 100
#

/*===hmrCompassFile===
Compass ファイル関係のクラス
　ログはスロットを通して行うことにする

hmrCompassFile v1_00/130720 amby
	作成開始

*/

#include <string>
#include <fstream>
#include <boost/signals2.hpp>
#include <hmLib/signals.hpp>
#include <HomuraViewer/File.hpp>
#include <HomuraViewer/Data.hpp>
#include <hmLib/coordinates.hpp>


namespace hmr{
	class cCompassFileAgent:public itfFileAgent{
		typedef hmLib::coordinates3D::position position;
		typedef hmLib::coordinates3D::polar polar;
	private:
		std::ofstream ofs;
		hmLib::signals::unique_connections SignalConnections;

		// log and write 関数
		void log_writeData(clock::time_point time_, position pos_){
			if(is_active()){
				ofs << hmr::time_to_hms(time_) << FILE_DEVIDE_CHARS << pos_.x <<
					 FILE_DEVIDE_CHARS << pos_.y << FILE_DEVIDE_CHARS << pos_.z <<std::endl;
			}
		}

	public:
		cCompassFileAgent(){}
		void activate(const std::string& Path_){
			if(!is_active()){
				ofs.open(Path_ + "Compass.txt");
				ofs << FILE_COMMENTOUT_CHARS << " This is the Compass data log file" << std::endl;
				ofs << FILE_COMMENTOUT_CHARS << " Time" << FILE_DEVIDE_CHARS << "CpmpassPos[x,y,z]" << std::endl;
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
		void slot_log_writeData(boost::signals2::signal<void(position, clock::time_point)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](position pos_, clock::time_point time_)->void{this->log_writeData(time_, pos_);}));	
		}
	
	};
}
#endif









