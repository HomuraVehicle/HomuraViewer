#ifndef HMR_THERMO_INC
#define HMR_THERMO_INC 104
#
/*===hmrThermo===
hmrThermo v1_04/140320 amby
	���x�f�[�^�̃o�b�t�@�@�\�ɑΉ�
hmrThermo v1_03/130720 amby
	���x�f�[�^�̎󂯓n�����@��inquiry����signal�ɕύX
hmrThermo v1_02/130602 iwahori
	�L�[�{�[�h�E�p�b�h�p��slot_setDataMode()��ǉ�(������void(void)�ɂȂ��Ă���)
hmrThermo v1_01/130414 iwahori
	���x�f�[�^�̎󂯓n�����@��signal���񂩂�inquiry�ɕύX
hmrThermo v1_00/130414 iwahori
	cThermoMsgAgent��signal-slot�ւ̑Ή�����
*/
#include "hmLibVer.hpp"
#include<boost/signals2.hpp>
#include<hmLib/signals.hpp>
#include <hmLib/inquiries.hpp>
#include <HomuraViewer/Message/ItfMessageAgent.hpp>
#include <homuraViewer/modeflags.hpp>

namespace hmr{
	namespace viewer{
		namespace thermo{
			//#define D_ADMaxValue 4096
			class cMsgAgent :public itfMessageAgent{
			public:
				modeflags DataMode;
				modeflags LogMode;
			public:
				static double toTemperature(unsigned char LowByte, unsigned char HighByte){
					unsigned int tmp = static_cast<unsigned int>(LowByte + HighByte * 256);
					//				return static_cast<double>(tmp);
					//		return (tmp*2*4096./4096-1069.3)/39.173;
					return (tmp * 2 * 4096. / 4096 - 1171.7) / 36.486;	//131104 �C���{�ЂƔ��ŕ␳
				}
				bool listen(datum::time_point Time_, bool Err_, const std::string& Data_)override{
					if(Data_.size() == 0)return true;

					if(static_cast<unsigned char>(Data_[0]) == 0x00){
						if(Data_.size() != 3)return true;
						//�f�[�^�����x�ɕϊ�
						Temperature = toTemperature(static_cast<unsigned char>(Data_[1]), static_cast<unsigned char>(Data_[2]));
						RawTemperature = static_cast<hmLib_uint16>(Data_.at(1)) + static_cast<hmLib_uint16>(Data_.at(2)) * 256;
						Time = Time_;

						// signal ���M
						signal_newData(Temperature, Time);
						signal_newRawData(Temperature, RawTemperature, Time);
						return false;
					} else if(static_cast<unsigned char>(Data_[0]) == 0x10){
						if(Data_.size() != 1)return true;
						DataModeFlagirl.set_pic(true);
						return false;
					} else if(static_cast<unsigned char>(Data_[0]) == 0x11){
						if(Data_.size() != 1)return true;
						DataModeFlagirl.set_pic(false);
						return false;
					} else if(static_cast<unsigned char>(Data_[0]) == 0xC0){
						if(Data_.size() != 1)return true;
						LogModeFlagirl.set_pic(true);
						return false;
					} else if(static_cast<unsigned char>(Data_[0]) == 0xC1){
						if(Data_.size() != 1)return true;
						LogModeFlagirl.set_pic(false);
						return false;
					} else if(static_cast<unsigned char>(Data_[0]) == 0xD0){
						if(Data_.size() != 7)return true;
						logTemperature = toTemperature(static_cast<unsigned char>(Data_.at(1)), static_cast<unsigned char>(Data_.at(2)));

						logRawTemperature =
							static_cast<hmLib_uint16>((hmLib_uint8)Data_.at(1))
							+ static_cast<hmLib_uint16>((hmLib_uint8)Data_.at(2)) * 256;

						// ���ݎ����擾
						hmLib_sint32 TimeSec =
							static_cast<hmLib_uint32>((hmLib_uint8)Data_.at(3))
							+ static_cast<hmLib_uint32>((hmLib_uint8)Data_.at(4)) * 256
							+ static_cast<hmLib_uint32>((hmLib_uint8)Data_.at(5)) * 256 * 256
							+ static_cast<hmLib_uint32>((hmLib_uint8)Data_.at(6)) * 256 * 256 * 256;
						// time_point �ɕϊ�
						logTime = std::chrono::system_clock::from_time_t(TimeSec);

						signal_newLogData(logTemperature, logTime);
						signal_newLogRawData(logTemperature, logRawTemperature, logTime);
						return false;
					}

					return true;
				}
				bool talk(std::string& Str)override{
					Str = "";
					if(DataModeFlagirl.talk()){
						if(DataModeFlagirl.request())Str.push_back(static_cast<unsigned char>(0x10));
						else Str.push_back(static_cast<unsigned char>(0x11));
						return false;
					} else if(LogModeFlagirl.talk()){
						if(LogModeFlagirl.request()) Str.push_back(static_cast<unsigned char>(0xC0));
						else Str.push_back(static_cast<unsigned char>(0xC1));
						return false;
					}

					return true;
				}
				void setup_talk(void)override{
					DataModeFlagirl.setup_talk();
					LogModeFlagirl.setup_talk();
				}
			private:
				hmLib::signals::unique_connections SignalConnections;
				hmLib::inquiries::unique_connections InquiryConnections;
			public:
				boost::signals2::signal<void(double data, clock::time_point time)> signal_newData;
				boost::signals2::signal<void(double data, clock::time_point time)> signal_newLogData;
			};
		}
	}
}

#
#endif
