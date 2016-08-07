#ifndef HMR_VIEWER_MESSAGE_ITFMESSAGEAGENT_INC
#define HMR_VIEWER_MESSAGE_ITFMESSAGEAGENT_INC 101
#
/*===ItfMessageAgent===
受け取ったデータを各モジュールに伝えるMessageクラスのインターフェース

hmrItfMesasge:v1_01/130324 hmIto
	connect関数をregistに変更
	itfMesasge::regist関数内で、クラス指定でアクセスするとリンクエラーになる問題を修正
hmrItfMessage:v1_00/130310 hmIto
	itfRecvMessageAgent,itfSendMessageAgent,itfMessageAgentを追加
	itfRecvMessage,itfSendMessage,itfMessageを追加
*/
#include <string>
#include <map>
#include <utility>
#include <HomuraViewer/chrono.hpp>
#include "PacketData.hpp"
namespace hmr{
	namespace viewer{
		class itfRecvMessageAgent{
			using datum = message::datum;
		public:
			virtual void setup_listen(){}
			virtual bool listen(datum::time_point Time_, bool Err_, const std::string& Str_) = 0;
		};
		class itfSendMessageAgent{
		public:
			virtual void setup_talk(){}
			virtual bool talk(std::string& Str_) = 0;
		};
		class itfMessageAgent
			:public itfRecvMessageAgent
			, public itfSendMessageAgent{
		};
	}
}
#
#endif
