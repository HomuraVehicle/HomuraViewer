#ifndef HMR_DXCO2MUI_INC
#define HMR_DXCO2MUI_INC 101
#
/*===hmrCO2===
hmrDxCO2MsgAgentUI v1_01/130705 amby
	BUIâª + ÉfÅ[É^éÊìæÇÃslotâª

hmrDxCO2MsgAgentUI v1_00/130414 iwahori
	çÏê¨
*/

#include"hmLibVer.hpp"
#include<string>
#include<cstdint>
#include<sstream>
#include<hmLib_v2/dxObject.hpp>
#include<hmLib_v2/dxColorSet.hpp>
#include<boost/signals2.hpp>
#include<hmLib/signals.hpp>
#include<hmLib/inquiries.hpp>
#include<HomuraViewer/chrono.hpp>
#include<HomuraViewer/View/DxBUI.hpp>

#include"CO2Data.hpp"
namespace hmr{
	namespace viewer{
		namespace co2{
			//x240*yNONE
			class dxosMUI :public dxosBUI{
			public:
				//hmLib::inquiries::inquiry<double> inquiry_getValue;
				//hmLib::inquiries::inquiry<clock::time_point> inquiry_getTime;

				data_t Data;

				dxosBUIWaitableBoolBut DataModeMUIBut;
				dxosBUIWaitableBoolBut PumpPWMUIBut;
				dxosBUIWaitableBoolBut SensorPWMUIBut;

			public:
				dxosMUI()
					: dxosBUI("CO2", 55, 55)
					, DataModeMUIBut(this)
					, PumpPWMUIBut(this)
					, SensorPWMUIBut(this){
				}
			public:
				int normal_draw(dxO& dxo)override{
					try{
						try{
							DataModeMUIBut.set(Pint(70, 20), "Data");
							dxo.draw(Pint(5, 5), DataModeMUIBut);
						}
						catch(const hmLib::inquiries::unconnected_exception&){
							dxo.draw(Pint(5, 5), dxoStrP(Pint(70, 20), "CO2", getClr(error, strobj)));
						}

						try{
							dxo.draw(Pint(80, 5), dxoBUITimeStr(this, Pint(70, 20), (boost::format("%.1fppm") % Data.value()).str(), Data.time()));
							dxo.draw(Pint(155, 5), dxoBUITimeStr(this, Pint(70, 20), (boost::format("0x%04x") % Data.raw_value()).str(), Data.time()));
						}
						catch(const hmLib::inquiries::unconnected_exception&){
							dxo.draw(Pint(80, 5), dxoStrP(Pint(70, 20), "NoCnct", getClr(error, strobj)));
						}
					}
					catch(const hmLib::exceptions::exception& Excp){
						dxo.draw(Pint(5, 5), dxoButIO(getSize(), std::string("=ERR=") + Excp.what(), getClr(error, butobj), true, CLR::White, ALI::left));
					}

					try{
						SensorPWMUIBut.set(Pint(70, 20), "SensorPW");
						dxo.draw(Pint(80, 30), SensorPWMUIBut);
					}
					catch(const hmLib::inquiries::unconnected_exception&){
						dxo.draw(Pint(80, 30), dxoStrP(Pint(70, 20), "NoCnct", getClr(error, strobj)));
					}

					try{
						PumpPWMUIBut.set(Pint(70, 20), "PumpPW");
						dxo.draw(Pint(155, 30), PumpPWMUIBut);
					}
					catch(const hmLib::inquiries::unconnected_exception&){
						dxo.draw(Pint(155, 30), dxoStrP(Pint(70, 20), "NoCnct", getClr(error, strobj)));
					}

					return 0;
				}
				int extend_draw(dxO& dxo)override{
					return normal_draw(dxo);
				}

				status getStatus()const override{
					if(DataModeMUIBut.Pic.is_connect() && DataModeMUIBut.Pic()){
						return normal;
					} else{
						return invalid;
					}
				}

			private:
				hmLib::signals::unique_connections SignalConnections;
				void setData(data_t Data_){
					Data = Data_;
				}
				// slot 
			public:
				void slot_getData(boost::signals2::signal<void(data_t)>& Signal_){
					SignalConnections(hmLib::signals::connect(Signal_, [&](data_t Data_)->void{this->setData(Data_); }));
				}
			};
		}
	}
}
#
#endif
