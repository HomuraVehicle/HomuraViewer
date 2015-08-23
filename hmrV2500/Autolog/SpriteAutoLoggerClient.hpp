#ifndef HMR_AUTOLOG_SPRITEAUTOLOGGERCLIENT_INC
#define HMR_AUTOLOG_SPRITEAUTOLOGGERCLIENT_INC 100
#
#include<fstream>
#include<boost/format.hpp>
#include"AutoLoggerClient.hpp"
namespace hmr {
	namespace autolog {
		class cSpriteAutoLoggerClient :public itfAutoLoggerClient{
		private:
			unsigned int DataCnt;
			unsigned int DataNum;
			bool IsObserve;
			bool ReqLight;

			unsigned char LightONHour;
			unsigned char LightOFFHour;

			std::string Path;
			hmr::clock::time_point NextCommand;
		private:
			void newData(const std::string& Data_, clock::time_point Time_) {
				if(IsObserve && DataCnt<DataNum) {
					hmr::date Date(Time_);
					std::ofstream fout(Path+(boost::format("autolog_data_sprite_%02d%02d%02d_%02d%02d%02d.jpg")%Date.Year%Date.Month%Date.Day%Date.Hour%Date.Min%Date.Sec).str());
					fout<<Data_;
					++DataCnt;
				}
			}
		public:
			boost::signals2::signal<void(bool)> signal_setStatusMode;
			hmLib::inquiries::inquiry<bool> inquiry_isStatusMode;
			hmLib::inquiries::inquiry<bool> inquiry_isError;

			boost::signals2::signal<void(void)> signal_request_takePic;
			hmLib::inquiries::inquiry<bool> inquiry_is_takePic;

			boost::signals2::signal<void(bool)> signal_requestAutoLight;
			hmLib::inquiries::inquiry<bool> inquiry_isAutoLight;

			boost::signals2::signal<void(bool)> signal_setAutoResetMode;
			hmLib::inquiries::inquiry<bool> inquiry_isAutoResetMode;

			void slot_newData(boost::signals2::signal<void(const std::string&, clock::time_point)>& Signal_) {
				hmLib::signals::connect(Signal_, [&](const std::string& Data_, clock::time_point Time_)->void { newData(Data_, Time_); });
			}
		public:
			cSpriteAutoLoggerClient(unsigned int DataSamplingNum_, unsigned char LightONHour_, unsigned char LightOFFHour_)
				: DataCnt(0)
				, DataNum(DataSamplingNum_)
				, IsObserve(false)
				, LightONHour(LightONHour_)
				, LightOFFHour(LightOFFHour_){
			}
		protected:
			//�������O�X�^�[�g
			void autolog_start(const std::string& Path_)override { Path=Path_; }
			//�������O�X�g�b�v
			void autolog_stop() override{}
			//observe�^�C�������O�ʒm����
			void autolog_setup_observe()override {
				DataCnt=0;
				IsObserve=false;

				hmr::date Date;
				Date.from_time(hmr::clock::now());
				ReqLight=(Date.Hour<LightOFFHour || LightONHour<Date.Hour);
			}
			//observe�^�C���ȍ~�A�}�C�X�e�b�v���ƂɎ��s�����
			//	���g�̊ϑ����I���܂ł�true�A�I�������false��Ԃ�
			//	���̖߂�l�����āAHost����Observe���I�����Ă������ǂ��������Ă���
			bool autolog_observe(hmr::clock::time_point ObserveTime_)override {
				//�f�[�^�ϑ����܂��ł��Ȃ��ꍇ
				if(!IsObserve && (!inquiry_isStatusMode() || ReqLight!=inquiry_isAutoLight() || !inquiry_isAutoResetMode())) {
					if(NextCommand<hmr::clock::now()) {
						if(!inquiry_isStatusMode())signal_setStatusMode(true);
						if(ReqLight!=inquiry_isAutoLight())signal_requestAutoLight(ReqLight);
						if(!inquiry_isAutoResetMode())signal_setAutoResetMode(true);

						NextCommand=hmr::clock::now()+std::chrono::seconds(5);
					}
					return true;
				}

				//�f�[�^�ϑ����ԂɂȂ��Ă��Ȃ��ꍇ
				if(hmr::clock::now()<ObserveTime_)return true;
				IsObserve=true;

				//�f�[�^�ϑ��������A�K�v�f�[�^���擾���Ă��Ȃ��ꍇ
				if(DataCnt<DataNum) {
					if(!inquiry_isError() && !inquiry_is_takePic() && NextCommand<hmr::clock::now()) {
						signal_request_takePic();
						NextCommand=hmr::clock::now()+std::chrono::seconds(5);
					}
					return true;
				}

				return false;
			}
		};
	}
}
#
#endif
