#ifndef HMR_VIRTUAL_GATE_INC
#define HMR_VIRTUAL_GATE_INC
#
#include<thread>
#include<mutex>
#include<hmLib_v3_06/gate.hpp>
#include"hmrVMC1.h"
namespace hmr{
	template<typename reaction_>
	class virtual_gate:public gate{
	private:
		reaction_ Reaction;
	private:
		template<unsigned int ch>
		class vmc_connecter{
		public:
			static 
		public:
			vmc_holder()
		};
	public:
		
	public://gate
		//��M�\��Ԃ��̊m�F
		bool can_get()override{
			return true;
		}
		//��M�\�f�[�^�̗L��
		bool empty()override=0;
		//����byte��M�@��M�����A�h���X�ƁA��M�������������@���ۂ̎�M���������߂�l
		size_type get(_Elem* str_,const size_type& size_)override=0;
		//���M�\��Ԃ��̊m�F
		bool can_put()override{
			return true;
		//���M�\�f�[�^�̗L��
		bool full()override=0;
		//����byte���M�@���M�����A�h���X�ƁA���M�������������@���ۂ̑��M���������߂�l
		size_type put(const _Elem* str_,const size_type& size_)override=0;
	};
}
#
#endif
