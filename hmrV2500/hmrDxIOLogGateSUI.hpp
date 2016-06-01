#ifndef HMR_DXIOLOGGATE_SUI_INC
#define HMR_DXIOLOGGATE_SUI_INC 100

#include <iostream>
#include <string>
#include "hmLibVer.hpp"
#include <hmLib_v2/hmBasic.hpp>
#include <hmLib_v2/dxObject.hpp>
#include <hmLib_v2/dxArea.hpp>
#include <hmLib_v3_06/signals.hpp>
#include <hmLib_v3_06/inquiries/inquiry.hpp>
#include <boost/signals2.hpp>
#include "hmrDxBUI.hpp"
//#include "hmrBaseDX.hpp"

//#define DEBUG_MODE 1

/* ---- hmrDxBufGateSUI-----

hmrDxIOLogGateSUI.hpp :v1_00/130615 amby
	DxIOLogGateSUIの製作開始
*/


namespace hmr{
	// 描画クラス static
	class dxosIOLogGateSUI:public dxosBUI{
	private:
		//unsigned int inBufSize, outBufSize;

	public:
		// signal 系列の定義
		boost::signals2::signal<void(void)> signal_require_clearBuf;
		//boost::signals2::signal<void(void)> signal_require_clearOutBuf;

		// inquire 系列の定義
		hmLib::inquiries::inquiry<unsigned int> BufSizeInq;
		//hmLib::inquiries::inquiry<unsigned int> outBufSizeInq;
	
		hmLib::inquiries::inquiry<bool> isOpenInq;
		hmLib::inquiries::inquiry<bool> canGetInq;
		hmLib::inquiries::inquiry<bool> canSendInq;

	public:
		//virtual int fndraw(dxO& dxf);
		int normal_draw(dxO& dxo)override{
			//int ans;

			/*
			// 自分自身を名乗って状態表示
			dxo.draw(Pint(5,5),dxoStrP(Pint(70,20),"IOLogGate",getClr(normal,strobj)));
			
			// is Open 状態表示
			try{
				if(isOpenInq()){
					dxo.draw(Pint(80,5),dxoStrP(Pint(140,20),"Opened",getClr(normal,strobj)));
				}else{
					dxo.draw(Pint(80,5),dxoStrP(Pint(140,20),"Closed",getClr(invalid,strobj)));
				}
			}catch(const hmLib::inquiries::unconnected_exception&){				
				dxo.draw(Pint(80, 5), dxoStr("Inquiry::isOpen 取得失敗") );
			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(80, 5), dxoStr( Excp.what()) );
			}
			*/
			// can get 系列の描画
			
			// is Open
			try{
				if(isOpenInq()){
					dxo.draw(Pint(5,5),dxoStrP(Pint(70,20),"Opened",getClr(normal,strobj)));
				}else{
					dxo.draw(Pint(5,5),dxoStrP(Pint(70,20),"Closed",getClr(invalid,strobj)));
				}
			}catch(const hmLib::inquiries::unconnected_exception& ){				
				// Inquire にアクセスできなかった！！　でも、そんなのメイン関数には関係ないからもみ消す
				dxo.draw(Pint(5, 5), dxoStr("Inquiry::isOpen 取得失敗") );
			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(5, 5), dxoStr(Excp.what()) );
			}
			// buf size
			try{
				dxo.draw(Pint(120, 5),dxoStrP(Pint(30,20),"Buf:", getClr(normal,strobj)));
				dxo.draw(Pint(155, 5),dxoStrP(Pint(70,20),hmstd::fStr("%d", BufSizeInq()), getClr(normal,strobj)));
			}catch(const hmLib::inquiries::unconnected_exception&){				
				dxo.draw(Pint(80, 5), dxoStr("Inquiry::BufSize 取得失敗") );
			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(80, 5), dxoStr( Excp.what()) );
			}

			return 0;
		}

		int extend_draw(dxO& dxo)override{
			// draw
			//normal_draw(dxo);

			// can get 系列の描画
			 // is Open
			try{
				if(isOpenInq()){
					dxo.draw(Pint(5,5),dxoStrP(Pint(70,20),"Opened",getClr(normal,strobj)));
				}else{
					dxo.draw(Pint(5,5),dxoStrP(Pint(70,20),"Closed",getClr(invalid,strobj)));
				}
			}catch(const hmLib::inquiries::unconnected_exception& ){				
				// Inquire にアクセスできなかった！！　でも、そんなのメイン関数には関係ないからもみ消す
				dxo.draw(Pint(5, 5), dxoStr("Inquiry::isOpen 取得失敗") );
			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(5, 5), dxoStr(Excp.what()) );
			}
			 // can send
			try{
				if(canSendInq()){
					dxo.draw(Pint(80,5),dxoStrP(Pint(70,20),"送信可",getClr(normal,strobj)));
				}else{
					dxo.draw(Pint(80,5),dxoStrP(Pint(70,20),"送信不可",getClr(invalid,strobj)));
				}
			}catch(const hmLib::inquiries::unconnected_exception& ){				
				dxo.draw(Pint(80, 5), dxoStr("Inquiry::canSend 取得失敗") );
			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(80, 5), dxoStr( Excp.what()) );
			}
			 // can recv
			try{
				if(canGetInq()){
					dxo.draw(Pint(155,5),dxoStrP(Pint(70,20),"受信可",getClr(normal,strobj)));
				}else{
					dxo.draw(Pint(155,5),dxoStrP(Pint(70,20),"受信不可",getClr(invalid,strobj)));
				}
			}catch(const hmLib::inquiries::unconnected_exception& ){				
				dxo.draw(Pint(155, 5), dxoStr("Inquiry::canGet 取得失敗") );
			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(155, 5), dxoStr( Excp.what()) );
			}


			// inBuf Size についての状態表示
			try{
				dxo.draw(Pint(5, 30),dxoStrP(Pint(70,20),"Buf", getClr(normal,strobj)));
				dxo.draw(Pint(80, 30),dxoStrP(Pint(70,20),hmstd::fStr("%d", BufSizeInq()), getClr(normal,strobj)));
				if( dxo.draw(Pint(155,30),dxoBut(Pint(70,20),"CLR", getClr(normal,strobj))) == 1){
					signal_require_clearBuf();
				}
			}catch(const hmLib::inquiries::unconnected_exception&){				
				dxo.draw(Pint(5, 30), dxoStr("Inquiry::BufSize 取得失敗") );
			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(5, 30), dxoStr( Excp.what()) );
			}

			/*
			// outBuf Size についての状態表示
			try{
				dxo.draw(Pint(5, 55),dxoStrP(Pint(70,20),"OutBuf", getClr(normal,strobj)));
				dxo.draw(Pint(80, 55),dxoStrP(Pint(70,20),hmstd::fStr("%d", outBufSizeInq()), getClr(normal,strobj)));
				if( dxo.draw(Pint(155, 55),dxoBut(Pint(70,20),"CLR", getClr(normal,strobj))) == 1){
					signal_require_clearOutBuf();
				}
			}catch(const hmLib::inquiries::unconnected_exception&){				
				dxo.draw(Pint(5, 55), dxoStr("Inquiry::outBufSize 取得失敗") );
			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(5, 55), dxoStr( Excp.what()) );
			}
			*/


			
			return 0;

		}

		status getStatus()const override{
			try{
				if(isOpenInq()){
					return normal;
				}else{
					return invalid;
				}
			}catch(const hmLib::exceptions::exception& ){
				return error;
			}
		}

	public:
		// constructor
		dxosIOLogGateSUI(void):dxosBUI("IOLogGate", 30, 55){}
	
	};		
}

#
#endif
