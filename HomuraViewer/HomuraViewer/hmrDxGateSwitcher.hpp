#ifndef HMR_DXGATE_INC
#define HMR_DXGATE_INC 101

#include <iostream>
#include <string>
#include "hmLibVer.hpp"
#include <hmLib_v2/hmBasic.hpp>
#include <hmLib_v2/dxObject.hpp>
#include <hmLib_v2/dxArea.hpp>
#include <hmLib/signals.hpp>
#include <boost/signals2.hpp>

namespace hmr{
	namespace viewer{
		// 描画クラス static
		class dxosGateSwitcher :public dxReObject, public hmoBox{
		private:
			int RS_PortCh;
			int FOMA_PortCh;

			int RS_BaudRate;
			int FOMA_BaudRate;

			dxRGB Clr, SubClr;

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
				connections(sig_.connect([&](bool flag_)->void{this->sync_isRSOpen(flag_); }));
			}
			// IsFOMAOpen の状態をもらってくる
			void slot_sync_isFOMAOpen(boost::signals2::signal<void(bool)> &sig_){
				connections(sig_.connect([&](bool flag_)->void{this->sync_isFOMAOpen(flag_); }));
			}

		protected:
			//virtual int fndraw(dxO& dxf);
			virtual int redraw(dxO& dxo){
				int ans;

				// RS系列についての制御系UI
				//　接続ボタン
				if(dxo.draw(Pint(10, 10), dxoButIO(Pint(220, 40), "RS接続", Clr, IsRSOpen), !IsFOMAOpen) == 1){

					if(!IsRSOpen){
						// Open signal 
						signal_require_openRS(RS_BaudRate, RS_PortCh);
						if(PortFileLinkFlag) signal_require_openFile();


					} else{
						// Close signal
						signal_require_closeRS();
						if(PortFileLinkFlag) signal_require_closeFile();
					}
				}

				//　チャンネル選択
				ans = dxo.draw(Pint(30, 60), dxoButLR(Pint(200, 20), hmstd::fStr("%2dch", RS_PortCh), hm::sel((!IsRSOpen && !IsFOMAOpen), Clr, SubClr)), (!IsRSOpen && !IsFOMAOpen));
				if(!IsRSOpen && !IsFOMAOpen){
					if(ans == 1)RS_PortCh = (RS_PortCh + 1) % 13;
					if(ans == 10)RS_PortCh = (RS_PortCh + 12) % 13;
				}

				//　ボーレート
				ans = dxo.draw(Pint(30, 90), dxoButLR(Pint(200, 20), hmstd::fStr("%5dbps", RS_BaudRate), hm::sel((!IsRSOpen && !IsFOMAOpen), Clr, SubClr)), (!IsRSOpen && !IsFOMAOpen));
				if(!IsRSOpen && !IsFOMAOpen){
					if(ans == 1)RS_BaudRate *= 2;
					if(ans == 10)RS_BaudRate /= 2;
					if(RS_BaudRate > 38400)RS_BaudRate = 2400;
					if(RS_BaudRate < 2400)RS_BaudRate = 38400;
				}

				// FOMA系についての制御系UI
				//　接続ボタン
				if(dxo.draw(Pint(10, 130), dxoButIO(Pint(220, 40), "FOMA接続", Clr, IsFOMAOpen), !IsRSOpen) == 1){
					if(!IsFOMAOpen){
						// Open signal 
						signal_require_openFOMA(FOMA_BaudRate, FOMA_PortCh);
						if(PortFileLinkFlag) signal_require_openFile();
					} else{
						// Close siganal
						signal_require_closeFOMA();
						if(PortFileLinkFlag) signal_require_closeFile();
					}
				}

				//チャンネル選択
				ans = dxo.draw(Pint(30, 180), dxoButLR(Pint(200, 20), hmstd::fStr("%2dch", FOMA_PortCh), hm::sel((!IsRSOpen && !IsFOMAOpen), Clr, SubClr)), (!IsRSOpen && !IsFOMAOpen));
				if(!IsRSOpen && !IsFOMAOpen){
					if(ans == 1)FOMA_PortCh = (FOMA_PortCh + 1) % 13;
					if(ans == 10)FOMA_PortCh = (FOMA_PortCh + 12) % 13;
				}

				//ボーレート
				ans = dxo.draw(Pint(30, 210), dxoButLR(Pint(200, 20), hmstd::fStr("%5dbps", FOMA_BaudRate), hm::sel((!IsRSOpen && !IsFOMAOpen), Clr, SubClr)), (!IsRSOpen && !IsFOMAOpen));
				if((!IsRSOpen && !IsFOMAOpen)){
					if(ans == 1)FOMA_BaudRate *= 2;
					if(ans == 10)FOMA_BaudRate /= 2;
					if(FOMA_BaudRate > 38400)FOMA_BaudRate = 2400;
					if(FOMA_BaudRate < 2400)FOMA_BaudRate = 38400;
				}

				//保存のリンクフラグ
				if(dxo.draw(Pint(10, 250), dxoCheck(Pint(200, 20), "接続と保存をリンクする", 20, Clr, PortFileLinkFlag)) == 1){
					if(!IsRSOpen && !IsFOMAOpen){
						PortFileLinkFlag = !PortFileLinkFlag;
					}
				}
				return 0;

			}

		public:
			dxosGateSwitcher(const dxRGB& _Clr)
				:IsRSOpen(false), IsFOMAOpen(false), PortFileLinkFlag(true), hmoBox(Pint(240, 290)){

				RS_PortCh = 0;
				FOMA_PortCh = 0;

				RS_BaudRate = 38400;
				FOMA_BaudRate = 38400;

				Clr = _Clr;
				dxHLS tmp(Clr);
				tmp.sat /= 2;
				SubClr = tmp;
			}
		};
	}
}
#
#endif
