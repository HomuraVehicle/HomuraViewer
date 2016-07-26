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

�P���Ƀt�@�C���Ƀ��O���Ƃ邩�ǂ��������肷��I������^����UI

hmrDxFileSUI.hpp :v1_00/130914 amby
	DxFILESUI�̐���J�n
*/

namespace hmr{
	// �`��N���X static
	class dxosFileSUI:public dxosBUI{
	private:
		hmLib::signals::unique_connections SignalConnections;
		bool isActive;

	// signal, slot ���
	public:
		// signal �n��̒�`
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

			//�@FILE �������݃{�^��
			if(dxo.draw(Pint(5,5),dxoButIO(Pint(105, 20),"FILE �ڑ�",getClr(normal,butobj), isActive), true) == 1){
				if(!isActive){
					
					// Open signal 
					signal_require_activate("");
				}else{
					// Close signal
					signal_require_inactivate();
				}
			}

			// ��ԕ\��
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
