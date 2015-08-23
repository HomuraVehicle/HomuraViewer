#include<thread>
#include"hmLibVer.hpp"
#include"hmrData.hpp"
#include"hmrGPSDataSet.hpp"
#include"hmrDxGPSMap.hpp"
#include<hmLib_v3_04/random.hpp>
#include<hmLib_v2/hmLib.cpp>

class cVirtualGPS{
private:
	std::mutex Mx;
	bool RequestEnd;
	double theta;
private:
	bool can_continue(){
		std::lock_guard<std::mutex> Lock(Mx);
		return !RequestEnd;
	}
public:
	void requestEnd(){
		std::lock_guard<std::mutex> Lock(Mx);
		RequestEnd=true;
	}
private:
	hmLib::pdouble pos;
public:
	boost::signals2::signal<void(const hmr::GPSDataSet&)> signal_newdata;
	boost::signals2::signal<void(double)> signal_newangle;
public:
	cVirtualGPS(hmLib::pdouble pos_):pos(pos_),RequestEnd(false),theta(0.){}
	int operator()(void){
		hmr::clock::time_point Time=hmr::clock::now();

		while(can_continue()){
			if(hmr::clock::now()-Time<std::chrono::seconds(3))continue;
			Time=hmr::clock::now();

			double length=0.00002;
			theta+=hmLib::random::uniform_real(-hmLib::pi()/4,hmLib::pi()/4);
			if(theta<0.)theta+=2*pi();
			if(theta>2*pi())theta-=2*pi();
			pos+=hmLib::pdouble(-length*cos(theta),length*sin(theta));

			hmr::GPSDataSet Data(pos,0.,Time,Time,false,false,1);

			signal_newdata(Data);
			signal_newangle(theta);
		}

		return 0;
	}
};
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow){

	dx::ini("Test",960,720);

	cVirtualGPS VirtualGPS(hmLib::pdouble(139.401416,34.753655));
	std::thread Thread(std::ref(VirtualGPS));

	hmr::dxosGPSMap GPSMap(Pint(480,360));
	GPSMap.slot_addNewData(VirtualGPS.signal_newdata);
	hmLib::inquiries::signaled_contact<double> sigcon_Angle(-1);
	sigcon_Angle.slot(VirtualGPS.signal_newangle);
	sigcon_Angle.conatct(GPSMap.inquiry_getMagneticAngle);

	 PlaySoundFile("hmr/sound/error.wav",DX_PLAYTYPE_LOOP);

	while(!dx::work(30)){
		dx::draw(Pint(100,100),GPSMap);
		if(dx::getKey(KEY::ESC)<0)break;
	}

	VirtualGPS.requestEnd();
	Thread.join();

	dx::end();


	return 0;
}
