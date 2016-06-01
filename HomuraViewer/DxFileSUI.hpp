#ifndef HMR_DXFILE_SUI_INC
#define HMR_DXFILE_SUI_INC 100

#include <iostream>
#include <string>
#include "hmLibVer.hpp"
#include <hmLib_v2/hmBasic.hpp>
#include <hmLib_v2/dxObject.hpp>
#include <hmLib_v2/dxArea.hpp>
#include <hmLib/signals.hpp>
#include <boost/signals2.hpp>
#include "hmrDxBUI.hpp"
//#include "hmrBaseDX.hpp"

//#define DEBUG_MODE 1

/* ---- hmrDx FILE SUI -----

単純にファイルにログをとるかどうかを決定する選択しを与えるUI

hmrDxFileSUI.hpp :v1_00/130914 amby
	DxFILESUIの製作開始
*/

namespace hmr{
	// 描画クラス static
	class dxosFileSUI:public dxosBUI{
	private:
		hmLib::signals::unique_connections SignalConnections;
		bool isActive;

	// signal, slot 例列
	public:
		// signal 系列の定義
		boost::signals2::signal<void(const std::string&)> signal_require_activate;
		boost::signals2::signal<void(void)> signal_require_inactivate;

		// slot 
		void setIsActive(bool flag){
			isActive = flag;
		}
		void slot_setIsActive(boost::signals2::signal<void(bool)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](bool flag_)->void{this->setIsActive(flag_);}));
		}

	public:
		// constructor
		dxosFileSUI(void):dxosBUI("File Controll", 30, 30){
			isActive = false;
		}
		
		//virtual int fndraw(dxO& dxf);
		int normal_draw(dxO& dxo)override{

			//　FILE 書き込みボタン
			if(dxo.draw(Pint(5,5),dxoButIO(Pint(105, 20),"FILE 接続",getClr(normal,butobj), isActive), true) == 1){
				if(!isActive){
					
					// Open signal 
					signal_require_activate("");
				}else{
					// Close signal
					signal_require_inactivate();
				}
			}

			// 状態表示
			if(isActive){
				dxo.draw(Pint(115,5),dxoStrP(Pint(105, 20),"Opened",getClr(normal,strobj)));
			}else{
				dxo.draw(Pint(115,5),dxoStrP(Pint(105, 20),"Closed",getClr(invalid,strobj)));
			}

			return 0;
		}

		int extend_draw(dxO& dxo)override{
			// draw
			normal_draw(dxo);
			return 0;

		}

		status getStatus()const override{
			if(isActive){
				return normal;
			}else{
				return invalid;
			}
		}
	
	};		
}

#
#endif
