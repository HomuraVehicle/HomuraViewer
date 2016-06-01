#ifndef HMR_DXMOTORMUI_INC
#define HMR_DXMOTORMUI_INC 101
#
/*===hmrDxMotorMUI===
hmrDxMotorMUI v1_01/130705 amby
  Å@BUIâª

hmrDxMotorMUI v1_00/130519 iwahori
	çÏê¨ÅD
*/

#include"hmLibVer.hpp"
#include<string>
#include<sstream>
#include<hmLib_v2/dxObject.hpp>
#include<hmLib_v2/dxColorSet.hpp>
#include<boost/signals2.hpp>
#include<hmLib_v3_06/signals.hpp>
#include<hmLib_v3_06/inquiries.hpp>
#include"hmrData.hpp"
#include"hmrDxBUI.hpp"
namespace hmr{
	//x240*yNONE
	class dxosMotorMUI:public dxosBUI{
	public:
		boost::signals2::signal<void(char)> Signal_MotorStickL;
		boost::signals2::signal<void(char)> Signal_MotorStickR;
		boost::signals2::signal<void(bool)> Signal_BackMode;

		hmLib::inquiries::inquiry<char> inquiry_getMotorStickL;
		hmLib::inquiries::inquiry<bool> inquiry_getMotorBrakeL;
		hmLib::inquiries::inquiry<char> inquiry_getMotorStickR;
		hmLib::inquiries::inquiry<bool> inquiry_getMotorBrakeR;
		hmLib::inquiries::inquiry<bool> inquiry_getBackMode;

	public:
		dxosMotorMUI():dxosBUI("Motor", 30,30){}
	public:
		int normal_draw(dxO& dxo)override{
			//dxo.draw(Pint(0,0),dxoButIO(Pint(75,25),"Motor",getClr(normal,butobj),true));

			try{
				try{
					bool BackMode=inquiry_getBackMode();
					if(dxo.draw(Pint(5,5),dxoButIO(Pint(70,20),"Back",getClr(normal,butobj),BackMode))==1){
						Signal_BackMode(!BackMode);
					}				
				}catch(const hmLib::inquiries::unconnected_exception&){
					dxo.draw(Pint(5,5),dxoStrP(Pint(70,20),"--",getClr(error,strobj)));
				}

				try{
					if(inquiry_getMotorBrakeL()){
						int buttL =  dxo.draw(Pint(80,5),dxoButLR(Pint(70,20),"Brake!!",getClr(error,strobj)));
					}else{
						char StickL=inquiry_getMotorStickL();
						int buttL =  dxo.draw(Pint(80,5),dxoButLR(Pint(70,20),"L "+std::to_string(StickL),getClr(normal,butobj)));
						if( buttL == 10){
							if(StickL>0)StickL = 0;
							else StickL = -100;
							Signal_MotorStickL(StickL);
						}else if(buttL == 1){
							if(StickL<0)StickL = 0;
							else StickL = 100;
							Signal_MotorStickL(StickL);
						}
					}

					if(inquiry_getMotorBrakeR()){
						int buttL =  dxo.draw(Pint(155,5),dxoButLR(Pint(70,20),"Brake!!",getClr(error,strobj)));
					}else{
						char StickR=inquiry_getMotorStickR();
						int buttR =  dxo.draw(Pint(155,5),dxoButLR(Pint(70,20),"R "+std::to_string(StickR),getClr(normal,butobj)));
						if( buttR == 10){
							if(StickR>0)StickR = 0;
							else StickR = -100;
							Signal_MotorStickR(StickR);
						}else if(buttR == 1){
							if(StickR<0)StickR = 0;
							else StickR = 100;
							Signal_MotorStickR(StickR);
						}
					}
				}catch(const hmLib::inquiries::unconnected_exception&){
					dxo.draw(Pint(80,5),dxoStrP(Pint(70,20),"NoCnct",getClr(error,strobj)));
					dxo.draw(Pint(155,5),dxoStrP(Pint(70,20),"NoCnct",getClr(error,strobj)));					
				}

			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(5,5),dxoButIO(getSize(),std::string("=ERR=")+Excp.what(),getClr(error,butobj),true,CLR::White,ALI::left));
			}

			return 0;
		}
		int extend_draw(dxO& dxo)override{
			return normal_draw(dxo);
		}

		status getStatus()const override{
			if(inquiry_getBackMode.is_connect() && inquiry_getMotorStickL.is_connect() && inquiry_getMotorStickR.is_connect()){
				return normal;
			}else{
				return invalid;
			}
		}

	};
}
#
#endif
