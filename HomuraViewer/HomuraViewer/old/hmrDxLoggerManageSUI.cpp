#ifndef HMR_DXLOGGERMANAGESUI_CPP_INC
#define HMR_DXLOGGERMANAGESUI_CPP_INC 100
#
#include<hmLib/config_vc.h>
#include"hmrDxLoggerManageSUI.hpp"

namespace hmr{
	namespace viewer{
		const hmLib_uint16 dxosLoggerMngSUI::LogData_BufSize[HMR_LOGGERSUI_BUFFERSIZE_SIZE] = {50, 100, 250, 500, 1000, 5000, 10000, 20000, 40000, 60000};
	}
}

#
#endif
