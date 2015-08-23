#include"hmLibVer.hpp"
#include"hmrDxCockpit.hpp"
#include<hmLib_v3_04/random.hpp>
#include<hmLib_v2/hmLib.cpp>
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow){
	dx::ini("Test",960,720);

	hmr::dxosDxCockpit DxCockpit;
	DxCockpit.ini();

	hmLib::coordinates3D::polar Accele(1.0,0.21,0.12);
	hmLib::coordinates3D::polar Compass(1.0,0.21,0.12);
	hmLib::inquiries::connect(DxCockpit.inquiry_accele,Accele);
	hmLib::inquiries::connect(DxCockpit.inquiry_compass,Compass);

	while(!dx::work(30)){
		dx::draw(Pint(30,30),DxCockpit);
		if(dx::getKey(KEY::ESC)<0)break;

		Pint pos=dx::getMosPos();
		Accele.phi=(pos.y-360)/360.*pi();
		Accele.theta=(pos.x-480)/480.*pi()/2;
		Compass.phi=(pos.y-360)/360.*pi();
		Compass.theta=(pos.x-480)/480.*pi()/2;
	}

	dx::end();
	return 0;
}
