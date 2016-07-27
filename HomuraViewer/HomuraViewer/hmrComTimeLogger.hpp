#ifndef HMR_COMTIMELOGGER_INC
#define HMR_COMTIMELOGGER_INC
#
#include<deque>
#include <HomuraViewer/Data.hpp>
namespace hmr{
	class cComTimeLogger{
	private:
		typedef std::deque<clock::time_point> deque;
	private:
		hmr::clock::duration RecvInterval;
		hmr::clock::duration SendInterval;
		hmr::clock::time_point RecvTime;
		hmr::clock::time_point SendTime;
	private:
		void recv(hmr::clock::time_point Time_){
			RecvInterval=Time_-RecvTime;
			RecvTime=Time_;
		}
		void send(hmr::clock::time_point Time_){
			SendInterval=Time_-SendTime;
			SendTime=Time_;
		}
	public:
		hmr::clock::duration recvTime
	public:
		void slot_pushInLogBuf(boost::signals2::signal<void(const packet&)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](const packet& Log)->void{this->push(true,Log);}));
		}
		void slot_pushOutLogBuf(boost::signals2::signal<void(const packet&)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](const packet& Log)->void{this->push(false,Log);}));
		}
	};
}
#
#endif
