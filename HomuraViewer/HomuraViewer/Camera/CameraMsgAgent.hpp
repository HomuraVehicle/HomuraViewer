#ifndef HMR_VIEWER_CAMERA_CAMERAMSGAGENT_INC
#define HMR_VIEWER_CAMERA_CAMERAMSGAGENT_INC 103
#
/*=======hmrSprite=========
hmrSprite:v1_03/130706 hmito
	DataSize���擾�����i�K�ŁADataPos������������悤�ɏC��
hmrSprite v1_02/130511 iwahori
	DoSetTakePicture,DoCommandReset,DoPowerReset�ւ�contact�ǉ�
	���擾��listen/talk���ŐV�̒ʐM�K��ɑΉ�
	setCommandReset��setPowerReset�֐��̈�����ύX
hmrSprite v1_01/130427 iwahori
	hmLib�̃o�[�W�����ύX�ɔ������C��
	Flagirl��Pic��request�ւ�inquiry��ǉ�
hmrSprite v1_00/130420 iwahori
	cSpriteMsgAgent��signal-slot,inquiry-contact�ւ̑Ή�����
2013.01.27 by amby
  �@�V�ʐM�K��ɑΉ�������B
*/
#include "hmLibVer.hpp"
#include <boost/signals2.hpp>
#include <hmLib/signals.hpp>
#include <hmLib/inquiries.hpp>
#include <HomuraViewer/Message/ItfMessageAgent.hpp>
#include <HomuraViewer/modeflags.hpp>

namespace hmr{
	namespace viewer{
		namespace camera{
			class cMsgAgent :public itfMessageAgent{
			private:
				//�f�[�^�֘A
				bool IsData;				//�f�[�^�擾�����H
				std::string DataStr;		//�f�[�^����
				unsigned int DataPos;		//�f�[�^�ʒu
				unsigned int DataSize;		//�f�[�^�T�C�Y
				clock::time_point DataTime;		//�f�[�^����

				//��
				unsigned char Status;		//�J�����X�e�[�^�X
				bool IsErr;					//�G���[��Ԃ��H
				unsigned char ErrCode;		//�G���[�R�[�h
				clock::time_point StatusTime;//�X�e�[�^�X����

				//�ʐ^�B�e�v���t���O
				bool DoSetTakePicture;
				//�w��ʐ^�T�C�Y
				unsigned char PictureSize;   //PC���̎ʐ^�T�C�Y�w��

				//�R�}���h���Z�b�g�v���t���O
				bool DoCommandReset;
				//�p���[���Z�b�g�v�����t���O
				bool DoPowerReset;
			public:
				//�I�[�g�p���[���Z�b�g
				modeflags AutoPowerReset;
				//�I�[�g���C�g�_���@�\
				modeflags AutoLight;
				//���C�gON/OFF
				modeflags Light;
				//�A���B�e���[�h
				modeflags AutoTakePic;
				//�~�j�p�P�b�g���M���[�h
				modeflags MiniPacketMode;
				//���擾���[�h
				modeflags InfoMode;
				//��M�f�[�^�V�O�i�����M
				boost::signals2::signal<void(std::string,clock::time_point)> signal_newPicture;
			private:
				unsigned int MsgPos;
			public:
				cMsgAgent()
					: DataStr()
					, DataPos(0)
					, DataSize(0)
					, PictureSize(0)
					, DoCommandReset(false)
					, DoPowerReset(false)
					, DoSetTakePicture(false)
					, Status(0x00)
					, IsErr(false)
					, ErrCode(0x00){
				}
				bool listen(datum::time_point Time_, bool Err_, const std::string& Str_)override{
					//�J�����̃X�e�[�^�X�擾
					unsigned char c = static_cast<unsigned char>(Str_[0]);

					//�J��������B�e�J�n���m�F
					//  �ʐ^�T�C�Y�ƃf�[�^�T�C�Y�����o��
					// �@auto reset �Ƃ�����Ă��܂��ƁA�J�����̏�Ԃ�������Ȃ��̂ŁA�Ƃ肠�������̃R�}���h�ŃJ�����̌��ݏ󋵂�K���B�e��Ԃɂ��Ă������Ƃɂ���
					if(c == 0x00){
						// �f�[�^������Ȃ���
						if(Str_.size()<4){
							return true;
						}

						// �ʐ^�T�C�Y�ƃf�[�^�T�C�Y�����o��
						IsData = true;
						DataPos = 0;
						DataSize = static_cast<unsigned int>(static_cast<unsigned char>(Str_.at(2))) + (static_cast<unsigned int>(static_cast<unsigned char>(Str_.at(3)) << 8));

						//DataStr���N���A
						DataStr.assign(DataSize, 0x00);

						return false;

					}//�J��������B�e���f�[�^������ꂽ�Ƃ�
					else if(c == 0x01){
						//�f�[�^������Ȃ���
						if(Str_.size()<3)return true;

						//���ۂɃJ��������f�[�^��҂��Ă�����
						// �ύX�̉\�������邽�߁A�����͕ۗ�����(2012.1.27)
						// dataPos ���A�����ł͂���܂łɃQ�b�g�����f�[�^���ɂȂ��Ă��邪�A�������ǂ����邩�H�H
						if(IsData){
							//�f�[�^�T�C�Y
							DataPos = static_cast<unsigned int>(static_cast<unsigned char>(Str_[1])) + (static_cast<unsigned int>(static_cast<unsigned char>(Str_[2])) << 8);
							//			unsigned int DataBegin=DataPos-(Str_.size()-3);
							unsigned int DataBegin = DataPos;

							//�f�[�^��������
							for(unsigned int i = 0; i<Str_.size() - 3; ++i){
								if(DataSize <= DataBegin + i)break;
								DataStr.at(DataBegin + i) = Str_.at(i + 3);
							}

							//�S�Ẵf�[�^����M�����Ƃ�
							//			if(DataSize<=DataPos){
							if(DataSize <= DataBegin + Str_.size() - 3){
								DataTime = Time_;
								signal_setPicture(DataStr, DataTime);
								IsData = false;
							}
							return false;
						}//�v�����ĂȂ���
						else{
							return false;
						}
					}//�R�}���h���Z�b�g�v��������
					else if(c == 0x60){
						DoCommandReset = false;
						return false;
					}//���Z�b�g�v��������
					else if(c == 0x70){
						DoPowerReset = false;
						return false;
					}
					//�I�[�g���Z�b�g�ύX��
					else if((c & 0xFE) == 0x80){
						AutoPowerReset.set_pic(c == 0x80);
						return false;
					}
					//���C�g�_����ԕύX
					else if((c & 0xFE) == 0x30){
						Light.set_pic(c == 0x30);
						return false;
					}
					//�I�[�g���C�g(�����t���b�V��)�ύX
					else if((c & 0xFE) == 0x40){
						AutoLight.set_pic(c == 0x40);
						return false;
					}
					//�~�j�p�P�b�g���M���[�h�ύX
					else if((c & 0xFE) == 0x50){
						MiniPacketMode.set_pic(c == 0x50);
						return false;
					}
					// �A���B�e���[�h�ύX
					else if((c & 0xFE) == 0x10){
						AutoTakePic.set_pic(c == 0x10);
						return false;
					}
					// �������X�V
					else if(c == 0xA0){
						// �f�[�^������Ȃ���
						if(Str_.size()<4){
							return true;
						}

						Status = static_cast<unsigned char>(Str_[1]);
						IsErr = (static_cast<unsigned char>(Str_[2]) != 0);
						ErrCode = static_cast<unsigned char>(Str_[3]);
						StatusTime = Time_;

						return false;
					}
					// ������񑗐M���[�h�ύX
					else if((c & 0xFE) == 0xB0){
						InfoMode.set_pic(c == 0xB0);
						return false;
					}

					return true;
				}
				bool talk(std::string& Str)override{
					Str = "";
					switch(MsgPos){
					case 0:
						++MsgPos;
						//PowerReset�v����
						if(DoPowerReset){
							DoPowerReset = false;
							Str.push_back(static_cast<unsigned char>(0x70));
							return false;
						}
					case 1:
						++MsgPos;
						//AutoPowerReset�ݒ�ύX��
						if(AutoPowerReset){
							if(AutoPowerReset.get_req())Str.push_back(static_cast<unsigned char>(0x80));
							else Str.push_back(static_cast<unsigned char>(0x81));
							return false;
						}
					case 2:
						++MsgPos;
						//CommandReset�v����
						if(DoCommandReset){
							DoCommandReset = false;
							Str.push_back(static_cast<unsigned char>(0x60));
							return false;
						}
					case 3:
						++MsgPos;
						//�I�[�g���C�g�ݒ莞
						if(AutoLight){
							if(AutoLight.get_req())Str.push_back(static_cast<unsigned char>(0x40));
							else Str.push_back(static_cast<unsigned char>(0x41));
							return false;
						}
					case 4:
						++MsgPos;
						//���C�g�̐ݒ�
						if(Light){
							if(Light.get_req())Str.push_back(static_cast<unsigned char>(0x30));
							else Str.push_back(static_cast<unsigned char>(0x31));
							return false;
						}
					case 5:
						++MsgPos;
						//MIni�p�P�b�g���[�h�̐ݒ�
						if(MiniPacketMode){
							if(MiniPacketMode.get_req())Str.push_back(static_cast<unsigned char>(0x50));
							else Str.push_back(static_cast<unsigned char>(0x51));
							return false;
						}
					case 6:
						++MsgPos;
						// �A���B�e���[�h�̐ݒ�
						if(AutoTakePic){
							if(AutoTakePic.get_req()){
								Str.push_back(static_cast<unsigned char>(0x10));
								Str.push_back(static_cast<unsigned char>(0x00));// Picture �T�C�Y�͈�ԏ���������
							} else Str.push_back(static_cast<unsigned char>(0x11));
							return false;
						}
					case 7:
						++MsgPos;
						//�ʐ^�v�����[�h�ɂȂ��Ă���
						if(DoSetTakePicture){
							DoSetTakePicture = false;
							//PictureSize��K���l�ɕ␳
							if(PictureSize<0)PictureSize = 0;
							else if(PictureSize>2)PictureSize = 2;
							//�R�}���h0x00���M
							Str.push_back(static_cast<unsigned char>(0x00));
							Str.push_back(PictureSize);
							return false;
						}
					case 8:
						++MsgPos;
						// ������񑗐M���[�h�̐ݒ�
						if(InfoMode){
							if(InfoMode.get_req())Str.push_back(static_cast<unsigned char>(0xB0));
							else Str.push_back(static_cast<unsigned char>(0xB1));
							return false;
						}
					default:
						return true;
					}
				}
				void setup_listen(void)override{
					ErrCode = 0x00;
					IsErr = false;
				}
				void setup_talk(void)override{
					MsgPos = 0;
				}
			private:
				hmLib::signals::unique_connections SignalConnections;
				hmLib::inquiries::unique_connections InquiryConnections;

			public:
				void setTakePicture(){
					InfoMode.set_req(true);
					DoSetTakePicture = true;
				}
				void setTakePictureLarge(){
					PictureSize = 2;
					setTakePicture();
				}

				void setCommandReset(){ DoCommandReset = true; }
				void setPowerReset(){ DoPowerReset = true; }

				void setPictureSize(unsigned char Size_){
					if(Size_>2)return;
					PictureSize = Size_;
				}
				unsigned char getPictureSize()const{ return PictureSize; }

				bool getIsData()const{ return IsData; }
				std::pair<unsigned int, unsigned int> getDataPosSize()const{
					return std::make_pair(DataPos, DataSize);
				}
				clock::time_point getDataTime()const{ return DataTime; }

				unsigned char getStatus()const{ return Status; }
				bool getIsErr()const{ return IsErr; }
				unsigned char getErrCode()const{ return ErrCode; }
				clock::time_point getStatusTime()const{ return StatusTime; }
			public:
				boost::signals2::signal<void(const std::string&, clock::time_point)> signal_setPicture;
				boost::signals2::signal<void(const std::string&, clock::time_point)> signal_setLogPicture;

				void slot_setTakePicture(boost::signals2::signal<void(void)>& Signal_){
					SignalConnections(hmLib::signals::connect(Signal_, [&](void)->void{this->setTakePicture(); }));
				}
				void contact_getDoTakePictureFlag(hmLib::inquiries::inquiry<bool>& Inquiry_){
					InquiryConnections(hmLib::inquiries::connect(Inquiry_, DoSetTakePicture));
				}
				void slot_setTakePictureLarge(boost::signals2::signal<void(void)>& Signal_){
					SignalConnections(hmLib::signals::connect(Signal_, [&](void)->void{this->setTakePictureLarge(); }));
				}

				void slot_setCommandReset(boost::signals2::signal<void(void)>& Signal_){
					SignalConnections(hmLib::signals::connect(Signal_, [&](void)->void{this->setCommandReset(); }));
				}
				void contact_getDoCommandResetFlag(hmLib::inquiries::inquiry<bool>& Inquiry_){
					InquiryConnections(hmLib::inquiries::connect(Inquiry_, DoCommandReset));
				}

				void slot_setPowerReset(boost::signals2::signal<void(void)>& Signal_){
					SignalConnections(hmLib::signals::connect(Signal_, [&](void)->void{this->setPowerReset(); }));
				}
				void contact_getDoPowerResetFlag(hmLib::inquiries::inquiry<bool>& Inquiry_){
					InquiryConnections(hmLib::inquiries::connect(Inquiry_, DoPowerReset));
				}

				void slot_setPictureSize(boost::signals2::signal<void(unsigned char)>& Signal_){
					SignalConnections(hmLib::signals::connect(Signal_, [&](unsigned char c)->void{this->setPictureSize(c); }));
				}
				void contact_getPictureSize(hmLib::inquiries::inquiry<unsigned char>& Inquiry_){
					InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->unsigned char{return this->getPictureSize(); }));
				}
				void slot_setPictureSizeLarge(boost::signals2::signal<void(void)>& Signal_){
					SignalConnections(hmLib::signals::connect(Signal_, [&](void)->void{this->setPictureSize(2); }));
				}

				void contact_getIsData(hmLib::inquiries::inquiry<bool>& Inquiry_){
					InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->bool{return this->getIsData(); }));
				}
				void contact_getDataPosSize(hmLib::inquiries::inquiry<std::pair<unsigned int, unsigned int>>& Inquiry_){
					InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->std::pair<unsigned int, unsigned int>{return this->getDataPosSize(); }));
				}
				void contact_getDataTime(hmLib::inquiries::inquiry<clock::time_point>& Inquiry_){
					InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->clock::time_point{return this->getDataTime(); }));
				}

				void contact_getStatus(hmLib::inquiries::inquiry<unsigned char>& Inquiry_){
					InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->unsigned char{return this->getStatus(); }));
				}
				void contact_getStatusTime(hmLib::inquiries::inquiry<clock::time_point>& Inquiry_){
					InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->clock::time_point{return this->getStatusTime(); }));
				}
				void contact_getIsErr(hmLib::inquiries::inquiry<bool>& Inquiry_){
					InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->bool{return this->getIsErr(); }));
				}
				void contact_getErrCode(hmLib::inquiries::inquiry<unsigned char>& Inquiry_){
					InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->unsigned char{return this->getErrCode(); }));
				}
			};
		}
	}
}

#
#endif
