#ifndef HMR_VIEWER_IO_DXIODRIVERSUI_INC
#define HMR_VIEWER_IO_DXIODRIVERSUI_INC 100

#include <iostream>
#include <string>
#include "hmLibVer.hpp"
#include <hmLib_v2/hmBasic.hpp>
#include <hmLib_v2/dxObject.hpp>
#include <hmLib_v2/dxArea.hpp>
#include <hmLib/signals.hpp>
#include <boost/signals2.hpp>
#include <HomuraViewer/View/DxBUI.hpp>

/* ---- hmrDxIOSUI-----

hmrDxIOSUI.hpp :v1_00/130615 amby
	DxIOSUIの製作開始
*/

namespace hmr{
	namespace viewer{
		namespace io{
			// 描画クラス static
			class dxosIODriverSUI :public dxosBUI{
			private:
				//unsigned int inBufSize, outBufSize;

			public:
				// inquire 系列の定義
				hmLib::inquiries::inquiry<bool> isOpenInq;
				hmLib::inquiries::inquiry<bool> canGetInq;
				hmLib::inquiries::inquiry<bool> canSendInq;

			public:
				//virtual int fndraw(dxO& dxf);
				int normal_draw(dxO& dxo)override{
					//int ans;

					/*
					// 自分自身を名乗って状態表示
					dxo.draw(Pint(5,5),dxoStrP(Pint(70,20),"IO",getClr(normal,strobj)));

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
							dxo.draw(Pint(5, 5), dxoStrP(Pint(70, 20), "Opened", getClr(normal, strobj)));
						} else{
							dxo.draw(Pint(5, 5), dxoStrP(Pint(70, 20), "Closed", getClr(invalid, strobj)));
						}
					}
					catch(const hmLib::inquiries::unconnected_exception&){
						// Inquire にアクセスできなかった！！　でも、そんなのメイン関数には関係ないからもみ消す
						dxo.draw(Pint(5, 5), dxoStr("Inquiry::isOpen 取得失敗"));
					}
					catch(const hmLib::exceptions::exception& Excp){
						dxo.draw(Pint(5, 5), dxoStr(Excp.what()));
					}
					// can send
					try{
						if(canSendInq()){
							dxo.draw(Pint(80, 5), dxoStrP(Pint(70, 20), "送信可", getClr(normal, strobj)));
						} else{
							dxo.draw(Pint(80, 5), dxoStrP(Pint(70, 20), "送信不可", getClr(invalid, strobj)));
						}
					}
					catch(const hmLib::inquiries::unconnected_exception&){
						dxo.draw(Pint(80, 5), dxoStr("Inquiry::canSend 取得失敗"));
					}
					catch(const hmLib::exceptions::exception& Excp){
						dxo.draw(Pint(80, 5), dxoStr(Excp.what()));
					}
					// can recv
					try{
						if(canGetInq()){
							dxo.draw(Pint(155, 5), dxoStrP(Pint(70, 20), "受信可", getClr(normal, strobj)));
						} else{
							dxo.draw(Pint(155, 5), dxoStrP(Pint(70, 20), "受信不可", getClr(invalid, strobj)));
						}
					}
					catch(const hmLib::inquiries::unconnected_exception&){
						dxo.draw(Pint(155, 5), dxoStr("Inquiry::canGet 取得失敗"));
					}
					catch(const hmLib::exceptions::exception& Excp){
						dxo.draw(Pint(155, 5), dxoStr(Excp.what()));
					}

					return 0;
				}

				int extend_draw(dxO& dxo)override{
					// draw
					normal_draw(dxo);

					/*
					// can get 系列の描画
					// is Open
					try{
					if(isOpenInq()){
					dxo.draw(Pint(5,30),dxoStrP(Pint(70,20),"Opened",getClr(normal,strobj)));
					}else{
					dxo.draw(Pint(5,30),dxoStrP(Pint(70,20),"Closed",getClr(invalid,strobj)));
					}
					}catch(const hmLib::inquiries::unconnected_exception& ){
					// Inquire にアクセスできなかった！！　でも、そんなのメイン関数には関係ないからもみ消す
					dxo.draw(Pint(5, 30), dxoStr("Inquiry::isOpen 取得失敗") );
					}catch(const hmLib::exceptions::exception& Excp){
					dxo.draw(Pint(5, 30), dxoStr( Excp.what()) );
					}
					// can send
					try{
					if(canSendInq()){
					dxo.draw(Pint(80,30),dxoStrP(Pint(70,20),"送信可",getClr(normal,strobj)));
					}else{
					dxo.draw(Pint(80,30),dxoStrP(Pint(70,20),"送信不可",getClr(invalid,strobj)));
					}
					}catch(const hmLib::inquiries::unconnected_exception& ){
					dxo.draw(Pint(80, 30), dxoStr("Inquiry::canSend 取得失敗") );
					}catch(const hmLib::exceptions::exception& Excp){
					dxo.draw(Pint(80, 30), dxoStr( Excp.what()) );
					}
					// can recv
					try{
					if(canGetInq()){
					dxo.draw(Pint(155,30),dxoStrP(Pint(70,20),"受信可",getClr(normal,strobj)));
					}else{
					dxo.draw(Pint(155,30),dxoStrP(Pint(70,20),"受信不可",getClr(invalid,strobj)));
					}
					}catch(const hmLib::inquiries::unconnected_exception& ){
					dxo.draw(Pint(155, 30), dxoStr("Inquiry::canGet 取得失敗") );
					}catch(const hmLib::exceptions::exception& Excp){
					dxo.draw(Pint(155, 30), dxoStr( Excp.what()) );
					}
					*/

					return 0;

				}

				status getStatus()const override{
					try{
						if(isOpenInq()){
							return normal;
						} else{
							return invalid;
						}
					}
					catch(const hmLib::exceptions::exception&){
						return error;
					}
				}

			public:
				// constructor
				dxosIODriverSUI(void) :dxosBUI("IO", 30, 30){}

			};
		}
	}
}

#
#endif
