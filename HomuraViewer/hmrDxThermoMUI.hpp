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
#include<hmLib/inquiries.hpp>
#include"hmrData.hpp"
#include"hmrDxBUI.hpp"
#include"hmrDxTools.hpp"
namespace hmr{
	//x240*yNONE
	class dxosThermoMUI:public dxosBUI{
	private:
		double temperature;
		std::uint16_t raw_temperature;
		clock::time_point time;
		hmLib::signals::unique_connections SignalConnections;
	public:
		//hmLib::inquiries::inquiry<double> inquiry_getThermo;
		//hmLib::inquiries::inquiry<clock::time_point> inquiry_getTime;

		dxosBUIWaitableBoolBut IsDataModeMUIBut;
		dxosBUIWaitableBoolBut IsLogModeMUIBut;
	public:
		dxosThermoMUI() :dxosBUI("Thermo", 55, 55), IsDataModeMUIBut(this), IsLogModeMUIBut(this){ raw_temperature = 0; }
	public:
		int normal_draw(dxO& dxo)override{
			try{
				try{
					IsDataModeMUIBut.set(Pint(70,20),"Data");
					dxo.draw(Pint(5,5),IsDataModeMUIBut);
				}catch(const hmLib::inquiries::unconnected_exception&){
					dxo.draw(Pint(5,5),dxoStrP(Pint(70,20),"Data",getClr(error,strobj)));
				}

				dxo.draw(Pint(80, 5), dxoBUITimeStr(this, Pint(70, 20), (boost::format("%.1fÅé") % temperature).str(), time));
				dxo.draw(Pint(155, 5), dxoBUITimeStr(this, Pint(70, 20), (boost::format("0x%04x") % raw_temperature).str(), time));

				try{
					IsLogModeMUIBut.set(Pint(70, 20), "Log");
					dxo.draw(Pint(5, 30), IsLogModeMUIBut);
				}
				catch(const hmLib::inquiries::unconnected_exception&){
					dxo.draw(Pint(5, 30), dxoStrP(Pint(70, 20), "Log", getClr(error, strobj)));
				}
			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(0,0),dxoButIO(getSize(),std::string("=ERR=")+Excp.what(),getClr(error,butobj),true,CLR::White,ALI::left));
			}

			return 0;
		}
		int extend_draw(dxO& dxo)override{
			return normal_draw(dxo);
		}

		status getStatus()const override{
			if(IsDataModeMUIBut.Pic.is_connect() && IsDataModeMUIBut.Pic()){
				return normal;
			}else{
				return invalid;
			}
		}


		void setData(clock::time_point time_, double data_){
			time = time_;
			temperature = data_;
		}
		void setRawData(clock::time_point time_, double data_){
			time = time_;
			raw_temperature = static_cast<std::uint16_t>(data_);
		}
	// slot 
	public:
		void slot_getData(boost::signals2::signal<void(double, clock::time_point)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](double data_, clock::time_point time_)->void{this->setData(time_, data_);}));	
		}
		void slot_getRawData(boost::signals2::signal<void(double, clock::time_point)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_, [&](double data_, clock::time_point time_)->void{this->setRawData(time_, data_); }));
		}
	};
}
#
#endif
