#ifndef HMRIODRAW_INC
#define HMRIODRAW_INC 100
#
/*===hmrIODraw===
hmrIOの描画専用クラス群

hmrIODraw:v1_00/130227 hmIto
	とりあえず、hmrIOから分離
*/
#include"hmrIO.h"

namespace hmr{
	class dxfIOSend:public dxFn{
	private:
		hmrIO& rIO;
		dxRGB Clr;
	public:
		dxInput Inp;
	protected:
		int fndraw(dxO& dxf);
	public:
		dxfIOSend(hmrIO& _io,dxColor& _Clr)
			:rIO(_io)
			,Clr(_Clr){
			Inp.make(1024);
		}
	};
	class dxfIOSend_x_30:public dxFn{
	private:
		hmrIO& rIO;
		dxRGB Clr;
		int SizeX;
	public:
		dxInput Inp;
	protected:
		int fndraw(dxO& dxf);
	public:
		dxfIOSend_x_30(hmrIO& _io,const dxColor& _Clr,int _SizeX):rIO(_io){
			Clr=_Clr;
			Inp.make(1024);
			SizeX=_SizeX;
		}
	};
	class dxfIOLog:public dxFn{
	public:
		class cInputSetter{
		private:
			dxInput& rInp;
		public:
			cInputSetter(dxInput& rInp_):rInp(rInp_){}
			cInputSetter(const cInputSetter& My_):rInp(My_.rInp){}
			void operator()(const std::string Str_){
				rInp.setStr(Str_);
				rInp.active();
			}
		};
	private:
		dxRGB InClr;
		dxRGB OutClr;
		hmrIO& rIO;
		cInputSetter InpSetter;
	protected:
		int fndraw(dxO& dxf);
	public:
		dxfIOLog(hmrIO& _io,cInputSetter InpSetter_,dxColor& _InClr,dxColor& _OutClr)
			:rIO(_io)
			,InpSetter(InpSetter)
			,InClr(_InClr)
			,OutClr(_OutClr){
		}
	};
	//size(80,50)
	class dxfIOSta_80_60:public dxFn{
	private:
		hmrIO& rIO;
		dxRGB InClr;
		dxRGB OutClr;
	protected:
		virtual int fndraw(dxO& dxo){
			dxo.draw(Pint(0,0),dxoBox(Pint(80,60),dxRGB(255,255,255),0),true,dxDMode(128));
			if(rIO.gfail()){
				dxo.draw(Pint(0,0),dxoStrP(Pint(80,60),"未接続"),true,dxDMode(196));
			}else{
				std::deque<hmrIO::iologType>::reverse_iterator itr=rIO.ioLogGate.Log.rbegin(),enditr=rIO.ioLogGate.Log.rend();
				static dxColor* pClr;
				static std::string str;

				for(int i=0;i<3;i++){
					if(itr==enditr)break;

					if(itr->getIO()){
						pClr=&InClr;
						str="In";
					}else{
						pClr=&OutClr;
						str="Out";
					}

					hmTime tmpTime=hm::time()-itr->getEndTime();
					if(tmpTime.time()>=0 && tmpTime.time()<=5){
						dxo.draw(Pint(0,i*20),dxoBox(Pint(80,20),*pClr,1),true,dxDMode(128-static_cast<int>(tmpTime.time())*10));
					}
					if(!(itr->getEndFlag())){
						dxo.draw(Pint(0,i*20),dxoBox(Pint(80,20),*pClr,0),true,dxDMode(196));
					}

					dxo.draw(Pint(0,i*20),dxoStrP(Pint(30,20),str,*pClr));
					dxo.draw(Pint(30,i*20),dxoStrP(Pint(45,20),hmstd::fStr("%d",itr->getStr().size()),*pClr,ALI::right));

					++itr;
				}
			}
			return 0;
		}
	public:
		dxfIOSta_80_60(hmrIO& _io,dxColor& _InClr,dxColor& _OutClr):rIO(_io){
			InClr=_InClr;
			OutClr=_OutClr;
		}
	};
	class dxfIOBufSta_80_60:public dxFn{
	private:
		hmrIO& rIO;
		dxRGB InClr;
		dxRGB OutClr;
	protected:
		virtual int fndraw(dxO& dxo){
			dxo.draw(Pint(0,0),dxoBox(Pint(80,60),dxRGB(255,255,255),0),true,dxDMode(128));
			if(dxo.draw(Pint(0,0),dxoBut(Pint(35,30),"IBuf",InClr))<0)rIO.BufGate.clearIBuf();
			dxo.draw(Pint(35,0),dxoStrP(Pint(50,30),hmstd::fStr("%4u",rIO.BufGate.sizeIBuf()),InClr));
			if(dxo.draw(Pint(0,30),dxoBut(Pint(35,30),"OBuf",OutClr))<0)rIO.BufGate.clearOBuf();
			dxo.draw(Pint(35,30),dxoStrP(Pint(50,30),hmstd::fStr("%4u",rIO.BufGate.sizeOBuf()),OutClr));
			return 0;
		}
	public:
		dxfIOBufSta_80_60(hmrIO& _io,dxColor& _InClr,dxColor& _OutClr):rIO(_io){
			InClr=_InClr;
			OutClr=_OutClr;
		}
	};
	class dxfRSIOCtr:public dxFn{
	private:
		hmrIO& rIO;
		hmrFile& rFile;
		bool& rFileNewOnlyFlag;
		int PortCh;
		int BaudRate;
//		bool PortChF[12];
		dxRGB Clr,SubClr;
	public:
		bool PortFileLinkFlag;
	protected:
		virtual int fndraw(dxO& dxf);
	public:
		dxfRSIOCtr(const dxRGB& _Clr,hmrIO& _IO,hmrFile& _File,bool& _FileNewOnlyFlag):rIO(_IO),rFile(_File),rFileNewOnlyFlag(_FileNewOnlyFlag){
			PortCh=0;
			BaudRate=9600;
//			for(int i=0;i<12;++i)PortChF[i]=false;
			PortFileLinkFlag=true;
			Clr=_Clr;
			dxHLS tmp(Clr);
			tmp.sat/=2;
			SubClr=tmp;
		}
	};
	class dxfMPIOCtr:public dxFn{
	private:
		hmrIO& rIO;
		hmrFile& rFile;
		bool& rFileNewOnlyFlag;
		int PortCh;
		int BaudRate;
//		bool PortChF[12];
		dxRGB Clr,SubClr;
	public:
		bool PortFileLinkFlag;
	protected:
		virtual int fndraw(dxO& dxf);
	public:
		dxfMPIOCtr(const dxRGB& _Clr,hmrIO& _IO,hmrFile& _File,bool& _FileNewOnlyFlag):rIO(_IO),rFile(_File),rFileNewOnlyFlag(_FileNewOnlyFlag){
			PortCh=0;
			BaudRate=9600;
//			for(int i=0;i<12;++i)PortChF[i]=false;
			PortFileLinkFlag=true;
			Clr=_Clr;
			dxHLS tmp(Clr);
			tmp.sat/=2;
			SubClr=tmp;
		}
	};
	class dxfMPIOCtr2:public dxFn{
	private:
		hmrIO& rIO;
		hmrFile& rFile;
		bool& rFileNewOnlyFlag;
		int PortCh;
		int BaudRate;
//		bool PortChF[12];
		dxRGB Clr,SubClr;
		bool AutoConnectMode;
		int Interval;
		int Length;
		int Adjustment;
		hmTime NextTime;
	public:
		bool PortFileLinkFlag;
	protected:
		virtual int fndraw(dxO& dxf);
	public:
		dxfMPIOCtr2(const dxRGB& _Clr,hmrIO& _IO,hmrFile& _File,bool& _FileNewOnlyFlag):rIO(_IO),rFile(_File),rFileNewOnlyFlag(_FileNewOnlyFlag){
			PortCh=0;
			BaudRate=9600;
//			for(int i=0;i<12;++i)PortChF[i]=false;
			PortFileLinkFlag=true;
			Clr=_Clr;
			dxHLS tmp(Clr);
			tmp.sat/=2;
			SubClr=tmp;
		}
		void work(void);
	};
	//size(80,50)
	class dxfIOTimeOut_80_50:public dxFn{
	private:
		dxRGB Clr;
	protected:
		virtual int fndraw(dxO& dxf);
	public:
		dxfIOTimeOut_80_50(dxColor& _Clr){
			Clr=_Clr;
		}
	};
	class dxfIOTimeOut_150_45:public dxFn{
	private:
		dxRGB Clr;
	protected:
		virtual int fndraw(dxO& dxf);
	public:
		dxfIOTimeOut_150_45(const dxColor& _Clr){
			Clr=_Clr;
		}
	};
	class dxoIsIOTimeOut:public dxReObject,public hmoBox{
	private:
		dxRGB Clr;
		int Size;
	public:
		dxoIsIOTimeOut(int Size_,const dxColor& Clr_)
			:Size(Size_)
			,Clr(Clr_)
			,hmoBox(Pint(Size_,Size_)){
		}
	protected:
		virtual int redraw(dxO& dxo){
			if(dxo.draw(Pint(0,0),dxoCheck(size,"",Size,Clr,hmrIO::iologType::ioTimeOutFlag))==1){
				hmrIO::iologType::ioTimeOutFlag=!hmrIO::iologType::ioTimeOutFlag;
			}
			return 0;
		}
	};
	class dxoIOTimeOut:public dxReObject,public hmoBox{
	private:
		dxRGB Clr;
	public:
		dxoIOTimeOut(Pint Size_,const dxColor& Clr_)
			:hmoBox(Size_)
			,Clr(Clr_){
		}
	protected:
		virtual int redraw(dxO& dxo){
			int val=static_cast<int>(hmrIO::iologType::ioTimeOut.time()*1000+hmrIO::iologType::ioTimeOut.msec());
			int ans=dxo.draw(Pint(0,0),dxoButLRIO(size,hmstd::fStr("%d",val),Clr,hmrIO::iologType::ioTimeOutFlag));
			if(ans==1){
				val*=2;
				if(val>4000)val=250;
				hmrIO::iologType::ioTimeOut.set(val/1000,val%1000);
			}
			else if(ans==10){
				val/=2;
				if(val<250)val=4000;
				hmrIO::iologType::ioTimeOut.set(val/1000,val%1000);
			}
			return 0;
		}
	};
	class dxoIOSendBox:public dxReObject,public hmoBox{
	private:
		hmrIO& rIO;
		dxRGB Clr;
	private:
		dxInput Inp;
	public:
		dxfIOLog::cInputSetter getInpSetter(){return dxfIOLog::cInputSetter(Inp);}
	public:
		dxoIOSendBox(hmrIO& rIO_,Pint Size_,const dxColor& Clr_)
			:hmoBox(Size_)
			,rIO(rIO_)
			,Clr(Clr_){
			Inp.make(1024);
		}
	protected:
		virtual int redraw(dxO& dxo){
			if(dxo.draw(Pint(0,0),dxoInpBox(size,Inp,Clr))==2){
				rIO.pout<<hm::hmAscToBin(Inp.getStr());
				Inp.setStr("");
			}
			return 0;
		}
	};
}
#
#endif
