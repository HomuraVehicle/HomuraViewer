#ifdef HMR_MAIN_INC_END
#ifndef HMR_CONNECTSUI_INC
#define HMR_CONNECTSUI_INC 100
#
namespace hmr{
	namespace viewer{
		// SUI 関連のコネクト
	#if defined(HMR_DXFILE_SUI_INC) && defined(HMR_VIEWER_TIMEDIRECTORYFILE_INC)
		void connect(dxosFileSUI& fileSUI_, cTimeDirectoryFile& agt_){
			fileSUI_.slot_setIsActive(agt_.signal_isActivate);
			agt_.slot_activate(fileSUI_.signal_require_activate);
			agt_.slot_inactivate(fileSUI_.signal_require_inactivate);
		}
	#endif

	#if defined(HMR_DXFILE_SUI_INC) && defined(HMR_VIEWER_CONSTNAMEDIRECTORYFILE_INC)
		void connect(dxosFileSUI& fileSUI_, cConstNameDirectoryFile& agt_){
			fileSUI_.slot_setIsActive(agt_.signal_isActivate);
			agt_.slot_activate(fileSUI_.signal_require_activate);
			agt_.slot_inactivate(fileSUI_.signal_require_inactivate);
		}
	#endif
	}
}
#
#endif
#endif