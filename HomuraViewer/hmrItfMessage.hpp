#ifndef HMR_ITFMESSAGE_INC
#define HMR_ITFMESSAGE_INC 101
#
/*===hmrItfMessage===
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
#include <hmrVLib/Data.hpp>
namespace hmr{
	class itfRecvMessageAgent{
	public:
		virtual void setup_listen(){}
		virtual bool listen(datum::time_point Time_, bool Err_,const std::string& Str_)=0;
	};
	class itfSendMessageAgent{
	public:
		virtual void setup_talk(){}
		virtual bool talk(std::string& Str_)=0;
	};
	class itfMessageAgent
		:public itfRecvMessageAgent
		,public itfSendMessageAgent{
	};
	class itfRecvMessage{
	public:
		virtual void regist(datum::id_type ID_,itfRecvMessageAgent* pAgent_)=0;
		virtual void setup_listen()=0;
		virtual bool listen(const datum& Dat_)=0;
	};
	class itfSendMessage{
	public:
		virtual void regist(datum::id_type ID_,itfSendMessageAgent* pAgent_)=0;
		virtual void setup_talk()=0;
		virtual bool talk(datum& Dat_)=0;
	};
	class itfMessage
		:public itfRecvMessage
		,public itfSendMessage{
	public:
		virtual void regist(datum::id_type ID_,itfMessageAgent* pAgent_){
			//static_cast���g�킸�ɃN���X�w��ŃA�N�Z�X����ƁA�����N�G���[
			static_cast<itfRecvMessage*>(this)->regist(ID_,pAgent_);
			static_cast<itfSendMessage*>(this)->regist(ID_,pAgent_);
		}
	};
}
#
#endif
