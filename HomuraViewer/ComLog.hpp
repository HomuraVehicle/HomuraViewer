#ifndef HMR_COMLOG_INC
#define HMR_COMLOG_INC 100
#
/*===hmrComLog===
Comの送受信ログ保存用クラス

hmrComLog v1_00/130410 hmIto
	1バッファ方式のcComLogを追加
*/
#include<deque>
#include<mutex>
#include<boost/signals2.hpp>
#include<hmLib/signals.hpp>
#include<hmLib/inquiries.hpp>
#include"hmrData.hpp"
namespace hmr{
	class cComLog{
		typedef std::deque<std::pair<bool,hmr::packet>> containor;
	public:
		typedef containor::const_iterator iterator;
	private:
		std::mutex Mx;
		hmr::packet InPacket;
		containor LogBuf;
	public://private:
		void push(bool IO_,const packet& Log_){
			std::unique_lock<std::mutex> Lock(Mx);
			LogBuf.push_back(std::make_pair(IO_,Log_));
		}
	public:
		iterator begin()const{return LogBuf.begin();}
		iterator end()const{return LogBuf.end();}
	private:
		hmLib::signals::unique_connections SignalConnections;
		hmLib::inquiries::unique_connections InquiryConnections;
	public:
		void slot_pushInLogBuf(boost::signals2::signal<void(const packet&)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](const packet& Log)->void{this->push(true,Log);}));
		}
		void slot_pushOutLogBuf(boost::signals2::signal<void(const packet&)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](const packet& Log)->void{this->push(false,Log);}));
		}
		template<typename iterator_>
		void contactLogBuf(hmLib::inquiries::range_inquiry<iterator_>& RangeInq_){
			InquiryConnections(hmLib::inquiries::connect(RangeInq_,LogBuf));
		}
	};
}
#
#endif
