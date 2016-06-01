#ifndef FOMAGATE_INC
#define FOMAGATE_INC 102
#
/*===fomagate===
foma接続用の簡易ゲート、fomagateと、
comを使ったfoma接続用ゲート、fomacom_gate

fomagate:v1_02/130324 hmIto
	名前をfoma_gateからfomagateに変更
fomagate:v1_01/130310 hmIto
	hmLib_v3_04に更新
	fomacom_gate追加
*/
#include <string>
#include <Windows.h>
#include <hmLib/comgate.hpp>
#include <hmLib/gate.hpp>
class fomagate:public hmLib::gate{
private:
	hmLib::gate* pGate;
public:
	fomagate():pGate(0){}
	bool open(hmLib::gate& rGate_,const std::string& foma_number){
		if(is_open())return true;

		pGate=&rGate_;

		hmLib::ogatestream gout(*pGate);

		/*相手の電話番号へ発信*/
		gout<<"ATD"<<foma_number<<static_cast<unsigned char>(0x0d)<<static_cast<unsigned char>(0x0a);

		return false;
	}
	bool close(){
		if(!is_open())return true;

		hmLib::gatestream gout(*pGate);

		Sleep(1000);

		gout<<"+++"<<static_cast<unsigned char>(0x0d)<<static_cast<unsigned char>(0x0a);

		Sleep(1000);

		gout<<"ATH"<<static_cast<unsigned char>(0x0d)<<static_cast<unsigned char>(0x0a);

		pGate=0;

		return false;
	}
	bool is_open(){return pGate!=0;}
public:
	//受信が続いているかどうかの確認
	virtual bool flowing()override{ return pGate->flowing(); }
	//受信可能かの確認
	virtual bool can_getc()override{return pGate->can_getc();}
	//複数byte受信　受信文字アドレスと、受信文字数が引数　実際の受信文字数が戻り値
	virtual size_type gets(char* str_,const size_type& size_)override{return pGate->gets(str_,size_);}
	// 単数バイト受信
	virtual char getc()override{ return pGate->getc(); }

	//送信可能状態かの確認
	virtual bool can_putc()override{return pGate->can_putc();}
	//無理やり送る
	virtual void flush()override{return pGate->flush();}
	//複数byte送信　送信文字アドレスと、送信文字数が引数　実際の送信文字数が戻り値
	virtual size_type puts(const char* str_,const size_type& size_)override{return pGate->puts(str_,size_);}
	// 単バイト送信
	virtual void putc(char c)override{ return pGate->putc(c); }

};
class fomacom_gate:public hmLib::gate{
private:
	hmLib::comgate ComGate;
	fomagate FomaGate;
public:
	bool open(int PortNo_,int bps_,const std::string& FomaNumber_){
		if(is_open())return true;

		if(ComGate.open(PortNo_,bps_))return true;
		
		if(FomaGate.open(ComGate,FomaNumber_)){
			ComGate.close();
			return true;
		}

		return false;
	}
	bool close(){
		if(!is_open())return true;

		FomaGate.close();

		ComGate.close();

		return false;
	}
	bool is_open(){return FomaGate.is_open();}
public:
	//受信可能データの有無
	virtual bool can_getc()override{return FomaGate.can_getc();}
	//受信が続いているか否か
	virtual bool flowing()override{return FomaGate.flowing();}
	//複数byte受信　受信文字アドレスと、受信文字数が引数　実際の受信文字数が戻り値
	virtual size_type gets(char* str_,const size_type& size_)override{return FomaGate.gets(str_,size_);}
	// 単バイト受信
	virtual char getc()override{ return FomaGate.getc(); }

	//送信可能データがあるかの確認
	virtual bool can_putc()override{return FomaGate.can_putc();}
	// 無理やり送る
	virtual void flush()override{return FomaGate.flush();}
	//複数byte送信　送信文字アドレスと、送信文字数が引数　実際の送信文字数が戻り値
	virtual size_type puts(const char* str_,const size_type& size_)override{return FomaGate.puts(str_,size_);}
	// 単バイト送信
	virtual void putc(char c)override{ return FomaGate.putc(c); }

};
#
#endif
