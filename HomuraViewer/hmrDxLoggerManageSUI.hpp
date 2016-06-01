#ifndef HMR_DXLOGGERMANAGESUI_INC
#define HMR_DXLOGGERMANAGESUI_INC 100

#include <iostream>
#include <string>
#include "hmLibVer.hpp"
#include <hmLib_v2/hmBasic.hpp>
#include <hmLib_v2/dxObject.hpp>
#include <hmLib_v2/dxArea.hpp>
#include <hmLib/signals.hpp>
#include <boost/signals2.hpp>
#include "hmrDxBUI.hpp"
#include <hmLib/type.h>

//#include "hmrBaseDX.hpp"

//#define DEBUG_MODE 1

/* ---- hmrDxLoggerManageSUI-----

===hmrDxLoggerManageSUI===

v1_00/130928 amby
	DxLoggerManageSUIの製作開始
	実装

*/

#define HMR_LOGGERSUI_BUFFERSIZE_SIZE 10

namespace hmr{
	// 描画クラス static
	class dxosLoggerMngSUI :public dxosBUI{
		static const hmLib_uint16 LogData_BufSize[HMR_LOGGERSUI_BUFFERSIZE_SIZE]; //LOGGER のデータ取得の際のバッファサイズ指定配列
	
	private:
		// logger にたまっているデータ数
		hmLib_uint32 logDataSize;
		// loggerから取り出すデータ数
		hmLib_uint16 logDataSetSize;
		int logDataSetSize_length;

		// Data getting
		bool is_getting_data;

	public:
		dxosBUIWaitableBoolBut isLogModeSUIBut;

	public:
		// signal 系列の定義
		boost::signals2::signal<void(bool)> signal_request_set_logMode;
		boost::signals2::signal<void(bool)> signal_request_get_logDataSize;
		boost::signals2::signal<void(bool, hmLib_uint16)> signal_request_logData;

		// inquire 系列の定義
		hmLib::inquiries::inquiry<bool> logModeInq;
		hmLib::inquiries::inquiry<hmLib_uint32> logDataSizeInq;
		hmLib::inquiries::inquiry<bool> is_getting_dataInq;
		hmLib::inquiries::inquiry<bool> is_sending_request_logDataInq;
		hmLib::inquiries::inquiry<bool> is_sending_request_logDataSizeInq;

	public:
		dxosLoggerMngSUI() :dxosBUI("logger", 60, 60),
			isLogModeSUIBut(this), is_getting_data(false), logDataSetSize_length(0){
		}

	public:
		int normal_draw(dxO& dxo)override{

			// log Mode
			try{
				isLogModeSUIBut.set(Pint(70, 20), "LOG");
				dxo.draw(Pint(5, 5), isLogModeSUIBut);
			}
			catch (const hmLib::inquiries::unconnected_exception&){
				dxo.draw(Pint(5, 5), dxoStrP(Pint(70, 20), "LOG", getClr(error, strobj)));
			}

			// log Buf
			try{
				logDataSize = logDataSizeInq();
				bool isSending = is_sending_request_logDataSizeInq();
				dxRGB Clr;

				if (isSending){
					Clr = getClr(active, butobj);
				}
				else{ Clr = getClr(normal, butobj); }

				if (dxo.draw(Pint(80, 5), dxoButIO(Pint(150, 20), hmstd::fStr("log %d", logDataSize), Clr, isSending)) == 1){
					if (!is_getting_data){
						signal_request_get_logDataSize(!isSending);
					}
				}
			}
			catch (const hmLib::inquiries::unconnected_exception&){
				dxo.draw(Pint(80, 5), dxoStr("Inquiry::logDataSize 取得失敗"));
			}
			catch (const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(80, 5), dxoStr(Excp.what()));
			}

			// log Data 取得UI
			try{
				is_getting_data = is_getting_dataInq();
				bool isSending = is_sending_request_logDataInq();

				dxRGB Clr;
				if (isSending){
					Clr = getClr(active, butobj);
				}
				else{ Clr = getClr(normal, butobj); }

				if (is_getting_data){
					dxo.draw(Pint(5, 30), dxoButIO(Pint(70, 20), "取得中", Clr, isSending));
				}
				else{
					if (dxo.draw(Pint(5, 30), dxoButIO(Pint(70, 20), "取得", Clr, isSending)) == 1){
						if (!is_getting_data){
							signal_request_logData(!isSending, logDataSetSize);
						}
					}
				}
			}
			catch (const hmLib::inquiries::unconnected_exception&){
				dxo.draw(Pint(5, 30), dxoStr("Inquiry::isGettingData 取得失敗"));
			}
			catch (const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(5, 30), dxoStr(Excp.what()));
			}

			// 取得サイズ 変更について
			int ans;
			logDataSetSize = LogData_BufSize[logDataSetSize_length];
			ans = dxo.draw(Pint(80, 30), dxoButLRIO(Pint(150, 20), hmstd::fStr("bufSize %d", logDataSetSize), getClr(normal, butobj), true));
			if (ans == 1){
				++logDataSetSize_length;
				if (logDataSetSize_length == HMR_LOGGERSUI_BUFFERSIZE_SIZE) logDataSetSize_length = 0;
			}
			else if (ans == 10){
				--logDataSetSize_length;
				if (logDataSetSize_length < 0) logDataSetSize_length = HMR_LOGGERSUI_BUFFERSIZE_SIZE - 1;
			}

			return 0;

		}

		int extend_draw(dxO& dxo)override{
			// draw
			normal_draw(dxo);
			return 0;

		}
		status getStatus()const override{return normal;}	
	};		
}

#
#endif
