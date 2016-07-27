#ifndef HMR_GPSKASHMIR_INC
#define HMR_GPSKASHMIR_INC
#
#include<hmLib/comgate.hpp>
#include<hmLib/signals.hpp>
#include<hmLib/inquiries.hpp>
#include <HomuraViewer/Data.hpp>
#include"hmrGPSDataSet.hpp"
namespace hmr{
	class cGPSKashmir{
	private:
		//RS232通信用クラス
		hmLib::comgate RSGate;
	private:
		void sendGPGGA(const GPSDataSet& Data_){
			if(!is_connect())return;
			std::string GPGGA=Data_.strGPGGA();
			RSGate.puts(GPGGA.c_str(),GPGGA.size());
		}
	public:
		void connect(){
			if(is_connect())return;
			RSGate.open(0,9600);
		}
		void disconnect(){
			if(!is_connect())return;
			RSGate.close();
		}
		bool is_connect(){return RSGate.is_open();}
	private:
		hmLib::signals::unique_connections SignalConnections;
		hmLib::inquiries::unique_connections InquiryConnections;
	public:
		void slot_sendGPGGA(boost::signals2::signal<void(const GPSDataSet&, clock::time_point)>& signal_){
			SignalConnections(hmLib::signals::connect(signal_,[&](const GPSDataSet& Data_, clock::time_point t)->void{this->sendGPGGA(Data_);}));
		}
		void slot_connect(boost::signals2::signal<void(void)>& signal_){
			SignalConnections(hmLib::signals::connect(signal_,[&](void)->void{this->connect();}));
		}
		void slot_disconnect(boost::signals2::signal<void(void)>& signal_){
			SignalConnections(hmLib::signals::connect(signal_,[&](void)->void{this->disconnect();}));
		}
		void contact_is_connect(hmLib::inquiries::inquiry<bool>& inquiry_){
			InquiryConnections(hmLib::inquiries::connect(inquiry_,[&](void)->bool{return this->is_connect();}));
		}
	};
}
#
#endif
