#ifndef HMRIO_INC
#define HMRIO_INC 106
#
/*===hmrIO===
hmrIO:v1_06/130525 amby
	VMC関連のスロットの実装

hmrIO:v1_05/130519 amby
	全二重、半二重通信の管理をすべて　Operatorに委譲
	それに従って、全二重、半二重による操作の変更をやめた。

hmrIO:v1_04/130406 amby
	手入力したデータを送信するためのSLOTの追加

hmrIO:v1_03/130324 hmIto
	cFHdxIOを整理して追加
	work関数以外の不要なものを整理
	open/close/is_openを追加
hmrIO:v1_02/130310 amby
	hmrGateHabに対応し始める。まだ始めたばかり。
	こいつはＶＭＣを所有している、gateHubを保持(ポインタだけ)している
	基本的に、ＧａｔｅとＶＭＣの橋渡しをしてあげるだけの責任
	Gateのオープンとか管理は、メイン関数がやってあげる

hmrIO:v1_01/130227 hmIto
	hmr名前空間へ移動

	dxシリーズを分離
	cFdxIOのコンストラクタを、templateでvmccreaterを取得する方法に変更
	unique_ptrのアドレスへのアクセスにget関数を利用するよう修正

hmrIO_v1_00/130223 amby
	VMC形式に対応途中
		1, BufGateを廃止
		2, VMC func をwork関数内に導入

	問題点
		1, VMC を unique pointerで実装したが、
		　Ｃ側が普通のptr引数にしているのでエラーっている。あきらめるべきか
		＞変更したように、unique_ptr::get関数を用いることで生ポにアクセスできる。
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
	// 全二重通信用IOクラス
	class cFdxIO{
	private:
		//送受信制御用クラス
		// このGateを通じてデータの出し入れを行う
		// これはメインで定義して、ポインタだけいただいてやっていく
		// ＩＯが死んでも、ほかのところで使われることを想定。。
		hmLib::gate* pGate;
		// deleter 用の関数オブジェクト
		struct deleter_VMC{void operator()(VMC1* pVMC){vmc1_release(pVMC);}};
		// VMC用の関数ポインタ保持変数のポインタ
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
			// 受信処理
			while(vmc1_can_recv(pVMC.get()) && (!pGate->empty())){
				char c;
				pGate->get(&c,1);
				vmc1_recv(pVMC.get(),c);

				if(clock::now() - time>std::chrono::milliseconds(500))break;
			}
		
			time=clock::now();
			// 送信処理
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

	// 全二重、半二重通信対応クラス
	class cFHdxIO{
		typedef boost::signals2::signal<void(bool)> signal;
	private:
		typedef enum{
			FULL_DUPLEX, HALF_DUPLEX
		}DuplexMode;
	private:
		// どちらのモードにするかフラグ
		//DuplexMode dxMode;
	private:
		//送受信制御用クラス
		hmLib::gate* pGate;
		// deleter 用の関数オブジェクト
		struct deleter_VMC{void operator()(VMC1* pVMC){vmc1_release(pVMC);}};
		// VMC用の関数ポインタ保持変数のポインタ
		std::unique_ptr<VMC1, deleter_VMC> pVMC;

	// signal slot 実装部分
	public:
		boost::signals2::signal<void(clock::time_point)> signal_inform_VMC_force_end_recv;
		boost::signals2::signal<void(clock::time_point)> signal_inform_VMC_force_end_send;

	// slot 系の実装
	private:
		// コネクト関数 PoutString
		hmLib::signals::unique_connections connections;		
		
		// 送信関数
		//  この関数を使う場合は、入力されたら否応なしにその瞬間に送る
		//  おそらく、そういう特殊な使いかたをしたいと思うので
		void send_direct(std::string str_){
			int size = str_.size();
			//char chars[256];
			//strcpy( chars, str_.c_str() );
			pGate->put(str_.c_str(), size);
		}

		// VMC の送信強制リセット関数
		void VMC_force_end_send(void){
			vmc1_force_end_send(pVMC.get(),0xFF);
			signal_inform_VMC_force_end_send(clock::now());
		}
		// VMCの受信強制リセット関数
		void VMC_force_end_recv(void){
			vmc1_force_end_recv(pVMC.get(),0xFF);
			signal_inform_VMC_force_end_recv(clock::now());
		}
		// VMCの送信エラー除去関数
		void VMC_clearSendErr(void){
			vmc1_clearSendErr(pVMC.get());
		}
		// VMCの受信エラー除去関数
		void VMC_clearRecvErr(void){
			vmc1_clearRecvErr(pVMC.get());
		}

	// slot 系のコネクト関数
	public:
		// 送信関数ようスロット設定
		void slot_send_direct(boost::signals2::signal<void(std::string)> &sig_){
			connections(sig_.connect([&](std::string str_)->void{this->send_direct( str_);}));
		}
		
		// VMC関数の強制リセット機能を提供
		void slot_VMC_force_end_send(boost::signals2::signal<void(void)> &sig_){
			connections(sig_.connect([&](void)->void{this->VMC_force_end_send();}));
		}
		void slot_VMC_force_end_recv(boost::signals2::signal<void(void)> &sig_){
			connections(sig_.connect([&](void)->void{this->VMC_force_end_recv();}));
		}
		// VMC エラー除去機能を提供
		void slot_VMC_clearSendErr(boost::signals2::signal<void(void)> &sig_){
			connections(sig_.connect([&](void)->void{this->VMC_clearSendErr();}));
		}
		void slot_VMC_clearRecvErr(boost::signals2::signal<void(void)> &sig_){
			connections(sig_.connect([&](void)->void{this->VMC_clearRecvErr();}));
		}

	// inq contact
	public:
		// Err をGet する関数
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
				// 受信処理
				while(vmc1_can_recv(pVMC.get()) && (pGate->can_get()) && (!pGate->empty())){
					char c;
					if(pGate->get(&c,1)==0)break;
					vmc1_recv(pVMC.get(),c);
				}
		
				// 送信処理
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
		// Inquire の定義
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
