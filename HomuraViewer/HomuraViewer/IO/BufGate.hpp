#ifndef HMR_VIEWER_IO_BUFGATE_INC
#define HMR_VIEWER_IO_BUFGATE_INC 101
#

#include <iostream>
#include <deque>
#include <vector>
#include <boost/signals2.hpp>
#include <hmLib/gate.hpp>
#include <hmLib/exceptions.hpp>
#include <hmLib/signals.hpp>
#include <hmLib/inquiries.hpp>

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
	namespace viewer{
		namespace io{
			class bufgate :public hmLib::gate{
			private:
				hmLib::gate* pGate;
				std::deque<char> oBuf;
				std::deque<char> iBuf;
			public:
				bufgate() :pGate(0){ return; }
				~bufgate(){ if(is_open())close(); }
				bool open(hmLib::gate& rGate_){
					hmLib_assert(!is_open(), hmLib::gate_opened_exception, "bufgate have already been opened.");

					pGate = &rGate_;

					return false;
				}
				bool close(){
					hmLib_assert(is_open(), hmLib::gate_not_opened_exception, "bufgate have not been opened yet.");

					pGate = 0;

					return false;
				}
				bool is_open()override{ return pGate != 0; }
				void operator()(void){
					hmLib_assert(is_open(), hmLib::gate_not_opened_exception, "bufgate have not been opened yet.");
					try{
						if(pGate->can_getc()){
							while(pGate->can_getc()){
								//hmLib::gate::size_type size=1;
								iBuf.push_back(pGate->getc());
							}
						}
						if(!oBuf.empty()){
							while(!oBuf.empty() && pGate->can_putc()){
								hmLib::gate::size_type size = 1;
								pGate->putc(oBuf.front());
								oBuf.pop_front();
							}
						}
					}
					catch(const hmLib::gate_not_opened_exception& Except_){
						hmLib_thrownext(Except_, hmLib::io_exceptions::not_opened, "gate connected with bufgate is not opened.");
					}
				}
				void operator()(unsigned int GetMaxNum_, unsigned int PutMaxNum_){
					hmLib_assert(is_open(), hmLib::gate_not_opened_exception, "bufgate have not been opened yet.");
					try{
						if(pGate->can_getc()){
							while(pGate->can_getc() && GetMaxNum_){
								//hmLib::gate::size_type size=1;
								//char tmp;
								//if(pGate->get(&tmp,size))break;
								iBuf.push_back(pGate->getc());
								--GetMaxNum_;
							}
						}
						if(!oBuf.empty()){
							while(!oBuf.empty() && pGate->can_putc() && PutMaxNum_){
								hmLib::gate::size_type size = 1;
								pGate->putc(oBuf.front());
								oBuf.pop_front();
								--PutMaxNum_;
							}
						}
					}
					catch(const hmLib::gate_not_opened_exception& Except_){
						hmLib_thrownext(Except_, hmLib::io_exceptions::not_opened, "gate connected with bufgate is not opened.");
					}
				}
				unsigned int psize()const{ return oBuf.size(); }
				unsigned int gsize()const{ return iBuf.size(); }
				void pclear(){ oBuf.clear(); }
				void gclear(){ iBuf.clear(); }
			public://itfGate
				   //受信可能か
				virtual bool can_getc()override{
					return !iBuf.empty(); // && pGate->can_getc();
				}
				//受信継続中か？？
				virtual bool flowing()override{
					return pGate->flowing();
				}
				//単byte受信
				virtual char getc()override{
					char tmp;
					tmp = iBuf.front();
					iBuf.pop_front();
					return tmp;
				}
				//複数byte受信
				virtual size_type gets(char* str_, const size_type& size_)override{
					size_type pos = 0;
					for(; pos<size_; ++pos){
						if(!can_getc())break;
						*str_ = iBuf.front();
						iBuf.pop_front();
						++str_;
					}
					return pos;
				}
				//送信待データ
				virtual void flush()override{ pGate->flush(); }
				//送信可能
				virtual bool can_putc()override{
					if(!is_open())return false;
					//return pGate->can_putc();
					return true;
				}
				//複数byte送信
				virtual size_type puts(const char* str_, const size_type& size_)override{
					size_type pos = 0;
					for(; pos<size_; ++pos){
						if(!can_putc())break;
						oBuf.push_back(*str_);
						++str_;
					}
					return pos;
				}
				// 1byte 送信
				virtual void putc(char c)override{
					oBuf.push_back(c);
					return;
				}


			private:
				// slot connections
				hmLib::signals::unique_connections connections;
			public:
				// connection
				void slot_clearRecvBuf(boost::signals2::signal<void(void)> &sig_){
					connections(sig_.connect([&]()->void{this->gclear(); }));
				}
				void slot_clearSendBuf(boost::signals2::signal<void(void)> &sig_){
					connections(sig_.connect([&]()->void{this->pclear(); }));
				}

				// Contact関数
				void contact_sendBufSize(hmLib::inquiries::inquiry<unsigned int>& SendBufSizeInq_){
					hmLib::inquiries::connect(SendBufSizeInq_, [&](void)->unsigned int{return this->psize(); });
				}
				void contact_recvBufSize(hmLib::inquiries::inquiry<unsigned int>& RecvBufSizeInq_){
					hmLib::inquiries::connect(RecvBufSizeInq_, [&](void)->unsigned int{return this->gsize(); });
				}

				void contact_is_open(hmLib::inquiries::inquiry<bool>& Inq_){
					hmLib::inquiries::connect(Inq_, [&](void)->bool{return this->is_open(); });
				}
				void contact_can_put(hmLib::inquiries::inquiry<bool>& Inq_){
					hmLib::inquiries::connect(Inq_, [&](void)->bool{return this->can_putc(); });
				}
				void contact_can_get(hmLib::inquiries::inquiry<bool>& Inq_){
					hmLib::inquiries::connect(Inq_, [&](void)->bool{return this->can_getc(); });
				}
			};
		}
	}
}
#
#endif









