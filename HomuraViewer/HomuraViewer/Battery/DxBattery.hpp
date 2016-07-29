#ifndef USE_HMRDXBATTERY_INC
#define USE_HMRDXBATTERY_INC 100
#
#include<hmLib/inquiries.hpp>
#include"hmLibVer.hpp"
#include<hmLib_v2/dxArea.hpp>
#include<hmLib_v2/dxObject.hpp>
#include<hmLib_v2/dxColorSet.hpp>
#include<HomuraViewer/chrono.hpp>
namespace hmr{
	namespace viewer{
		class dxosBattery :public dxReObject, public hmoBox{
		private:
			double MinVoltage;
			double MaxVoltage;
			double Voltage;
			clock::time_point Time;
		public:
			dxosBattery(Pint size_, double MinVoltage_, double MaxVoltage_)
				: hmoBox(size_)
				, MinVoltage(MinVoltage_)
				, MaxVoltage(MaxVoltage_)
				, Voltage(0.){}
		protected:
			int redraw(dxO& dxo){
				Pint boxsize(size.x - size.y * 2 / 3, size.y);

				hmRGB Clr;

				bool IsRecent = (clock::now() - Time > std::chrono::seconds(10));
				if(Voltage > 0.){
					double Ratio = (Voltage - MinVoltage) / (MaxVoltage - MinVoltage);
					Ratio = std::min(1.0, Ratio);
					Ratio = std::max(0.0, Ratio);
					if(Ratio > 0.50)Clr.set(0, 0, 255);
					else if(Ratio > 0.25)Clr.set(0, 196, 196);
					else if(Ratio > 0.10)Clr.set(255, 128, 0);
					else Clr.set(255, 0, 0);
					dxo.draw(Pint(0, 0), dxoProgress(boxsize, static_cast<int>(Ratio * 1000), 1000, Clr, false));
					dxo.draw(Pint(0, 0), dxoStrP(boxsize, hmstd::fStr("%.2fV", Voltage), IsRecent ? CLR::White : CLR::Gray, ALI::center));
				} else{
					Clr.set(196, 196, 196);
					dxo.draw(Pint(0, 0), dxoProgress(boxsize, 0, 1000, Clr, false));
					dxo.draw(Pint(0, 0), dxoStrP(boxsize, "No Data", Clr, ALI::center));
				}

				dxo.draw(Pint(boxsize.x, size.y / 3), dxoBox(Pint(size.y / 3, size.y / 3), Clr));

				return 0;
			}
		private:
			hmLib::signals::unique_connections SignalConnections;
		public:
			void slot_newData(boost::signals2::signal<void(clock::time_point, double)>& signal_){
				SignalConnections(hmLib::signals::connect(signal_, [&](clock::time_point time, double val)->void{this->Voltage = val; this->Time = time; }));
			}
		};
	}
}
#
#endif
