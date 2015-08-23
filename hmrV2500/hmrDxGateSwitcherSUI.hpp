#ifndef HMR_DXGATE_SUI_INC
#define HMR_DXGATE_SUI_INC 100

#include <iostream>
#include <string>
#include "hmLibVer.hpp"
#include <hmLib_v2/hmBasic.hpp>
#include <hmLib_v2/dxObject.hpp>
#include <hmLib_v2/dxArea.hpp>
#include <hmLib_v3_05/signals.hpp>
#include <boost/signals2.hpp>
#include "hmrDxBUI.hpp"
//#include "hmrBaseDX.hpp"

//#define DEBUG_MODE 1

/* ---- hmrDxGateSwitcherSUI-----

hmrDxGateSwitcherSUI.hpp :v1_00/130615 amby
	Gate SwitcherSUIの製作開始


使われ方としてどういう可能性があるか

	１、いじられるのは1つの場所ではないかもしれない
		すべての変数を同期しておきたい
		できるなら、描画クラスを再利用できる形にしたい

	２、ポートが増えるかもしれない
		共通化できるところは共通化してしまいたい

	３、UIを通さずにいじられるかもしれない
		すべての変数を同期しておく必要がある
		ボーレート、ポートナンバー、その他設定変数

	４、場合によって、ゲートをリアルタイムに切り替えたいことがある
		ポート設定は独立にできたほうがよいと思われる

設計
	１、

　Gate Switcher 

hmrDxGateSwitcher.hpp :v1_01/130414 amby
	signal, slot 系の名前を統一
	connection の間違いを修正
	描画の場所を変更

hmrDxGateSwitcher.hpp :v1_00/130406 amby
	Gate switcher 関連のUIを提供する
	Port open/close について実装
	file open/close については保留してある
*/

namespace hmr{
	// 描画クラス static
	class dxosGateSwitcherSUI:public dxosBUI{
	private:
		int RS_PortCh;
		int FOMA_PortCh;

		int RS_BaudRate;
		int FOMA_BaudRate;
		
		dxRGB Clr,SubClr;

		bool IsRSOpen;
		bool IsFOMAOpen;
		
		bool PortFileLinkFlag;

	// signal系の実装
	public:
		// Port シグナル
		boost::signals2::signal<void(int, int)> signal_require_openRS;
		boost::signals2::signal<void(void)> signal_require_closeRS;

		boost::signals2::signal<void(int, int)> signal_require_openFOMA;
		boost::signals2::signal<void(void)> signal_require_closeFOMA;

		// File　シグナル
		boost::signals2::signal<void(void)> signal_require_openFile;  
		boost::signals2::signal<void(void)> signal_require_closeFile; 

		// inquire系列の定義
		hmLib::inquiries::inquiry<bool> isOpenInq;
		hmLib::inquiries::inquiry<bool> canGetInq;
		hmLib::inquiries::inquiry<bool> canSendInq;

	// slot 系の実装
	private:
		// Connection
		hmLib::signals::unique_connections connections;
		
		// 実際に呼ばれる slot 用の関数
		void sync_isRSOpen(bool flag_){
			IsRSOpen = flag_;
		}
		void sync_isFOMAOpen(bool flag_){
			IsFOMAOpen = flag_;
		}
		

	public:
		// slot と　signal　をつなげる関数
			// IsRSOpen の状態をもらってくる
		void slot_sync_isRSOpen(boost::signals2::signal<void(bool)> &sig_){
			connections(sig_.connect([&](bool flag_)->void{this->sync_isRSOpen(flag_);}));
		}
			// IsFOMAOpen の状態をもらってくる
		void slot_sync_isFOMAOpen(boost::signals2::signal<void(bool)> &sig_){
			connections(sig_.connect([&](bool flag_)->void{this->sync_isFOMAOpen(flag_);}));
		}
	
	public:
		//virtual int fndraw(dxO& dxf);
		int normal_draw(dxO& dxo)override{
			int ans;

			// 自分自身を名乗って状態表示
			/*
			dxo.draw(Pint(5,5),dxoStrP(Pint(70,20),"GateSwitcher",getClr(normal,strobj)));
			if(IsRSOpen || IsFOMAOpen ){
				dxo.draw(Pint(80,5),dxoStrP(Pint(140,20),"Opened",getClr(normal,strobj)));
			}else{
				dxo.draw(Pint(80,5),dxoStrP(Pint(140,20),"Closed",getClr(invalid,strobj)));
			}
			*/

			// can get 系列の描画
			 // is Open
			try{
				if(isOpenInq()){
					dxo.draw(Pint(5,5),dxoStrP(Pint(70,20),"Opened",getClr(normal,strobj)));
				}else{
					dxo.draw(Pint(5,5),dxoStrP(Pint(70,20),"Closed",getClr(invalid,strobj)));
				}
			}catch(const hmLib::inquiries::unconnected_exception& ){				
				// Inquire にアクセスできなかった！！　でも、そんなのメイン関数には関係ないからもみ消す
				dxo.draw(Pint(5, 5), dxoStr("Inquiry::isOpen 取得失敗") );
			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(5, 5), dxoStr( Excp.what()) );
			}
			
			 // can send
			try{
				if(canSendInq()){
					dxo.draw(Pint(80,5),dxoStrP(Pint(70,20),"送信可",getClr(normal,strobj)));
				}else{
					dxo.draw(Pint(80,5),dxoStrP(Pint(70,20),"送信不可",getClr(invalid,strobj)));
				}
			}catch(const hmLib::inquiries::unconnected_exception& ){				
				dxo.draw(Pint(80, 5), dxoStr("Inquiry::canSend 取得失敗") );
			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(80, 5), dxoStr( Excp.what()) );
			}
			
			 // can recv
			try{
				if(canGetInq()){
					dxo.draw(Pint(155,5),dxoStrP(Pint(70,20),"受信可",getClr(normal,strobj)));
				}else{
					dxo.draw(Pint(155,5),dxoStrP(Pint(70,20),"受信不可",getClr(invalid,strobj)));
				}
			}catch(const hmLib::inquiries::unconnected_exception& ){				
				dxo.draw(Pint(155, 5), dxoStr("Inquiry::canGet 取得失敗") );
			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(155, 5), dxoStr( Excp.what()) );
			}
			


			// RS系列についての制御系UI
			//　接続ボタン
			if(dxo.draw(Pint(5,30),dxoButIO(Pint(70, 20),"RS接続",getClr(normal,butobj), IsRSOpen), !IsFOMAOpen) == 1){
				if(!IsRSOpen){
					// Open signal 
					signal_require_openRS(RS_BaudRate, RS_PortCh);
					if(PortFileLinkFlag) signal_require_openFile();
				}else{
					// Close signal
					signal_require_closeRS();
					if(PortFileLinkFlag) signal_require_closeFile();
				}
			}

			//　チャンネル選択
			ans=dxo.draw(Pint(80,30),dxoButLR(Pint(70, 20),hmstd::fStr("%2dch",RS_PortCh),hm::sel((!IsRSOpen && !IsFOMAOpen),getClr(normal,butobj),getClr(invalid,butobj))), (!IsRSOpen && !IsFOMAOpen) );
			if( !IsRSOpen && !IsFOMAOpen ){
				if(ans== 1)RS_PortCh=(RS_PortCh+1)%13;
				if(ans==10)RS_PortCh=(RS_PortCh+12)%13;
			}

			//　ボーレート
			ans=dxo.draw(Pint(155,30),dxoButLR(Pint(70, 20),hmstd::fStr("%5dbps",RS_BaudRate),hm::sel((!IsRSOpen && !IsFOMAOpen),getClr(normal,butobj),getClr(invalid,butobj))), (!IsRSOpen && !IsFOMAOpen) );
			if( !IsRSOpen && !IsFOMAOpen ){
				if(ans==1)RS_BaudRate*=2;
				if(ans==10)RS_BaudRate/=2;		
				if(RS_BaudRate>38400)RS_BaudRate=2400;
				if(RS_BaudRate<2400)RS_BaudRate=38400;
			}

			// FOMA系についての制御系UI
			//　接続ボタン
			if(dxo.draw(Pint(5,55),dxoButIO(Pint(70, 20),"FOMA接続", getClr(normal,butobj), IsFOMAOpen),!IsRSOpen) == 1){
				if(!IsFOMAOpen){
					// Open signal 
					signal_require_openFOMA(FOMA_BaudRate, FOMA_PortCh);
					if(PortFileLinkFlag) signal_require_openFile();
				}else{
					// Close siganal
					signal_require_closeFOMA();
					if(PortFileLinkFlag) signal_require_closeFile();
				}
			}

			//チャンネル選択
			ans=dxo.draw(Pint(80,55),dxoButLR(Pint(70, 20),hmstd::fStr("%2dch",FOMA_PortCh),hm::sel( (!IsRSOpen && !IsFOMAOpen),getClr(normal,butobj),getClr(invalid,butobj))),  (!IsRSOpen && !IsFOMAOpen));
			if(  !IsRSOpen && !IsFOMAOpen ){
				if(ans== 1)FOMA_PortCh=(FOMA_PortCh+1)%13;
				if(ans==10)FOMA_PortCh=(FOMA_PortCh+12)%13;
			}

			//ボーレート
			ans=dxo.draw(Pint(155,55),dxoButLR(Pint(70, 20),hmstd::fStr("%5dbps",FOMA_BaudRate),hm::sel( (!IsRSOpen && !IsFOMAOpen) ,getClr(normal,butobj),getClr(invalid,butobj))), (!IsRSOpen && !IsFOMAOpen));
			if( (!IsRSOpen && !IsFOMAOpen) ){
				if(ans==1)FOMA_BaudRate*=2;
				if(ans==10)FOMA_BaudRate/=2;		
				if(FOMA_BaudRate>38400)FOMA_BaudRate=2400;
				if(FOMA_BaudRate<2400)FOMA_BaudRate=38400;
			}

			//保存のリンクフラグ
			/*
			if(dxo.draw(Pint(10,250),dxoCheck(Pint(200,20),"接続と保存をリンクする",20,Clr,PortFileLinkFlag))==1){
				if( !IsRSOpen && !IsFOMAOpen ){
					PortFileLinkFlag=!PortFileLinkFlag;
				}
			}
			*/
			return 0;

		}

		int extend_draw(dxO& dxo)override{
			normal_draw(dxo);
			return 0;
		}

		status getStatus()const override{
			if(IsRSOpen || IsFOMAOpen){
				return normal;
			}else{
				return invalid;
			}
		}

	public:
		dxosGateSwitcherSUI()
			:dxosBUI("GateSwitcher", 80, 80), IsRSOpen(false), IsFOMAOpen(false), PortFileLinkFlag(true){
			
			RS_PortCh=0;
			FOMA_PortCh = 0;

			RS_BaudRate=38400;
			FOMA_BaudRate = 38400;

			//Clr=_Clr;
			//dxHLS tmp(Clr);
			//tmp.sat/=2;
			//SubClr=tmp;
		}
	};		
}

#
#endif
