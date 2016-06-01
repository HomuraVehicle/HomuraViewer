#ifndef HMR_DXTOOLS_INC
#define HMR_DXTOOLS_INC 100
#
#include"hmLibVer.hpp"
#include<boost/signals2.hpp>
#include<hmLib/inquiries.hpp>
#include<hmLib/signals.hpp>
#include<hmLib_v2/dxColor.hpp>
#include<hmLib_v2/dxObject.hpp>
namespace hmr{
	class dxoWaitableBoolBut:public dxReObject,public hmoBox{
	private:
		std::string Str;
		dxRGB NormalClr;
		dxRGB NormalStrClr;
		dxRGB WaitClr;
		dxRGB WaitStrClr;
		bool ONOFF_Req;
		bool ONOFF_Pic;
	public:
		dxoWaitableBoolBut(){}
		dxoWaitableBoolBut(Pint Size_,const std::string& Str_,const dxRGB& NormalClr_,const dxRGB& NormalStrClr_,const dxRGB& WaitClr_,const dxRGB WaitStrClr_,bool ONOFF_Req_,bool ONOFF_Pic_)
			:hmoBox(Size_),Str(Str_),NormalClr(NormalClr_),NormalStrClr(NormalStrClr_),WaitClr(WaitClr_),WaitStrClr(WaitStrClr_),ONOFF_Req(ONOFF_Req_),ONOFF_Pic(ONOFF_Pic_){
		}
	public:
		int redraw(dxO& dxo)override{
			bool OnOff=ONOFF_Req;
			const dxRGB& ButColor=(ONOFF_Req==ONOFF_Pic?NormalClr:WaitClr);
			const dxRGB& SColor=(ONOFF_Req==ONOFF_Pic?NormalStrClr:WaitStrClr);
			return dxo.draw(Pint(0,0),dxoButIO(size,Str,ButColor,OnOff,SColor));
		}
	};
}
#
#endif
