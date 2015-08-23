#ifndef HMR_DXCONTROLMAINDISPLAY_INC
#define HMR_DXCONTROLMAINDISPLAY_INC
#
/*===hmrDxControlMainDisplay===
操縦画面用メインディスプレイ
480*360のLが2スロット
240*360のSが2スロット

dxosControlMainDisplay
	L:GPSマップ
	L:カメラ表示用
	S:航行計器
	S:制御警告板
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
