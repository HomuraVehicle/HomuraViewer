#ifndef HMR_DXSHT75MUI_INC
#define HMR_DXSHT75MUI_INC 100
#
/*
===hmrDxSHT75MUI===
v1_00/131026
	äÓñ{ìIÇ…äÆê¨
*/

#include"hmLibVer.hpp"
#include<string>
#include<sstream>
#include<hmLib_v2/dxObject.hpp>
#include<hmLib_v2/dxColorSet.hpp>
#include<boost/signals2.hpp>
#include<hmLib/signals.hpp>
#include<hmLib/inquiries.hpp>
#include"Data.hpp"
#include"hmrDxBUI.hpp"
#include"hmrDxTools.hpp"
namespace hmr{
	//x240*yNONE
	class dxosSHT75MUI:public dxosBUI{
	private:
		double temperature;
		double humidity;
		clock::time_point time;
		hmLib::signals::unique_connections SignalConnections;
	public:
		//hmLib::inquiries::inquiry<double> inquiry_getThermo;
		//hmLib::inquiries::inquiry<clock::time_point> inquiry_getTime;

		dxosBUIWaitableBoolBut IsDataModeMUIBut;
		dxosBUIWaitableBoolBut IsLowAccMUIBut;
		dxosBUIWaitableBut RequestRestMUIBut;
	public:
		dxosSHT75MUI():dxosBUI("SHT75",30,55), IsDataModeMUIBut(this),IsLowAccMUIBut(this),RequestRestMUIBut(this){}
	public:
		int normal_draw(dxO& dxo)override{
			try{
				try{
					IsDataModeMUIBut.set(Pint(70,20),"Data");
					dxo.draw(Pint(5,5),IsDataModeMUIBut);
				}catch(const hmLib::inquiries::unconnected_exception&){
					dxo.draw(Pint(5,5),dxoStrP(Pint(70,20),"Data",getClr(error,strobj)));
				}

				dxo.draw(Pint(80,5),dxoBUITimeStr(this,Pint(70,20),(boost::format("%.1fÅé")%temperature).str(),time));
				dxo.draw(Pint(155,5),dxoBUITimeStr(this,Pint(70,20),(boost::format("%.1fÅì")%humidity).str(),time));

			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(0,0),dxoButIO(getSize(),std::string("=ERR=")+Excp.what(),getClr(error,butobj),true,CLR::White,ALI::left));
			}

			return 0;
		}
		int extend_draw(dxO& dxo)override{
			try{
				normal_draw(dxo);

				try{
					IsLowAccMUIBut.set(Pint(70,20),"LowAcc");
					dxo.draw(Pint(5,30),IsLowAccMUIBut);
				}catch(const hmLib::inquiries::unconnected_exception&){
					dxo.draw(Pint(5,30),dxoStrP(Pint(70,20),"LowAcc",getClr(error,strobj)));
				}

				try{
					RequestRestMUIBut.set(Pint(70,20),"Reset");
					dxo.draw(Pint(80,30),RequestRestMUIBut);
				}catch(const hmLib::inquiries::unconnected_exception&){
					dxo.draw(Pint(80,30),dxoStrP(Pint(70,20),"Reset",getClr(error,strobj)));
				}

			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(0,0),dxoButIO(getSize(),std::string("=ERR=")+Excp.what(),getClr(error,butobj),true,CLR::White,ALI::left));
			}

			return 0;
		}
		status getStatus()const override{
			if(IsDataModeMUIBut.Pic.is_connect() && IsDataModeMUIBut.Pic()){
				return normal;
			}else{
				return invalid;
			}
		}
		void setData(clock::time_point time_, double ThermoData_, double HumidData_){
			time = time_;
			temperature = ThermoData_;
			humidity = HumidData_;
		}
	// slot 
	public:
		void slot_getData(boost::signals2::signal<void(double, double,clock::time_point)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](double ThermoData_, double HumidData_, clock::time_point time_)->void{this->setData(time_, ThermoData_, HumidData_);}));	
		}
	};
}
#
#endif
