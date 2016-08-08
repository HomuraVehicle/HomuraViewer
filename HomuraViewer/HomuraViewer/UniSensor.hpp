#ifndef HMR_UNISENSOR_INC
#define HMR_UNISENSOR_INC 104
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
#include <cstdint>
#include <functional>
#include <boost/signals2.hpp>
#include <hmLib/signals.hpp>
#include <hmLib/inquiries.hpp>
#include <HomuraViewer/Message/ItfMessageAgent.hpp>
#include "modeflags.hpp"

namespace hmr{
	namespace viewer{
		namespace unisensor{
			struct data16{
				clock::time_point Time;
				std::uint16_t RawValue;
				double Value;
			public:
				data16() = default;
				data16(clock::time_point Time_, std::uint16_t RawValue_, double Value_)
					: Time(Time_)
					, RawValue(RawValue_)
					, Value(Value_){}
			};
			struct data1616{
				clock::time_point Time;
				std::uint16_t RawValue1;
				std::uint16_t RawValue2;
				double Value1;
				double Value2;
			public:
				data1616() = default;
				data1616(clock::time_point Time_, std::uint16_t RawValue1_, double Value1_, std::uint16_t RawValue2_, double Value2_)
					: Time(Time_)
					, RawValue1(RawValue1_)
					, Value1(Value1_)
					, RawValue2(RawValue2_)
					, Value2(Value2_){}
			};
		}
		class cUniSensor16MsgAgent :public itfMessageAgent{
			using data_t = unisensor::data16;
		private:
			modeflags DataModeFlag;
			data_t Data;
			unsigned int TalkPos;
			std::function<double(std::uint16_t)> ConvertFn;
		public:
			bool listen(datum::time_point Time_, bool Err_, const std::string& Data_)override{
				if(Data_.size() == 0)return true;

				if(static_cast<unsigned char>(Data_[0]) == 0x00){
					//�������G���[�Ȃ�܂��͂���
					if(Data_.size() != 3)return true;

					//�f�[�^�����x�ɕϊ�
					Data.Time = Time_;
					Data.RawValue = static_cast<std::uint16_t>(Data_.at(1)) + static_cast<std::uint16_t>(Data_.at(2)) * 256;
					Data.Value = ConvertFn(Data.RawValue);

					// signal ���M
					signal_newData(Data);

					return false;
				} else if(static_cast<unsigned char>(Data_[0]) == 0x10){
					if(Data_.size() != 1)return true;
					DataModeFlag.set_pic(true);
					return false;
				} else if(static_cast<unsigned char>(Data_[0]) == 0x11){
					if(Data_.size() != 1)return true;
					DataModeFlag.set_pic(false);
					return false;
				}

				return true;
			}
			void setup_talk(void)override{
				TalkPos = 0;
			}
			bool talk(std::string& Str)override{
				Str = "";

				switch(TalkPos){
				case 0:
					if(DataModeFlag){
						if(DataModeFlag.get_req())Str.push_back(static_cast<unsigned char>(0x10));
						else Str.push_back(static_cast<unsigned char>(0x11));

						TalkPos = 1;
						return false;
					}
				default:
					return true;
				}
			}
		private:
			hmLib::signals::unique_connections SignalConnections;
			hmLib::inquiries::unique_connections InquiryConnections;
		public:
			boost::signals2::signal<void(data_t)> signal_newData;

			void contact_getData(hmLib::inquiries::inquiry<data_t>& Inquiry_){
				InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->data_t{return this->Data; }));
			}
			void contact_getPicDataMode(hmLib::inquiries::inquiry<bool>& Inquiry_){
				InquiryConnections(DataModeFlag.contact_getPic(Inquiry_));
			}
			void contact_getRequestDataMode(hmLib::inquiries::inquiry<bool>& Inquiry_){
				InquiryConnections(DataModeFlag.contact_getReq(Inquiry_));
			}
			void slot_setDataMode(boost::signals2::signal<void(bool)>& Signal_){
				SignalConnections(DataModeFlag.slot_setReq(Signal_));
			}
		};
		class cUniSensor1616MsgAgent :public itfMessageAgent{
			using data_t = unisensor::data1616;
		private:
			modeflags DataModeFlag;
			data_t Data;
			unsigned int TalkPos;
			std::function<std::pair<double, double>(std::uint16_t, std::uint16_t)> ConvertFn;
		public:
			bool listen(datum::time_point Time_, bool Err_, const std::string& Data_)override{
				if(Data_.size() == 0)return true;

				if(static_cast<unsigned char>(Data_[0]) == 0x00){
					//�������G���[�Ȃ�܂��͂���
					if(Data_.size() != 5)return true;

					//�f�[�^�����x�ɕϊ�
					Data.Time = Time_;
					Data.RawValue1 = static_cast<std::uint16_t>(Data_.at(1)) + static_cast<std::uint16_t>(Data_.at(2)) * 256;
					Data.RawValue2 = static_cast<std::uint16_t>(Data_.at(3)) + static_cast<std::uint16_t>(Data_.at(4)) * 256;
					std::tie(Data.Value1, Data.Value2) = ConvertFn(Data.RawValue1, Data.RawValue2);

					// signal ���M
					signal_newData(Data);

					return false;
				} else if(static_cast<unsigned char>(Data_[0]) == 0x10){
					if(Data_.size() != 1)return true;
					DataModeFlag.set_pic(true);
					return false;
				} else if(static_cast<unsigned char>(Data_[0]) == 0x11){
					if(Data_.size() != 1)return true;
					DataModeFlag.set_pic(false);
					return false;
				}

				return true;
			}
			void setup_talk(void)override{
				TalkPos = 0;
			}
			bool talk(std::string& Str)override{
				Str = "";

				switch(TalkPos){
				case 0:
					if(DataModeFlag){
						if(DataModeFlag.get_req())Str.push_back(static_cast<unsigned char>(0x10));
						else Str.push_back(static_cast<unsigned char>(0x11));

						TalkPos = 1;
						return false;
					}
				default:
					return true;
				}
			}
		private:
			hmLib::signals::unique_connections SignalConnections;
			hmLib::inquiries::unique_connections InquiryConnections;
		public:
			boost::signals2::signal<void(data_t)> signal_newData;

			void contact_getData(hmLib::inquiries::inquiry<data_t>& Inquiry_){
				InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->data_t{return this->Data; }));
			}
			void contact_getPicDataMode(hmLib::inquiries::inquiry<bool>& Inquiry_){
				InquiryConnections(DataModeFlag.contact_getPic(Inquiry_));
			}
			void contact_getRequestDataMode(hmLib::inquiries::inquiry<bool>& Inquiry_){
				InquiryConnections(DataModeFlag.contact_getReq(Inquiry_));
			}
			void slot_setDataMode(boost::signals2::signal<void(bool)>& Signal_){
				SignalConnections(DataModeFlag.slot_setReq(Signal_));
			}
		};
	}
}

#
#endif
