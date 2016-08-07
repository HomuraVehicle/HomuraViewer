#ifdef HMR_MAIN_INC_END
#ifndef HMR_CONNECTCORE_INC
#define HMR_CONNECTCORE_INC 100
#

//#include "hmrDxGateSwitcher.hpp"
//#include "hmrGateSwitcher.hpp"

/*===hmrConnect===
接続補助関数群を定義する
*/
namespace hmr{
	namespace viewer{

	#if defined(HMR_OPERATOR_INC) && defined(HMR_COM_INC) && defined(HMR_VIEWER_IO_IODRIVER_INC)
		void connect(cFullRecvTimeSendOperator& ope_, io::cFHdxIODriver& io_, cCom& com_){
			io_.slot_VMC_force_end_recv(ope_.signal_require_VMC_force_end_recv);
			io_.slot_VMC_force_end_send(ope_.signal_require_VMC_force_end_send);
			//com_.contactLastRecvDatTime(ope_.LastRecvDatTimeInq);
			//com_.contactLastRecvPacTime(ope_.LastRecvPacTimeInq);
			//com_.contactLastSendPacTime(ope_.LastSendPacTimeInq);
			com_.contactIsFinishedRecv(ope_.IsFinishedRecvInq);
		}
		void connect(cFHDxOperator& ope_, io::cFHdxIODriver& io_, cCom& com_){
			io_.slot_VMC_force_end_recv(ope_.signal_require_VMC_force_end_recv);
			io_.slot_VMC_force_end_send(ope_.signal_require_VMC_force_end_send);
			//com_.contactLastRecvDatTime(ope_.LastRecvDatTimeInq);
			//com_.contactLastRecvPacTime(ope_.LastRecvPacTimeInq);
			//com_.contactLastSendPacTime(ope_.LastSendPacTimeInq);
			com_.contactIsFinishedRecv(ope_.IsFinishedRecvInq);
		}
	#endif
	}
}
#
#endif
#endif
