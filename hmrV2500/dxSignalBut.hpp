#ifndef HMRBASEDX_INC
#define HMRBASEDX_INC 100

#include "hmLibVer.hpp"
#include <iostream>
#include <string>
#include <boost/signals2/signal.hpp>
#include <hmLib_v2/hmBasic.hpp>
#include <hmLib_v2/dxObject.hpp>
#include <hmLib_v2/dxArea.hpp>

/* ---- hmrBaseDX-----

hmrBaseDx.hpp :v1_00/130406 amby
	初期バージョン
	Baseのクラスをラッピングする形で対応する

*/
/*
namespace hmr{
	namespace dx{
		typedef boost::signals2::signal<void(void)> clicked_signal;
		class dxoSignalBut:public dxoBut{
		public:
			dxoSignalBut():dxoBut(){return;}
			dxoSignalBut(const Pint& _size,const std::string _str,const dxRGB& _ButColor,
				const dxRGB& _SColor=dxRGB(255,255,255),ALI::ALIGN _Ali=ALI::center,const dxRGB& _SEColor=dxRGB(0,0,0))
				:dxoBut(_size,_str,_ButColor,_SColor,_Ali,_SEColor){
			}
			dxoSignalBut(const Pint& _size,const std::string _str,const dxRGB& _ButColor,
				const dxFont& _font,const dxRGB& _SColor=dxRGB(255,255,255),ALI::ALIGN _Ali=ALI::center,const dxRGB& _SEColor=dxRGB(0,0,0))
				:dxoBut(_size,_str,_ButColor,_font,_SColor,_Ali,_SEColor){
			}
		public:
			clicked_signal signal_click;
		public:
			virtual int draw(Pint _pos,bool _active=true,dxDMode _mode=dxDMode()){
				int ans=dxoBut::draw(_pos,_active,_mode);
				if(ans==-1)signal_click();
				return ans;
			}
		};
		class dxoSignalButLR:public dxoButLR{
		public:
			dxoSignalButLR():dxoButLR(){return;}
			dxoSignalButLR(const Pint& _size,const std::string _str,const dxRGB& _ButColor,
				const dxRGB& _SColor=dxRGB(255,255,255),ALI::ALIGN _Ali=ALI::center,const dxRGB& _SEColor=dxRGB(0,0,0))
				:dxoButLR(_size,_str,_ButColor,_SColor,_Ali,_SEColor){
			}
			dxoSignalButLR(const Pint& _size,const std::string _str,const dxRGB& _ButColor,
				const dxFont& _font,const dxRGB& _SColor=dxRGB(255,255,255),ALI::ALIGN _Ali=ALI::center,const dxRGB& _SEColor=dxRGB(0,0,0))
				:dxoButLR(_size,_str,_ButColor,_font,_SColor,_Ali,_SEColor){
			}
		public:
			clicked_signal signal_click;
			clicked_signal signal_right_click;
		public:
			virtual int draw(Pint _pos,bool _active=true,dxDMode _mode=dxDMode()){
				int ans=dxoButLR::draw(_pos,_active,_mode);
				if(ans==-1)signal_click();
				else if(ans==-10)signal_right_click();
				return ans;
			}
		};
		class dxoSignalButIO:public dxoButIO{
		public:
			clicked_signal signal_click;
		public:
			dxoSignalButIO():dxoButIO(){return;}
			dxoSignalButIO(const Pint& _size,const std::string _str,const dxRGB& _ButColor,bool _OnOff,
				const dxRGB& _SColor=dxRGB(255,255,255),ALI::ALIGN _Ali=ALI::center,const dxRGB& _SEColor=dxRGB(0,0,0))
				:dxoButIO(_size,_str,_ButColor,_OnOff,_SColor,_Ali,_SEColor){
			}
			dxoSignalButIO(const Pint& _size,const std::string _str,const dxRGB& _ButColor,bool _OnOff,
				const dxFont& _font,const dxRGB& _SColor=dxRGB(255,255,255),ALI::ALIGN _Ali=ALI::center,const dxRGB& _SEColor=dxRGB(0,0,0))
				:dxoButIO(_size,_str,_ButColor,_OnOff,_font,_SColor,_Ali,_SEColor){
			}
		public:
			virtual int draw(Pint _pos,bool _active=true,dxDMode _mode=dxDMode()){
				int ans=dxoButIO::draw(_pos,_active,_mode);
				if(ans==1)signal_click();
				return ans;
			}
		};
		class dxoSignalButLRIO:public dxoButLRIO{
		public:
			clicked_signal signal_click;
			clicked_signal signal_right_click;
		public:
			dxoSignalButLRIO():dxoButLRIO(){return;}
			dxoSignalButLRIO(const Pint& _size,const std::string _str,const dxRGB& _ButColor,bool _OnOff,
				const dxRGB& _SColor=dxRGB(255,255,255),ALI::ALIGN _Ali=ALI::center,const dxRGB& _SEColor=dxRGB(0,0,0))
				:dxoButLRIO(_size,_str,_ButColor,_OnOff,_SColor,_Ali,_SEColor){
			}
			dxoSignalButLRIO(const Pint& _size,const std::string _str,const dxRGB& _ButColor,bool _OnOff,
				const dxFont& _font,const dxRGB& _SColor=dxRGB(255,255,255),ALI::ALIGN _Ali=ALI::center,const dxRGB& _SEColor=dxRGB(0,0,0))
				:dxoButLRIO(_size,_str,_ButColor,_OnOff,_font,_SColor,_Ali,_SEColor){
			}
			virtual int draw(Pint _pos,bool _active=true,dxDMode _mode=dxDMode()){
				int ans=dxoButLRIO::draw(_pos,_active,_mode);
				if(ans==1)signal_click();
				else if(ans==10)signal_right_click();
				return ans;
			}
		};
	}
}
*/
#
#endif
