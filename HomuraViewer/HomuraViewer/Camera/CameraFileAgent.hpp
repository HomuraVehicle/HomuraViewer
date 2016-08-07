
#ifndef HMR_VIEWER_CAMERA_CAMERAFILEAGENT_INC
#define HMR_VIEWER_CAMERA_CAMERAFILEAGENT_INC 101
#

/*===hmrSpriteFile===
 Sprite �t�@�C���֌W�̃N���X
	�@
	 �P�A�ejpeg�t�@�C�������N���X
	 �Q�A�S�̂𓝊�����N���X
	 �@�@�P�̃N���X���q�N���X�Ƃ��Ď����Ă���𐧌䂷��

	�̓�i�\���ɂ���
	���ꂼ��́@itfFileAgent �̌`���ɏ]��


===hmrSpriteFile===
v1_01/140320 hmIto
	���O��t�����悤�ɏC��
v1_00/130907 amby
	�쐬�J�n

*/

#include <string>
#include <fstream>
#include <boost/signals2.hpp>
#include <boost/filesystem.hpp>
#include <hmLib/signals.hpp>
#include <HomuraViewer/File.hpp>
#include <HomuraViewer/chrono.hpp>
#include <hmLib/coordinates.hpp>

namespace hmr{
	namespace viewer{
		namespace camera{
			// Jpeg�t�@�C�����ꊇ���ĕۑ����邱�Ƃ̐ӔC�N���X
			//  activate �̍ۂɂ� PATH ���w�肷�邱��
			class cFileAgent :public itfFileAgent{
			private:
				bool activeFlag;
				std::string filePath;
				std::string Name;
				std::ofstream fout;

				hmLib::signals::unique_connections SignalConnections;
			public:
				cFileAgent(const std::string& Name_) :activeFlag(false), Name(Name_){}

				bool is_active()const{ return activeFlag; }

			private:
				// log �֐�
				void log_writeData(clock::time_point time_, std::string data_){
					// �J���Ă��Ȃ���΁E�E�E�E
					if(!is_active()) return;

					// ���Ԃ��擾
					date Date;
					Date.from_time(time_);
					std::string str1 = (boost::format("%02d%02d%02d_%02d%02d%02d_%03d") % (Date.Year % 100) % Date.Month%Date.Day%Date.Hour%Date.Min%Date.Sec%Date.mSec).str();

					// ���̎��ԂŃt�@�C������ݒ�
					std::string str = filePath + str1 + ".jpg";

					// �f�[�^�����O
					fout.open(str);
					fout << data_;
					fout.close();
				}
			public:
				void activate(const std::string& Path_){
					if(!is_active()){
						activeFlag = true;
						filePath = Path_ + Name + "\\";

						if(!boost::filesystem::exists(boost::filesystem::current_path() / (Path_ + Name))){
							boost::filesystem::create_directory(boost::filesystem::current_path() / (Path_ + Name));
						}
					}
				}
				void inactivate(){
					if(is_active()){
						activeFlag = false;
					}
				}

				// Log �p�� signal slot
			public:
				// Log�Ƃ��ď������ގ���slot
				void slot_log_writeData(boost::signals2::signal<void(const std::string&, clock::time_point)>& Signal_){
					SignalConnections(hmLib::signals::connect(Signal_, [&](const std::string& data_, clock::time_point time_)->void{this->log_writeData(time_, data_); }));
				}

			};
		}
	}

}
#endif









