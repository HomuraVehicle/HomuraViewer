#ifndef HMR_DXGPSMUI_INC
#define HMR_DXGPSMUI_INC 101
#
/*===hmrDxGPSMUI===
hmrDxGPSMsgAgentUI v1_01/130713 amby
	GPSの裏表を変更できるようなボタンを追加

hmrDxGPSMsgAgentUI v1_00/130414 iwahori
	作成
*/

#include"hmLibVer.hpp"
#include<string>
#include<sstream>
#include<hmLib_v2/dxObject.hpp>
#include<hmLib_v2/dxColorSet.hpp>
#include<boost/signals2.hpp>
#include<hmLib/signals.hpp>
#include<hmLib/inquiries.hpp>
#include <hmrVLib/Data.hpp>
#include"hmrDxBUI.hpp"
#include"hmrDxTools.hpp"
#include"hmrGPSDataSet.hpp"
namespace hmr{
	//x240*yNONE
	class dxosGPSMUI:public dxosBUI{
	public:
		//hmLib::inquiries::inquiry<GPSDataSet> inquiry_getGPSData;
		GPSDataSet GPSData;

		//hmLib::inquiries::inquiry<unsigned char> inq_getGPSCh;
		hmLib::inquiries::inquiry<bool> inq_getPic_GPSCh;
		hmLib::inquiries::inquiry<bool> inq_getRequest_GPSCh;

		boost::signals2::signal<void(void)> signal_changeGPSCh;


		dxosBUIWaitableBoolBut IsDataModeMUIBut;
		dxoBut SetGPSNumBut;
	public:
		dxosGPSMUI():dxosBUI("GPS",30,105),IsDataModeMUIBut(this){}
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
					int gpsCh;
					dxRGB clr;
					if(inq_getPic_GPSCh() == inq_getRequest_GPSCh()){clr = getClr(normal,butobj);}
					else{clr =  getClr(active,butobj);}

					if(inq_getRequest_GPSCh()){gpsCh = 1;}
					else{gpsCh = 0;}

					SetGPSNumBut.set(Pint(70,20),(boost::format("GPS#:%d")%(int)gpsCh).str(),clr);
					auto ans = dxo.draw(Pint(80,5),SetGPSNumBut);
					if(ans == -1){
						signal_changeGPSCh();
					}

				}catch(const hmLib::inquiries::unconnected_exception&){
					dxo.draw(Pint(80,5),dxoStrP(Pint(70,20),"NoCnct",getClr(error,strobj)));				
				}

				try{
					auto Data=GPSData;//inquiry_getGPSData();
					auto Time=Data.getTime();
					dxo.draw(Pint(155,5),dxoStrP(Pint(70, 20),(boost::format("Fail:%d")%static_cast<int>(Data.getFail())).str(),getTimeStrClr(Time)));
				}catch(const hmLib::inquiries::unconnected_exception&){
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
				auto Data=GPSData;//inquiry_getGPSData();
				auto Time=Data.getTime();
				dxo.draw(Pint(80,30),dxoStrP(Pint(145,20),(boost::format("N:%.4f")%Data.getPos().y).str(),getTimeStrClr(Time)));
				dxo.draw(Pint(80,55),dxoStrP(Pint(145,20),(boost::format("E:%.4f")%Data.getPos().x).str(),getTimeStrClr(Time)));
				dxo.draw(Pint(80,80),dxoStrP(Pint(145,20),(boost::format("H:%.4f")%Data.getHeight()).str(),getTimeStrClr(Time)));
				dxo.draw(Pint(5,55),dxoStrP(Pint(70,20),(boost::format("GPS#:%d")%Data.getUseGPS()).str(),getTimeStrClr(Time)));
				//dxo.draw(Pint(5,80),dxoStrP(Pint(70,20),(boost::format("Fail:%d")%static_cast<int>(Data.getFail())).str(),getTimeStrClr(Time)));
			}catch(const hmLib::inquiries::unconnected_exception&){
				dxo.draw(Pint(80,30),dxoStrP(Pint(145,20),"NoCnct",getClr(error,strobj)));
				dxo.draw(Pint(80,55),dxoStrP(Pint(145,20),"NoCnct",getClr(error,strobj)));
				dxo.draw(Pint(80,80),dxoStrP(Pint(145,20),"NoCnct",getClr(error,strobj)));
				dxo.draw(Pint(5,55),dxoStrP(Pint(70,20),"NoCnct",getClr(error,strobj)));
				//dxo.draw(Pint(5,80),dxoStrP(Pint(70,20),"NoCnct",getClr(error,strobj)));
			}
			return 0;
		}
		status getStatus()const override{
			if(IsDataModeMUIBut.Pic.is_connect() && (!IsDataModeMUIBut.Pic()))return invalid;
			//else if(inquiry_getGPSData.is_connect() && (inquiry_getGPSData().getErr() || inquiry_getGPSData().getFail()))return error;
			else return normal;
		}

	private:
		hmLib::signals::unique_connections SignalConnections;
		void setData(clock::time_point time_, const GPSDataSet& GPSData_){
			//Time = time_;
			GPSData = GPSData_;
		}
	
	// slot 
	public:
		void slot_getData(boost::signals2::signal<void(const GPSDataSet&, clock::time_point)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](const GPSDataSet& data_, clock::time_point time_)->void{this->setData(time_, data_);}));	
		}

	};
}
#
#endif
