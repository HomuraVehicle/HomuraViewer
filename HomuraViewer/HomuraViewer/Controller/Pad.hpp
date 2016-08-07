#ifndef HMRPAD_HPP
#define HMRPAD_HPP 102

#include<boost/signals2.hpp>
#include<hmLib/inquiries.hpp>
#include<hmLib/position.hpp>
#include"hmLibVer.hpp"
#include<hmLib_v2/dxBasic.hpp>
#include<hmLib_v2/dxPad.hpp>
#include<hmLib_v2/hmPos.hpp>
#include<HomuraViewer/any_predicate.hpp>


namespace PAD{
	enum PADNAME{
		But1=0,
		But2=1,
		But3=2,
		But4=3,
		But5=4,
		But6=5,
		But7=6,
		But8=7,
		But9=8,
		But10=9,
		But11=10,
		But12=11
	};
	enum CROSS_KEY{
		UP=0,
		DOWN=18000,
		RIGHT=9000,
		LEFT=27000,
		NONE=0xFFFFFFFF
	};
}
/*
#define D_PAD_1 4
#define D_PAD_2 5
#define D_PAD_3 6
#define D_PAD_4 7
#define D_PAD_5 8
#define D_PAD_6 9
#define D_PAD_7 10
#define D_PAD_8 11
#define D_PAD_9 12
#define D_PAD_10 13
#define D_PAD_13 16
#define D_PAD_14 17
#define D_PAD_15 18
#define D_PAD_16 19
*/

namespace hmr{
	namespace viewer{
		namespace dxmodule{
			template<unsigned int DX_INPUT_PAD_CH>
			class cPad{
			private:
				static DINPUT_JOYSTATE Pad;//現在のPadData
				static DINPUT_JOYSTATE bPad;//以前のPadData
			public:
				cPad(){
					Pad.X = 0;
					Pad.Y = 0;
					Pad.Z = 0;
					Pad.Rx = 0;
					Pad.Ry = 0;
					Pad.Rz = 0;
					//PadData初期化
					for(int i = 0; i < 32; i++){
						Pad.Buttons[i] = 0;
					}
				}
				static void work(){
					//If there is no Pad, return.
					if(!is_connect())return;

					//今回のボタン覚えとく
					bPad = Pad;
					//スティックデータ更新
					GetJoypadDirectInputState(DX_INPUT_PAD_CH, &Pad);
				}
				static Pint getLStick(){ return Pint(Pad.X, -Pad.Y); }
				static Pint getRStick(){ return Pint(-Pad.Rz, -Pad.Z); }
				static int getPad(PAD::PADNAME Pad_){
					return (Pad.Buttons[Pad_] != 0) * 2 + (Pad.Buttons[Pad_] == bPad.Buttons[Pad_]) - 1;
				}
				static int getCrossKey(){ return Pad.POV[0]; }
				static int getbCrossKey(){ return bPad.POV[0]; }
			public:
				static void connect(){ dxPad::connect(); }
				static bool is_connect(){ return GetJoypadNum() >= DX_INPUT_PAD_CH; }
			};
			template<unsigned int DX_INPUT_PAD_CH>
			DINPUT_JOYSTATE cPad<DX_INPUT_PAD_CH>::Pad;//現在のPadData
			template<unsigned int DX_INPUT_PAD_CH>
			DINPUT_JOYSTATE cPad<DX_INPUT_PAD_CH>::bPad;//以前のPadData

			template<unsigned int DX_INPUT_PAD_CH>
			class is_pad_pushed{
			private:
				PAD::PADNAME PadName;
			public:
				is_pad_pushed(PAD::PADNAME PadName_) :PadName(PadName_){}
				bool operator()(void)const{ return cPad<DX_INPUT_PAD_CH>::getPad(PadName) == 1; }
			};
			typedef is_pad_pushed<DX_INPUT_PAD1> is_pad1_pushed;
			template<unsigned int DX_INPUT_PAD_CH>
			class have_pad_pushed{
			private:
				PAD::PADNAME PadName;
			public:
				have_pad_pushed(PAD::PADNAME PadName_) :PadName(PadName_){}
				bool operator()(void)const{ return cPad<DX_INPUT_PAD_CH>::getPad(PadName) > 0; }
			};
			typedef have_pad_pushed<DX_INPUT_PAD1> have_pad1_pushed;
			template<unsigned int DX_INPUT_PAD_CH>
			class is_pad_pulled{
			private:
				PAD::PADNAME PadName;
			public:
				is_pad_pulled(PAD::PADNAME PadName_) :PadName(PadName_){}
				bool operator()(void)const{ return cPad<DX_INPUT_PAD_CH>::getPad(PadName) == -1; }
			};
			typedef is_pad_pulled<DX_INPUT_PAD1> is_pad1_pulled;
			template<unsigned int DX_INPUT_PAD_CH>
			class is_pad_cross_key_pushed{
			private:
				PAD::CROSS_KEY CrossKey;
			public:
				is_pad_cross_key_pushed(PAD::CROSS_KEY CrossKey_) :CrossKey(CrossKey_){}
				bool operator()(void)const{ return cPad<DX_INPUT_PAD_CH>::getCrossKey() == CrossKey; }
			};
			typedef is_pad_cross_key_pushed<DX_INPUT_PAD1> is_pad1_cross_key_pushed;
			template<unsigned int DX_INPUT_PAD_CH>
			class is_pad_cross_key_pulled{
			private:
				PAD::CROSS_KEY CrossKey;
			public:
				is_pad_cross_key_pulled(PAD::CROSS_KEY CrossKey_) :CrossKey(CrossKey_){}
				bool operator()(void)const{ return ((cPad<DX_INPUT_PAD_CH>::getbCrossKey() == CrossKey) && (cPad<DX_INPUT_PAD_CH>::getCrossKey() == PAD::CROSS_KEY::NONE)); }
			};
			typedef is_pad_cross_key_pulled<DX_INPUT_PAD1> is_pad1_cross_key_pulled;


			class cDxPad1{
				typedef boost::signals2::signal<void(void)> signal_t;
				typedef boost::signals2::signal<void(hmLib::pint)> signal_stick_t;
			private:
				std::map<hmLib::any_predicate, std::unique_ptr<signal_t>> SignalMap;
				hmLib::inquiries::unique_connections InquiryConnections;
			public:
				signal_t& signal(hmLib::any_predicate&& AnyPredicate_){
					auto itr = SignalMap.find(AnyPredicate_);
					if(itr != SignalMap.end())return *(itr->second);
					std::unique_ptr<signal_t> Ptr(new signal_t());
					return *(SignalMap[std::move(AnyPredicate_)] = std::move(Ptr));
				}
				void contact(PAD::PADNAME Pad_, hmLib::inquiries::inquiry<int> Inquiry_){
					InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->int{return cPad<DX_INPUT_PAD1>::getPad(Pad_); }));
				}
				signal_stick_t signal_LStick;
				signal_stick_t signal_RStick;
			public:
				void work(){
					cPad<DX_INPUT_PAD1>::work();

					unsigned int Cnt = 0;
					for(auto itr = std::begin(SignalMap); itr != std::end(SignalMap); ++itr){
						if((itr->first)()){
							(*(itr->second))();
						}
						++Cnt;
					}
					Pint LPos = cPad<DX_INPUT_PAD1>::getLStick();
					signal_LStick(hmLib::pint(LPos.x,LPos.y));
					Pint RPos = cPad<DX_INPUT_PAD1>::getRStick();
					signal_RStick(hmLib::pint(RPos.x, RPos.y));
				}
			};
		}
	}
}
#
#endif
