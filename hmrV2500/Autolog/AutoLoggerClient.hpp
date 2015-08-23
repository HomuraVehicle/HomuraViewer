#ifndef HMR_AUTOLOGGER_AUTOLOGGERCLIENT_INC
#define HMR_AUTOLOGGER_AUTOLOGGERCLIENT_INC 100
#
#include<hmrChrono.hpp>
namespace hmr {
	namespace autolog {
		class itfAutoLoggerClient {
		private:
			bool IsActive;
			bool IsStart;
			std::string Path;
		protected:
			virtual void autolog_start(const std::string& Path_)=0;
			virtual void autolog_stop()=0;
			virtual void autolog_setup_observe()=0;
			virtual bool autolog_observe(hmr::clock::time_point ObserveTime_)=0;
		public:
			itfAutoLoggerClient() :IsActive(false), IsStart(false) {}
			void activate() {
				if(is_active())return;
				IsActive=true; 
				if(IsStart)autolog_start(Path);
			}
			void inactivate() {
				if(!is_active())return;
				IsActive=false;
				if(IsStart)autolog_stop();
			}
			bool is_active()const {
				return IsActive;
			}
			//���O�p�X�ƂƂ��ɁA�������O��ʒm
			void start(const std::string& Path_) {
				if(is_start())return;

				IsStart=true;
				Path=Path_;

				if(IsActive)autolog_start(Path_);
			}
			//�������O�I����ʒm
			void stop() {
				if(!is_start())return;
				IsStart=false;
				
				if(IsActive)autolog_stop();
			}
			//�X�^�[�g���Ă��邩�ǂ���
			bool is_start()const {
				return IsStart;
			}
			//observe�^�C�������O�ʒm����
			void setup_observe() {
				autolog_setup_observe();
			}
			//observe�^�C���ȍ~�A�}�C�X�e�b�v���ƂɎ��s�����
			//	���g�̊ϑ����I���܂ł�true�A�I�������false��Ԃ�
			//	���̖߂�l�����āAHost����Observe���I�����Ă������ǂ��������Ă���
			bool observe(hmr::clock::time_point ObserveTime_) {
				if(!IsActive)return false;
				return autolog_observe(ObserveTime_);
			}
		};
	}
}
#
#endif
