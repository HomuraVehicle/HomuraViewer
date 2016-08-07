#ifndef HMR_VIEWER_MESSAGE_MESSAGEDRIVER_INC
#define HMR_VIEWER_MESSAGE_MESSAGEDRIVER_INC 103
#
/*===hmrMessageDriver===
Mesasge��Com�𐧌䂵�đ���M�^�C�~���O���R���g���[������N���X

hmrMessageDriver:v1_03/130525 amby  
	Interval �Ɓ@TimeOut �̍l���^��ύX�A����ɔ����āA�������Ȃ���

	�ȉ��ɐ���
�@�@�@ VMC��signal�ɂ���āA�p�P�b�g�̎�M�I����m�点������B
�@�@�@ ���̎�M�I���������L�^���Ă����āA

�@�@�@�S��d�ɂ�����
�@�@�@�@�Ƃ肠�����ATimeInterval�ő��M���āA���ł���M���s���B�@�@�@�@
�@�@�@�@�ŐV�̎�M�I����������TimeOut���߂��Ă��܂����Ƃ��ɁAVMC�̎�M�t�F�[�Y�������I������B

�@�@�@����d�ɂ�����
�@�@�@�@�ŐV�̎�M�I����������ATimeInterval�҂��đ��M����B(Interval��0�ɂ����sync�Ƃ�������)
�@�@�@�@�ŐV�̎�M�I����������TimeOut���߂��Ă��܂����Ƃ��ɁAVMC�̎�M�t�F�[�Y�������I������B

hmrMessageDriver:v1_02/130519 amby
	sendMode �ɂ��Ă͎g���Ă��Ȃ�
	�S��d�A����d�A�^�C���A�E�g�Ƃ��͂��ׂĂ����ŊǗ����Ă���B

	�ŏI�I�Ƀf�[�^���������ĂȂ������ŁATimeOut �𔻒f���Ă���B
	�����܂Ńf�[�^�����Ă��Ȃ���Α��M��������x�s���Ƃ�������ɂȂ��Ă���

	���M����t���O��ǉ�

	����d���[�h�ɂ��ďC������K�v����A����v�m�F
	VMC�̃��Z�b�g�̃^�C�~���O�ɂ��Ă����l�@

hmrMessageDriver:v1_01/130511 amby
	Interval �𐧌䂷�邽�߂Ɂ@
	cFullRecvTimeSendMessageDriver��
	signal slot ��ǉ�����

	IO��sendMode �ɂ��ā@���ꂪ�ӔC�������H�H
	OP�����ׂ��Ȃ̂�����ǁA���M�����I���������m��悤���Ȃ��B
	VMC����Signal�Ƃ��Œm�����Ƃ��Ă�邱�Ƃ͂ł��邪�A���ꂪ�d�v�Ȃ̂��ǂ����H�H

	���ǁAOP�Ő��䂵�Ă��܂��΁ASendMode�𐧌䂷�郁���b�g�͂��܂�Ȃ��̂ł́H�Ɗ�����
	����āASendMode�ɂ��Ă͕ۗ��ɂ��Ă���.


hmrMessageDriver:v1_00/130310 hmito
	cFullRecvOneSendMessageDriver��ǉ�

*/
#include <hmLib/signals.hpp>
#include <hmLib/inquiries/inquiry.hpp>
#include <boost/signals2.hpp>
#include <HomuraViewer/Message/ItfMessage.hpp>
#include <HomuraViewer/chrono.hpp>
#include"ItfCom.hpp"

namespace hmr{
	namespace viewer{
		namespace message{
			class cFullRecvOneSendMessageDriver{
				using datum = message::datum;
			private:
				itfMessageHost* pMessage;
				itfCom* pCom;
			public:
				cFullRecvOneSendMessageDriver(itfMessageHost* pMessage_, itfCom* pCom_)
					:pMessage(pMessage_)
					, pCom(pCom_){}
				void operator()(void){
					while(true){
						if(pCom->empty()){
							if(!pCom->can_sync())break;
							pCom->sync();
							pMessage->setup_listen();
							continue;
						}
						pMessage->listen(pCom->recv());
					}

					pMessage->setup_talk();
					while(true){
						if(pCom->full())break;

						datum Dat;
						if(pMessage->talk(Dat))break;

						pCom->send(Dat);
					}
					if(pCom->can_flush())pCom->flush();
				}
			};
			class cFullRecvTimeSendMessageDriver{
				using datum = message::datum;
			private:
				itfMessageHost* pMessage;
				itfCom* pCom;

				//�@�S��d�A����d�̑I���t���O
				bool IsFullDuplex; // �S��d�Ȃ�true ����d�Ȃ�false
								   // IO �ɂ���ĎQ�Ƃ����

								   //  ���M���̂��s�����ǂ����̃t���O
				bool IsSendActiveFlag; // ���M�����ԂȂ� true    
									   // ���M���Ȃ���ԂȂ� false 

									   // �S��d, ����d�ʐM����̂��߂̕ϐ��ʐM����̂��߂̕ϐ�
				clock::duration Interval;// ���M�Ԋu	
										 //bool SyncFlag;// Sync���邩�ǂ����̃t���O
				clock::duration TimeOut;// Sync���ɕԐM�����Ȃ������Ƃ��̍đ��܂ł̎��ԊԊu
										// Sync ���Ă��Ȃ���΁A���M�̎��ԊԊu�ɂȂ�

										// ����d�ʐM�̂��߂̃��[�h����t���O
				bool RecvMode;

				bool RecevedFlag;// ����Ɏ�M�������ǂ����̃t���O  
				bool IsTimeoutedFlag;
				bool IsSendedFlag;

				// �O���M���M��������
				clock::time_point LastRecvTime;
				clock::time_point LastSendTime;

			private:
				// slot
				hmLib::signals::unique_connections connections;
			public:
				//signal 
				//boost::signals2::signal<void(bool)> signal_require_setSendMode;
				// VMC�֘A signal
				boost::signals2::signal<void(void)> signal_require_VMC_force_end_recv;
				boost::signals2::signal<void(void)> signal_require_VMC_force_end_send;

				boost::signals2::signal<void(clock::time_point)> signal_inform_TimeOut;
				boost::signals2::signal<void(clock::time_point)> signal_inform_Sended;
				boost::signals2::signal<void(clock::time_point)> signal_inform_Received;


				// slot 
				void setSendInterval(clock::duration Interval_){
					Interval = Interval_;
				}
				// slot 
				void setTimeOut(clock::duration TimeOut_){
					TimeOut = TimeOut_;
				}
				// slot 
				void setIsSendActiveFlag(bool IsSendActive_){
					IsSendActiveFlag = IsSendActive_;
				}
				// slot 
				void setIsFullDuplex(bool IsFullDuplex_){
					IsFullDuplex = IsFullDuplex_;
				}


				// inquire �n��̒�`
				hmLib::inquiries::inquiry<bool> IsFinishedRecvInq;


			public:
				// connection
				void slot_setSendInterval(boost::signals2::signal<void(clock::duration)> &sig_){
					connections(sig_.connect([&](clock::duration v)->void{this->setSendInterval(v); }));
				}
				void slot_setTimeOut(boost::signals2::signal<void(clock::duration)> &sig_){
					connections(sig_.connect([&](clock::duration v)->void{this->setTimeOut(v); }));
				}
				void slot_setIsSendActiveFlag(boost::signals2::signal<void(bool)> &sig_){
					connections(sig_.connect([&](bool v)->void{this->setIsSendActiveFlag(v); }));
				}
				void slot_setIsFullDuplex(boost::signals2::signal<void(bool)> &sig_){
					connections(sig_.connect([&](bool v)->void{this->setIsFullDuplex(v); }));
				}


				// Contact�֐�
				void contact_Interval(hmLib::inquiries::inquiry<clock::duration>& IntervalInq_){
					hmLib::inquiries::connect(IntervalInq_, Interval);
				}
				void contact_TimeOut(hmLib::inquiries::inquiry<clock::duration>& TimeOutInq_){
					hmLib::inquiries::connect(TimeOutInq_, TimeOut);
				}
				void contact_IsFullDuplex(hmLib::inquiries::inquiry<bool>& IsFullDuplexInq_){
					hmLib::inquiries::connect(IsFullDuplexInq_, IsFullDuplex);
				}
				void contact_IsSendActive(hmLib::inquiries::inquiry<bool>& IsSendActiveInq_){
					hmLib::inquiries::connect(IsSendActiveInq_, IsSendActiveFlag);
				}

			public:
				cFullRecvTimeSendMessageDriver(itfMessageHost* pMessage_, itfCom* pCom_, clock::duration Interval_, clock::duration TimeOut_, bool IsFullDuplex_, bool SyncFlag_)
					:pMessage(pMessage_)
					, pCom(pCom_)
					, IsFullDuplex(IsFullDuplex_)
					, Interval(Interval_)
					//,SyncFlag(SyncFlag_)
					, TimeOut(TimeOut_)
					, RecevedFlag(false)
					, IsSendActiveFlag(false)
					, RecvMode(true){
					LastRecvTime = clock::now();
					LastSendTime = clock::now();
				}
				void operator()(void){

					// �S��d���[�h�̏ꍇ�A��M�ł������S�Ď�M����莞�ԊԊu�ő��M
					if(IsFullDuplex){
						// ��M�t�F�[�Y
						while(true){
							if(pCom->empty()){
								if(!pCom->can_sync())break;
								pCom->sync();
								pMessage->setup_listen();
								// packet��M����
								LastRecvTime = clock::now();
								signal_inform_Received(LastRecvTime);

								continue;
							}
							pMessage->listen(pCom->recv());
						}

						// ���M�t�F�[�Y
						if(IsSendActiveFlag && (LastSendTime + Interval < clock::now())){
							pMessage->setup_talk();
							while(true){
								if(pCom->full())break;

								datum Dat;
								if(pMessage->talk(Dat))break;

								pCom->send(Dat);
							}
							if(pCom->can_flush())pCom->flush();

							// ���M���I���������Ƃ𔭐M
							LastSendTime = clock::now();
							signal_inform_Sended(LastSendTime);

						}

					}
					// ����d���[�ǂ̏ꍇ
					else{
						// ��M�t�F�[�Y
						if(RecvMode){
							while(true){
								// ��M���I�����Ă���̂����m�F
								if(IsFinishedRecvInq() && pCom->empty() && !pCom->can_sync()){
									RecvMode = false;
									// ��M���I���������Ƃ�ʒm
									LastRecvTime = clock::now();
									signal_inform_Received(LastRecvTime);
								}
								// ��M
								if(pCom->empty()){
									if(!pCom->can_sync())break;
									pCom->sync();
									pMessage->setup_listen();
									continue;
								}
								pMessage->listen(pCom->recv());

							}
						}

						// ���M�t�F�[�Y
						if(IsSendActiveFlag){
							// ��M���[�h�͏I�����Ă���A���C���^�[�o���������Ă�ꍇ
							if(!RecvMode && ((clock::now() - LastSendTime) > Interval)){
								//���M�����J�n
								pMessage->setup_talk();
								// ���M�f�[�^����
								while(true){
									if(!pCom->full()){
										datum Dat;
										if(pMessage->talk(Dat))break;
										pCom->send(Dat);
									}
								}
								// ���M
								if(pCom->can_flush()){
									pCom->flush();
									RecvMode = true;

									// ���M���I���������Ƃ�ʒm
									LastSendTime = clock::now();
									signal_inform_Sended(LastSendTime);
								}
							}
							// ��M�f�[�^��TimeOut���ŗ��Ȃ������ꍇ
							//  VMC�̃��Z�b�g�������Ă���A
							//  �d���Ȃ����������񑗂��Ă݂�
							else if(clock::now() - LastSendTime > TimeOut){
								// VMC ���Z�b�g�̃R�}���h�𑗂�@
								signal_require_VMC_force_end_recv();
								signal_require_VMC_force_end_send();

								// Time Out �������Ƃ�ʒm
								signal_inform_TimeOut(clock::now());

								//���M�����J�n
								pMessage->setup_talk();
								// ���M�f�[�^����
								while(true){
									if(!pCom->full()){
										datum Dat;
										if(pMessage->talk(Dat))break;
										pCom->send(Dat);
									}
								}
								// ���M
								if(pCom->can_flush()){
									pCom->flush();
									RecvMode = true;

									// ���M���I���������Ƃ�ʒm
									LastSendTime = clock::now();
									signal_inform_Sended(LastSendTime);
								}
							}
						}
					}


				}
			};
			class cFHDxMessageDriver{
				using datum = message::datum;
			private:
				itfCom* pCom;
				itfMessageHost* pMessage;
				//�O���M��������
				clock::time_point LastRecvTime;
				//�O�񑗐M��������
				clock::time_point LastSendTime;
				//�p�P�b�g��҂��Ă��邩�ǂ���
				bool IsWaitPacket;
			private:
				//���M���̂��s�����ǂ����̃t���O
				bool IsSendActiveFlag;
				//�S��d�A����d�̑I���t���O
				bool IsFullDuplex;
				//����d/�S��d��p�A���M�Ԋu
				clock::duration Interval;
				//����d��p�A��M�҂���������߂čđ�����܂ł̎���
				clock::duration TimeOut;
			private:
				void setIsSendActiveFlag(bool IsSendActive_){ IsSendActiveFlag = IsSendActive_; }
				void setIsFullDuplex(bool IsFullDuplex_){ IsFullDuplex = IsFullDuplex_; }
				void setInterval(clock::duration Interval_){ Interval = Interval_; }
				void setTimeOut(clock::duration TimeOut_){ TimeOut = TimeOut_; }
			private:
				hmLib::signals::unique_connections connections;
			public:
				// inquire �n��̒�`
				hmLib::inquiries::inquiry<bool> IsFinishedRecvInq;
				// VMC�֘A signal
				boost::signals2::signal<void(void)> signal_require_VMC_force_end_recv;
				boost::signals2::signal<void(void)> signal_require_VMC_force_end_send;
				boost::signals2::signal<void(clock::time_point)> signal_informTimeOut;
				boost::signals2::signal<void(clock::time_point)> signal_informFinSend;
				boost::signals2::signal<void(clock::time_point)> signal_informFinReceive;
			public:
				// connection
				void slot_setIsSendActiveFlag(boost::signals2::signal<void(bool)> &sig_){
					connections(sig_.connect([&](bool v)->void{this->setIsSendActiveFlag(v); }));
				}
				void slot_setIsFullDuplex(boost::signals2::signal<void(bool)> &sig_){
					connections(sig_.connect([&](bool v)->void{this->setIsFullDuplex(v); }));
				}
				void slot_setInterval(boost::signals2::signal<void(clock::duration)> &sig_){
					connections(sig_.connect([&](clock::duration v)->void{this->setInterval(v); }));
				}
				void slot_setTimeOut(boost::signals2::signal<void(clock::duration)> &sig_){
					connections(sig_.connect([&](clock::duration v)->void{this->setTimeOut(v); }));
				}

				// Contact�֐�
				void contactIsSendActive(hmLib::inquiries::inquiry<bool>& IsSendActiveInq_){
					hmLib::inquiries::connect(IsSendActiveInq_, IsSendActiveFlag);
				}
				void contactIsFullDuplex(hmLib::inquiries::inquiry<bool>& IsFullDuplexInq_){
					hmLib::inquiries::connect(IsFullDuplexInq_, IsFullDuplex);
				}
				void contactInterval(hmLib::inquiries::inquiry<clock::duration>& IntervalInq_){
					hmLib::inquiries::connect(IntervalInq_, Interval);
				}
				void contactTimeOut(hmLib::inquiries::inquiry<clock::duration>& TimeOutInq_){
					hmLib::inquiries::connect(TimeOutInq_, TimeOut);
				}

			public:
				cFHDxMessageDriver(itfMessageHost* pMessage_, itfCom* pCom_, bool IsFullDuplex_, clock::duration Interval_, clock::duration TimeOut_)
					:pMessage(pMessage_)
					, pCom(pCom_)
					, IsSendActiveFlag(false)
					, IsFullDuplex(IsFullDuplex_)
					, Interval(Interval_)
					, TimeOut(TimeOut_){
					LastRecvTime = clock::now();
					LastSendTime = clock::now();
				}
				void operator()(void){
					//��M����
					while(true){
						//��ɂȂ��Ă��Async�ł������sync���ēǂݍ��݂𑱂���
						if(pCom->empty()){
							if(!pCom->can_sync()){
								break;
							}

							pCom->sync();
							pMessage->setup_listen();
							// packet��M����
							LastRecvTime = clock::now();
							signal_informFinReceive(LastRecvTime);

							continue;
						}
						pMessage->listen(pCom->recv());
						if(IsFinishedRecvInq())IsWaitPacket = false;
					}

					// ���M�\�łȂ���΁A�I��
					if(!IsSendActiveFlag || pCom->full())return;

					// �S��d���[�h�̏ꍇ
					if(IsFullDuplex){
						//��莞�Ԍo�߂��Ă��Ȃ���΁A�I��
						if(LastSendTime + Interval > clock::now())return;
					}//����d���[�h�̏ꍇ
					else{
						//��莞�Ԍo�߂��Ă��Ȃ���΁A�I��
						if(LastSendTime + Interval > clock::now())return;
						//�p�P�b�g�҂����ŁATimeOut���Ԍo�߂��Ă��Ȃ���΁A�I��
						if(IsWaitPacket && (LastSendTime + TimeOut > clock::now()))return;
					}

					//���M����
					pMessage->setup_talk();
					while(true){
						if(pCom->full())break;

						datum Dat;
						if(pMessage->talk(Dat))break;

						pCom->send(Dat);
					}
					if(pCom->can_flush())pCom->flush();

					IsWaitPacket = true;

					// ���M���I���������Ƃ𔭐M
					LastSendTime = clock::now();
					signal_informFinSend(LastSendTime);
				}
			};
		}
	}

}
#
#endif
