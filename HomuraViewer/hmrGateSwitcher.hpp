

#ifndef HMR_GATEHAB_INC
#define HMR_GATEHAB_INC 104
#
/*===hmrGateHab===
Gateの切り替えなどを制御するgateクラス

hmrGateSwitcher:v1_04/130414 amby
	signal, slot　の名前
	connection　を一つに統一

	signal を呼ぶ場所に問題があったため解決
	open, closeなど、実際に状態を変えるの関数の中で実装。

hmrGateSwitcher:v1_03/130406 amby
	ポートオープン、クローズのスロットを追加
	ポートオープンの成否を返すシグナルも追加

hmrGateSwitcher:v1_02/130324 hmIto
	GateKindをGateTypeに変更し、内部変数名ｍ変更
	ComとFomaの設定を行うComSetting/FomaSettingを追加
	open関数でGateTypeを選択して開く形に変更
hmrGateHab:v1.01/130324 amby
	chekGaitKind 実装
	GaitKind の　typedef化
	設計
	　１、Open, close ですべてを完結できるようにする
			どうせ切り替えても、open close するんだから…
	  　　　set とか、よくわからない操作は要求しない
	　２、Parameter　はとりあえずOPEN関数で設定してもらえればよい
			厳しくなったらストラクト化す
hmrGateHab:v1.00/130310 amby
	hmrGateSwitcherの作成。
	設定は基本的に、セット関数でopen前に行う
	変数初期化の位置をもう少し考えたほうがよい、責任がしっかりしてない

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
		// 選択されたGate のポインタ
		hmLib::gate* pGate;
		// 接続するGateの選択
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
		//Copy禁止
		cGateSwitcher(const my_type& My_);
		const my_type& operator=(const my_type& My_);
		//Move禁止（してもいいけど面倒）
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
					// port が　空けれなかったとき
					SwitchedGate = NullGateType;
					pGate = nullptr;
					// signal 送信
					//signal_inform_isFOMAOpen(false);
					return true;
				}else{
					// port Open 成功
					signal_inform_isFOMAOpen(true);
					return false;
				}

			}else if(GateType_==ComGateType){
				SwitchedGate = ComGateType;
				pGate = &comGate;
			
				if( comGate.open(ComSetting.PortNo,ComSetting.Baudrate)){
					// port が空けれなかったとき
					SwitchedGate = NullGateType;
					pGate = nullptr;
					// signal 送信
					//signal_inform_isRSOpen(false);
					return true;
				}else{
					// port Open 成功
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
					// close 成功した場合
					pGate = nullptr;
					SwitchedGate=NullGateType;
					//signal 送信
					signal_inform_isFOMAOpen(false);
					return false;
				}else return true;

			}else if(SwitchedGate == ComGateType){
				if(!comGate.close()){
					// close 成功した場合
					pGate = nullptr;
					SwitchedGate=NullGateType;
					// signal 送信
					signal_inform_isRSOpen(false);
					return false;
				}else return true;
			}else{
				hmLib_assert(false, hmLib::memory_exceptions::memory_exception, "A kind of a gate is not determined");
				return true;
			}
		}
	public:
		// 受信可能か？？
		bool can_getc() override{
			if(!is_open())return false;
			return pGate->can_getc();
		}
		// 受信続いているかどうか
		bool flowing() override{
			if(pGate==nullptr)return true;
//			hmLib_assert(pGate!=nullptr, hmLib::exceptions::nullptr_dereferemce,"You try to access a gate which is not defined");
			return pGate->flowing();
		}
		//複数byte受信　受信文字アドレスと、受信文字数が引数　実際の受信文字数が戻り値
		size_type gets(char* str_,const size_type& size_) override{
			hmLib_assert(pGate!=nullptr, hmLib::memory_exceptions::memory_exception,"You try to access a gate which is not defined");
			return pGate->gets(str_, size_);
		}
		char getc()override{
			hmLib_assert(pGate != nullptr, hmLib::memory_exceptions::memory_exception, "You try to access a gate which is not defined");
			return pGate->getc();
		}



		//送信可能状態かの確認
		bool can_putc() override{
			if(!is_open())return false;
			return pGate->can_putc();
		}
		//送信無理やりしてまう
		void flush() override{
			if(pGate==nullptr)return;
//			hmLib_assert(pGate!=nullptr, hmLib::exceptions::nullptr_dereferemce,"You try to access a gate which is not defined");
			return pGate->flush();
		}
		//複数byte送信　送信文字アドレスと、送信文字数が引数　実際の送信文字数が戻り値
		size_type puts(const char* str_,const size_type& size_) override{
			hmLib_assert(pGate!=nullptr, hmLib::memory_exceptions::memory_exception,"You try to access a gate which is not defined");
			return pGate->puts(str_,size_);
		}
		void putc(char c)override{ 
			hmLib_assert(pGate != nullptr, hmLib::memory_exceptions::memory_exception, "You try to access a gate which is not defined");
			return pGate->putc(c); 
		}


	// SIGNAL SLOT 関数
	// 変数
	public:
		// signals 
		boost::signals2::signal<void(bool)> signal_inform_isRSOpen;
		boost::signals2::signal<void(bool)> signal_inform_isFOMAOpen;

	private: 
		//slots
		hmLib::signals::unique_connections connections;

	// SLOTの中身
	private:
		// OPEN 関数
		void set_and_openRS(int baudrate, int port){
			// set
			ComSetting.Baudrate = baudrate;
			ComSetting.PortNo = port;

			// open 
			bool isOpen = !open(ComGateType);

			// signal 自体は、open 関数で送ることにした。
			// 　結局、一番原始的なところで送っておかないと、メインからOpenするとかいう場合に対応できない
			//   いじられたら必ず知らせる必要があるので。。。

			// signal 送る
			//signal_inform_isRSOpen(isOpen);
		}

		void set_and_openFOMA(int baudrate, int port){
			// set
			FomaSetting.Baudrate = baudrate;
			FomaSetting.PortNo = port;

			// open 
			bool isOpen = !open(FomaGateType);

			// signal 送る
			//signal_inform_isFOMAOpen(isOpen);
		}

	
	// 外部用のスロット関数
	public:
		// RS ポート Open
		// slot
		void slot_set_and_openRS(boost::signals2::signal<void(int, int)> &sig_){
			connections(sig_.connect([&](int v1, int v2)->void{this->set_and_openRS(v1, v2);}));
		}
		// FOMA ポート Open
		// slot
		void slot_set_and_openFOMA(boost::signals2::signal<void(int, int)> &sig_){
			connections(sig_.connect([&](int v1, int v2)->void{this->set_and_openFOMA(v1, v2);}));
		}
		// ポート　Close
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
