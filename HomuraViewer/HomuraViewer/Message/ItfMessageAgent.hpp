#ifndef HMR_VIEWER_MESSAGE_ITFMESSAGEAGENT_INC
#define HMR_VIEWER_MESSAGE_ITFMESSAGEAGENT_INC 101
#
/*===ItfMessageAgent===
�󂯎�����f�[�^���e���W���[���ɓ`����Message�N���X�̃C���^�[�t�F�[�X

hmrItfMesasge:v1_01/130324 hmIto
	connect�֐���regist�ɕύX
	itfMesasge::regist�֐����ŁA�N���X�w��ŃA�N�Z�X����ƃ����N�G���[�ɂȂ�����C��
hmrItfMessage:v1_00/130310 hmIto
	itfRecvMessageAgent,itfSendMessageAgent,itfMessageAgent��ǉ�
	itfRecvMessage,itfSendMessage,itfMessage��ǉ�
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
