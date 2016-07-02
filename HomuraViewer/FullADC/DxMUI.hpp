#ifndef HMR_FULLADC_DXMUI_INC
#define HMR_FULLADC_DXMUI_INC 101
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
#include<hmrVLib/Data.hpp>
#include"../hmrDxBUI.hpp"
#include"../hmrDxTools.hpp"
#include"Data.hpp"
namespace hmr{
	namespace fulladc{
		//x240*yNONE
		class dxosMUI :public dxosBUI{
			static constexpr unsigned int adc_size = 8;
		private:
			data_t<adc_size> Data;

			hmLib::signals::unique_connections SignalConnections;
		public:
			dxosBUIWaitableBoolBut IsDataModeMUIBut;
			dxosBUIWaitableBoolBut Ctr1MUIBut;
			dxosBUIWaitableBoolBut Ctr2MUIBut;
			dxosBUIWaitableBoolBut Ctr3MUIBut;
			dxosBUIWaitableBoolBut Ctr4MUIBut;
		public:
			dxosMUI() :dxosBUI("ADC", 30, 80), IsDataModeMUIBut(this), Ctr1MUIBut(this), Ctr2MUIBut(this), Ctr3MUIBut(this), Ctr4MUIBut(this){}
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
						Ctr1MUIBut.set(Pint(30, 20), "1");
						dxo.draw(Pint(80, 5), Ctr1MUIBut);
						Ctr1MUIBut.set(Pint(30, 20), "2");
						dxo.draw(Pint(115, 5), Ctr1MUIBut);
						Ctr1MUIBut.set(Pint(30, 20), "3");
						dxo.draw(Pint(150, 5), Ctr1MUIBut);
						Ctr1MUIBut.set(Pint(30, 20), "4");
						dxo.draw(Pint(185, 5), Ctr1MUIBut);
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
				dxo.draw(Pint(  5, 30), dxoBUITimeStr(this, Pint(50, 20), (boost::format("0:%5d") % Data.Data[0]).str(), Data.Time));
				dxo.draw(Pint( 60, 30), dxoBUITimeStr(this, Pint(50, 20), (boost::format("1:%5d") % Data.Data[1]).str(), Data.Time));
				dxo.draw(Pint(115, 30), dxoBUITimeStr(this, Pint(50, 20), (boost::format("2:%5d") % Data.Data[2]).str(), Data.Time));
				dxo.draw(Pint(170, 30), dxoBUITimeStr(this, Pint(50, 20), (boost::format("3:%5d") % Data.Data[3]).str(), Data.Time));
				dxo.draw(Pint(  5, 55), dxoBUITimeStr(this, Pint(50, 20), (boost::format("4:%5d") % Data.Data[4]).str(), Data.Time));
				dxo.draw(Pint( 60, 55), dxoBUITimeStr(this, Pint(50, 20), (boost::format("5:%5d") % Data.Data[5]).str(), Data.Time));
				dxo.draw(Pint(115, 55), dxoBUITimeStr(this, Pint(50, 20), (boost::format("6:%5d") % Data.Data[6]).str(), Data.Time));
				dxo.draw(Pint(170, 55), dxoBUITimeStr(this, Pint(50, 20), (boost::format("7:%5d") % Data.Data[7]).str(), Data.Time));
				return normal_draw(dxo);
			}
			status getStatus()const override{
				if(IsDataModeMUIBut.Pic.is_connect() && IsDataModeMUIBut.Pic()){
					return normal;
				} else{
					return invalid;
				}
			}
		};
	}
	}

#
#endif
