#include"hmLibVer.hpp"
#include <hmLib_v3_06/config_vc.h> // std::min, std::max　は　windowsマクロでも定義されており、明示的に分離してやらないとだめ
#include <hmLib_v2/dxColorSet.hpp>
//#include"hmrCom.hpp"
#include"hmrComLog.hpp"
//#include"hmrDxCom.hpp"
//#include "hmrIO.hpp" // 
//#include "hmrOperator.hpp" // 
//#include "hmrBufGate.hpp"//
//#include "hmrOperator.hpp"//
//#include "hmrCom.hpp"//
#include "iologgate.hpp"
#include "hmrGateSwitcher.hpp"
#include "hmrDxGateSwitcher.hpp"
#include "hmrDxIO.hpp"
#include "hmrDxIODisplay.hpp"
#include "hmrDxDisplay.hpp"
#include "hmrDxControlMainDisplay.hpp"
#include "hmrDxMUI.hpp"
#include "hmrDXCom.hpp"
#include<hmLib_v2/hmLib.cpp>
#include "hmrDxKeyboard.hpp"
#include"predicate.hpp"
#define HMR_MAIN_INC_END 1
#include "hmrConnectDx.hpp"
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

	dx::ini("Test Dx v2", 960,720);

	try{
		hmr::dxosIOMainPage<contanerItr> IOMainDisp(Pint(720,720), CLR::DarkDullGreen);
		hmr::dxosIOSubPage IOSideDisp(Pint(240,720), CLR::DarkDullGreen);
		// Gate Switcher クラスの定義
		hmr::cGateSwitcher GateSwitcher;
	
		// Data　Buf　のクラス定義
		cDummyPlug_LogData dummyData;


		// Inquirely をコネクト
		dummyData.contact_LogData(IOMainDisp.DxIO.IoLogBufInq);
		IOMainDisp.DxIO.contact_InPaintFlag(IOMainDisp.DxSelectIOPaint.InPaintFlagInq);
		IOMainDisp.DxIO.contact_OutPaintFlag(IOMainDisp.DxSelectIOPaint.OutPaintFlagInq);

		// signal をコネクト
		IOMainDisp.DxIO.slot_setInPaintMode(IOMainDisp.DxSelectIOPaint.signal_require_paintInput);
		IOMainDisp.DxIO.slot_setOutPaintMode(IOMainDisp.DxSelectIOPaint.signal_require_paintOutput);

		// Gate Switcher をコネクト
		hmr::connect(IOSideDisp.DxosGateSwitcher,GateSwitcher);
	
		IOMainDisp.initialize();
		//DxIO.ini();

		hmr::dxosControlMainDisplay ControlMainDisp;

		hmr::dxosMUISideDisplay MUISideDisp;

		hmr::dxosPacketList_withView<hmr::cComLog::iterator> PacketMainDisp(Pint(720,720),360,CLR::DullOrenge);;


		hmr::dxosDisplay Display(Pint(720,720),Pint(240,720));
		Display.registMain(&PacketMainDisp);
		Display.registMain(&ControlMainDisp);
		Display.registMain(&IOMainDisp);
		Display.registSide(&IOSideDisp);
		Display.registSide(&MUISideDisp);
		
		hmr::cDxKeyboard Keyboard;
	
		Display.slot_setMainNo0(Keyboard.signal(hmLib::predicate_and(hmr::is_key_pushed(KEY::NUM1),hmLib::predicate_not(hmr::have_key_pushed(KEY::SHIFT)))));
		Display.slot_setMainNo1(Keyboard.signal(hmLib::predicate_and(hmr::is_key_pushed(KEY::NUM2),hmLib::predicate_not(hmr::have_key_pushed(KEY::SHIFT)))));
		Display.slot_setMainNo2(Keyboard.signal(hmLib::predicate_and(hmr::is_key_pushed(KEY::NUM3),hmLib::predicate_not(hmr::have_key_pushed(KEY::SHIFT)))));
		Display.slot_setSideNo0(Keyboard.signal(hmLib::predicate_and(hmr::is_key_pushed(KEY::NUM1),hmr::have_key_pushed(KEY::SHIFT))));
		Display.slot_setSideNo1(Keyboard.signal(hmLib::predicate_and(hmr::is_key_pushed(KEY::NUM2),hmr::have_key_pushed(KEY::SHIFT))));

		while(!dx::work(30)){

			Keyboard.work();

			dx::draw(Pint(0,0),Display);


			if(dx::getKey(KEY::ESC)<0)break;
		}
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

	dx::end();
	
	return 0;

}
