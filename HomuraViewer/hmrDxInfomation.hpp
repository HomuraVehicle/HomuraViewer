#ifndef HMR_DXINFORMATIRON_INC
#define HMR_DXINFORMATIRON_INC
#
#include"hmLibVer.hpp"
#include<hmLib_v2/dxObject.hpp>
#include"hmrDxGPSKashmir.hpp"
#include"hmrDxPadStatus.hpp"
#include"hmrDxBattery.hpp"
namespace hmr{
	class dxosInformation:public dxReObject, public hmoBox{
	public:
		dxosGPSKashmirBut GPSKashmirBut;
		dxoPadStatusBut PadStatusBut;
		dxosBattery Battery0;
		dxosBattery Battery1;
		dxosBattery Battery2;
	public:
		dxosInformation(Pint size_)
			:GPSKashmirBut(Pint(80,30))
			,PadStatusBut(Pint(80,30))
			,Battery0(Pint(120,30),6.5,8.5)
			,Battery1(Pint(120,30),6.5,8.5)
			,Battery2(Pint(120,30),6.5,8.5){
		}
		int redraw(dxO& dxo)override{
			dxo.draw(Pint(5,5),GPSKashmirBut);
			dxo.draw(Pint(5,45),PadStatusBut);
			dxo.draw(Pint(5,85),Battery0);
			dxo.draw(Pint(5,120),Battery1);
			dxo.draw(Pint(5,155),Battery2);
			return 0;
		}
	};
}
#
#endif
