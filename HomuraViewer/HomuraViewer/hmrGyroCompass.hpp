#ifndef HMR_GYROCOMPASS_INC
#define HMR_GYROCOMPASS_INC
#
#include<boost/signals2/signal.hpp>
#include<hmLib/signals.hpp>
#include<hmLib/inquiries.hpp>
#include <hmLib/coordinates.hpp>
#include <hmrVLib/Data.hpp>
namespace hmr{
	class cGyroCompass{
//		typedef hmLib::coordinates3D::polar polar;
		typedef hmLib::coordinates3D::angle angle;
	public:
		angle Angle;
	private:
		void addAngle(const angle& Angle_){Angle&=Angle_;}
		void clearAngle(){Angle.roll=0;Angle.pitch=0;Angle.yaw=0;}
	public:
		angle getAngle()const{return Angle;}
	private:
		hmLib::signals::unique_connections SignalConnections;
		hmLib::inquiries::unique_connections InquiryConnections;
	public:
		void slot_addAngle(boost::signals2::signal<void(angle, double, clock::time_point)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](const angle& Angle_,double dt_, clock::time_point)->void{this->addAngle(Angle_*dt_);}));
		}
		void slot_clearAngle(boost::signals2::signal<void(void)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](void)->void{this->clearAngle();}));
		}
		void inquiry_getAngle(hmLib::inquiries::inquiry<angle>& Inq_){
			InquiryConnections(hmLib::inquiries::connect(Inq_,[&](void)->angle{return this->getAngle();}));
		}
	};
}
#
#endif
