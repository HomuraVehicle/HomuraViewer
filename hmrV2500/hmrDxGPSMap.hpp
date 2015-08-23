#ifndef HMR_DXGPSMAP_INC
#define HMR_DXGPSMAP_INC 100
#
/*===hmrDxGPSMap===
GPS簡易マップを提供する

*/
#include<mutex>
#include<algorithm>
#include<string>
#include<fstream>
#include<boost/format.hpp>
#include<hmLib_v3_05/position.hpp>
#include<hmLib_v3_05/signals.hpp>
#include<hmLib_v3_05/inquiries.hpp>
#include"hmLibVer.hpp"
#include<hmLib_v2/dxArea.hpp>
#include<hmLib_v2/dxObject.hpp>
#include<hmLib_v2/dxColorSet.hpp>
#include"hmrGPSDataSet.hpp"
#include"coordinates.hpp"
namespace hmr{
	class dxosGPSMap:public dxReObject,public hmoBox{
		typedef std::mutex mutex;
		typedef std::lock_guard<mutex> lock;
		typedef hmLib::pdouble pdouble;
	private:
		mutex Mx;
	private:
		//経度緯度から地図座標への変換モジュール
		class map_positioner{
			//GPSデータから地図座標への変換プロセス
			pdouble GPSScale;		//GPSマップ上のスケーリング 経度/緯度->メートル
			pdouble GPSRefPos;		//GPS基準位置となる緯度経度北がx軸正、東がy軸正
		public:
			map_positioner()
				:GPSRefPos(0,0)
				,GPSScale(0,0){
			}
			//経度緯度情報を地図座標に変換する
			Pint gps_to_map(pdouble GPSPos_)const{
				pdouble Pos=(GPSPos_-GPSRefPos)*GPSScale;
				return Pint(Pos.x,-Pos.y);
			}
			//地図座標を経度緯度情報に変換する
			pdouble map_to_gps(Pint MapPos_)const{
				pdouble Pos(MapPos_.x,-MapPos_.y);
				return (Pos/GPSScale)+GPSRefPos;
			}
			//現在の座標をもとに初期化
			void activate(pdouble GPSPos_){
				//変なデータだとはじく
				if(GPSPos_.x==0 && GPSPos_.y==0)return;
			
				//最後に受信した座標位置を基準位置とする
				GPSRefPos=GPSPos_;
				
				//緯度経度<=>距離変換用のGPSScale設定準備
				double p=GPSRefPos.y/180.*pi();	//地理緯度
				double a=6378137;		//地球の赤道半径
				double e2=0.00669438;	//地球の離心率の二条
				
				//地球を楕円と考えるGRS80に基づいて、GPSScale設定
				GPSScale.x=a*cos(p)/sqrt(1-e2*sin(p)*sin(p))*hmLib::pi()/180.;
				GPSScale.y=a*(1-e2)/pow(1-e2*sin(p)*sin(p),1.5)*hmLib::pi()/180.;
			}
			//不活性化する
			void deactivate(){
				GPSRefPos.x=0;
				GPSRefPos.y=0;
			}
			//初期化が完了しているかを確認
			bool is_active()const{
				return GPSRefPos.x!=0 || GPSRefPos.y!=0;
			}
		};
		//GPS情報を記録しておく地図 GPS基準位置が中心
		class plot_logger{
			dxScreen Grp;		//描画地図
			Pint GrpPos;		//描画地図基準位置
			Pint GrpSize;		//描画地図基準サイズ

			dxImGraph PlotImGrp;	//記録地図 1pixel=1メートル
			dxImGraph TmpImGrp;		//Screenへのコピー用ImGrp
			Pint PlotPos;			//記録地図基準位置（左上の地図座標）
			Pint PlotSize;			//記録地図サイズ
		public:
			dxRGB Bright;
		public:
			plot_logger():Bright(255,255,255){reset();}
			void reset(){
				PlotImGrp.make(Pint(400,400),true,false);
				PlotPos=Pint(-200,-200);
				PlotSize=Pint(400,400);
				PlotImGrp.fillAlpha(0);
			}
			void screen(dxScreen& Screen_,Pint MapPos_,Pint MapSize_){
				//描画範囲決定
				Pint GrpPos1(
					std::min(std::max(0,MapPos_.x-PlotPos.x),PlotSize.x),
					std::min(std::max(0,MapPos_.y-PlotPos.y),PlotSize.y)
				);
				Pint GrpPos2(
					std::min(std::max(0,MapPos_.x+MapSize_.x-PlotPos.x),PlotSize.x),
					std::min(std::max(0,MapPos_.y+MapSize_.y-PlotPos.y),PlotSize.y)
				);
				//描画すべき範囲が存在しなかった場合は、描画せず終了
				if(GrpPos1.x==GrpPos2.x || GrpPos1.y==GrpPos2.y)return;
				
				Pint AdjPos(
					std::max(0,PlotPos.x-MapPos_.x),		//この辺りが怪しい
					std::max(0,PlotPos.y-MapPos_.y)
				);

				//一時描画用dxImGraphを初期化
				if(TmpImGrp.getSize()!=MapSize_){
					TmpImGrp.make(MapSize_,true);
				}
				TmpImGrp.fill(dxRGB(0,0,0),0);
				TmpImGrp.copy(AdjPos,GrpPos2-GrpPos1,PlotImGrp,GrpPos1);

				SetDrawValidAlphaChannelGraphCreateFlag(TRUE);
				Screen_.load(TmpImGrp);
				SetDrawValidAlphaChannelGraphCreateFlag(FALSE);
			}
			void plot(Pint MapPos_){
				//描画後の新たなPlotPos/Size
				Pint NewPlotPos=PlotPos;
				Pint NewPlotSize=PlotSize;

				//プロット位置がはみ出ていた場合、マップを拡大
				while(MapPos_.x<PlotPos.x){
					NewPlotPos.x-=200;
					NewPlotSize.x+=200;
				}
				while(PlotPos.x+PlotSize.x<MapPos_.x){
					NewPlotSize.x+=200;
				}
				while(MapPos_.y<PlotPos.y){
					NewPlotPos.y-=200;
					NewPlotSize.y+=200;
				}
				while(PlotPos.y+PlotSize.y<MapPos_.y){
					NewPlotSize.y+=200;
				}

				//マップ更新が必要だった場合
				if(NewPlotSize!=PlotSize){
					TmpImGrp.make(NewPlotSize,true,false);
					TmpImGrp.copy(PlotPos-NewPlotPos,PlotSize,PlotImGrp,Pint(0,0));
					TmpImGrp.swap(PlotImGrp);
					PlotPos=NewPlotPos;
					PlotSize=NewPlotSize;
				}

				//実際にプロットする
				PlotImGrp.put(MapPos_-PlotPos,dxRGB(255,255,255),255);

				//自身の保持する画面にも描画する
				if(GrpPos.x<=MapPos_.x && MapPos_.x<GrpPos.x+GrpSize.x && GrpPos.y<=MapPos_.y && MapPos_.y<GrpPos.y+GrpSize.y){
					dx::setDrawScreen(Grp);
					dx::drawPixel(MapPos_-GrpPos,dxRGB(255,255,255));
					dx::resetDrawScreen();
				}
			}
			void draw(dxO& dxo,Pint MapPos_,Pint MapSize_,Pint DrawSize_,dxRGB Bright_){
//				dxo.draw(Pint(0,0),dxoBox(DrawSize_,CLR::Pink));

				//loadしていなければ、終える
				if(PlotImGrp.getHandle()<=0)return;

				if(Grp.getHandle()<=0){
					GrpSize=DrawSize_*2;
					GrpPos=MapPos_-GrpSize/4;
					screen(Grp,GrpPos,GrpSize);
				}else if(MapPos_.x<GrpPos.x || MapPos_.y<GrpPos.y || MapPos_.x+MapSize_.x>GrpPos.x+GrpSize.x || MapPos_.y+MapSize_.y>GrpPos.y+GrpSize.y){
					GrpPos=MapPos_-(GrpSize-MapSize_)/2;
					screen(Grp,GrpPos,GrpSize);
				}
				Pdouble Rate=Pdouble(DrawSize_)/Pdouble(MapSize_);
//				dx::drawGraph(dxo.DtoG(Pint((GrpPos-MapPos_)*Rate)),Grp,Pint(GrpSize*Rate),1);
				dxo.draw(Pint((GrpPos-MapPos_)*Rate),dxoGraph(Grp,Pint(GrpSize*Rate),true),true,dxDMode(Bright_));
			}
		};
		//背景に表示する地図情報
		class bmap_logger{
			dxScreen Grp;		//描画地図
			Pint GrpPos;		//描画地図基準位置
			Pint GrpSize;		//描画地図基準サイズ

			dxImGraph BmapImGrp;	//背景地図
			dxImGraph TmpImGrp;		//Screenへのコピー用ImGrp
//			Pdouble BmapScale;		//背景地図スケール メートル->pixel
			Pint BmapPos;			//背景地図基準位置（左上の地図座標）
			Pint BmapSize;			//背景地図サイズ（左上から右下までの地図座標距離）
			std::string BmapName;	//地図名
			pdouble BmapGPS;		//地図基準座標
		public:
			void screen(dxScreen& Screen_,Pint MapPos_,Pint MapSize_){
				//描画範囲決定
				Pint GrpPos1(
					std::min(std::max(0,MapPos_.x-BmapPos.x),BmapSize.x),
					std::min(std::max(0,MapPos_.y-BmapPos.y),BmapSize.y)
				);
				Pint GrpPos2(
					std::min(std::max(0,MapPos_.x+MapSize_.x-BmapPos.x),BmapSize.x),
					std::min(std::max(0,MapPos_.y+MapSize_.y-BmapPos.y),BmapSize.y)
				);
				//描画すべき範囲が存在しなかった場合は、描画せず終了
				if(GrpPos1.x==GrpPos2.x || GrpPos1.y==GrpPos2.y)return;
				
				Pint AdjPos(
					std::max(0,BmapPos.x-MapPos_.x),		//この辺りが怪しい
					std::max(0,BmapPos.y-MapPos_.y)
				);

				//一時描画用dxImGraphを初期化
				if(TmpImGrp.getSize()!=MapSize_){
					TmpImGrp.make(MapSize_,true);
				}
				TmpImGrp.fill(dxRGB(0,0,0),255);
				TmpImGrp.copy(AdjPos,GrpPos2-GrpPos1,BmapImGrp,GrpPos1);
				Screen_.load(TmpImGrp);
			}
			void load(const std::string& FileName_,const map_positioner& MapPositioner_){
				std::ifstream fin(FileName_);

//				double Scale;	//倍率
				fin>>BmapName;
				fin>>BmapGPS.y>>BmapGPS.x;
//				fin>>Scale;

//				BmapScale.set(Scale,Scale);
				BmapImGrp.open("hmr/DxGPSMap/map/"+BmapName);
				BmapSize=BmapImGrp.getSize();

				if(MapPositioner_.is_active()){
					BmapPos=MapPositioner_.gps_to_map(BmapGPS);
				}else{
					BmapPos.set(0,0);
				}
			}
			void setMapPos(Pint BmapPos_){
				GrpPos+=BmapPos_-BmapPos;
				BmapPos=BmapPos_;
			}
			Pint getMapPos()const{return BmapPos;}
			Pint getMapSize()const{return BmapSize;}
			std::string getMapName()const{return BmapName;}
			pdouble getMapGPS()const{return BmapGPS;}
			Pint getGrpPos()const{return GrpPos;}
			Pint getGrpSize()const{return GrpSize;}
			void draw(dxO& dxo,Pint MapPos_,Pint MapSize_,Pint DrawSize_){
				dxo.draw(Pint(0,0),dxoBox(DrawSize_,CLR::DarkGray));

				//loadしていなければ、終える
				if(BmapImGrp.getHandle()<=0)return;

				if(Grp.getHandle()<=0){
					GrpSize=DrawSize_*2;
					Grp.make(GrpSize);
					GrpPos=MapPos_-GrpSize/4;
					screen(Grp,GrpPos,GrpSize);
				}else if(MapPos_.x<GrpPos.x || MapPos_.y<GrpPos.y || MapPos_.x+MapSize_.x>GrpPos.x+GrpSize.x || MapPos_.y+MapSize_.y>GrpPos.y+GrpSize.y){
					GrpPos=MapPos_-(GrpSize-MapSize_)/2;
					screen(Grp,GrpPos,GrpSize);
				}
				Pdouble Rate=Pdouble(DrawSize_)/Pdouble(MapSize_);
				dxo.draw(Pint((GrpPos-MapPos_)*Rate),dxoGraph(Grp,Pint(GrpSize*Rate)));
			}
		};
	private:
		//データホルダー
		plot_logger Plot;
		bmap_logger Bmap;
		map_positioner Positioner;
		//描画関連
		Pint DrawPos;			//記録地図の表示位置（左上の地図座標）
		Pint DrawSize;			//記録地図の表示サイズ
		double DrawScale;		//描画スケール
		//データ関連
		bool IsConfigMode;		//設定変更窓を開いている場合
		pdouble LastGPSPos;		//最後に受信したGPS座標
		double Theta;			//コンパスからの方位情報
		bool MapMoveMode;		//マップ動作モード
		Pint HoldMapPos;		//マウス保持時の位置情報
		//描画色情報
		dxRGB PlotClr;
		clock::time_point PlotClrTime;
	private:
		//初期化する
		void activate(){
			Positioner.activate(LastGPSPos);
			DrawPos=-size/2;
			DrawSize=size;
			DrawScale=1.0;
			MapMoveMode=false;
			PlotClr.set(255,255,255);
			PlotClrTime=clock::now();
		}
		//初期化する
		void deactivate(){
			Positioner.deactivate();
			Plot.reset();
		}		//初期化済みか確認
		bool is_active()const{return Positioner.is_active();}
		//地図データを読み込む
		void load(){
			Bmap.load("hmr/DxGPSMap/map.txt",Positioner);
		}
		//GPSデータを追加する
		void addPoint(pdouble GPSPos_){
			//とりあえず記録
			LastGPSPos=GPSPos_;
		
			//Positionerがアクティブ化されていなければ終了
			if(!Positioner.is_active())return;
			
			//Map座標計算
			Pint MapPos=Positioner.gps_to_map(GPSPos_);

			//10kmを超える距離が基準座標からある場合には、はじく
			if(MapPos.x*MapPos.x+MapPos.y*MapPos.y>100000000)return;
			
			//Plotに描画する
			Plot.plot(MapPos);
		
		}
	private:
		hmLib::signals::unique_connections SignalConnections;
		//hmLib::inquiries::unique_connections InquiryConnections;
	public:
/*		void slot_activate(boost::signals2::signal<void(void)>& signal_){
			SignalConnections(hmLib::signals::connect(signal_,[&](void)->void{this->activate();}));
		}
		void contact_is_active(hmLib::inquiries::inquiry<bool>& inquiry_){
			InquiryConnections(hmLib::inquiries::connect(inquiry_,[&](void)->bool{this->is_active();}));
		}
		void slot_load(boost::signals2::signal<void(void)>& signal_){
			SignalConnections(hmLib::signals::connect(signal_,[&](void)->void{this->load();}));
		}
		void slot_setMoveMapMode(boost::signals2::signal<void(bool)>& signal_){
			SignalConnections(hmLib::signals::connect(signal_,[&](bool b)->void{this->MapMoveMode=b;}));
		}
		void contact_getMoveMapMode(hmLib::inquiries::inquiry<bool>& inquiry_){
			InquiryConnections(hmLib::inquiries::connect(inquiry_,MapMoveMode));
		}
*/
		hmLib::inquiries::inquiry<double> inquiry_getMagneticAngle; 
		void slot_addNewData(boost::signals2::signal<void(const GPSDataSet&, clock::time_point)>& signal_){
			SignalConnections(hmLib::signals::connect(signal_,
				[&](const GPSDataSet& Data_, clock::time_point t)->void{
						lock Lock(this->Mx);
						if(!(Data_.getErr()||Data_.getFail()))this->addPoint(Data_.getPos());
					}
				)
			);
		}
	private:
		dxArea Area;
		class fnDraw:public dxFn{
		private:
			dxosGPSMap* Ptr;
		public:
			fnDraw(dxosGPSMap* Ptr_):Ptr(Ptr_){}
			int fndraw(dxO& dxo){
				//背景地図描画
				Ptr->Bmap.draw(dxo,Ptr->DrawPos,Pint(Ptr->DrawSize/Ptr->DrawScale),Ptr->DrawSize);

				//Plotを描画
				Ptr->Plot.draw(dxo,Ptr->DrawPos,Pint(Ptr->DrawSize/Ptr->DrawScale),Ptr->DrawSize,Ptr->PlotClr);

				//現在の位置の描画すべき位置を取得
				Pint DPos=Pint((Ptr->Positioner.gps_to_map(Ptr->LastGPSPos)-Ptr->DrawPos)*Ptr->DrawScale+Pdouble(Ptr->DrawScale/2.,Ptr->DrawScale/2.));

				//コンパスとの連動描画
				double Angle=-1.0;
				if(Ptr->inquiry_getMagneticAngle.is_connect()){
					Angle=Ptr->inquiry_getMagneticAngle();
				}
				//不正な値の場合には、丸だけ
				if(Angle<0. || Angle>2*pi()){
					dxo.draw(DPos,dxoCircle(10,Ptr->PlotClr),true,dxDMode(128));
				}else{
					Angle=pi()/2.-Angle;
					dxo.draw(DPos,dxoTriangle(Pint(-5*cos(Angle)+20*sin(Angle),5*sin(Angle)+20*cos(Angle)),Pint(5*cos(Angle)+20*sin(Angle),-5*sin(Angle)+20*cos(Angle)),Ptr->PlotClr),true,dxDMode(128));
				}

				return 0;
			}
		};
	public:
		dxosGPSMap(Pint Size_)
			:hmoBox(Size_)
			,PlotClr(255,255,255)
			,IsConfigMode(true){
		}
		int redraw(dxO& dxo){
			lock Lock(Mx);
			//すでに初期化されている場合
			if(Positioner.is_active()){
				//色の設定
				long long int ms=std::chrono::duration_cast<std::chrono::milliseconds>(clock::now()-PlotClrTime).count();
				PlotClr.r=255;
				PlotClr.g=static_cast<unsigned char>(abs((ms%510)-255));
				PlotClr.b=64;

				//Plot&Bmap描画
				fnDraw Fn(this);
				Area.set(Fn,size);
				dxo.draw(Pint(0,0),Area);

				//地図修正モード
				if(MapMoveMode){
					Pint MosPos=dxo.GtoD(dx::getMosPos());
					int LimX=0;
					if(IsConfigMode)LimX=160;
					if(LimX<=MosPos.x&&MosPos.x<size.x&&0<=MosPos.y&&MosPos.y<size.y){
						if(dx::getMosL()==1){
							HoldMapPos=Bmap.getMapPos();
						}else if(dx::getMosL()==2){
							Bmap.setMapPos(Pint(HoldMapPos+(dx::getMosPos()-dx::getMosLPos())/DrawScale));
						}
					}
				}//地図移動モード
				else{
					Pint MosPos=dxo.GtoD(dx::getMosPos());
					int LimX=0;
					if(IsConfigMode)LimX=160;
					if(LimX<=MosPos.x&&MosPos.x<size.x&&0<=MosPos.y&&MosPos.y<size.y){
						if(dx::getMosL()==1){
							HoldMapPos=DrawPos;
						}else if(dx::getMosL()==2){
							DrawPos=Pint(HoldMapPos+(dx::getMosLPos()-dx::getMosPos())/DrawScale);
						}
					}
				}

				//地図の拡大縮小
				if(dx::getMosH()){
					Pint Center=dxo.GtoD(dx::getMosPos())/DrawScale+DrawPos;
					DrawScale*=std::pow(2.,dx::getMosH());
					if(DrawScale<0.5)DrawScale=0.5;
					else if(DrawScale>8.0)DrawScale=8.0;
					DrawPos=Center-dxo.GtoD(dx::getMosPos())/DrawScale;
				}
			}//初期化が済んでいない場合
			else{
				dxo.draw(Pint(0,0),dxoStrP(size,"Inactive"));
			}

			//COnfig画面の描画
			if(IsConfigMode){
				bool IsActive=Positioner.is_active();

				if(!IsActive){
					dxo.draw(Pint(0,20),dxoBox(Pint(160,95),CLR::White),true,dxDMode(128));

					if(dxo.draw(Pint(5,25),dxoButIO(Pint(90,20),"Inactive",CLR::SoftRed,false))==1){
						activate();
					}

					//最後の座標
					dxo.draw(Pint(5,50),dxoStr((boost::format("%c:%10.6f")%(LastGPSPos.y>0?'N':'S')%abs(LastGPSPos.y)).str()));
					dxo.draw(Pint(5,75),dxoStr((boost::format("%c:%10.6f")%(LastGPSPos.x>0?'E':'W')%abs(LastGPSPos.x)).str()));

				}else{
					dxo.draw(Pint(0,20),dxoBox(Pint(160,340),CLR::White),true,dxDMode(128));

					if(dxo.draw(Pint(5,25),dxoButIO(Pint(90,20),"Active",CLR::SoftBlue,true))==1){
						deactivate();
					}

					//基準座標
					pdouble StdGPS=Positioner.map_to_gps(Pint(0,0));
					dxo.draw(Pint(5,50),dxoStr((boost::format("%c:%10.6f")%(StdGPS.y>0?'N':'S')%abs(StdGPS.y)).str(),CLR::SoftBlue));
					dxo.draw(Pint(5,70),dxoStr((boost::format("%c:%10.6f")%(StdGPS.x>0?'E':'W')%abs(StdGPS.x)).str(),CLR::SoftBlue));

					//最後の座標
					Pint LastMapPos=Positioner.gps_to_map(LastGPSPos);
					dxo.draw(Pint(5,95),dxoStr((boost::format("(%5d,%5d)")%LastMapPos.x%LastMapPos.y).str()));

					//表示座標&倍率
					dxo.draw(Pint(5,120),dxoStr((boost::format("(%5d,%5d)")%DrawPos.x%DrawPos.y).str()));
					dxo.draw(Pint(5,140),dxoStr((boost::format("-(%5d,%5d)")%(DrawPos.x+DrawSize.x/DrawScale)%(DrawPos.y+DrawSize.y/DrawScale)).str()));
					dxo.draw(Pint(5,160),dxoStr((boost::format("×%.1f")%DrawScale).str()));

					if(dxo.draw(Pint(5,185),dxoBut(Pint(70,20),"Load",CLR::SoftBlue))==-1){
						load();
					}

					if(dxo.draw(Pint(85,185),dxoButIO(Pint(70,20),"MapMove",CLR::SoftBlue,MapMoveMode))==-1){
						MapMoveMode=!MapMoveMode;
					}
					dxo.draw(Pint(5,210),dxoStr(Bmap.getMapName()));
					Pint BmapPos1=Bmap.getMapPos();
					Pint BmapPos2=BmapPos1+Bmap.getMapSize();
					dxo.draw(Pint(5,230),dxoStr((boost::format("(%5d,%5d)")%BmapPos1.x%BmapPos1.y).str()));
					dxo.draw(Pint(5,250),dxoStr((boost::format("-(%5d,%5d)")%BmapPos2.x%BmapPos2.y).str()));

					Pint BmapGrpPos1=Bmap.getGrpPos();
					Pint BmapGrpPos2=BmapGrpPos1+Bmap.getGrpSize();
					dxo.draw(Pint(5,270),dxoStr((boost::format("(%5d,%5d)")%BmapGrpPos1.x%BmapGrpPos1.y).str()));
					dxo.draw(Pint(5,290),dxoStr((boost::format("-(%5d,%5d)")%BmapGrpPos2.x%BmapGrpPos2.y).str()));
				}	
			}

			if(dxo.draw(Pint(0,0),dxoButIO(Pint(20,20),"",CLR::Red,IsConfigMode),true,dxDMode(196))==1){
				IsConfigMode=!IsConfigMode;
			}

			return 0;
		}
	};
}
#
#endif
