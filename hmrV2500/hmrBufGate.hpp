#ifndef HMR_BUFGATE_INC
#define HMR_BUFGATE_INC 101
#

#include <iostream>
#include <hmLib_v3_05/gate.hpp>
#include <hmLib_v3_05/exceptions.hpp>
#include <hmLib_v3_05/signals.hpp>
#include <hmLib_v3_05/inquiries.hpp>
#include <boost/signals2.hpp>
#include <deque>
#include <vector>

/*===hmrBufGate===

バッファにデータをためて、送受信していくクラス
将来別スレッドで動くことを考えて実装しておく。
Mutex とかは考えていない

hmrBufGate:v1_01/130519 amby
	signal, slot, contact の実装を行う

hmrBufGate:v1_00/130511 hmIto
	work関数をoperator()(unsigned int GetMaxNum,unsigned int PutMaxNum)に変更しました。
*/

//バッファGate
//using namespace hmLib;
namespace hmr{
	class bufgate:public hmLib::gate{
	private:
		hmLib::gate* pGate;
		std::deque<char> oBuf;
		std::deque<char> iBuf;
	public:
		bufgate():pGate(0){return;}
		~bufgate(){if(is_open())close();}
		bool open(hmLib::gate& rGate_){
			hmLib_assert(!is_open(),hmLib::gate_opened_exception,"bufgate have already been opened.");

			pGate=&rGate_;

			return false;
		}
		bool close(){
			hmLib_assert(is_open(),hmLib::gate_not_opened_exception,"bufgate have not been opened yet.");

			pGate=0;

			return false;
		}
		bool is_open()const{return pGate!=0;}
		void operator()(void){
			hmLib_assert(is_open(),hmLib::gate_not_opened_exception,"bufgate have not been opened yet.");
			try{
				if(!pGate->empty()){
					while(!pGate->empty() && pGate->can_get()){
						hmLib::gate::size_type size=1;
						char tmp;
						pGate->get(&tmp,size);
						iBuf.push_back(tmp);
					}
				}
				if(!oBuf.empty()){
					while(!oBuf.empty() && pGate->can_put()){
						hmLib::gate::size_type size=1;
						pGate->put(&oBuf.front(),size);
						oBuf.pop_front();
					}
				}
			}catch(const hmLib::gate_not_opened_exception& Except_){
				hmLib_thrownext(Except_,hmLib::exceptions::invalid_status,"gate connected with bufgate is not opened.");
			}catch(const std::exception& Except_){
				hmLib_thrownext(Except_,hmLib::exceptions::invalid_status,"bufgate fail to get/put work.");
			}
		}
		void operator()(unsigned int GetMaxNum_,unsigned int PutMaxNum_){
			hmLib_assert(is_open(),hmLib::gate_not_opened_exception,"bufgate have not been opened yet.");
			try{
				if(!pGate->empty()){
					while(!pGate->empty() && pGate->can_get() && GetMaxNum_){
						hmLib::gate::size_type size=1;
						char tmp;
						if(pGate->get(&tmp,size))break;
						iBuf.push_back(tmp);
						--GetMaxNum_;
					}
				}
				if(!oBuf.empty()){
					while(!oBuf.empty() && pGate->can_put() && PutMaxNum_){
						hmLib::gate::size_type size=1;
						pGate->put(&oBuf.front(),size);
						oBuf.pop_front();
						--PutMaxNum_;
					}
				}
			}catch(const hmLib::gate_not_opened_exception& Except_){
				hmLib_thrownext(Except_,hmLib::exceptions::invalid_status,"gate connected with bufgate is not opened.");
			}catch(const std::exception& Except_){
				hmLib_thrownext(Except_,hmLib::exceptions::invalid_status,"bufgate fail to get/put work.");
			}
		}
		unsigned int psize()const{return oBuf.size();}
		unsigned int gsize()const{return iBuf.size();}
		void pclear(){oBuf.clear();}
		void gclear(){iBuf.clear();}
	public://itfGate
		//受信待データ
		virtual bool empty()override{return iBuf.empty();}
		//受信可能
		virtual bool can_get()override{
			if(!is_open())return false;
			return pGate->can_get();
		}
		//複数byte受信
		virtual size_type get(char* str_,const size_type& size_)override{	
			size_type pos=0;
			for(;pos<size_;++pos){
				if(!can_get())break;
				*str_=iBuf.front();
				iBuf.pop_front();
				++str_;
			}
			return pos;
		}
		//送信待データ
		virtual bool full()override{return false;}
		//送信可能
		virtual bool can_put()override{
			if(!is_open())return false;
			return pGate->can_put();
		}
		//複数byte送信
		virtual size_type put(const char* str_,const size_type& size_)override{
			size_type pos=0;
			for(;pos<size_;++pos){
				if(!can_put())break;
				oBuf.push_back(*str_);
				++str_;
			}
			return pos;
		}

	private: 
		// slot connections
		hmLib::signals::unique_connections connections;
	public:
		// connection
		void slot_clearRecvBuf(boost::signals2::signal<void(void)> &sig_){
			connections(sig_.connect([&]()->void{this->gclear();}));
		}
		void slot_clearSendBuf(boost::signals2::signal<void(void)> &sig_){
			connections(sig_.connect([&]()->void{this->pclear();}));
		}

		// Contact関数
		void contact_sendBufSize(hmLib::inquiries::inquiry<unsigned int>& SendBufSizeInq_){
			hmLib::inquiries::connect(SendBufSizeInq_, [&](void)->unsigned int{return this->psize();});
		}
		void contact_recvBufSize(hmLib::inquiries::inquiry<unsigned int>& RecvBufSizeInq_){
			hmLib::inquiries::connect(RecvBufSizeInq_, [&](void)->unsigned int{return this->gsize();});
		}

		void contact_is_open(hmLib::inquiries::inquiry<bool>& Inq_){
			hmLib::inquiries::connect(Inq_,[&](void)->bool{return this->is_open();});
		}
		void contact_can_put(hmLib::inquiries::inquiry<bool>& Inq_){
			hmLib::inquiries::connect(Inq_,[&](void)->bool{return this->can_put();});
		}
		void contact_can_get(hmLib::inquiries::inquiry<bool>& Inq_){
			hmLib::inquiries::connect(Inq_,[&](void)->bool{return this->can_get();});
		}
	};
}
/*	//有限バッファGate
	class limbufgate:public itfGate{
	private:
		itfGate* pGate;
		hmLib::bicircular<char> oBuf;
		hmLib::bicircular<char> iBuf;
	public:
		limbufgate():pGate(0){return;}
		~limbufgate(){close();}
		bool open(itfGate& rGate_,unsigned int oBufSize,unsigned int iBufSize){
			//すでに開かれていたら、エラーを返す
			if(is_open())return true;

			pGate=&rGate_;

			//Buf確保
			oBuf.create(oBufSize);
			iBuf.create(iBufSize);

			return false;
		}
		bool close(){
			//すでに閉じられていたら、エラーを返す
			if(!is_open())return true;

			pGate=0;

			oBuf.release();
			iBuf.release();

			return false;
		}
		bool is_open()const{return pGate!=0;}
		void operator()(void){
			if(!pGate->empty()){
				while(!pGate->empty()){
					iBuf.push_back(pGate->getc());
				}
			}else if(!oBuf.empty()){
				while(!oBuf.empty()){
					pGate->putc(oBuf.front());
					oBuf.pop_front();
				}
			}
		}
		void operator()(unsigned int GetMaxNum_,unsigned int PutMaxNum_){
			if(!pGate->empty()){
				while(!pGate->empty() && GetMaxNum_){
					iBuf.push_back(pGate->getc());
					--GetMaxNum_;
				}
			}else if(!oBuf.empty()){
				while(!oBuf.empty() && PutMaxNum_){
					pGate->putc(oBuf.front());
					oBuf.pop_front();
					--PutMaxNum_;
				}
			}
		}
		unsigned int psize()const{return oBuf.size();}
		unsigned int gsize()const{return iBuf.size();}
		void pclear(){oBuf.clear();}
		void gclear(){iBuf.clear();}
	public://itfGate
		//受信待データ
		virtual bool empty(){return iBuf.empty();}
		//受信可能
		virtual bool can_get(){return !empty();}
		//複数byte受信
		virtual bool get(char* str_,size_type& size_){
			size_type max=size_;
			for(size_=0;size_<max;++size_){
				if(!can_get())return true;
				iBuf.front(*str_);
				iBuf.pop_front();
				++str_;
			}
			return false;
		}
		//送信待データ
		virtual bool full(){return oBuf.full();}
		//送信可能
		virtual bool can_put(){return !full();}
		//複数byte送信
		virtual bool put(const char* str_,size_type& size_){
			size_type max=size_;
			for(size_=0;size_<max;++size_){
				if(!can_put())return true;
				oBuf.push_back(*str_);
				++str_;
			}
			return false;
		}
	};
	*/
#
#endif









