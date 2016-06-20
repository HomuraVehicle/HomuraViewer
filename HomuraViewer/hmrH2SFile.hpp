
#ifndef HMR_H2SFILE_INC
#define HMR_H2SFILE_INC 100
#

/*===hmrH2SFile===
H2S �t�@�C���֌W�̃N���X
�@���O�̓X���b�g��ʂ��čs�����Ƃɂ���

hmrH2SFile v1_00/130720 amby
	�쐬�J�n

*/

#include <string>
#include <fstream>
#include <boost/signals2.hpp>
#include <hmLib/signals.hpp>
#include <hmrVLib/File.hpp>
#include <hmrVLib/Data.hpp>


namespace hmr{
	class cH2SFileAgent:public itfFileAgent{
	private:
		std::ofstream ofs;
		hmLib::signals::unique_connections SignalConnections;

		// log and write �֐�
		void log_writeData(clock::time_point time_, double data_){
			if(is_active()){
				ofs << hmr::time_to_hms(time_) << FILE_DEVIDE_CHARS << data_ << std::endl;
			}
		}

	public:
		cH2SFileAgent(){}
		void activate(const std::string& Path_){
			if(!is_active()){
				ofs.open(Path_ + "H2S.txt");
				ofs << FILE_COMMENTOUT_CHARS << " This is the H2S data log file" << std::endl;
				ofs << FILE_COMMENTOUT_CHARS << " Time" << FILE_DEVIDE_CHARS << "Data," << std::endl;
			}
		}
		bool is_active() const {return ofs.is_open();}
		void inactivate(){
			if(is_active()){
				ofs << FILE_COMMENTOUT_CHARS << "EOF" << std::endl;
				ofs.close();
			}
		}
	
	// Log �p�� signal slot
	public:
		void slot_log_writeData(boost::signals2::signal<void(double, clock::time_point)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](double data_, clock::time_point time_)->void{this->log_writeData(time_, data_);}));	
		}
	
	};
}
#endif









