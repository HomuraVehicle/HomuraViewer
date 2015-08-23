#ifdef HMR_MAIN_INC_END
#ifndef HMR_CONNECTCORE_INC
#define HMR_CONNECTCORE_INC 100
#

//#include "hmrDxGateSwitcher.hpp"
//#include "hmrGateSwitcher.hpp"

/*===hmrConnect===
ê⁄ë±ï‚èïä÷êîåQÇíËã`Ç∑ÇÈ
*/
namespace hmr{
#if defined(HMR_COMLOG_INC) && defined(HMR_COM_INC)
	void connect(cComLog& ComLog_,cCom& Com_){
		ComLog_.slot_pushInLogBuf(Com_.signal_finRecvPacket);
		ComLog_.slot_pushOutLogBuf(Com_.signal_finSendPacket);
	}
#endif

#if defined(IOLOGGATE_INC)
	template<class iologtype>
	void connect(fdx_vector_iologbuf<iologtype>& IOLogBuf_,iologgate<fdx_crlf_timeout_iologger<iologtype>>& IOLogGate_){
		IOLogBuf_.slot_pushInLogBuf(IOLogGate_.Logger.signal_resetInLog);
		IOLogBuf_.slot_pushOutLogBuf(IOLogGate_.Logger.signal_resetOutLog);
	}
#endif

#if defined(HMR_OPERATOR_INC) && defined(HMR_COM_INC) && defined(HMRIO_INC)
	void connect(cFullRecvTimeSendOperator& ope_, cFHdxIO& io_,cCom& com_){
		io_.slot_VMC_force_end_recv(ope_.signal_require_VMC_force_end_recv);
		io_.slot_VMC_force_end_send(ope_.signal_require_VMC_force_end_send);
		//com_.contactLastRecvDatTime(ope_.LastRecvDatTimeInq);
		//com_.contactLastRecvPacTime(ope_.LastRecvPacTimeInq);
		//com_.contactLastSendPacTime(ope_.LastSendPacTimeInq);
		com_.contactIsFinishedRecv(ope_.IsFinishedRecvInq);
	}
	void connect(cFHDxOperator& ope_, cFHdxIO& io_,cCom& com_){
		io_.slot_VMC_force_end_recv(ope_.signal_require_VMC_force_end_recv);
		io_.slot_VMC_force_end_send(ope_.signal_require_VMC_force_end_send);
		//com_.contactLastRecvDatTime(ope_.LastRecvDatTimeInq);
		//com_.contactLastRecvPacTime(ope_.LastRecvPacTimeInq);
		//com_.contactLastSendPacTime(ope_.LastSendPacTimeInq);
		com_.contactIsFinishedRecv(ope_.IsFinishedRecvInq);
	}
#endif






}
#
#endif
#endif