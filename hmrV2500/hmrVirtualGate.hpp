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
		//受信可能状態かの確認
		bool can_get()override{
			return true;
		}
		//受信可能データの有無
		bool empty()override=0;
		//複数byte受信　受信文字アドレスと、受信文字数が引数　実際の受信文字数が戻り値
		size_type get(_Elem* str_,const size_type& size_)override=0;
		//送信可能状態かの確認
		bool can_put()override{
			return true;
		//送信可能データの有無
		bool full()override=0;
		//複数byte送信　送信文字アドレスと、送信文字数が引数　実際の送信文字数が戻り値
		size_type put(const _Elem* str_,const size_type& size_)override=0;
	};
}
#
#endif
