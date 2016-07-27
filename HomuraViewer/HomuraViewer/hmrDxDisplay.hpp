#ifndef HMR_DXDISPLAY_INC
#define HMR_DXDISPLAY_INC 101
#
/*===hmrDxMainSide===
Side‰æ–Ê‚ÆMain‰æ–Ê‚ÌØ‚è‘Ö‚¦“™‚ğ’²®‚·‚éƒNƒ‰ƒX

hmrDxMainSide:v1_01/130713 iwahori
	Pad‚Å‚ÌØ‚è‘Ö‚¦—p‚ÉMainNo‚ÆSideNo‚ğ‘«‚µˆø‚«‚·‚éŠÖ”‚ğì¬
hmrDxMainSide:v1_00/130525 hmIto
	dxosMainSide‚ğ’Ç‰Á
*/
#include<vector>
#include<hmLib/signals.hpp>
#include"hmLibVer.hpp"
#include<hmLib_v2/dxObject.hpp>

namespace hmr{
	namespace viewer{
		class dxosDisplay :public dxReObject, public hmoBox{
		private:
			std::vector<dxObject*> Main;
			std::vector<dxObject*> Side;
			unsigned int MainNo;
			unsigned int SideNo;
			unsigned int SideWidth;
		public:
			dxosDisplay(Pint MainSize_, Pint SideSize_) :hmoBox(Pint(MainSize_.x + SideSize_.x, MainSize_.y)), SideWidth(SideSize_.x), MainNo(0), SideNo(0){}
			void registMain(dxObject* MainObj_){
				Main.push_back(MainObj_);
			}
			void registSide(dxObject* SideObj_){
				Side.push_back(SideObj_);
			}
		public:
			virtual int redraw(dxO& dxo)override{
				try{
					if(SideNo < Side.size()){
						dxo.draw(Pint(0, 0), *Side.at(SideNo));
					}
				}
				catch(const std::exception& Excp_){
					dxo.draw(Pint(0, 0), dxoStrP(Pint(SideWidth, 20), "===ERROR==="));
					dxo.draw(Pint(0, 20), dxoStrP(Pint(SideWidth, size.y - 20), Excp_.what()));
				}
				try{
					if(MainNo < Main.size()){
						dxo.draw(Pint(SideWidth, 0), *Main.at(MainNo));
					}
				}
				catch(const std::exception& Excp_){
					dxo.draw(Pint(SideWidth, 0), dxoStrP(Pint(size.x - SideWidth, 20), "===ERROR==="));
					dxo.draw(Pint(SideWidth, 20), dxoStrP(Pint(size.x - SideWidth, size.y - 20), Excp_.what()));
				}

				return 0;
			}
		private:
			hmLib::signals::unique_connections SignalConnections;
			void setMainNo(unsigned int No_){
				if(No_ < Main.size())MainNo = No_;
			}
			void plusMainNo(void){
				if(MainNo + 1 < Main.size())++MainNo;
			}
			void minusMainNo(void){
				if(MainNo > 0)--MainNo;
			}
			void setSideNo(unsigned int No_){
				if(No_ < Side.size())SideNo = No_;
			}
			void plusSideNo(void){
				if(SideNo + 1 < Side.size())++SideNo;
			}
			void minusSideNo(void){
				if(SideNo > 0)--SideNo;
			}
		public:
			void slot_setMainNo(boost::signals2::signal<void(unsigned int)>& Signal_){
				SignalConnections(hmLib::signals::connect(Signal_, [&](unsigned int No_)->void{this->setMainNo(No_); }));
			}
			void slot_plusMainNo(boost::signals2::signal<void(void)>& Signal_){
				SignalConnections(hmLib::signals::connect(Signal_, [&](void)->void{this->plusMainNo(); }));
			}
			void slot_minusMainNo(boost::signals2::signal<void(void)>& Signal_){
				SignalConnections(hmLib::signals::connect(Signal_, [&](void)->void{this->minusMainNo(); }));
			}
			void slot_setSideNo(boost::signals2::signal<void(unsigned int)>& Signal_){
				SignalConnections(hmLib::signals::connect(Signal_, [&](unsigned int No_)->void{this->setSideNo(No_); }));
			}
			void slot_plusSideNo(boost::signals2::signal<void(void)>& Signal_){
				SignalConnections(hmLib::signals::connect(Signal_, [&](void)->void{this->plusSideNo(); }));
			}
			void slot_minusSideNo(boost::signals2::signal<void(void)>& Signal_){
				SignalConnections(hmLib::signals::connect(Signal_, [&](void)->void{this->minusSideNo(); }));
			}
			void slot_setMainNo0(boost::signals2::signal<void(void)>& Signal_){
				SignalConnections(hmLib::signals::connect(Signal_, [&](void)->void{this->setMainNo(0); }));
			}
			void slot_setSideNo0(boost::signals2::signal<void(void)>& Signal_){
				SignalConnections(hmLib::signals::connect(Signal_, [&](void)->void{this->setSideNo(0); }));
			}
			void slot_setMainNo1(boost::signals2::signal<void(void)>& Signal_){
				SignalConnections(hmLib::signals::connect(Signal_, [&](void)->void{this->setMainNo(1); }));
			}
			void slot_setSideNo1(boost::signals2::signal<void(void)>& Signal_){
				SignalConnections(hmLib::signals::connect(Signal_, [&]()->void{this->setSideNo(1); }));
			}
			void slot_setMainNo2(boost::signals2::signal<void(void)>& Signal_){
				SignalConnections(hmLib::signals::connect(Signal_, [&]()->void{this->setMainNo(2); }));
			}
			void slot_setSideNo2(boost::signals2::signal<void(void)>& Signal_){
				SignalConnections(hmLib::signals::connect(Signal_, [&](void)->void{this->setSideNo(2); }));
			}
		};
	}
}
#
#endif
