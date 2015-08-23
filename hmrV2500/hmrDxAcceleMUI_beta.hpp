#ifndef HMR_DXACCELEMUI_INC
#define HMR_DXACCELEMUI_INC 100
#
/*===hmrDxAcceleMUI===
hmrDxAcceleMUI v1_00/130511 hmIto
	’Ç‰Á
*/

#include"hmLibVer.hpp"
#include<string>
#include<sstream>
#include<hmLib_v2/dxObject.hpp>
#include<hmLib_v2/dxColorSet.hpp>
#include<boost/signals2.hpp>
#include<hmLib_v3_06/signals.hpp>
#include<hmLib_v3_06/inquiries.hpp>
#include"coordinates.hpp"
#include"hmrData.hpp"
#include"hmrDxMUI.hpp"
namespace hmr{
	//x240*yNONE
	class dxosAcceleMUI:public dxosMUI{
	public:
		hmLib::inquiries::inquiry<hmLib::coordinates3D::position> inquiry_getAcceleData;
		hmLib::inquiries::inquiry<clock::time_point> inquiry_getTime;
		dxosWaitableBoolMUIBut IsDataModeMUIBut;
	public:
		dxosAcceleMUI():dxosMUI(Pint(240,30),Pint(240,55)),IsDataModeMUIBut(this){}
	public:
		int normal_draw(dxO& dxo)override{
			try{
				try{
					IsDataModeMUIBut.set(Pint(75,25),"‰Á‘¬“x");
					dxo.draw(Pint(0,0),IsDataModeMUIBut);
				}catch(const hmLib::inquiries::unconnected_exception&){
					dxo.draw(Pint(0,0),dxoStrP(Pint(75,25),"‰Á‘¬“x",getClr(error,strobj)));
				}

				try{
					auto Value=inquiry_getAcceleData();
					hmLib::coorinates3D::polar Pol(hmLib::coorinates3D::position(-Value.z,-Value.x,Value.y));
					dxo.draw(Pint(80,5),dxoStrP(Pint(70,20),(boost::format("P:%.1f")%Pol.phi).str(),getTimeStrClr(inquiry_getTime())));
					dxo.draw(Pint(155,5),dxoStrP(Pint(70,20),(boost::format("R:%.1f")%Pol.theta).str(),getTimeStrClr(inquiry_getTime())));
				}catch(const hmLib::inquiries::unconnected_exception&){
					dxo.draw(Pint(80,5),dxoStrP(Pint(70,20),"NoCnct",getClr(error,strobj)));
					dxo.draw(Pint(155,5),dxoStrP(Pint(70,20),"NoCnct",getClr(error,strobj)));
				}
			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(0,0),dxoButIO(getSize(),std::string("=ERR=")+Excp.what(),getClr(error,butobj),true,CLR::White,ALI::left));
			}

			return 0;
		}
		int extend_draw(dxO& dxo)override{
			normal_draw(dxo);

			try{
				auto Value=inquiry_getAcceleData();
				dxo.draw(Pint(5,30),dxoStrP(Pint(70,20),(boost::format("X:%.1f")%Value.x).str(),getTimeStrClr(inquiry_getTime())));
				dxo.draw(Pint(80,30),dxoStrP(Pint(70,20),(boost::format("Y:%.1f")%Value.y).str(),getTimeStrClr(inquiry_getTime())));
				dxo.draw(Pint(155,30),dxoStrP(Pint(70,20),(boost::format("Z:%.1f")%Value.z).str(),getTimeStrClr(inquiry_getTime())));
			}catch(const hmLib::inquiries::unconnected_exception&){
				dxo.draw(Pint(5,30),dxoStrP(Pint(70,20),"NoCnct",getClr(error,strobj)));
				dxo.draw(Pint(80,30),dxoStrP(Pint(70,20),"NoCnct",getClr(error,strobj)));
				dxo.draw(Pint(155,30),dxoStrP(Pint(70,20),"NoCnct",getClr(error,strobj)));
			}

			return 0;
		}
	};
}
#
#endif
