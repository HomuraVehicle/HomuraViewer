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
#include"hmrDxSprite.hpp"
//#include"hmrDxGPSMap.hpp"
#include"hmrGyroLogger.hpp"
#include"hmrDxNavigator.hpp"
#include"hmrDxInfomation.hpp"
#include"DxAutolog.hpp" 

namespace hmr{
	class dxosControlMainDisplay:public dxReObject,public hmoBox{
	public:
		dxosNavigator<hmr::cGyroLogger::iterator> Navigator;
		dxosSprite Sprite;
		dxosInformation Infomation;
		dxosAutolog_480_360 Autolog;

	private:
		class fnControlMain:public dxFn{
		private:
			dxosControlMainDisplay& My;
		public:
			fnControlMain(dxosControlMainDisplay& My_):My(My_){}
		public:
			int fndraw(dxO& dxo)override{
				dxo.draw(Pint(0,360),My.Infomation);
				dxo.draw(Pint(240,0),My.Sprite);
				dxo.draw(Pint(0,0),My.Navigator);
				dxo.draw(Pint(240, 360), My.Autolog);
				return 0;
			}
		};
		dxNormFrame Frame;
	public:
		dxosControlMainDisplay()
			:hmoBox(Pint(720,720))
			,Sprite(Pint(480,360))
			,Navigator()
			,Infomation(Pint(240,360))
			,Autolog()
		{
			Navigator.ini();
		}
		int redraw(dxO& dxo)override{
			fnControlMain Fn(*this);
			Frame.set(Fn,Pint(720,720),CLR::DarkSoftRed);
			dxo.draw(Pint(0,0),Frame);
			return 0;
		}
	};
}
#
#endif
