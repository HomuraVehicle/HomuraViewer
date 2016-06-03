#ifndef HMR_TEST_COMCORE_INC
#define HMR_TEST_COMCORE_INC 100
#
/*===Test_ComCore===
通信系オブジェクトのテスト用ファイル
注意　このプログラムは、COM9を開いている
相手がいないのに開くと、しぬ
*/
#include"hmLibVer.hpp"
#include<iostream>
#include<hmLib_v3_06/config_vc.h>
#include<hmLib_v3_06/bufgate.hpp>
//#include<hmLib_v3_06/any_iterator.hpp>
#include"dxSignalBut.hpp"
#include"iologgate.hpp"
#include"hmrGateSwitcher.hpp"
#include"hmrIO.hpp"
#include"hmrCom.hpp"
#include"hmrOperator.hpp"
#include"hmrMessage.hpp"
#include<fstream>
#include"hmrCom.hpp"
#include"hmrComLog.hpp"
#include"hmrDxCom.hpp"
#include"hmrConnect.hpp"
#include"hmrDxSprite.hpp"
#include"hmrDxThermoMsgAgentUI.hpp"
#include"hmrSprite.hpp"
#include<hmLib_v3_06/random.hpp>
#include<hmLib_v2/hmLib.cpp>

using namespace hmr;
using namespace std;
class console_gate:public hmLib::gate{
public:
	//受信可能状態かの確認
	bool can_get(){return true;}
	//受信可能データの有無
	bool empty(){return false;}
	//複数byte受信　受信文字アドレスと、受信文字数が引数　実際の受信文字数が戻り値
	size_type get(char* str_,const size_type& size_){std::cin.read(str_,size_);return size_;}
	//送信可能状態かの確認
	bool can_put(){return true;}
	//送信可能データの有無
	bool full(){return false;}
	//複数byte送信　送信文字アドレスと、送信文字数が引数　実際の送信文字数が戻り値
	size_type put(const char* str_,const size_type& size_){std::cout.write(str_,size_);return size_;}
};
class TestMessageAgent:public hmr::itfMessageAgent{
	bool Send;
	bool Ack;
	bool SendMode;
public:
	TestMessageAgent():Send(false),Ack(false),SendMode(false){}
	bool listen(datum::time_point Time_, bool Err_,const std::string& Str_)override{
		std::cout<<"--listen:"<<Time_.time_since_epoch().count()<<":"<<Err_<<":"<<":"<<Str_<<std::endl;
		SendMode=!SendMode;
		Ack=true;
		return false;
	}
	void setup_talk(void)override{
		Send=SendMode;
	}
	bool talk(std::string& Str_)override{
		if(Send){
			std::cout<<"--talk:send"<<std::endl;
			Str_="return_test";
			Send=false;
			return false;
		}else if(Ack){
			std::cout<<"--talk:ack"<<std::endl;
			Str_="Ack";
			Ack=false;
			return false;
		}
		return true;
	}
};
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow){
//	hmr::cGateSwitcher GateSwitcher;
//	console_gate cgate;
//	hmLib::bufgate cgate;

	dx::ini("Test",960,720);

	try{
		hmr::cGateSwitcher GateSW;

		hmr::cCom Com;
		hmr::cCom::VMC1Creater<1> ComVMC(&Com);

		hmr::cMessage Message;
		hmr::cFullRecvTimeSendOperator Operator(&Message,&Com,std::chrono::seconds(2));
		hmr::cFdxIO IO(ComVMC);
		iologgate<fdx_crlf_timeout_iologger<system_clock_iologtype>> ioLogGate;
		ioLogGate.open(GateSW);
		IO.open(&ioLogGate);
		GateSW.ComSetting.Baudrate=9600;
		GateSW.ComSetting.PortNo=9;
		GateSW.open(GateSW.ComGateType);

		fdx_vector_iologbuf<system_clock_iologtype> LogBuf;
		LogBuf.slot_pushOutLogBuf(ioLogGate.Logger.signal_resetOutLog);
		LogBuf.slot_pushInLogBuf(ioLogGate.Logger.signal_resetInLog);

	//	hmLib::inquiries::range_inquiry<hmLib::const_random_access_any_iterator<system_clock_iologtype>> RangeInq;
	//	LogBuf.contactLogBuf(RangeInq);

	//	TestMessageAgent Test;
	//	Message.regist('Q',&Test);

		std::cout<<"---testCoCore---"<<std::endl;

		hmr::cSpriteMsgAgent SpriteMsgAgent;
		Message.regist('j',&SpriteMsgAgent);

		hmr::dxosSprite DxosSprite(Pint(480,320));
		hmr::dxoThermoMsgAgentUI DxThermoMsgAgentUI;
		hmr::dxoThermoMsgAgentUI DxThermoMsgAgentUI2;
	//	DxosSprite.slot_setNewGraph(SpriteMsgAgent.signal_setPicture);
	//	DxosSprite.slot_setNewGraph(DebugSignal);


		while(!dx::work(30)){
			if(dx::getKey(KEY::ESC)<0)break;
//			dx::draw(Pint(30,30),DxosSprite);
			Pint p1,p2;
			
			DxThermoMsgAgentUI.setArea();
			dx::draw(Pint(0,0),dxoBox(DxThermoMsgAgentUI.getSize(),CLR::DarkSoftRed));
			dx::draw(Pint(0,0),DxThermoMsgAgentUI);

			DxThermoMsgAgentUI2.setArea();
			dx::draw(Pint(0,DxThermoMsgAgentUI.getSize().y),dxoBox(DxThermoMsgAgentUI2.getSize(),CLR::DarkSoftRed));
			dx::draw(Pint(0,DxThermoMsgAgentUI.getSize().y),DxThermoMsgAgentUI2);
	//		if(dx::getKey(KEY::SPACE)<0)DebugSignal(str);
			IO.work();
			Operator();
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
#endif
