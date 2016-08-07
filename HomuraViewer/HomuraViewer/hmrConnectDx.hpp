#ifdef HMR_MAIN_INC_END
#ifndef HMR_CONNECTDX_INC
#define HMR_CONNECTDX_INC 100
#

//#include "hmrDxGateSwitcher.hpp"
//#include "hmrGateSwitcher.hpp"

/*===hmrConnect===
接続補助関数群を定義する
*/
//#include"hmrPublicSignal.hpp"
namespace hmr{
	namespace viewer{
	#if defined(HMR_DXCOM_INC) && defined(HMR_COM_INC) && defined(HMR_COMLOG_INC)
		void connect(dxosPacketList_withView<cComLog::iterator>& DxCom_, cCom& Com_, cComLog& ComLog_){
			Com_.contactRecvLog(DxCom_.inquiryNewInPacket);
			Com_.contactSendLog(DxCom_.inquiryNewOutPacket);
			ComLog_.contactLogBuf(DxCom_.inquiryPacketLog);
		}
	#endif

		// DXIO に関するコネクトすべてをつかさどる関数
		 // DxIOmain関連
	#if defined(HMR_DXIODISPLAY_INC) && defined(HMRIO_INC) && defined(IOLOGGATE_INC)
		typedef std::pair<bool, system_clock_iologtype> io_iologtype;
		typedef std::vector<io_iologtype> container;
		typedef container::const_iterator contanerItr;
		void connect(dxosIOMainPage<contanerItr>& IOmain_, cFHdxIO& cIO_, fdx_vector_iologbuf<system_clock_iologtype>& ioLog_){
			//自身の内部の接続
			IOmain_.DxIO.slot_setInPaintMode(IOmain_.DxSelectIOPaint.signal_require_paintInput);
			IOmain_.DxIO.slot_setOutPaintMode(IOmain_.DxSelectIOPaint.signal_require_paintOutput);
			IOmain_.DxIO.contact_InPaintFlag(IOmain_.DxSelectIOPaint.InPaintFlagInq);
			IOmain_.DxIO.contact_OutPaintFlag(IOmain_.DxSelectIOPaint.OutPaintFlagInq);

			//対IO
			cIO_.slot_send_direct(IOmain_.DxSendBox.signal_poutString);

			//対ioLog
			ioLog_.contactLogBuf(IOmain_.DxIO.IoLogBufInq);
		}
	#endif
		// DxIOsub関連
	#if defined(HMR_DXIODISPLAY_INC) && defined(HMR_GATEHAB_INC) && defined(HMR_OPERATOR_INC) && defined(HMR_BUFGATE_INC)
		void connect(dxosIOSubPage& IOsub_, cGateSwitcher& cGateSwitcher_, cFHDxOperator& operator_, bufgate& bufgate_){
			//対GateSwitcher
			cGateSwitcher_.slot_set_and_openRS(IOsub_.DxosGateSwitcher.signal_require_openRS);
			cGateSwitcher_.slot_set_and_openFOMA(IOsub_.DxosGateSwitcher.signal_require_openFOMA);
			cGateSwitcher_.slot_closePort(IOsub_.DxosGateSwitcher.signal_require_closeRS);
			cGateSwitcher_.slot_closePort(IOsub_.DxosGateSwitcher.signal_require_closeFOMA);
			dxosGateSwitcher_.slot_sync_isFOMAOpen(IOsub_.DxosGateSwitcher.signal_inform_isFOMAOpen);
			dxosGateSwitcher_.slot_sync_isRSOpen(IOsub_.DxosGateSwitcher.signal_inform_isRSOpen);

			//対Operator
			operator_.slot_setIsSendActiveFlag(IOsub_.DxSendStartButt.signal_require_setStartFlag);
			operator_.contact_IsSendActive(IOsub_.DxSendStartButt.StartFlagInq);
			operator_.slot_setIsFullDuplex(IOsub_.DxSelectDuplex.signal_require_isFullDuplex);
			operator_.contact_IsFullDuplex(IOsub_.DxSelectDuplex.IsFullDuplexFlagInq);
			operator_.slot_setTimeOut(IOsub_.DxSendTimeOutCtr.signal_require_setTimeOut);
			operator_.contact_TimeOut(IOsub_.DxSendTimeOutCtr.TimeOutInq);
			operator_.slot_setSendInterval(IOsub_.DxSendIntervalCtr.signal_require_setTimeInterval);
			operator_.contact_Interval(IOsub_.DxSendIntervalCtr.TimeIntervalInq);
			operator_.slot_setSendInterval(IOsub_.DxSyncCheckBox.signal_require_setTimeInterval);
			operator_.contact_IsFullDuplex(IOsub_.DxSyncCheckBox.IsFullDuplexInq);
			operator_.contact_Interval(IOsub_.DxSyncCheckBox.TimeIntervalInq);

			//対BufGate
			bufgate_.slot_clearSendBuf(IOsub_.DxSendBufCtr.signal_require_clearBuf);
			bufgate_.contact_sendBufSize(IOsub_.DxSendBufCtr.BufSizeInq);
			bufgate_.slot_clearRecvBuf(IOsub_.DxSendBufCtr.signal_require_clearBuf);
			bufgate_.contact_recvBufSize(IOsub_.DxSendBufCtr.BufSizeInq);
		}

		void connect(dxosIOSubPage& IOsub_, cGateSwitcher& cGateSwitcher_, cFullRecvTimeSendOperator& operator_, bufgate& bufgate_){
			//対GateSwitcher
			cGateSwitcher_.slot_set_and_openRS(IOsub_.DxosGateSwitcher.signal_require_openRS);
			cGateSwitcher_.slot_set_and_openFOMA(IOsub_.DxosGateSwitcher.signal_require_openFOMA);
			cGateSwitcher_.slot_closePort(IOsub_.DxosGateSwitcher.signal_require_closeRS);
			cGateSwitcher_.slot_closePort(IOsub_.DxosGateSwitcher.signal_require_closeFOMA);
			dxosGateSwitcher_.slot_sync_isFOMAOpen(IOsub_.DxosGateSwitcher.signal_inform_isFOMAOpen);
			dxosGateSwitcher_.slot_sync_isRSOpen(IOsub_.DxosGateSwitcher.signal_inform_isRSOpen);

			//対Operator
			operator_.slot_setIsSendActiveFlag(IOsub_.DxSendStartButt.signal_require_setStartFlag);
			operator_.contact_IsSendActive(IOsub_.DxSendStartButt.StartFlagInq);
			operator_.slot_setIsFullDuplex(IOsub_.DxSelectDuplex.signal_require_isFullDuplex);
			operator_.contact_IsFullDuplex(IOsub_.DxSelectDuplex.IsFullDuplexFlagInq);
			operator_.slot_setTimeOut(IOsub_.DxSendTimeOutCtr.signal_require_setTimeOut);
			operator_.contact_TimeOut(IOsub_.DxSendTimeOutCtr.TimeOutInq);
			operator_.slot_setSendInterval(IOsub_.DxSendIntervalCtr.signal_require_setTimeInterval);
			operator_.contact_Interval(IOsub_.DxSendIntervalCtr.TimeIntervalInq);
			operator_.slot_setSendInterval(IOsub_.DxSyncCheckBox.signal_require_setTimeInterval);
			operator_.contact_IsFullDuplex(IOsub_.DxSyncCheckBox.IsFullDuplexInq);
			operator_.contact_Interval(IOsub_.DxSyncCheckBox.TimeIntervalInq);

			//対BufGate
			bufgate_.slot_clearSendBuf(IOsub_.DxSendBufCtr.signal_require_clearBuf);
			bufgate_.contact_sendBufSize(IOsub_.DxSendBufCtr.BufSizeInq);
			bufgate_.slot_clearRecvBuf(IOsub_.DxSendBufCtr.signal_require_clearBuf);
			bufgate_.contact_recvBufSize(IOsub_.DxSendBufCtr.BufSizeInq);
		}
	#endif

	#if defined(HMR_DXIO2_INC) && defined(HMRIO_INC) && defined(IOLOGGATE_INC)
		void connect(dxosIO2<fdx_vector_iologbuf<system_clock_iologtype>::iterator>& IOmain_, cFHdxIO& IO_, fdx_vector_iologbuf<system_clock_iologtype>& ioLogBuf_, fdx_crlf_timeout_iologger<system_clock_iologtype>& ioLogger_){
			//DxIO & cIO
			IO_.slot_send_direct(IOmain_.signal_poutString);

			//DxIO & ioLogBuf
			ioLogBuf_.contactLogBuf(IOmain_.inquiry_LogBuf);

			//DxIO & ioLogger
			ioLogger_.contacInLog(IOmain_.inquiry_InStr);
			ioLogger_.contacOutLog(IOmain_.inquiry_OutStr);
		}
	#endif

	#if defined(HMR_VIEWER_CAMERA_DXCAMERA_INC) && defined(HMR_VIEWER_CAMERA_CAMERAMSGAGENT_INC)
		void connect(camera::dxosCamera& Camera_, camera::cMsgAgent& Agent_){
			Camera_.slot_setNewGraph(Agent_.signal_setPicture);
		}
	#endif

	#if defined(HMR_DXNAVIGATOR_INC) && defined(HMR_VIEWER_ACCELE_INC) && defined(HMR_VIEWER_COMPASS_MAGNETICCOMPASS_INC) && defined(HMR_VIEWER_GYRO_GYROLOGGER_INC)
		void connect(dxosNavigator<gyro::cGyroLogger::iterator>& Navigator_, accele::cMsgAgent& Accele_, compass::cMagneticCompass& Compass_, gyro::cGyroLogger& GyroLogger_){
			Accele_.contact_getAccelePolar(Navigator_.inquiry_accele);
			Compass_.contact_getPolar(Navigator_.inquiry_compass);
			GyroLogger_.contact_getLogs(Navigator_.range_inquiry_gyro);
		}
		void connect(dxosNavigator<gyro::cGyroLogger::iterator>& Navigator_, accele::cLogger& AcceleLogger_, compass::cMagneticCompass& Compass_, gyro::cGyroLogger& GyroLogger_){
			AcceleLogger_.contact_getAcceleMeanPolar(Navigator_.inquiry_accele);
			Compass_.contact_getPolar(Navigator_.inquiry_compass);
			GyroLogger_.contact_getLogs(Navigator_.range_inquiry_gyro);
		}
	#endif

	#if defined(HMR_DXNAVIGATOR_INC) && defined(HMR_VIEWER_ACCELE_INC) && defined(HMR_VIEWER_COMPASS_MAGNETICCOMPASS_INC) && defined(HMR_VIEWER_GYRO_GYROLOGGER_INC) && defined(HMR_VIEWER_GYRO_GYROCOMPASS_INC)
		void connect(dxosNavigator<gyro::cGyroLogger::iterator>& Navigator_, accele::cLogger& AcceleLogger_, compass::cMagneticCompass& Compass_, gyro::cGyroLogger& GyroLogger_, gyro::cGyroCompass& GyroCompass_){
			AcceleLogger_.contact_getAcceleMeanPolar(Navigator_.inquiry_accele);
			Compass_.contact_getPolar(Navigator_.inquiry_compass);
			GyroLogger_.contact_getLogs(Navigator_.range_inquiry_gyro);
			GyroCompass_.inquiry_getAngle(Navigator_.inquiry_gyrocompass);
			GyroCompass_.slot_clearAngle(Navigator_.signal_clearGyroCompassAngle);
		}
	#endif

	#if defined(HMR_DXINFORMATIRON_INC) && defined(HMR_VIEWER_GPS_GPSKASHMIR_INC) && defined(HMR_BATTERY_INC)
		void connect(dxosInformation& Information, gps::cGPSKashmir& GPSKashmir, cBattery& Battery){
			GPSKashmir.slot_connect(Information.GPSKashmirBut.signal_connect);
			GPSKashmir.slot_disconnect(Information.GPSKashmirBut.signal_disconnect);
			GPSKashmir.contact_is_connect(Information.GPSKashmirBut.inquiry_is_connect);
			Information.Battery0.slot_newData(Battery.signal_newEachData[0]);
			Information.Battery1.slot_newData(Battery.signal_newEachData[1]);
			Information.Battery2.slot_newData(Battery.signal_newEachData[2]);
		}
	#endif

	#if defined(HMR_DXGPSMAP_INC) && defined(HMR_VIEWER_GPS_INC) && defined(HMR_VIEWER_COMPASS_MAGNETICCOMPASS_INC)
		void connect(gps::dxosGPSMap& GPSMap, gps::cMsgAgent& Agent, compass::cMagneticCompass& Compass_){
			GPSMap.slot_addNewData(Agent.signal_newData);
			Compass_.contact_getYawAngle(GPSMap.inquiry_getMagneticAngle);
		}
	#endif

	}
}
#
#endif
#endif
