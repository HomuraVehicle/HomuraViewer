#ifndef HMR_VIEWER_FILE_INC
#define HMR_VIEWER_FILE_INC 100
#
#include"File/ItfFile.hpp"
#include"File/TimeDirectoryFile.hpp"
#include"File/CSVFileAgent.hpp"
#include"File/UniSensorFileAgent.hpp"
#include"File/ConstNameFileAgent.hpp"
#include"File/ConstNameDirectoryFile.hpp"
#include"File/DxFileSUI.hpp"
namespace hmr{
	namespace viewer{
		struct cFile{
		public:
			cConstNameDirectoryFile Master;
			file::dxosSUI SUI;
		public:
			cFile():Master("Data"){
				SUI.slot_setIsActive(Master.signal_isActivate);
				Master.slot_activate(SUI.signal_require_activate);
				Master.slot_inactivate(SUI.signal_require_inactivate);
			}
			void regist(itfFileAgent& FileAgent_){
				Master.regist(&FileAgent_);
			}
		};
	}
}
#
#endif
