#ifndef HMR_DXBUI_INC
#define HMR_DXBUI_INC 100
#
#include"hmLibVer.hpp"
#include<hmLib/inquiries.hpp>
#include<hmLib/signals.hpp>
#include<hmLib/exceptions.hpp>
#include<hmLib_v2/dxColorSet.hpp>
#include<hmLib_v2/dxObject.hpp>
#include<hmLib_v2/dxArea.hpp>
#include"DxTools.hpp"
#include"Data.hpp"
namespace hmr{
	struct color_set{
		dxRGB Background;
		dxRGB Thema;
		dxRGB NormalBut;
		dxRGB ActiveBut;
		dxRGB ErrorBut;
		dxRGB InvalidBut;
		dxRGB NormalStr;
		dxRGB ActiveStr;
		dxRGB ErrorStr;
		dxRGB InvalidStr;
		dxRGB PresentStr;
		dxRGB RecentStr;
		dxRGB PastStr;
		color_set(){}
		void set(
			const dxRGB& Background_,
			const dxRGB& Thema_,
			const dxRGB& NormalBut_,
			const dxRGB& ActiveBut_,
			const dxRGB& ErrorBut_,
			const dxRGB& InvalidBut_,
			const dxRGB& NormalStr_,
			const dxRGB& ActiveStr_,
			const dxRGB& ErrorStr_,
			const dxRGB& InvalidStr_,
			const dxRGB& PresentStr_,
			const dxRGB& RecentStr_,
			const dxRGB& PastStr_){
			Background=Background_;
			Thema=Thema_;
			NormalBut=NormalBut_;
			ActiveBut=ActiveBut_;
			ErrorBut=ErrorBut_;
			InvalidBut=InvalidBut_;
			NormalStr=NormalStr_;
			ActiveStr=ActiveStr_;
			ErrorStr=ErrorStr_;
			InvalidStr=InvalidStr_;
			PresentStr=PresentStr_;
			RecentStr=RecentStr_;
			PastStr=PastStr_;
		}
	};
	//サイド画面用共通UIの基底クラス
	class dxosBUI:public dxReObject,public hmoBox{
	public:
		enum status{normal,active,error,invalid};
		enum time{present,recent,past};
		enum object{butobj,strobj};
	private:
		const color_set* pClrSet;
	public:
		void setClrSet(const color_set* pClrSet_){
			pClrSet=pClrSet_;
		}
	private:
		std::string Name;
		Pint NormalSize;
		Pint ExtendSize;
		bool IsExtend;
	public:
		virtual int normal_draw(dxO& dxo)=0;
		virtual int extend_draw(dxO& dxo)=0;
		virtual status getStatus()const=0;
	public:
		Pint getSize()const{return size;}
		bool getExtend()const{return IsExtend;}
		void setExtend(bool Extend_){IsExtend=Extend_;}
		const std::string& name()const{return Name;}
		const dxRGB& getClr(status Sta_,object Obj_)const{
			switch(Obj_){
			case butobj:
				switch(Sta_){
				case normal:
					return pClrSet->NormalBut;
				case active:
					return pClrSet->ActiveBut;
				case error:
					return pClrSet->ErrorBut;
				case invalid:
					return pClrSet->InvalidBut;
				default:
					hmLib_throw(hmLib::exceptions::invalid_request,"Invalid status.");
				}
			case strobj:
				switch(Sta_){
				case normal:
					return pClrSet->NormalStr;
				case active:
					return pClrSet->ActiveStr;
				case error:
					return pClrSet->ErrorStr;
				case invalid:
					return pClrSet->InvalidStr;
				default:
					hmLib_throw(hmLib::exceptions::invalid_request,"Invalid status.");
				}
			default:
				hmLib_throw(hmLib::exceptions::invalid_request,"Invalid object.");
			}
		}
		const dxRGB& getTimeStrClr(hmr::clock::time_point Time_)const{
			switch(getTime(Time_)){
			case present:
				return pClrSet->PresentStr;
			case recent:
				return pClrSet->RecentStr;
			case past:
				return pClrSet->PastStr;
			default:
				hmLib_throw(hmLib::exceptions::invalid_request,"Invalid Time Category");
			}
		}
		time getTime(hmr::clock::time_point Time_)const{
			if(hmr::clock::now()-Time_<std::chrono::seconds(5))return present;
			else if(hmr::clock::now()-Time_<std::chrono::seconds(15))return recent;
			else return past;
		}
		const dxRGB& getBGClr()const{return pClrSet->Background;}
	public:
		void setArea()override{
			if(IsExtend)size=ExtendSize;
			else size=NormalSize;
		}
		int redraw(dxO& dxo)override{
			if(IsExtend){
				return extend_draw(dxo);
			}else{
				return normal_draw(dxo);
			}
		}
	public:
		dxosBUI(const std::string& Name_,int NormalSize_,int ExtendSize_):Name(Name_),NormalSize(220,NormalSize_),ExtendSize(220,ExtendSize_),IsExtend(false),pClrSet(nullptr){}
		virtual ~dxosBUI(){}
	};
	//データの取得時間に応じて表示色を変えるオブジェクト
	class dxoBUITimeStr:public dxReObject,public hmoBox{
	private:
		const dxosBUI* pBUI;
	public:
		std::string Str;
		hmr::clock::time_point Time;
		ALI::ALIGN Ali;
	public:
		dxoBUITimeStr(const dxosBUI* pBUI_,Pint Size_,const std::string& Str_,hmr::clock::time_point Time_,ALI::ALIGN Ali_=ALI::center):pBUI(pBUI_),hmoBox(Size_),Str(Str_),Time(Time_),Ali(Ali_){}
	public:
		int redraw(dxO& dxo)override{
			dxo.draw(Pint(0,0),dxoStrP(size,Str,pBUI->getTimeStrClr(Time),Ali));
			return 0;
		}
	};
	class dxosBUIWaitableBut:public dxReObject,public hmoBox{
	private:
		const dxosBUI* pBUI;
	public:
		boost::signals2::signal<void(void)> Signal;
		hmLib::inquiries::inquiry<bool> Req;
		std::string Str;
	public:
		dxosBUIWaitableBut(const dxosBUI* pBUI_):pBUI(pBUI_){}
		void set(Pint Size_,const std::string& Str_){
			hmoBox::size=Size_;
			Str=Str_;
		}
	public:
		int redraw(dxO& dxo)override{
			bool OnOff_Req=Req();

			if(dxo.draw(Pint(0,0),dxoWaitableBoolBut(size,Str,pBUI->getClr(dxosBUI::normal,dxosBUI::butobj),CLR::White,pBUI->getClr(dxosBUI::active,dxosBUI::butobj),CLR::White,OnOff_Req,false))==1){
				Signal();
			}
			return 0;
		}
	};
	class dxosBUIWaitableBoolBut:public dxReObject,public hmoBox{
	private:
		const dxosBUI* pBUI;
	public:
		boost::signals2::signal<void(bool)> Signal;
		hmLib::inquiries::inquiry<bool> Req;
		hmLib::inquiries::inquiry<bool> Pic;
		std::string Str;
	public:
		dxosBUIWaitableBoolBut(const dxosBUI* pBUI_):pBUI(pBUI_){}
		void set(Pint Size_,const std::string& Str_){
			hmoBox::size=Size_;
			Str=Str_;
		}
	public:
		int redraw(dxO& dxo)override{
			bool OnOff_Req=Req();
			bool OnOff_Pic=Pic();

			if(dxo.draw(Pint(0,0),dxoWaitableBoolBut(size,Str,pBUI->getClr(dxosBUI::normal,dxosBUI::butobj),CLR::White,pBUI->getClr(dxosBUI::active,dxosBUI::butobj),CLR::White,OnOff_Req,OnOff_Pic))==1){
				Signal(!OnOff_Req);
			}
			return 0;
		}
	};
}
#
#endif
