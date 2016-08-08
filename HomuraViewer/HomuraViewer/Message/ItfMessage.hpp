#ifndef HMR_VIEWER_MESSAGE_ITFMESSAGE_INC
#define HMR_VIEWER_MESSAGE_ITFMESSAGE_INC 101
#
/*===ItfMessage===
受け取ったデータを各モジュールに伝えるMessageクラスのインターフェース

hmrItfMesasge:v1_01/130324 hmIto
	connect関数をregistに変更
	itfMesasge::regist関数内で、クラス指定でアクセスするとリンクエラーになる問題を修正
hmrItfMessage:v1_00/130310 hmIto
	itfRecvMessageAgent,itfSendMessageAgent,itfMessageAgentを追加
	itfRecvMessage,itfSendMessage,itfMessageを追加
*/
#include<string>
#include<map>
#include<utility>
#include <HomuraViewer/chrono.hpp>
#include <HomuraViewer/Message/PacketData.hpp>
#include "ItfMessageAgent.hpp"
namespace hmr{
	namespace viewer{
		class itfRecvMessageHost{
			using datum = message::datum;
		public:
			virtual void regist(datum::id_type ID_, itfRecvMessageAgent* pAgent_) = 0;
			virtual void setup_listen() = 0;
			virtual bool listen(const datum& Dat_) = 0;
		};
		class itfSendMessageHost{
			using datum = message::datum;
		public:
			virtual void regist(datum::id_type ID_, itfSendMessageAgent* pAgent_) = 0;
			virtual void setup_talk() = 0;
			virtual bool talk(datum& Dat_) = 0;
		};
		class itfMessageHost
			:public itfRecvMessageHost
			, public itfSendMessageHost{
			using datum = message::datum;
		public:
			virtual void regist(datum::id_type ID_, itfMessageAgent* pAgent_){
				//static_castを使わずにクラス指定でアクセスすると、リンクエラー
				static_cast<itfRecvMessageHost*>(this)->regist(ID_, pAgent_);
				static_cast<itfSendMessageHost*>(this)->regist(ID_, pAgent_);
			}
		};
	}
}
#
#endif
