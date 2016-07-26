#include "Sample_for_vmc_hmrCom.h"
#include<hmLib_v2/dxObject.hpp>

namespace hmr{
	namespace dx{
		class dxoCom:public dxObject,public hmoBox{
		private:
			class dxfPacSelFra:public dxFn{
			private:
				const vecPacket& rPac;
				hmTime& rSelPacTime;
				int& rSelDatNo;
				bool NewPacFlag;
			public:
				dxfPacSelFra(const vecPacket& _rPac,hmTime& _rSelPacTime,int& _rSelDatNo)
					:rPac(_rPac),rSelPacTime(_rSelPacTime),rSelDatNo(_rSelDatNo){
						NewPacFlag=false;
				}
				int fndraw(dxO& dxo);
			};
			class dxfPacFra:public dxFn{
			private:
				const int& rFraPosY;
				const vecPacket& rPac;
				hmTime& rSelPacTime;
				int& rSelDatNo;
			public:
				dxfPacFra(const int& _rFraPosY,const vecPacket& _rPac,hmTime& _rSelPacTime,int& _rSelDatNo)
					:rFraPosY(_rFraPosY),rPac(_rPac),rSelPacTime(_rSelPacTime),rSelDatNo(_rSelDatNo){return;}
				int fndraw(dxO& dxo);
			};
			class dxfDatSelFra:public dxFn{
			private:
				const vecPacket& rPac;
				const hmTime& rSelPacTime;
				int& rSelDatNo;
			public:
				dxfDatSelFra(const vecPacket& _rPac,const hmTime& _rSelPacTime,int& _rSelDatNo)
					:rPac(_rPac),rSelPacTime(_rSelPacTime),rSelDatNo(_rSelDatNo){return;}
				int fndraw(dxO& dxo);
			};
			class dxfDatFra:public dxFn{
			private:
				const int& rFraPosY;
				const vecPacket& rPac;
				const hmTime& rSelPacTime;
				const int& rSelDatNo;
			public:
				dxfDatFra(const int& _rFraPosY,const vecPacket& _rPac,const hmTime& _rSelPacTime,const int& _rSelDatNo)
					:rFraPosY(_rFraPosY),rPac(_rPac),rSelPacTime(_rSelPacTime),rSelDatNo(_rSelDatNo){return;}
				int fndraw(dxO& dxo);
			};
			class dxfDatBinFra:public dxFn{
			private:
				const vecPacket& rPac;
				const hmTime& rSelPacTime;
				const int& rSelDatNo;
			public:
				dxfDatBinFra(const vecPacket& _rPac,const hmTime& _rSelPacTime,const int& _rSelDatNo)
					:rPac(_rPac),rSelPacTime(_rSelPacTime),rSelDatNo(_rSelDatNo){return;}
				int fndraw(dxO& dxo);
			};
			class dxfDatDrawFra:public dxFn{
			private:
				ComTypePMap& rComTypePMap;
				const vecPacket& rPac;
				const hmTime& rSelPacTime;
				const int& rSelDatNo;
			public:
				dxfDatDrawFra(ComTypePMap& _rMap,const vecPacket& _rPac,const hmTime& _rSelPacTime,const int& _rSelDatNo)
					:rComTypePMap(_rMap),rPac(_rPac),rSelPacTime(_rSelPacTime),rSelDatNo(_rSelDatNo){return;}
				int fndraw(dxO& dxo);
			};
		private:
			hmTime SelPacTime;
			int SelDatNo;
			dxfPacSelFra fPacSelFra;
			dxScrlFrame PacSelFra;
			dxfPacFra fPacFra;
			dxNormFrame PacFra;
			dxfDatSelFra fDatSelFra;
			dxScrlFrame DatSelFra;
			dxfDatFra fDatFra;
			dxNormFrame DatFra;
			dxfDatBinFra fDatBinFra;
			dxScrlFrame DatBinFra;
			dxfDatDrawFra fDatDrawFra;
			dxScrlFrame DatDrawFra;
		public:
			dxoCom(Pint _size,cCom& _rCom,const dxRGB& _Clr)
				:hmoBox(_size)
				,fPacSelFra(_rCom.Packets,SelPacTime,SelDatNo)
				,PacSelFra(fPacSelFra,Pint(120,_size.y),_Clr)
				,fPacFra(PacSelFra.viewpos.y,_rCom.Packets,SelPacTime,SelDatNo)
				,PacFra(fPacFra,Pint(_size.x-120,_size.y),_Clr)
				,fDatSelFra(_rCom.Packets,SelPacTime,SelDatNo)
				,DatSelFra(fDatSelFra,Pint(90,_size.y-240),_Clr)
				,fDatFra(DatSelFra.viewpos.x,_rCom.Packets,SelPacTime,SelDatNo)
				,DatFra(fDatFra,Pint(_size.x-210,_size.y-240),_Clr)
				,fDatBinFra(_rCom.Packets,SelPacTime,SelDatNo)
				,DatBinFra(fDatBinFra,Pint(_size.x-210,_size.y-240),_Clr)
				,fDatDrawFra(_rCom.ComTypes,_rCom.Packets,SelPacTime,SelDatNo)
				,DatDrawFra(fDatDrawFra,Pint(_size.x-120,240),_Clr){
					SelPacTime.set(-1,0);
					SelDatNo=-1;
			}
			int draw(Pint _pos,bool _active=true,dxDMode _mode=dxDMode());
		};
		class dxfComTypes:public dxFn{
		private:
			cCom& rCom;
			dxRGB Clr;
		protected:
			virtual int fndraw(dxO& dxo);
		public:
			dxfComTypes(const dxRGB& _Clr, cCom& _Com):rCom(_Com){Clr=_Clr;}
		};
		//size 200*140
		class dxfComConfig_200_140:public dxFn{
		private:
			cCom& rCom;
			dxRGB InClr,OutClr;
		public:
			dxfComConfig_200_140(const dxColor& _InClr,const dxColor& _OutClr,cCom& _Com):rCom(_Com){
				InClr=_InClr;
				OutClr=_OutClr;
			}
			virtual int fndraw(dxO& dxo);
		};
		//size 
		class dxfComConfig_385_45:public dxFn{
		private:
			cCom& rCom;
			dxRGB InClr,OutClr;
		public:
			dxfComConfig_385_45(const dxColor& _InClr,const dxColor& _OutClr,cCom& _Com):rCom(_Com){
				InClr=_InClr;
				OutClr=_OutClr;
			}
			virtual int fndraw(dxO& dxo);
		};
	}
}