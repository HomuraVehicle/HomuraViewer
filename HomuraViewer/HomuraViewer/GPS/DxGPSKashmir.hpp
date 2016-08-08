#ifndef HMR_VIEWER_GPS_DXGPSKASHMIR_INC
#define HMR_VIEWER_GPS_DXGPSKASHMIR_INC
#
#include<hmLib/signals.hpp>
#include<hmLib/inquiries.hpp>
#include"hmLibVer.hpp"
#include<hmLib_v2/dxObject.hpp>
#include<hmLib_v2/dxPad.hpp>
#include<hmLib_v2/dxColorSet.hpp>
namespace hmr{
	namespace viewer{
		namespace gps{
			class dxosGPSKashmirBut :public dxReObject, public hmoBox{
			public:
				boost::signals2::signal<void(void)> signal_connect;
				boost::signals2::signal<void(void)> signal_disconnect;
				hmLib::inquiries::inquiry<bool> inquiry_is_connect;
			public:
				dxosGPSKashmirBut(Pint size_) :hmoBox(size_){}
				int redraw(dxO& dxo)override{
					try{
						if(inquiry_is_connect()){
							if(dxo.draw(Pint(0, 0), dxoButIO(size, "Kashmir", CLR::SoftBlue, true)) == 1){
								signal_disconnect();
							}
						} else{
							if(dxo.draw(Pint(0, 0), dxoButIO(size, "Kashmir", CLR::Gray, false)) == 1){
								signal_connect();
							}
						}
					}
					catch(const hmLib::inquiries::unconnected_exception&){
						dxo.draw(Pint(0, 0), dxoButIO(size, "KashmirErr", CLR::SoftRed, false));
					}
					return 0;
				}
			};
		}
	}
}
#
#endif
