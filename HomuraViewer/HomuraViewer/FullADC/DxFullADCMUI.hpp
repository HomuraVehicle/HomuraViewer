#ifndef HMR_VIEWER_FULLADC_DXMUI_INC
#define HMR_VIEWER_FULLADC_DXMUI_INC 101
#
/*===fulladc::dxosMUI===
*/

#include"../hmLibVer.hpp"
#include<string>
#include<sstream>
#include<hmLib_v2/dxObject.hpp>
#include<hmLib_v2/dxColorSet.hpp>
#include<boost/signals2.hpp>
#include<hmLib/signals.hpp>
#include<hmLib/inquiries.hpp>
#include<HomuraViewer/chrono.hpp>
#include"../hmrDxBUI.hpp"
#include"../hmrDxTools.hpp"
#include"FullADCData.hpp"
namespace hmr{
	namespace viewer{
		namespace fulladc{
			//x240*yNONE
			class dxosMUI :public dxosBUI{
				static constexpr unsigned int adc_size = 8;
				using this_data = data_t<adc_size>;
			private:
				this_data Data;
				hmLib::signals::unique_connections SignalConnections;
			public:
				dxosBUIWaitableBoolBut DataModeMUIBut;
			public:
				dxosMUI() :dxosBUI("ADC", 30, 105), DataModeMUIBut(this){}
			public://dxosBUI
				int normal_draw(dxO& dxo)override{
					try{
						try{
							DataModeMUIBut.set(Pint(70, 20), "Data");
							dxo.draw(Pint(5, 5), DataModeMUIBut);
						}
						catch(const hmLib::inquiries::unconnected_exception&){
							dxo.draw(Pint(5, 5), dxoStrP(Pint(70, 20), "Data", getClr(error, strobj)));
						}
					}
					catch(const hmLib::exceptions::exception& Excp){
						dxo.draw(Pint(0, 0), dxoButIO(getSize(), std::string("=ERR=") + Excp.what(), getClr(error, butobj), true, CLR::White, ALI::left));
					}

					return 0;
				}
				int extend_draw(dxO& dxo)override{
					dxo.draw(Pint(5, 30), dxoBUITimeStr(this, Pint(70, 20), (boost::format("0:%5d") % Data.Data[0]).str(), Data.Time));
					dxo.draw(Pint(80, 30), dxoBUITimeStr(this, Pint(70, 20), (boost::format("1:%5d") % Data.Data[1]).str(), Data.Time));
					dxo.draw(Pint(155, 30), dxoBUITimeStr(this, Pint(70, 20), (boost::format("2:%5d") % Data.Data[2]).str(), Data.Time));
					dxo.draw(Pint(5, 55), dxoBUITimeStr(this, Pint(70, 20), (boost::format("3:%5d") % Data.Data[3]).str(), Data.Time));
					dxo.draw(Pint(80, 55), dxoBUITimeStr(this, Pint(70, 20), (boost::format("4:%5d") % Data.Data[4]).str(), Data.Time));
					dxo.draw(Pint(155, 55), dxoBUITimeStr(this, Pint(70, 20), (boost::format("5:%5d") % Data.Data[5]).str(), Data.Time));
					dxo.draw(Pint(5, 80), dxoBUITimeStr(this, Pint(70, 20), (boost::format("6:%5d") % Data.Data[6]).str(), Data.Time));
					dxo.draw(Pint(80, 80), dxoBUITimeStr(this, Pint(70, 20), (boost::format("7:%5d") % Data.Data[7]).str(), Data.Time));
					return normal_draw(dxo);
				}
				status getStatus()const override{
					if(DataModeMUIBut.Pic.is_connect() && DataModeMUIBut.Pic()){
						return normal;
					} else{
						return invalid;
					}
				}
				void set_data(const this_data& Data_){ Data = Data_; }
			public:
				void slot_getData(boost::signals2::signal<void(this_data)>& Signal_){
					SignalConnections(hmLib::signals::connect(Signal_, [&](this_data Data_)->void{this->set_data(Data_); }));
				}
			};
		}
	}
}
#
#endif
