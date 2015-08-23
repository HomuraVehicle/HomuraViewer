#ifndef HMR_DUMMYPICV1_INC
#define HMR_DUMMYPICV1_INC 100
#
/*===hmrDummyPic_v1===
Picの動作を疑似的に再現するプログラム
*/
#include<string>
#include<mutex>
#include<chrono>
#include<hmLib_v3_06/lab/virtual_com/virtual_comgate.hpp>
#include"hmrVMC1.h"
namespace hmr{
	//Picの動作を疑似的に再現するクラス
	class cDummmyPic_v1{
		typedef std::mutex mutex;
		typedef std::lock_guard<mutex> lock;
		typedef std::chrono::system_clock clock;
	private:
		clock::time_point Time;
		mutex Mx;
		bool RequestJoin;
		bool is_request_join(){
			lock Lock(Mx);
			return RequestJoin;
		}
	public:
		hmLib::vcom::virtual_comgate VComGate;
	public:
		void request_join(){
			lock Lock(Mx);
			RequestJoin=true;
		}
	public:
		dummy_plug_v1():RequestJoin(false),Time(clock::now()){
		}
		int operator()(void){
			std::string Str;
			Str.push_back('#');
			Str.push_back('c');
			Str.push_back('t');
			Str.push_back('r');
			
			Str.push_back('a');
			Str.push_back(0x07);
			Str.push_back(0x00);
			Str.push_back(0x00);
			Str.push_back(0x20);
			Str.push_back(0x00);
			Str.push_back(0x10);
			Str.push_back(0x00);
			Str.push_back(0x34);
			Str.push_back(0x12);
			
			Str.push_back('b');
			Str.push_back(0x07);
			Str.push_back(0x00);
			Str.push_back(0x00);
			Str.push_back(0x10);
			Str.push_back(0x12);
			Str.push_back(0xff);
			Str.push_back(0x01);
			Str.push_back(0xae);
			Str.push_back(0x00);
			
			Str.push_back('c');
			Str.push_back(0x07);
			Str.push_back(0x00);
			Str.push_back(0x00);
			Str.push_back(0xa0);
			Str.push_back(0x00);
			Str.push_back(0xa0);
			Str.push_back(0x00);
			Str.push_back(0xa0);
			Str.push_back(0x00);
			
			Str.push_back('g');
			Str.push_back(0x15);
			Str.push_back(0x00);
			Str.push_back(0x00);
			Str.push_back(0x01);
			Str.push_back(0x17);
			Str.push_back(0x2d);
			Str.push_back(0x43);
			Str.push_back(0x59);
			Str.push_back(0x26);
			Str.push_back(0x00);
			Str.push_back(0x00);
			Str.push_back(0x00);
			Str.push_back('N');
			Str.push_back(0x87);
			Str.push_back(0x00);
			Str.push_back(0x00);
			Str.push_back(0x00);
			Str.push_back('E');
			Str.push_back(0x01);
			Str.push_back(0x03);
			Str.push_back(0x4d);
			Str.push_back(0x3c);
			Str.push_back(0x00);
			
			Str.push_back('G');
			Str.push_back(0x10);
			Str.push_back(0x00);			
			Str.push_back(0x00);
			Str.push_back(0x00);
			Str.push_back(0x10);
			Str.push_back(0x00);
			Str.push_back(0x10);			
			Str.push_back(0x00);			
			Str.push_back(0x00);			
			Str.push_back(0x00);			
			Str.push_back(0x00);			
			Str.push_back(0x10);			
			Str.push_back(0x00);			
			Str.push_back(0x00);			
			Str.push_back(0x00);			
			Str.push_back(0x00);			
			Str.push_back(0x10);			
			Str.push_back(0x00);			
			
			Str.push_back('t');			
			Str.push_back(0x03);			
			Str.push_back(0x00);			
			Str.push_back(0x00);	
			Str.push_back(0xF4);			
			Str.push_back(0x01);			

			Str.push_back('#');
			Str.push_back('#');
			Str.push_back(0x0a);
			Str.push_back(0x0d);

			while(!is_request_join()){
				clock::time_point Now=clock::now();
				while(!VComGate.empty())VComGate.get(c,1);
				if(Now-Time>std::chrono::milleseconds(100)){
					char c;
					VComGate.put(Str.c_str(),Str.size());
					Time=clock::now();
				}
			}
			return 0;
		}
	};
}
#
#endif
