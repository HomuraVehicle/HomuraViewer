#ifndef HMR_AUTOLOG_AUTOLOGGER_INC
#define HMR_AUTOLOG_AUTOLOGGER_INC 100
#
#include<vector>
#include<fstream>
#include<hmLib/signals.hpp>
#include<hmLib/inquiries.hpp>
#include"Chrono.hpp"
#include"AutoLoggerClient.hpp"
#include<boost/filesystem.hpp>
namespace hmr {
	namespace autolog {
		class cAutoLogger {
			friend class test_cAutoLogger;
		private://User Config
			hmr::clock::time_point ObserveStartPoint;	//�ϑ����Ԏn�_
			hmr::clock::duration ObserveInteraval;		//�ϑ��Ԋu
			hmr::clock::duration PreObserveLength;		//�ϑ��O�������Ԓ�
			double HomuraTimeRate;
		public:
			void setObserveStartPoint(unsigned int Min_) {
				ObserveStartPoint=hmr::date(2000, 1, 1, 0, 0, 0, 0).to_time()+std::chrono::minutes(Min_);
			}
			unsigned int getObserveStartPoint() const{
				return std::chrono::duration_cast<std::chrono::minutes>(ObserveStartPoint - hmr::date(2000, 1, 1, 0, 0, 0, 0).to_time()).count();
			}
			void setObserveInterval(unsigned int Min_) {
				if(Min_>0)ObserveInteraval=std::chrono::minutes(Min_);
			}
			unsigned int getObserveInterval()const {
				return std::chrono::duration_cast<std::chrono::minutes>(ObserveInteraval).count();
			}
			void setPreObserveLength(unsigned int Min_) {
				if(Min_>=0)PreObserveLength=std::chrono::minutes(Min_);
			}
			unsigned int getPreObserveLength()const {
				return std::chrono::duration_cast<std::chrono::minutes>(PreObserveLength).count();
			}

		private:
			std::vector<itfAutoLoggerClient*> Clients;
		public:
			//Client�o�^
			void regist(itfAutoLoggerClient& rClient_) { Clients.push_back(&rClient_); }
		private:
			void setup_observe_forAllClients() {
				for(itfAutoLoggerClient* pClient:Clients) {
					pClient->setup_observe();
				}
			}
			bool observe_forAllClients(hmr::clock::time_point ObserveTime_) {
				bool IsLock=false;
				for(itfAutoLoggerClient* pClient:Clients) {
					IsLock=(pClient->observe(ObserveTime_)||IsLock);
				}
				return IsLock;
			}

		public:
			enum class status {
				SetSleep,	//Sleep���[�h�ݒ蒆
				ReqSleep,	//Sleep���[�h�֕ύX��
				Sleep,		//Sleep��
				Call,		//�Đڑ��g���C��
				ReqNormal,	//Normal���[�h�֕ύX��
				Observe		//�ϑ���
			};
		private://Inner Status
			status Status;
			hmr::clock::time_point NextAwake;
			hmr::clock::time_point NextObserve;
			hmr::clock::time_point NextCommand;
			hmr::clock::time_point NextCall;
		private:
			//���݂����Next���ԎY�o
			void setNextObserveTime() {
				NextObserve = ObserveStartPoint 
					+ ((std::chrono::duration_cast<std::chrono::seconds>(hmr::clock::now()-ObserveStartPoint).count()/std::chrono::duration_cast<std::chrono::seconds>(ObserveInteraval).count()) + 1)*ObserveInteraval;
				NextAwake = NextObserve - PreObserveLength;
			}
			//���݂���̃X���[�v���ԎZ�o�p
			std::pair<int, int> getSleepTime() {
				std::pair<int, int> SleepTimePair;

				//SleepTime
				SleepTimePair.first=static_cast<int>(std::chrono::duration_cast<std::chrono::seconds>(NextAwake-hmr::clock::now() - get_duration<duration_tag::sleep_command_lag>()).count()*HomuraTimeRate);
				SleepTimePair.second=static_cast<int>(std::chrono::duration_cast<std::chrono::seconds>(NextObserve-NextAwake + get_duration<duration_tag::sleep_command_lag>()).count()*HomuraTimeRate);
				return SleepTimePair;
			}
			//�e�탉�O���ԎY�o
			struct duration_tag {
				struct command_delay{};
				struct call_delay{};
				struct awake_wait_limit{};
				struct com_connect_limit{};
				struct com_disconnect_limit{};
				struct sleep_command_lag {};
				struct observe_limit {};
			};
			template<typename duration_tag_> hmr::clock::duration get_duration();
			template<> hmr::clock::duration get_duration<duration_tag::command_delay>(){return std::chrono::seconds(5); }
			template<> hmr::clock::duration get_duration<duration_tag::call_delay>() { return std::chrono::seconds(10); }//return std::chrono::minutes(2);}
			template<> hmr::clock::duration get_duration<duration_tag::awake_wait_limit>() { return std::chrono::seconds(45); }//return std::chrono::minutes(5);}
			template<> hmr::clock::duration get_duration<duration_tag::com_connect_limit>(){return std::chrono::seconds(5); }
			template<> hmr::clock::duration get_duration<duration_tag::com_disconnect_limit>(){return std::chrono::seconds(10); }
			template<> hmr::clock::duration get_duration<duration_tag::sleep_command_lag>(){return std::chrono::seconds(30); }
			template<> hmr::clock::duration get_duration<duration_tag::observe_limit>(){return std::chrono::minutes(10); }
		public:
			status getStatus()const { return Status; }
			std::string getStatusStr()const {
				if(IsReqStart==false)return "����";
				switch(Status) {
				case status::SetSleep:
					return "�x�~����";
				case status::ReqSleep:
					return "�x�~�v��";
				case status::Sleep:
					return "�x�~��";
				case status::Call:
					return "�ڑ�";
				case status::ReqNormal:
					return "�ϑ��v��";
				case status::Observe:
					return "�ϑ���";
				default:
					return "�s��";
				}
			}
			hmr::clock::time_point getNextAwake()const {return NextAwake;}
			hmr::clock::time_point getNextObserve()const {return NextObserve;}
			hmr::clock::time_point getNextCommand()const {return NextCommand;}
			hmr::clock::time_point getNextCall()const {return NextCall;}
		public:
			//DeviceManager�R���g���[��
			boost::signals2::signal<void(bool, int, int)> signal_setSleepMode;
			hmLib::inquiries::inquiry<bool> inquiry_isReadySleep;
			boost::signals2::signal<void(bool)> signal_exeSleepMode;
			boost::signals2::signal<void(bool)> signal_exeNormalMode;
			hmLib::inquiries::inquiry<bool> inquiry_isNormalMode;

			//IO�R���g���[��

			//Com�R���g���[��
			hmLib::inquiries::inquiry<clock::time_point> inquiry_getLastRecvTime;
			boost::signals2::signal<void(void)> signal_clearSendBuf;

			//Operator�R���g���[��
			boost::signals2::signal<void(bool)> signal_setIsSendActive;

			//GateSwitcher�R���g���[��
			boost::signals2::signal<void(void)> signal_openFoma;
			boost::signals2::signal<void(void)> signal_closeFoma;

		private:
			bool IsStart;		//���݃V�X�e���ғ������H
			bool IsReqStart;	//���݃V�X�e���ɑ΂���v���͉ғ���Ԃ��H

		private:
			std::ofstream flog;
				
		public:
			cAutoLogger()
				: ObserveStartPoint(hmr::date(2000, 1, 1, 0, 0, 0, 0).to_time())
				, ObserveInteraval(std::chrono::minutes(60))
				, PreObserveLength(std::chrono::minutes(10))
				, HomuraTimeRate(1.0)
				, Status(status::SetSleep)
				, IsStart(false)
				, IsReqStart(false) {
			}
			//�������O�J�n�v��
			bool start() {
				IsReqStart=true;
				//���ۂ̃X�^�[�g��operator()�ōs����

				return false;
			}
			//�������O�I���v��
			void stop() {
				IsReqStart=false;
				//���ۂ̃X�g�b�v��operator()�ōs����
			}
			//�������O�v�������H
			bool is_start()const {
				return IsReqStart;
			}
			//�������O���ғ����Ă���Œ����H
			bool is_start_autolog()const {
				return IsStart;
			}
			//�������O�@�\�쓮�֐�
			void operator()(void) {
				//�V�K���s�v�����o�Ă����ꍇ
				if(IsReqStart && !IsStart) {
					IsStart=true;

					// Pass���w�肵�Ă��܂�				
					hmr::date Date;
					Date.from_time(hmr::clock::now());
					std::string Path = (boost::format("autolog_%02d%02d%02d_%02d%02d%02d")%(Date.Year%100)%Date.Month%Date.Day%Date.Hour%Date.Min%Date.Sec).str();

					boost::filesystem::create_directory(boost::filesystem::current_path()/Path);
				
//					std::string Path="Autolog";
					flog.open(Path+"\\"+"autolog.txt",std::ios_base::out|std::ios_base::app);
					
					flog<<hmr::time_to_ymdhms(hmr::clock::now())<<" "<<"start autolog"<<std::endl;

					for(auto Ptr:Clients){
						Ptr->start(Path+"\\");
					}


					setNextObserveTime();
					flog<<hmr::time_to_ymdhms(hmr::clock::now())<<" "<<"NextAwake="<<hmr::time_to_ymdhms(NextAwake)<<std::endl;
					flog<<hmr::time_to_ymdhms(hmr::clock::now())<<" "<<"NextObserve="<<hmr::time_to_ymdhms(NextObserve)<<std::endl;

					//���̋N�����Ԃ܂�5���ȏ゠���
					if(hmr::clock::now()+get_duration<duration_tag::awake_wait_limit>()>NextAwake) {
						setup_observe_forAllClients();
						Status=status::Observe;
						flog<<hmr::time_to_ymdhms(hmr::clock::now())<<" "<<"status Observe"<<std::endl;
					} else {
						Status=status::SetSleep;
						flog<<hmr::time_to_ymdhms(hmr::clock::now())<<" "<<"status SetSleep"<<std::endl;
					}
				}
				
				//status change
				while(IsStart) {
					//�������O��~�v�����o�Ă����ꍇ
					if(!IsReqStart && (Status==status::SetSleep || Status==status::Observe)) {
						IsStart=false;
						
						for(auto Ptr:Clients) {
							Ptr->stop();
						}

						flog<<hmr::time_to_ymd(hmr::clock::now())<<" "<<hmr::time_to_hms(hmr::clock::now())<<" "<<"stop autolog"<<std::endl;
						flog.close();

						return;
					}

					switch(Status) {
					case status::SetSleep:
						//���ł�Sleep�ւ̑J�ڏ����������Ă���ꍇ
						if(inquiry_isReadySleep()) {
							//ReqSleep�ɂ��Ă�蒼��
							Status=status::ReqSleep;
							flog<<hmr::time_to_ymdhms(hmr::clock::now())<<" "<<"status ReqSleep"<<std::endl;
							continue;
						}
						//�R�}���h�������Ȃ�
						if(NextCommand < hmr::clock::now()) {
							auto SleepTime=getSleepTime();
	
							signal_setSleepMode(true, SleepTime.first, SleepTime.second);
							flog<<hmr::time_to_ymdhms(hmr::clock::now())<<" "<<"signal setSleepMode(true,"<<SleepTime.first<<","<<SleepTime.second<<")"<<std::endl;

							NextCommand = hmr::clock::now()+get_duration<duration_tag::command_delay>();
						}
						return;
					case status::ReqSleep:
						//�ʐM�r�₩��10�b�ȏソ���Ă����
						if(clock::now()-inquiry_getLastRecvTime()>get_duration<duration_tag::com_disconnect_limit>()) {
							signal_setIsSendActive(false);
							signal_clearSendBuf();
							signal_closeFoma();
							flog<<hmr::time_to_ymdhms(hmr::clock::now())<<" "<<"signal setIsSendActive(false), clearSendBuf(), closeFoma()"<<std::endl;

							Status=status::Sleep;
							flog<<hmr::time_to_ymdhms(hmr::clock::now())<<" "<<"status Sleep"<<std::endl;
							continue;
						}
						//�R�}���h�������Ȃ�
						if(NextCommand < hmr::clock::now()) {
							signal_exeSleepMode(true);
							flog<<hmr::time_to_ymdhms(hmr::clock::now())<<" "<<"signal exeSleepMode(true)"<<std::endl;
							NextCommand = hmr::clock::now()+get_duration<duration_tag::command_delay>();
						}
						return;
					case status::Sleep:
						//�N�����Ԃ��߂��Ă����
						if(NextAwake < hmr::clock::now()) {
							Status=status::Call;
							flog<<hmr::time_to_ymdhms(hmr::clock::now())<<" "<<"status Call"<<std::endl;
							continue;
						}
						return;
					case status::Call:
						//�ʐM��5�b�ȓ��ɑ��݂����
						if(clock::now()-inquiry_getLastRecvTime()<get_duration<duration_tag::com_connect_limit>()) {
							signal_setIsSendActive(true);
							flog<<hmr::time_to_ymdhms(hmr::clock::now())<<" "<<"signal setIsSendActive(true)"<<std::endl;
							Status=status::ReqNormal;
							flog<<hmr::time_to_ymdhms(hmr::clock::now())<<" "<<"status ReqNormal"<<std::endl;
							continue;
						}
						//�R�[���������Ȃ�
						if(NextCall < hmr::clock::now()) {
							signal_openFoma();
							NextCall = hmr::clock::now()+get_duration<duration_tag::call_delay>();
							flog<<hmr::time_to_ymdhms(hmr::clock::now())<<" "<<"signal openFoma()"<<std::endl;
						}
						return;
					case status::ReqNormal:
						//���[�h���m�[�}���ɂȂ��Ă����
						if(inquiry_isNormalMode()) {
							//�ʒm
							setup_observe_forAllClients();
							Status=status::Observe;
							flog<<hmr::time_to_ymdhms(hmr::clock::now())<<" "<<"status PreObserve"<<std::endl;

							continue;
						}
						//�R�}���h�������Ȃ�
						if(NextCommand < hmr::clock::now()) {
							signal_exeNormalMode(true);
							flog<<hmr::time_to_ymdhms(hmr::clock::now())<<" "<<"signal exeNormalMode(true)"<<std::endl;
							NextCommand = hmr::clock::now()+get_duration<duration_tag::command_delay>();
						}
						return;
					case status::Observe:
						//���ׂĂ�Client�����b�N���������Ă���� ���邢�́A�ϑ����E�𒴂��Ă����
						if(!observe_forAllClients(NextObserve) || NextObserve+get_duration<duration_tag::observe_limit>()<clock::now()) {
							setNextObserveTime();
							flog<<hmr::time_to_ymdhms(hmr::clock::now())<<" "<<"NextAwake="<<hmr::time_to_ymdhms(NextAwake)<<std::endl;
							flog<<hmr::time_to_ymdhms(hmr::clock::now())<<" "<<"NextObserve="<<hmr::time_to_ymdhms(NextObserve)<<std::endl;
							Status=status::SetSleep;
							flog<<hmr::time_to_ymdhms(hmr::clock::now())<<" "<<"status SetSleep"<<std::endl;
							continue;
						}
						return;
					}
				}
			}
		};
	}
}
#
#endif
