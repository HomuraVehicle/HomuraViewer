

#ifndef HMR_GATEHAB_INC
#define HMR_GATEHAB_INC 104
#
/*===hmrGateHab===
Gate�̐؂�ւ��Ȃǂ𐧌䂷��gate�N���X

hmrGateSwitcher:v1_04/130414 amby
	signal, slot�@�̖��O
	connection�@����ɓ���

	signal ���Ăԏꏊ�ɖ�肪���������߉���
	open, close�ȂǁA���ۂɏ�Ԃ�ς���̊֐��̒��Ŏ����B

hmrGateSwitcher:v1_03/130406 amby
	�|�[�g�I�[�v���A�N���[�Y�̃X���b�g��ǉ�
	�|�[�g�I�[�v���̐��ۂ�Ԃ��V�O�i�����ǉ�

hmrGateSwitcher:v1_02/130324 hmIto
	GateKind��GateType�ɕύX���A�����ϐ������ύX
	Com��Foma�̐ݒ���s��ComSetting/FomaSetting��ǉ�
	open�֐���GateType��I�����ĊJ���`�ɕύX
hmrGateHab:v1.01/130324 amby
	chekGaitKind ����
	GaitKind �́@typedef��
	�݌v
	�@�P�AOpen, close �ł��ׂĂ������ł���悤�ɂ���
			�ǂ����؂�ւ��Ă��Aopen close ����񂾂���c
	  �@�@�@set �Ƃ��A�悭�킩��Ȃ�����͗v�����Ȃ�
	�@�Q�AParameter�@�͂Ƃ肠����OPEN�֐��Őݒ肵�Ă��炦��΂悢
			�������Ȃ�����X�g���N�g����
hmrGateHab:v1.00/130310 amby
	hmrGateSwitcher�̍쐬�B
	�ݒ�͊�{�I�ɁA�Z�b�g�֐���open�O�ɍs��
	�ϐ��������̈ʒu�����������l�����ق����悢�A�ӔC���������肵�ĂȂ�

*/

#include <string>
#include <hmLib/gate.hpp>
#include <hmLib/comgate.hpp>
#include <hmLib/exceptions.hpp>
#include <hmLib/signals.hpp>
#include <boost/signals2.hpp>
#include <hmLib/inquiries/inquiry.hpp>
#include "fomagate.hpp"

namespace hmr{
	class cGateSwitcher:public hmLib::gate{
		typedef cGateSwitcher my_type;
	public:
		enum GateType{
			NullGateType,
			FomaGateType,
			ComGateType
		};
		struct cFomaSetting{
			int PortNo;
			int Baudrate;
			std::string PhoneNo;
			cFomaSetting():PortNo(0),Baudrate(9600),PhoneNo("08012345678"){}
		};
		struct cComSetting{
			int PortNo;
			int Baudrate;
			cComSetting():PortNo(0),Baudrate(9600){}
		};

	private:
		// �I�����ꂽGate �̃|�C���^
		hmLib::gate* pGate;
		// �ڑ�����Gate�̑I��
		GateType SwitchedGate;
		//FomaGate
		fomacom_gate fomaGate;
		//ComGate
		hmLib::comgate comGate;
	public:
		//FomaGate
		cFomaSetting FomaSetting;
		//ComGate
		cComSetting ComSetting;
	public:
		// Default Const
		cGateSwitcher():pGate(nullptr){}
		// Destructor
		~cGateSwitcher(){
			if(is_open()){ close();}
			pGate = nullptr;
		}
		//Copy�֎~
		cGateSwitcher(const my_type& My_);
		const my_type& operator=(const my_type& My_);
		//Move�֎~�i���Ă��������ǖʓ|�j
		cGateSwitcher(my_type&& My_);
		const my_type& operator=(my_type&& My_);
		// What Kind of gate do you use
		GateType getSwitchedGate(){return SwitchedGate;}
		// port open
		// Open Foma Gate
		bool open(GateType GateType_){
			hmLib_assert(!is_open(), hmLib::io_exceptions::not_opened,"Other port is still open, we cannot open a port");

			if(GateType_==FomaGateType){
				SwitchedGate = FomaGateType;
				pGate = &fomaGate;

				if(fomaGate.open(FomaSetting.PortNo, FomaSetting.Baudrate, FomaSetting.PhoneNo)){
					// port ���@�󂯂�Ȃ������Ƃ�
					SwitchedGate = NullGateType;
					pGate = nullptr;
					// signal ���M
					//signal_inform_isFOMAOpen(false);
					return true;
				}else{
					// port Open ����
					signal_inform_isFOMAOpen(true);
					return false;
				}

			}else if(GateType_==ComGateType){
				SwitchedGate = ComGateType;
				pGate = &comGate;
			
				if( comGate.open(ComSetting.PortNo,ComSetting.Baudrate)){
					// port ���󂯂�Ȃ������Ƃ�
					SwitchedGate = NullGateType;
					pGate = nullptr;
					// signal ���M
					//signal_inform_isRSOpen(false);
					return true;
				}else{
					// port Open ����
					signal_inform_isRSOpen(true);
					return false;
				}
			}else{
				return true;
			}

			return false;	
		}
		// is open??
		bool is_open(){
			if(pGate == nullptr){
				return false;
			}else{
				return true;
			}
		}
		// port close
		bool close(){
			hmLib_assert(is_open(), hmLib::io_exceptions::not_opened, "A Port is not open, we cannot close the port");

			if(SwitchedGate == FomaGateType){
				if(!fomaGate.close()){
					// close ���������ꍇ
					pGate = nullptr;
					SwitchedGate=NullGateType;
					//signal ���M
					signal_inform_isFOMAOpen(false);
					return false;
				}else return true;

			}else if(SwitchedGate == ComGateType){
				if(!comGate.close()){
					// close ���������ꍇ
					pGate = nullptr;
					SwitchedGate=NullGateType;
					// signal ���M
					signal_inform_isRSOpen(false);
					return false;
				}else return true;
			}else{
				hmLib_assert(false, hmLib::memory_exceptions::memory_exception, "A kind of a gate is not determined");
				return true;
			}
		}
	public:
		// ��M�\���H�H
		bool can_getc() override{
			if(!is_open())return false;
			return pGate->can_getc();
		}
		// ��M�����Ă��邩�ǂ���
		bool flowing() override{
			if(pGate==nullptr)return true;
//			hmLib_assert(pGate!=nullptr, hmLib::exceptions::nullptr_dereferemce,"You try to access a gate which is not defined");
			return pGate->flowing();
		}
		//����byte��M�@��M�����A�h���X�ƁA��M�������������@���ۂ̎�M���������߂�l
		size_type gets(char* str_,const size_type& size_) override{
			hmLib_assert(pGate!=nullptr, hmLib::memory_exceptions::memory_exception,"You try to access a gate which is not defined");
			return pGate->gets(str_, size_);
		}
		char getc()override{
			hmLib_assert(pGate != nullptr, hmLib::memory_exceptions::memory_exception, "You try to access a gate which is not defined");
			return pGate->getc();
		}



		//���M�\��Ԃ��̊m�F
		bool can_putc() override{
			if(!is_open())return false;
			return pGate->can_putc();
		}
		//���M������肵�Ă܂�
		void flush() override{
			if(pGate==nullptr)return;
//			hmLib_assert(pGate!=nullptr, hmLib::exceptions::nullptr_dereferemce,"You try to access a gate which is not defined");
			return pGate->flush();
		}
		//����byte���M�@���M�����A�h���X�ƁA���M�������������@���ۂ̑��M���������߂�l
		size_type puts(const char* str_,const size_type& size_) override{
			hmLib_assert(pGate!=nullptr, hmLib::memory_exceptions::memory_exception,"You try to access a gate which is not defined");
			return pGate->puts(str_,size_);
		}
		void putc(char c)override{ 
			hmLib_assert(pGate != nullptr, hmLib::memory_exceptions::memory_exception, "You try to access a gate which is not defined");
			return pGate->putc(c); 
		}


	// SIGNAL SLOT �֐�
	// �ϐ�
	public:
		// signals 
		boost::signals2::signal<void(bool)> signal_inform_isRSOpen;
		boost::signals2::signal<void(bool)> signal_inform_isFOMAOpen;

	private: 
		//slots
		hmLib::signals::unique_connections connections;

	// SLOT�̒��g
	private:
		// OPEN �֐�
		void set_and_openRS(int baudrate, int port){
			// set
			ComSetting.Baudrate = baudrate;
			ComSetting.PortNo = port;

			// open 
			bool isOpen = !open(ComGateType);

			// signal ���̂́Aopen �֐��ő��邱�Ƃɂ����B
			// �@���ǁA��Ԍ��n�I�ȂƂ���ő����Ă����Ȃ��ƁA���C������Open����Ƃ������ꍇ�ɑΉ��ł��Ȃ�
			//   ������ꂽ��K���m�点��K�v������̂ŁB�B�B

			// signal ����
			//signal_inform_isRSOpen(isOpen);
		}

		void set_and_openFOMA(int baudrate, int port){
			// set
			FomaSetting.Baudrate = baudrate;
			FomaSetting.PortNo = port;

			// open 
			bool isOpen = !open(FomaGateType);

			// signal ����
			//signal_inform_isFOMAOpen(isOpen);
		}

	
	// �O���p�̃X���b�g�֐�
	public:
		// RS �|�[�g Open
		// slot
		void slot_set_and_openRS(boost::signals2::signal<void(int, int)> &sig_){
			connections(sig_.connect([&](int v1, int v2)->void{this->set_and_openRS(v1, v2);}));
		}
		// FOMA �|�[�g Open
		// slot
		void slot_set_and_openFOMA(boost::signals2::signal<void(int, int)> &sig_){
			connections(sig_.connect([&](int v1, int v2)->void{this->set_and_openFOMA(v1, v2);}));
		}
		// �|�[�g�@Close
		// slot 
		void slot_closePort(boost::signals2::signal<void(void)> &sig_){
			connections(sig_.connect([&](void)->void{this->close();}));
		}


	// INquirely
		// is open 
		void contact_is_open(hmLib::inquiries::inquiry<bool>& Inq_){
			hmLib::inquiries::connect(Inq_,[&](void)->bool{return this->is_open();});
		}
		// can put
		void contact_can_put(hmLib::inquiries::inquiry<bool>& Inq_){
			hmLib::inquiries::connect(Inq_,[&](void)->bool{return this->can_putc();});
		}
		// can get
		void contact_can_get(hmLib::inquiries::inquiry<bool>& Inq_){
			hmLib::inquiries::connect(Inq_,[&](void)->bool{return this->can_getc();});
		}


	};

}

#
#endif
