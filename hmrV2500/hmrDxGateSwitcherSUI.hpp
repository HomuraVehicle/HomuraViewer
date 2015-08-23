#ifndef HMR_DXGATE_SUI_INC
#define HMR_DXGATE_SUI_INC 100

#include <iostream>
#include <string>
#include "hmLibVer.hpp"
#include <hmLib_v2/hmBasic.hpp>
#include <hmLib_v2/dxObject.hpp>
#include <hmLib_v2/dxArea.hpp>
#include <hmLib_v3_05/signals.hpp>
#include <boost/signals2.hpp>
#include "hmrDxBUI.hpp"
//#include "hmrBaseDX.hpp"

//#define DEBUG_MODE 1

/* ---- hmrDxGateSwitcherSUI-----

hmrDxGateSwitcherSUI.hpp :v1_00/130615 amby
	Gate SwitcherSUI�̐���J�n


�g�����Ƃ��Ăǂ������\�������邩

	�P�A��������̂�1�̏ꏊ�ł͂Ȃ���������Ȃ�
		���ׂĂ̕ϐ��𓯊����Ă�������
		�ł���Ȃ�A�`��N���X���ė��p�ł���`�ɂ�����

	�Q�A�|�[�g�������邩������Ȃ�
		���ʉ��ł���Ƃ���͋��ʉ����Ă��܂�����

	�R�AUI��ʂ����ɂ������邩������Ȃ�
		���ׂĂ̕ϐ��𓯊����Ă����K�v������
		�{�[���[�g�A�|�[�g�i���o�[�A���̑��ݒ�ϐ�

	�S�A�ꍇ�ɂ���āA�Q�[�g�����A���^�C���ɐ؂�ւ��������Ƃ�����
		�|�[�g�ݒ�͓Ɨ��ɂł����ق����悢�Ǝv����

�݌v
	�P�A

�@Gate Switcher 

hmrDxGateSwitcher.hpp :v1_01/130414 amby
	signal, slot �n�̖��O�𓝈�
	connection �̊ԈႢ���C��
	�`��̏ꏊ��ύX

hmrDxGateSwitcher.hpp :v1_00/130406 amby
	Gate switcher �֘A��UI��񋟂���
	Port open/close �ɂ��Ď���
	file open/close �ɂ��Ă͕ۗ����Ă���
*/

namespace hmr{
	// �`��N���X static
	class dxosGateSwitcherSUI:public dxosBUI{
	private:
		int RS_PortCh;
		int FOMA_PortCh;

		int RS_BaudRate;
		int FOMA_BaudRate;
		
		dxRGB Clr,SubClr;

		bool IsRSOpen;
		bool IsFOMAOpen;
		
		bool PortFileLinkFlag;

	// signal�n�̎���
	public:
		// Port �V�O�i��
		boost::signals2::signal<void(int, int)> signal_require_openRS;
		boost::signals2::signal<void(void)> signal_require_closeRS;

		boost::signals2::signal<void(int, int)> signal_require_openFOMA;
		boost::signals2::signal<void(void)> signal_require_closeFOMA;

		// File�@�V�O�i��
		boost::signals2::signal<void(void)> signal_require_openFile;  
		boost::signals2::signal<void(void)> signal_require_closeFile; 

		// inquire�n��̒�`
		hmLib::inquiries::inquiry<bool> isOpenInq;
		hmLib::inquiries::inquiry<bool> canGetInq;
		hmLib::inquiries::inquiry<bool> canSendInq;

	// slot �n�̎���
	private:
		// Connection
		hmLib::signals::unique_connections connections;
		
		// ���ۂɌĂ΂�� slot �p�̊֐�
		void sync_isRSOpen(bool flag_){
			IsRSOpen = flag_;
		}
		void sync_isFOMAOpen(bool flag_){
			IsFOMAOpen = flag_;
		}
		

	public:
		// slot �Ɓ@signal�@���Ȃ���֐�
			// IsRSOpen �̏�Ԃ�������Ă���
		void slot_sync_isRSOpen(boost::signals2::signal<void(bool)> &sig_){
			connections(sig_.connect([&](bool flag_)->void{this->sync_isRSOpen(flag_);}));
		}
			// IsFOMAOpen �̏�Ԃ�������Ă���
		void slot_sync_isFOMAOpen(boost::signals2::signal<void(bool)> &sig_){
			connections(sig_.connect([&](bool flag_)->void{this->sync_isFOMAOpen(flag_);}));
		}
	
	public:
		//virtual int fndraw(dxO& dxf);
		int normal_draw(dxO& dxo)override{
			int ans;

			// �������g�𖼏���ď�ԕ\��
			/*
			dxo.draw(Pint(5,5),dxoStrP(Pint(70,20),"GateSwitcher",getClr(normal,strobj)));
			if(IsRSOpen || IsFOMAOpen ){
				dxo.draw(Pint(80,5),dxoStrP(Pint(140,20),"Opened",getClr(normal,strobj)));
			}else{
				dxo.draw(Pint(80,5),dxoStrP(Pint(140,20),"Closed",getClr(invalid,strobj)));
			}
			*/

			// can get �n��̕`��
			 // is Open
			try{
				if(isOpenInq()){
					dxo.draw(Pint(5,5),dxoStrP(Pint(70,20),"Opened",getClr(normal,strobj)));
				}else{
					dxo.draw(Pint(5,5),dxoStrP(Pint(70,20),"Closed",getClr(invalid,strobj)));
				}
			}catch(const hmLib::inquiries::unconnected_exception& ){				
				// Inquire �ɃA�N�Z�X�ł��Ȃ������I�I�@�ł��A����Ȃ̃��C���֐��ɂ͊֌W�Ȃ�������ݏ���
				dxo.draw(Pint(5, 5), dxoStr("Inquiry::isOpen �擾���s") );
			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(5, 5), dxoStr( Excp.what()) );
			}
			
			 // can send
			try{
				if(canSendInq()){
					dxo.draw(Pint(80,5),dxoStrP(Pint(70,20),"���M��",getClr(normal,strobj)));
				}else{
					dxo.draw(Pint(80,5),dxoStrP(Pint(70,20),"���M�s��",getClr(invalid,strobj)));
				}
			}catch(const hmLib::inquiries::unconnected_exception& ){				
				dxo.draw(Pint(80, 5), dxoStr("Inquiry::canSend �擾���s") );
			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(80, 5), dxoStr( Excp.what()) );
			}
			
			 // can recv
			try{
				if(canGetInq()){
					dxo.draw(Pint(155,5),dxoStrP(Pint(70,20),"��M��",getClr(normal,strobj)));
				}else{
					dxo.draw(Pint(155,5),dxoStrP(Pint(70,20),"��M�s��",getClr(invalid,strobj)));
				}
			}catch(const hmLib::inquiries::unconnected_exception& ){				
				dxo.draw(Pint(155, 5), dxoStr("Inquiry::canGet �擾���s") );
			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(155, 5), dxoStr( Excp.what()) );
			}
			


			// RS�n��ɂ��Ă̐���nUI
			//�@�ڑ��{�^��
			if(dxo.draw(Pint(5,30),dxoButIO(Pint(70, 20),"RS�ڑ�",getClr(normal,butobj), IsRSOpen), !IsFOMAOpen) == 1){
				if(!IsRSOpen){
					// Open signal 
					signal_require_openRS(RS_BaudRate, RS_PortCh);
					if(PortFileLinkFlag) signal_require_openFile();
				}else{
					// Close signal
					signal_require_closeRS();
					if(PortFileLinkFlag) signal_require_closeFile();
				}
			}

			//�@�`�����l���I��
			ans=dxo.draw(Pint(80,30),dxoButLR(Pint(70, 20),hmstd::fStr("%2dch",RS_PortCh),hm::sel((!IsRSOpen && !IsFOMAOpen),getClr(normal,butobj),getClr(invalid,butobj))), (!IsRSOpen && !IsFOMAOpen) );
			if( !IsRSOpen && !IsFOMAOpen ){
				if(ans== 1)RS_PortCh=(RS_PortCh+1)%13;
				if(ans==10)RS_PortCh=(RS_PortCh+12)%13;
			}

			//�@�{�[���[�g
			ans=dxo.draw(Pint(155,30),dxoButLR(Pint(70, 20),hmstd::fStr("%5dbps",RS_BaudRate),hm::sel((!IsRSOpen && !IsFOMAOpen),getClr(normal,butobj),getClr(invalid,butobj))), (!IsRSOpen && !IsFOMAOpen) );
			if( !IsRSOpen && !IsFOMAOpen ){
				if(ans==1)RS_BaudRate*=2;
				if(ans==10)RS_BaudRate/=2;		
				if(RS_BaudRate>38400)RS_BaudRate=2400;
				if(RS_BaudRate<2400)RS_BaudRate=38400;
			}

			// FOMA�n�ɂ��Ă̐���nUI
			//�@�ڑ��{�^��
			if(dxo.draw(Pint(5,55),dxoButIO(Pint(70, 20),"FOMA�ڑ�", getClr(normal,butobj), IsFOMAOpen),!IsRSOpen) == 1){
				if(!IsFOMAOpen){
					// Open signal 
					signal_require_openFOMA(FOMA_BaudRate, FOMA_PortCh);
					if(PortFileLinkFlag) signal_require_openFile();
				}else{
					// Close siganal
					signal_require_closeFOMA();
					if(PortFileLinkFlag) signal_require_closeFile();
				}
			}

			//�`�����l���I��
			ans=dxo.draw(Pint(80,55),dxoButLR(Pint(70, 20),hmstd::fStr("%2dch",FOMA_PortCh),hm::sel( (!IsRSOpen && !IsFOMAOpen),getClr(normal,butobj),getClr(invalid,butobj))),  (!IsRSOpen && !IsFOMAOpen));
			if(  !IsRSOpen && !IsFOMAOpen ){
				if(ans== 1)FOMA_PortCh=(FOMA_PortCh+1)%13;
				if(ans==10)FOMA_PortCh=(FOMA_PortCh+12)%13;
			}

			//�{�[���[�g
			ans=dxo.draw(Pint(155,55),dxoButLR(Pint(70, 20),hmstd::fStr("%5dbps",FOMA_BaudRate),hm::sel( (!IsRSOpen && !IsFOMAOpen) ,getClr(normal,butobj),getClr(invalid,butobj))), (!IsRSOpen && !IsFOMAOpen));
			if( (!IsRSOpen && !IsFOMAOpen) ){
				if(ans==1)FOMA_BaudRate*=2;
				if(ans==10)FOMA_BaudRate/=2;		
				if(FOMA_BaudRate>38400)FOMA_BaudRate=2400;
				if(FOMA_BaudRate<2400)FOMA_BaudRate=38400;
			}

			//�ۑ��̃����N�t���O
			/*
			if(dxo.draw(Pint(10,250),dxoCheck(Pint(200,20),"�ڑ��ƕۑ��������N����",20,Clr,PortFileLinkFlag))==1){
				if( !IsRSOpen && !IsFOMAOpen ){
					PortFileLinkFlag=!PortFileLinkFlag;
				}
			}
			*/
			return 0;

		}

		int extend_draw(dxO& dxo)override{
			normal_draw(dxo);
			return 0;
		}

		status getStatus()const override{
			if(IsRSOpen || IsFOMAOpen){
				return normal;
			}else{
				return invalid;
			}
		}

	public:
		dxosGateSwitcherSUI()
			:dxosBUI("GateSwitcher", 80, 80), IsRSOpen(false), IsFOMAOpen(false), PortFileLinkFlag(true){
			
			RS_PortCh=0;
			FOMA_PortCh = 0;

			RS_BaudRate=38400;
			FOMA_BaudRate = 38400;

			//Clr=_Clr;
			//dxHLS tmp(Clr);
			//tmp.sat/=2;
			//SubClr=tmp;
		}
	};		
}

#
#endif
