//#include"hmrCom.hpp"
//#include"hmrComLog.hpp"
//#include"hmrDxCom.hpp"
#include <hmLib_v3_06/config_vc.h> // std::min, std::max　は　windowsマクロでも定義されており、明示的に分離してやらないとだめ
#include <string>
#include <vector>
#include "hmrGateSwitcher.hpp"
#include "hmrDxGateSwitcher.hpp"
#include "hmrDxIO.hpp"
#include "iologgate.hpp"
//#include "hmrIO.hpp" // 
//#include "hmrOperator.hpp" // 
//#include "hmrBufGate.hpp"//
//#include "hmrOperator.hpp"//
//#include "hmrCom.hpp"//

#include<hmLib_v2/dxColorSet.hpp>
#include "coordinates.hpp"
#include<hmLib_v2/hmLib.cpp>

#define HMR_MAIN_INC_END 1
#include "hmrConnectDx.hpp"
#include "hmrDxIODisplay.hpp"
//#include "hmrConnectCore.hpp"//

// Test用のメイン関数  
//  
//  
//  
//                     

class cDummyPlug_LogData{
private:
	// IOデータタイプのtypedef 
	typedef std::pair<bool,system_clock_iologtype> io_iologtype;
	typedef std::vector<io_iologtype> container;
	typedef container::const_iterator contanerItr;

	// コンテナ
	container LogBuf;

public:
	// Construct
	cDummyPlug_LogData(){
		io_iologtype LogBufElement;

		for(int i = 0; i<20; i++){
			LogBufElement.first = false;
			LogBufElement.second.Str = "bakayarou soituga Rupan da!!";
			LogBuf.push_back(LogBufElement);

			LogBufElement.first = false;
			LogBufElement.second.Str = "Rupan ha taisetuna mono wo ubatte iki mashita.....";
			LogBuf.push_back(LogBufElement);

			LogBufElement.first = true;
			LogBufElement.second.Str = "iie anokataha nanimo totte ikare masendesitawa.";
			LogBuf.push_back(LogBufElement);

			LogBufElement.first = false;
			LogBufElement.second.Str = "Anata no kokoro desu";
			LogBuf.push_back(LogBufElement);

			LogBufElement.first = true;
			LogBufElement.second.Str = "RURURURURURURURURURURURURURU uuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuu";
			LogBuf.push_back(LogBufElement);
		}
	}

	// Contact関数
	void contact_LogData(hmLib::inquiries::range_inquiry<contanerItr>& RangeInq_){
		hmLib::inquiries::connect(RangeInq_, LogBuf);
	}

};



int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow){
	// IO Logger View 用のテスト関数
	// IOデータタイプのtypedef 
	typedef std::pair<bool,system_clock_iologtype> io_iologtype;
	typedef std::vector<io_iologtype> container;
	typedef container::const_iterator contanerItr;

	/*
	// IO View　クラスの定義
	hmr::dxosIO<contanerItr> DxIO(Pint(640, 600), CLR::DarkDullGreen);
	// IO send box クラスの定義
	hmr::dxosIOSendBox DxSendBox(Pint(640, 30), CLR::DarkDullGreen);
	// IO setFlag クラスの定義
	hmr::dxosSelectIOPaint DxSelectIOPaint(Pint(640, 20), CLR::DarkDullGreen);
	
	// IO Control クラスの定義
	hmr::dxosStartControl DxSendStartButt(Pint(200, 30), CLR::DarkDullGreen, "Send Start");

	hmr::dxosBufControl DxReadBufCtr(Pint(200, 30), CLR::DarkDullGreen, "Read Buf Clear");
	hmr::dxosBufControl DxSendBufCtr(Pint(200, 30), CLR::DarkDullGreen, "Send Buf Clear");

	hmr::dxosTimeIntervalControl DxSendIntervalCtr(Pint(200, 30), CLR::DarkDullGreen, "Send Intr");
	hmr::dxosTimeOutControl DxSendTimeOutCtr(Pint(200, 30), CLR::DarkDullGreen, "Send TimeOut");

	hmr::dxosSyncControl DxSyncCheckBox(Pint(200, 20),CLR::DarkDullGreen); 

	hmr::dxosSelectDuplex DxSelectDuplex(Pint(250, 20),CLR::DarkDullGreen);

	// Gate Switcher クラスの定義
	hmr::cGateSwitcher GateSwitcher;
	hmr::dxosGateSwitcher DxosGateSwitcher(CLR::Cyan);

	// Data　Buf　のクラス定義
	cDummyPlug_LogData dummyData;

	// Inquirely をコネクト
	dummyData.contact_LogData(DxIO.IoLogBufInq);
	DxIO.contact_InPaintFlag(DxSelectIOPaint.InPaintFlagInq);
	DxIO.contact_OutPaintFlag(DxSelectIOPaint.OutPaintFlagInq);

	// signal をコネクト
	DxIO.slot_setInPaintMode(DxSelectIOPaint.signal_require_paintInput);
	DxIO.slot_setOutPaintMode(DxSelectIOPaint.signal_require_paintOutput);

	// Gate Switcher をコネクト
	hmr::connect(DxosGateSwitcher,GateSwitcher);
	*/

	hmr::dxosIOMainPage<contanerItr> IOMain(Pint(720,720), CLR::DarkDullGreen);
	hmr::dxosIOSubPage IOSub(Pint(240,720), CLR::DarkDullGreen);
	// Gate Switcher クラスの定義
	hmr::cGateSwitcher GateSwitcher;
	
	// Data　Buf　のクラス定義
	cDummyPlug_LogData dummyData;


	// Inquirely をコネクト
	dummyData.contact_LogData(IOMain.DxIO.IoLogBufInq);
	IOMain.DxIO.contact_InPaintFlag(IOMain.DxSelectIOPaint.InPaintFlagInq);
	IOMain.DxIO.contact_OutPaintFlag(IOMain.DxSelectIOPaint.OutPaintFlagInq);

	// signal をコネクト
	IOMain.DxIO.slot_setInPaintMode(IOMain.DxSelectIOPaint.signal_require_paintInput);
	IOMain.DxIO.slot_setOutPaintMode(IOMain.DxSelectIOPaint.signal_require_paintOutput);

	// Gate Switcher をコネクト
	hmr::connect(IOSub.DxosGateSwitcher,GateSwitcher);
	
	dx::ini("Test ambe", 960,720);
	IOMain.initialize();
	//DxIO.ini();

//try{

	/*
	// coordinate のテスト
	using namespace hmLib::coordinates3D;
	double PI = atan(1) * 4.;
	transfer test = transfer(position(3, 0, 0)) *  transfer(angle( PI / 6., 0., PI/3.)) * transfer(angle( 0., PI/4., 0.));
	angle testAng;
	position testPos(1, 0, 0);
	testPos = test * testPos;

	test.getAngleAndPosition(testAng, testPos);
	angle testAng2 = ~testAng;
	transfer tet(testAng2);
	transfer s = tet * test;

	angle ang = coordinate_transfer(position(1,0,0), position(1,1,0));
	angle ang1 = coordinate_transfer(position(0,1,0), position(1,1,0));
	angle ang2 = coordinate_transfer(position(1,1,0), position(1,2,0));
	
	polar polar(testPos);
	*/


	while(!dx::work(30)){
		dx::draw(Pint(270,10),IOMain);
		dx::draw(Pint(10, 10), IOSub);


		if(dx::getKey(KEY::ESC)<0)break;
	}
	/*
}catch(const std::exception& Escp){
	printfDx("===ERROR===\n");
	printfDx("%s\n",Escp.what());
	while(!dx::work(30)){
		if(dx::getKey(KEY::ESC)<0)break;
	}
}catch(const std::string& Escp){
	printfDx("===ERROR===\n");
	printfDx("%s\n",Escp.c_str());
	while(!dx::work(30)){
		if(dx::getKey(KEY::ESC)<0)break;
	}
}
*/
	dx::end();
	
/*
	//  Gate switcher 用の　テスト関数
	//  
	hmr::cGateSwitcher GateSwitcher;
	hmr::dxosGateSwitcher DxosGateSwitcher(CLR::Cyan);

	hmr::connect(DxosGateSwitcher,GateSwitcher);

	dx::ini("Test",960,720);
	while(!dx::work(30)){
		dx::draw(Pint(30,30), DxosGateSwitcher);
		if(dx::getKey(KEY::ESC)<0)break;
	}
	dx::end();
*/

	return 0;

}