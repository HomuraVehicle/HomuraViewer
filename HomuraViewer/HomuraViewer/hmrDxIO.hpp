#ifndef HMRDXIO_INC
#define HMRDXIO_INC 103

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
#include <HomuraViewer/chrono.hpp>
#include <HomuraViewer/Message/PacketData.hpp>
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
	namespace viewer{
		//  コマンド打ち込む用窓
		class dxosIOSendBox:public dxReObject,public hmoBox{
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
			dxosIOSendBox(Pint Size_,const dxColor& Clr_)
				:hmoBox(Size_)
				,Clr(Clr_){
	//			Inp.active();
			}

			// DxInput, DxImage とかの場合、必ずこのini()関数をdx::ini()された後に呼ぶことが必要
			void ini(){
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

		// データ表示切り替えよう窓
		class dxosSelectIOPaint:public dxReObject,public hmoBox{
		private:
			bool InPaintFlag;
			bool OutPaintFlag;
			dxRGB inClr, outClr;
			Pint Size;

		public:
			// コンストラクタ
			dxosSelectIOPaint(Pint Size_, const dxRGB& inClr_ = CLR::LightDullOrenge, const dxRGB& outClr_ = CLR::LightDullSkyblue)
				:hmoBox(Size_),InPaintFlag(true),OutPaintFlag(true), inClr(inClr_), outClr(outClr_), Size(Size_){}

		public:
			// signal 系列の定義
			boost::signals2::signal<void(bool)> signal_require_paintInput;
			boost::signals2::signal<void(bool)> signal_require_paintOutput;
		public:
			// inquire 系列の定義
			hmLib::inquiries::inquiry<bool> InPaintFlagInq;
			hmLib::inquiries::inquiry<bool> OutPaintFlagInq;

		protected:
			// 描画
			virtual int redraw(dxO& dxo){
				try{
					// 更新 とともに チェックボックスの描画
					try{
						InPaintFlag = InPaintFlagInq();
						if(dxo.draw(Pint(0,0),dxoButIO(Pint(Size.x / 2, Size.y),"IN", inClr, InPaintFlag),true) == 1){
							//if(dxo.draw(Pint(0,0),dxoCheck(Pint(Size.x / 2, Size.y),"Inデータ描画",Size.y,Clr,InPaintFlag))==1){
							InPaintFlag = !InPaintFlag;
							signal_require_paintInput(InPaintFlag);
						}
					}catch(const hmLib::inquiries::unconnected_exception&){
						// Inquire にアクセスできなかった！！　でも、そんなのメイン関数には関係ないからもみ消す
						dxo.draw(Pint(0, 0), dxoStr("Inquiry:: InPaintFlag取得失敗") );
					}

					try{
						OutPaintFlag = OutPaintFlagInq();
						if(dxo.draw(Pint(Size.x /2 ,0),dxoButIO(Pint(Size.x / 2, Size.y),"OUT", outClr, OutPaintFlag),true) == 1){
							//if(dxo.draw(Pint(Size.x /2 ,0),dxoCheck(Pint(Size.x / 2, Size.y),"Outデータ描画",20,Clr,OutPaintFlag))==1){
							OutPaintFlag = !OutPaintFlag;
							signal_require_paintOutput(OutPaintFlag);
						}
					}catch(const hmLib::inquiries::unconnected_exception&){
						// Inquire にアクセスできなかった！！　でも、そんなのメイン関数には関係ないからもみ消す
						dxo.draw(Pint(Size.x /2, 0), dxoStr("Inquiry:: OutPaintFlag取得失敗") );
					}
				}catch(const hmLib::exceptions::exception&){
					// 想定外のエラー
					dxo.draw(Pint(0, 0), dxoStr("想定外のエラー　"));
				}
				return 0;
			}
		};

		//! データ確認用の窓 Function
		//		送信、受信をフラグで切り替えるようにする
		//		取得時間とInOut種別、取得データを出してあげる
		#define IO_DISPLAY_INFO_SIZE 125 // IO画面の初めに出す情報用の枠（時間のみ）
		template<typename iterator>
		class dxFnIOLog:public dxFn{
		private:
			// IOデータタイプのtypedef 
			//typedef std::pair<bool,system_clock_iologtype> io_iologtype;
			//typedef std::vector<io_iologtype> container;
			//typedef container::const_iterator contanerItr;

			// 描画に必要な変数
			iterator ItrBegin;
			iterator ItrEnd;

			dxRGB InClr;
			dxRGB OutClr;

			bool InPaintFlag;
			bool OutPaintFlag;

		public:
			// コンストラクタ
			dxFnIOLog(iterator Begin_, iterator End_, bool InPaint_, bool OutPaint_, dxRGB InClr_=CLR::LightDullOrenge, dxRGB OutClr_= CLR::LightDullSkyblue)
				:ItrBegin(Begin_), ItrEnd(End_), InPaintFlag(InPaint_), OutPaintFlag(OutPaint_), 
				InClr(InClr_), OutClr(OutClr_)
			{}
			// Draw 関数
			int fndraw(dxO& dxo){
				// 描画位置
				dxO::Acs<dxScrlFrame> Acs(dxo);
				int DrawPosY = 0;

				// これは一番左上の座標であることに注意
				// 　下方向がｙ軸正、右方向がx軸正の方向になっている　
				Pint ViewPos  = Pint(Acs().viewpos.x, Acs().viewpos.y);
				Pint ViewSize = Pint(Acs().size.x, Acs().size.y);

				// 描画開始 混在の場合のみ
				if(InPaintFlag && OutPaintFlag){
					// データがあることを確認
					if( ItrEnd - ItrBegin > 0 ){
						// y方向についてダミーで描画しておく
						dxo.draw(Pint(0, static_cast<int>((ItrEnd - ItrBegin)*45+20)),dxoNull());
						//dxo.draw(Pint(5,5),dxoStr("Test TEst"));
					
						// 現在見ている位置だけを描画する

						//y方向について描画する前後を確保
						// 最終データから描画していくので、引き算してイテレータを足していく
						auto DBegin = ItrEnd - std::min(std::max(0,(ViewPos.y)/45 -1),(ItrEnd - ItrBegin));
						auto DEnd = ItrEnd   - std::min(std::max(0,(ViewPos.y+ViewSize.y)/45 + 1),ItrEnd - ItrBegin);
						//描画位置となるように表示
						if((ViewPos.y)/45 -1 < 0) DrawPosY = 0;
						else DrawPosY=((ViewPos.y)/45)*45;

						//x方向について描画する領域を確保
						// 文字のサイズは　24、初めのデータ情報のサイズは 125 としてみた

						int StrBegin = std::max(0, (ViewPos.x  - IO_DISPLAY_INFO_SIZE) / 24);
						int StrEnd = std::max(0, (ViewPos.x  + ViewSize.x - IO_DISPLAY_INFO_SIZE) / 24 );
						//描画位置
						//if((ViewPos.x - ViewSize.x)/24 -1 < 0) DrawPos.x = 5;
						//else DrawPos.x = ( (ViewPos.x - ViewSize.x)/24 ) * 24 + 5;

						//描画位置のみ描画
						//  イテレータを引き算して計算していく
						for(auto pitr=DBegin - 1;pitr != DEnd;--pitr){
							dxRGB Clr;
							// Color 設定
							if(pitr->first)Clr=InClr;
							else Clr=OutClr;

							// Dummyでx方向に描画しておく
							dxo.draw(Pint(IO_DISPLAY_INFO_SIZE + pitr->second.Str.size() * 24, DrawPosY),dxoNull());

							//-------  String データを順に描画	
							if(StrBegin == 0)// Header 部分の描画
							{
								/*
								// In or Out
								if(pitr->first){
									dxo.draw(Pint(5,DrawPosY +10), dxoStr("I", Clr));
								}else{
									dxo.draw(Pint(5,DrawPosY +10), dxoStr("O", Clr));
								}
								*/

								// 時間描画
								//dxo.draw(Pint(25,DrawPosY+2), dxoStr("Time Start", Clr));
								//dxo.draw(Pint(120,DrawPosY+2),dxoStr(time_to_hms(pitr->second.BeginTime),Clr));
								dxo.draw(Pint(5,DrawPosY+2),dxoStr(time_to_hms(pitr->second.EndTime),Clr));

								//dxo.draw(Pint(25,DrawPosY + 22), dxoStr("Time End", Clr));
								//dxo.draw(Pint(120,DrawPosY + 22),dxoStr(time_to_hms(pitr->second.EndTime), Clr));
								dxo.draw(Pint(5,DrawPosY + 22),dxoStr(time_to_hms(pitr->second.BeginTime), Clr));
														
							}
							// データ部分の描画
							if( (int)pitr->second.Str.size() > StrBegin ){
								for(int i = StrBegin; i < std::min( (StrEnd) , ((int)pitr->second.Str.size()) ) ; i++){
									dxo.draw(Pint(IO_DISPLAY_INFO_SIZE + i*24, DrawPosY),dxoStrP(Pint(24,20),hmstd::fStr("%c",pitr->second.Str[i]),Clr));
									dxo.draw(Pint(IO_DISPLAY_INFO_SIZE + i*24, DrawPosY+ 20),dxoStrP(Pint(24,20),hmstd::fStr("%02x",pitr->second.Str[i] & 0xff),Clr));
									// char型なので負の数になっており、printfは内部で4byte int　に型変換しているので、0x99 -> 0xffffff99　になってしまう。よってand演算をしておく
								}
							}
							//描画場所の変更
							DrawPosY += 45;
						}
					}else{
						dxo.draw(Pint(5,10), dxoStr("No Data", CLR::DarkYellow));
					
						/*
						dxo.draw(Pint(5, 20), dxoStr("Iaaaaaaa", InClr));
						dxo.draw(Pint(5, 35), dxoStr("Iaaaaaaa", OutClr));
						dxo.draw(Pint(5, 50), dxoStr("LightDullMagenta", CLR::LightDullMagenta ));
						dxo.draw(Pint(5, 65), dxoStr("LightDullSkyblue", CLR::LightDullSkyblue ));
						dxo.draw(Pint(5, 80), dxoStr("LightDullOrenge", CLR::LightDullOrenge ));
						dxo.draw(Pint(5, 95), dxoStr("SoftBlue", CLR::SoftBlue));
						dxo.draw(Pint(5, 110), dxoStr("SoftOrenge", CLR::SoftOrenge));
						dxo.draw(Pint(5, 125), dxoStr("SoftSkyblue", CLR::SoftSkyblue ));
						dxo.draw(Pint(5, 140), dxoStr("SoftViolet", CLR::SoftViolet ));
						dxo.draw(Pint(5, 155), dxoStr("SoftYellow", CLR::SoftYellow ));
						dxo.draw(Pint(5, 170), dxoStr("DarkSeagreen", CLR::DarkSeagreen));
						dxo.draw(Pint(5, 185), dxoStr("DarkBlue", CLR::DarkBlue));
						dxo.draw(Pint(5, 200), dxoStr("LightGreen", CLR::LightGreen ));
						dxo.draw(Pint(5, 215), dxoStr("Seagreen", CLR::Seagreen ));
						dxo.draw(Pint(5, 230), dxoStr("DarkSoftMagenta", CLR::DarkSoftMagenta ));
						dxo.draw(Pint(5, 245), dxoStr("CLR::DarkSoftSeagreen", CLR::DarkSoftSeagreen));
						dxo.draw(Pint(5, 260), dxoStr("CLR::PaleDullSeaGreen", CLR::PaleDullSeagreen));
						*/
					}
				}
				// 描画データがない場合
				else if(!InPaintFlag && !OutPaintFlag){
					dxo.draw(Pint(5,10), dxoStr("No Data"));
				}
				// 描画するものが決められている場合
				// こちらは処理に時間がかかってしまう(全部にアクセスしてしまっている)ので、処理ごとif分岐にした
				else{
					// データがあることを確認
					if( ItrEnd - ItrBegin > 0 ){
						int DataNum = 0;// 現在のデータの番号

						// ひたすらに描画していく
						for(auto pitr = ItrEnd - 1; pitr != ItrBegin ; --pitr){
							// 適合データを取り出して行く
							if(pitr->first == InPaintFlag){
								++DataNum;
								DrawPosY = (DataNum - 1) * 45;

								// 描画範囲に入っていれば描画する
								if( (DataNum) * 45 > ViewPos.y && (DataNum - 1) * 45 < (ViewPos.y + ViewSize.y)){
									// 描画処理を行う
									dxRGB Clr;
									// Color 設定
									if(pitr->first)Clr=InClr;
									else Clr=OutClr;

									// Dummyでx方向に描画しておく
									dxo.draw(Pint(IO_DISPLAY_INFO_SIZE + pitr->second.Str.size() * 24, DrawPosY),dxoNull());

									// 描画位置の決定
									int StrBegin = std::max(0, (ViewPos.x  - IO_DISPLAY_INFO_SIZE) / 24);
									int StrEnd = std::max(0, (ViewPos.x  + ViewSize.x - IO_DISPLAY_INFO_SIZE) / 24 );

									//-------  String データを順に描画	
									if(StrBegin == 0)// Header 部分の描画
									{
										/*
										// In or Out
										if(pitr->first){
											dxo.draw(Pint(5,DrawPosY +10), dxoStr("I", Clr));
										}else{
											dxo.draw(Pint(5,DrawPosY +10), dxoStr("O", Clr));
										}
										*/

										// 時間描画
										//dxo.draw(Pint(25,DrawPosY+2), dxoStr("Time Start", Clr));
										//dxo.draw(Pint(120,DrawPosY+2),dxoStr(time_to_hms(pitr->second.BeginTime),Clr));
										dxo.draw(Pint(5, DrawPosY+2),dxoStr(time_to_hms(pitr->second.EndTime),Clr));

										//dxo.draw(Pint(25,DrawPosY + 22), dxoStr("Time End", Clr));
										//dxo.draw(Pint(120,DrawPosY + 22),dxoStr(time_to_hms(pitr->second.EndTime), Clr));
										dxo.draw(Pint(5, DrawPosY + 22),dxoStr(time_to_hms(pitr->second.BeginTime), Clr));
									}
									// データ部分の描画
									if( (int)pitr->second.Str.size() > StrBegin ){
										for(int i = StrBegin; i < std::min( (StrEnd) , ((int)pitr->second.Str.size()) ) ; i++){
											dxo.draw(Pint(IO_DISPLAY_INFO_SIZE + i*24, DrawPosY),dxoStrP(Pint(24,20),hmstd::fStr("%c",pitr->second.Str[i]),Clr));
											dxo.draw(Pint(IO_DISPLAY_INFO_SIZE + i*24, DrawPosY+ 20),dxoStrP(Pint(24,20),hmstd::fStr("%02x",pitr->second.Str[i] & 0xff),Clr));
											// char型なので負の数になっており、printfは内部で4byte int　に型変換しているので、0x99 -> 0xffffff99　になってしまう。よってand演算をしておく
										}
									}	
								}
							}
						}
						// 適合データがなかった場合の処理
						if(DataNum == 0){
							dxo.draw(Pint(5,10), dxoStr("No Data"));
						}
						// y方向についてダミーで描画しておく
						else{
							dxo.draw(Pint(0, static_cast<int>(DataNum*45+0)),dxoNull());
						}
					}else{
						dxo.draw(Pint(5,10), dxoStr("No Data"));
					}
				}

				return 0;

			}
		};

		//! IOデータ確認用の窓
		// 描画クラス static
		template<typename iterator>
		class dxosIO:public dxReObject,public hmoBox{
		private:
			// Frame描画用関数
			//dxFnIOLog FnIOLog;
			// Frame 
			dxScrlFrame IOLogFrame;

		private:
			// Color 用の変数
			dxRGB Clr,SubClr;
			Pint size;
			// 実際にはこのフラグはこのクラスで管理する
			bool InPaintFlag, OutPaintFlag;

		// Inquiry 系統の宣言
		private:
			// Inquiry用の型変換
			typedef hmLib::inquiries::range_inquiry<iterator> iolog_inquiry;
		public:
			//ioLogバッファへのアクセス用
			iolog_inquiry IoLogBufInq;

		// contact 系統
			void contact_InPaintFlag(hmLib::inquiries::inquiry<bool> &inq_){
				hmLib::inquiries::connect(inq_, InPaintFlag);
			}
			void contact_OutPaintFlag(hmLib::inquiries::inquiry<bool> &inq_){
				hmLib::inquiries::connect(inq_, OutPaintFlag);
			}

		// Signal-Slot 系統の宣言
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
					// dxScrlFlame 描画
					dxFnIOLog<iterator> FnIOLog(IoLogBufInq.begin(), IoLogBufInq.end(), InPaintFlag, OutPaintFlag);
			
					IOLogFrame.set(FnIOLog, size, Clr); 
					dxo.draw(Pint(0,0), IOLogFrame);
			
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

			dxosIO(Pint size_, const dxRGB& Clr_)
				:hmoBox(size_), InPaintFlag(true), OutPaintFlag(true)
				//FnIOLog(iterator Begin_, iterator End_, bool InPaint_, bool OutPaint_)
				//IOLogBox(Pint(600,800),CLR::Cyan,CLR::Pink)
			{
			
				Clr=Clr_;
				dxHLS tmp(Clr);

				tmp.sat/=2;
				SubClr=tmp;

				size = size_;
		
			}
		};		

		//最新データの描画にも対応したバージョン
		template<typename iterator>
		class dxFnIOLog2:public dxFn{
			using datum = message::datum;
			using packet = message::packet;
			using seltype = std::pair<packet::no_type,datum::no_type> ;
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
			dxFnIOLog2(iterator Begin_,iterator End_,const std::string* pInStr_,const std::string* pOutStr_,bool InPaintFlag_,bool OutPaintFlag_,const dxRGB& InputClr_=CLR::LightDullOrenge,const dxRGB& OutputClr_=CLR::LightDullSkyblue)
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
							dxo.draw(Pint(0,DrawPos),dxoStrP(Pint(120,45),time_to_hms(pitr->second.EndTime),CLR::LightGray));

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
								dxo.draw(Pint(0,DrawPos),dxoStrP(Pint(120,45),time_to_hms(pitr->second.EndTime),CLR::LightGray));

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
			// Color 用の変数
			dxRGB Clr,SubClr;
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
					dxFnIOLog2<iterator> FnIOLog(Begin,End,pInStr,pOutStr,InPaintFlag,OutPaintFlag);
					IOLogFrame.set(FnIOLog,size,Clr);
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
					dxo.draw(Pint(0,0),IOLogFrame);
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

			dxosIO2(Pint size_, const dxRGB& Clr_)
				:hmoBox(size_)
				,InPaintFlag(true)
				,OutPaintFlag(true){
				Clr=Clr_;
				dxHLS tmp(Clr);

				tmp.sat/=2;
				SubClr=tmp;

				IOLogFrame.resetMoveArea(ALI::all);
				IOLogFrame.setMoveArea(ALI::bottom|ALI::left,Pint(0,0));
				IOLogFrame.viewpos.y=-size_.y;
			}
		};		

		//! コントロール系のクラス
		// Start のスイッチ
		class dxosStartControl:public dxReObject,public hmoBox{
		private:
			dxRGB Clr;
			bool StartFlag;
			std::string ButtName;

		public:
			// signal 系列の定義
			boost::signals2::signal<void(bool)> signal_require_setStartFlag;
			// inquire 系列の定義
			hmLib::inquiries::inquiry<bool> StartFlagInq;

			// constructor
			dxosStartControl(Pint Size_, const dxColor& Clr_, const std::string& ButtName_)
				:hmoBox(Size_)
				,Clr(Clr_)
				,ButtName(ButtName_)
				,StartFlag(false) 
			{}

		protected:
			virtual int redraw(dxO& dxo){
				try{
					// データ更新
					StartFlag = StartFlagInq(); // debug

					// ボタン
					if( dxo.draw(Pint(0,0),dxoButIO(size, ButtName, Clr, StartFlag)) == 1){
						StartFlag = !StartFlag;
						signal_require_setStartFlag(StartFlag);
					}

				}catch(const hmLib::inquiries::unconnected_exception&){				
					// Inquire にアクセスできなかった！！　でも、そんなのメイン関数には関係ないからもみ消す
					dxo.draw(Pint(0, 0), dxoStr("Inquiry::" + ButtName + "取得失敗") );
				}catch(const hmLib::exceptions::exception& Excp){
					dxo.draw(Pint(0, 0), dxoStr("Error::" + ButtName + Excp.what()) );
				}
				return 0;
			}		
		};

		// Buf操作
		class dxosBufControl:public dxReObject,public hmoBox{
		private:
			dxRGB Clr;
			unsigned int BufSize;
			std::string ButtName;
		public:
			// signal 系列の定義
			boost::signals2::signal<void(void)> signal_require_clearBuf;
			// inquire 系列の定義
			hmLib::inquiries::inquiry<unsigned int> BufSizeInq;
		
		public:
			dxosBufControl(Pint Size_,const dxColor& Clr_, const std::string& ButtName_)
				:hmoBox(Size_)
				,Clr(Clr_)
				,ButtName(ButtName_)
				,BufSize(0) 
			{}
		protected:
			virtual int redraw(dxO& dxo){
				try{
					// データ更新
					BufSize = BufSizeInq(); //debug

					// ボタン
					dxRGB Clr_ = Clr;
					if((int)BufSize > 0){
						Clr_ = CLR::DarkDullRed;
					}
					if( dxo.draw(Pint(0,0),dxoBut(size, (ButtName + hmstd::fStr("( %d )", (int)BufSize)), Clr_)) == 1){
						signal_require_clearBuf();
					}
				}catch(const hmLib::inquiries::unconnected_exception&){				
					// Inquire にアクセスできなかった！！　でも、そんなのメイン関数には関係ないからもみ消す
					dxo.draw(Pint(0, 0), dxoStr("Inquiry::" + ButtName +"取得失敗") );
				}catch(const hmLib::exceptions::exception& Excp){
					dxo.draw(Pint(0, 0), dxoStr("Error::" + ButtName + Excp.what()) );
				}

				return 0;
			}
		};

		// TimeOut操作
		class dxosTimeOutControl:public dxReObject,public hmoBox{
		private:
			dxRGB Clr;
			std::string ButtName;

			clock::duration TimeOut;
			//bool IsFullDuplex;

		public:
			// signal 系列の定義
			boost::signals2::signal<void(clock::duration)> signal_require_setTimeOut;
		
			// inquire 系列の定義
			hmLib::inquiries::inquiry<clock::duration> TimeOutInq;
			//hmLib::inquiries::inquiry<bool> IsFullDuplexInq;
		
		public:
			dxosTimeOutControl(Pint Size_, const dxColor& Clr_, const std::string& ButtName_)
				:hmoBox(Size_)
				,Clr(Clr_)
				,ButtName(ButtName_)
				//,IsFullDuplex(true)
				,TimeOut( std::chrono::milliseconds(250) )
			{}

		protected:
			virtual int redraw(dxO& dxo){
				try{
					// データ更新
					TimeOut = TimeOutInq(); // debug
				
					// 扱いやすくINTにしておく
					int Interval_ms = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(TimeOut).count());
				
					// Interval 設定
					int ans=dxo.draw(Pint(0,0), dxoButLRIO(size,ButtName + hmstd::fStr(" %d(ms)",Interval_ms), Clr, true));
				
					if(ans==1){
						Interval_ms*=2;
						if(Interval_ms>4000)Interval_ms = 250;
					}
					else if(ans==10){
						Interval_ms/=2;
						if(Interval_ms<250)Interval_ms=4000;
					}
				

					// データ変換
					TimeOut = std::chrono::milliseconds(Interval_ms);

					// signal 発信
					signal_require_setTimeOut(TimeOut);

				}catch(const hmLib::inquiries::unconnected_exception& ){				
					// Inquire にアクセスできなかった！！　でも、そんなのメイン関数には関係ないからもみ消す
					dxo.draw(Pint(0, 0), dxoStr("Inquiry::" + ButtName + "取得失敗") );
				}catch(const hmLib::exceptions::exception& Excp){
					dxo.draw(Pint(0, 0), dxoStr("Error::" + ButtName + Excp.what()) );
				}

				return 0;
			}
		};

		// Sync 操作
		//  半二重通信時のみ
		class dxosSyncControl:public dxReObject,public hmoBox{
		private:
			bool SyncFlag;

			clock::duration Interval;
			bool IsFullDuplex;
			dxRGB Clr;
		
		public:
			// コンストラクタ
			dxosSyncControl(Pint Size_, const dxRGB& Clr_)
				:hmoBox(Size_)
				,SyncFlag(false)
				,IsFullDuplex(true)
				,Interval(std::chrono::milliseconds(250))
				,Clr(Clr_)
				{}

		public:
			// signal 系列の定義
			//boost::signals2::signal<void(bool)> signal_require_setSyncFlag;
			// interval の設定
			boost::signals2::signal<void(clock::duration)> signal_require_setTimeInterval;


		public:
			// inquire 系列の定義
			//hmLib::inquiries::inquiry<bool> SyncFlagInq;
			hmLib::inquiries::inquiry<bool> IsFullDuplexInq;
			hmLib::inquiries::inquiry<clock::duration> TimeIntervalInq;
		
		protected:
			// 描画
			virtual int redraw(dxO& dxo){
				// 更新 とともに チェックボックスの描画
				try{
					IsFullDuplex = IsFullDuplexInq(); //debug

					if(dxo.draw(Pint(0,0),dxoCheck(size,"Sync",size.y,Clr,SyncFlag))==1){
						if(!IsFullDuplex){
							SyncFlag = !SyncFlag;
							if(SyncFlag){
								// Sync 要求がこればIntervalを0にする
								Interval = TimeIntervalInq();//　以前の値を保持
								signal_require_setTimeInterval(std::chrono::milliseconds(0));
							}else{
								//Sync解除要求がこれば、元に戻す
								signal_require_setTimeInterval(Interval);
							}
						}
					}
				}catch(const hmLib::inquiries::unconnected_exception& ){
					// Inquire にアクセスできなかった！！　でも、そんなのメイン関数には関係ないからもみ消す
					dxo.draw(Pint(0, 0), dxoStr("Inquiry:: Interval 取得失敗") );
				}catch(const hmLib::exceptions::exception& Excp){
					std::string str1 = "Error:: Interval ";
					dxo.draw(Pint(0, 0), dxoStr(str1 + Excp.what()) );
				}

				return 0;
			}
		};

		// TimeInterval操作
		class dxosTimeIntervalControl:public dxReObject,public hmoBox{
		private:
			dxRGB Clr;
			std::string ButtName;

			clock::duration TimeInterval;
			//bool IsFullDuplex;
			//int TimeOut;//(ms)
			//bool TimeOutFlag;

		public:
			// signal 系列の定義
			boost::signals2::signal<void(clock::duration)> signal_require_setTimeInterval;
		
			// inquire 系列の定義
			hmLib::inquiries::inquiry<clock::duration> TimeIntervalInq;
			//hmLib::inquiries::inquiry<bool> IsFullDuplexInq;
		
		public:
			dxosTimeIntervalControl(Pint Size_, const dxColor& Clr_, const std::string& ButtName_ = "SendInterval")
				:hmoBox(Size_)
				,Clr(Clr_)
				,ButtName(ButtName_)
				//,IsFullDuplex(true)
				,TimeInterval( std::chrono::milliseconds(250) )
			{}

		protected:
			virtual int redraw(dxO& dxo){
				try{
					// データ更新
					TimeInterval = TimeIntervalInq(); // debug
				
					// 扱いやすくINTにしておく
					int Interval_ms = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(TimeInterval).count());
				
					// Interval 設定
					int ans=dxo.draw(Pint(0,0), dxoButLRIO(size,ButtName + hmstd::fStr(" %d(ms)",Interval_ms), Clr, true));
				

					if(ans==1){
						Interval_ms*=2;
						if(Interval_ms>4000)Interval_ms = 250;
					}
					else if(ans==10){
						Interval_ms/=2;
						if(Interval_ms<250)Interval_ms=4000;
					}
				

					// データ変換
					TimeInterval = std::chrono::milliseconds(Interval_ms);

					// signal 発信
					signal_require_setTimeInterval(TimeInterval);

				}catch(const hmLib::inquiries::unconnected_exception& ){				
					// Inquire にアクセスできなかった！！　でも、そんなのメイン関数には関係ないからもみ消す
					dxo.draw(Pint(0, 0), dxoStr("Inquiry::" + ButtName + "取得失敗") );
				}catch(const hmLib::exceptions::exception& Excp){
					dxo.draw(Pint(0, 0), dxoStr("Error::" + ButtName + Excp.what()) );
				}

				return 0;
			}
		};

		// 全二重、半二重　選択
		class dxosSelectDuplex:public dxReObject,public hmoBox{
		private:
			bool IsFullDuplexFlag;
			dxRGB Clr;
			Pint Size;

		public:
			// コンストラクタ
			dxosSelectDuplex(Pint Size_, const dxRGB& Clr_)
				:hmoBox(Size_),IsFullDuplexFlag(true), Clr(Clr_), Size(Size_){}

		public:
			// signal 系列の定義
			boost::signals2::signal<void(bool)> signal_require_isFullDuplex;
		public:
			// inquire 系列の定義
			hmLib::inquiries::inquiry<bool> IsFullDuplexFlagInq;

		protected:
			// 描画
			virtual int redraw(dxO& dxo){
				try{
					// 更新 とともに チェックボックスの描画
					try{
						IsFullDuplexFlag = IsFullDuplexFlagInq();//debug
						if(dxo.draw(Pint(0,0),dxoCheck(Pint(Size.x / 2, Size.y),"全二重通信",Size.y,Clr,IsFullDuplexFlag))==1){
							IsFullDuplexFlag = !IsFullDuplexFlag;
							signal_require_isFullDuplex(IsFullDuplexFlag);
						}
						if(dxo.draw(Pint(Size.x /2 ,0),dxoCheck(Pint(Size.x / 2, Size.y),"半二重通信",Size.y,Clr,!IsFullDuplexFlag))==1){
							IsFullDuplexFlag = !IsFullDuplexFlag;
							signal_require_isFullDuplex(IsFullDuplexFlag);
						}
					}catch(const hmLib::inquiries::unconnected_exception& ){
						// Inquire にアクセスできなかった！！　でも、そんなのメイン関数には関係ないからもみ消す
						dxo.draw(Pint(0, 0), dxoStr("Inquiry:: IsFullDuplexFlag取得失敗") );
					}
				}catch(const hmLib::exceptions::exception& ){
					// 想定外のエラー
					dxo.draw(Pint(0, 0), dxoStr("想定外のエラー　"));
				}
				return 0;
			}
		};
	}
}

#endif









