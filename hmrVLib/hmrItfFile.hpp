#ifndef HMR_ITFFILE_INC
#define HMR_ITFFILE_INC 100
#
/*===hmrFile===
ログファイルを管理するクラスインターフェース
*/
#include<string>

#define FILE_COMMENTOUT_CHARS "#"
#define FILE_DEVIDE_CHARS ", " 

namespace hmr{
	class itfFileAgent{
	public:
		virtual void activate(const std::string& Path_)=0;
		virtual bool is_active()const=0;
		virtual void inactivate()=0;
	};
	class itfFile:public itfFileAgent{
	public:
		virtual void regist(itfFileAgent* pAgent_)=0;
	};
}
#endif
