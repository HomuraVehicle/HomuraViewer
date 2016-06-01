#ifndef HMR_DXCOMPASSMUI_INC
#define HMR_DXCOMPASSMUI_INC 101
#
/*===hmrDxCompassMUI===
hmrDxCompassMUI v1_01/130519 iwahori
	補正値取得ボタン追加（場所はてきとう）
hmrDxCompassMUI v1_00/130511 hmIto
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
#include"hmrCompassData.hpp"

namespace hmr{
	//x240*yNONE
	class dxosCompassMUI:public dxosBUI{
		typedef hmLib::coordinates3D::position position;
		typedef hmLib::coordinates3D::polar polar;
		hmLib::signals::unique_connections SignalConnections;

		position pos;
		clock::time_point time;

		void setCompassData_and_time(position pos_, clock::time_point time_){
			pos = pos_;
			time = time_;
		}
	public:
		//hmLib::inquiries::inquiry<position> inquiry_getCompassPossition;
		//hmLib::inquiries::inquiry<polar> inquiry_getCompassPolar;
		//hmLib::inquiries::inquiry<clock::time_point> inquiry_getTime;
		void slot_setCompassData_and_time(boost::signals2::signal<void(position, clock::time_point)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](position pos, clock::time_point t)->void{this->setCompassData_and_time(pos,t);}));		
		}

		dxosBUIWaitableBoolBut IsDataModeMUIBut;

//		dxosBUIWaitableBoolBut CorrectionModeBut;
//		dxosBUIWaitableBut CorrectionDataBut;

		hmLib::inquiries::inquiry<bool> inquiry_isCorrectionMode;
		hmLib::inquiries::inquiry<unsigned int> inquiry_getCorrectionNum;
		hmLib::inquiries::inquiry<position> inquiry_getCorrectionValue;
		boost::signals2::signal<void(void)> signal_setCorrectionMode;
		boost::signals2::signal<void(void)> signal_setCorrectionFromFile;
		boost::signals2::signal<void(void)> signal_addCorrection;
	public:
		dxosCompassMUI():dxosBUI("Compass",30,105),IsDataModeMUIBut(this){}
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
					auto Pol= cCompass::transPos_to_Polar(pos);
					dxo.draw(Pint(80,5),dxoStrP(Pint(70,20),(boost::format("Y:%.1f")%Pol.phi).str(),getTimeStrClr(time)));
					dxo.draw(Pint(155,5),dxoStrP(Pint(70,20),(boost::format("P:%.1f")%Pol.theta).str(),getTimeStrClr(time)));
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
				auto Value= pos;
				dxo.draw(Pint(5,30),dxoStrP(Pint(70,20),(boost::format("X:%.1f")%Value.x).str(),getTimeStrClr(time)));
				dxo.draw(Pint(80,30),dxoStrP(Pint(70,20),(boost::format("Y:%.1f")%Value.y).str(),getTimeStrClr(time)));
				dxo.draw(Pint(155,30),dxoStrP(Pint(70,20),(boost::format("Z:%.1f")%Value.z).str(),getTimeStrClr(time)));
			}catch(const hmLib::inquiries::unconnected_exception&){
				dxo.draw(Pint(5,30),dxoStrP(Pint(70,20),"NoCnct",getClr(error,strobj)));
				dxo.draw(Pint(80,30),dxoStrP(Pint(70,20),"NoCnct",getClr(error,strobj)));
				dxo.draw(Pint(155,30),dxoStrP(Pint(70,20),"NoCnct",getClr(error,strobj)));
			}

			try{
				bool IsCorrection=inquiry_isCorrectionMode();
				unsigned int CorrectionNum=inquiry_getCorrectionNum();
				if(dxo.draw(Pint(5,55),dxoButIO(Pint(70,20),IsCorrection?"補正終了":"補正開始",getClr(IsCorrection?active:normal,butobj),IsCorrection&&(CorrectionNum%2==0)))==1){
					signal_setCorrectionMode();
				}
				if(dxo.draw(Pint(80,55),dxoBut(Pint(70,20),(boost::format("補正:%d")%CorrectionNum).str(),getClr(IsCorrection?normal:invalid,butobj)),IsCorrection)==-1){
					signal_addCorrection();
				}
				if(dxo.draw(Pint(155,55),dxoBut(Pint(70,20),"文書補正",getClr((!IsCorrection)?normal:invalid,butobj)),!IsCorrection)==-1){
					signal_setCorrectionFromFile();
				}
			}catch(const hmLib::inquiries::unconnected_exception&){
				dxo.draw(Pint(5,55),dxoStrP(Pint(225,20),"NoCnct",getClr(error,strobj)));
			}

			try{
				auto Value=inquiry_getCorrectionValue();
				dxo.draw(Pint(5,80),dxoStrP(Pint(70,20),(boost::format("補X:%.1f")%Value.x).str(),getTimeStrClr(time)));
				dxo.draw(Pint(80,80),dxoStrP(Pint(70,20),(boost::format("補Y:%.1f")%Value.y).str(),getTimeStrClr(time)));
				dxo.draw(Pint(155,80),dxoStrP(Pint(70,20),(boost::format("補Z:%.1f")%Value.z).str(),getTimeStrClr(time)));
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
