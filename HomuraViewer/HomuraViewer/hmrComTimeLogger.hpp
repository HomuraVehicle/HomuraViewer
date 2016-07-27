#ifndef HMR_COMTIMELOGGER_INC
#define HMR_COMTIMELOGGER_INC
#
#include<deque>
#include<HomuraViewer/chrono.hpp>
#include<HomuraViewer/Message/PacketData.hpp>
namespace hmr{
	class cComTimeLogger{
		using packet = message::packet;
	private:
		typedef std::deque<clock::time_point> deque;
	private:
		clock::duration RecvInterval;
		clock::duration SendInterval;
		clock::time_point RecvTime;
		clock::time_point SendTime;
	private:
		void recv(clock::time_point Time_){
			RecvInterval=Time_-RecvTime;
			RecvTime=Time_;
		}
		void send(clock::time_point Time_){
			SendInterval=Time_-SendTime;
			SendTime=Time_;
		}
	public:
		clock::duration recvTime
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
