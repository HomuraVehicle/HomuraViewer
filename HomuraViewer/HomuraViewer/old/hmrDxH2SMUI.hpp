#ifndef HMR_DXH2SMUI_INC
#define HMR_DXH2SMUI_INC 100
#
/*===hmrDxH2SMUI===
hmrDxH2SMUI v1_00/130519 iwahori
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
#include <HomuraViewer/chrono.hpp>
#include"hmrDxBUI.hpp"
#include"hmrDxTools.hpp"
namespace hmr{
	namespace viewer{
		//x240*yNONE
		class dxosH2SMUI :public dxosBUI{
		public:
			//hmLib::inquiries::inquiry<double> inquiry_getValue;
			//hmLib::inquiries::inquiry<clock::time_point> inquiry_getTime;
			double Value;
			clock::time_point Time;


			dxosBUIWaitableBoolBut IsDataModeMUIBut;
		public:
			dxosH2SMUI() :dxosBUI("H2S", 30, 30), IsDataModeMUIBut(this), Value(0){}
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
						dxo.draw(Pint(80, 5), dxoBUITimeStr(this, Pint(70, 20), (boost::format("%.1f ppm") % Value).str(), Time));
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
				if(IsDataModeMUIBut.Pic.is_connect() && IsDataModeMUIBut.Pic())return normal;
				else return invalid;
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
				SignalConnections(hmLib::signals::connect(Signal_, [&](double data_, clock::time_point time_)->void{this->setData(time_, data_); }));
			}
		};
	}
}
#
#endif
