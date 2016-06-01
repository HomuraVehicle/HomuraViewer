#ifndef HMR_DXAUTOLOG_INC
#define HMR_DXAUTOLOG_INC 100
#
#include<string>
#include<boost/format.hpp>
#include"hmLibVer.hpp"
#include<hmLib_v2/dxObject.hpp>
#include<hmLib_v2/dxColorSet.hpp>
#include<hmLib/signals.hpp>
#include<hmLib/inquiries.hpp>
#include<hmrData.hpp>
namespace hmr {
	class dxosAutolog_480_360 : public dxReObject, public hmoBox {
	public:
		boost::signals2::signal<void(bool)> signal_setStart;
		hmLib::inquiries::inquiry<bool> inquiry_isStart;
		hmLib::inquiries::inquiry<bool> inquiry_isStartAutolog;

		boost::signals2::signal<void(unsigned int)> signal_setObserveStartPoint;
		hmLib::inquiries::inquiry<unsigned int> inquiry_getObserveStartPoint;

		boost::signals2::signal<void(unsigned int)> signal_setObserveInteraval;
		hmLib::inquiries::inquiry<unsigned int> inquiry_getObserveInterval;

		boost::signals2::signal<void(unsigned int)> signal_setPreObserveLength;
		hmLib::inquiries::inquiry<unsigned int> inquiry_getPreObserveLength;

		hmLib::inquiries::inquiry<std::string> inquiry_getStatusStr;
		hmLib::inquiries::inquiry<hmr::clock::time_point> inquiry_getNextAwake;
		hmLib::inquiries::inquiry<hmr::clock::time_point> inquiry_getNextObserve;

		boost::signals2::signal<void(bool)> signal_startThermo;
		hmLib::inquiries::inquiry<bool> inquiry_is_startThermo;

		boost::signals2::signal<void(bool)> signal_startBattery;
		hmLib::inquiries::inquiry<bool> inquiry_is_startBattery;

		boost::signals2::signal<void(bool)> signal_startCO2;
		hmLib::inquiries::inquiry<bool> inquiry_is_startCO2;

		boost::signals2::signal<void(bool)> signal_startSprite;
		hmLib::inquiries::inquiry<bool> inquiry_is_startSprite;
	public:
		dxosAutolog_480_360()
			:hmoBox(Pint(480,360)){
		}
		int redraw(dxO& dxo)override{

			try {
				if(dxo.draw(Pint(5, 5), dxoButIO(Pint(80, 30), "自動観測", (inquiry_isStart()==inquiry_isStartAutolog()?CLR::Blue:CLR::DeepGreen), inquiry_isStart()))==1) {
					signal_setStart(!inquiry_isStart());
				}
			} catch(const hmLib::inquiries::unconnected_exception& ){				
				// Inquire にアクセスできなかった！！　でも、そんなのメイン関数には関係ないからもみ消す
				dxo.draw(Pint(5, 5), dxoStrP(Pint(80,30),"No Connect") );
			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(5, 5), dxoStrP(Pint(80,30), Excp.what()) );
			}

			try{
				dxo.draw(Pint(90,5),dxoStrP(Pint(70,30),inquiry_getStatusStr(),CLR::White,ALI::left));
			}catch(const hmLib::inquiries::unconnected_exception& ){				
				// Inquire にアクセスできなかった！！　でも、そんなのメイン関数には関係ないからもみ消す
				dxo.draw(Pint(90, 5), dxoStrP(Pint(70,30),"No Connect",CLR::White,ALI::left) );
			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(90, 5), dxoStrP(Pint(70,30), Excp.what(),CLR::White,ALI::left) );
			}

			try{
				dxo.draw(Pint(5, 40), dxoStrP(Pint(70, 30), "現在:", CLR::White, ALI::left));
				dxo.draw(Pint(80, 40), dxoStrP(Pint(70, 30),hmr::time_to_ymdhms(hmr::clock::now()),CLR::White,ALI::left));
			}catch(const hmLib::inquiries::unconnected_exception& ){				
				// Inquire にアクセスできなかった！！　でも、そんなのメイン関数には関係ないからもみ消す
				dxo.draw(Pint(5, 40), dxoStrP(Pint(70,30),"No Connect",CLR::White,ALI::left) );
			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(5, 40), dxoStrP(Pint(70,30), Excp.what(),CLR::White,ALI::left) );
			}

			try{
				dxo.draw(Pint(5, 75), dxoStrP(Pint(70, 30), "起床:", CLR::White, ALI::left));
				dxo.draw(Pint(80,75),dxoStrP(Pint(70,30),hmr::time_to_ymdhms(inquiry_getNextAwake()),CLR::White,ALI::left));
			}catch(const hmLib::inquiries::unconnected_exception& ){				
				// Inquire にアクセスできなかった！！　でも、そんなのメイン関数には関係ないからもみ消す
				dxo.draw(Pint(80, 75), dxoStrP(Pint(70,30),"No Connect",CLR::White,ALI::left) );
			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(80, 75), dxoStrP(Pint(70,30), Excp.what(),CLR::White,ALI::left) );
			}

			try{
				dxo.draw(Pint(5, 110), dxoStrP(Pint(70, 30), "観測:", CLR::White, ALI::left));
				dxo.draw(Pint(80,110),dxoStrP(Pint(70,30),hmr::time_to_ymdhms(inquiry_getNextObserve()),CLR::White,ALI::left));
			}catch(const hmLib::inquiries::unconnected_exception& ){				
				// Inquire にアクセスできなかった！！　でも、そんなのメイン関数には関係ないからもみ消す
				dxo.draw(Pint(80, 110), dxoStrP(Pint(70,30),"No Connect",CLR::White,ALI::left) );
			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(80, 110), dxoStrP(Pint(70,30), Excp.what(),CLR::White,ALI::left) );
			}


			try {
				if(dxo.draw(Pint(5, 155), dxoButIO(Pint(70, 30), "Thermo",CLR::Blue, inquiry_is_startThermo()))==1) {
					signal_startThermo(!inquiry_is_startThermo());
				}
			} catch(const hmLib::inquiries::unconnected_exception& ){				
				// Inquire にアクセスできなかった！！　でも、そんなのメイン関数には関係ないからもみ消す
				dxo.draw(Pint(5, 155), dxoStrP(Pint(70,30),"No Connect") );
			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(5, 155), dxoStrP(Pint(70,30), Excp.what()) );
			}
			try {
				if(dxo.draw(Pint(80, 155), dxoButIO(Pint(70, 30), "Battery",CLR::Blue, inquiry_is_startBattery()))==1) {
					signal_startBattery(!inquiry_is_startBattery());
				}
			} catch(const hmLib::inquiries::unconnected_exception& ){				
				// Inquire にアクセスできなかった！！　でも、そんなのメイン関数には関係ないからもみ消す
				dxo.draw(Pint(80, 155), dxoStrP(Pint(70,30),"No Connect") );
			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(80, 155), dxoStrP(Pint(70,30), Excp.what()) );
			}
			try {
				if(dxo.draw(Pint(155, 155), dxoButIO(Pint(70, 30), "CO2",CLR::Blue, inquiry_is_startCO2()))==1) {
					signal_startCO2(!inquiry_is_startCO2());
				}
			} catch(const hmLib::inquiries::unconnected_exception& ){				
				// Inquire にアクセスできなかった！！　でも、そんなのメイン関数には関係ないからもみ消す
				dxo.draw(Pint(155, 155), dxoStrP(Pint(70,30),"No Connect") );
			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(155, 155), dxoStrP(Pint(70,30), Excp.what()) );
			}
			try {
				if(dxo.draw(Pint(230, 155), dxoButIO(Pint(70, 30), "Sprite",CLR::Blue, inquiry_is_startSprite()))==1) {
					signal_startSprite(!inquiry_is_startSprite());
				}
			} catch(const hmLib::inquiries::unconnected_exception& ){				
				// Inquire にアクセスできなかった！！　でも、そんなのメイン関数には関係ないからもみ消す
				dxo.draw(Pint(230, 155), dxoStrP(Pint(70,30),"No Connect") );
			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(230, 155), dxoStrP(Pint(70,30), Excp.what()) );
			}


			try {
				int Min=inquiry_getObserveStartPoint();
				Pint TabPos(Min/5, 0);
				dxo.draw(Pint(5, 190), dxoStrP(Pint(145,30),(boost::format("観測起点:% 3dmin")%Min ).str(), CLR::White, ALI::left));
				dxo.draw(Pint(155, 195), dxoSlider(Pint(320, 20), Pint(20, 20), TabPos, Pint(72, 0), CLR::Blue));
				signal_setObserveStartPoint(static_cast<unsigned int>(TabPos.x*5));
			} catch(const hmLib::inquiries::unconnected_exception& ){				
				// Inquire にアクセスできなかった！！　でも、そんなのメイン関数には関係ないからもみ消す
				dxo.draw(Pint(5, 190), dxoStrP(Pint(470,30),"No Connect", CLR::White, ALI::left) );
			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(5, 190), dxoStrP(Pint(470,30), Excp.what(), CLR::White, ALI::left) );
			}

			try {
				int Min=inquiry_getObserveInterval();
				Pint TabPos(Min/5, 0);
				dxo.draw(Pint(5, 225), dxoStrP(Pint(145,30),(boost::format("観測間隔:% 3dmin")%Min ).str(), CLR::White, ALI::left));
				dxo.draw(Pint(155, 230), dxoSlider(Pint(320, 20), Pint(20, 20), TabPos, Pint(72, 0), CLR::Blue));
				signal_setObserveInteraval(static_cast<unsigned int>(TabPos.x*5));
			} catch(const hmLib::inquiries::unconnected_exception& ){				
				// Inquire にアクセスできなかった！！　でも、そんなのメイン関数には関係ないからもみ消す
				dxo.draw(Pint(5, 225), dxoStrP(Pint(470,30),"No Connect", CLR::White, ALI::left) );
			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(5, 225), dxoStrP(Pint(470,30), Excp.what(), CLR::White, ALI::left) );
			}

			try {
				int Min=inquiry_getPreObserveLength();
				Pint TabPos(Min, 0);
				dxo.draw(Pint(5, 260), dxoStrP(Pint(145,30),(boost::format("準備期間:% 3dmin")%Min ).str(), CLR::White, ALI::left));
				dxo.draw(Pint(155, 265), dxoSlider(Pint(320, 20), Pint(20, 20), TabPos, Pint(30, 0), CLR::Blue));
				signal_setPreObserveLength(static_cast<unsigned int>(TabPos.x));
			} catch(const hmLib::inquiries::unconnected_exception& ){				
				// Inquire にアクセスできなかった！！　でも、そんなのメイン関数には関係ないからもみ消す
				dxo.draw(Pint(5, 260), dxoStrP(Pint(470,30),"No Connect", CLR::White, ALI::left) );
			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(5, 260), dxoStrP(Pint(470,30), Excp.what(), CLR::White, ALI::left) );
			}

			try {
				hmr::clock::time_point ObserveStartPoint=hmr::date(2000, 1, 1, 0, 0, 0, 0).to_time() + std::chrono::minutes(inquiry_getObserveStartPoint());
				hmr::clock::duration ObserveInteraval=std::chrono::minutes(inquiry_getObserveInterval());
				hmr::clock::duration PreObserveLength=std::chrono::minutes(inquiry_getPreObserveLength());

				hmr::clock::time_point NextObserve = ObserveStartPoint 
					+ ((std::chrono::duration_cast<std::chrono::seconds>(hmr::clock::now()-ObserveStartPoint).count()/std::chrono::duration_cast<std::chrono::seconds>(ObserveInteraval).count()) + 1)*ObserveInteraval;
				hmr::clock::time_point NextAwake = NextObserve - PreObserveLength;


				dxo.draw(Pint(5, 295), dxoStrP(Pint(70, 30), "起床予定:", CLR::White, ALI::left));
				dxo.draw(Pint(80,295),dxoStrP(Pint(70,30),hmr::time_to_ymdhms(NextAwake),CLR::White,ALI::left));

				dxo.draw(Pint(5, 330), dxoStrP(Pint(70, 30), "観測予定:", CLR::White, ALI::left));
				dxo.draw(Pint(80,330),dxoStrP(Pint(70,30),hmr::time_to_ymdhms(NextObserve),CLR::White,ALI::left));
			} catch(const hmLib::inquiries::unconnected_exception& ){				
				// Inquire にアクセスできなかった！！　でも、そんなのメイン関数には関係ないからもみ消す
				dxo.draw(Pint(5, 295), dxoStrP(Pint(470,30),"No Connect", CLR::White, ALI::left) );
			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(5, 295), dxoStrP(Pint(470,30), Excp.what(), CLR::White, ALI::left) );
			}

			return 0;
		}
	};
}
#
#endif
