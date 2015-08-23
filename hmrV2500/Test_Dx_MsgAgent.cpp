#include<fstream>
#include"hmrCom.hpp"
#include"hmrComLog.hpp"
#include"hmrDxCom.hpp"
#include"hmrConnect.hpp"
//#include"hmrDxSpriteMsgAgentUI.hpp"
#include"hmrDxThermoMUI.hpp"
#include"hmrSprite.hpp"
#include<hmLib_v3_06/random.hpp>
#include<hmLib_v3_06/hmLib.cpp>

void slot_debugsignal(const std::string& Str_){
	printfDx("Signaled %d\n",Str_.size());
}
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow){
	boost::signals2::signal<void(const std::string&)> DebugSignal;
	DebugSignal.connect(slot_debugsignal);

	std::ifstream fin("hmr/SpriteDebug.jpg",std::ios::binary|std::ios::in);
	fin.seekg(0,std::ios::end);
	unsigned int Size=fin.tellg();
	fin.seekg(0,std::ios::beg);
	std::string str;
	for(unsigned int Cnt=0;Cnt<Size;++Cnt)str.push_back(fin.get());

	hmr::dxosSprite DxosSprite(Pint(480,320));
	hmr::cSpriteMsgAgent SpriteMsgAgent;
	DxosSprite.slot_setNewGraph(SpriteMsgAgent.signal_setPicture);
	DxosSprite.slot_setNewGraph(DebugSignal);


	dx::ini("Test",960,720);
	while(!dx::work(30)){
		dx::draw(Pint(30,30),DxosSprite);
		if(dx::getKey(KEY::ESC)<0)break;

		if(dx::getKey(KEY::SPACE)<0)DebugSignal(str);
	}
	dx::end();
	return 0;
}
