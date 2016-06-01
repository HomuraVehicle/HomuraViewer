#ifndef HMR_DXCOM_INC
#define HMR_DXCOM_INC 101
#
/*===hmrDxCom===
Com関連のDxLib依存の描画クラスを定義

hmrDxCom:v1_01/130615 hmIto
	Packet/Datumの識別をtimeからno_typeに移行
		timeだと、同一パケット内では全て同じ時間を示す場合がある。
hmrDxCom v1_00/130410 hmIto
	dxoPacketListを追加
*/
#include"hmLibVer.hpp"
#include<hmLib_v3_06/config_vc.h>
#include<mutex>
#include<vector>
#include<chrono>
#include<iomanip>
#include<sstream>
#include<boost/format.hpp>
#include<boost/signals2.hpp>
#include<hmLib_v3_06/signals.hpp>
#include<hmLib_v3_06/inquiries.hpp>
#include<hmLib_v3_06/any_iterator.hpp>
#include<hmLib_v2/dxObject.hpp>
#include<hmLib_v2/dxArea.hpp>
#include<hmLib_v2/dxColorSet.hpp>
#include"hmrData.hpp"
#include"iologgate.hpp"
namespace hmr{

	//たすく
	// IDList用のシグナル
	//	clear
	//	add
	//データのid:先頭数文字を描画する、dxButIO型の描画クラス
	class dxoCatDatum:public dxReObject,public hmoBox{
	private:
		const datum& rDatum;
		unsigned int CatNum;
		dxHLS Clr;
		bool ON;
	public:
		dxoCatDatum(Pint Size_,const datum& Datum_,unsigned int CatNum_,const dxRGB& Clr_,bool ON_)
			:hmoBox(Size_),rDatum(Datum_),CatNum(CatNum_),Clr(Clr_),ON(ON_){
		}
		int redraw(dxO& dxo){
			std::string Str;
			Str+=rDatum.ID;
			Str+=":";
			for(unsigned int Cnt=0;Cnt<CatNum;++Cnt){
				if(Cnt>=rDatum.Str.size())Str+="--";
				else Str+=(boost::format("%02x")%(static_cast<unsigned int>(static_cast<unsigned char>(rDatum.Str.at(Cnt))))).str();
			}
			Clr.lgt=std::min(240,Clr.lgt+rDatum.Size);
			return dxo.draw(Pint(0,0),dxoButIO(size,Str,Clr,ON,rDatum.ErrFlag?CLR::LightSoftRed:CLR::White));
		}
	};
	//パケットの羅列を描画する描画関数クラス
	template<typename iterator>
	class dxFnPacketList:public dxFn{
		typedef std::pair<packet::no_type,datum::no_type> seltype;
	private:
		iterator Begin;									//PacketLogの先頭
		iterator End;									//PacketLogの末尾
		const packet* pInPacket;						//最新入力Packet
		const packet* pOutPacket;						//最新出力Packet
		const dxRGB& InputClr;							//入力側の色
		const dxRGB& OutputClr;							//出力側の色
		const seltype& SelData;							//選択されているデータ Log入りしていないデータはclocl::time_point::max
		bool LinkNewPacket;								//最新データを常に最上部に表示するよう、表示位置を固定するかどうか？
		const std::vector<datum::id_type>& SelID;		//ID選択表示時に使用
	public:
		boost::signals2::signal<void(seltype)> signal_pushedDatum;		//選択されているデータが変更された場合に発信
	public:
		dxFnPacketList(iterator Begin_,iterator End_,const packet* pInPacket_,const packet* pOutPacket_,const seltype& SelData_,bool LinkNewPacket_,const std::vector<datum::id_type>& SelID_,const dxRGB& InputClr_=CLR::SoftRed,const dxRGB& OutputClr_=CLR::SoftBlue)
			:Begin(std::move(Begin_))
			,End(std::move(End_))
			,pInPacket(pInPacket_)
			,pOutPacket(pOutPacket_)
			,SelData(SelData_)
			,LinkNewPacket(LinkNewPacket_)
			,SelID(SelID_)
			,InputClr(InputClr_)
			,OutputClr(OutputClr_){
		}
		int fndraw(dxO& dxo){
			/*ScrlFrame限定*/
			try{
//				//最新パケットの位置を決定　（-1を底辺=最古パケットとし、どんどんマイナスy方向に描画していく形）
//				int TotalDrawSize=25*(End-Begin)+10;
//				if(pInPacket!=nullptr)TotalDrawSize+=25;
//				if(pOutPacket!=nullptr)TotalDrawSize+=25;

				//描画位置確定
				dxO::Acs<dxScrlFrame> Acs(dxo);
				int ViewPos=Acs().viewpos.y;
				int ViewSize=Acs().size.y;

				dx::draw(Acs().AtoF(Pint(5,5),dxo),dxoStr((boost::format("Top:%d Bottom:%d")%ViewPos%(ViewPos+ViewSize)).str()));

				//y方向の描画位置を覚えておく変数
				int DrawPos;

				//PacketLog内
				{
					//IDの選択が行われていない場合
					if(SelID.size()==0){
						//まず、描画する前後を確保
						auto DBegin=Begin+std::min(std::max(0,(-ViewPos-ViewSize)/25-1),End-Begin);
						auto DEnd=Begin+std::min(std::max(0,(-ViewPos)/25+1),End-Begin);
						//描画位置となるように表示
						if((-ViewPos-ViewSize)/25-1<0)DrawPos=-25;
						else if((-ViewPos-ViewSize)/25-1>End-Begin)DrawPos=-25*(End-Begin);
						else DrawPos=((ViewPos+ViewSize)/25)*25;


						//描画位置のみ描画
						for(auto pitr=DBegin;pitr!=DEnd;++pitr){
							dxRGB Clr;
							if(pitr->first)Clr=InputClr;
							else Clr=OutputClr;

							int DrawXPos=120;

							//時刻描画
							dxo.draw(Pint(0,DrawPos),dxoStrP(Pint(120,25),hmr::time_to_hms(pitr->second.Time),pitr->second.ErrFlag?CLR::LightSoftRed:CLR::LightGray));

							//Packet内データを順に描画
							for(auto itr=pitr->second.Data.begin();itr!=pitr->second.Data.end();++itr){
								bool IsSelected=(SelData.first==pitr->second.No && SelData.second==itr->No);
								if(dxo.draw(Pint(DrawXPos,DrawPos),dxoCatDatum(Pint(70,20),*itr,2,Clr,IsSelected))<0){
									if(IsSelected)signal_pushedDatum(seltype(0,0));
									else signal_pushedDatum(seltype(pitr->second.No,itr->No));
								}
								DrawXPos+=75;
							}
							DrawPos-=25;
						}

						//描画位置を本来の位置に移動
						DrawPos=(End-Begin)*-25-25;
					}//IDが選択されている場合
					else{
						//描画位置が不明なんで、初期地からスタート
						DrawPos=-25;

						//総当たりで調べていく
						for(auto pitr=Begin;pitr!=End;++pitr){
							dxRGB Clr;
							if(pitr->first)Clr=InputClr;
							else Clr=OutputClr;

							int DrawXPos=125;

							bool DrawFlag=false;
							bool ESCFlag=false;

							//時刻描画
							dxo.draw(Pint(0,DrawPos),dxoStrP(Pint(120,25),hmr::time_to_hms(pitr->second.Time),CLR::LightGray));

							//Packet内データを順に描画
							for(auto itr=pitr->second.Data.begin();itr!=pitr->second.Data.end();++itr){
								//IDListと照らし合わせてみる
								for(auto selitr=SelID.begin();selitr!=SelID.end();++selitr){
									//IDListにdatum::IDが含まれている場合
									if(*selitr==itr->ID){
										DrawFlag=true;
										//描画範囲に入っていなければ、チェックだけして脱出
										if(DrawPos<ViewPos-25 || ViewPos+ViewSize+25<DrawPos){
											//Packet内のforからも脱出するようにフラグを立てておく
											ESCFlag=true;
										}else{
											bool IsSelected=(SelData.first==pitr->second.No && SelData.second==itr->No);
											if(dxo.draw(Pint(DrawXPos,DrawPos),dxoCatDatum(Pint(70,20),*itr,2,Clr,IsSelected))<0){
												if(IsSelected)signal_pushedDatum(seltype(0,0));
												else signal_pushedDatum(seltype(pitr->second.No,itr->No));
											}
											DrawXPos+=75;
										}
										break;
									}
								}
								//描画範囲外の場合、パケット内に描画データがあることさえわかればいいので脱出
								if(ESCFlag)break;
							}
							if(DrawFlag)DrawPos-=25;
						}
					}
				}

				//OutPacket内
				if(pOutPacket!=nullptr && SelID.size()==0){
					//描画範囲内の時
					if(ViewPos<=DrawPos+20 && DrawPos<ViewPos+ViewSize){
						dxo.draw(Pint(0,DrawPos),dxoTriangle(Pint(0,20),Pint(5,10),OutputClr));
						int DrawXPos=5;
						//Packet内データを順に描画
						for(auto itr=pOutPacket->Data.begin();itr!=pOutPacket->Data.end();++itr){
							bool IsSelected=(SelData.first==0 && SelData.second==itr->No);
							if(dxo.draw(Pint(DrawXPos,DrawPos),dxoCatDatum(Pint(70,20),*itr,2,OutputClr,IsSelected))<0){
								if(IsSelected)signal_pushedDatum(seltype(0,0));
								else signal_pushedDatum(seltype(0,itr->No));
							}
							DrawXPos+=75;
						}
						DrawPos-=25;
					}
				}

				//InPacket内
				if(pInPacket!=nullptr && SelID.size()==0){
					//描画範囲内の時
					if(ViewPos<=DrawPos+20 && DrawPos<ViewPos+ViewSize){
						dxo.draw(Pint(0,DrawPos),dxoTriangle(Pint(0,20),Pint(5,10),InputClr));
						int DrawXPos=5;
						//Packet内データを順に描画
						for(auto itr=pInPacket->Data.begin();itr!=pInPacket->Data.end();++itr){
							bool IsSelected=(SelData.first==0 && SelData.second==itr->No);
							if(dxo.draw(Pint(DrawXPos,DrawPos),dxoCatDatum(Pint(70,20),*itr,2,InputClr,IsSelected))<0){
								if(IsSelected)signal_pushedDatum(seltype(0,0));
								else signal_pushedDatum(seltype(0,itr->No));
							}
							DrawXPos+=75;
						}
						DrawPos-=25;
					}
				}

				//最後に描画位置の指定を行っておく
				dxo.draw(Pint(0,0),dxoNull());
				dxo.draw(Pint(0,DrawPos+25),dxoNull());
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
	//データの中身を描画する描画関数クラス
	class dxFnDatumView:public dxFn{
	private:
		const datum* Ptr;
	public:
		dxFnDatumView(const datum* Ptr_):Ptr(Ptr_){}
		int fndraw(dxO& dxo){
			/*ScrlFrame限定*/
			try{
				//選択されていないときには、何も表示しない。
				if(Ptr==nullptr){
					dxo.draw(Pint(5,5),dxoStr("No Data"));
					return 0;
				}

				int DrawPos=5;

				//描画幅確定
				dxO::Acs<dxScrlFrame> Acs(dxo);
				//5文字単位のStrBlockの横方向の数
				//	25*5-5=120pixelがStrBlock
				//	両端5+位置情報45=55が固定して必要な長さ
				int StrBlockNum=(Acs().size.x-55)/145;

				//スクロール確保
				dxo.draw(Pint(0,0),dxoNull());
				dxo.draw(Pint(0,50+25*StrBlockNum),dxoNull());

				//描画　まず基礎情報
				dxo.draw(Pint(5,DrawPos),dxoStr((boost::format("ID:%c Size:%d(%d) Err:%d")%Ptr->ID%Ptr->Size%Ptr->Str.size()%Ptr->ErrFlag).str()));
				DrawPos+=20;
				dxo.draw(Pint(5,DrawPos),dxoStr(hmr::time_to_hms(Ptr->Time)));

				//次に文字列情報
				int DrawPosX=5;
				unsigned int Cnt=0;
				for(auto itr=Ptr->Str.begin();itr!=Ptr->Str.end();++itr){
					if((Cnt%(StrBlockNum*5))==0){
						DrawPos+=45;
						DrawPosX=5;
						dxo.draw(Pint(DrawPosX,DrawPos+10),dxoStr((boost::format("%4d")%Cnt).str()));
						DrawPosX+=40+5;	//文字幅40pixel+空白5pixel
					}else if(Cnt%5==0){
						DrawPosX+=10;
					}
					dxo.draw(Pint(DrawPosX,DrawPos),dxoStr((boost::format("%02x")%static_cast<unsigned int>(static_cast<unsigned char>(*itr))).str()));
					dxo.draw(Pint(DrawPosX,DrawPos+20),dxoStr((boost::format("%c")%(*itr)).str()));
					DrawPosX+=25;
					++Cnt;
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
	//dxFnPacketListとdxFnDatumViewを統合した描画クラス
	template<typename iterator>
	class dxosPacketList_withView:public dxReObject,public hmoBox{
		typedef std::pair<packet::no_type,datum::no_type> seltype;
	private:
		seltype SelData;	//現在選択されているデータ
		bool LinkNewPacket;	//NewPacketにリンクさせるかのフラグ
		bool ActiveDatumView;
		std::vector<datum::id_type> IDList;
		dxRGB Clr;
	public:
		int DatumViewSize;	//Datum表示画面のy方向のサイズ
	private:
		hmLib::signals::unique_connections SignalConenctions;
	private:
		void setSelData(seltype SelData_){
			SelData=SelData_;
			signal_setSelData(SelData);
		}
		void setLinkNewPacket(bool LinkNewPacket_){LinkNewPacket=LinkNewPacket_;}
		void syncDatumViewSize(int DatumViewSize_){DatumViewSize=DatumViewSize_;}
		void addIDList(datum::id_type ID_){
			auto itr=IDList.begin();
			for(;itr!=IDList.end();++itr){
				if(*itr!=ID_)break;
			}
			if(itr==IDList.end())IDList.push_back(ID_);
		}
		void clearIDList(datum::id_type ID_){
			auto itr=IDList.begin();
			for(;itr!=IDList.end();++itr){
				if(*itr!=ID_)break;
			}
			if(itr!=IDList.end())IDList.erase(itr);
		}
	public:
		boost::signals2::signal<void(seltype)> signal_setSelData;
		hmLib::inquiries::range_inquiry<iterator> inquiryPacketLog;
		hmLib::inquiries::inquiry<const packet&> inquiryNewInPacket;
		hmLib::inquiries::inquiry<const packet&> inquiryNewOutPacket;
		void slot_addIDList(boost::signals2::signal<void(datum::id_type)>& Signal_){
			SignalConenctions(hmLib::signals::connect(Signal_,[&](datum::id_type ID_)->void{this->addIDList(ID_);}));
		}
		void slot_clearIDList(boost::signals2::signal<void(datum::id_type)>& Signal_){
			SignalConenctions(hmLib::signals::connect(Signal_,[&](datum::id_type ID_)->void{this->clearIDList(ID_);}));
		}
		void slot_setSelData(boost::signals2::signal<void(seltype)>& Signal_){
			SignalConenctions(hmLib::signals::connect(Signal_,[&](seltype SelData_)->void{this->setSelData(SelData_);}));
		}
		void slot_setLinkNewPacket(boost::signals2::signal<void(bool)>& Signal_){
			SignalConenctions(hmLib::signals::connect(Signal_,[&](bool LinkNewPacket_)->void{this->setLinkNewPacket(LinkNewPacket_);}));
		}
	private:
		dxScrlFrame PacketListFrame;
		dxScrlFrame DatumViewFrame;
	public:
		int redraw(dxO& dxo)override{
			if(!inquiryPacketLog.is_connect()){
				dxo.draw(Pint(0,0),dxoStr("NoConnection of RangeInquiry"));
				return 0;
			}
			//Mutex同時ロック
//			std::lock(inquiryPacketLog,inquiryNewInPacket,inquiryNewOutPacket);

			try{
				//DatumViewの設定値が以上でない場合
				const datum* pSelDatum=nullptr;
				std::pair<bool,packet> TargetP;
				datum Target;
				TargetP.second.No=SelData.first;
				Target.No=SelData.second;
				if(0<=DatumViewSize && DatumViewSize<=size.y){
					//Packet::Timeが不定値を指しているときは、現在のパケットから探す
					if(SelData.first==0){
						if(inquiryNewInPacket.is_connect() && inquiryNewInPacket().Data.size()){

							//Inquiryの中身のbegin/endを呼んでいるので、このInquiryは参照取得型(T&)すべき
							//	通常のタイプ(inquiry<T>)を使うと、コピーされたDataのbegin/endを使ってしまう。
							auto rditr=std::equal_range(inquiryNewInPacket().Data.begin(),inquiryNewInPacket().Data.end(),Target);
							
							if(rditr.first!=rditr.second){
								pSelDatum=&(*(rditr.first));
							}
						}
						if(inquiryNewOutPacket.is_connect() && inquiryNewOutPacket().Data.size()){
							// これではエラーが出てしまうので変更
							auto sditr=std::equal_range(inquiryNewOutPacket().Data.begin(),inquiryNewOutPacket().Data.end(),Target);
							
							if(sditr.first!=sditr.second){
								pSelDatum=&(*(sditr.first));
							}
						}
					}else{
						// これではエラーが出てしまうので変更
						 auto pitr=std::equal_range(inquiryPacketLog.begin(),inquiryPacketLog.end(),TargetP,[](const std::pair<bool,packet>& Pair1_,const std::pair<bool,packet>& Pair2_)->bool{return Pair1_.second.No<Pair2_.second.No;});
						
						if(pitr.first!=pitr.second){
							// これではエラーが出るので変更
							auto ditr=std::equal_range((*pitr.first).second.Data.begin(),(*pitr.first).second.Data.end(),Target,[&](const datum& d1,const datum& d2)->bool{return d1.No<d2.No;});

							if(ditr.first!=ditr.second){
								pSelDatum=&(*(ditr.first));
							}
						}
					}
				}
				
				//Datumが選択されていないとき
				if(pSelDatum==nullptr){
					const packet* pInPacket=nullptr;
					const packet* pOutPacket=nullptr;

					if(inquiryNewInPacket.is_connect()){
						if(inquiryNewInPacket().Data.size()){
							pInPacket=&inquiryNewInPacket();
						}
					}
					if(inquiryNewOutPacket.is_connect()){
						if(inquiryNewOutPacket().Data.size()){
							pOutPacket=&inquiryNewOutPacket();
						}
					}

					//パケット描画の設定
					dxFnPacketList<iterator> FnPacketList(inquiryPacketLog.begin(),inquiryPacketLog.end(),pInPacket,pOutPacket,SelData,LinkNewPacket,IDList);
					slot_setSelData(FnPacketList.signal_pushedDatum);
					PacketListFrame.set(FnPacketList,Pint(size.x,size.y),Clr);
					PacketListFrame.resetMoveArea(ALI::all);

//					if(ActiveDatumView){
//						ActiveDatumView=false;
//						PacketListFrame.viewpos.y+=DatumViewSize;
//					}

					//描画
//					dxo.draw(Pint(0,0),dxoStr("PacketList"));
					dxo.draw(Pint(0,0),PacketListFrame);
				}//Datumが選択されている時
				else{
					const packet* pInPacket=nullptr;
					const packet* pOutPacket=nullptr;

					if(inquiryNewInPacket.is_connect()){
						if(inquiryNewInPacket().Data.size()){
							pInPacket=&inquiryNewInPacket();
						}
					}
					if(inquiryNewOutPacket.is_connect()){
						if(inquiryNewOutPacket().Data.size()){
							pOutPacket=&inquiryNewOutPacket();
						}
					}

					//パケット描画の設定
					dxFnPacketList<iterator> FnPacketList(inquiryPacketLog.begin(),inquiryPacketLog.end(),pInPacket,pOutPacket,SelData,LinkNewPacket,IDList);
					slot_setSelData(FnPacketList.signal_pushedDatum);
					PacketListFrame.set(FnPacketList,Pint(size.x,size.y-DatumViewSize),Clr);
					PacketListFrame.resetMoveArea(ALI::all);

//					if(!ActiveDatumView){
//						ActiveDatumView=true;
//						PacketListFrame.viewpos.y-=DatumViewSize;
//					}

					//データ描画用Frameの設定
					dxFnDatumView FnDatumView(pSelDatum);
					DatumViewFrame.set(FnDatumView,Pint(size.x,DatumViewSize),Clr);

					//描画
					dxo.draw(Pint(0,0),PacketListFrame);
					dxo.draw(Pint(0,DatumViewSize),DatumViewFrame);
				}
			}catch(const std::exception& Except){
				dxo.draw(Pint(5, 5),dxoStr("***ERROR***"));
				dxo.draw(Pint(5,25),dxoStr(Except.what()));
				printfDx(Except.what());
				printfDx("\n");
			}catch(const std::string& Str){
				dxo.draw(Pint(5, 5),dxoStr("***ERROR***"));
				dxo.draw(Pint(5,25),dxoStr(Str));
			}

			//Mutexアンロック
//			inquiryPacketLog.unlock();
	//		inquiryNewInPacket.unlock();
		//	inquiryNewOutPacket.unlock();

			return 0;
		}
		dxosPacketList_withView(Pint Size_,int DatumViewSize_,const dxRGB& Clr_)
			:hmoBox(Size_),SelData(0,0),LinkNewPacket(false),DatumViewSize(DatumViewSize_),Clr(Clr_),ActiveDatumView(false){
			PacketListFrame.resetMoveArea(ALI::all);
			PacketListFrame.setMoveArea(ALI::bottom|ALI::left,Pint(0,0));
			PacketListFrame.viewpos.y=-Size_.y;
		}
	};
}
#
#endif
