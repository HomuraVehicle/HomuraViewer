#include<hmLib_v3_06/random.hpp>
#include"hmLibVer.hpp"
#include"hmrGyroLogger.hpp"
#include"hmrDxNavigator.hpp"
#include<hmLib_v2/hmLib.cpp>
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow){
	dx::ini("Test",960,720);

	hmr::cGyroLogger GyroLog;
	boost::signals2::signal<void(hmLib::coordinates3D::angle)> signal_addGyroLog;
	GyroLog.slot_addLog(signal_addGyroLog);

	hmr::dxosNavigator<hmr::cGyroLogger::iterator> DxNavi;
	DxNavi.ini();

	hmLib::coordinates3D::polar Accele(1.0,0.21,0.12);
	hmLib::coordinates3D::polar Compass(1.0,0.21,0.12);
	hmLib::inquiries::connect(DxNavi.inquiry_accele,Accele);
	hmLib::inquiries::connect(DxNavi.inquiry_compass,Compass);
	GyroLog.contact_getLogs(DxNavi.range_inquiry_gyro);



	hmr::clock::time_point Time=hmr::clock::now();
	double Phi=0.;
	while(!dx::work(30)){
		dx::draw(Pint(30,30),DxNavi);
		if(dx::getKey(KEY::ESC)<0)break;

		Pint pos=dx::getMosPos();
		Accele.phi=(pos.y-360)/360.*pi();
		Accele.theta=(pos.x-480)/480.*pi()/2;
		Compass.phi=(pos.y-360)/360.*pi();
		Compass.theta=(pos.x-480)/480.*pi()/2;

		if(hmr::clock::now()-Time>std::chrono::seconds(2)){
			signal_addGyroLog(hmLib::coordinates3D::angle(0,0,Compass.phi-Phi));
			Phi=Compass.phi;
			Time+=std::chrono::seconds(2);
		}
	}

	dx::end();
	return 0;
}
