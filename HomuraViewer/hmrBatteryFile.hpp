
#ifndef HMR_BATTERYFILE_INC
#define HMR_BATTERYFILE_INC 100
#

/*===hmrBatteryFile===
Battery �t�@�C���֌W�̃N���X
�@���O�̓X���b�g��ʂ��čs�����Ƃɂ���

hmrBatteryFile v1_01/130907 amby
�@�@����܂ŁA�o�b�e�����̃V�O�i���ƃX���b�g���Ȃ��Ƃ���ŁA�X���b�g�Ƃ��ē����֐��������̒l�����o�b�e���ɂ���ĕω������邱�ƂőΉ����Ă����B�@
  �@�@���̂悤�Ȍ`�FSignalConnections(hmLib::signals::connect(Signal_,[&](clock::time_point time_, double data_)->void{this->logData(time_, data_, num_);}));	

	�������A���Ǌ֐��̓|�C���^�ł͈�����p�ӂ���Ȃ��̂ŁA�|�C���^�ł͂����̈����̈Ⴂ����ʂ��邱�Ƃ��ł��Ȃ��̂��ȁH�H
	����āA�������e���v���[�g�֐��ɕύX

hmrBatteryFile v1_00/130720 amby
	�쐬�J�n
*/

#include <array>
#include <string>
#include <fstream>
#include <boost/signals2.hpp>
#include <hmLib/signals.hpp>
#include <hmrVLib/File.hpp>
#include <hmrVLib/Data.hpp>


namespace hmr{
	template<unsigned int BatteryNum_>
	class cBatteryFileAgent:public itfFileAgent{
	private:
		// Data�������Ă���� true, �łȂ���� false
		typedef std::pair<bool, double> battData;
		// �o�b�e�����ꂼ��̗v�f
		typedef std::array<battData, BatteryNum_> battDataSet;
		
		battDataSet bBuf;
		clock::time_point time;
		
		std::ofstream ofs;
		hmLib::signals::unique_connections SignalConnections;


		// log �֐�
		template<unsigned int BattNo>
		void logData(clock::time_point time_, double data_){
			// �J���Ă��Ȃ���΁E�E�E�E
			if(!is_active()) return;

			// Data���l�܂��Ă����
			if(bBuf.at(BattNo).first){
				writeFile();
			}
			
			// Log���n�߂�
			time = time_;
			bBuf.at(BattNo).second = data_;
			bBuf.at(BattNo).first = true;

			return;	
		}

		// write file�֐�
		void writeFile(void){
			// �J���ĂȂ���΁E�E�E�E
			if(!is_active()) return;

			// File �ɏ�������
			ofs << hmr::time_to_hms(time) ;
			for(int i = 0; i< BatteryNum_; i++){
				// write
				if( bBuf.at(i).first ){
					ofs << FILE_DEVIDE_CHARS << bBuf.at(i).second;
				}else{
					ofs << FILE_DEVIDE_CHARS;
				}

				//Flag clear
				bBuf.at(i).first = false;
			}
			ofs << std::endl;

			return;
		}


	public:
		cBatteryFileAgent(){
			for(int i=0;i<BatteryNum_;i++){
				bBuf.at(i).first = false;
				bBuf.at(i).second = 0.;
			}
		}
		void activate(const std::string& Path_){
			if(!is_active()){
				ofs.open(Path_ + "Battery.txt");
				ofs << FILE_COMMENTOUT_CHARS << " This is the Battery data log file" << std::endl;
				ofs << FILE_COMMENTOUT_CHARS << " Time" << FILE_DEVIDE_CHARS << "Data.... [" << BatteryNum_ << "]" << std::endl;
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
		// Log�Ƃ��ď������ގ���slot
		template <unsigned int BattNo>
		void slot_logData(boost::signals2::signal<void(clock::time_point, double)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](clock::time_point time_, double data_)->void{this->logData<BattNo>(time_, data_);}));	
		}

		// �t�@�C����ۑ����邽�߂�slot
		void slot_writeData(boost::signals2::signal<void(void)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](void)->void{this->writeFile();}));	
		}


		/*
		template<typename iterator>
		unsigned int get_rangesize(iterator begin,iterator end){return end-begin;}
		template<unsigned int No>
		void output_no(void){
			unsigned int Values[No];

			std::cout<<No<<std::endl;
		}
		output_no<45>();

		get_rangesize<std::vector<int>::iterator>(Begin,End);
		*/
	
	};
}
#endif









