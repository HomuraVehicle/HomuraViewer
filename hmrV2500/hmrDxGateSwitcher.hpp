#ifndef HMR_DXGATE_INC
#define HMR_DXGATE_INC 101

#include <iostream>
#include <string>
#include "hmLibVer.hpp"
#include <hmLib_v2/hmBasic.hpp>
#include <hmLib_v2/dxObject.hpp>
#include <hmLib_v2/dxArea.hpp>
#include <hmLib_v3_06/signals.hpp>
#include <boost/signals2.hpp>
//#include "hmrBaseDX.hpp"

//#define DEBUG_MODE 1

/* ---- hmrDxGateSwitcher-----

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

/*
namespace hmr{
	// RS�ɂ��Ă̐ڑ�����UI
	class dxsoPortSetter:public dxReObject,public hmoBox{
	private:
		int PortCh;
		int BaudRate;
		dxRGB Clr,SubClr;
		bool PortFileLinkFlag;
		bool IsPortOpen;
		bool IsOtherPortOpen;


	// signal�n�̎���
	public:
		// Port �V�O�i��
		boost::signals2::signal<void(void)> signal_require_open;
		boost::signals2::signal<void(int)> signal_require_setBaudrate;
		boost::signals2::signal<void(int)> signal_require_setPortCh;
		
			


		boost::signals2::signal<void(int, int)> signal_require_openRS;
		boost::signals2::signal<void(void)> signal_require_closeRS;
		// File�@�V�O�i��
		boost::signals2::signal<void(bool)> signal_require_openFile; // �������@����  
		boost::signals2::signal<void(bool)> signal_require_closeFile; // �������@����  

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
	
	protected:
		//virtual int fndraw(dxO& dxf);
		virtual int fndraw(dxO& dxo){
			int ans;
			//�ڑ��{�^��
			if(dxo.draw(Pint(10,10),dxoButIO(Pint(120,50),"RS�ڑ�",Clr, IsRSOpen), !IsFOMAOpen){
				if(!IsRSOpen){
					// Open signal 
					signal_require_openRS(BaudRate, PortCh);
					signal_require_openFile(PortFileLinkFlag);
				}else{
					// Close signal
					signal_require_closeRS();
					if(PortFileLinkFlag)signal_require_closeFile();
				}
			}

			//�`�����l���I��
			ans=dxo.draw(Pint(20,70),dxoButLR(Pint(120,20),hmstd::fStr("%2dch",PortCh),hm::sel((!IsRSOpen && !IsFOMAOpen),Clr,SubClr)), (!IsRSOpen && !IsFOMAOpen) );
			if( !IsRSOpen && !IsFOMAOpen ){
				if(ans== 1)PortCh=(PortCh+1)%13;
				if(ans==10)PortCh=(PortCh+12)%13;
			}

			//�{�[���[�g
			ans=dxo.draw(Pint(20,100),dxoButLR(Pint(120,20),hmstd::fStr("%5dbps",BaudRate),hm::selhm::sel((!IsRSOpen && !IsFOMAOpen),Clr,SubClr)), (!IsRSOpen && !IsFOMAOpen) );
			if( !IsRSOpen && !IsFOMAOpen ){
				if(ans==1)BaudRate*=2;
				if(ans==10)BaudRate/=2;		
				if(BaudRate>38400)BaudRate=2400;
				if(BaudRate<2400)BaudRate=38400;
			}

			//�ۑ��̃����N�t���O
			if(dxo.draw(Pint(20,130),dxoCheck(Pint(200,20),"�ڑ��ƕۑ��������N����",20,Clr,PortFileLinkFlag))==1){
				PortFileLinkFlag=!PortFileLinkFlag;
			}
			return 0;
		}

	public:
		dxfRSIOCtr(const dxRGB& _Clr)
			:IsRSOpen(false), IsFOMAOpen(false), PortFileLinkFlag(true){
			
			PortCh=0;
			BaudRate=9600;
			Clr=_Clr;
			dxHLS tmp(Clr);
			tmp.sat/=2;
			SubClr=tmp;
		}
	};





}
*/



namespace hmr{
	// �`��N���X static
	class dxosGateSwitcher:public dxReObject,public hmoBox{
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
	
	protected:
		//virtual int fndraw(dxO& dxf);
		virtual int redraw(dxO& dxo){
			int ans;

			// RS�n��ɂ��Ă̐���nUI
			//�@�ڑ��{�^��
			if(dxo.draw(Pint(10,10),dxoButIO(Pint(220,40),"RS�ڑ�",Clr, IsRSOpen), !IsFOMAOpen) == 1){

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
			ans=dxo.draw(Pint(30,60),dxoButLR(Pint(200,20),hmstd::fStr("%2dch",RS_PortCh),hm::sel((!IsRSOpen && !IsFOMAOpen),Clr,SubClr)), (!IsRSOpen && !IsFOMAOpen) );
			if( !IsRSOpen && !IsFOMAOpen ){
				if(ans== 1)RS_PortCh=(RS_PortCh+1)%13;
				if(ans==10)RS_PortCh=(RS_PortCh+12)%13;
			}

			//�@�{�[���[�g
			ans=dxo.draw(Pint(30,90),dxoButLR(Pint(200,20),hmstd::fStr("%5dbps",RS_BaudRate),hm::sel((!IsRSOpen && !IsFOMAOpen),Clr,SubClr)), (!IsRSOpen && !IsFOMAOpen) );
			if( !IsRSOpen && !IsFOMAOpen ){
				if(ans==1)RS_BaudRate*=2;
				if(ans==10)RS_BaudRate/=2;		
				if(RS_BaudRate>38400)RS_BaudRate=2400;
				if(RS_BaudRate<2400)RS_BaudRate=38400;
			}

			// FOMA�n�ɂ��Ă̐���nUI
			//�@�ڑ��{�^��
			if(dxo.draw(Pint(10,130),dxoButIO(Pint(220,40),"FOMA�ڑ�",Clr, IsFOMAOpen),!IsRSOpen) == 1){
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
			ans=dxo.draw(Pint(30,180),dxoButLR(Pint(200,20),hmstd::fStr("%2dch",FOMA_PortCh),hm::sel( (!IsRSOpen && !IsFOMAOpen),Clr,SubClr)),  (!IsRSOpen && !IsFOMAOpen));
			if(  !IsRSOpen && !IsFOMAOpen ){
				if(ans== 1)FOMA_PortCh=(FOMA_PortCh+1)%13;
				if(ans==10)FOMA_PortCh=(FOMA_PortCh+12)%13;
			}

			//�{�[���[�g
			ans=dxo.draw(Pint(30,210),dxoButLR(Pint(200,20),hmstd::fStr("%5dbps",FOMA_BaudRate),hm::sel( (!IsRSOpen && !IsFOMAOpen) ,Clr,SubClr)), (!IsRSOpen && !IsFOMAOpen));
			if( (!IsRSOpen && !IsFOMAOpen) ){
				if(ans==1)FOMA_BaudRate*=2;
				if(ans==10)FOMA_BaudRate/=2;		
				if(FOMA_BaudRate>38400)FOMA_BaudRate=2400;
				if(FOMA_BaudRate<2400)FOMA_BaudRate=38400;
			}

			//�ۑ��̃����N�t���O
			if(dxo.draw(Pint(10,250),dxoCheck(Pint(200,20),"�ڑ��ƕۑ��������N����",20,Clr,PortFileLinkFlag))==1){
				if( !IsRSOpen && !IsFOMAOpen ){
					PortFileLinkFlag=!PortFileLinkFlag;
				}
			}
			return 0;

		}

	public:
		dxosGateSwitcher(const dxRGB& _Clr)
			:IsRSOpen(false), IsFOMAOpen(false), PortFileLinkFlag(true),hmoBox(Pint(240,290)){
			
			RS_PortCh=0;
			FOMA_PortCh = 0;

			RS_BaudRate=38400;
			FOMA_BaudRate = 38400;

			Clr=_Clr;
			dxHLS tmp(Clr);
			tmp.sat/=2;
			SubClr=tmp;
		}
	};		
}

/*


	public:
		// RS�ɂ��Ă̐ڑ�����UI
		class dxfRSIOCtr:public dxFn{
		private:
			int PortCh;
			int BaudRate;
			dxRGB Clr,SubClr;
			bool IsRSOpen;
			bool IsFOMAOpen;
			bool PortFileLinkFlag;

		// signal�n�̎���
		public:
			// Port �V�O�i��
			boost::signals2::signal<void(int, int)> signal_require_openRS;
			boost::signals2::signal<void(void)> signal_require_closeRS;
			// File�@�V�O�i��
			boost::signals2::signal<void(bool)> signal_require_openFile; // �������@����  
			boost::signals2::signal<void(bool)> signal_require_closeFile; // �������@����  

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
	
		protected:
			//virtual int fndraw(dxO& dxf);
			virtual int fndraw(dxO& dxo){
				int ans;
				//�ڑ��{�^��
				if(dxo.draw(Pint(10,10),dxoButIO(Pint(120,50),"RS�ڑ�",Clr, IsRSOpen), !IsFOMAOpen){
					if(!IsRSOpen){
						// Open signal 
						signal_require_openRS(BaudRate, PortCh);
						signal_require_openFile(PortFileLinkFlag);
					}else{
						// Close signal
						signal_require_closeRS();
						if(PortFileLinkFlag)signal_require_closeFile();
					}
				}

				//�`�����l���I��
				ans=dxo.draw(Pint(20,70),dxoButLR(Pint(120,20),hmstd::fStr("%2dch",PortCh),hm::sel((!IsRSOpen && !IsFOMAOpen),Clr,SubClr)), (!IsRSOpen && !IsFOMAOpen) );
				if( !IsRSOpen && !IsFOMAOpen ){
					if(ans== 1)PortCh=(PortCh+1)%13;
					if(ans==10)PortCh=(PortCh+12)%13;
				}

				//�{�[���[�g
				ans=dxo.draw(Pint(20,100),dxoButLR(Pint(120,20),hmstd::fStr("%5dbps",BaudRate),hm::selhm::sel((!IsRSOpen && !IsFOMAOpen),Clr,SubClr)), (!IsRSOpen && !IsFOMAOpen) );
				if( !IsRSOpen && !IsFOMAOpen ){
					if(ans==1)BaudRate*=2;
					if(ans==10)BaudRate/=2;		
					if(BaudRate>38400)BaudRate=2400;
					if(BaudRate<2400)BaudRate=38400;
				}

				//�ۑ��̃����N�t���O
				if(dxo.draw(Pint(20,130),dxoCheck(Pint(200,20),"�ڑ��ƕۑ��������N����",20,Clr,PortFileLinkFlag))==1){
					PortFileLinkFlag=!PortFileLinkFlag;
				}
				return 0;
			}

		public:
			dxfRSIOCtr(const dxRGB& _Clr)
				:IsRSOpen(false), IsFOMAOpen(false), PortFileLinkFlag(true){
			
				PortCh=0;
				BaudRate=9600;
				Clr=_Clr;
				dxHLS tmp(Clr);
				tmp.sat/=2;
				SubClr=tmp;
			}
		};

		// FOMA�ɂ��Ă̐ڑ�����UI
		class dxfMPIOCtr:public dxFn{
		private:
			int PortCh;
			int BaudRate;
			dxRGB Clr,SubClr;
			bool IsRSOpen;
			bool IsFOMAOpen;
			bool PortFileLinkFlag;

		// signal�n�̎���
		private:
			// Port �V�O�i��
			boost::signals2::signal<void(int, int)> signal_require_openFOMA;
			boost::signals2::signal<void(void)> signal_require_closeFOMA;
			// File�@�V�O�i��
			boost::signals2::signal<void(bool)> signal_require_openFile; // �������@����  
			boost::signals2::signal<void(bool)> signal_require_closeFile; // �������@����  

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

		protected:
			virtual int fndraw(dxO& dxo){

				int ans;
				//�ڑ��{�^��
				if(dxo.draw(Pint(10,180),dxoButIO(Pint(120,50),"MP�ڑ�",Clr, IsFOMAOpen),!IsRSOpen){
					if(!IsFOMAOpen){
						// Open signal 
						signal_require_openFOMA(BaudRate, PortCh);
						signal_require_openFile(PortFileLinkFlag);
					}else{
						// Close siganal
						signal_require_closeFOMA();
						if(PortFileLinkFlag)signal_closeFile();
					}
				}


				//�`�����l���I��
				ans=dxo.draw(Pint(20,240),dxoButLR(Pint(120,20),hmstd::fStr("%2dch",PortCh),hm::sel( (!IsRSOpen && !IsFOMAOpen),Clr,SubClr)),  (!IsRSOpen && !IsFOMAOpen));
				if(  !IsRSOpen && !IsFOMAOpen ){
					if(ans== 1)PortCh=(PortCh+1)%13;
					if(ans==10)PortCh=(PortCh+12)%13;
				}

				//�{�[���[�g
				ans=dxo.draw(Pint(20,270),dxoButLR(Pint(120,20),hmstd::fStr("%5dbps",BaudRate),hm::sel( (!IsRSOpen && !IsFOMAOpen) ,Clr,SubClr)), (!IsRSOpen && !IsFOMAOpen));
				if( (!IsRSOpen && !IsFOMAOpen) ){
					if(ans==1)BaudRate*=2;
					if(ans==10)BaudRate/=2;		
					if(BaudRate>38400)BaudRate=2400;
					if(BaudRate<2400)BaudRate=38400;
				}

				//�ۑ��̃����N�t���O
				if(dxo.draw(Pint(100,30),dxoCheck(Pint(20,300),"�ڑ��ƕۑ��������N����",20,Clr,PortFileLinkFlag))==1){
					PortFileLinkFlag=!PortFileLinkFlag;
				}
				return 0;
			}

		public:
			dxfMPIOCtr(const dxRGB& _Clr)
				: IsRSOpen(false), IsFOMAOpen(false), PortFileLinkFlag(true)
			{
				PortCh=0;
				BaudRate=9600;
				Clr=_Clr;
				dxHLS tmp(Clr);
				tmp.sat/=2;
				SubClr=tmp;
			}
		};

	public:
		dxfRSIOCtr cDxfRS;
		dxfMPIOCtr cDxfFOMA;

	public: 
		// �R���X�g���N�^
		dxfGateSwitcher(const dxRGB& _Clr)
			:cDxfRS(_Clr), cDxfFOMA(_Ctr)
		{
			// �������Ȃ�
		}

		// �C���^�[�t�F�[�X
		virtual int fndraw(dxO& dxo){
			cDxfRS.fndraw(dxo);
			cDxfFOMA.fndraw(dxo);
			return 0;
		}
	};
}

*/

#endif
