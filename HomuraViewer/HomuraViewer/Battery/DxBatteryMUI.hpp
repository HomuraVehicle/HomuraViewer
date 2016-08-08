#ifndef HMR_DXBATTERYMUI_INC
#define HMR_DXBATTERYMUI_INC 100
#
/*===hmrDxBatteryMUL===
hmrDxBatteryMUI v1_00/130519 iwahori
作成．電圧表示はテキトー
*/

#include"hmLibVer.hpp"
#include<string>
#include<sstream>
#include<boost/signals2.hpp>
#include<hmLib/signals.hpp>
#include<hmLib/inquiries.hpp>
#include<hmLib_v2/dxObject.hpp>
#include<hmLib_v2/dxObject.hpp>
#include<hmLib_v2/dxColorSet.hpp>
#include<HomuraViewer/chrono.hpp>
#include<HomuraViewer/View/DxBUI.hpp>

#include"BatteryData.hpp"

namespace hmr{
	namespace viewer{
		namespace battery{
			//x240*yNONE
			template<unsigned int BatteryNum_>
			class dxosMUI :public dxosBUI{
				using this_data_t = data<BatteryNum_>;
			public:
				hmLib::inquiries::inquiry<this_data_t> inquiry_getData;
				dxosBUIWaitableBoolBut IsDataModeMUIBut;
			public:
				dxosMUI() :dxosBUI("Battery", 30, 55 + 25 * ((BatteryNum_ - 1) / 3)), IsDataModeMUIBut(this){}
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
							auto Data = inquiry_getData();
							dxo.draw(Pint(80, 5), dxoBUITimeStr(this, Pint(70, 20), (boost::format("%.1fV") % (Data.at(0))).str(), Data.time()));
							dxo.draw(Pint(155, 5), dxoBUITimeStr(this, Pint(70, 20), (boost::format("%.1fV") % (Data.at(1))).str(), Data.time()));
						}
						catch(const hmLib::inquiries::unconnected_exception&){
							dxo.draw(Pint(80, 5), dxoStrP(Pint(70, 20), "NoCnct", getClr(error, strobj)));
							dxo.draw(Pint(155, 5), dxoStrP(Pint(70, 20), "NoCnct", getClr(error, strobj)));
						}
					}
					catch(const hmLib::exceptions::exception& Excp){
						dxo.draw(Pint(0, 0), dxoButIO(getSize(), std::string("=ERR=") + Excp.what(), getClr(error, butobj), true, CLR::White, ALI::left));
					}

					return 0;
				}
				int extend_draw(dxO& dxo)override{
					normal_draw(dxo);

					//Cnt0,1は通常時表示、2以降を拡張時表示に変更
					try{
						auto Data = inquiry_getData();
						for(unsigned Cnt = 2; Cnt < BatteryNum_; ++Cnt){
							dxo.draw(Pint(5 + 75 * ((Cnt - 2) % 3), 30 + 25 * ((Cnt - 2) / 3)), dxoBUITimeStr(this, Pint(70, 20), (boost::format("%.1fV") % (Data.at(Cnt))).str(), Data.time()));
						}
					}
					catch(const hmLib::inquiries::unconnected_exception&){
						for(unsigned Cnt = 2; Cnt < BatteryNum_; ++Cnt){
							dxo.draw(Pint(5 + 75 * ((Cnt - 2) % 3), 30 + 25 * ((Cnt - 2) / 3)), dxoStrP(Pint(70, 20), "NoCnct", getClr(error, strobj)));
						}
					}

					return 0;
				}
				status getStatus()const override{
					if(IsDataModeMUIBut.Pic.is_connect() && IsDataModeMUIBut.Pic())return normal;
					else return invalid;
				}
			};
		}
	}
}
#
#endif
