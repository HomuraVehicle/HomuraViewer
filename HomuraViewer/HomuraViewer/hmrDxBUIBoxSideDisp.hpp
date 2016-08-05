#ifndef HMR_DXBUIBOXSIDEDISP_INC
#define HMR_DXBUIBOXSIDEDISP_INC 100
#
#include"hmLibVer.hpp"
#include<hmLib/inquiries.hpp>
#include<hmLib/signals.hpp>
#include<hmLib/exceptions.hpp>
#include<hmLib_v2/dxColorSet.hpp>
#include<hmLib_v2/dxObject.hpp>
#include<hmLib_v2/dxArea.hpp>
#include"hmrDxBUI.hpp"
#include <HomuraViewer/chrono.hpp>
namespace hmr{
	namespace viewer{
		//dxosMUIを登録すると、サイドバー(240*720)として表示してくれる。
		class dxosBUIBoxSideDisplay :public dxReObject, public hmoBox{
			typedef std::vector<dxosBUI*> containor;
			typedef containor::iterator iterator;
		private:
			containor BUIPtrVec;
			dxScrlFrame BUIFrame;
		public:
			color_set ClrSet;
		public:
			class dxFnBUI :public dxFn{
				iterator Begin;
				iterator End;
				const color_set* pClrSet;
			public:
				dxFnBUI(iterator Begin_, iterator End_, const color_set* pClrSet_) :Begin(Begin_), End(End_), pClrSet(pClrSet_){}
				virtual int fndraw(dxO& dxo)override{
					int Pos = 0;
					for(auto Itr = Begin; Itr != End; ++Itr){
						dxRGB Clr = (*Itr)->getClr((*Itr)->getStatus(), dxosBUI::butobj);
						dxHLS HLSClr = Clr;
						HLSClr.lgt /= 2;

						if(dxo.draw(Pint(0, Pos), dxoButIO(Pint(240, 20), (*Itr)->name(), Clr, true)) == 1){
							(*Itr)->setExtend(!(*Itr)->getExtend());
						}

						dxo.draw(Pint(0, Pos + 20), dxoButLRIO(Pint(240, (*Itr)->getSize().y), "", HLSClr, (*Itr)->getExtend()));

						dxo.draw(Pint(0, Pos + 20), *(*Itr));
						Pos += (*Itr)->getSize().y + 20;
					}
					return 0;
				}
			};
		public:
			void regist(dxosBUI* pBUI_){
				pBUI_->setClrSet(&ClrSet);
				BUIPtrVec.push_back(pBUI_);
			}
		public:
			dxosBUIBoxSideDisplay() :hmoBox(Pint(240, 720)){
				ClrSet.set(
					CLR::DarkSoftRed,
					CLR::SoftRed,
					CLR::LightSoftBlue,
					CLR::DeepSoftGreen,
					CLR::SoftRed,
					CLR::Gray,
					CLR::ThinSoftBlue,
					CLR::SoftGreen,
					CLR::LightSoftRed,
					CLR::LightGray,
					CLR::White,
					CLR::LightGray,
					CLR::Gray
				);
			}
			virtual int redraw(dxO& dxo){
				try{
					dxFnBUI Fn(BUIPtrVec.begin(), BUIPtrVec.end(), &ClrSet);
					BUIFrame.set(Fn, size, ClrSet.Background);

					Pint MPos = dxo.GtoD(dx::getMosPos());
					if(BUIFrame.pIn(Pint(0, 0), MPos)){
						BUIFrame.viewpos.y -= 10 * dx::getMosH();
					}
					dxo.draw(Pint(0, 0), BUIFrame);

				}
				catch(const std::exception& Excp){
					dxo.draw(Pint(0, 0), dxoButIO(size, Excp.what(), CLR::Red, false));
				}
				return 0;
			}
		};
	}
}
#
#endif
