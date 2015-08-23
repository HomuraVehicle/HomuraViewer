#ifndef HMR_DXPADSTATUS_INC
#define HMR_DXPADSTATUS_INC 100
#
#include"hmLibVer.hpp"
#include<hmLib_v2/dxObject.hpp>
#include<hmLib_v2/dxPad.hpp>
#include<hmLib_v2/dxColorSet.hpp>
namespace hmr{
	class dxoPadStatusBut:public dxReObject,public hmoBox{
	protected:
		int redraw(dxO& dxo)override{
			if(GetJoypadNum()){
				if(dxo.draw(Pint(0,0),dxoButIO(size,"Use Pad",CLR::SoftBlue,true))<0){
					dxPad::connect();
				}
			}else{
				if(dxo.draw(Pint(0,0),dxoButIO(size,"No Pad",CLR::Gray,false))<0){
					dxPad::connect();
				}
			}
			return 0;
		}
	public:
		dxoPadStatusBut(Pint _size):hmoBox(_size){return;}
	};
}
#
#endif
