#ifndef HMR_VIEWER_THERMO_THERMOMSGAGENT_INC
#define HMR_VIEWER_THERMO_THERMOMSGAGENT_INC 104
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
#include <HomuraViewer/Message/ItfMessageAgent.hpp>
#include <homuraViewer/modeflags.hpp>

namespace hmr{
	namespace viewer{
		namespace thermo{
			//#define D_ADMaxValue 4096
			class cMsgAgent :public itfMessageAgent{
			public:
				modeflags DataMode;
				unsigned int MsgPos;
			public:
				static double toTemperature(unsigned int RawData){
					//		return (Data*2*4096./4096-1069.3)/39.173;
					return (RawData * 2 * 4096. / 4096 - 1171.7) / 36.486;	//131104 �C���{�ЂƔ��ŕ␳
				}
				bool listen(datum::time_point Time_, bool Err_, const std::string& Data_)override{
					if(Data_.size() == 0)return true;

					if(static_cast<unsigned char>(Data_[0]) == 0x00){
						if(Data_.size() != 3)return true;
						data_t Data;

						Data.Time = Time_;
						Data.RawData = static_cast<hmLib_uint16>(Data_.at(1)) + static_cast<hmLib_uint16>(Data_.at(2)) * 256;

						//�f�[�^�����x�ɕϊ�
						Data.Temperature = toTemperature(Data.RawData);

						// signal ���M
						signal_newData(Data);

						return false;
					} else if(static_cast<unsigned char>(Data_[0]) == 0x10){
						if(Data_.size() != 1)return true;
						DataMode.set_pic(true);
						return false;
					} else if(static_cast<unsigned char>(Data_[0]) == 0x11){
						if(Data_.size() != 1)return true;
						DataMode.set_pic(false);
						return false;
					}

					return true;
				}
				bool talk(std::string& Str)override{
					Str = "";
					switch(MsgPos){
					case 0:
						++MsgPos;
						if(DataMode){
							if(DataMode.get_req())Str.push_back(static_cast<unsigned char>(0x10));
							else Str.push_back(static_cast<unsigned char>(0x11));
							return false;
						}
					default:
						return true;
					}
				}
				void setup_talk(void)override{
					MsgPos = 0;
				}
			public:
				boost::signals2::signal<void(data_t)> signal_newData;
			};
		}
	}
}

#
#endif
