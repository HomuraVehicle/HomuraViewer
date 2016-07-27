#ifndef HMR_DXGYROEMUI_INC
#define HMR_DXGYROMUI_INC 101
#
/*===hmrDxGyroMUI===
hmrDxGyroMUI v1_01/130705 amby
	BUI化

hmrDxGyroMUI v1_00/130519 iwahori
	とりあえず作成．補正値取得ボタン追加
	ジャイロ用のデータクラスができていないのでほぼAcceleのコピー
*/

#include"hmLibVer.hpp"
#include<string>
#include<sstream>
#include<hmLib_v2/dxObject.hpp>
#include<hmLib_v2/dxColorSet.hpp>
#include<boost/signals2.hpp>
#include<hmLib/signals.hpp>
#include<hmLib/inquiries.hpp>
#include <hmLib/coordinates.hpp>
#include <HomuraViewer/Data.hpp>
#include"hmrDxBUI.hpp"
namespace hmr{
	//x240*yNONE
	class dxosGyroMUI:public dxosBUI{
		typedef hmLib::coordinates3D::angle angle;
	public:
		//hmLib::inquiries::inquiry<angle> inquiry_getGyroData;
		//hmLib::inquiries::inquiry<clock::time_point> inquiry_getTime;
		angle GyroData;
		clock::time_point Time;

		dxosBUIWaitableBoolBut IsDataModeMUIBut;

		dxosBUIWaitableBut CorrectionDataBut;
		hmLib::inquiries::inquiry<angle> inquiry_getCorrectionValue;

	public:
		dxosGyroMUI():dxosBUI("Gyro",55,105),IsDataModeMUIBut(this),CorrectionDataBut(this){}
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
					auto Value= GyroData;//inquiry_getGyroData();
					dxo.draw(Pint(5,30),dxoStrP(Pint(70,20),(boost::format("R:%.1f")%Value.roll).str(),getTimeStrClr(Time)));
					dxo.draw(Pint(80,30),dxoStrP(Pint(70,20),(boost::format("P:%.1f")%Value.pitch).str(),getTimeStrClr(Time)));
					dxo.draw(Pint(155,30),dxoStrP(Pint(70,20),(boost::format("Y:%.1f")%Value.yaw).str(),getTimeStrClr(Time)));
				}catch(const hmLib::inquiries::unconnected_exception&){
					dxo.draw(Pint(5,30),dxoStrP(Pint(70,20),"NoCnct",getClr(error,strobj)));
					dxo.draw(Pint(80,30),dxoStrP(Pint(70,20),"NoCnct",getClr(error,strobj)));
					dxo.draw(Pint(155,30),dxoStrP(Pint(70,20),"NoCnct",getClr(error,strobj)));
				}

			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(5,5),dxoButIO(getSize(),std::string("=ERR=")+Excp.what(),getClr(error,butobj),true,CLR::White,ALI::left));
			}

			return 0;
		}
		int extend_draw(dxO& dxo)override{
			normal_draw(dxo);

			try{
				CorrectionDataBut.set(Pint(70,20),"補正");
				dxo.draw(Pint(5,55),CorrectionDataBut);
			}catch(const hmLib::inquiries::unconnected_exception&){
				dxo.draw(Pint(5,55),dxoStrP(Pint(70,20),"NoCnct",getClr(error,strobj)));
			}

			try{
				auto Value=inquiry_getCorrectionValue();
				dxo.draw(Pint(5,80),dxoStrP(Pint(70,20),(boost::format("補R:%.1f")%Value.roll).str(),getTimeStrClr(Time)));
				dxo.draw(Pint(80,80),dxoStrP(Pint(70,20),(boost::format("補P:%.1f")%Value.pitch).str(),getTimeStrClr(Time)));
				dxo.draw(Pint(155,80),dxoStrP(Pint(70,20),(boost::format("補Y:%.1f")%Value.yaw).str(),getTimeStrClr(Time)));
			}catch(const hmLib::inquiries::unconnected_exception&){
				dxo.draw(Pint(5,80),dxoStrP(Pint(70,20),"NoCnct",getClr(error,strobj)));
				dxo.draw(Pint(80,80),dxoStrP(Pint(70,20),"NoCnct",getClr(error,strobj)));
				dxo.draw(Pint(155,80),dxoStrP(Pint(70,20),"NoCnct",getClr(error,strobj)));
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

	private:
		hmLib::signals::unique_connections SignalConnections;
		void setData(clock::time_point time_, angle data_){
			Time = time_;
			GyroData = data_;
		}
	// slot 
	public:
		void slot_getData(boost::signals2::signal<void(angle, double, clock::time_point)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](angle data_, double d_, clock::time_point time_)->void{this->setData(time_, data_);}));	
		}
	};
}
#
#endif
