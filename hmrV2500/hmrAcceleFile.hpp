
#ifndef HMR_ACCELEFILE_INC
#define HMR_ACCELEFILE_INC 100
#

/*===hmrAcceleFile===
 Accele �t�@�C���֌W�̃N���X
�@���O�̓X���b�g��ʂ��čs�����Ƃɂ���

hmrAcceleFile v1_00/130720 amby
	�쐬�J�n

*/

#include <string>
#include <fstream>
#include <boost/signals2.hpp>
#include <hmLib_v3_06/signals.hpp>
#include "hmrItfFile.hpp"
#include"hmrData.hpp"
#include "coordinates.hpp"


namespace hmr{
	class cAcceleFileAgent:public itfFileAgent{
	private:
		typedef hmLib::coordinates3D::position position;
		typedef hmLib::coordinates3D::polar polar;

		std::ofstream ofs;
		hmLib::signals::unique_connections SignalConnections;

	public:
		bool is_active()const{return ofs.is_open();}

	private:
		// log �֐�
		void log_writeData(clock::time_point time_, position pos_, polar por_){
			// �J���Ă��Ȃ���΁E�E�E�E
			if(!is_active()) return;

			// File �ɏo��
			ofs << hmr::time_to_hms(time_) ;
			ofs <<  FILE_DEVIDE_CHARS << pos_.x <<  FILE_DEVIDE_CHARS << pos_.y <<  FILE_DEVIDE_CHARS << pos_.z;
			ofs << 	 FILE_DEVIDE_CHARS << por_.theta <<  FILE_DEVIDE_CHARS << por_.phi;
			ofs << std::endl;	

			return;	
		}


	public:
		cAcceleFileAgent(){}
		void activate(const std::string& Path_){
			if(!is_active()){
				ofs.open(Path_ + "Accele.txt");
				ofs << FILE_COMMENTOUT_CHARS << " This is the Accele data log file" << std::endl;
				ofs << FILE_COMMENTOUT_CHARS << " Time" << FILE_DEVIDE_CHARS << "Position[x, y, z]" << FILE_DEVIDE_CHARS << "Polar[theta, phi]" << std::endl;
			}
		}
		void inactivate(){
			if(is_active()){
				ofs << FILE_COMMENTOUT_CHARS << "EOF" << std::endl;
				ofs.close();
			}
		}
	
	// Log �p�� signal slot
	public:
		// Log�Ƃ��ď������ގ���slot
		void slot_log_writeData(boost::signals2::signal<void(clock::time_point, position, polar)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](clock::time_point time_, position pos_, polar por_)->void{this->log_writeData(time_, pos_, por_);}));	
		}


	
	};
}
#endif









