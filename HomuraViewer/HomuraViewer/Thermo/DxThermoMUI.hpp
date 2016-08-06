#ifndef HMR_DXTHERMOMUI_INC
#define HMR_DXTHERMOMUI_INC 101
#
/*===hmrThermo===
hmrDxThermoMsgAgentUI v1_01/130705 amby
	BUIÇåpè≥Ç∑ÇÈ

hmrDxThermoMsgAgentUI v1_00/130414 iwahori
	dxoThermoMsgAgentUIÇÃsignal-slotëŒâûíÜÅB
	redrawä÷êîÇ™ìríÜÅA
*/

#include"hmLibVer.hpp"
#include<string>
#include<sstream>
#include<hmLib_v2/dxObject.hpp>
#include<hmLib_v2/dxColorSet.hpp>
#include<boost/signals2.hpp>
#include<hmLib/signals.hpp>
#include<HomuraViewer/chrono.hpp>
#include"hmrDxBUI.hpp"
#include"hmrDxTools.hpp"
#include<HomuraViewer/Thermo/ThermoData.hpp>
namespace hmr{
	namespace viewer{
		namespace thermo{
			//x240*yNONE
			class dxosMUI :public dxosBUI{
			private:
				data_t Data;
				hmLib::signals::unique_connections SignalConnections;
			public:
				dxosBUIWaitableBoolBut IsDataModeMUIBut;
			public:
				dxosMUI() :dxosBUI("Thermo", 55, 55), IsDataModeMUIBut(this){}
			public:
				int normal_draw(dxO& dxo)override{
					try{
						try{
							IsDataModeMUIBut.set(Pint(70, 20), "Data");
							dxo.draw(Pint(5, 5), IsDataModeMUIBut);
						}
						catch(const hmLib::inquiries::unconnected_exception&){
							dxo.draw(Pint(5, 5), dxoStrP(Pint(70, 20), "Data", getClr(error, strobj)));
						}

						try{
							dxo.draw(Pint(80, 5), dxoBUITimeStr(this, Pint(70, 20), (boost::format("%.1fÅé") % Data.value()).str(), Data.Time));
							dxo.draw(Pint(155, 5), dxoBUITimeStr(this, Pint(70, 20), (boost::format("0x%04x") % Data.raw_value()).str(), Data.Time));
						}
						catch(const hmLib::inquiries::unconnected_exception&){
							dxo.draw(Pint(80, 5), dxoStrP(Pint(70, 20), "NoCnct", getClr(error, strobj)));
						}
					}
					catch(const hmLib::exceptions::exception& Excp){
						dxo.draw(Pint(0, 0), dxoButIO(getSize(), std::string("=ERR=") + Excp.what(), getClr(error, butobj), true, CLR::White, ALI::left));
					}

					return 0;
				}
				int extend_draw(dxO& dxo)override{
					return normal_draw(dxo);
				}
				status getStatus()const override{
					if(IsDataModeMUIBut.Pic.is_connect() && IsDataModeMUIBut.Pic()){
						return normal;
					} else{
						return invalid;
					}
				}
				void setData(thermo::data_t Data_){
					Data = Data_;
				}
				// slot 
			public:
				void slot_getData(boost::signals2::signal<void(thermo::data_t)>& Signal_){
					SignalConnections(hmLib::signals::connect(Signal_, [&](thermo::data_t Data)->void{this->setData(Data); }));
				}
			};
		}
	}
}
#
#endif
