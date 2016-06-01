#include"hmrDxCockpit.hpp"
#include"hmrConnect.hpp"
#include<hmLib_v3_06/random.hpp>
#include<hmLib_v2/hmLib.cpp>
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow){
	hmr::cCom Com;

	hmr::cComLog ComLog;
	hmr::connect(ComLog,Com);

	hmr::dxosPacketList_withView<hmr::cComLog::iterator> DxosPacketList(Pint(480,640),320,CLR::Yellow);
	hmr::connect(DxosPacketList,Com,ComLog);

	hmr::clock::time_point Time=hmr::clock::now();

	for(unsigned int PacketCnt=0;PacketCnt<32;++PacketCnt){
		hmr::packet Pac;

		unsigned int DatumNum=hmLib::random::uniform_int(1,8);
		for(unsigned int DatumCnt=0;DatumCnt<DatumNum;++DatumCnt){
			hmr::datum Dat;
			Dat.ErrFlag=false;
			Dat.ID='A'+hmLib::random::uniform_int(0,5);
			Dat.Size=hmLib::random::uniform_int(1,150);
			for(unsigned int StrCnt=0;StrCnt<Dat.Size;++StrCnt){
				Dat.Str.push_back('a'+(StrCnt%23));
			}
			Time+=std::chrono::milliseconds(hmLib::random::uniform_int(1,10000));
			Dat.Time=Time;
			Pac.Data.push_back(Dat);
		}
		Pac.ErrFlag=false;
		Time+=std::chrono::milliseconds(hmLib::random::uniform_int(1,10000));
		Pac.Time=Time;
		ComLog.push(hmLib::random::uniform_bool(),Pac);
	}

	dx::ini("Test",960,720);
	while(!dx::work(30)){
		dx::draw(Pint(30,30),DxosPacketList);
		if(dx::getKey(KEY::ESC)<0)break;
	}
	dx::end();
	return 0;
}
