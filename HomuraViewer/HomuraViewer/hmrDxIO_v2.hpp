#ifndef HMR_DXIO2_INC
#define HMR_DXIO2_INC 103

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
#include "dxSignalBut.hpp"
#include <HomuraViewer/Data.hpp>
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
	//  �R�}���h�ł����ޗp��
	class dxosIOSendBox2:public dxReObject,public hmoBox{
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
		dxosIOSendBox2(Pint Size_,const dxColor& Clr_)
			:hmoBox(Size_)
			,Clr(Clr_){
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
	//�ŐV�f�[�^�̕`��ɂ��Ή������o�[�W����
	template<typename iterator>
	class dxFnIOLog2:public dxFn{
		typedef std::pair<packet::no_type,datum::no_type> seltype;
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
		dxFnIOLog2(iterator Begin_,iterator End_,const std::string* pInStr_,const std::string* pOutStr_,bool InPaintFlag_,bool OutPaintFlag_,const dxRGB& InputClr_,const dxRGB& OutputClr_)
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
						dxo.draw(Pint(0,DrawPos),dxoStrP(Pint(120,45),hmr::time_to_hms(pitr->second.EndTime),CLR::LightGray));

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
							dxo.draw(Pint(0,DrawPos),dxoStrP(Pint(120,45),hmr::time_to_hms(pitr->second.EndTime),CLR::LightGray));

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
		dxosIOSendBox2 IOSendBox;
	public:
		boost::signals2::signal<void(std::string)>& signal_poutString;
	private:
		// Color �p�̕ϐ�
		dxRGB FrameClr;
		dxRGB Clr;
		dxRGB InClr;
		dxRGB OutClr;
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
				//�{�^���\��
				if(dxo.draw(Pint(0,0),dxoButIO(Pint(60,30),"Sync",Clr,LinkNewStr))==1){
					LinkNewStr=!LinkNewStr;
				}
				//�{�^���\��
				if(dxo.draw(Pint(60,0),dxoButIO(Pint(40,30),"IN",InClr,InPaintFlag))==1){
					InPaintFlag=!InPaintFlag;
				}
				//�{�^���\��
				if(dxo.draw(Pint(100,0),dxoButIO(Pint(40,30),"OUT",OutClr,OutPaintFlag))==1){
					OutPaintFlag=!OutPaintFlag;
				}
				dxo.draw(Pint(140,0),IOSendBox);


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
				dxFnIOLog2<iterator> FnIOLog(Begin,End,pInStr,pOutStr,InPaintFlag,OutPaintFlag,InClr,OutClr);
				IOLogFrame.set(FnIOLog,size-Pint(0,30),FrameClr);
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
				dxo.draw(Pint(0,30),IOLogFrame);
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
		dxosIO2(Pint size_, const dxRGB& FrameClr_,const dxRGB& Clr_,const dxRGB& InClr_,const dxRGB& OutClr_)
			:hmoBox(size_)
			,IOSendBox(Pint(580,30),Clr_)
			,signal_poutString(IOSendBox.signal_poutString)
			,InPaintFlag(true)
			,OutPaintFlag(true)
			,FrameClr(FrameClr_)
			,Clr(Clr_)
			,InClr(InClr_)
			,OutClr(OutClr_){

			IOLogFrame.resetMoveArea(ALI::all);
			IOLogFrame.setMoveArea(ALI::bottom|ALI::left,Pint(0,0));
			IOLogFrame.viewpos.y=-size_.y+30;
		}
	};
}
#
#endif
