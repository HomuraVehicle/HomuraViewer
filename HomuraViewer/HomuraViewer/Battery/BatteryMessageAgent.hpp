#ifndef HMR_BATTERY_INC
#define HMR_BATTERY_INC 102
#

/*=======hmrBattery=========
hmrBattery v1_02/130602 iwahori
	�L�[�{�[�h�E�p�b�h�p��slot_setDataMode()��ǉ�(������void(void)�ɂȂ��Ă���)
hmrBattery v1_01/130519 iwahori
	BatteryDataSet�p�~�Dstd::array�Ńf�[�^��ێ�
hmrBattery v1_00/130427 iwahori
	cBatteryMsgAgent��signal-slot,inquiry-contact�ւ̑Ή�����
*/

#include "hmLibVer.hpp"
#include <array>
#include <iterator>
#include <boost/signals2.hpp>
#include <hmLib/signals.hpp>
#include <hmLib/inquiries.hpp>
#include <HomuraViewer/Message/ItfMessageAgent.hpp>
#include <HomuraViewer/modeflags.hpp>
#include "BatteryData.hpp"
namespace hmr{
	namespace viewer{
		namespace battery{
			template<unsigned int BatteryNum_>
			class cMsgAgent :public itfMessageAgent{
				using this_data_t = battery::data<BatteryNum_>;
			private:
				static double toVoltage(unsigned char LowByte, unsigned char HighByte){
					unsigned int tmp = static_cast<unsigned int>(LowByte + HighByte * 256);
					return (static_cast<double>(tmp) / 4096)*10.7;
				}
			private:
				unsigned int TalkPos;
			public:
				modeflags DataMode;
			public:
				bool listen(datum::time_point Time_, bool Err_, const std::string& Str_)override{
					if(Str_.size() == 0) return true;

					// Batt read ON
					if(static_cast<unsigned char>(Str_[0]) == 0x10){
						if(Str_.size() != 1) return true;
						DataMode.set_pic(true);
						return false;
						// Batt read OFF
					} else if(static_cast<unsigned char>(Str_[0]) == 0x11){
						if(Str_.size() != 1) return true;
						DataMode.set_pic(false);
						return false;
						// Batt data coming
					} else if(static_cast<unsigned char>(Str_[0]) == 0x00){
						if(Str_.size() != BatteryNum_ * 2 + 1) return true;

						this_data_t Data;
						Data.time() = Time_;
						unsigned char i = 0;
						for(auto Itr = Data.begin(); Itr != Data.end(); ++Itr){
							(*Itr) = toVoltage(static_cast<unsigned char>(Str_[2 * i + 1]), static_cast<unsigned char>(Str_[2 * i + 2]));
						}

						//�V�f�[�^�@�V�O�i�����M
						signal_newData(Data);

						return false;
					}
					return true;
				}
				bool talk(std::string& Str)override{
					Str = "";
					switch(TalkPos){
					case 0:
						++TalkPos;
						if(DataMode){
							if(DataMode.get_req()) Str.push_back(static_cast<unsigned char>(0x10));
							else Str.push_back(static_cast<unsigned char>(0x11));
							return false;
						}
					default:
						return true;
					}
				}
				void setup_talk(void)override{
					TalkPos = 0;
				}
			public:
				boost::signals2::signal<void(this_data_t)> signal_newData;
			};
		}
	}
}


#
#endif
