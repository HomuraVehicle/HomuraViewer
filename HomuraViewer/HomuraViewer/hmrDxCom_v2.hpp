#ifndef HMR_DXCOM_INC
#define HMR_DXCOM_INC 200
#
/*===hmrDxCom===
Com関連のDxLib依存の描画クラスを定義

hmrDxCom:v2_00/130622 hmito
	IDによって表示の有無を選択できる湯ように変更
	ホイールスクロールに対応
hmrDxCom:v1_01/130615 hmIto
	Packet/Datumの識別をtimeからno_typeに移行
		timeだと、同一パケット内では全て同じ時間を示す場合がある。
hmrDxCom v1_00/130410 hmIto
	dxoPacketListを追加
*/
#include"hmLibVer.hpp"
#include<hmLib/config_vc.h>
#include<mutex>
#include<vector>
#include<chrono>
#include<iomanip>
#include<sstream>
#include<boost/format.hpp>
#include<boost/signals2.hpp>
#include<hmLib/signals.hpp>
#include<hmLib/inquiries.hpp>
#include<hmLib/any_iterator.hpp>
#include<hmLib_v2/dxObject.hpp>
#include<hmLib_v2/dxArea.hpp>
#include<hmLib_v2/dxColorSet.hpp>
#include <HomuraViewer/chrono.hpp>
#include <HomuraViewer/Message/PacketData.hpp>
#include<HomuraViewer/IO/IOLogGate.hpp>
namespace hmr{
	namespace viewer{
		//データのid:先頭数文字を描画する、dxButIO型の描画クラス
		class dxoCatDatum :public dxReObject, public hmoBox{
			using datum = message::datum;
		private:
			const datum& rDatum;
			unsigned int CatNum;
			dxHLS Clr;
			bool ON;
		public:
			dxoCatDatum(Pint Size_, const datum& Datum_, unsigned int CatNum_, const dxRGB& Clr_, bool ON_)
				:hmoBox(Size_), rDatum(Datum_), CatNum(CatNum_), Clr(Clr_), ON(ON_){}
			int redraw(dxO& dxo){
				std::string Str;
				Str += rDatum.ID;
				Str += ":";
				for(unsigned int Cnt = 0; Cnt < CatNum; ++Cnt){
					if(Cnt >= rDatum.Str.size())Str += "--";
					else Str += (boost::format("%02x") % (static_cast<unsigned int>(static_cast<unsigned char>(rDatum.Str.at(Cnt))))).str();
				}
				Clr.lgt = std::min(240, Clr.lgt + rDatum.Size);
				return dxo.draw(Pint(0, 0), dxoButIO(size, Str, Clr, ON, rDatum.ErrFlag ? CLR::LightSoftRed : CLR::White));
			}
		};
		//データの中身を描画する描画関数クラス
		class dxFnPacketListCtrl :public dxFn{
			using datum = message::datum;
		public:
			boost::signals2::signal<void(bool)> signal_setLinkNewPacket;
			std::vector<datum::id_type>& IDList;			//表示の有無を制御するためのIDのリスト
			const std::vector<datum::id_type>& SwIDList;	//スイッチとして表示の有無を制御したいIDのリスト
			bool Link;
		public:
			dxFnPacketListCtrl(bool Link_, std::vector<datum::id_type>& IDList_, const std::vector<datum::id_type>& SwIDList_)
				:Link(Link_)
				, IDList(IDList_)
				, SwIDList(SwIDList_){}
			int fndraw(dxO& dxo){
				//dxNormFrame限定
				try{
					//描画幅確定
					dxO::Acs<dxNormFrame> Acs(dxo);
					int Size = Acs().size.y;

					//Linkボタン
					if(dxo.draw(Pint(0, 0), dxoButIO(Pint(60, Size), "Sync", CLR::SoftGreen, Link)) == 1){
						signal_setLinkNewPacket(!Link);
					}

					int PosX = 60;
					for(auto itr = SwIDList.begin(); itr != SwIDList.end(); ++itr){
						bool OnOff = false;
						auto sitr = IDList.begin();
						for(; sitr != IDList.end(); ++sitr){
							if(*itr == *sitr){
								OnOff = true;
								break;
							}
						}
						//Swボタン
						if(dxo.draw(Pint(PosX, 0), dxoButIO(Pint(30, Size), (boost::format("%c") % (*itr)).str(), CLR::DarkSoftGreen, OnOff)) == 1){
							if(OnOff){
								IDList.erase(sitr);
							} else{
								IDList.push_back(*itr);
							}
						}
						PosX += 30;
					}
				}
				catch(const std::exception& Except){
					dxo.draw(Pint(0, 0), dxoStr(std::string("***ERROR***") + Except.what()));
				}
				catch(const std::string& Str){
					dxo.draw(Pint(0, 0), dxoStr(std::string("***ERROR***") + Str));
				}

				return 0;
			}
		};
		//パケットの羅列を描画する描画関数クラス
		template<typename iterator>
		class dxFnPacketList :public dxFn{
			using datum = message::datum;
			using packet = message::packet;

			typedef std::pair<packet::no_type, datum::no_type> seltype;
		private:
			iterator Begin;									//PacketLogの先頭
			iterator End;									//PacketLogの末尾
			const packet* pInPacket;						//最新入力Packet
			const packet* pOutPacket;						//最新出力Packet
			const dxRGB& InputClr;							//入力側の色
			const dxRGB& OutputClr;							//出力側の色
			const seltype& SelData;							//選択されているデータ Log入りしていないデータはclocl::time_point::max
			const std::vector<datum::id_type>& SelID;		//ID選択表示時に使用
		public:
			boost::signals2::signal<void(seltype)> signal_pushedDatum;		//選択されているデータが変更された場合に発信
		public:
			dxFnPacketList(iterator Begin_, iterator End_, const packet* pInPacket_, const packet* pOutPacket_, const seltype& SelData_, const std::vector<datum::id_type>& SelID_, const dxRGB& InputClr_ = CLR::SoftRed, const dxRGB& OutputClr_ = CLR::SoftBlue)
				:Begin(std::move(Begin_))
				, End(std::move(End_))
				, pInPacket(pInPacket_)
				, pOutPacket(pOutPacket_)
				, SelData(SelData_)
				, SelID(SelID_)
				, InputClr(InputClr_)
				, OutputClr(OutputClr_){}
			int fndraw(dxO& dxo){
				/*ScrlFrame限定*/
				try{
					//描画位置確定
					dxO::Acs<dxScrlFrame> Acs(dxo);

					//IDの選択が行われていない場合
					if(SelID.size() == 0){
						int ViewPos = Acs().viewpos.y;
						int ViewSize = Acs().size.y;

						//描画範囲に含まれるpacketのiteratorを計算
						auto DBegin = Begin + std::min(std::max(0, (-ViewPos - ViewSize) / 25 - 1), End - Begin);
						auto DEnd = Begin + std::min(std::max(0, (-ViewPos) / 25 + 1), End - Begin);

						//最下部のデータの描画位置となるように初期地設定
						int DrawPos = -25 * ((DBegin - Begin) + 1);

						//描画位置のみ描画
						for(auto pitr = DBegin; pitr != DEnd; ++pitr){
							dxRGB Clr;
							if(pitr->first)Clr = InputClr;
							else Clr = OutputClr;

							int DrawXPos = 125;

							//時刻描画
							dxo.draw(Pint(0, DrawPos), dxoStrP(Pint(120, 25), time_to_hms(pitr->second.Time), pitr->second.ErrFlag ? CLR::LightSoftRed : CLR::LightGray));

							//Packet内データを順に描画
							for(auto itr = pitr->second.Data.begin(); itr != pitr->second.Data.end(); ++itr){
								bool IsSelected = (SelData.first == pitr->second.No && SelData.second == itr->No);
								if(dxo.draw(Pint(DrawXPos, DrawPos), dxoCatDatum(Pint(70, 20), *itr, 2, Clr, IsSelected)) < 0){
									if(IsSelected)signal_pushedDatum(seltype(0, 0));
									else signal_pushedDatum(seltype(pitr->second.No, itr->No));
								}
								DrawXPos += 75;
							}
							DrawPos -= 25;
						}

						//描画位置を本来の位置に移動
						DrawPos = -25 * ((End - Begin) + 1);

						//OutPacket内
						if(pOutPacket != nullptr){
							//描画範囲内の時
							if(ViewPos <= DrawPos + 20 && DrawPos < ViewPos + ViewSize){
								dxo.draw(Pint(0, DrawPos), dxoTriangle(Pint(0, 20), Pint(20, 10), OutputClr));
								int DrawXPos = 125;
								//Packet内データを順に描画
								for(auto itr = pOutPacket->Data.begin(); itr != pOutPacket->Data.end(); ++itr){
									bool IsSelected = (SelData.first == 0 && SelData.second == itr->No);
									if(dxo.draw(Pint(DrawXPos, DrawPos), dxoCatDatum(Pint(70, 20), *itr, 2, OutputClr, IsSelected)) < 0){
										if(IsSelected)signal_pushedDatum(seltype(0, 0));
										else signal_pushedDatum(seltype(0, itr->No));
									}
									DrawXPos += 75;
								}
							}
							DrawPos -= 25;
						}

						//InPacket内
						if(pInPacket != nullptr){
							//描画範囲内の時
							if(ViewPos <= DrawPos + 20 && DrawPos < ViewPos + ViewSize){
								dxo.draw(Pint(0, DrawPos), dxoTriangle(Pint(0, 20), Pint(20, 10), InputClr));
								int DrawXPos = 125;
								//Packet内データを順に描画
								for(auto itr = pInPacket->Data.begin(); itr != pInPacket->Data.end(); ++itr){
									bool IsSelected = (SelData.first == 0 && SelData.second == itr->No);
									if(dxo.draw(Pint(DrawXPos, DrawPos), dxoCatDatum(Pint(70, 20), *itr, 2, InputClr, IsSelected)) < 0){
										if(IsSelected)signal_pushedDatum(seltype(0, 0));
										else signal_pushedDatum(seltype(0, itr->No));
									}
									DrawXPos += 75;
								}
							}
							DrawPos -= 25;
						}

						//最後に描画位置の指定を行っておく
						dxo.draw(Pint(0, 0), dxoNull());
						dxo.draw(Pint(0, DrawPos + 25), dxoNull());
					}//IDが選択されている場合
					else{
						//描画範囲を取得
						int ViewPos = Acs().viewpos.y;
						int ViewSize = Acs().size.y;
						//描画位置が不明なんで、初期地からスタート
						int DrawPos = -25;

						//総当たりで調べていく
						for(auto pitr = Begin; pitr != End; ++pitr){
							dxRGB Clr;
							if(pitr->first)Clr = InputClr;
							else Clr = OutputClr;

							int DrawXPos = 125;

							bool DrawFlag = false;
							bool ESCFlag = false;

							//時刻描画
							dxo.draw(Pint(0, DrawPos), dxoStrP(Pint(120, 25), time_to_hms(pitr->second.Time), CLR::LightGray));

							//Packet内データを順に描画
							for(auto itr = pitr->second.Data.begin(); itr != pitr->second.Data.end(); ++itr){
								//IDListと照らし合わせてみる
								for(auto selitr = SelID.begin(); selitr != SelID.end(); ++selitr){
									//IDListにdatum::IDが含まれている場合
									if(*selitr == itr->ID){
										DrawFlag = true;
										//描画範囲に入っていなければ、チェックだけして脱出
										if(DrawPos < ViewPos - 25 || ViewPos + ViewSize + 25 < DrawPos){
											//Packet内のforからも脱出するようにフラグを立てておく
											ESCFlag = true;
										} else{
											bool IsSelected = (SelData.first == pitr->second.No && SelData.second == itr->No);
											if(dxo.draw(Pint(DrawXPos, DrawPos), dxoCatDatum(Pint(70, 20), *itr, 2, Clr, IsSelected)) < 0){
												if(IsSelected)signal_pushedDatum(seltype(0, 0));
												else signal_pushedDatum(seltype(pitr->second.No, itr->No));
											}
											DrawXPos += 75;
										}
										break;
									}
								}
								//描画範囲外の場合、パケット内に描画データがあることさえわかればいいので脱出
								if(ESCFlag)break;
							}
							//描画対象だった場合、描画位置を記録
							if(DrawFlag)DrawPos -= 25;
						}

						//最後に描画位置の指定を行っておく
						dxo.draw(Pint(0, 0), dxoNull());
						dxo.draw(Pint(0, DrawPos + 25), dxoNull());
					}


				}
				catch(const std::exception& Except){
					dxo.draw(Pint(5, 5), dxoStr("***ERROR***"));
					dxo.draw(Pint(5, 25), dxoStr(Except.what()));
				}
				catch(const std::string& Str){
					dxo.draw(Pint(5, 5), dxoStr("***ERROR***"));
					dxo.draw(Pint(5, 25), dxoStr(Str));
				}

				return 0;
			}
		};
		//データの中身を描画する描画関数クラス
		class dxFnDatumView :public dxFn{
			using datum = message::datum;
			using packet = message::packet;
		private:
			const datum* Ptr;
		public:
			dxFnDatumView(const datum* Ptr_) :Ptr(Ptr_){}
			int fndraw(dxO& dxo){
				/*ScrlFrame限定*/
				try{
					//選択されていないときには、何も表示しない。
					if(Ptr == nullptr){
						dxo.draw(Pint(5, 5), dxoStr("No Data"));
						return 0;
					}

					int DrawPos = 5;

					//描画幅確定
					dxO::Acs<dxScrlFrame> Acs(dxo);
					//5文字単位のStrBlockの横方向の数
					//	25*5-5=120pixelがStrBlock
					//	両端5+位置情報45=55が固定して必要な長さ
					int StrBlockNum = (Acs().size.x - 55) / 145;

					//スクロール確保
					dxo.draw(Pint(0, 0), dxoNull());
					dxo.draw(Pint(0, 50 + 25 * StrBlockNum), dxoNull());

					//描画　まず基礎情報
					dxo.draw(Pint(5, DrawPos), dxoStr((boost::format("ID:%c Size:%d(%d) Err:%d") % Ptr->ID%Ptr->Size%Ptr->Str.size() % Ptr->ErrFlag).str()));
					DrawPos += 20;
					dxo.draw(Pint(5, DrawPos), dxoStr(time_to_hms(Ptr->Time)));

					//次に文字列情報
					int DrawPosX = 5;
					unsigned int Cnt = 0;
					for(auto itr = Ptr->Str.begin(); itr != Ptr->Str.end(); ++itr){
						if((Cnt % (StrBlockNum * 5)) == 0){
							DrawPos += 45;
							DrawPosX = 5;
							dxo.draw(Pint(DrawPosX, DrawPos + 10), dxoStr((boost::format("%4d") % Cnt).str()));
							DrawPosX += 40 + 5;	//文字幅40pixel+空白5pixel
						} else if(Cnt % 5 == 0){
							DrawPosX += 10;
						}
						dxo.draw(Pint(DrawPosX, DrawPos), dxoStr((boost::format("%02x") % static_cast<unsigned int>(static_cast<unsigned char>(*itr))).str()));
						dxo.draw(Pint(DrawPosX, DrawPos + 20), dxoStr((boost::format("%c") % (*itr)).str()));
						DrawPosX += 25;
						++Cnt;
					}

				}
				catch(const std::exception& Except){
					dxo.draw(Pint(5, 5), dxoStr("***ERROR***"));
					dxo.draw(Pint(5, 25), dxoStr(Except.what()));
				}
				catch(const std::string& Str){
					dxo.draw(Pint(5, 5), dxoStr("***ERROR***"));
					dxo.draw(Pint(5, 25), dxoStr(Str));
				}

				return 0;
			}
		};
		//dxFnPacketListとdxFnDatumViewを統合した描画クラス
		template<typename iterator>
		class dxosPacketList_withView :public dxReObject, public hmoBox{
			using datum = message::datum;
			using packet = message::packet;
			typedef std::pair<packet::no_type, datum::no_type> seltype;
		private:
			seltype SelData;							//現在選択されているデータ
			bool LinkNewPacket;							//NewPacketにリンクさせるかのフラグ					
			std::vector<datum::id_type> IDList;			//表示の有無を制御するためのIDのリスト
			const std::vector<datum::id_type> SwIDList;	//スイッチとして表示の有無を制御したいIDのリスト
			dxRGB Clr;									//背景カラー
			int DatumCtrlSize;							//Datumコントロール画面のy方向サイズ
			int DatumViewSize;							//Datum表示画面のy方向のサイズ
		private:
			hmLib::signals::unique_connections SignalConenctions;
		private:
			void setSelData(seltype SelData_){ SelData = SelData_; }
			void setLinkNewPacket(bool LinkNewPacket_){ LinkNewPacket = LinkNewPacket_; }
			void syncDatumViewSize(int DatumViewSize_){ DatumViewSize = DatumViewSize_; }
		public:
			hmLib::inquiries::range_inquiry<iterator> inquiryPacketLog;
			//Inquiryの中身のbegin/endを呼んでいるので、このInquiryは参照取得型(T&)すべき
			//	通常のタイプ(inquiry<T>)を使うと、コピーされたDataのbegin/endを使ってしまう。
			hmLib::inquiries::inquiry<const packet&> inquiryNewInPacket;
			hmLib::inquiries::inquiry<const packet&> inquiryNewOutPacket;
			void slot_setSelData(boost::signals2::signal<void(seltype)>& Signal_){
				SignalConenctions(hmLib::signals::connect(Signal_, [&](seltype SelData_)->void{this->setSelData(SelData_); }));
			}
			void slot_setLinkNewPacket(boost::signals2::signal<void(bool)>& Signal_){
				SignalConenctions(hmLib::signals::connect(Signal_, [&](bool LinkNewPacket_)->void{this->setLinkNewPacket(LinkNewPacket_); }));
			}
		private:
			dxNormFrame PacketListCtrlFrame;
			dxScrlFrame PacketListFrame;
			dxScrlFrame DatumViewFrame;
		public:
			int redraw(dxO& dxo)override{
				if(!inquiryPacketLog.is_connect()){
					dxo.draw(Pint(0, 0), dxoStr("NoConnection of RangeInquiry"));
					return 0;
				}

				try{
					//選択しているDatumを取得
					const datum* pSelDatum = nullptr;
					std::pair<bool, packet> TargetP;
					datum Target;
					TargetP.second.No = SelData.first;
					Target.No = SelData.second;

					//DatumViewの設定値が異常でない場合、Datumを描画するために選択されているデータをまず探してみる
					if(0 < DatumViewSize && DatumViewSize <= size.y){
						//Packet::Timeが不定値を指しているときは、現在のパケットから探す
						if(SelData.first == 0){
							if(inquiryNewInPacket.is_connect() && inquiryNewInPacket().Data.size()){
								auto rditr = std::equal_range(inquiryNewInPacket().Data.begin(), inquiryNewInPacket().Data.end(), Target);
								if(rditr.first != rditr.second)pSelDatum = &(*(rditr.first));
							}
							if(inquiryNewOutPacket.is_connect() && inquiryNewOutPacket().Data.size()){
								auto sditr = std::equal_range(inquiryNewOutPacket().Data.begin(), inquiryNewOutPacket().Data.end(), Target);
								if(sditr.first != sditr.second)pSelDatum = &(*(sditr.first));
							}
						}//指定されているパケットが存在している場合は、そのパケットをまず探す
						else{
							auto pitr = std::equal_range(inquiryPacketLog.begin(), inquiryPacketLog.end(), TargetP, [](const std::pair<bool, packet>& Pair1_, const std::pair<bool, packet>& Pair2_)->bool{return Pair1_.second.No < Pair2_.second.No; });
							if(pitr.first != pitr.second){
								auto ditr = std::equal_range((*pitr.first).second.Data.begin(), (*pitr.first).second.Data.end(), Target, [&](const datum& d1, const datum& d2)->bool{return d1.No < d2.No; });
								if(ditr.first != ditr.second)pSelDatum = &(*(ditr.first));
							}
						}
					}

					//最新Packetを取得
					const packet* pInPacket = nullptr;
					if(inquiryNewInPacket.is_connect()){
						if(inquiryNewInPacket().Data.size())pInPacket = &inquiryNewInPacket();
					}
					const packet* pOutPacket = nullptr;
					if(inquiryNewOutPacket.is_connect()){
						if(inquiryNewOutPacket().Data.size())pOutPacket = &inquiryNewOutPacket();
					}

					//Datumが選択されていないとき
					if(pSelDatum == nullptr){
						//パケット描画制御
						dxFnPacketListCtrl FnPacketListCtrl(LinkNewPacket, IDList, SwIDList);
						slot_setLinkNewPacket(FnPacketListCtrl.signal_setLinkNewPacket);
						PacketListCtrlFrame.set(FnPacketListCtrl, Pint(size.x, DatumCtrlSize), Clr);

						//パケット描画の設定
						auto Begin = inquiryPacketLog.begin();
						auto End = inquiryPacketLog.end();
						dxFnPacketList<iterator> FnPacketList(Begin, End, pInPacket, pOutPacket, SelData, IDList);
						slot_setSelData(FnPacketList.signal_pushedDatum);
						PacketListFrame.set(FnPacketList, Pint(size.x, size.y - DatumCtrlSize), Clr);
						PacketListFrame.resetMoveArea(ALI::all);
						//Linkフラグがたっている場合、描画位置を修正
						if(LinkNewPacket){
							PacketListFrame.viewpos.y = -25 * (End - Begin);
							if(pOutPacket != nullptr)PacketListFrame.viewpos.y -= 25;
							if(pInPacket != nullptr)PacketListFrame.viewpos.y -= 25;
						}
						Pint MPos = dxo.GtoD(dx::getMosPos());
						if(PacketListFrame.pIn(Pint(0, DatumCtrlSize), MPos)){
							PacketListFrame.viewpos.y -= 25 * dx::getMosH()*(dx::getKey(KEY::SHIFT) > 0 ? 3 : 1);
						}

						//描画
						dxo.draw(Pint(0, 0), PacketListCtrlFrame);
						dxo.draw(Pint(0, DatumCtrlSize), PacketListFrame);
					}//Datumが選択されている時
					else{
						//パケット描画制御
						dxFnPacketListCtrl FnPacketListCtrl(LinkNewPacket, IDList, SwIDList);
						slot_setLinkNewPacket(FnPacketListCtrl.signal_setLinkNewPacket);
						PacketListCtrlFrame.set(FnPacketListCtrl, Pint(size.x, DatumCtrlSize), Clr);

						//パケット描画の設定
						auto Begin = inquiryPacketLog.begin();
						auto End = inquiryPacketLog.end();
						dxFnPacketList<iterator> FnPacketList(Begin, End, pInPacket, pOutPacket, SelData, IDList);
						slot_setSelData(FnPacketList.signal_pushedDatum);
						PacketListFrame.set(FnPacketList, Pint(size.x, size.y - DatumViewSize - DatumCtrlSize), Clr);
						PacketListFrame.resetMoveArea(ALI::all);
						//Linkフラグがたっている場合、描画位置を修正
						if(LinkNewPacket){
							PacketListFrame.viewpos.y = -25 * (End - Begin);
							if(pOutPacket != nullptr)PacketListFrame.viewpos.y -= 25;
							if(pInPacket != nullptr)PacketListFrame.viewpos.y -= 25;
						}
						Pint MPos = dxo.GtoD(dx::getMosPos());
						if(PacketListFrame.pIn(Pint(0, DatumCtrlSize), MPos)){
							PacketListFrame.viewpos.y -= 25 * dx::getMosH()*(dx::getKey(KEY::SHIFT) > 0 ? 3 : 1);
						}

						//データ描画用Frameの設定
						dxFnDatumView FnDatumView(pSelDatum);
						DatumViewFrame.set(FnDatumView, Pint(size.x, DatumViewSize), Clr);

						//描画
						dxo.draw(Pint(0, 0), PacketListCtrlFrame);
						dxo.draw(Pint(0, DatumCtrlSize), PacketListFrame);
						dxo.draw(Pint(0, size.y - DatumViewSize), DatumViewFrame);
					}
				}
				catch(const std::exception& Except){
					dxo.draw(Pint(5, 5), dxoStr("***ERROR***"));
					dxo.draw(Pint(5, 25), dxoStr(Except.what()));
					printfDx(Except.what());
					printfDx("\n");
				}
				catch(const std::string& Str){
					dxo.draw(Pint(5, 5), dxoStr("***ERROR***"));
					dxo.draw(Pint(5, 25), dxoStr(Str));
				}

				return 0;
			}
			dxosPacketList_withView(Pint Size_, int DatumCtrlSize_, int DatumViewSize_, const std::vector<datum::id_type>& SwIDList_, const dxRGB& Clr_)
				:hmoBox(Size_)
				, SelData(0, 0)
				, LinkNewPacket(false)
				, DatumCtrlSize(DatumCtrlSize_)
				, DatumViewSize(DatumViewSize_)
				, Clr(Clr_)
				, SwIDList(SwIDList_){
				PacketListFrame.resetMoveArea(ALI::all);
				PacketListFrame.setMoveArea(ALI::bottom | ALI::left, Pint(0, 0));
				PacketListFrame.viewpos.y = -Size_.y;
			}
		};
	}
}
#
#endif
