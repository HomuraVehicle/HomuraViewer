#ifndef HMR_VIEWER_MESSAGE_ITFMESSAGE_INC
#define HMR_VIEWER_MESSAGE_ITFMESSAGE_INC 101
#
/*===ItfMessage===
�󂯎�����f�[�^���e���W���[���ɓ`����Message�N���X�̃C���^�[�t�F�[�X

hmrItfMesasge:v1_01/130324 hmIto
	connect�֐���regist�ɕύX
	itfMesasge::regist�֐����ŁA�N���X�w��ŃA�N�Z�X����ƃ����N�G���[�ɂȂ�����C��
hmrItfMessage:v1_00/130310 hmIto
	itfRecvMessageAgent,itfSendMessageAgent,itfMessageAgent��ǉ�
	itfRecvMessage,itfSendMessage,itfMessage��ǉ�
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
				//static_cast���g�킸�ɃN���X�w��ŃA�N�Z�X����ƁA�����N�G���[
				static_cast<itfRecvMessageHost*>(this)->regist(ID_, pAgent_);
				static_cast<itfSendMessageHost*>(this)->regist(ID_, pAgent_);
			}
		};
	}
}
#
#endif
