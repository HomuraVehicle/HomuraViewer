#ifndef HMR_DXACCELEMUI_INC
#define HMR_DXACCELEMUI_INC 101
#
/*===hmrDxAcceleMUI===
hmrDxAcceleMUI v1_01/130720 amby 
	データ受信をsignal化
hmrDxAcceleMUI v1_00/130511 hmIto
	追加
*/

#include"hmLibVer.hpp"
#include<string>
#include<sstream>
#include<hmLib_v2/dxObject.hpp>
#include<hmLib_v2/dxColorSet.hpp>
#include<boost/signals2.hpp>
#include<hmLib/signals.hpp>
#include<hmLib/inquiries.hpp>
#include"coordinates.hpp"
#include"hmrData.hpp"
#include"hmrDxBUI.hpp"
namespace hmr{
	//x240*yNONE
	class dxosAcceleMUI:public dxosBUI{
		typedef hmLib::coordinates3D::position position;
		typedef hmLib::coordinates3D::polar polar;

		hmLib::signals::unique_connections SignalConnections;
	public:
		//hmLib::inquiries::inquiry<position> inquiry_getAccelePosition;
		//hmLib::inquiries::inquiry<polar> inquiry_getAccelePolar;
		//hmLib::inquiries::inquiry<clock::time_point> inquiry_getTime;

		position AccelePosition;
		polar AccelePolar;
		clock::time_point Time;

		dxosBUIWaitableBoolBut IsDataModeMUIBut;

		dxosBUIWaitableBut CorrectionDataBut;
		hmLib::inquiries::inquiry<position> inquiry_getCorrectionValue;
	public:
		dxosAcceleMUI():dxosBUI("Accele",30,105),IsDataModeMUIBut(this),CorrectionDataBut(this){}

		void setData(clock::time_point time_, position pos_, polar por_){
			Time = time_;
			AccelePosition = pos_;
			AccelePolar = por_;
		}
	// slot 
	public:
		void slot_getData(boost::signals2::signal<void(clock::time_point, position, polar)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](clock::time_point time_, position pos_, polar por_)->void{this->setData(time_, pos_, por_);}));	
		}
	public:
		int normal_draw(dxO& dxo)override{
			try{
				try{
					IsDataModeMUIBut.set(Pint(70,20),"Data");
					dxo.draw(Pint(5,5),IsDataModeMUIBut);
				}catch(const hmLib::inquiries::unconnected_exception&){
					dxo.draw(Pint(5,5),dxoStrP(Pint(70,20),"Data",getClr(error,strobj)));
				}

				try{
					polar Value=AccelePolar;
					dxo.draw(Pint(80,5),dxoStrP(Pint(70,20),(boost::format("P%.1f")%Value.phi).str(),getTimeStrClr(Time)));
					dxo.draw(Pint(155,5),dxoStrP(Pint(70,20),(boost::format("R%.1f")%Value.theta).str(),getTimeStrClr(Time)));
				}catch(const hmLib::inquiries::unconnected_exception&){
					dxo.draw(Pint(80,5),dxoStrP(Pint(70,20),"NoCnct",getClr(error,strobj)));
					dxo.draw(Pint(155,5),dxoStrP(Pint(70,20),"NoCnct",getClr(error,strobj)));
				}
			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(0,0),dxoButIO(getSize(),std::string("=ERR=")+Excp.what(),getClr(error,butobj),true,CLR::White,ALI::left));
			}

			return 0;
		}
		int extend_draw(dxO& dxo)override{
			normal_draw(dxo);

			try{
				position Value=AccelePosition;
				dxo.draw(Pint(5,30),dxoStrP(Pint(70,20),(boost::format("X%.1f")%Value.x).str(),getTimeStrClr(Time)));
				dxo.draw(Pint(80,30),dxoStrP(Pint(70,20),(boost::format("Y%.1f")%Value.y).str(),getTimeStrClr(Time)));
				dxo.draw(Pint(155,30),dxoStrP(Pint(70,20),(boost::format("Z%.1f")%Value.z).str(),getTimeStrClr(Time)));
			}catch(const hmLib::inquiries::unconnected_exception&){
				dxo.draw(Pint(5,30),dxoStrP(Pint(70,20),"NoCnct",getClr(error,strobj)));
				dxo.draw(Pint(80,30),dxoStrP(Pint(70,20),"NoCnct",getClr(error,strobj)));
				dxo.draw(Pint(155,30),dxoStrP(Pint(70,20),"NoCnct",getClr(error,strobj)));
			}

			try{
				CorrectionDataBut.set(Pint(70,20),"補正");
				dxo.draw(Pint(5,55),CorrectionDataBut);
			}catch(const hmLib::inquiries::unconnected_exception&){
				dxo.draw(Pint(5,55),dxoStrP(Pint(70,20),"NoCnct",getClr(error,strobj)));
			}

			try{
				auto Value=inquiry_getCorrectionValue();
				dxo.draw(Pint(5,80),dxoStrP(Pint(70,20),(boost::format("補X:%.1f")%Value.x).str(),getTimeStrClr(Time)));
				dxo.draw(Pint(80,80),dxoStrP(Pint(70,20),(boost::format("補Y:%.1f")%Value.y).str(),getTimeStrClr(Time)));
				dxo.draw(Pint(155,80),dxoStrP(Pint(70,20),(boost::format("補Z:%.1f")%Value.z).str(),getTimeStrClr(Time)));
			}catch(const hmLib::inquiries::unconnected_exception&){
				dxo.draw(Pint(5,80),dxoStrP(Pint(70,20),"NoCnct",getClr(error,strobj)));
				dxo.draw(Pint(80,80),dxoStrP(Pint(70,20),"NoCnct",getClr(error,strobj)));
				dxo.draw(Pint(155,80),dxoStrP(Pint(70,20),"NoCnct",getClr(error,strobj)));
			}

			return 0;
		}
		status getStatus()const override{
			if(IsDataModeMUIBut.Pic.is_connect() && IsDataModeMUIBut.Pic())return normal;
			else return invalid;
		}
	};
}
#
#endif
