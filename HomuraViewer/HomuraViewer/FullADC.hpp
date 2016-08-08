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
			fulladc::dxosMUI MUI;
		public:
			cFullADC():FileAgent("FullADC.txt",'\t'){
				FileAgent.slot_logData(MsgAgent.signal_newData);

				MUI.slot_getData(MsgAgent.signal_newData);
				MUI.DataModeMUIBut.connect(MsgAgent.DataMode);
			}
		};
	}
}

#
#endif
