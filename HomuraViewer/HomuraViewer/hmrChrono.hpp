#ifndef HMR_CHRONO_INC
#define HMR_CHRONO_INC 100
#
/*========== hmrChrono ============
Chrono
*/

#include "hmLibVer.hpp"
#include <boost/signals2.hpp>
#include <hmLib/signals.hpp>
#include <hmLib/inquiries.hpp>
#include <hmLib/type.h>
#include <HomuraViewer/Message/ItfMessageAgent.hpp>
#include "vmstatus.hpp"
#include <HomuraViewer/Data.hpp>

namespace hmr{
	//Chrono�֘A
	class cChrono :public itfMessageAgent {
	public:
		typedef std::chrono::milliseconds milliseconds;
	private:
		//�A���[���̎��s�Ԋu
		vmstatus<unsigned int> AutoGetInterval;
		//�����␳�@�\�����s����
		milliseconds RequestCorrectMachineTime;
		//�@�̂̎��Ԃ̂�����L�^���Ă���
		milliseconds MachineDifTime;
	private:
		//�����␳�@�\
		class ping {
			//Ping�̉񐔋L�^
			unsigned int Cnt;
			//PIC �̓������Ԃ��󂯎��R�}���h�𑗂������ԂƕԐM�̎���
			clock::time_point SendTime;
			clock::time_point RecvTime;
			//�ݐϗp
			clock::duration WaitTimeAccumulation;
			clock::duration  DifTimeAccumulation;
		public:
			void reset() {
				Cnt=0;
				WaitTimeAccumulation= std::chrono::milliseconds(0);
				DifTimeAccumulation= std::chrono::milliseconds(0);
			}
			void start() { SendTime = clock::now(); }
			void stop(const clock::time_point& MachineTime_) {
				RecvTime = clock::now();
				WaitTimeAccumulation+=RecvTime-SendTime;
				DifTimeAccumulation+= MachineTime_ - RecvTime + (RecvTime-SendTime)/2;
				++Cnt;
			}
			unsigned int getCnt()const { return Cnt; }
			std::chrono::milliseconds getAverageWaitTime() const {
				return std::chrono::duration_cast<std::chrono::milliseconds>(WaitTimeAccumulation/Cnt);
			}
			std::chrono::milliseconds getAverageDifTime() const {
				return std::chrono::duration_cast<std::chrono::milliseconds>(DifTimeAccumulation/Cnt);
			}
		};
		ping Ping;
		//Ping��ł�
		unsigned int RequestPingNum;
	private:
		bool SendNowTime;
		bool SendCorrection;
		bool SendAlarm;
	public:
		//���ݎ����̎擾��v������
		void requestMachineTime(unsigned int Num_) {
			if(RequestPingNum)return;
			RequestPingNum =Num_;
			Ping.reset();
		}
		//���ݎ������擾�����H
		bool is_requestMachineTime()const { return RequestPingNum>0; }
		//���ݎ������␳�\��?
		bool can_correctMachineTime()const { return Ping.getCnt()>0 && RequestPingNum==0; }
		//�����␳��v��
		void correctMachineTime() {
			if(!can_correctMachineTime())return;
			RequestCorrectMachineTime = -Ping.getAverageDifTime();
		}
		//���ݎ������␳�\��?
		bool can_getMachineDifTime()const { return Ping.getCnt()>0 && RequestPingNum==0; }
		//PC���Ԃ��猩���@�̎��Ԃ̂�����擾
		std::chrono::milliseconds getMachineDifTime()const { return  Ping.getAverageDifTime(); }
		//����̃C���^�[�o��
		void setArarmInterval(unsigned int AutoGetIntervalSec_) { AutoGetInterval.setv(AutoGetIntervalSec_); }
	public:
		void setup_talk(void)override {
			if(AutoGetInterval()) {
				SendAlarm=true;
			}

			if(RequestPingNum) {
				SendNowTime=true;
			}

			if(RequestCorrectMachineTime!=milliseconds(0)) {
				SendCorrection=true;
			}

			return;
		}
		bool talk(std::string& Str)override {
			// chrono :: get now time
			if(SendNowTime) {
				Str.push_back(static_cast<unsigned char>(0x00));
				// �R�}���h�𑗂������̎���
				Ping.start();
				--RequestPingNum;

				SendNowTime = false;
				return false;
			}
			// chrono :: set auto get interval
			if(SendAlarm) {
				// interval �̐ݒ�
				hmLib_sint32 t_int = static_cast<hmLib_sint32>(AutoGetInterval.getv());

				//���M
				Str.push_back(static_cast<unsigned char>(0x01));
				Str.push_back(static_cast<unsigned char>(t_int & 0xFF));
				Str.push_back(static_cast<unsigned char>((t_int >> 8) & 0xFF));
				Str.push_back(static_cast<unsigned char>((t_int >> 16) & 0xFF));
				Str.push_back(static_cast<unsigned char>((t_int >> 24) & 0xFF));

				// 1����̑��M�ŏI���
				SendAlarm = false;

				return false;
			}

			// chrono :: modify time 
			if(SendCorrection) {
				// ���Ԃ�]�܂����`�ɕύX����
				auto TimeSec = std::chrono::duration_cast< std::chrono::seconds >(RequestCorrectMachineTime);
				std::chrono::milliseconds TimeMSec = RequestCorrectMachineTime - TimeSec;

				// ms �����̎��͂��ꂪ���ɂȂ�Ȃ��悤�ɕύX����
				if(TimeMSec.count() < 0) {
					TimeSec -= std::chrono::seconds(1);
					TimeMSec += std::chrono::seconds(1);
				}

				// ���M�p�ɕύX
				hmLib_sint32 t_s = static_cast<hmLib_sint32>(TimeSec.count());
				hmLib_uint16 t_ms = static_cast<hmLib_uint16>(TimeMSec.count());

				Str.push_back(static_cast<unsigned char>(0x02));
				Str.push_back(static_cast<unsigned char>(t_s & 0xFF));
				Str.push_back(static_cast<unsigned char>((t_s >> 8) & 0xFF));
				Str.push_back(static_cast<unsigned char>((t_s >> 16) & 0xFF));
				Str.push_back(static_cast<unsigned char>((t_s >> 24) & 0xFF));
				Str.push_back(static_cast<unsigned char>(t_ms & 0xFF));
				Str.push_back(static_cast<unsigned char>((t_ms >> 8) & 0xFF));

				SendCorrection = false;
				RequestCorrectMachineTime = std::chrono::milliseconds::zero();
				return false;
			}

			return true;
		}
		bool listen(datum::time_point Time_, bool Err_, const std::string& Str_)override {
			if(Str_.size() == 0)return true;

			if(static_cast<unsigned char>(Str_.at(0)) == 0x00) {
				if(Str_.size() != 7)return true;

				// ���ݎ����擾
				hmLib_sint32 MachineTimeSec = static_cast<hmLib_uint8>(Str_.at(1)) + static_cast<hmLib_uint8>(Str_.at(2)) * 256
					+ static_cast<hmLib_uint8>(Str_.at(3)) * 256 * 256 + static_cast<hmLib_uint8>(Str_.at(4)) * 256 * 256 * 256;
				hmLib_uint16 MachineTimeMSec = static_cast<hmLib_uint16>((hmLib_uint8)Str_.at(5)) + static_cast<hmLib_uint16>((hmLib_uint8)Str_.at(6)) * 256;

				// time_point �ɕϊ�
				clock::time_point MachineTimePoint = std::chrono::system_clock::from_time_t(MachineTimeSec) + std::chrono::milliseconds(MachineTimeMSec);

				// �f�[�^����M�������Ԃ��L�^
				Ping.stop(MachineTimePoint);

				//�����v�����I��������A�V�O�i������
				if(can_getMachineDifTime()) {
					MachineDifTime=getMachineDifTime();
					signal_getMachineDifTime(MachineDifTime);
				}

				return false;
			}
			if(static_cast<unsigned char>(Str_.at(0)) == 0x01) {
				if(Str_.size() != 3)return true;

				// ���ݎ����擾
				hmLib_uint16 Interval = static_cast<hmLib_uint16>(static_cast<hmLib_uint8>(Str_.at(1))) + static_cast<hmLib_uint16>(static_cast<hmLib_uint8>(Str_.at(1))) * 256;
				AutoGetInterval.setm(Interval);

				return false;
			}
			// chrono :: �����C���ɑ΂���Ԏ�
			else if(static_cast<unsigned char>(Str_.at(0)) == 0x02) {
				if(Str_.size() != 7)return true;

				// ���ݎ����擾
				hmLib_sint32 t_s = (hmLib_sint32)((hmLib_uint32)(static_cast<hmLib_uint8>(Str_.at(1)) + static_cast<hmLib_uint8>(Str_.at(2)) * 256
					+ static_cast<hmLib_uint8>(Str_.at(3)) * 256 * 256 + static_cast<hmLib_uint8>(Str_.at(4)) * 256 * 256 * 256));
				hmLib_uint16 t_ms = static_cast<hmLib_uint16>((hmLib_uint8)Str_.at(5)) + static_cast<hmLib_uint16>((hmLib_uint8)Str_.at(6)) * 256;

				// millisec time �ɕϊ�
				std::chrono::milliseconds time = std::chrono::seconds(t_s) + std::chrono::milliseconds(t_ms);

				//�␳���ꂽ���Ԃ���������
				RequestCorrectMachineTime = std::chrono::milliseconds::zero();

				//MachineTime���X�V���Ă���
				MachineDifTime += time;
				signal_getMachineDifTime(MachineDifTime);

				return false;
			}

			// chrono :: ����
			else if(static_cast<unsigned char>(Str_.at(0)) == 0x0A) {
				if(Str_.size() != 5)return true;

				// ���ݎ����擾
				hmLib_sint32 t_s = (hmLib_sint32)((hmLib_uint32)(static_cast<hmLib_uint8>(Str_.at(1)) + static_cast<hmLib_uint8>(Str_.at(2)) * 256
					+ static_cast<hmLib_uint8>(Str_.at(3)) * 256 * 256 + static_cast<hmLib_uint8>(Str_.at(4)) * 256 * 256 * 256));

				// time_point �ɕϊ�
				clock::time_point tp = std::chrono::system_clock::from_time_t(t_s);


				//�{���́A�󂯎�����������t�@�C���ɋL�^�����肵�����B
				//	���X�̖ړI�́A���̃Z���T�[�f�[�^�̂�����������񋟂��Ȃ��f�[�^�̂��߂́A
				//	�␳�p�̃^�C���X�^���v�B�����A�����̊ϑ��f�[�^��UnixTime�����ȏア��Ȃ��B

				return false;
			}

			return true;
		}
	private:
		hmLib::signals::unique_connections SignalConnections;
		hmLib::inquiries::unique_connections InquiryConnections;
	public:
		//�@�̎����擾�v��
		void slot_requestMachineTime(boost::signals2::signal<void(void)>& Signal_) {
			SignalConnections(hmLib::signals::connect(Signal_, [&](void)->void {requestMachineTime(3); }));
		}

		//�@�̎����擾���ɔ���
		boost::signals2::signal<void(std::chrono::milliseconds)> signal_getMachineDifTime;

		//�����␳�����s�\���H
		void contact_can_correctMachineTime(hmLib::inquiries::inquiry<bool>& Inquiry_) {
			InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&]()->bool {return can_correctMachineTime(); }));
		}

		//�����␳�v��
		void slot_correctMachineTime(boost::signals2::signal<void(void)>& Signal_) {
			SignalConnections(hmLib::signals::connect(Signal_, [&](void)->void {correctMachineTime(); }));
		}
	};
}
#
#endif
