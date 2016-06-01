#ifndef HMR_CO2_INC
#define HMR_CO2_INC 101
#
/*======hmrCO2=======
hmrCO2 v1_01/130602 iwahori
	キーボード・パッド用のslot_setDataMode()を追加(引数がvoid(void)になっている)
hmrCO2 v1_00/130427 iwahori
	cCO2MsgAgentのsignal-slot,inquiry-contactへの対応完了
*/

#include "hmLibVer.hpp"
#include<boost/signals2.hpp>
#include<hmLib_v3_06/signals.hpp>
#include <hmLib_v3_06/inquiries.hpp>
#include "hmrItfMessage.hpp"
#include "hmrFlagirl.hpp"

//#include <hmLib_v3_06/hmTime.hpp>
//#include <hmLib_v3_06/dxArea.hpp>
//#include <cmath>
//#include "hmr.h"
//#include "hmrCom.h"
//#include "hmrBufCtr.hpp"
//#include "hmrComCtr.hpp"
//#include "hmrPad.hpp"
//#include "hmrUniFileType.hpp"
//#include "hmrUniDraw.h"
//#include "hmrDataBaseType.hpp"

namespace hmr{
	class cCO2MsgAgent:public itfMessageAgent{
	private:
		double Value;
		clock::time_point Time;

		double LogValue;
		clock::time_point LogTime;

		flagirl DataModeFlagirl;
		flagirl PumpPWFlagirl;
		flagirl SensorPWFlagirl;
		flagirl LogModeFlagirl;
		static const double D_ADMaxValue;
	public:
		static double toCO2(unsigned char LowByte,unsigned char HighByte);
		bool listen(datum::time_point Time_,bool Err_,const std::string& Str_)override;
		bool talk(std::string& Str)override;
		void setup_talk(void)override;
	private:
		hmLib::signals::unique_connections SignalConnections;
		hmLib::inquiries::unique_connections InquiryConnections;

		void setDataMode(bool Mode_);
		void setPumpPW(bool Mode_);
		void setSensorPW(bool Mode_);
		double getValue(void)const;
	public:
		boost::signals2::signal<void(double data, clock::time_point time)> signal_newData;
		boost::signals2::signal<void(double data, clock::time_point time)> signal_newLogData;

		void slot_setDataMode(boost::signals2::signal<void(bool)>& Signal_);
		void slot_setDataMode(boost::signals2::signal<void(void)>& Signal_);

		void slot_setSensorPW(boost::signals2::signal<void(bool)>& Signal_);
		void slot_setSensorPW(boost::signals2::signal<void(void)>& Signal_);

		void slot_setPumpPW(boost::signals2::signal<void(bool)>& Signal_);
		void slot_setPumpPW(boost::signals2::signal<void(void)>& Signal_);

		void slot_setLogMode(boost::signals2::signal<void(bool)>& Signal_);

		void contact_getValue(hmLib::inquiries::inquiry<double>& Inquiry_);
		void contact_getTime(hmLib::inquiries::inquiry<clock::time_point>& Inquiry_);
		void contact_getLogValue(hmLib::inquiries::inquiry<double>& Inquiry_);
		void contact_getLogTime(hmLib::inquiries::inquiry<clock::time_point>& Inquiry_);

		void contact_getPicDataMode(hmLib::inquiries::inquiry<bool>& Inquiry_);
		void contact_getPicPumpPW(hmLib::inquiries::inquiry<bool>& Inquiry_);
		void contact_getPicSensorPW(hmLib::inquiries::inquiry<bool>& Inquiry_);
		void contact_getPicLogMode(hmLib::inquiries::inquiry<bool>& Inquiry_);

		void contact_getRequestDataMode(hmLib::inquiries::inquiry<bool>& Inquiry_);
		void contact_getRequestPumpPW(hmLib::inquiries::inquiry<bool>& Inquiry_);
		void contact_getRequestSensorPW(hmLib::inquiries::inquiry<bool>& Inquiry_);
		void contact_getRequestLogMode(hmLib::inquiries::inquiry<bool>& Inquiry_);


	public:
		cCO2MsgAgent():
			Value(0.), LogValue(0.),
			Time(), LogTime(),
			DataModeFlagirl(),
			PumpPWFlagirl(),
			SensorPWFlagirl(),
			LogModeFlagirl()
		{
		}
	};
}

#
#endif