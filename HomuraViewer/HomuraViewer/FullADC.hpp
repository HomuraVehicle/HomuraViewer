#ifndef HMR_FULLADC_INC
#define HMR_FULLADC_INC 100
#
/*=== FullADC ===
ADC全チャンネル取得用モジュール

*/

#include"FullADC/FullADCData.hpp"
#include"FullADC/FullADCMsgAgent.hpp"
#include"FullADC/DxFullADCMUI.hpp"
#include"FullADC/FullADCFileAgent.hpp"
namespace hmr{
	namespace viewer{
		struct cFullADC{
			using dxosMUI = fulladc::dxosMUI;
		public:
			fulladc::cMsgAgent<8> MsgAgent;
			fulladc::cFileAgent<8> FileAgent;
		public:
			cFullADC():FileAgent("FullADC.txt",'\t'){
				FileAgent.slot_logData(MsgAgent.signal_newData);
			}
			void connect(fulladc::dxosMUI& MUI){
				MUI.slot_getData(MsgAgent.signal_newData);

				MsgAgent.slot_setDataMode(MUI.IsDataModeMUIBut.Signal);
				MsgAgent.contact_getPicDataMode(MUI.IsDataModeMUIBut.Pic);
				MsgAgent.contact_getReqDataMode(MUI.IsDataModeMUIBut.Req);

				MsgAgent.slot_setCtr1(MUI.Ctr1MUIBut.Signal);
				MsgAgent.contact_getPicCtr1(MUI.Ctr1MUIBut.Pic);
				MsgAgent.contact_getReqCtr1(MUI.Ctr1MUIBut.Req);

				MsgAgent.slot_setCtr2(MUI.Ctr2MUIBut.Signal);
				MsgAgent.contact_getPicCtr2(MUI.Ctr2MUIBut.Pic);
				MsgAgent.contact_getReqCtr2(MUI.Ctr2MUIBut.Req);

				MsgAgent.slot_setCtr3(MUI.Ctr3MUIBut.Signal);
				MsgAgent.contact_getPicCtr3(MUI.Ctr3MUIBut.Pic);
				MsgAgent.contact_getReqCtr3(MUI.Ctr3MUIBut.Req);

				MsgAgent.slot_setCtr4(MUI.Ctr4MUIBut.Signal);
				MsgAgent.contact_getPicCtr4(MUI.Ctr4MUIBut.Pic);
				MsgAgent.contact_getReqCtr4(MUI.Ctr4MUIBut.Req);
			}
		};
	}
}

#
#endif
