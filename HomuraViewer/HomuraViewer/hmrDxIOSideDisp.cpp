#ifndef HMR_DXIOSIDEDISP_INC
#define HMR_DXIOSIDEDISP_INC 100
#
/*===hmrDxIOSideDisp===
通信関連のステータス/設定を表示する
*/
#include <hmLib/config_vc.h> // std::min, std::max　は　windowsマクロでも定義されており、明示的に分離してやらないとだめ

#include <iostream>
#include <string>
#include <vector>

#include "hmLibVer.hpp"

#include <hmLib_v2/hmBasic.hpp>
#include <hmLib_v2/dxObject.hpp>
#include <hmLib_v2/dxArea.hpp>
#include <hmLib_v2/dxColorSet.hpp>

#include "hmrDxGateSwitcher.hpp"
#include "hmrDxIO.hpp"

/* === hmrDxIODisplay.hpp ===   

IO関連の描画用のページを作る

hmrDxIODisplay:v1_00/130602 amby

============================= */

namespace hmr{
	namespace viewer{
		// Sub Page
		class dxosIOSideDisp :public dxReObject, public hmoBox{
		private:
			dxRGB Clr;

		public:
			// Gait switcher 
			dxosGateSwitcher DxosGateSwitcher;

			// 描画のための要素を定義
			dxosStartControl DxSendStartButt;
			dxosBufControl DxReadBufCtr;
			dxosBufControl DxSendBufCtr;
			dxosTimeIntervalControl DxSendIntervalCtr;
			dxosTimeOutControl DxSendTimeOutCtr;
			dxosSyncControl DxSyncCheckBox;
			dxosSelectDuplex DxSelectDuplex;

		public:
			// constructor
			dxosIOSideDisp(Pint Size_, const dxColor& Clr_)
				:hmoBox(Size_)
				, Clr(Clr_)
				, DxosGateSwitcher(CLR::DeepSoftOrenge)
				, DxSendStartButt(Pint(220, 40), Clr, "Send Start")
				, DxReadBufCtr(Pint(200, 30), Clr, "Read Buf Clear")
				, DxSendBufCtr(Pint(200, 30), Clr, "Send Buf Clear")
				, DxSendIntervalCtr(Pint(200, 30), Clr, "Interval")
				, DxSendTimeOutCtr(Pint(200, 30), Clr, "TimeOut")
				, DxSyncCheckBox(Pint(200, 20), Clr)
				, DxSelectDuplex(Pint(220, 20), Clr){}

		protected:
			virtual int redraw(dxO& dxo){
				dxo.draw(Pint(0, 0), DxosGateSwitcher);

				dxo.draw(Pint(10, 300), DxSendStartButt);
				dxo.draw(Pint(10, 350), DxSelectDuplex);

				dxo.draw(Pint(30, 420), DxSendIntervalCtr);
				dxo.draw(Pint(30, 380), DxSendTimeOutCtr);
				dxo.draw(Pint(30, 455), DxSyncCheckBox);


				dxo.draw(Pint(30, 500), DxSendBufCtr);
				dxo.draw(Pint(30, 540), DxReadBufCtr);






				return 0;
			}
		};
	}

}
#
#endif


