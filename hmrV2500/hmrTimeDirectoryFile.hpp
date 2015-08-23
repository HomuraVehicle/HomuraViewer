#ifndef HMR_WHOLEFILE_INC
#define HMR_WHOLEFILE_INC 100
#

/*===hmrWholeFile===
�قނ�̖���̃f�[�^�t�@�C���֌W�̃N���X
�����ŁA���ꂼ��̃t�@�C�����܂Ƃ߂ăf�B���N�g���ɂԂ�����

�����̓f�B���N�g���Ƃ����Ӗ��ł܂��A�t�@�C���V�X�e�����p�����Ă���

hmrWholeFile v1_00/130914 amby
	�쐬�J�n
*/

#include <string>
#include <fstream>
#include <boost/signals2.hpp>
#include <boost/filesystem.hpp>
#include <hmLib_v3_06/signals.hpp>
#include "hmrItfFile.hpp"
#include "hmrData.hpp"


namespace hmr{
	class cTimeDirectoryFile:public itfFile{
		//typedef std::pair<itfFileAgent*, std::string> FileAgentSet;
		typedef std::vector<itfFileAgent*> FileAgentBuf;

	private:
		std::ofstream ofs;
		hmLib::signals::unique_connections SignalConnections;

		FileAgentBuf fAgents;
		bool activateFlag;

	public:
		// signal �n��̒�`
		boost::signals2::signal<void(bool)> signal_isActivate;

		cWholeFileAgent(){
			activateFlag = false;
		}
		void regist(itfFileAgent* file_){
			fAgents.push_back(file_);
			return;
		}
		void activate(const std::string& Path_){
			// ���Ԃ��擾
			clock::time_point time_ =  clock::now(); 
			hmr::date Date;
			Date.from_time(time_);

			// Pass���w�肵�Ă��܂�
			std::string str1 = Path_ +  (boost::format("%02d%02d%02d_%02d%02d%02d")%(Date.Year%100)%Date.Month%Date.Day%Date.Hour%Date.Min%Date.Sec).str();

			boost::filesystem::create_directory(boost::filesystem::current_path()/str1);
			if(!is_active()){
				for(auto itr = fAgents.begin(); itr != fAgents.end(); ++itr){
					std::string str = str1  + "\\";
					(*itr)->activate( str );
				}

				activateFlag = true;
				signal_isActivate(true);
			}
		}	
		bool is_active() const {return activateFlag;}
		void inactivate(){
			if(is_active()){
				for(auto itr = fAgents.begin(); itr != fAgents.end(); ++itr){
					(*itr)->inactivate();
				}
				activateFlag = false;
				signal_isActivate(false);
			}
		}
	// activate�p, inactivate�p �� signal slot
	public:
		void slot_activate(boost::signals2::signal<void(const std::string&)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](const std::string& path_)->void{this->activate(path_);}));
		}
		void slot_inactivate(boost::signals2::signal<void()>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&]()->void{this->inactivate();}));
		}
	};
}
#endif









