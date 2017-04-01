#ifndef HMR_DXCONTROLMAINDISPLAY_INC
#define HMR_DXCONTROLMAINDISPLAY_INC
#
/*===hmrDxControlMainDisplay===
���c��ʗp���C���f�B�X�v���C
480*360��L��2�X���b�g
240*360��S��2�X���b�g

dxosControlMainDisplay
	L:GPS�}�b�v
	L:�J�����\���p
	S:�q�s�v��
	S:����x����
*/
#include"hmLibVer.hpp"
#include<hmLib_v2/dxObject.hpp>
#include<HomuraViewer/Camera/DxCamera.hpp>
#include<HomuraViewer/Gyro/GyroLogger.hpp>
#include<HomuraViewer/GPS/DxGPSMap.hpp>
#include"DxInfomation.hpp"
#include"DxNavigator.hpp"

namespace hmr{
	namespace viewer{
		namespace view{
			class dxosControlMainDisplay :public dxReObject, public hmoBox{
			public:
				dxosNavigator<gyro::cGyroLogger::iterator> Navigator;
				camera::dxosCamera Camera;
				dxosInformation Infomation;
				gps::dxosGPSMap GPSMap;
			private:
				class fnControlMain :public dxFn{
				private:
					dxosControlMainDisplay& My;
				public:
					fnControlMain(dxosControlMainDisplay& My_) :My(My_){}
				public:
					int fndraw(dxO& dxo)override{
						dxo.draw(Pint(0, 360), My.Infomation);
						dxo.draw(Pint(240, 0), My.Camera);
						dxo.draw(Pint(0, 0), My.Navigator);
						dxo.draw(Pint(240, 360), My.GPSMap);
						return 0;
					}
				};
				dxNormFrame Frame;
			public:
				dxosControlMainDisplay()
					: hmoBox(Pint(720, 720))
					, Camera(Pint(480, 360))
					, Navigator()
					, Infomation(Pint(240, 360))
					, GPSMap(Pint(480, 360)){
					Navigator.ini();
				}
				int redraw(dxO& dxo)override{
					fnControlMain Fn(*this);
					Frame.set(Fn, Pint(720, 720), CLR::DarkSoftRed);
					dxo.draw(Pint(0, 0), Frame);
					return 0;
				}
			};
		}
	}
}
#
#endif