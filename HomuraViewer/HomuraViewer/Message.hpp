#ifndef HMR_VIEWER_MESSAGE_INC
#define HMR_VIEWER_MESSAGE_INC 100
#
/*===hmrMessage===
受信データをモジュールに引き渡すMessageの実装タイプ
Mapで制御する

hmrMessage:v1_00/130310
	cRecvMessage,cSendMessage,cMessageを追加
*/
#include<string>
#include<map>
#include<utility>
#include <HomuraViewer/chrono.hpp>
#include"Message/Message.hpp"
#include"Message/Com.hpp"
#include"Message/ComLog.hpp"
#include"Message/MessageDriver.hpp"
#include"Message/DxComSUI.hpp"
#include"Message/DxMessageDriverSUI.hpp"

namespace hmr{
	namespace viewer{
		struct cMessage{
			//Com, Message, Operatorを宣言
			message::cCom Com;
			message::cComLog ComLog;

			message::cCom::VMC1Creater<1> ComVMC;

			//Message, Operator
			cMessageHost MessageHost;
			message::cFHDxMessageDriver MessageDriver;

			message::dxosComSUI ComSUI;
			message::dxosMessageDriverSUI MessageDriverSUI;
		public:
			cMessage()
				: ComVMC(&Com)
				, MessageDriver(&MessageHost, &Com, true, std::chrono::milliseconds(250), std::chrono::seconds(1)){
				ComLog.slot_pushInLogBuf(Com.signal_finRecvPacket);
				ComLog.slot_pushOutLogBuf(Com.signal_finSendPacket);

				Com.contactIsFinishedRecv(MessageDriver.IsFinishedRecvInq);

				Com.contactRecvBufSize(ComSUI.RecvBufSizeInq);
				Com.contactSendBufSize(ComSUI.SendBufSizeInq);
				Com.contactLastRecvPacTime(ComSUI.RecvTimeInq);
				Com.contactLastSendPacTime(ComSUI.SendTimeInq);

				Com.slot_clearRecvBuf(ComSUI.signal_require_clearRecvBuf);
				Com.slot_clearSendBuf(ComSUI.signal_require_clearSendBuf);

				MessageDriver.slot_setIsSendActiveFlag(MessageDriverSUI.signal_require_setStartFlag);
				MessageDriver.slot_setIsFullDuplex(MessageDriverSUI.signal_require_isFullDuplex);
				MessageDriver.slot_setInterval(MessageDriverSUI.signal_require_setTimeInterval);
				MessageDriver.slot_setTimeOut(MessageDriverSUI.signal_require_setTimeOut);

				MessageDriver.contactIsSendActive(MessageDriverSUI.startFlagInq);
				MessageDriver.contactIsFullDuplex(MessageDriverSUI.isFullDuplexFlagInq);
				MessageDriver.contactInterval(MessageDriverSUI.timeIntervalInq);
				MessageDriver.contactTimeOut(MessageDriverSUI.timeOutInq);

				MessageDriverSUI.slot_setIsReceivedFlag(MessageDriver.signal_informFinReceive);
				MessageDriverSUI.slot_setIsSendedFlag(MessageDriver.signal_informFinSend);
				MessageDriverSUI.slot_setIsTimeOutedFlag(MessageDriver.signal_informTimeOut);
			}
			void regist(unsigned char ID, itfMessageAgent& MsgAgent_){
				MessageHost.regist(ID, &MsgAgent_);
			}
			void operator()(void){
				MessageDriver();
			}
		};
	}
}
#
#endif
