#ifndef HMR_DXNAVIGATOR_INC
#define HMR_DXNAVIGATOR_INC 100
#
/*===dxNavigator===
加速度計、コンパス、ジャイロの統合描画クラス
v1_00/131112 hmIto
	GyroCompassを追加
*/
#include<boost/signals2/signal.hpp>
#include<hmLib_v3_05/inquiries.hpp>
#include"hmLibVer.hpp"
#include<hmLib_v2/dxObject.hpp>
#include<hmLib_v2/dxArea.hpp>
#include<hmLib_v2/dxColorSet.hpp>
#include"coordinates.hpp"
namespace hmr{
	template<typename gyro_iterator_>
	class dxosNavigator:public dxReObject,public hmoBox{
		typedef hmLib::coordinates3D::angle angle;
	private:
		dxGraph FrameGrp;
		dxGraph AcceleGrp;
		dxGraph CompassGrp;
		dxGraph GyroCompassGrp;
		dxArea AcceleArea;
		dxArea CompassArea;
		bool BackMode;
	public:
		class dxfnAccele:public dxFn{
			dxGraph& AcceleGrp;
			hmLib::coordinates3D::polar Accele;
		public:
			dxfnAccele(dxGraph& AcceleGrp_,hmLib::coordinates3D::polar Accele_)
				:AcceleGrp(AcceleGrp_)
				,Accele(Accele_){
			}
			int fndraw(dxO& dxo){
				dxo.draw(Pint(120,120),dxoRotGraph(AcceleGrp,1.0,pi()/2-Accele.theta,Pint(120,static_cast<int>(-Accele.phi/pi()*180*4)+960)));
				return 0;
			}
		};
		class dxfnCompass:public dxFn{
		public:
			bool BackMode;
		private:
			dxGraph& CompassGrp;
			dxGraph& GyroCompassGrp;
			hmLib::coordinates3D::polar Compass;
			hmLib::inquiries::range_inquiry<gyro_iterator_>& Gyro;
			hmLib::inquiries::inquiry<angle>& GyroCompass;
		public:
			dxfnCompass(dxGraph& CompassGrp_,dxGraph& GyroCompassGrp_,hmLib::coordinates3D::polar Compass_,hmLib::inquiries::inquiry<angle>& GyroCompass_,hmLib::inquiries::range_inquiry<gyro_iterator_>& Gyro_,bool BackMode_)
				:CompassGrp(CompassGrp_)
				,GyroCompassGrp(GyroCompassGrp_)
				,Compass(Compass_)
				,Gyro(Gyro_)
				,GyroCompass(GyroCompass_)
				,BackMode(BackMode_){
			}
			int fndraw(dxO& dxo){
				double Sign=BackMode?-1:1;

				dxo.draw(Pint(120,120),dxoRotGraph(CompassGrp,1.0,Sign*(-Compass.phi)+pi()/2,Pint(120,120)));

				if(GyroCompass.is_connect()){
					dxo.draw(Pint(120,120),dxoRotGraph(GyroCompassGrp,1.0,-Sign*GyroCompass().yaw,Pint(120,120)));
				}				

				if(Gyro.is_connect()){
					hmr::clock::time_point now=hmr::clock::now();
					hmLib::coordinates3D::angle Angle(0,0,0);

					std::lock_guard<decltype(Gyro)> Lock(Gyro);
					auto itr=Gyro.begin();
					for(;itr!=Gyro.end();++itr){
						std::chrono::seconds Sec=std::chrono::duration_cast<std::chrono::seconds>(now-itr->first);
						if(Sec>std::chrono::seconds(5))break;
						Angle&=(~itr->second);
					}

					if(itr!=Gyro.end()){
						while(true){
							std::chrono::seconds Sec=std::chrono::duration_cast<std::chrono::seconds>(now-itr->first);
							Angle&=itr->second;
							dxo.draw(Pint(120,120),dxoPLine(Pint(Sign*-80*sin(Angle.yaw),Sign*-80*cos(Angle.yaw)),Pint(Sign*40*sin(Angle.yaw),Sign*40*cos(Angle.yaw)),dxHSV(static_cast<int>(Sec.count())*30,196,196)),true,dxDMode(196));
							if(itr==Gyro.begin())break;
							--itr;
						}
					}
				}

				return 0;
			}
		};
	public:
		hmLib::inquiries::inquiry<hmLib::coordinates3D::polar> inquiry_accele;
		hmLib::inquiries::inquiry<hmLib::coordinates3D::polar> inquiry_compass;
		hmLib::inquiries::inquiry<angle> inquiry_gyrocompass;
		hmLib::inquiries::range_inquiry<gyro_iterator_> range_inquiry_gyro;
		boost::signals2::signal<void(void)> signal_clearGyroCompassAngle;
	public:
		dxosNavigator():hmoBox(Pint(240,360)),BackMode(false){}
		int redraw(dxO& dxo)override{
			dxfnAccele FnAccele(AcceleGrp,inquiry_accele());
			AcceleArea.set(FnAccele,Pint(240,240));
			dxo.draw(Pint(0,0),AcceleArea);

			dxfnCompass FnCompass(CompassGrp,GyroCompassGrp,inquiry_compass(),inquiry_gyrocompass,range_inquiry_gyro,BackMode);
			CompassArea.set(FnCompass,Pint(240,120));
			dxo.draw(Pint(0,240),CompassArea);

			dxo.draw(Pint(0,0),dxoGraph(FrameGrp,true));

			if(dxo.draw(Pint(0,230),dxoButIO(Pint(20,20),"",CLR::Blue,BackMode))==1)BackMode=!BackMode;
			if(dxo.draw(Pint(220,230),dxoBut(Pint(20,20),"",CLR::Red))==-1)signal_clearGyroCompassAngle();

			return 0;
		}
		void ini(){
			AcceleGrp.open("hmr/DxNavigator/Accele2.png");
			CompassGrp.open("hmr/DxNavigator/Compass2.png");
			GyroCompassGrp.open("hmr/DxNavigator/GyroCompass2.png");
			FrameGrp.open("hmr/DxNavigator/Frame2.png");
		}
	};
}
#
#endif
