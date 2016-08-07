#ifndef HMR_VIEWER_MESSAGE_MESSAGE_INC
#define HMR_VIEWER_MESSAGE_MESSAGE_INC 100
#
/*===hmrMessage===
受信データをモジュールに引き渡すMessageの実装タイプ
Mapで制御する

hmrMessage:v1_00/130310
	cRecvMessage,cSendMessage,cMessageを追加
*/
#include<string>
#include<map>
#include<utility>
#include <HomuraViewer/chrono.hpp>
#include"ItfMessage.hpp"
namespace hmr{
	namespace viewer{
		class cRecvMessageHost :public itfRecvMessageHost{
			using datum = message::datum;
		private:
			std::map<datum::id_type, itfRecvMessageAgent*> Agents;
		public:
			void regist(datum::id_type ID_, itfRecvMessageAgent* pAgent_)override{
				Agents.insert(std::make_pair(ID_, pAgent_));
			}
			void setup_listen()override{
				for(auto itr = std::begin(Agents); itr != std::end(Agents); ++itr){
					itr->second->setup_listen();
				}
			}
			bool listen(const datum& Dat_)override{
				auto itr = Agents.find(Dat_.ID);
				if(itr == Agents.end())return true;
				return itr->second->listen(Dat_.Time, Dat_.ErrFlag, Dat_.Str);
			}
		};
		class cSendMessageHost :public itfSendMessageHost{
			using datum = message::datum;
		private:
			std::map<datum::id_type, itfSendMessageAgent*> Agents;
			datum::time_point Time;
		public:
			void regist(datum::id_type ID_, itfSendMessageAgent* pAgent_)override{
				Agents.insert(std::make_pair(ID_, pAgent_));
			}
			void setup_talk()override{
				for(auto itr = std::begin(Agents); itr != std::end(Agents); ++itr){
					itr->second->setup_talk();
				}
				Time = clock::now();
			}
			bool talk(datum& Dat_)override{
				//			std::string Str;
				for(auto itr = std::begin(Agents); itr != std::end(Agents); ++itr){
					if(!itr->second->talk(Dat_.Str)){
						Dat_.ID = itr->first;
						Dat_.ErrFlag = false;
						Dat_.Time = Time;
						Dat_.Size = static_cast<vmc1_dsize_t>(Dat_.Str.size());
						return false;
					}
				}
				return true;
			}
		};
		class cMessageHost :public itfMessageHost{
			using datum = message::datum;
		private:
			cRecvMessageHost RecvMessageHost;
			cSendMessageHost SendMessageHost;
		public://cRecvMessage
			void regist(datum::id_type ID_, itfRecvMessageAgent* pRecvAgent_)override{
				RecvMessageHost.regist(ID_, pRecvAgent_);
			}
			void setup_listen()override{
				RecvMessageHost.setup_listen();
			}
			bool listen(const datum& Dat_)override{
				return RecvMessageHost.listen(Dat_);
			}
		public://cSendMessage
			void regist(datum::id_type ID_, itfSendMessageAgent* pSendAgent_)override{
				SendMessageHost.regist(ID_, pSendAgent_);
			}
			void setup_talk()override{
				SendMessageHost.setup_talk();
			}
			bool talk(datum& Dat_)override{
				return SendMessageHost.talk(Dat_);
			}
		public:
			void regist(datum::id_type ID_, itfMessageAgent* pAgent_){
				RecvMessageHost.regist(ID_, pAgent_);
				SendMessageHost.regist(ID_, pAgent_);
			}
		};
	}
}
#
#endif
