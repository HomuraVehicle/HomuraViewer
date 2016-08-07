#ifndef HMR_VIEWER_IO_INC
#define HMR_VIEWER_IO_INC 100
#
#include"IO/IODriver.hpp"
#include"IO/IOLogGate.hpp"
#include"IO/BufGate.hpp"
#include"IO/GateSwitcher.hpp"

#include"IO/DxGateSwitcherSUI.hpp"
#include"IO/DxBufGateSUI.hpp"
#include"IO/DxIOLogGateSUI.hpp"
#include"IO/DxIODriverSUI.hpp"
#include"IO/DxVMCSUI.hpp"

#include"Message.hpp"
namespace hmr{
	namespace viewer{
		struct cIO{
			using this_iologbuf = io::fdx_vector_iologbuf<io::system_clock_iologtype>;
			using this_iologgate = io::iologgate<io::fdx_crlf_timeout_iologger<io::system_clock_iologtype>>;
		public:
			//GateSwitcherを宣言
			io::cGateSwitcher GateSW;
			io::bufgate BufGate;
			this_iologgate ioLogGate;
			this_iologbuf ioLogBuf;
			io::cFHdxIODriver IODriver;

			//SUI
			io::dxosGateSwitcherSUI GateSwSUI;
			io::dxosBufGateSUI BufGateSUI;
			io::dxosIOLogGateSUI ioLogGateSUI;
			io::dxosIODriverSUI IODriverSUI;
			io::dxosVMCSUI VMCSUI;
		public:
			template<typename VMC_creater>
			cIO(VMC_creater Creater, message::cFHDxMessageDriver& MessageDriver) :IODriver(Creater){
				GateSW.readFomaSetting("hmr\\phone_config.txt");
				BufGate.open(GateSW);
				ioLogGate.open(BufGate);

				//--- MessageDriver ---
				IODriver.open(&ioLogGate);
				IODriver.slot_VMC_force_end_recv(MessageDriver.signal_require_VMC_force_end_recv);
				IODriver.slot_VMC_force_end_send(MessageDriver.signal_require_VMC_force_end_send);

				ioLogBuf.slot_pushInLogBuf(ioLogGate.Logger.signal_resetInLog);
				ioLogBuf.slot_pushOutLogBuf(ioLogGate.Logger.signal_resetOutLog);

				//=== SUI connect ===
				//--- GateSw ---
				// GateSwUI を　GateSwitcher　に　つなげる
				// Switch系の　require signal　をつなげる
				GateSW.slot_set_and_openRS(GateSwSUI.signal_require_openRS);
				GateSW.slot_set_and_openFOMA(GateSwSUI.signal_require_openFOMA);
				GateSW.slot_closePort(GateSwSUI.signal_require_closeRS);
				GateSW.slot_closePort(GateSwSUI.signal_require_closeFOMA);

				// Switch系の　データのシンクロ　を行う
				GateSwSUI.slot_sync_isFOMAOpen(GateSW.signal_inform_isFOMAOpen);
				GateSwSUI.slot_sync_isRSOpen(GateSW.signal_inform_isRSOpen);

				// Inquire 系列の接続
				GateSW.contact_is_open(GateSwSUI.isOpenInq);
				GateSW.contact_can_put(GateSwSUI.canSendInq);
				GateSW.contact_can_get(GateSwSUI.canGetInq);


				//--- BufGate SUI ---
				BufGate.slot_clearSendBuf(BufGateSUI.signal_require_clearOutBuf);
				BufGate.slot_clearRecvBuf(BufGateSUI.signal_require_clearInBuf);

				BufGate.contact_sendBufSize(BufGateSUI.outBufSizeInq);
				BufGate.contact_recvBufSize(BufGateSUI.inBufSizeInq);

				BufGate.contact_is_open(BufGateSUI.isOpenInq);
				BufGate.contact_can_get(BufGateSUI.canGetInq);
				BufGate.contact_can_put(BufGateSUI.canSendInq);

				//--- IOLogGate SUI ---
				ioLogBuf.slot_clearLogBuf(ioLogGateSUI.signal_require_clearBuf);
				ioLogBuf.contactLogBufSize(ioLogGateSUI.BufSizeInq);

				ioLogGate.contact_is_open(ioLogGateSUI.isOpenInq);
				ioLogGate.contact_can_put(ioLogGateSUI.canSendInq);
				ioLogGate.contact_can_get(ioLogGateSUI.canGetInq);

				//--- IODriver ---
				IODriver.contact_is_open(IODriverSUI.isOpenInq);
				IODriver.contact_can_get(IODriverSUI.canGetInq);
				IODriver.contact_can_put(IODriverSUI.canSendInq);

				//--- VMC ---
				IODriver.contactRecvErr(VMCSUI.recvErrInq);
				IODriver.contactSendErr(VMCSUI.sendErrInq);
				IODriver.slot_VMC_clearRecvErr(VMCSUI.signal_require_clear_recvErr);
				IODriver.slot_VMC_clearSendErr(VMCSUI.signal_require_clear_sendErr);
				IODriver.slot_VMC_force_end_recv(VMCSUI.signal_require_end_recv);
				IODriver.slot_VMC_force_end_send(VMCSUI.signal_require_end_send);

				VMCSUI.slot_set_forceResetRecvTime(IODriver.signal_inform_VMC_force_end_recv);
				VMCSUI.slot_set_forceResetSendTime(IODriver.signal_inform_VMC_force_end_send);
			}
			void operator()(void){
				BufGate();
				IODriver.work();
			}
		};
	}
}
#
#endif
