#ifndef HMRDXIO_INC
#define HMRDXIO_INC 103

#include <hmLib/config_vc.h> // std::min, std::max�@�́@windows�}�N���ł���`����Ă���A�����I�ɕ������Ă��Ȃ��Ƃ���

#include <iostream>
#include <string>
#include <algorithm>
#include <boost/signals2.hpp>

#include "hmLibVer.hpp"
//#include "iologgate.hpp"
#include <hmLib_v2/hmBasic.hpp>
#include <hmLib_v2/dxObject.hpp>
#include <hmLib_v2/dxArea.hpp>
#include <hmLib_v2/dxColorSet.hpp>
#include <hmLib/signals.hpp>
#include <hmLib/inquiries/inquiry.hpp>
#include <hmLib/inquiries/range_inquiry.hpp>

#include <hmLib_v2/hmBin_To_hmAsc.hpp>
#include <HomuraViewer/chrono.hpp>
#include <HomuraViewer/Message/PacketData.hpp>
#include "iologgate.hpp"

/* ---- hmrDxIO-----

�l������g�p���@
	�P�A�f�[�^��IN,OUT�����n��ł܂Ƃ܂��ĕ\���ł���΂悢
		����f�[�^�ɂ͎��ԂƁA�f�[�^��\������

	�Q�A�S��d�̎��ɁA���M�A��M�̑��𕪂��������Ƃ����邾�낤
		���͕ʂɂł���悤�ɍ��

	�R�A�o�b�t�@�̏�Ԃ��m�F������
		����Ă��鐔�A�x��Ă��Ȃ�����������Ɗm�F����

	�S�A��������ł��ő��M�ł���
		��ł��őłĂ��ʂ����

	�T�A���M�f�[�^���~�߂�A�Ԋu��ύX���������Ƃ����邾�낤
		���M�Ԋu�����߂邽�߂̃`�F�b�N�{�b�N�X
		
	�U�A�f�o�b�O���Ɏ�M�f�[�^���~�߂����������邾�낤
		�ʐM�R�l�N�g�{�^���őΉ�

	�V�A����M���Z�b�g�Ŏ~�߂������Ƃ����邾�낤
		����M�X�g�b�v�{�^��������


	IO�֘A�̕`��N���X
	����͋@�\�ɂ���ĕ����Ă���
	IO�֘A�̋@�\�̃N���X�������ɂԂ�����

	�����炻�ꂼ��̗v�f�̏ꏊ�́A�ʂ̃N���X�ɂ���Đݒ肷��B
	�V�O�i���A�X���b�g�ɑΉ�����DX�N���X�����

hmrDxIO.hpp :v1_03/130519 amby
	�`�敔����ύX
	
	�̂���
	�@TimeOut�Ȃǂ̃R���g���[�� -> OK
	�@Buf�̃R���g���[�� -> OK
	
	��x���ׂĂȂ��Ă݂�K�v�������D�B�B�B
	�Ȃ��ADEBUG�Ə����Ă镔����Inquirely �́@�Ȃ��Ă��Ȃ��Ƃ���ƂȂ��Ă���

hmrDxIO.hpp :v1_02/130427 amby
	�`�敔�����쐬
	�@�\�֘A����������

	�c��^�X�N
	�@Signal �Ȃǂ̐ڑ�
	�@�X�y�[�X��L�����p
	�@�o�b�t�@�̕\�����@�ɂ��ā@�Œ�ŕ\���ł����肷�邩	 �@�@
	 

hmrDxIO.hpp :v1_01/130420 amby
	�`�敔�����쐬
	Frame ���쐬�@Fn �œǂނ悤�ɂ��� 

	�`��Ƀo�O���c��B
	�����炭�A���Ă�[���֘A���Ǝv���邩��A�o�O�����s���B



hmrDxIO.hpp :v1_00/130406 amby
	�����o�[�W����
	�����̃N���X�����b�s���O����`�őΉ�����

	�ȉ��ɂ��Ă��̃t�@�C���Ŏ���
	dxIOSendBox 
	dxfIOSta
	dxfIOBufSta
*/

namespace hmr{
	namespace viewer{
		//  �R�}���h�ł����ޗp��
		class dxosIOSendBox:public dxReObject,public hmoBox{
		private:
			dxRGB Clr;
			dxInput Inp;
			//Pint inpBoxSize;

		// signal ����
		public:
			// ���M�����񑗐M �V�O�i��
			boost::signals2::signal<void(std::string)> signal_poutString;

		public:
			//dxfIOLog::cInputSetter getInpSetter(){return dxfIOLog::cInputSetter(Inp);}
		public:
			dxosIOSendBox(Pint Size_,const dxColor& Clr_)
				:hmoBox(Size_)
				,Clr(Clr_){
	//			Inp.active();
			}

			// DxInput, DxImage �Ƃ��̏ꍇ�A�K������ini()�֐���dx::ini()���ꂽ��ɌĂԂ��Ƃ��K�v
			void ini(){
				Inp.make(256,1,0,0," please input a string");
			}
		protected:
			virtual int redraw(dxO& dxo){
			
				// InpBox�@�́@�`��
				if(dxo.draw(Pint(0,0),dxoInpBox(size,Inp,Clr))==2){
					// signal ���M
					signal_poutString(hm::hmAscToBin(Inp.getStr()));
					//rIO.pout<<hm::hmAscToBin(Inp.getStr());
					Inp.setStr("");
					Inp.inactive();
				}
				return 0;
			}
		};

		// �f�[�^�\���؂�ւ��悤��
		class dxosSelectIOPaint:public dxReObject,public hmoBox{
		private:
			bool InPaintFlag;
			bool OutPaintFlag;
			dxRGB inClr, outClr;
			Pint Size;

		public:
			// �R���X�g���N�^
			dxosSelectIOPaint(Pint Size_, const dxRGB& inClr_ = CLR::LightDullOrenge, const dxRGB& outClr_ = CLR::LightDullSkyblue)
				:hmoBox(Size_),InPaintFlag(true),OutPaintFlag(true), inClr(inClr_), outClr(outClr_), Size(Size_){}

		public:
			// signal �n��̒�`
			boost::signals2::signal<void(bool)> signal_require_paintInput;
			boost::signals2::signal<void(bool)> signal_require_paintOutput;
		public:
			// inquire �n��̒�`
			hmLib::inquiries::inquiry<bool> InPaintFlagInq;
			hmLib::inquiries::inquiry<bool> OutPaintFlagInq;

		protected:
			// �`��
			virtual int redraw(dxO& dxo){
				try{
					// �X�V �ƂƂ��� �`�F�b�N�{�b�N�X�̕`��
					try{
						InPaintFlag = InPaintFlagInq();
						if(dxo.draw(Pint(0,0),dxoButIO(Pint(Size.x / 2, Size.y),"IN", inClr, InPaintFlag),true) == 1){
							//if(dxo.draw(Pint(0,0),dxoCheck(Pint(Size.x / 2, Size.y),"In�f�[�^�`��",Size.y,Clr,InPaintFlag))==1){
							InPaintFlag = !InPaintFlag;
							signal_require_paintInput(InPaintFlag);
						}
					}catch(const hmLib::inquiries::unconnected_exception&){
						// Inquire �ɃA�N�Z�X�ł��Ȃ������I�I�@�ł��A����Ȃ̃��C���֐��ɂ͊֌W�Ȃ�������ݏ���
						dxo.draw(Pint(0, 0), dxoStr("Inquiry:: InPaintFlag�擾���s") );
					}

					try{
						OutPaintFlag = OutPaintFlagInq();
						if(dxo.draw(Pint(Size.x /2 ,0),dxoButIO(Pint(Size.x / 2, Size.y),"OUT", outClr, OutPaintFlag),true) == 1){
							//if(dxo.draw(Pint(Size.x /2 ,0),dxoCheck(Pint(Size.x / 2, Size.y),"Out�f�[�^�`��",20,Clr,OutPaintFlag))==1){
							OutPaintFlag = !OutPaintFlag;
							signal_require_paintOutput(OutPaintFlag);
						}
					}catch(const hmLib::inquiries::unconnected_exception&){
						// Inquire �ɃA�N�Z�X�ł��Ȃ������I�I�@�ł��A����Ȃ̃��C���֐��ɂ͊֌W�Ȃ�������ݏ���
						dxo.draw(Pint(Size.x /2, 0), dxoStr("Inquiry:: OutPaintFlag�擾���s") );
					}
				}catch(const hmLib::exceptions::exception&){
					// �z��O�̃G���[
					dxo.draw(Pint(0, 0), dxoStr("�z��O�̃G���[�@"));
				}
				return 0;
			}
		};

		//! �f�[�^�m�F�p�̑� Function
		//		���M�A��M���t���O�Ő؂�ւ���悤�ɂ���
		//		�擾���Ԃ�InOut��ʁA�擾�f�[�^���o���Ă�����
		#define IO_DISPLAY_INFO_SIZE 125 // IO��ʂ̏��߂ɏo�����p�̘g�i���Ԃ̂݁j
		template<typename iterator>
		class dxFnIOLog:public dxFn{
		private:
			// IO�f�[�^�^�C�v��typedef 
			//typedef std::pair<bool,system_clock_iologtype> io_iologtype;
			//typedef std::vector<io_iologtype> container;
			//typedef container::const_iterator contanerItr;

			// �`��ɕK�v�ȕϐ�
			iterator ItrBegin;
			iterator ItrEnd;

			dxRGB InClr;
			dxRGB OutClr;

			bool InPaintFlag;
			bool OutPaintFlag;

		public:
			// �R���X�g���N�^
			dxFnIOLog(iterator Begin_, iterator End_, bool InPaint_, bool OutPaint_, dxRGB InClr_=CLR::LightDullOrenge, dxRGB OutClr_= CLR::LightDullSkyblue)
				:ItrBegin(Begin_), ItrEnd(End_), InPaintFlag(InPaint_), OutPaintFlag(OutPaint_), 
				InClr(InClr_), OutClr(OutClr_)
			{}
			// Draw �֐�
			int fndraw(dxO& dxo){
				// �`��ʒu
				dxO::Acs<dxScrlFrame> Acs(dxo);
				int DrawPosY = 0;

				// ����͈�ԍ���̍��W�ł��邱�Ƃɒ���
				// �@���������������A�E������x�����̕����ɂȂ��Ă���@
				Pint ViewPos  = Pint(Acs().viewpos.x, Acs().viewpos.y);
				Pint ViewSize = Pint(Acs().size.x, Acs().size.y);

				// �`��J�n ���݂̏ꍇ�̂�
				if(InPaintFlag && OutPaintFlag){
					// �f�[�^�����邱�Ƃ��m�F
					if( ItrEnd - ItrBegin > 0 ){
						// y�����ɂ��ă_�~�[�ŕ`�悵�Ă���
						dxo.draw(Pint(0, static_cast<int>((ItrEnd - ItrBegin)*45+20)),dxoNull());
						//dxo.draw(Pint(5,5),dxoStr("Test TEst"));
					
						// ���݌��Ă���ʒu������`�悷��

						//y�����ɂ��ĕ`�悷��O����m��
						// �ŏI�f�[�^����`�悵�Ă����̂ŁA�����Z���ăC�e���[�^�𑫂��Ă���
						auto DBegin = ItrEnd - std::min(std::max(0,(ViewPos.y)/45 -1),(ItrEnd - ItrBegin));
						auto DEnd = ItrEnd   - std::min(std::max(0,(ViewPos.y+ViewSize.y)/45 + 1),ItrEnd - ItrBegin);
						//�`��ʒu�ƂȂ�悤�ɕ\��
						if((ViewPos.y)/45 -1 < 0) DrawPosY = 0;
						else DrawPosY=((ViewPos.y)/45)*45;

						//x�����ɂ��ĕ`�悷��̈���m��
						// �����̃T�C�Y�́@24�A���߂̃f�[�^���̃T�C�Y�� 125 �Ƃ��Ă݂�

						int StrBegin = std::max(0, (ViewPos.x  - IO_DISPLAY_INFO_SIZE) / 24);
						int StrEnd = std::max(0, (ViewPos.x  + ViewSize.x - IO_DISPLAY_INFO_SIZE) / 24 );
						//�`��ʒu
						//if((ViewPos.x - ViewSize.x)/24 -1 < 0) DrawPos.x = 5;
						//else DrawPos.x = ( (ViewPos.x - ViewSize.x)/24 ) * 24 + 5;

						//�`��ʒu�̂ݕ`��
						//  �C�e���[�^�������Z���Čv�Z���Ă���
						for(auto pitr=DBegin - 1;pitr != DEnd;--pitr){
							dxRGB Clr;
							// Color �ݒ�
							if(pitr->first)Clr=InClr;
							else Clr=OutClr;

							// Dummy��x�����ɕ`�悵�Ă���
							dxo.draw(Pint(IO_DISPLAY_INFO_SIZE + pitr->second.Str.size() * 24, DrawPosY),dxoNull());

							//-------  String �f�[�^�����ɕ`��	
							if(StrBegin == 0)// Header �����̕`��
							{
								/*
								// In or Out
								if(pitr->first){
									dxo.draw(Pint(5,DrawPosY +10), dxoStr("I", Clr));
								}else{
									dxo.draw(Pint(5,DrawPosY +10), dxoStr("O", Clr));
								}
								*/

								// ���ԕ`��
								//dxo.draw(Pint(25,DrawPosY+2), dxoStr("Time Start", Clr));
								//dxo.draw(Pint(120,DrawPosY+2),dxoStr(time_to_hms(pitr->second.BeginTime),Clr));
								dxo.draw(Pint(5,DrawPosY+2),dxoStr(time_to_hms(pitr->second.EndTime),Clr));

								//dxo.draw(Pint(25,DrawPosY + 22), dxoStr("Time End", Clr));
								//dxo.draw(Pint(120,DrawPosY + 22),dxoStr(time_to_hms(pitr->second.EndTime), Clr));
								dxo.draw(Pint(5,DrawPosY + 22),dxoStr(time_to_hms(pitr->second.BeginTime), Clr));
														
							}
							// �f�[�^�����̕`��
							if( (int)pitr->second.Str.size() > StrBegin ){
								for(int i = StrBegin; i < std::min( (StrEnd) , ((int)pitr->second.Str.size()) ) ; i++){
									dxo.draw(Pint(IO_DISPLAY_INFO_SIZE + i*24, DrawPosY),dxoStrP(Pint(24,20),hmstd::fStr("%c",pitr->second.Str[i]),Clr));
									dxo.draw(Pint(IO_DISPLAY_INFO_SIZE + i*24, DrawPosY+ 20),dxoStrP(Pint(24,20),hmstd::fStr("%02x",pitr->second.Str[i] & 0xff),Clr));
									// char�^�Ȃ̂ŕ��̐��ɂȂ��Ă���Aprintf�͓�����4byte int�@�Ɍ^�ϊ����Ă���̂ŁA0x99 -> 0xffffff99�@�ɂȂ��Ă��܂��B�����and���Z�����Ă���
								}
							}
							//�`��ꏊ�̕ύX
							DrawPosY += 45;
						}
					}else{
						dxo.draw(Pint(5,10), dxoStr("No Data", CLR::DarkYellow));
					
						/*
						dxo.draw(Pint(5, 20), dxoStr("Iaaaaaaa", InClr));
						dxo.draw(Pint(5, 35), dxoStr("Iaaaaaaa", OutClr));
						dxo.draw(Pint(5, 50), dxoStr("LightDullMagenta", CLR::LightDullMagenta ));
						dxo.draw(Pint(5, 65), dxoStr("LightDullSkyblue", CLR::LightDullSkyblue ));
						dxo.draw(Pint(5, 80), dxoStr("LightDullOrenge", CLR::LightDullOrenge ));
						dxo.draw(Pint(5, 95), dxoStr("SoftBlue", CLR::SoftBlue));
						dxo.draw(Pint(5, 110), dxoStr("SoftOrenge", CLR::SoftOrenge));
						dxo.draw(Pint(5, 125), dxoStr("SoftSkyblue", CLR::SoftSkyblue ));
						dxo.draw(Pint(5, 140), dxoStr("SoftViolet", CLR::SoftViolet ));
						dxo.draw(Pint(5, 155), dxoStr("SoftYellow", CLR::SoftYellow ));
						dxo.draw(Pint(5, 170), dxoStr("DarkSeagreen", CLR::DarkSeagreen));
						dxo.draw(Pint(5, 185), dxoStr("DarkBlue", CLR::DarkBlue));
						dxo.draw(Pint(5, 200), dxoStr("LightGreen", CLR::LightGreen ));
						dxo.draw(Pint(5, 215), dxoStr("Seagreen", CLR::Seagreen ));
						dxo.draw(Pint(5, 230), dxoStr("DarkSoftMagenta", CLR::DarkSoftMagenta ));
						dxo.draw(Pint(5, 245), dxoStr("CLR::DarkSoftSeagreen", CLR::DarkSoftSeagreen));
						dxo.draw(Pint(5, 260), dxoStr("CLR::PaleDullSeaGreen", CLR::PaleDullSeagreen));
						*/
					}
				}
				// �`��f�[�^���Ȃ��ꍇ
				else if(!InPaintFlag && !OutPaintFlag){
					dxo.draw(Pint(5,10), dxoStr("No Data"));
				}
				// �`�悷����̂����߂��Ă���ꍇ
				// ������͏����Ɏ��Ԃ��������Ă��܂�(�S���ɃA�N�Z�X���Ă��܂��Ă���)�̂ŁA��������if����ɂ���
				else{
					// �f�[�^�����邱�Ƃ��m�F
					if( ItrEnd - ItrBegin > 0 ){
						int DataNum = 0;// ���݂̃f�[�^�̔ԍ�

						// �Ђ�����ɕ`�悵�Ă���
						for(auto pitr = ItrEnd - 1; pitr != ItrBegin ; --pitr){
							// �K���f�[�^�����o���čs��
							if(pitr->first == InPaintFlag){
								++DataNum;
								DrawPosY = (DataNum - 1) * 45;

								// �`��͈͂ɓ����Ă���Ε`�悷��
								if( (DataNum) * 45 > ViewPos.y && (DataNum - 1) * 45 < (ViewPos.y + ViewSize.y)){
									// �`�揈�����s��
									dxRGB Clr;
									// Color �ݒ�
									if(pitr->first)Clr=InClr;
									else Clr=OutClr;

									// Dummy��x�����ɕ`�悵�Ă���
									dxo.draw(Pint(IO_DISPLAY_INFO_SIZE + pitr->second.Str.size() * 24, DrawPosY),dxoNull());

									// �`��ʒu�̌���
									int StrBegin = std::max(0, (ViewPos.x  - IO_DISPLAY_INFO_SIZE) / 24);
									int StrEnd = std::max(0, (ViewPos.x  + ViewSize.x - IO_DISPLAY_INFO_SIZE) / 24 );

									//-------  String �f�[�^�����ɕ`��	
									if(StrBegin == 0)// Header �����̕`��
									{
										/*
										// In or Out
										if(pitr->first){
											dxo.draw(Pint(5,DrawPosY +10), dxoStr("I", Clr));
										}else{
											dxo.draw(Pint(5,DrawPosY +10), dxoStr("O", Clr));
										}
										*/

										// ���ԕ`��
										//dxo.draw(Pint(25,DrawPosY+2), dxoStr("Time Start", Clr));
										//dxo.draw(Pint(120,DrawPosY+2),dxoStr(time_to_hms(pitr->second.BeginTime),Clr));
										dxo.draw(Pint(5, DrawPosY+2),dxoStr(time_to_hms(pitr->second.EndTime),Clr));

										//dxo.draw(Pint(25,DrawPosY + 22), dxoStr("Time End", Clr));
										//dxo.draw(Pint(120,DrawPosY + 22),dxoStr(time_to_hms(pitr->second.EndTime), Clr));
										dxo.draw(Pint(5, DrawPosY + 22),dxoStr(time_to_hms(pitr->second.BeginTime), Clr));
									}
									// �f�[�^�����̕`��
									if( (int)pitr->second.Str.size() > StrBegin ){
										for(int i = StrBegin; i < std::min( (StrEnd) , ((int)pitr->second.Str.size()) ) ; i++){
											dxo.draw(Pint(IO_DISPLAY_INFO_SIZE + i*24, DrawPosY),dxoStrP(Pint(24,20),hmstd::fStr("%c",pitr->second.Str[i]),Clr));
											dxo.draw(Pint(IO_DISPLAY_INFO_SIZE + i*24, DrawPosY+ 20),dxoStrP(Pint(24,20),hmstd::fStr("%02x",pitr->second.Str[i] & 0xff),Clr));
											// char�^�Ȃ̂ŕ��̐��ɂȂ��Ă���Aprintf�͓�����4byte int�@�Ɍ^�ϊ����Ă���̂ŁA0x99 -> 0xffffff99�@�ɂȂ��Ă��܂��B�����and���Z�����Ă���
										}
									}	
								}
							}
						}
						// �K���f�[�^���Ȃ������ꍇ�̏���
						if(DataNum == 0){
							dxo.draw(Pint(5,10), dxoStr("No Data"));
						}
						// y�����ɂ��ă_�~�[�ŕ`�悵�Ă���
						else{
							dxo.draw(Pint(0, static_cast<int>(DataNum*45+0)),dxoNull());
						}
					}else{
						dxo.draw(Pint(5,10), dxoStr("No Data"));
					}
				}

				return 0;

			}
		};

		//! IO�f�[�^�m�F�p�̑�
		// �`��N���X static
		template<typename iterator>
		class dxosIO:public dxReObject,public hmoBox{
		private:
			// Frame�`��p�֐�
			//dxFnIOLog FnIOLog;
			// Frame 
			dxScrlFrame IOLogFrame;

		private:
			// Color �p�̕ϐ�
			dxRGB Clr,SubClr;
			Pint size;
			// ���ۂɂ͂��̃t���O�͂��̃N���X�ŊǗ�����
			bool InPaintFlag, OutPaintFlag;

		// Inquiry �n���̐錾
		private:
			// Inquiry�p�̌^�ϊ�
			typedef hmLib::inquiries::range_inquiry<iterator> iolog_inquiry;
		public:
			//ioLog�o�b�t�@�ւ̃A�N�Z�X�p
			iolog_inquiry IoLogBufInq;

		// contact �n��
			void contact_InPaintFlag(hmLib::inquiries::inquiry<bool> &inq_){
				hmLib::inquiries::connect(inq_, InPaintFlag);
			}
			void contact_OutPaintFlag(hmLib::inquiries::inquiry<bool> &inq_){
				hmLib::inquiries::connect(inq_, OutPaintFlag);
			}

		// Signal-Slot �n���̐錾
		private:
			hmLib::signals::unique_connections connections;
		public:
			// slot �p�̊֐�
			void setInPaintMode(bool InPaintFlag_){
				InPaintFlag = InPaintFlag_;
			}
			void setOutPaintMode(bool OutPaintFlag_){
				OutPaintFlag = OutPaintFlag_;
			}
			// slot �ݒ�֐�
			void slot_setInPaintMode(boost::signals2::signal<void(bool)> &sig_){
				connections(sig_.connect([&](bool v1)->void{this->setInPaintMode(v1);}));
			}
			void slot_setOutPaintMode(boost::signals2::signal<void(bool)> &sig_){
				connections(sig_.connect([&](bool v1)->void{this->setOutPaintMode(v1);}));
			}
	
		protected:
			//virtual int redraw(dxO& dxf);
			virtual int redraw(dxO& dxo){
			
				try{
					// dxScrlFlame �`��
					dxFnIOLog<iterator> FnIOLog(IoLogBufInq.begin(), IoLogBufInq.end(), InPaintFlag, OutPaintFlag);
			
					IOLogFrame.set(FnIOLog, size, Clr); 
					dxo.draw(Pint(0,0), IOLogFrame);
			
				}catch(const hmLib::inquiries::unconnected_exception& Excp){
					// Inquire �ł̃G���[
					dxo.draw(Pint(0,0), dxoStr("Inquiry:: �f�[�^�o�b�t�@�擾�Ɏ��s"));
					dxo.draw(Pint(0,30), dxoStr(Excp.what()));
					return 1;
				}catch(const hmLib::exceptions::exception& Excp){
					// ���̑��ł̃G���[
					dxo.draw(Pint(0,0), dxoStr("�G���[����"));
					dxo.draw(Pint(0,30), dxoStr(Excp.what()));
					return 1;
				}
			
				return 0;
			}

		public:

			dxosIO(Pint size_, const dxRGB& Clr_)
				:hmoBox(size_), InPaintFlag(true), OutPaintFlag(true)
				//FnIOLog(iterator Begin_, iterator End_, bool InPaint_, bool OutPaint_)
				//IOLogBox(Pint(600,800),CLR::Cyan,CLR::Pink)
			{
			
				Clr=Clr_;
				dxHLS tmp(Clr);

				tmp.sat/=2;
				SubClr=tmp;

				size = size_;
		
			}
		};		

		//�ŐV�f�[�^�̕`��ɂ��Ή������o�[�W����
		template<typename iterator>
		class dxFnIOLog2:public dxFn{
			using datum = message::datum;
			using packet = message::packet;
			using seltype = std::pair<packet::no_type,datum::no_type> ;
		private:
			iterator Begin;									//PacketLog�̐擪
			iterator End;									//PacketLog�̖���
			const std::string* pInStr;						//�ŐV����Packet
			const std::string* pOutStr;						//�ŐV�o��Packet
			const dxRGB& InputClr;							//���͑��̐F
			const dxRGB& OutputClr;							//�o�͑��̐F
			bool InPaintFlag;								//In�f�[�^��\�����邩�ǂ���
			bool OutPaintFlag;								//Out�f�[�^��\�����邩�ǂ���
		public:
			dxFnIOLog2(iterator Begin_,iterator End_,const std::string* pInStr_,const std::string* pOutStr_,bool InPaintFlag_,bool OutPaintFlag_,const dxRGB& InputClr_=CLR::LightDullOrenge,const dxRGB& OutputClr_=CLR::LightDullSkyblue)
				:Begin(std::move(Begin_))
				,End(std::move(End_))
				,pInStr(pInStr_)
				,pOutStr(pOutStr_)
				,InPaintFlag(InPaintFlag_)
				,OutPaintFlag(OutPaintFlag_)
				,InputClr(InputClr_)
				,OutputClr(OutputClr_){
			}
			int fndraw(dxO& dxo){
				/*ScrlFrame����*/
				try{
					//�`��ʒu�m��
					dxO::Acs<dxScrlFrame> Acs(dxo);

					//IO�̗�����`�悷��ꍇ
					if(InPaintFlag && OutPaintFlag){
						int ViewPos=Acs().viewpos.y;
						int ViewSize=Acs().size.y;

						//�`��͈͂Ɋ܂܂��data��iterator���v�Z
						auto DBegin=Begin+std::min(std::max(0,(-ViewPos-ViewSize)/45-1),End-Begin);
						auto DEnd=Begin+std::min(std::max(0,(-ViewPos)/45+1),End-Begin);

						//�ŉ����̃f�[�^�̕`��ʒu�ƂȂ�悤�ɏ����n�ݒ�
						int DrawPos=-45*((DBegin-Begin)+1);

						//�`��ʒu�̂ݕ`��
						for(auto pitr=DBegin;pitr!=DEnd;++pitr){
							dxRGB Clr;
							if(pitr->first)Clr=InputClr;
							else Clr=OutputClr;

							int DrawXPos=125;

							//�����`��
							dxo.draw(Pint(0,DrawPos),dxoStrP(Pint(120,45),time_to_hms(pitr->second.EndTime),CLR::LightGray));

							//Packet���f�[�^�����ɕ`��
							for(auto itr=pitr->second.Str.begin();itr!=pitr->second.Str.end();++itr){
								dxo.draw(Pint(DrawXPos, DrawPos),dxoStrP(Pint(24,20),hmstd::fStr("%c",*itr),Clr));
								dxo.draw(Pint(DrawXPos, DrawPos+20),dxoStrP(Pint(24,20),hmstd::fStr("%02x",static_cast<unsigned char>(*itr)),Clr));
								DrawXPos+=24;
							}

							DrawPos-=45;
						}

						//�`��ʒu��{���̈ʒu�Ɉړ�
						DrawPos=-45*((End-Begin)+1);

						//OutPacket��
						if(pOutStr!=nullptr){
							//�`��͈͓��̎�
							if(ViewPos<=DrawPos+40 && DrawPos<ViewPos+ViewSize){
								dxo.draw(Pint(0,DrawPos),dxoTriangle(Pint(0,40),Pint(10,20),OutputClr));
								int DrawXPos=125;
								//Packet���f�[�^�����ɕ`��
								for(auto itr=pOutStr->begin();itr!=pOutStr->end();++itr){
									dxo.draw(Pint(DrawXPos, DrawPos),dxoStrP(Pint(24,20),hmstd::fStr("%c",*itr),OutputClr));
									dxo.draw(Pint(DrawXPos, DrawPos+20),dxoStrP(Pint(24,20),hmstd::fStr("%02x",static_cast<unsigned char>(*itr)),OutputClr));
									DrawXPos+=24;
								}
							}
							DrawPos-=45;
						}

						//InPacket��
						if(pInStr!=nullptr){
							//�`��͈͓��̎�
							if(ViewPos<=DrawPos+40 && DrawPos<ViewPos+ViewSize){
								dxo.draw(Pint(0,DrawPos),dxoTriangle(Pint(0,40),Pint(10,20),InputClr));
								int DrawXPos=125;
								//Packet���f�[�^�����ɕ`��
								for(auto itr=pInStr->begin();itr!=pInStr->end();++itr){
									dxo.draw(Pint(DrawXPos, DrawPos),dxoStrP(Pint(24,20),hmstd::fStr("%c",*itr),InputClr));
									dxo.draw(Pint(DrawXPos, DrawPos+20),dxoStrP(Pint(24,20),hmstd::fStr("%02x",static_cast<unsigned char>(*itr)),InputClr));
									DrawXPos+=24;
								}
							}
							DrawPos-=45;
						}

						//�Ō�ɕ`��ʒu�̎w����s���Ă���
						dxo.draw(Pint(0,0),dxoNull());
						dxo.draw(Pint(0,DrawPos+45),dxoNull());
					}//ID���I������Ă���ꍇ
					else if(InPaintFlag || OutPaintFlag){
						//�`��͈͂��擾
						int ViewPos=Acs().viewpos.y;
						int ViewSize=Acs().size.y;
						//�`��ʒu���s���Ȃ�ŁA�����n����X�^�[�g
						int DrawPos=-45;

						//��������Œ��ׂĂ���
						for(auto pitr=Begin;pitr!=End;++pitr){
							dxRGB Clr;
							if(pitr->first)Clr=InputClr;
							else Clr=OutputClr;

							int DrawXPos=125;

							bool DrawFlag=false;
							bool ESCFlag=false;

							//�`��Ώۂ̏ꍇ
							if((pitr->first&&InPaintFlag)||((!pitr->first)&&OutPaintFlag)){
								//�����`��
								dxo.draw(Pint(0,DrawPos),dxoStrP(Pint(120,45),time_to_hms(pitr->second.EndTime),CLR::LightGray));

								//�`��͈͂ɓ����Ă��鎞�����`��
								if(ViewPos-40<DrawPos && DrawPos<ViewPos+ViewSize){
									//Packet���f�[�^�����ɕ`��
									for(auto itr=pitr->second.Str.begin();itr!=pitr->second.Str.end();++itr){
										dxo.draw(Pint(DrawXPos, DrawPos),dxoStrP(Pint(24,20),hmstd::fStr("%c",*itr),Clr));
										dxo.draw(Pint(DrawXPos, DrawPos+20),dxoStrP(Pint(24,20),hmstd::fStr("%02x",static_cast<unsigned char>(*itr)),Clr));
										DrawXPos+=24;
									}
								}

								//�`�悵�����ǂ����Ɋւ�炸�A�ʒu��񂾂��͍X�V���Ă���
								DrawPos-=45;
							}
						}

						//�Ō�ɕ`��ʒu�̎w����s���Ă���
						dxo.draw(Pint(0,0),dxoNull());
						dxo.draw(Pint(0,DrawPos+45),dxoNull());
					}//�`�悷�ׂ��f�[�^���Ȃ��ꍇ
					else{
						dxo.draw(Pint(5,10), dxoStr("No Data"));
					}


				}catch(const std::exception& Except){
					dxo.draw(Pint(5, 5),dxoStr("***ERROR***"));
					dxo.draw(Pint(5,25),dxoStr(Except.what()));
				}catch(const std::string& Str){
					dxo.draw(Pint(5, 5),dxoStr("***ERROR***"));
					dxo.draw(Pint(5,25),dxoStr(Str));
				}

				return 0;
			}
		};
		template<typename iterator>
		class dxosIO2:public dxReObject,public hmoBox{
		private:
			// Frame�`��p�֐�
			//dxFnIOLog FnIOLog;
			// Frame 
			dxScrlFrame IOLogFrame;

		private:
			// Color �p�̕ϐ�
			dxRGB Clr,SubClr;
			bool InPaintFlag, OutPaintFlag;					// ���ۂɂ͂��̃t���O�͂��̃N���X�ŊǗ�����
			bool LinkNewStr;								//�ŐV�f�[�^����ɍŏ㕔�ɕ\������悤�A�\���ʒu���Œ肷�邩�ǂ����H

		// Inquiry �n���̐錾
		private:
			// Inquiry�p�̌^�ϊ�
			typedef hmLib::inquiries::range_inquiry<iterator> iologbuf_inquiry;
			typedef hmLib::inquiries::inquiry<const system_clock_iologtype&> iolog_inquiry;
		public:
			//ioLog�o�b�t�@�ւ̃A�N�Z�X�p
			iologbuf_inquiry inquiry_LogBuf;
			iolog_inquiry inquiry_InStr;
			iolog_inquiry inquiry_OutStr;
			// contact �n��
			void contact_InPaintFlag(hmLib::inquiries::inquiry<bool> &inq_){
				hmLib::inquiries::connect(inq_, InPaintFlag);
			}
			void contact_OutPaintFlag(hmLib::inquiries::inquiry<bool> &inq_){
				hmLib::inquiries::connect(inq_, OutPaintFlag);
			}
		private:
			hmLib::signals::unique_connections connections;
		public:
			// slot �p�̊֐�
			void setInPaintMode(bool InPaintFlag_){
				InPaintFlag = InPaintFlag_;
			}
			void setOutPaintMode(bool OutPaintFlag_){
				OutPaintFlag = OutPaintFlag_;
			}
			// slot �ݒ�֐�
			void slot_setInPaintMode(boost::signals2::signal<void(bool)> &sig_){
				connections(sig_.connect([&](bool v1)->void{this->setInPaintMode(v1);}));
			}
			void slot_setOutPaintMode(boost::signals2::signal<void(bool)> &sig_){
				connections(sig_.connect([&](bool v1)->void{this->setOutPaintMode(v1);}));
			}
		protected:
			//virtual int redraw(dxO& dxf);
			virtual int redraw(dxO& dxo){
			
				try{
					//�ŐVPacket���擾
					const std::string* pInStr=nullptr;
					if(inquiry_InStr.is_connect()){
						if(inquiry_InStr().Str.size())pInStr=&(inquiry_InStr().Str);
					}
					const std::string* pOutStr=nullptr;
					if(inquiry_OutStr.is_connect()){
						if(inquiry_OutStr().Str.size())pOutStr=&(inquiry_OutStr().Str);
					}

					//�p�P�b�g�`��̐ݒ�
					auto Begin=inquiry_LogBuf.begin();
					auto End=inquiry_LogBuf.end();
					dxFnIOLog2<iterator> FnIOLog(Begin,End,pInStr,pOutStr,InPaintFlag,OutPaintFlag);
					IOLogFrame.set(FnIOLog,size,Clr);
					IOLogFrame.resetMoveArea(ALI::all);

					//Link�t���O�������Ă���ꍇ�A�`��ʒu���C��
					if(LinkNewStr){
						IOLogFrame.viewpos.y=-45*(End-Begin);
						if(pOutStr!=nullptr)IOLogFrame.viewpos.y-=45;
						if(pInStr!=nullptr)IOLogFrame.viewpos.y-=45;
					}
					Pint MPos=dxo.GtoD(dx::getMosPos());

					if(IOLogFrame.pIn(Pint(0,0),MPos)){
						IOLogFrame.viewpos.y-=45*dx::getMosH()*(dx::getKey(KEY::SHIFT)>0?3:1);
					}

					//�`��
					dxo.draw(Pint(0,0),IOLogFrame);
				}catch(const hmLib::inquiries::unconnected_exception& Excp){
					// Inquire �ł̃G���[
					dxo.draw(Pint(0,0), dxoStr("Inquiry:: �f�[�^�o�b�t�@�擾�Ɏ��s"));
					dxo.draw(Pint(0,30), dxoStr(Excp.what()));
					return 1;
				}catch(const hmLib::exceptions::exception& Excp){
					// ���̑��ł̃G���[
					dxo.draw(Pint(0,0), dxoStr("�G���[����"));
					dxo.draw(Pint(0,30), dxoStr(Excp.what()));
					return 1;
				}
			
				return 0;
			}

		public:

			dxosIO2(Pint size_, const dxRGB& Clr_)
				:hmoBox(size_)
				,InPaintFlag(true)
				,OutPaintFlag(true){
				Clr=Clr_;
				dxHLS tmp(Clr);

				tmp.sat/=2;
				SubClr=tmp;

				IOLogFrame.resetMoveArea(ALI::all);
				IOLogFrame.setMoveArea(ALI::bottom|ALI::left,Pint(0,0));
				IOLogFrame.viewpos.y=-size_.y;
			}
		};		

		//! �R���g���[���n�̃N���X
		// Start �̃X�C�b�`
		class dxosStartControl:public dxReObject,public hmoBox{
		private:
			dxRGB Clr;
			bool StartFlag;
			std::string ButtName;

		public:
			// signal �n��̒�`
			boost::signals2::signal<void(bool)> signal_require_setStartFlag;
			// inquire �n��̒�`
			hmLib::inquiries::inquiry<bool> StartFlagInq;

			// constructor
			dxosStartControl(Pint Size_, const dxColor& Clr_, const std::string& ButtName_)
				:hmoBox(Size_)
				,Clr(Clr_)
				,ButtName(ButtName_)
				,StartFlag(false) 
			{}

		protected:
			virtual int redraw(dxO& dxo){
				try{
					// �f�[�^�X�V
					StartFlag = StartFlagInq(); // debug

					// �{�^��
					if( dxo.draw(Pint(0,0),dxoButIO(size, ButtName, Clr, StartFlag)) == 1){
						StartFlag = !StartFlag;
						signal_require_setStartFlag(StartFlag);
					}

				}catch(const hmLib::inquiries::unconnected_exception&){				
					// Inquire �ɃA�N�Z�X�ł��Ȃ������I�I�@�ł��A����Ȃ̃��C���֐��ɂ͊֌W�Ȃ�������ݏ���
					dxo.draw(Pint(0, 0), dxoStr("Inquiry::" + ButtName + "�擾���s") );
				}catch(const hmLib::exceptions::exception& Excp){
					dxo.draw(Pint(0, 0), dxoStr("Error::" + ButtName + Excp.what()) );
				}
				return 0;
			}		
		};

		// Buf����
		class dxosBufControl:public dxReObject,public hmoBox{
		private:
			dxRGB Clr;
			unsigned int BufSize;
			std::string ButtName;
		public:
			// signal �n��̒�`
			boost::signals2::signal<void(void)> signal_require_clearBuf;
			// inquire �n��̒�`
			hmLib::inquiries::inquiry<unsigned int> BufSizeInq;
		
		public:
			dxosBufControl(Pint Size_,const dxColor& Clr_, const std::string& ButtName_)
				:hmoBox(Size_)
				,Clr(Clr_)
				,ButtName(ButtName_)
				,BufSize(0) 
			{}
		protected:
			virtual int redraw(dxO& dxo){
				try{
					// �f�[�^�X�V
					BufSize = BufSizeInq(); //debug

					// �{�^��
					dxRGB Clr_ = Clr;
					if((int)BufSize > 0){
						Clr_ = CLR::DarkDullRed;
					}
					if( dxo.draw(Pint(0,0),dxoBut(size, (ButtName + hmstd::fStr("( %d )", (int)BufSize)), Clr_)) == 1){
						signal_require_clearBuf();
					}
				}catch(const hmLib::inquiries::unconnected_exception&){				
					// Inquire �ɃA�N�Z�X�ł��Ȃ������I�I�@�ł��A����Ȃ̃��C���֐��ɂ͊֌W�Ȃ�������ݏ���
					dxo.draw(Pint(0, 0), dxoStr("Inquiry::" + ButtName +"�擾���s") );
				}catch(const hmLib::exceptions::exception& Excp){
					dxo.draw(Pint(0, 0), dxoStr("Error::" + ButtName + Excp.what()) );
				}

				return 0;
			}
		};

		// TimeOut����
		class dxosTimeOutControl:public dxReObject,public hmoBox{
		private:
			dxRGB Clr;
			std::string ButtName;

			clock::duration TimeOut;
			//bool IsFullDuplex;

		public:
			// signal �n��̒�`
			boost::signals2::signal<void(clock::duration)> signal_require_setTimeOut;
		
			// inquire �n��̒�`
			hmLib::inquiries::inquiry<clock::duration> TimeOutInq;
			//hmLib::inquiries::inquiry<bool> IsFullDuplexInq;
		
		public:
			dxosTimeOutControl(Pint Size_, const dxColor& Clr_, const std::string& ButtName_)
				:hmoBox(Size_)
				,Clr(Clr_)
				,ButtName(ButtName_)
				//,IsFullDuplex(true)
				,TimeOut( std::chrono::milliseconds(250) )
			{}

		protected:
			virtual int redraw(dxO& dxo){
				try{
					// �f�[�^�X�V
					TimeOut = TimeOutInq(); // debug
				
					// �����₷��INT�ɂ��Ă���
					int Interval_ms = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(TimeOut).count());
				
					// Interval �ݒ�
					int ans=dxo.draw(Pint(0,0), dxoButLRIO(size,ButtName + hmstd::fStr(" %d(ms)",Interval_ms), Clr, true));
				
					if(ans==1){
						Interval_ms*=2;
						if(Interval_ms>4000)Interval_ms = 250;
					}
					else if(ans==10){
						Interval_ms/=2;
						if(Interval_ms<250)Interval_ms=4000;
					}
				

					// �f�[�^�ϊ�
					TimeOut = std::chrono::milliseconds(Interval_ms);

					// signal ���M
					signal_require_setTimeOut(TimeOut);

				}catch(const hmLib::inquiries::unconnected_exception& ){				
					// Inquire �ɃA�N�Z�X�ł��Ȃ������I�I�@�ł��A����Ȃ̃��C���֐��ɂ͊֌W�Ȃ�������ݏ���
					dxo.draw(Pint(0, 0), dxoStr("Inquiry::" + ButtName + "�擾���s") );
				}catch(const hmLib::exceptions::exception& Excp){
					dxo.draw(Pint(0, 0), dxoStr("Error::" + ButtName + Excp.what()) );
				}

				return 0;
			}
		};

		// Sync ����
		//  ����d�ʐM���̂�
		class dxosSyncControl:public dxReObject,public hmoBox{
		private:
			bool SyncFlag;

			clock::duration Interval;
			bool IsFullDuplex;
			dxRGB Clr;
		
		public:
			// �R���X�g���N�^
			dxosSyncControl(Pint Size_, const dxRGB& Clr_)
				:hmoBox(Size_)
				,SyncFlag(false)
				,IsFullDuplex(true)
				,Interval(std::chrono::milliseconds(250))
				,Clr(Clr_)
				{}

		public:
			// signal �n��̒�`
			//boost::signals2::signal<void(bool)> signal_require_setSyncFlag;
			// interval �̐ݒ�
			boost::signals2::signal<void(clock::duration)> signal_require_setTimeInterval;


		public:
			// inquire �n��̒�`
			//hmLib::inquiries::inquiry<bool> SyncFlagInq;
			hmLib::inquiries::inquiry<bool> IsFullDuplexInq;
			hmLib::inquiries::inquiry<clock::duration> TimeIntervalInq;
		
		protected:
			// �`��
			virtual int redraw(dxO& dxo){
				// �X�V �ƂƂ��� �`�F�b�N�{�b�N�X�̕`��
				try{
					IsFullDuplex = IsFullDuplexInq(); //debug

					if(dxo.draw(Pint(0,0),dxoCheck(size,"Sync",size.y,Clr,SyncFlag))==1){
						if(!IsFullDuplex){
							SyncFlag = !SyncFlag;
							if(SyncFlag){
								// Sync �v���������Interval��0�ɂ���
								Interval = TimeIntervalInq();//�@�ȑO�̒l��ێ�
								signal_require_setTimeInterval(std::chrono::milliseconds(0));
							}else{
								//Sync�����v��������΁A���ɖ߂�
								signal_require_setTimeInterval(Interval);
							}
						}
					}
				}catch(const hmLib::inquiries::unconnected_exception& ){
					// Inquire �ɃA�N�Z�X�ł��Ȃ������I�I�@�ł��A����Ȃ̃��C���֐��ɂ͊֌W�Ȃ�������ݏ���
					dxo.draw(Pint(0, 0), dxoStr("Inquiry:: Interval �擾���s") );
				}catch(const hmLib::exceptions::exception& Excp){
					std::string str1 = "Error:: Interval ";
					dxo.draw(Pint(0, 0), dxoStr(str1 + Excp.what()) );
				}

				return 0;
			}
		};

		// TimeInterval����
		class dxosTimeIntervalControl:public dxReObject,public hmoBox{
		private:
			dxRGB Clr;
			std::string ButtName;

			clock::duration TimeInterval;
			//bool IsFullDuplex;
			//int TimeOut;//(ms)
			//bool TimeOutFlag;

		public:
			// signal �n��̒�`
			boost::signals2::signal<void(clock::duration)> signal_require_setTimeInterval;
		
			// inquire �n��̒�`
			hmLib::inquiries::inquiry<clock::duration> TimeIntervalInq;
			//hmLib::inquiries::inquiry<bool> IsFullDuplexInq;
		
		public:
			dxosTimeIntervalControl(Pint Size_, const dxColor& Clr_, const std::string& ButtName_ = "SendInterval")
				:hmoBox(Size_)
				,Clr(Clr_)
				,ButtName(ButtName_)
				//,IsFullDuplex(true)
				,TimeInterval( std::chrono::milliseconds(250) )
			{}

		protected:
			virtual int redraw(dxO& dxo){
				try{
					// �f�[�^�X�V
					TimeInterval = TimeIntervalInq(); // debug
				
					// �����₷��INT�ɂ��Ă���
					int Interval_ms = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(TimeInterval).count());
				
					// Interval �ݒ�
					int ans=dxo.draw(Pint(0,0), dxoButLRIO(size,ButtName + hmstd::fStr(" %d(ms)",Interval_ms), Clr, true));
				

					if(ans==1){
						Interval_ms*=2;
						if(Interval_ms>4000)Interval_ms = 250;
					}
					else if(ans==10){
						Interval_ms/=2;
						if(Interval_ms<250)Interval_ms=4000;
					}
				

					// �f�[�^�ϊ�
					TimeInterval = std::chrono::milliseconds(Interval_ms);

					// signal ���M
					signal_require_setTimeInterval(TimeInterval);

				}catch(const hmLib::inquiries::unconnected_exception& ){				
					// Inquire �ɃA�N�Z�X�ł��Ȃ������I�I�@�ł��A����Ȃ̃��C���֐��ɂ͊֌W�Ȃ�������ݏ���
					dxo.draw(Pint(0, 0), dxoStr("Inquiry::" + ButtName + "�擾���s") );
				}catch(const hmLib::exceptions::exception& Excp){
					dxo.draw(Pint(0, 0), dxoStr("Error::" + ButtName + Excp.what()) );
				}

				return 0;
			}
		};

		// �S��d�A����d�@�I��
		class dxosSelectDuplex:public dxReObject,public hmoBox{
		private:
			bool IsFullDuplexFlag;
			dxRGB Clr;
			Pint Size;

		public:
			// �R���X�g���N�^
			dxosSelectDuplex(Pint Size_, const dxRGB& Clr_)
				:hmoBox(Size_),IsFullDuplexFlag(true), Clr(Clr_), Size(Size_){}

		public:
			// signal �n��̒�`
			boost::signals2::signal<void(bool)> signal_require_isFullDuplex;
		public:
			// inquire �n��̒�`
			hmLib::inquiries::inquiry<bool> IsFullDuplexFlagInq;

		protected:
			// �`��
			virtual int redraw(dxO& dxo){
				try{
					// �X�V �ƂƂ��� �`�F�b�N�{�b�N�X�̕`��
					try{
						IsFullDuplexFlag = IsFullDuplexFlagInq();//debug
						if(dxo.draw(Pint(0,0),dxoCheck(Pint(Size.x / 2, Size.y),"�S��d�ʐM",Size.y,Clr,IsFullDuplexFlag))==1){
							IsFullDuplexFlag = !IsFullDuplexFlag;
							signal_require_isFullDuplex(IsFullDuplexFlag);
						}
						if(dxo.draw(Pint(Size.x /2 ,0),dxoCheck(Pint(Size.x / 2, Size.y),"����d�ʐM",Size.y,Clr,!IsFullDuplexFlag))==1){
							IsFullDuplexFlag = !IsFullDuplexFlag;
							signal_require_isFullDuplex(IsFullDuplexFlag);
						}
					}catch(const hmLib::inquiries::unconnected_exception& ){
						// Inquire �ɃA�N�Z�X�ł��Ȃ������I�I�@�ł��A����Ȃ̃��C���֐��ɂ͊֌W�Ȃ�������ݏ���
						dxo.draw(Pint(0, 0), dxoStr("Inquiry:: IsFullDuplexFlag�擾���s") );
					}
				}catch(const hmLib::exceptions::exception& ){
					// �z��O�̃G���[
					dxo.draw(Pint(0, 0), dxoStr("�z��O�̃G���[�@"));
				}
				return 0;
			}
		};
	}
}

#endif









