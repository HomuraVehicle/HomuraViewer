#ifndef HMR_DXINFORMATIRON_INC
#define HMR_DXINFORMATIRON_INC
#
#include<chrono>
#include"hmLibVer.hpp"
#include<hmLib_v2/dxObject.hpp>
#include"hmrDxGPSKashmir.hpp"
#include"hmrDxPadStatus.hpp"
#include"battery/DxBattery.hpp"
#include"fulladc/Data.hpp"
#include"hmrDxBUI.hpp"
namespace hmr{
	namespace viewer{
		class dxosInformation :public dxReObject, public hmoBox{
			using this_adc_data_t = viewer::fulladc::data_t<8>;
		public:
			dxosGPSKashmirBut GPSKashmirBut;
			dxoPadStatusBut PadStatusBut;
			dxosBattery Battery0;
			dxosBattery Battery1;
			dxosBattery Battery2;
			viewer::fulladc::data_t<8> Data;
		public:
			dxosInformation(Pint size_)
				:GPSKashmirBut(Pint(80, 30))
				, PadStatusBut(Pint(80, 30))
				, Battery0(Pint(120, 30), 6.5, 8.5)
				, Battery1(Pint(120, 30), 6.5, 8.5)
				, Battery2(Pint(120, 30), 6.5, 8.5){}
			//240-360
			int redraw(dxO& dxo)override{
				dxo.draw(Pint(5, 5), GPSKashmirBut);
				dxo.draw(Pint(90, 5), PadStatusBut);
				dxo.draw(Pint(5, 45), Battery0);
				dxo.draw(Pint(5, 80), Battery1);
				dxo.draw(Pint(5, 115), Battery2);

				dxRGB Clr = CLR::Gray;
				if(std::chrono::duration_cast<std::chrono::seconds>(clock::now() - Data.Time).count() < 10){
					Clr = CLR::White;
				}
				dxo.draw(Pint(5, 165), dxoStrP(Pint(70, 20), (boost::format("0:%5d") % Data.Data[0]).str(), Clr));
				dxo.draw(Pint(125, 165), dxoStrP(Pint(70, 20), (boost::format("1:%5d") % Data.Data[1]).str(), Clr));
				dxo.draw(Pint(5, 190), dxoStrP(Pint(70, 20), (boost::format("2:%5d") % Data.Data[2]).str(), Clr));
				dxo.draw(Pint(125, 190), dxoStrP(Pint(70, 20), (boost::format("3:%5d") % Data.Data[3]).str(), Clr));
				dxo.draw(Pint(5, 215), dxoStrP(Pint(70, 20), (boost::format("4:%5d") % Data.Data[4]).str(), Clr));
				dxo.draw(Pint(125, 215), dxoStrP(Pint(70, 20), (boost::format("5:%5d") % Data.Data[5]).str(), Clr));
				dxo.draw(Pint(5, 240), dxoStrP(Pint(70, 20), (boost::format("6:%5d") % Data.Data[6]).str(), Clr));
				dxo.draw(Pint(125, 240), dxoStrP(Pint(70, 20), (boost::format("7:%5d") % Data.Data[7]).str(), Clr));

				return 0;
			}
		public:
			void slot_logData(boost::signals2::signal<void(this_adc_data_t)>& Signal_){
				hmLib::signals::connect(Signal_, [&](this_adc_data_t Data_)->void{this->Data = Data_; });
			}
		};
	}
}
#
#endif
