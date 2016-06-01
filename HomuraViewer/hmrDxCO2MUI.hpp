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
	class dxosCO2MUI:public dxosBUI{
	public:
		//hmLib::inquiries::inquiry<double> inquiry_getValue;
		//hmLib::inquiries::inquiry<clock::time_point> inquiry_getTime;

		double Value;
		clock::time_point Time;

		dxosBUIWaitableBoolBut IsDataModeMUIBut;
		dxosBUIWaitableBoolBut PumpPWMUIBut;
		dxosBUIWaitableBoolBut SensorPWMUIBut;
		dxosBUIWaitableBoolBut IsLogModeMUIBut;

	public:
		dxosCO2MUI()
			:dxosBUI("CO2",55,55)
			,IsDataModeMUIBut(this)
			,PumpPWMUIBut(this)
			,SensorPWMUIBut(this)
			,IsLogModeMUIBut(this)
		{
		}
	public:
		int normal_draw(dxO& dxo)override{
			try{
				try{
					IsDataModeMUIBut.set(Pint(70,20),"Data");
					dxo.draw(Pint(5,5),IsDataModeMUIBut);
				}catch(const hmLib::inquiries::unconnected_exception&){
					dxo.draw(Pint(5,5),dxoStrP(Pint(70,20),"CO2",getClr(error,strobj)));
				}

				try{
					dxo.draw(Pint(80,5),dxoBUITimeStr(this,Pint(70,20),(boost::format("%.1fppm")%Value).str(),Time));
				}catch(const hmLib::inquiries::unconnected_exception&){
					dxo.draw(Pint(80,5),dxoStrP(Pint(70,20),"NoCnct",getClr(error,strobj)));
				}

				try{
					IsLogModeMUIBut.set(Pint(70, 20), "Log");
					dxo.draw(Pint(5, 30), IsLogModeMUIBut);
				}
				catch (const hmLib::inquiries::unconnected_exception&){
					dxo.draw(Pint(5, 30), dxoStrP(Pint(70, 20), "LogMode_NoCnct", getClr(error, strobj)));
				}

			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(5,5),dxoButIO(getSize(),std::string("=ERR=")+Excp.what(),getClr(error,butobj),true,CLR::White,ALI::left));
			}

			try{
				SensorPWMUIBut.set(Pint(70,20),"SensorPW");
				dxo.draw(Pint(80,30),SensorPWMUIBut);
			}catch(const hmLib::inquiries::unconnected_exception&){
				dxo.draw(Pint(80,30),dxoStrP(Pint(70,20),"NoCnct",getClr(error,strobj)));
			}

			try{
				PumpPWMUIBut.set(Pint(70,20),"PumpPW");
				dxo.draw(Pint(155,30),PumpPWMUIBut);
			}catch(const hmLib::inquiries::unconnected_exception&){
				dxo.draw(Pint(155,30),dxoStrP(Pint(70,20),"NoCnct",getClr(error,strobj)));
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

	private:
		hmLib::signals::unique_connections SignalConnections;
		void setData(clock::time_point time_, double data_){
			Time = time_;
			Value = data_;
		}
	// slot 
	public:
		void slot_getData(boost::signals2::signal<void(double, clock::time_point)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](double data_, clock::time_point time_)->void{this->setData(time_, data_);}));	
		}
	};
}
#
#endif
