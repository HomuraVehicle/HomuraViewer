#ifndef HMR_VIEWER_CAMERA_DXCAMERA_INC
#define HMR_VIEWER_CAMERA_DXCAMERA_INC 100
#
#include"hmLibVer.hpp"
#include<boost/format.hpp>
#include<boost/signals2.hpp>
//#include<hmLib/dxImage.hpp>
#include<hmLib_v2/dxObject.hpp>
#include<hmLib_v2/dxArea.hpp>
#include<hmLib_v2/dxColorSet.hpp>
#include<hmLib/signals.hpp>
#include<hmLib/inquiries.hpp>
#include <HomuraViewer/chrono.hpp>
namespace hmr{
	namespace viewer{
		namespace camera{
			class dxosCamera :public dxReObject, public hmoBox{
			private:
				dxGraph Grph;
				std::string Data;
				clock::time_point Time;
				bool IsConfigMode;
				bool IsMirrorMode;
				bool IsConvertMode;
				unsigned char AddSat;
				unsigned int RateVal;
			private:
				void convertToGrph(dxImGraph& ImGrph_){
					Pint GrphSize = ImGrph_.getSize();
					for(int x = 0; x < GrphSize.x; ++x){
						for(int y = 0; y < GrphSize.y; ++y){
							dxHSV TmpClr = ImGrph_.getClr(Pint(x, y));
							TmpClr.sat = std::min(static_cast<int>(255), static_cast<int>(TmpClr.sat) + static_cast<int>(AddSat));
							TmpClr.vlu = std::max(0, std::min(255, static_cast<int>(static_cast<double>(TmpClr.vlu)*RateVal / 100.0)));
							ImGrph_.putClr(Pint(x, y), TmpClr);
						}
					}

					Grph.load(ImGrph_);
				}
			public:
				dxosCamera(Pint _size)
					:IsConfigMode(false)
					, IsMirrorMode(false)
					, IsConvertMode(false)
					, AddSat(0)
					, RateVal(100)
					, hmoBox(_size){}
				int redraw(dxO& dxo){
					if(Grph.fail()){
						dxo.draw(Pint(0, 0), dxoStrP(size, "No Data"));
						return 0;
					}

					double Rate = std::min(1.0*size.x / Grph.getSize().x, 1.0*size.y / Grph.getSize().y);
					dxo.draw(size / 2, dxoRotGraph(Grph, Rate, hm::sel(IsMirrorMode, hmLib::pi(), 0.0), false));

					dxo.draw(Pint(25, 0), dxoStrP(Pint(0, 20), (boost::format("%d*%d") % Grph.getSize().x%Grph.getSize().y).str(), CLR::Red, ALI::left));

					if(IsConfigMode){
						dxo.draw(Pint(0, 20), dxoBox(Pint(160, 120), CLR::White), true, dxDMode(128));

						if(dxo.draw(Pint(5, 25), dxoButIO(Pint(70, 20), "反転表示", CLR::SoftBlue, IsMirrorMode)) == 1){
							IsMirrorMode = !IsMirrorMode;
						}

						if(dxo.draw(Pint(5, 50), dxoButIO(Pint(70, 20), "自動変換", CLR::SoftBlue, IsConvertMode)) == 1){
							IsConvertMode = !IsConvertMode;
						}

						if(dxo.draw(Pint(85, 25), dxoBut(Pint(70, 20), "変換", CLR::SoftRed)) < 0){
							dxImGraph TmpImGrph;
							TmpImGrph.load(const_cast<char*>(Data.c_str()), Data.size());
							convertToGrph(TmpImGrph);
						}

						Pint Tmp;

						dxo.draw(Pint(85, 50), dxoStrP(Pint(0, 20), (boost::format("彩度:+%u") % static_cast<unsigned int>(AddSat)).str(), CLR::White, ALI::left));
						Tmp.set(AddSat, 0);
						dxo.draw(Pint(85, 70), dxoSlider(Pint(70, 20), Pint(10, 20), Tmp, Pint(255, 0), CLR::Red));
						AddSat = static_cast<unsigned char>(Tmp.x);

						dxo.draw(Pint(85, 95), dxoStrP(Pint(0, 20), (boost::format("輝度:%u%%") % RateVal).str(), CLR::White, ALI::left));
						Tmp.set(RateVal, 0);
						dxo.draw(Pint(85, 115), dxoSlider(Pint(70, 20), Pint(10, 20), Tmp, Pint(500, 0), CLR::Red));
						RateVal = static_cast<unsigned int>(Tmp.x);
					}

					if(dxo.draw(Pint(0, 0), dxoButIO(Pint(20, 20), "", CLR::Red, IsConfigMode), true, dxDMode(196)) == 1){
						IsConfigMode = !IsConfigMode;
					}

					return 0;
				}
			private:
				void setNewGraph(const std::string& Data_, clock::time_point Time_){
					Data = Data_;
					Time = Time_;
					//
					if(Data.size() == 0){
						Grph.del();
						return;
					}

					//画質変換モードの場合
					if(IsConvertMode){
						//一時的に、SoftImageにデータ移行
						dxImGraph TmpImGrph;
						TmpImGrph.load(const_cast<char*>(Data.c_str()), Data.size());
						//変換して、Graphにぶちこむ
						convertToGrph(TmpImGrph);
					} else{
						//直接Graphにぶちこむ
						Grph.load(const_cast<char*>(Data.c_str()), Data.size());
					}
				}
			private:
				hmLib::signals::unique_connections SignalConnections;
			public:
				void slot_setNewGraph(boost::signals2::signal<void(const std::string&, clock::time_point)>& Signal_){
					SignalConnections(hmLib::signals::connect(Signal_, [&](const std::string& Str_, clock::time_point Time_)->void{this->setNewGraph(Str_, Time_); }));
				}
			};
		}
	}
}
#
#endif
