#ifndef HMR_VIEWER_CO2_CO2MSGAGENT_INC
#define HMR_VIEWER_CO2_CO2MSGAGENT_INC 101
#
/*======hmrCO2=======
hmrCO2 v1_01/130602 iwahori
	キーボード・パッド用のslot_setDataMode()を追加(引数がvoid(void)になっている)
hmrCO2 v1_00/130427 iwahori
	cCO2MsgAgentのsignal-slot,inquiry-contactへの対応完了
*/

#include "hmLibVer.hpp"
#include<cstdint>
#include<boost/signals2.hpp>
#include<hmLib/signals.hpp>
#include <hmLib/inquiries.hpp>
#include <HomuraViewer/Message/ItfMessageAgent.hpp>
#include <HomuraViewer/modeflags.hpp>
#include "CO2Data.hpp"
namespace hmr{
	namespace viewer{
		namespace co2{
			class cMsgAgent :public itfMessageAgent{
			public:
				modeflags DataMode;
				modeflags PumpPW;
				modeflags SensorPW;
			private:
				unsigned int MsgPos;
			public:
				static double toCO2(std::uint16_t Bytes){
					std::uint16_t tmp = static_cast<std::uint16_t>(Bytes);
					return ((static_cast<double>(tmp) / 4096. * 2.5) - 0.5)*1000.;
				}
				bool listen(datum::time_point Time_, bool Err_, const std::string& Str_)override{

					// data getting mode
					if(static_cast<unsigned char>(Str_[0]) == 0x00){
						if(Str_.size() != 3)return true;

						data_t Data;
						Data.Time = Time_;
						Data.RawValue = static_cast<std::uint16_t>(Str_[1]) + (static_cast<std::uint16_t>(Str_[2]) << 8);
						Data.Value = toCO2(Data.RawValue);

						signal_newData(Data);
						//		Ref.insert(Datum(_time,_errFlag,static_cast<unsigned char>(Str_[2])*256+static_cast<unsigned char>(Str_[1])));
						return false;
					}

					// Change Data getting mode
					else if(static_cast<unsigned char>(Str_[0]) == 0x10){
						if(Str_.size() != 1)return true;
						DataMode.set_pic(true);
						return false;
					} else if(static_cast<unsigned char>(Str_[0]) == 0x11){
						if(Str_.size() != 1)return true;
						DataMode.set_pic(false);
						return false;
					}

					// Change sensor power status
					else if(static_cast<unsigned char>(Str_[0]) == 0x20){
						if(Str_.size() != 1)return true;
						SensorPW.set_pic(true);
						return false;
					} else if(static_cast<unsigned char>(Str_[0]) == 0x21){
						if(Str_.size() != 1)return true;
						SensorPW.set_pic(false);
						return false;
					}

					// Change pump power status
					else if(static_cast<unsigned char>(Str_[0]) == 0x30){
						if(Str_.size() != 1)return true;
						PumpPW.set_pic(true);
						return false;
					} else if(static_cast<unsigned char>(Str_[0]) == 0x31){
						if(Str_.size() != 1)return true;
						PumpPW.set_pic(false);
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
					case 1:
						++MsgPos;
						if(SensorPW){
							if(SensorPW.get_req())Str.push_back(static_cast<unsigned char>(0x20));
							else Str.push_back(static_cast<unsigned char>(0x21));
							return false;
						}
					case 2:
						++MsgPos;
						if(PumpPW){
							if(PumpPW.get_req())Str.push_back(static_cast<unsigned char>(0x30));
							else Str.push_back(static_cast<unsigned char>(0x31));
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
