#ifndef HMRIO_INC
#define HMRIO_INC 106
#
/*===hmrIO===
hmrIO:v1_06/130525 amby
	VMC�֘A�̃X���b�g�̎���

hmrIO:v1_05/130519 amby
	�S��d�A����d�ʐM�̊Ǘ������ׂā@Operator�ɈϏ�
	����ɏ]���āA�S��d�A����d�ɂ�鑀��̕ύX����߂��B

hmrIO:v1_04/130406 amby
	����͂����f�[�^�𑗐M���邽�߂�SLOT�̒ǉ�

hmrIO:v1_03/130324 hmIto
	cFHdxIO�𐮗����Ēǉ�
	work�֐��ȊO�̕s�v�Ȃ��̂𐮗�
	open/close/is_open��ǉ�
hmrIO:v1_02/130310 amby
	hmrGateHab�ɑΉ����n�߂�B�܂��n�߂��΂���B
	�����͂u�l�b�����L���Ă���AgateHub��ێ�(�|�C���^����)���Ă���
	��{�I�ɁA�f�������Ƃu�l�b�̋��n�������Ă����邾���̐ӔC
	Gate�̃I�[�v���Ƃ��Ǘ��́A���C���֐�������Ă�����

hmrIO:v1_01/130227 hmIto
	hmr���O��Ԃֈړ�

	dx�V���[�Y�𕪗�
	cFdxIO�̃R���X�g���N�^���Atemplate��vmccreater���擾������@�ɕύX
	unique_ptr�̃A�h���X�ւ̃A�N�Z�X��get�֐��𗘗p����悤�C��

hmrIO_v1_00/130223 amby
	VMC�`���ɑΉ��r��
		1, BufGate��p�~
		2, VMC func ��work�֐����ɓ���

	���_
		1, VMC �� unique pointer�Ŏ����������A
		�@�b�������ʂ�ptr�����ɂ��Ă���̂ŃG���[���Ă���B������߂�ׂ���
		���ύX�����悤�ɁAunique_ptr::get�֐���p���邱�ƂŐ��|�ɃA�N�Z�X�ł���B
*/

#include <fstream>
#include <memory>
#include <boost/signals2.hpp>
#include <hmLib/signals.hpp>
#include <hmLib/exceptions.hpp>
#include <hmLib/inquiries/inquiry.hpp>
#include <hmLib/gate.hpp>
#include "Data.hpp"
#include "VMC1.h"

namespace hmr{
	// �S��d�ʐM�pIO�N���X
	class cFdxIO{
	private:
		//����M����p�N���X
		// ����Gate��ʂ��ăf�[�^�̏o��������s��
		// ����̓��C���Œ�`���āA�|�C���^�������������Ă���Ă���
		// �h�n������ł��A�ق��̂Ƃ���Ŏg���邱�Ƃ�z��B�B
		hmLib::gate* pGate;
		// deleter �p�̊֐��I�u�W�F�N�g
		struct deleter_VMC{void operator()(VMC1* pVMC){vmc1_release(pVMC);}};
		// VMC�p�̊֐��|�C���^�ێ��ϐ��̃|�C���^
		std::unique_ptr<VMC1, deleter_VMC> pVMC;
	public:
		template<class VMCCreater>
		cFdxIO(VMCCreater& vmcc_):pVMC(vmcc_()),pGate(nullptr){
			static const unsigned char ID[3]={'c','t','r'};
			static const unsigned char TID[3]={'h','m','r'};
			vmc1_initialize(pVMC.get(),ID,TID);
		}
		void work(){
			hmLib_assert(pGate,hmLib::exceptions::io_not_opened,"pGate have no address");
			hmLib_assert(pVMC,hmLib::exceptions::io_not_opened,"pVMC have no address");

			clock::time_point time;
			time=clock::now();
			// ��M����
			while(vmc1_can_recv(pVMC.get()) && (!pGate->empty())){
				char c;
				pGate->get(&c,1);
				vmc1_recv(pVMC.get(),c);

				if(clock::now() - time>std::chrono::milliseconds(500))break;
			}
		
			time=clock::now();
			// ���M����
			while(vmc1_can_send(pVMC.get()) && (!pGate->full())){
				char c;
				c=vmc1_send(pVMC.get());
				pGate->put(&c,1);

				if(clock::now() - time>std::chrono::milliseconds(500))break;
			}
		}
		void open(hmLib::gate* pGate_){
			hmLib_assert(!is_open(),hmLib::exceptions::io_opened,"FdxIO have already opened");
			pGate=pGate_;
		}
		bool is_open(){return pGate!=nullptr;}
		void close(){
			hmLib_assert(is_open(),hmLib::exceptions::io_not_opened,"FdxIO have already closed");
			pGate=nullptr;
		}
	};

	// �S��d�A����d�ʐM�Ή��N���X
	class cFHdxIO{
		typedef boost::signals2::signal<void(bool)> signal;
	private:
		typedef enum{
			FULL_DUPLEX, HALF_DUPLEX
		}DuplexMode;
	private:
		// �ǂ���̃��[�h�ɂ��邩�t���O
		//DuplexMode dxMode;
	private:
		//����M����p�N���X
		hmLib::gate* pGate;
		// deleter �p�̊֐��I�u�W�F�N�g
		struct deleter_VMC{void operator()(VMC1* pVMC){vmc1_release(pVMC);}};
		// VMC�p�̊֐��|�C���^�ێ��ϐ��̃|�C���^
		std::unique_ptr<VMC1, deleter_VMC> pVMC;

	// signal slot ��������
	public:
		boost::signals2::signal<void(clock::time_point)> signal_inform_VMC_force_end_recv;
		boost::signals2::signal<void(clock::time_point)> signal_inform_VMC_force_end_send;

	// slot �n�̎���
	private:
		// �R�l�N�g�֐� PoutString
		hmLib::signals::unique_connections connections;		
		
		// ���M�֐�
		//  ���̊֐����g���ꍇ�́A���͂��ꂽ��ۉ��Ȃ��ɂ��̏u�Ԃɑ���
		//  �����炭�A������������Ȏg���������������Ǝv���̂�
		void send_direct(std::string str_){
			int size = str_.size();
			//char chars[256];
			//strcpy( chars, str_.c_str() );
			pGate->put(str_.c_str(), size);
		}

		// VMC �̑��M�������Z�b�g�֐�
		void VMC_force_end_send(void){
			vmc1_force_end_send(pVMC.get(),0xFF);
			signal_inform_VMC_force_end_send(clock::now());
		}
		// VMC�̎�M�������Z�b�g�֐�
		void VMC_force_end_recv(void){
			vmc1_force_end_recv(pVMC.get(),0xFF);
			signal_inform_VMC_force_end_recv(clock::now());
		}
		// VMC�̑��M�G���[�����֐�
		void VMC_clearSendErr(void){
			vmc1_clearSendErr(pVMC.get());
		}
		// VMC�̎�M�G���[�����֐�
		void VMC_clearRecvErr(void){
			vmc1_clearRecvErr(pVMC.get());
		}

	// slot �n�̃R�l�N�g�֐�
	public:
		// ���M�֐��悤�X���b�g�ݒ�
		void slot_send_direct(boost::signals2::signal<void(std::string)> &sig_){
			connections(sig_.connect([&](std::string str_)->void{this->send_direct( str_);}));
		}
		
		// VMC�֐��̋������Z�b�g�@�\���
		void slot_VMC_force_end_send(boost::signals2::signal<void(void)> &sig_){
			connections(sig_.connect([&](void)->void{this->VMC_force_end_send();}));
		}
		void slot_VMC_force_end_recv(boost::signals2::signal<void(void)> &sig_){
			connections(sig_.connect([&](void)->void{this->VMC_force_end_recv();}));
		}
		// VMC �G���[�����@�\���
		void slot_VMC_clearSendErr(boost::signals2::signal<void(void)> &sig_){
			connections(sig_.connect([&](void)->void{this->VMC_clearSendErr();}));
		}
		void slot_VMC_clearRecvErr(boost::signals2::signal<void(void)> &sig_){
			connections(sig_.connect([&](void)->void{this->VMC_clearRecvErr();}));
		}

	// inq contact
	public:
		// Err ��Get ����֐�
		void contactSendErr(hmLib::inquiries::inquiry<unsigned char>& sendErrInq_){
			//unsigned char a = vmc1_getSendErr(this->pVMC.get());
			hmLib::inquiries::connect(sendErrInq_,[&](void)->unsigned char{return vmc1_getSendErr(this->pVMC.get());});
		}
		void contactRecvErr(hmLib::inquiries::inquiry<unsigned char>& recvErrInq_){
			hmLib::inquiries::connect(recvErrInq_,[&](void)->unsigned char{return vmc1_getRecvErr(this->pVMC.get());});
		}

	public:
		template<class VMCCreater>
		cFHdxIO(VMCCreater& vmcc_):pVMC(vmcc_()), pGate(nullptr){
			unsigned char OID[3]={'c','t','r'};
			unsigned char TID[3]={'h','m','r'};
			vmc1_initialize(pVMC.get(),OID,TID);
		}
		~cFHdxIO(){
			vmc1_finalize(pVMC.get());
		}
		void work(){
			if(is_open()){
				// ��M����
				while(vmc1_can_recv(pVMC.get()) && (pGate->can_get()) && (!pGate->empty())){
					char c;
					if(pGate->get(&c,1)==0)break;
					vmc1_recv(pVMC.get(),c);
				}
		
				// ���M����
				while(vmc1_can_send(pVMC.get()) && (pGate->can_put()) && (!pGate->full())){
					char c;
					c=vmc1_send(pVMC.get());
					pGate->put(&c,1);
				}
			}
		}
		void open(hmLib::gate* pGate_){
			hmLib_assert(!is_open(),hmLib::exceptions::io_opened,"FdxIO have already opened");
			pGate=pGate_;
		}
		bool is_open(){return pGate!=nullptr;}
		void close(){
			hmLib_assert(is_open(),hmLib::exceptions::io_not_opened,"FdxIO have already closed");
			pGate=nullptr;
		}

	public:
		// Inquire �̒�`
		void contact_is_open(hmLib::inquiries::inquiry<bool>& Inq_){
			hmLib::inquiries::connect(Inq_,[&](void)->bool{return this->is_open();});
		}
		void contact_can_put(hmLib::inquiries::inquiry<bool>& Inq_){
			hmLib::inquiries::connect(Inq_,[&](void)->bool{return this->pGate->can_put();});
		}
		void contact_can_get(hmLib::inquiries::inquiry<bool>& Inq_){
			hmLib::inquiries::connect(Inq_,[&](void)->bool{return this->pGate->can_get();});
		}
	};
}
#endif
