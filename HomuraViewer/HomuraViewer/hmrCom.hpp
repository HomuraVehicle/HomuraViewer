#ifndef HMR_COM_INC
#define HMR_COM_INC 113
#
/*===Sample for vmc hmrCom ===

hmrCom:v1_13/130615 hmito
	packet/datum�̑���M���ɌŗLno��U��悤�ɕύX
		���Ԃł́A������datum�����ꎞ�Ԃ������ꍇ�����������߁B
hmrCom v1_12/130525 amby
	��M�������ԁA��M�����ǂ����̏�ԃt���O��inquiry������
hmrCom:v1_11/130324 amby
	�f�[�^�̃o�b�t�@������啝�ɕύX
	�P�A����M�o�b�t�@�� Deque<Datum>�@�Ƃ����`�ŕێ����āA�p�P�b�g�̋��E�̓_�~�[�f�[�^(ID = 0x00)�ɂ���ċ��
	�Q�ASigna�@slot �́Async, flush �֐��ɂĎ�������
	VMC���Ή��ς�
hmrCom v1_11/130410 hmIto
	����M����packet�擾�p��inquiry������
hmrCom v1_10/130310 amby
	�f�[�^�̎󂯓n���^���APacket ����@Datum �ɕύX
	����ɂ�����N���X���ύX���Ă�
hmrCom v1_02/130302 amby
	cCom�̂u�l�b�p�֐�������
	hmrOperator �p�̃C���^�[�t�F�[�X����������
	abtCom �ɂ͑Ή��ł��Ă��Ȃ�
hmrCom v1_01/130227 hmIto
	VMC�쐬�̎��̂��錾����Ă����̂��폜
	����M�p�o�b�t�@�Ȃǂ�cCom�����o�ϐ��𐮗�
	cCom��Signal/Slot���Ƃ肠��������
hmrCom v1_00/130223 amby
	VMC�֘A�֐��̎�M�܂��𐻍�B
*/

#include<hmLib/config_vc.h>
#include<deque>
#include<boost/signals2/signal.hpp>
#include<hmLib/signals.hpp>
#include<hmLib/inquiries/inquiry.hpp>
#include<hmLib/inquiries/unique_connections.hpp>
#include<hmLib/exceptions.hpp>
#include <HomuraViewer/VMC1.h>
#include <HomuraViewer/chrono.hpp>
#include"hmrItfCom.hpp"

namespace hmr{
	namespace viewer{
		class cCom :public itfCom{
			using datum = message::datum;
			using packet = message::packet;
		private:
			typedef std::deque<datum> datum_que;
			static datum getEndDatum(bool Error_ = false){
				datum Datum;
				Datum.No = 0;
				Datum.ID = 0x00;
				Datum.ErrFlag = Error_;
				Datum.Time = clock::now();
				return Datum;
			}
			static bool isEndDatum(const datum& Datum_){ return Datum_.ID == 0x00; }
		private://Debug
				//����M�̐�����s���N���X
			template<unsigned int VMC1_Connection_Ch_>
			class VMCConnection{
			private:
				static cCom* pCom;
				static datum TemporalSendDatum;
				static datum TemporalRecvDatum;
				static int SendedDatumCnt; // �������f�[�^���J�E���g
			private:
				//PacStrt����M���悤�Ƃ��Ă悢���ǂ���
				static hmLib_boolian can_iniRecvPac(void){
					return true;
				}
				//PacStrt��M�������ɌĂ΂��  
				static void iniRecvPac(){
					//*DEBUG*/printfDx("<");
					pCom->IsFinishedRecv = false;
				}
				//PacTrmn��M�������ɌĂ΂��@�����̓G���[�̗L��
				static void finRecvPac(unsigned char Err){
					//*DEBUG*/printfDx("/%02x>\n",static_cast<unsigned int>(Err));
					// 1 Packet ��M�I��
					// ���ʂ��邽�߂Ƀo�b�t�@�Ƀ_�~�[�f�[�^��}��
					pCom->RecvBuf.push_back(cCom::getEndDatum(Err != 0));
					pCom->LastRecvPacTime = clock::now();
					pCom->IsFinishedRecv = true;
					//				printfDx("\n");

				}
				//iniRecvDat�����s���ėǂ����̊m�F�ɌĂ΂��
				static hmLib_boolian can_iniRecvDat(void){
					return true;
				}
				//Dat��M�J�n���ɌĂ΂�� �����͎�M����f�[�^�T�C�Y
				static void iniRecvDat(vmc1_did_t ID, vmc1_dsize_t Size){
					//*DEBUG*/printfDx("[%c:%4d",ID,Size);
					// ������
					TemporalRecvDatum.No = datum::no_gen::no();
					TemporalRecvDatum.ID = ID;
					TemporalRecvDatum.Size = Size;
					TemporalRecvDatum.Str.clear();
					TemporalRecvDatum.Time = clock::now();
				}
				//Dat��M�I�����ɌĂ΂�� �����̓G���[�̗L��
				static void finRecvDat(unsigned char Err){
					//*DEBUG*/printfDx("/%02x]",static_cast<unsigned int>(Err));
					// Err �̑��
					TemporalRecvDatum.ErrFlag = (Err != 0);

					// �o�b�t�@�ɂԂ�����
					pCom->RecvBuf.push_back(TemporalRecvDatum);
					//				printfDx("%c",TemporalRecvDatum.ID);
				}
				//recv�����s���Ă悢���̊m�F�ɌĂ΂��
				static hmLib_boolian can_recv(void){
					return true;
				}
				//Dat�̒��g��M���ɌĂ΂��@��������M�����f�[�^
				static void recv(unsigned char c){
					TemporalRecvDatum.Str.push_back(c);
					//				clock::time_point LastRecvDatTime;
					pCom->LastRecvDatTime = clock::now();
				}
				//PacStrt���M���J�n���Ă悢���ǂ����H
				static hmLib_boolian can_iniSendPac(void){
					// Packet ����łȂ����Strt���M
					if(!pCom->SendBuf.empty()){
						return true;
					} else{
						return false;
					}
				}
				//RacStrt���M�������ɌĂ΂��
				static void iniSendPac(void){
					// �������Ȃ�
					//pCom->PacSendedFlag = false;
				}
				//PacTrmn���M�������ɌĂ΂��@�����̓G���[�̗L��
				static void finSendPac(unsigned char Err){
					//�����̃_�~�[�f�[�^�𗎂Ƃ�
					if(pCom->SendBuf.empty() || !cCom::isEndDatum(pCom->SendBuf.front()))return;

					pCom->SendBuf.pop_front();
					pCom->LastSendPacTime = clock::now();
					//pCom->PacSendedFlag = true;
				}
				//existSendDat�֐������s�\����₢���킹��
				static hmLib_boolian can_existSendDat(void){
					// ���̃f�[�^���p�ӂ���Ă��Ȃ���΁A�Ƃ肠�����҂��Ă��炤
					if(pCom->SendBuf.empty()) return false;
					else return true;
				}
				//���M���K�v�ȃf�[�^�̗L���̊m�F
				// �Ȃ���΃^�[�~�l�[������
				static hmLib_boolian existSendDat(void){
					// �O�̂���buff�`�F�b�N
					hmLib_assert(!pCom->SendBuf.empty(), hmLib::io_exceptions::end_of_file, " VMCConnection::hmLib_boolian existSendDat -> SendBuf is empty !!");

					//���ꂪ�Ō�̃_�~�[�f�[�^�łȂ������`�F�b�N
					if(cCom::isEndDatum(pCom->SendBuf.front())){
						// �Ō�̃_�~�[�f�[�^�Ȃ炨���܂�
						return false;
					} else return true;
				}
				//iniSendDat�����s���ėǂ����̊m�F�ɌĂ΂��
				static hmLib_boolian can_iniSendDat(void){
					return true;
				}
				//Dat���M�m�莞�ɌĂ΂��@�T�C�Y��߂�
				// Dat���M1�Z�b�g�ɂ��n�߂�1��Ă΂��
				static void iniSendDat(vmc1_did_t* pID, vmc1_dsize_t* pSize){
					// Data �J�E���g�̏�����
					SendedDatumCnt = 0;

					//�f�[�^�擾
					TemporalSendDatum = pCom->SendBuf.front();
					pCom->SendBuf.pop_front();

					// Data���ԋp
					*pID = TemporalSendDatum.ID;
					*pSize = TemporalSendDatum.Size;
				}
				//Dat���M�I�����ɌĂ΂��@�����̓G���[�̗L��
				static void finSendDat(unsigned char Err){
					SendedDatumCnt = 0;
				}
				//send�����s���Ă悢���̊m�F�ɌĂ΂��
				static hmLib_boolian can_send(void){
					return true;
				}
				//Dat�̒��g���M���ɌĂ΂��
				static unsigned char send(void){
					// �f�[�^���M
					return TemporalSendDatum.Str[SendedDatumCnt++];
				}
			public:
				static VMC1* create_vmc1(cCom* pCom_){
					pCom = pCom_;
					return vmc1_create
					(can_iniRecvPac, iniRecvPac, finRecvPac
						, can_iniRecvDat, iniRecvDat, finRecvDat
						, can_recv, recv
						, can_iniSendPac, iniSendPac, finSendPac
						, can_existSendDat, existSendDat
						, can_iniSendDat, iniSendDat, finSendDat
						, can_send, send);
				}
			};
		public:
			//vmc�̎��Ԃ��m�ۂ��ēn���N���X
			template<unsigned int VMC_Connection_Ch_>
			class VMC1Creater{
			private:
				cCom* pCom;
			public:
				VMC1Creater(cCom* pCom_) :pCom(pCom_){}
				VMC1* operator()(void){ return VMCConnection<VMC_Connection_Ch_>::create_vmc1(pCom); }
			};
		private:
			//���ݎ�M���̃f�[�^/�p�P�b�g�̋L�^
			packet RecvLog;
			//��M��datum�p�L���[
			datum_que RecvBuf;
			//���ݑ��M���̃f�[�^/�p�P�b�g�̋L�^
			packet SendLog;
			//���M��datum�p�L���[
			datum_que SendBuf;

			//��M�I�����̎��Ԃ��L�^
			// �ŐV��Pac��M�I������
			clock::time_point LastRecvPacTime;
			// �ŐV��Data��M�I������
			clock::time_point LastRecvDatTime;

			// ���ݎ�M�����ۂ�
			bool IsFinishedRecv;

			//���M�I�����Ԃ̋L�^
			clock::time_point LastSendPacTime;

		public:
			// Constructor
			cCom() :itfCom(), IsFinishedRecv(false){
				LastRecvDatTime = clock::now();
				LastSendPacTime = clock::now();
				LastRecvPacTime = clock::now();
			}

		private:
			/*
			// Packet�̑��M���������t���O
			bool PacSendedFlag;
			// �Ō�Ɏ�M��������
			clock::time_point ReceivedTime;
			public:
			// Packet�̑��M���������t���O
			bool isPacSended(void){
			return PacSendedFlag;
			}
			void resetPacSendedFlag(void){
			PacSendedFlag = false;
			}
			*/

		private://signals/inquiries
			hmLib::inquiries::unique_connections InquiryConnections;
		public://signals/inquiry
			   //�p�P�b�g����M�������āARecvBuf�ɓ����^�C�~���O�ŌĂ΂��
			boost::signals2::signal<void(const packet& Pac_)> signal_finRecvPacket;
			//�p�P�b�g�𑗐M�������āA���̑��M�f�[�^��SendBuf����Ă΂��^�C�~���O�ŌĂ΂��B
			boost::signals2::signal<void(const packet& Pac_)> signal_finSendPacket;
			//��M���̃��O���擾����
			void contactRecvLog(hmLib::inquiries::inquiry<const packet&>& Inq_){
				InquiryConnections(Inq_.connect([&]()->const packet&{return this->RecvLog; }));
			}
			//���M���̃��O���擾����
			void contactSendLog(hmLib::inquiries::inquiry<const packet&>& Inq_){
				InquiryConnections(Inq_.connect([&]()->const packet&{return this->SendLog; }));
			}
			//��M�������Ԃ��擾����
			void contactLastRecvPacTime(hmLib::inquiries::inquiry<clock::time_point>& Inq_){
				InquiryConnections(hmLib::inquiries::connect(Inq_, LastRecvPacTime));
			}
			//���M�������Ԃ��擾����
			void contactLastSendPacTime(hmLib::inquiries::inquiry<clock::time_point>& Inq_){
				InquiryConnections(hmLib::inquiries::connect(Inq_, LastSendPacTime));
			}
			//Data��M���Ԃ��m�F����
			void contactLastRecvDatTime(hmLib::inquiries::inquiry<clock::time_point>& Inq_){
				InquiryConnections(hmLib::inquiries::connect(Inq_, LastRecvDatTime));
			}
			//��M�I�����Ă邩�ǂ����̊m�F
			void contactIsFinishedRecv(hmLib::inquiries::inquiry<bool>& Inq_){
				InquiryConnections(hmLib::inquiries::connect(Inq_, IsFinishedRecv));
			}
			//��M���̃��O�̃T�C�Y���擾����
			void contactRecvLogSize(hmLib::inquiries::inquiry<unsigned int>& Inq_){
				InquiryConnections(hmLib::inquiries::connect(Inq_, [&](void)->unsigned int{return this->RecvLog.Data.size(); }));
			}
			//���M���̃��O�̃T�C�Y���擾����
			void contactSendLogSize(hmLib::inquiries::inquiry<unsigned int>& Inq_){
				InquiryConnections(hmLib::inquiries::connect(Inq_, [&](void)->unsigned int{return this->SendLog.Data.size(); }));
			}
			//��M�o�b�t�@�̃T�C�Y���擾����
			void contactRecvBufSize(hmLib::inquiries::inquiry<unsigned int>& Inq_){
				InquiryConnections(hmLib::inquiries::connect(Inq_, [&](void)->unsigned int{return this->RecvBuf.size(); }));
			}
			//���M�o�b�t�@�̃T�C�Y���擾����
			void contactSendBufSize(hmLib::inquiries::inquiry<unsigned int>& Inq_){
				InquiryConnections(hmLib::inquiries::connect(Inq_, [&](void)->unsigned int{return this->SendBuf.size(); }));
			}
		public://slots
			   // ��M�o�b�t�@�ɂ��܂������̂��Ƃ肠������������
			void clearRecvBuf(){ RecvBuf.clear(); }
			// ���M�o�b�t�@�ɂ��܂������̂��Ƃ肠������������
			void clearSendBuf(){ SendBuf.clear(); }

		private:
			//slot 
			hmLib::signals::unique_connections connections;
		public:
			//slot
			void slot_clearRecvBuf(boost::signals2::signal<void(void)> &sig_){
				connections(sig_.connect([&](void)->void{this->clearRecvBuf(); }));
			}
			void slot_clearSendBuf(boost::signals2::signal<void(void)> &sig_){
				connections(sig_.connect([&](void)->void{this->clearSendBuf(); }));
			}
		public:
			//��M�o�b�t�@���̃f�[�^�̑��݂̗L�����m�F����
			bool empty()override{
				//����ۂ��A���邢�͐擪��EndDatum�Ȃ��
				return RecvBuf.empty() || isEndDatum(RecvBuf.front());
			}
			//��M�o�b�t�@������f�[�^���擾����
			datum recv()override{
				hmLib_assert(!empty(), hmLib::io_exceptions::end_of_file, "hmrCom::RecvBuf is empty");

				//�o�b�t�@�̐擪���擾
				datum Datum = RecvBuf.front();
				//�o�b�t�@�̐擪��j��
				RecvBuf.pop_front();
				//���O�ɏ�������
				RecvLog.Data.push_back(Datum);

				return Datum;
			}
			//��M�X�g���[������X�V����K�v�����邩���m�F���� �ŐV�Ȃ�true
			bool can_sync()override{
				//��łȂ��A�擪��EndDatum�̂Ƃ��ɐ^
				return ((!RecvBuf.empty())) && isEndDatum(RecvBuf.front());
			}
			//��M�X�g���[�������M�o�b�t�@���X�V����
			void sync()override{
				hmLib_assert(can_sync(), hmLib::io_exceptions::end_of_file, "hmrCom::RecvBuf have already been syncd");

				//RecvLog���V�O�i�����M���ď�����
				RecvLog.ErrFlag = RecvBuf.front().ErrFlag;
				RecvLog.Time = RecvBuf.front().Time;
				RecvLog.No = packet::no_gen::no();
				signal_finRecvPacket(RecvLog);
				RecvLog.Data.clear();
				RecvLog.Time = clock::time_point::max();
				//�o�b�t�@�̐擪��EndDatum���폜����
				RecvBuf.pop_front();
			}
			//���M�o�b�t�@�̋󂫂��m�F����֐�
			bool full()override{ return SendBuf.size()>0 && isEndDatum(SendBuf.back()); }
			//���M�o�b�t�@�Ƀf�[�^��ǉ�����
			void send(const datum& Datum)override{
				datum Dat(Datum);
				Dat.No = datum::no_gen::no();
				//�o�b�t�@�̖����ɒǉ�����
				SendBuf.push_back(Dat);
				//���O�ɏ�������
				SendLog.Data.push_back(Dat);
			}
			//���M�X�g���[���ɑ��M�o�b�t�@�𔽉f����
			void flush()override{
				hmLib_assert(can_flush(), hmLib::io_exceptions::end_of_file, "hmrCom::SendBuf cannot flush");
				SendBuf.push_back(getEndDatum());

				//RecvLog���V�O�i�����M���ď�����
				SendLog.No = packet::no_gen::no();
				SendLog.ErrFlag = false;
				SendLog.Time = clock::now();
				signal_finSendPacket(SendLog);
				SendLog.Data.clear();
				RecvLog.Time = clock::time_point::max();
			}
			//���M�X�g���[���ɔ��f�\�����m�F����
			bool can_flush()override{ return true; }
		};
		template<unsigned int VMC1_Connection_Ch_>
		cCom* cCom::VMCConnection<VMC1_Connection_Ch_>::pCom;
		template<unsigned int VMC1_Connection_Ch_>
		message::datum cCom::VMCConnection<VMC1_Connection_Ch_>::TemporalSendDatum;
		template<unsigned int VMC1_Connection_Ch_>
		message::datum cCom::VMCConnection<VMC1_Connection_Ch_>::TemporalRecvDatum;
		template<unsigned int VMC1_Connection_Ch_>
		int cCom::VMCConnection<VMC1_Connection_Ch_>::SendedDatumCnt;
		//	template<unsigned int VMC1_Connection_Ch_>
		//	int cCom::VMCConnection<VMC1_Connection_Ch_>::DebugNum=0;
	}
}
#
#endif
