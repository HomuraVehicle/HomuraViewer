#ifndef HMR_DXIO2_INC
#define HMR_DXIO2_INC 103

#include <hmLib/config_vc.h> // std::min, std::max　は　windowsマクロでも定義されており、明示的に分離してやらないとだめ

#include <iostream>
#include <string>
#include <algorithm>
#include <boost/signals2.hpp>

#include "hmLibVer.hpp"
//#include "iologgate.hpp"
#include <hmLib_v2/hmBasic.hpp>
#include <hmLib_v2/dxObject.hpp>
#include <hmLib_v2/dxArea.hpp>
#include <hmLib_v2/dxColorSet.hpp>
#include <hmLib/signals.hpp>
#include <hmLib/inquiries/inquiry.hpp>
#include <hmLib/inquiries/range_inquiry.hpp>

#include <hmLib_v2/hmBin_To_hmAsc.hpp>
#include "dxSignalBut.hpp"
#include <HomuraViewer/Data.hpp>
#include "iologgate.hpp"

/* ---- hmrDxIO-----

考えられる使用方法
	１、データのIN,OUTが時系列でまとまって表示できればよい
		来るデータには時間と、データを表示する

	２、全二重の時に、送信、受信の窓を分けたいことがあるだろう
		窓は別にできるように作る

	３、バッファの状態を確認したい
		送れている数、遅れていない数をきちんと確認する

	４、無理やり手打ちで送信できる
		手打ちで打てる画面を作る

	５、送信データを止める、間隔を変更したいことがあるだろう
		送信間隔を決めるためのチェックボックス
		
	６、デバッグ時に受信データを止めたい時があるだろう
		通信コネクトボタンで対応

	７、送受信をセットで止めたいこともあるだろう
		送受信ストップボタンを実装


	IO関連の描画クラス
	これは機能によって分けてある
	IO関連の機能のクラスをここにぶち込む

	こいつらそれぞれの要素の場所は、別のクラスによって設定する。
	シグナル、スロットに対応したDXクラスを作る

hmrDxIO.hpp :v1_03/130519 amby
	描画部分を変更
	
	のこり
	　TimeOutなどのコントロール -> OK
	　Bufのコントロール -> OK
	
	一度すべてつなげてみる必要があるよ．。。。
	なお、DEBUGと書いてる部分はInquirely の　つなげていないところとなっている

hmrDxIO.hpp :v1_02/130427 amby
	描画部分を作成
	機能関連を実装した

	残るタスク
	　Signal などの接続
	　スペースを有効活用
	　バッファの表示方法について　固定で表示できたりするか	 　　
	 

hmrDxIO.hpp :v1_01/130420 amby
	描画部分を作成
	Frame を作成　Fn で読むようにした 

	描画にバグが残る。
	おそらく、いてれーた関連だと思われるから、バグ鳥を行う。



hmrDxIO.hpp :v1_00/130406 amby
	初期バージョン
	既存のクラスをラッピングする形で対応する

	以下についてこのファイルで実装
	dxIOSendBox 
	dxfIOSta
	dxfIOBufSta
*/

namespace hmr{
	//  コマンド打ち込む用窓
	class dxosIOSendBox2:public dxReObject,public hmoBox{
	private:
		dxRGB Clr;
		dxInput Inp;
		//Pint inpBoxSize;

	// signal 実装
	public:
		// 送信文字列送信 シグナル
		boost::signals2::signal<void(std::string)> signal_poutString;

	public:
		//dxfIOLog::cInputSetter getInpSetter(){return dxfIOLog::cInputSetter(Inp);}
	public:
		dxosIOSendBox2(Pint Size_,const dxColor& Clr_)
			:hmoBox(Size_)
			,Clr(Clr_){
			Inp.make(256,1,0,0," please input a string");
		}
	protected:
		virtual int redraw(dxO& dxo){
			
			// InpBox　の　描画
			if(dxo.draw(Pint(0,0),dxoInpBox(size,Inp,Clr))==2){
				// signal 発信
				signal_poutString(hm::hmAscToBin(Inp.getStr()));
				//rIO.pout<<hm::hmAscToBin(Inp.getStr());
				Inp.setStr("");
				Inp.inactive();
			}
			return 0;
		}
	};
	//最新データの描画にも対応したバージョン
	template<typename iterator>
	class dxFnIOLog2:public dxFn{
		typedef std::pair<packet::no_type,datum::no_type> seltype;
	private:
		iterator Begin;									//PacketLogの先頭
		iterator End;									//PacketLogの末尾
		const std::string* pInStr;						//最新入力Packet
		const std::string* pOutStr;						//最新出力Packet
		const dxRGB& InputClr;							//入力側の色
		const dxRGB& OutputClr;							//出力側の色
		bool InPaintFlag;								//Inデータを表示するかどうか
		bool OutPaintFlag;								//Outデータを表示するかどうか
	public:
		dxFnIOLog2(iterator Begin_,iterator End_,const std::string* pInStr_,const std::string* pOutStr_,bool InPaintFlag_,bool OutPaintFlag_,const dxRGB& InputClr_,const dxRGB& OutputClr_)
			:Begin(std::move(Begin_))
			,End(std::move(End_))
			,pInStr(pInStr_)
			,pOutStr(pOutStr_)
			,InPaintFlag(InPaintFlag_)
			,OutPaintFlag(OutPaintFlag_)
			,InputClr(InputClr_)
			,OutputClr(OutputClr_){
		}
		int fndraw(dxO& dxo){
			/*ScrlFrame限定*/
			try{
				//描画位置確定
				dxO::Acs<dxScrlFrame> Acs(dxo);

				//IOの両方を描画する場合
				if(InPaintFlag && OutPaintFlag){
					int ViewPos=Acs().viewpos.y;
					int ViewSize=Acs().size.y;

					//描画範囲に含まれるdataのiteratorを計算
					auto DBegin=Begin+std::min(std::max(0,(-ViewPos-ViewSize)/45-1),End-Begin);
					auto DEnd=Begin+std::min(std::max(0,(-ViewPos)/45+1),End-Begin);

					//最下部のデータの描画位置となるように初期地設定
					int DrawPos=-45*((DBegin-Begin)+1);

					//描画位置のみ描画
					for(auto pitr=DBegin;pitr!=DEnd;++pitr){
						dxRGB Clr;
						if(pitr->first)Clr=InputClr;
						else Clr=OutputClr;

						int DrawXPos=125;

						//時刻描画
						dxo.draw(Pint(0,DrawPos),dxoStrP(Pint(120,45),hmr::time_to_hms(pitr->second.EndTime),CLR::LightGray));

						//Packet内データを順に描画
						for(auto itr=pitr->second.Str.begin();itr!=pitr->second.Str.end();++itr){
							dxo.draw(Pint(DrawXPos, DrawPos),dxoStrP(Pint(24,20),hmstd::fStr("%c",*itr),Clr));
							dxo.draw(Pint(DrawXPos, DrawPos+20),dxoStrP(Pint(24,20),hmstd::fStr("%02x",static_cast<unsigned char>(*itr)),Clr));
							DrawXPos+=24;
						}

						DrawPos-=45;
					}

					//描画位置を本来の位置に移動
					DrawPos=-45*((End-Begin)+1);

					//OutPacket内
					if(pOutStr!=nullptr){
						//描画範囲内の時
						if(ViewPos<=DrawPos+40 && DrawPos<ViewPos+ViewSize){
							dxo.draw(Pint(0,DrawPos),dxoTriangle(Pint(0,40),Pint(10,20),OutputClr));
							int DrawXPos=125;
							//Packet内データを順に描画
							for(auto itr=pOutStr->begin();itr!=pOutStr->end();++itr){
								dxo.draw(Pint(DrawXPos, DrawPos),dxoStrP(Pint(24,20),hmstd::fStr("%c",*itr),OutputClr));
								dxo.draw(Pint(DrawXPos, DrawPos+20),dxoStrP(Pint(24,20),hmstd::fStr("%02x",static_cast<unsigned char>(*itr)),OutputClr));
								DrawXPos+=24;
							}
						}
						DrawPos-=45;
					}

					//InPacket内
					if(pInStr!=nullptr){
						//描画範囲内の時
						if(ViewPos<=DrawPos+40 && DrawPos<ViewPos+ViewSize){
							dxo.draw(Pint(0,DrawPos),dxoTriangle(Pint(0,40),Pint(10,20),InputClr));
							int DrawXPos=125;
							//Packet内データを順に描画
							for(auto itr=pInStr->begin();itr!=pInStr->end();++itr){
								dxo.draw(Pint(DrawXPos, DrawPos),dxoStrP(Pint(24,20),hmstd::fStr("%c",*itr),InputClr));
								dxo.draw(Pint(DrawXPos, DrawPos+20),dxoStrP(Pint(24,20),hmstd::fStr("%02x",static_cast<unsigned char>(*itr)),InputClr));
								DrawXPos+=24;
							}
						}
						DrawPos-=45;
					}

					//最後に描画位置の指定を行っておく
					dxo.draw(Pint(0,0),dxoNull());
					dxo.draw(Pint(0,DrawPos+45),dxoNull());
				}//IDが選択されている場合
				else if(InPaintFlag || OutPaintFlag){
					//描画範囲を取得
					int ViewPos=Acs().viewpos.y;
					int ViewSize=Acs().size.y;
					//描画位置が不明なんで、初期地からスタート
					int DrawPos=-45;

					//総当たりで調べていく
					for(auto pitr=Begin;pitr!=End;++pitr){
						dxRGB Clr;
						if(pitr->first)Clr=InputClr;
						else Clr=OutputClr;

						int DrawXPos=125;

						bool DrawFlag=false;
						bool ESCFlag=false;

						//描画対象の場合
						if((pitr->first&&InPaintFlag)||((!pitr->first)&&OutPaintFlag)){
							//時刻描画
							dxo.draw(Pint(0,DrawPos),dxoStrP(Pint(120,45),hmr::time_to_hms(pitr->second.EndTime),CLR::LightGray));

							//描画範囲に入っている時だけ描画
							if(ViewPos-40<DrawPos && DrawPos<ViewPos+ViewSize){
								//Packet内データを順に描画
								for(auto itr=pitr->second.Str.begin();itr!=pitr->second.Str.end();++itr){
									dxo.draw(Pint(DrawXPos, DrawPos),dxoStrP(Pint(24,20),hmstd::fStr("%c",*itr),Clr));
									dxo.draw(Pint(DrawXPos, DrawPos+20),dxoStrP(Pint(24,20),hmstd::fStr("%02x",static_cast<unsigned char>(*itr)),Clr));
									DrawXPos+=24;
								}
							}

							//描画したかどうかに関わらず、位置情報だけは更新しておく
							DrawPos-=45;
						}
					}

					//最後に描画位置の指定を行っておく
					dxo.draw(Pint(0,0),dxoNull());
					dxo.draw(Pint(0,DrawPos+45),dxoNull());
				}//描画すべきデータがない場合
				else{
					dxo.draw(Pint(5,10), dxoStr("No Data"));
				}


			}catch(const std::exception& Except){
				dxo.draw(Pint(5, 5),dxoStr("***ERROR***"));
				dxo.draw(Pint(5,25),dxoStr(Except.what()));
			}catch(const std::string& Str){
				dxo.draw(Pint(5, 5),dxoStr("***ERROR***"));
				dxo.draw(Pint(5,25),dxoStr(Str));
			}

			return 0;
		}
	};
	template<typename iterator>
	class dxosIO2:public dxReObject,public hmoBox{
	private:
		// Frame描画用関数
		//dxFnIOLog FnIOLog;
		// Frame 
		dxScrlFrame IOLogFrame;

	private:
		dxosIOSendBox2 IOSendBox;
	public:
		boost::signals2::signal<void(std::string)>& signal_poutString;
	private:
		// Color 用の変数
		dxRGB FrameClr;
		dxRGB Clr;
		dxRGB InClr;
		dxRGB OutClr;
		bool InPaintFlag, OutPaintFlag;					// 実際にはこのフラグはこのクラスで管理する
		bool LinkNewStr;								//最新データを常に最上部に表示するよう、表示位置を固定するかどうか？

	// Inquiry 系統の宣言
	private:
		// Inquiry用の型変換
		typedef hmLib::inquiries::range_inquiry<iterator> iologbuf_inquiry;
		typedef hmLib::inquiries::inquiry<const system_clock_iologtype&> iolog_inquiry;
	public:
		//ioLogバッファへのアクセス用
		iologbuf_inquiry inquiry_LogBuf;
		iolog_inquiry inquiry_InStr;
		iolog_inquiry inquiry_OutStr;
		// contact 系統
		void contact_InPaintFlag(hmLib::inquiries::inquiry<bool> &inq_){
			hmLib::inquiries::connect(inq_, InPaintFlag);
		}
		void contact_OutPaintFlag(hmLib::inquiries::inquiry<bool> &inq_){
			hmLib::inquiries::connect(inq_, OutPaintFlag);
		}
	private:
		hmLib::signals::unique_connections connections;
	public:
		// slot 用の関数
		void setInPaintMode(bool InPaintFlag_){
			InPaintFlag = InPaintFlag_;
		}
		void setOutPaintMode(bool OutPaintFlag_){
			OutPaintFlag = OutPaintFlag_;
		}
		// slot 設定関数
		void slot_setInPaintMode(boost::signals2::signal<void(bool)> &sig_){
			connections(sig_.connect([&](bool v1)->void{this->setInPaintMode(v1);}));
		}
		void slot_setOutPaintMode(boost::signals2::signal<void(bool)> &sig_){
			connections(sig_.connect([&](bool v1)->void{this->setOutPaintMode(v1);}));
		}
	protected:
		//virtual int redraw(dxO& dxf);
		virtual int redraw(dxO& dxo){
			try{
				//ボタン表示
				if(dxo.draw(Pint(0,0),dxoButIO(Pint(60,30),"Sync",Clr,LinkNewStr))==1){
					LinkNewStr=!LinkNewStr;
				}
				//ボタン表示
				if(dxo.draw(Pint(60,0),dxoButIO(Pint(40,30),"IN",InClr,InPaintFlag))==1){
					InPaintFlag=!InPaintFlag;
				}
				//ボタン表示
				if(dxo.draw(Pint(100,0),dxoButIO(Pint(40,30),"OUT",OutClr,OutPaintFlag))==1){
					OutPaintFlag=!OutPaintFlag;
				}
				dxo.draw(Pint(140,0),IOSendBox);


				//最新Packetを取得
				const std::string* pInStr=nullptr;
				if(inquiry_InStr.is_connect()){
					if(inquiry_InStr().Str.size())pInStr=&(inquiry_InStr().Str);
				}
				const std::string* pOutStr=nullptr;
				if(inquiry_OutStr.is_connect()){
					if(inquiry_OutStr().Str.size())pOutStr=&(inquiry_OutStr().Str);
				}

				//パケット描画の設定
				auto Begin=inquiry_LogBuf.begin();
				auto End=inquiry_LogBuf.end();
				dxFnIOLog2<iterator> FnIOLog(Begin,End,pInStr,pOutStr,InPaintFlag,OutPaintFlag,InClr,OutClr);
				IOLogFrame.set(FnIOLog,size-Pint(0,30),FrameClr);
				IOLogFrame.resetMoveArea(ALI::all);

				//Linkフラグがたっている場合、描画位置を修正
				if(LinkNewStr){
					IOLogFrame.viewpos.y=-45*(End-Begin);
					if(pOutStr!=nullptr)IOLogFrame.viewpos.y-=45;
					if(pInStr!=nullptr)IOLogFrame.viewpos.y-=45;
				}
				Pint MPos=dxo.GtoD(dx::getMosPos());

				if(IOLogFrame.pIn(Pint(0,0),MPos)){
					IOLogFrame.viewpos.y-=45*dx::getMosH()*(dx::getKey(KEY::SHIFT)>0?3:1);
				}

				//描画
				dxo.draw(Pint(0,30),IOLogFrame);
			}catch(const hmLib::inquiries::unconnected_exception& Excp){
				// Inquire でのエラー
				dxo.draw(Pint(0,0), dxoStr("Inquiry:: データバッファ取得に失敗"));
				dxo.draw(Pint(0,30), dxoStr(Excp.what()));
				return 1;
			}catch(const hmLib::exceptions::exception& Excp){
				// その他でのエラー
				dxo.draw(Pint(0,0), dxoStr("エラー発生"));
				dxo.draw(Pint(0,30), dxoStr(Excp.what()));
				return 1;
			}
			
			return 0;
		}
	public:
		dxosIO2(Pint size_, const dxRGB& FrameClr_,const dxRGB& Clr_,const dxRGB& InClr_,const dxRGB& OutClr_)
			:hmoBox(size_)
			,IOSendBox(Pint(580,30),Clr_)
			,signal_poutString(IOSendBox.signal_poutString)
			,InPaintFlag(true)
			,OutPaintFlag(true)
			,FrameClr(FrameClr_)
			,Clr(Clr_)
			,InClr(InClr_)
			,OutClr(OutClr_){

			IOLogFrame.resetMoveArea(ALI::all);
			IOLogFrame.setMoveArea(ALI::bottom|ALI::left,Pint(0,0));
			IOLogFrame.viewpos.y=-size_.y+30;
		}
	};
}
#
#endif
