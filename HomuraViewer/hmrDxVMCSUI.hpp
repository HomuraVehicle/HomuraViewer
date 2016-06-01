#ifndef HMR_DXVMC_SUI_INC
#define HMR_DXVMC_SUI_INC 100

#include <iostream>
#include <string>
#include "hmLibVer.hpp"
#include <hmLib_v2/hmBasic.hpp>
#include <hmLib_v2/dxObject.hpp>
#include <hmLib_v2/dxArea.hpp>
#include <hmLib/signals.hpp>
#include <boost/signals2.hpp>
#include "hmrDxBUI.hpp"
//#include "hmrBaseDX.hpp"

//#define DEBUG_MODE 1

/* ---- hmrDxBufGateSUI-----

hmrDxVMCSUI.hpp :v1_00/130615 amby
	DxVMCSUIの製作開始
*/


namespace hmr{
	// 描画クラス static
	class dxosVMCSUI:public dxosBUI{
	private:
		unsigned char recvErr, sendErr;
		hmLib::signals::unique_connections connections;
		clock::time_point forceResetRecvTime;
		clock::time_point forceResetSendTime;

	public:
		// signal 系列の定義
		boost::signals2::signal<void(void)> signal_require_end_send;
		boost::signals2::signal<void(void)> signal_require_end_recv;

		boost::signals2::signal<void(void)> signal_require_clear_sendErr;
		boost::signals2::signal<void(void)> signal_require_clear_recvErr;

		// slot
		void set_forceResetRecvTime(clock::time_point time_){
			forceResetRecvTime = time_;
		}
		void set_forceResetSendTime(clock::time_point time_){
			forceResetSendTime = time_;
		}

		void slot_set_forceResetRecvTime(boost::signals2::signal<void(clock::time_point)> &sig_){
			connections(sig_.connect([&](clock::time_point time_)->void{this->set_forceResetRecvTime(time_);}));
		}
		void slot_set_forceResetSendTime(boost::signals2::signal<void(clock::time_point)> &sig_){
			connections(sig_.connect([&](clock::time_point time_)->void{this->set_forceResetSendTime(time_);}));
		}
		
		
		
		// inquire 系列の定義
		hmLib::inquiries::inquiry<unsigned char> sendErrInq;
		hmLib::inquiries::inquiry<unsigned char> recvErrInq;




	public:
		//virtual int fndraw(dxO& dxf);
		int normal_draw(dxO& dxo)override{
			//int ans;

			// 自分自身を名乗って状態表示
			//dxo.draw(Pint(5,5),dxoStrP(Pint(70,20),"VMC",getClr(normal,strobj)));
			
			// is Open 状態表示
			try{
				sendErr = sendErrInq();
				recvErr = recvErrInq();

				if( sendErr  == 0x00 && recvErr == 0x00 ){
					dxo.draw(Pint(5,5),dxoStrP(Pint(70,20),"Normal",getClr(normal,strobj)));
				}else{
					if(sendErr  != 0x00){
						dxo.draw(Pint(5,5),dxoStrP(Pint(70,20),"OutErr",getClr(error,strobj)));
					}
					if(recvErr  != 0x00){
						dxo.draw(Pint(80,5),dxoStrP(Pint(70,20),"InErr",getClr(error,strobj)));
					}
				}
			}catch(const hmLib::inquiries::unconnected_exception&){				
				dxo.draw(Pint(80, 5), dxoStr("Inquiry::Err 取得失敗") );
			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(80, 5), dxoStr( Excp.what()) );
			}

			return 0;
		}

		int extend_draw(dxO& dxo)override{
			// draw
			normal_draw(dxo);

			// Send ERR について
			dxRGB clr;
			dxo.draw(Pint(5, 30),dxoStrP(Pint(70,20),"SendErr", getClr(normal,strobj)));
			if(sendErr == 0x00){clr = getClr(normal,strobj);
			}else{clr = getClr(error,strobj);}
			dxo.draw(Pint(80, 30),dxoStrP(Pint(70,20),hmstd::fStr("%X", sendErr), clr));
			if( dxo.draw(Pint(155,30),dxoBut(Pint(70,20),"CLR", clr)) == 1){
				signal_require_clear_sendErr();
			}
			
			// Recv ERR について
			dxo.draw(Pint(5, 55),dxoStrP(Pint(70,20),"RecvErr", getClr(normal,strobj)));
			if(recvErr == 0x00){clr = getClr(normal,strobj);
			}else{clr = getClr(error,strobj);}
			dxo.draw(Pint(80, 55),dxoStrP(Pint(70,20),hmstd::fStr("%X", recvErr), clr));
			if( dxo.draw(Pint(155,55),dxoBut(Pint(70,20),"CLR", clr)) == 1){
				signal_require_clear_recvErr();
			}

			// force end recv, send　機能
			if( dxo.draw(Pint(5,80),dxoBut(Pint(105,20),"Force S End", getClr(normal, butobj))) == 1){
				signal_require_end_send();
			}
			if( dxo.draw(Pint(125,80),dxoBut(Pint(105,20),"Force R End", getClr(normal, butobj))) == 1){
				signal_require_end_recv();
			}

			// force end recv send time の表h字
			//dxo.draw(Pint(5,105), dxoStrP(Pint(105,20),"Last Force Send End", getClr(normal,strobj)));
			dxo.draw(Pint(5, 105), dxoStrP(Pint(105,20),hmr::time_to_hms(forceResetSendTime),getClr(normal,strobj)));
			
			//dxo.draw(Pint(5,130), dxoStrP(Pint(105,20),"Last Force Recv End", getClr(normal,strobj)));
			dxo.draw(Pint(125, 105), dxoStrP(Pint(105,20),hmr::time_to_hms(forceResetRecvTime),getClr(normal,strobj)));

			return 0;

		}

		status getStatus()const override{
			if( (sendErr | recvErr) == 0x00 ){
				return normal;
			}else{
				return error;
			}
		}
	public:
		// constructor
		dxosVMCSUI(void):dxosBUI("VMC", 30, 130), recvErr(0xFF), sendErr(0xFF){
			forceResetRecvTime = clock::now();
			forceResetSendTime = clock::now();
		}

	};		
}

#
#endif
