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
#include<hmLib_v2/dxObject.hpp>
#include<hmLib_v2/dxColorSet.hpp>
#include<boost/signals2.hpp>
#include<hmLib/signals.hpp>
#include<hmLib/inquiries.hpp>
#include <HomuraViewer/Data.hpp>
#include"hmrDxBUI.hpp"
#include"hmrDxTools.hpp"
#include<array>

namespace hmr{
	//x240*yNONE
	template<unsigned int BatteryNum_>
	class dxosBatteryMUI:public dxosBUI{
	public:
		std::array<hmLib::inquiries::inquiry<double>,BatteryNum_> inquiry_getBatteryData;
		hmLib::inquiries::inquiry<clock::time_point> inquiry_getTime;
		dxosBUIWaitableBoolBut IsDataModeMUIBut;
	public:
		dxosBatteryMUI():dxosBUI("Battery",30,55+25*((BatteryNum_-1)/3)),IsDataModeMUIBut(this){}
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
					dxo.draw(Pint(80,5),dxoBUITimeStr(this,Pint(70,20),(boost::format("%.1fV")%(inquiry_getBatteryData[0]())).str(),inquiry_getTime()));
				}catch(const hmLib::inquiries::unconnected_exception&){
					dxo.draw(Pint(80,5),dxoStrP(Pint(70,20),"NoCnct",getClr(error,strobj)));
				}

				try{
					dxo.draw(Pint(155,5),dxoBUITimeStr(this,Pint(70,20),(boost::format("%.1fV")%(inquiry_getBatteryData[1]())).str(),inquiry_getTime()));
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

			for(unsigned Cnt=2;Cnt<BatteryNum_;++Cnt){
//Cnt0,1は通常時表示、2以降を拡張時表示に変更
				try{
					dxo.draw(Pint(5+75*((Cnt-2)%3),30+25*((Cnt-2)/3)),dxoBUITimeStr(this,Pint(70,20),(boost::format("%.1fV")%(inquiry_getBatteryData[Cnt]())).str(),inquiry_getTime()));
				}catch(const hmLib::inquiries::unconnected_exception&){
					dxo.draw(Pint(5+75*((Cnt-2)%3),30+25*((Cnt-2)/3)),dxoStrP(Pint(70,20),"NoCnct",getClr(error,strobj)));
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
#
#endif
