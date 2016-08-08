#ifndef HMR_VIEWER_VIEW_INC
#define HMR_VIEWER_VIEW_INC 100
#
#include<vector>
#include"predicate.hpp"
#include"Message.hpp"
#include"IO/DxIO_v2.hpp"
#include"View/DxBUIBoxSideDisp.hpp"
#include"View/DxDisplay.hpp"
#include"View/ControlMainDisplay.hpp"
#include"IO.hpp"
#include"Message.hpp"
namespace hmr{
	namespace viewer{
		struct cView{
			using dxosIOMainDisplay = io::dxosIO2<cIO::this_iologbuf::iterator>;
			using dxosMessageMainDisplay = message::dxosPacketList_withView<message::cComLog::iterator>;
		public:
			view::dxosDisplay Display;

			//IO用メイン画面
			dxosIOMainDisplay IOMainDisplay;
			//Message用メイン画面
			dxosMessageMainDisplay MessageMainDisp;
			//Control用メイン画面
			view::dxosControlMainDisplay ControlMainDisplay;

			//SUI用サイド画面
			view::dxosBUIBoxSideDisplay SUISideDisp;
			//MUI用サイド画面
			view::dxosBUIBoxSideDisplay MUISideDisp;
		private:
			void setupIOMainDsiplay(cIO& IO){
				//DxIO & cIO
				IO.IODriver.slot_send_direct(IOMainDisplay.signal_poutString);

				//DxIO & ioLogBuf
				IO.ioLogBuf.contactLogBuf(IOMainDisplay.inquiry_LogBuf);

				//DxIO & ioLogger
				IO.ioLogGate.Logger.contacInLog(IOMainDisplay.inquiry_InStr);
				IO.ioLogGate.Logger.contacOutLog(IOMainDisplay.inquiry_OutStr);
			}
			void setupMessageMainDsiplay(cMessage& Message){
				Message.Com.contactRecvLog(MessageMainDisp.inquiryNewInPacket);
				Message.Com.contactSendLog(MessageMainDisp.inquiryNewOutPacket);
				Message.ComLog.contactLogBuf(MessageMainDisp.inquiryPacketLog);

				const std::vector<message::datum::id_type> SwList{'a', 'b', 'c', 'C', 'g', 'G', 'j', 'm', 't'};
				MessageMainDisp.registSwIDList(SwList.begin(), SwList.end());
			}
			void setupControlMainDisplay(cFullADC& FullADC, cAccele& Accele, cCompass& Compass, cGyro& Gyro, cCamera& Camera, cGPS& GPS,cBattery& Battery){
				Accele.Logger.contact_getAcceleMeanPolar(ControlMainDisplay.Navigator.inquiry_accele);
				Compass.Compass.contact_getPolar(ControlMainDisplay.Navigator.inquiry_compass);
				Gyro.Logger.contact_getLogs(ControlMainDisplay.Navigator.range_inquiry_gyro);
				Gyro.Compass.inquiry_getAngle(ControlMainDisplay.Navigator.inquiry_gyrocompass);
				Gyro.Compass.slot_clearAngle(ControlMainDisplay.Navigator.signal_clearGyroCompassAngle);

				ControlMainDisplay.Camera.slot_setNewGraph(Camera.MsgAgent.signal_setPicture);

				GPS.GPSKashmir.slot_connect(ControlMainDisplay.Infomation.GPSKashmirBut.signal_connect);
				GPS.GPSKashmir.slot_disconnect(ControlMainDisplay.Infomation.GPSKashmirBut.signal_disconnect);
				GPS.GPSKashmir.contact_is_connect(ControlMainDisplay.Infomation.GPSKashmirBut.inquiry_is_connect);
				ControlMainDisplay.Infomation.Battery0.slot_newData(Battery.signal_newEachData[0]);
				ControlMainDisplay.Infomation.Battery1.slot_newData(Battery.signal_newEachData[1]);
				ControlMainDisplay.Infomation.Battery2.slot_newData(Battery.signal_newEachData[2]);
				
				ControlMainDisplay.GPSMap.slot_addNewData(GPS.MsgAgent.signal_newData);
				Compass.Compass.contact_getYawAngle(ControlMainDisplay.GPSMap.inquiry_getMagneticAngle);

				ControlMainDisplay.Infomation.slot_logData(FullADC.MsgAgent.signal_newData);
			}
		public:
			cView(cIO& IO, cMessage& Message, cFullADC& FullADC, cAccele& Accele, cCompass& Compass, cGyro& Gyro, cCamera& Camera, cGPS& GPS, cBattery& Battery)
				: Display(Pint(720, 720), Pint(240, 720))
				, IOMainDisplay(Pint(720, 720), CLR::DarkDullGreen, CLR::SoftGreen, CLR::LightSoftOrenge, CLR::LightSoftSkyblue)
				, MessageMainDisp(Pint(720, 720), 30, 360, CLR::DullOrenge){
				SUISideDisp.ClrSet.Background = CLR::DarkSoftBlue;
				MUISideDisp.ClrSet.Background = CLR::DarkSoftYellow;

				setupIOMainDsiplay(IO);
				setupMessageMainDsiplay(Message);
				setupControlMainDisplay(FullADC, Accele, Compass, Gyro, Camera, GPS, Battery);

				//Display構成
				Display.registMain(&IOMainDisplay);
				Display.registMain(&MessageMainDisp);
				Display.registMain(&ControlMainDisplay);
				Display.registSide(&SUISideDisp);
				Display.registSide(&MUISideDisp);
			}
			void registSUI(dxosBUI& SUI_){
				SUISideDisp.regist(&SUI_);
			}
			void registMUI(dxosBUI& MUI_){
				MUISideDisp.regist(&MUI_);
			}
			void operator()(void){
				dx::draw(Pint(0, 0), Display);
			}
		};
	}
}
#
#endif
