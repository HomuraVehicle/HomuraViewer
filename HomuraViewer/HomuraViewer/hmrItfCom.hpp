#ifndef HMR_ITFCOM_INC
#define HMR_ITFCOM_INC 100
#
#include <HomuraViewer/Data.hpp>

/*===hmrItfCom===
hmrItfCom:v1_01/130324 hmIto
	is_empty/is_full��empty/full�ɕύX
	�e�֐��̃R�����g����������
hmrItfCom:v1_00/130310 hmito
	�֐���������
Sample_for_vmc_hmrAbtCom: v1_00/130302 amby
	�Ƃ肠������g���쐬���Ă݂�B

�v�z
�@Operator�ɕK�v�Œ���̋@�\��񋟂��邽�߂̃N���X�B
�@hmrCom��Operator�̊֌W�𖾊m�ɂ���

*/
namespace hmr{
	class itfRecvCom{
	public:
		//��M�o�b�t�@���̃f�[�^�̑��݂̗L�����m�F����
		virtual bool empty()=0;
		//��M�o�b�t�@������f�[�^���擾����
		virtual hmr::datum recv()=0;
		//��M�X�g���[������X�V����K�v�����邩���m�F����
		virtual bool can_sync() = 0;
		//��M�X�g���[�������M�o�b�t�@���X�V����
		virtual void sync()=0;
	};
	class itfSendCom{
	public:
		//���M�o�b�t�@�̋󂫂��m�F����֐�
		virtual bool full()=0;
		//���M�o�b�t�@�Ƀf�[�^��ǉ�����
		virtual void send(const hmr::datum& dat_)=0;
		//���M�X�g���[���ɑ��M�o�b�t�@�𔽉f����
		virtual void flush()=0;
		//���M�X�g���[���ɔ��f�\�����m�F����
		virtual bool can_flush() = 0;
	};
	class itfCom:public itfRecvCom,public itfSendCom{};
}

#
#endif
