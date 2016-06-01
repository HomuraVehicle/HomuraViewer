#ifndef IOLOGGATE_INC
#define IOLOGGATE_INC 102
#
/*===iologgate===
送受信のログ取得関連のクラス群
	iolog_gate<iologger>
		gateとiologgerの仲立ちをつかさどる
		iolggerにlogを蓄積しながら透過的に別のgateに渡す
	iologtype
		送受信において記録すべき情報をつかさどる
		文字列、時間など、任意の保存したい情報セット
	iologger<iologtype>
		timeoutや改行など、保存の方法をつかさどる
		iologtype型の情報の保持を行うクラス
		一般的に最新のもののみ保存する
	iologbuf<iologtype>
		過去の送受信データの保存をつかさどる
		signal/slot接続で、過去のiologtypeのデータ全てを蓄積蓄積している

iologgate:v1_02/130420 hmIto
	dequeバッファをvector型に変更
iologgate:v1_01/130324 hmIto
	iolotypeの文字列記録関数log(char)をoperator()(char)に変更
iologgate:v1_00/130310 hmito
	basic_iologgate, system_clock_iologtype, fdx_crlf_timeout_iologger,fdx_deque_iologbufを追加
*/
#include <chrono>
#include <string>
#include <vector>
#include <mutex>
#include <iostream>
#include <boost/format.hpp>
#include <boost/signals2.hpp>
#include <hmLib_v3_06/inquiries.hpp>
#include <hmLib_v3_06/signals.hpp>
#include <hmLib_v3_06/gate.hpp>

template<typename iologger,typename _Elem, typename _Traits=std::char_traits<_Elem>>
class basic_iologgate:public hmLib::basic_gate<_Elem,_Traits>{
private:
	typedef basic_iologgate<iologger,_Elem,_Traits> my_type;
	typedef hmLib::basic_gate<_Elem,_Traits> gate_type;
private:
	gate_type* pGate;
public:
	iologger Logger;
public:
	basic_iologgate():pGate(nullptr){}
	basic_iologgate(const my_type& My_);
	const my_type& operator=(const my_type& My_);
	basic_iologgate(my_type&& My_):pGate(My_.pGate),Logger(std::move(My_.Logger)){}
	const my_type& operator=(my_type&& My_){
		hmLib_assert(!is_open(),hmLib::gate_opened_exception,"basic_iologgaet have already opened");

		pGate=My_.pGate;
		Logger=std::move(My_.Logger);

		return *this;
	}
	void open(gate_type& rGate_){pGate=&rGate_;}
	void close(){pGate=nullptr;}
	virtual bool is_open()override{return pGate!=nullptr;}
public://gate
	//受信可能状態かの確認
	virtual bool can_getc()override{
		if(!is_open())return false;
		return pGate->can_getc();
	}
	//flowing
	virtual bool flowing()override{
		hmLib_assert(pGate!=nullptr,hmLib::gate_not_opened_exception,"gate have not been opened");
		return pGate->flowing();
	}
	//複数byte受信　受信文字アドレスと、受信文字数が引数　実際の受信文字数が戻り値
	virtual size_type gets(_Elem* str_,const size_type& size_)override{
		hmLib_assert(pGate!=nullptr,hmLib::gate_not_opened_exception,"gate have not been opened");
		size_type Size=pGate->gets(str_,size_);
		for(unsigned int i=0;i<Size;++i)Logger(1,str_[i]);
		return Size;
	}
	virtual _Elem getc(){
		_Elem tmp = pGate->getc();
		Logger(1, tmp);
		return tmp;
	}


	//送信可能状態かの確認
	virtual bool can_putc()override{
		if(!is_open())return false;
		return pGate->can_putc();
	}
	//送信可能データの有無
	virtual void flush()override{
		hmLib_assert(pGate!=nullptr,hmLib::gate_not_opened_exception,"gate have not been opened");
		return pGate->flush();
	}
	//複数byte送信　送信文字アドレスと、送信文字数が引数　実際の送信文字数が戻り値
	virtual size_type puts(const _Elem* str_,const size_type& size_)override{
		hmLib_assert(pGate!=nullptr,hmLib::gate_not_opened_exception,"gate have not been opened");
		size_type Size=pGate->puts(str_,size_);
		for(unsigned int i=0;i<Size;++i){
			Logger(0,str_[i]);
		}
		return Size;
	}
	// 単数バイト送信
	virtual void putc(char c){
		pGate->putc(c);
		Logger(1, c);
	}


	// Inquire の定義
	void contact_is_open(hmLib::inquiries::inquiry<bool>& Inq_){
		hmLib::inquiries::connect(Inq_,[&](void)->bool{return this->is_open();});
	}
	void contact_can_put(hmLib::inquiries::inquiry<bool>& Inq_){
		hmLib::inquiries::connect(Inq_,[&](void)->bool{return this->can_putc();});
	}
	void contact_can_get(hmLib::inquiries::inquiry<bool>& Inq_){
		hmLib::inquiries::connect(Inq_,[&](void)->bool{return this->can_getc();});
	}

};
template<typename iologger>
class iologgate:public basic_iologgate<iologger,char,std::char_traits<char>>{};
//std::chrono::system_clockを用いたloggerタイプ
struct system_clock_iologtype{
	std::chrono::system_clock::time_point BeginTime;
	std::chrono::system_clock::time_point EndTime;
	std::string Str;
	void reset(){
		Str.clear();
	}
	void operator()(char c){
		if(Str.size()==0){
			EndTime=std::chrono::system_clock::now();
			BeginTime=EndTime;
		}else{
			EndTime=std::chrono::system_clock::now();
		}
		Str.push_back(c);
	}
};
//全二重通信用ロガー(マルチスレッド対応済)
template<typename iologtype>
class fdx_crlf_timeout_iologger{
	typedef boost::signals2::signal<void(iologtype)> signal;
private:
	std::mutex Mx;
	std::chrono::milliseconds TimeOut;
	iologtype InLogger;
	int InCnt;
	std::chrono::system_clock::time_point InTime;
	iologtype OutLogger;
	int OutCnt;
	std::chrono::system_clock::time_point OutTime;
public:
	fdx_crlf_timeout_iologger():TimeOut(0),InCnt(-1),OutCnt(-1){
		InLogger.reset();
		OutLogger.reset();
	}
	void setTimeout(std::chrono::milliseconds TimeOut_){TimeOut=TimeOut_;}
	void operator()(void){
		std::unique_lock<std::mutex> Lock(Mx);
		std::chrono::system_clock::time_point Now=std::chrono::system_clock::now();
		if(InCnt!=0 && Now-InTime>TimeOut){
			signal_resetInLog(InLogger);
			InLogger.reset();
			InCnt=-1;
		}
		if(OutCnt!=0 && Now-OutTime>TimeOut){
			signal_resetOutLog(OutLogger);
			OutLogger.reset();
			OutCnt=-1;
		}
	}
	void operator()(bool io_,char c){
		std::unique_lock<std::mutex> Lock(Mx);
		if(io_){
			std::chrono::system_clock::time_point Now=std::chrono::system_clock::now();
			if(InCnt<0){
				InTime=Now;
				InCnt=0;
			}
			if(TimeOut>TimeOut.zero() && Now-InTime>TimeOut){
				signal_resetInLog(InLogger);
				InLogger.reset();
				InCnt=0;
				InTime=Now;
			}
			InLogger(c);
			if(InCnt==1 && c==0x0a){
				signal_resetInLog(InLogger);
				InLogger.reset();
				InCnt=-1;
			}else if (c==0x0d){
				++InCnt;
			}else{
				InCnt=0;
			}
		}else{
			std::chrono::system_clock::time_point Now=std::chrono::system_clock::now();
			if(OutCnt<0){
				OutTime=Now;
				OutCnt=0;
			}
			if(TimeOut>TimeOut.zero() && Now-OutTime>TimeOut){
				signal_resetOutLog(OutLogger);
				OutLogger.reset();
				OutCnt=0;
				OutTime=Now;
			}
			OutLogger(c);
			if(OutCnt==1 && c==0x0a){
				signal_resetOutLog(OutLogger);
				OutLogger.reset();
				OutCnt=-1;
			}else if (c==0x0d){
				++OutCnt;
			}else{
				OutCnt=0;
			}
		}
	}
public:
	signal signal_resetInLog;
	signal signal_resetOutLog;
	void contacInLog(hmLib::inquiries::inquiry<const iologtype&>& Inq_){
		hmLib::inquiries::connect(Inq_,[&](void)->const iologtype&{return this->InLogger;},Mx);
	}
	void contacOutLog(hmLib::inquiries::inquiry<const iologtype&>& Inq_){
		hmLib::inquiries::connect(Inq_,[&](void)->const iologtype&{return this->OutLogger;},Mx);
	}
};
//全二重通信用バッファ(マルチスレッド対応済)
template<typename iologtype>
struct fdx_vector_iologbuf{
private:
	typedef std::pair<bool,iologtype> io_iologtype;
	typedef boost::signals2::signal<void(iologtype)> signal;
public:
	typedef std::vector<io_iologtype> containor;
	typedef typename containor::const_iterator iterator;
private:
	std::mutex Mx;
	containor LogBuf;
	
private:
	hmLib::signals::unique_connections connections;

private:
	void push(bool IO_,iologtype Log_){
		std::unique_lock<std::mutex> Lock(Mx);
		LogBuf.push_back(std::make_pair(IO_,Log_));
	}
public:
	iterator begin()const{return LogBuf.begin();}
	iterator end()const{return LogBuf.end();}
public:

	void slot_pushInLogBuf(signal& Signal_){
		hmLib::signals::connect(Signal_,[&](iologtype Log)->void{this->push(true,Log);});
	}
	void slot_pushOutLogBuf(signal& Signal_){
		hmLib::signals::connect(Signal_,[&](iologtype Log)->void{this->push(false,Log);});
	}

	template<typename iterator_>
	void contactLogBuf(hmLib::inquiries::range_inquiry<iterator_>& RangeInq_){
		hmLib::inquiries::connect(RangeInq_,LogBuf);
	}

	// データ削除用スロット
	void slot_clearLogBuf(boost::signals2::signal<void(void)> &sig_){
		connections(sig_.connect([&]()->void{this->LogBuf.clear();}));
	}

	// Buf size 取得用inquirely
	void contactLogBufSize(hmLib::inquiries::inquiry<unsigned int>& BufSizeInq_){
		hmLib::inquiries::connect(BufSizeInq_,[&](void)->unsigned int{return this->LogBuf.size();});
	}

};
//全二重通信用二重バッファ(マルチスレッド対応済)
template<typename iologtype>
struct fdx_dualvector_iologbuf{
private:
	typedef boost::signals2::signal<void(iologtype)> signal;
	typedef std::vector<iologtype> containor;
	typedef typename containor::const_iterator iterator;
private:
	std::mutex Mx;
	containor InLogBuf;
	containor OutLogBuf;
private:
	void pushInLogBuf(iologtype Log_){
		std::unique_lock<std::mutex> Lock(Mx);
		InLogBuf.push_back(Log_);
	}
	void pushOutLogBuf(iologtype Log_){
		std::unique_lock<std::mutex> Lock(Mx);
		OutLogBuf.push_back(Log_);
	}

	// Buf 消去用のSLOT用
	void clearInLogBuf(){
		InLogBuf.clear();
	}
	void clearOutLogBuf(){
		OutLogBuf.clear();
	}

public:
	iterator beginInLog()const{return InLogBuf.begin();}
	iterator endInLog()const{return InLogBuf.end();}
	iterator beginOutLog()const{return OutLogBuf.begin();}
	iterator endOutLog()const{return OutLogBuf.end();}

private:
	hmLib::signals::unique_connections connections;
public:
	// データ挿入用スロット
	void slot_pushInLogBuf(signal& Signal_){
		hmLib::signals::connect(Signal_,[&](iologtype Log)->void{this->pushInLogBuf(Log);});
	}
	void slot_pushOutLogBuf(signal& Signal_){
		hmLib::signals::connect(Signal_,[&](iologtype Log)->void{this->pushOutLogBuf(Log);});
	}

	// データ削除用スロット
	void slot_clearInLogBuf(boost::signals2::signal<void(void)> &sig_){
		connections(sig_.connect([&]()->void{this->clearInLogBuf();}));
	}
	void slot_clearOutLogBuf(boost::signals2::signal<void(void)> &sig_){
		connections(sig_.connect([&]()->void{this->clearOutLogBuf();}));
	}

	template<typename iterator_>
	void contactInLogBuf(hmLib::inquiries::range_inquiry<iterator_>& RangeInq_){
		hmLib::inquiries::connect(RangeInq_,InLogBuf);
	}
	template<typename iterator_>
	void contactOutLogBuf(hmLib::inquiries::range_inquiry<iterator_>& RangeInq_){
		hmLib::inquiries::connect(RangeInq_,OutLogBuf);
	}

	// Buf size 取得用inquirely
	void contactInLogBufSize(hmLib::inquiries::inquiry<unsigned int>& InBufSizeInq_){
		hmLib::inquiries::connect(InBufSizeInq_,[&](void)->unsigned int{return this->InLogBuf.size();});
	}
	void contactOutLogBufSize(hmLib::inquiries::inquiry<unsigned int>& OutBufSizeInq_){
		hmLib::inquiries::connect(OutBufSizeInq_,[&](void)->unsigned int{return this->OutLogBuf.size();});
	}

};
#
#endif
