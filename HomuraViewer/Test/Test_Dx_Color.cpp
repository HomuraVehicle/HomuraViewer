#include<fstream>
#include"hmLibVer.hpp"
#include<hmLib_v2/dxColorSet.hpp>
#include<hmLib_v2/dxObject.hpp>
#include<hmLib_v2/hmLib.cpp>
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow){
	dxRGB Color;
	Color=CLR::Red;

	dx::ini("Test",960,720);
	while(!dx::work(30)){
		if(dx::getKey(KEY::ESC)<0)break;
		dx::draw(Pint(100,100),dxoBox(Pint(300,400),Color));
	}
	dx::end();
	return 0;
}
