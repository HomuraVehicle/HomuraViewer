#ifndef INC_HMR_GATETYPE
#define INC_HMR_GATETYPE 100
#
#include <string>
#include "hmLibVer.hpp"
#include <hmLib_v3_06/hmRSgate.hpp>
#include <hmLib_v3_06/dxArea.hpp>
#include <hmLib_v3_06/gatestream.hpp>
#include "foma_gate.hpp"
#include "hmrFile.h"
#include "hmrIO.h"
namespace hmr{
	class cRSGateConnector:public hmrIO::cGateConnector{
	private:
		hmstd::RSgate& rRSGate;
	public:
		class fnBuilder:public hmrIO::cGateConnector::fnBuilder{
		public:
			hmstd::RSgate& rRSGate;
			int portnum;
			int portbps;
		public:
			fnBuilder(RSgate& rRSGate_,int portnum_,int portbps_)
				:rRSGate(rRSGate_)
				,portnum(portnum_)
				,portbps(portbps_){
			}
			virtual cGateConnector* operator()(){
				return new cRSGateConnector(rRSGate,portnum,portbps);
			}
		};
	public:
		cRSGateConnector(hmstd::RSgate& rRSGate_,int portnum_,int portbps_):rRSGate(rRSGate_){
			disconnect();
			if(rRSGate.open(portnum_,portbps_))return;
			connect(rRSGate);
		}
		~cRSGateConnector(){
			disconnect();
			rRSGate.close();
		}
	};
	class cFomaGateConnector:public hmrIO::cGateConnector{
		hmstd::RSgate& rRSGate;
		foma_gate& rFomaGate;
	public:
		class fnBuilder:public hmrIO::cGateConnector::fnBuilder{
		public:
			hmstd::RSgate& rRSGate;
			int portnum;
			int portbps;
			foma_gate& rFomaGate;
			std::string PhoneNum;
		public:
			fnBuilder(hmstd::RSgate& rRSGate_,int portnum_,int portbps_,foma_gate& rFomaGate_,const std::string& PhoneNum_)
				:rRSGate(rRSGate_)
				,portnum(portnum_)
				,portbps(portbps_)
				,rFomaGate(rFomaGate_)
				,PhoneNum(PhoneNum_){
			}
			virtual cGateConnector* operator()(){
				return new cFomaGateConnector(rRSGate,portnum,portbps,rFomaGate,PhoneNum);
			}
		};
	public:
		cFomaGateConnector(hmstd::RSgate& rRSGate_,int portnum_,int portbps_,foma_gate& rFomaGate_,const std::string& PhoneNum_)
			:rRSGate(rRSGate_)
			,rFomaGate(rFomaGate_){
			disconnect();
			if(rRSGate.open(portnum_,portbps_))return;
			if(rFomaGate.open(rRSGate,PhoneNum_))return;
			connect(rFomaGate);
		}
		~cFomaGateConnector(){
			disconnect();
			rFomaGate.close();
			rRSGate.close();
		}
	};
	class cGateCtr{
	private:
		hmrIO& rIO;
		hmstd::RSgate RSGate;
		bool IsConnectRSGate;
		foma_gate FomaGate;
		bool IsConnectFomaGate;
	public:
		cGateCtr(hmrIO& rIO_):rIO(rIO_),IsConnectRSGate(false),IsConnectFomaGate(false){}
		bool is_connect()const{return rIO.is_open();}
		void connectRS(int ch,int bps){
			if(rIO.is_open())return;
			if(rIO.open(hmr::cRSGateConnector::fnBuilder(RSGate,ch,bps)))IsConnectRSGate=false;
			else IsConnectRSGate=rIO.is_open();
		}
		void disconnectRS(){
			if(!IsConnectRSGate)return;
			rIO.close();
			IsConnectRSGate=rIO.is_open();
		}
		bool is_connectRS()const{return IsConnectRSGate;}
		void connectFoma(int ch,int bps,const std::string& PhoneNum){
			if(rIO.is_open())return;
			if(rIO.open(hmr::cFomaGateConnector::fnBuilder(RSGate,ch,bps,FomaGate,PhoneNum)))IsConnectFomaGate=false;
			else IsConnectFomaGate=rIO.is_open();
		}
		void disconnectFoma(){
			if(!IsConnectFomaGate)return;
			rIO.close();
			IsConnectFomaGate=rIO.is_open();
		}
		bool is_connectFoma()const{return IsConnectFomaGate;}
	public:
		class dxfGateCtr_360_150:public dxFn{
		private:
			cGateCtr& rGateCtr;
			hmrFile::fnOpenSignal FileOpenSignal;
			hmrFile::fnCloseSignal FileCloseSignal;
			dxRGB Clr,SubClr;
			int PortCh;
			int BaudRate;

			std::string PhoneNum;
			dxInput PhoneNumInput;
		public:
			bool PortFileLinkFlag;
		protected:
			int fndraw(dxO& dxo){
				int ans;
				//接続ボタン
				if(dxo.draw(Pint(0,0),dxoButIO(Pint(90,50),"接続-RS",Clr,rGateCtr.is_connectRS()))==1){
					if(!rGateCtr.is_connect()){
						rGateCtr.connectRS(PortCh,BaudRate);
						if(PortFileLinkFlag && rGateCtr.is_connectRS())FileOpenSignal();
					}else if(rGateCtr.is_connectRS()){
						rGateCtr.disconnectRS();
						if(PortFileLinkFlag && !rGateCtr.is_connect())FileCloseSignal();
					}
				}

				//接続ボタン
				if(dxo.draw(Pint(100,0),dxoButIO(Pint(90,50),"接続-Foma",Clr,rGateCtr.is_connectFoma()))==1){
					if(!rGateCtr.is_connect()){
						rGateCtr.connectFoma(PortCh,BaudRate,PhoneNum);
						if(PortFileLinkFlag && rGateCtr.is_connectFoma())FileOpenSignal();
					}else if(rGateCtr.is_connectFoma()){
						rGateCtr.disconnectFoma();
						if(PortFileLinkFlag && !rGateCtr.is_connect())FileCloseSignal();
					}
				}

				//保存のリンクフラグ
				if(dxo.draw(Pint(0,60),dxoCheck(Pint(200,20),"接続と保存をリンクする",20,Clr,PortFileLinkFlag))==1){
					PortFileLinkFlag=!PortFileLinkFlag;
				}



				dxo.draw(Pint(20,90),dxoStrP(Pint(30,20),"RS"));
				//RSGate状況
				std::string tmpStr;
				if(rGateCtr.RSGate.gfail())tmpStr="無効";
				else tmpStr="有効";
				dxRGB tmpClr;
				if(rGateCtr.is_connect())tmpClr=SubClr;
				else tmpClr=Clr;
				if(dxo.draw(Pint(60,90),dxoButIO(Pint(40,20),tmpStr,tmpClr,!rGateCtr.RSGate.gfail()))==1){
					if(!rGateCtr.is_connect()){
						if(rGateCtr.RSGate.gfail())rGateCtr.RSGate.open(PortCh,BaudRate);
						else rGateCtr.RSGate.close();
					}
				}

				//チャンネル選択
				ans=dxo.draw(Pint(110,90),dxoButLR(Pint(120,20),hmstd::fStr("%2dch",PortCh),tmpClr));
				if(!rGateCtr.is_connect() && rGateCtr.RSGate.gfail()){
					if(ans== 1)PortCh=(PortCh+1)%13;
					if(ans==10)PortCh=(PortCh+12)%13;
				}

				//ボーレート
				ans=dxo.draw(Pint(240,90),dxoButLR(Pint(120,20),hmstd::fStr("%5dbps",BaudRate),tmpClr));
				if(!rGateCtr.is_connect() && rGateCtr.RSGate.gfail()){
					if(ans==1)BaudRate*=2;
					if(ans==10)BaudRate/=2;		
					if(BaudRate>38400)BaudRate=2400;
					if(BaudRate<2400)BaudRate=38400;
				}


				dxo.draw(Pint(20,120),dxoStrP(Pint(30,20),"Foma"));
				//FomaGate状況
				if(rGateCtr.FomaGate.gfail())tmpStr="無効";
				else tmpStr="有効";
				if(rGateCtr.is_connect())tmpClr=SubClr;
				else tmpClr=Clr;
				if(dxo.draw(Pint(60,120),dxoButIO(Pint(40,20),tmpStr,tmpClr,!rGateCtr.FomaGate.gfail()))==1){
					if(!rGateCtr.is_connect()){
						if(rGateCtr.FomaGate.gfail())rGateCtr.FomaGate.open(rGateCtr.RSGate,PhoneNum);
						else rGateCtr.FomaGate.close();
					}
				}
				
				//電話番号選択
				if(dxo.draw(Pint(110,120),dxoInpBox(Pint(120,20),PhoneNumInput,tmpClr))==2){
					if(!rGateCtr.is_connect() && rGateCtr.RSGate.gfail()){
						PhoneNum=PhoneNumInput.getStr();
					}
					PhoneNumInput.setStr(PhoneNum);
				}

				return 0;
			}
		public:
			dxfGateCtr_360_150(const dxRGB& Clr_,cGateCtr& rGateCtr_,hmrFile::fnOpenSignal FileOpenSignal_,hmrFile::fnCloseSignal FileCloseSignal_)
				:rGateCtr(rGateCtr_)
				,FileOpenSignal(FileOpenSignal_)
				,FileCloseSignal(FileCloseSignal_)
				,PortCh(0)
				,BaudRate(9600)
				,PortFileLinkFlag(true)
				,PhoneNum("08024766917"){
				Clr=Clr_;
				dxHLS tmp(Clr_);
				tmp.sat/=2;
				SubClr=tmp;
				PhoneNumInput.make(1024,0,0,1,PhoneNum);
			}
		};
	};
}
#
#endif
