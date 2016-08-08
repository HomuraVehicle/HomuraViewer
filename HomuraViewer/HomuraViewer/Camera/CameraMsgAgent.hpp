#ifndef HMR_VIEWER_CAMERA_CAMERAMSGAGENT_INC
#define HMR_VIEWER_CAMERA_CAMERAMSGAGENT_INC 103
#
/*=======hmrSprite=========
hmrSprite:v1_03/130706 hmito
	DataSizeを取得した段階で、DataPosも初期化するように修正
hmrSprite v1_02/130511 iwahori
	DoSetTakePicture,DoCommandReset,DoPowerResetへのcontact追加
	情報取得のlisten/talkを最新の通信規約に対応
	setCommandResetとsetPowerReset関数の引数を変更
hmrSprite v1_01/130427 iwahori
	hmLibのバージョン変更に伴う微修正
	FlagirlのPicとrequestへのinquiryを追加
hmrSprite v1_00/130420 iwahori
	cSpriteMsgAgentのsignal-slot,inquiry-contactへの対応完了
2013.01.27 by amby
  　新通信規約に対応させる。
*/
#include "hmLibVer.hpp"
#include <boost/signals2.hpp>
#include <hmLib/signals.hpp>
#include <hmLib/inquiries.hpp>
#include <HomuraViewer/Message/ItfMessageAgent.hpp>
#include <HomuraViewer/modeflags.hpp>

namespace hmr{
	namespace viewer{
		namespace camera{
			class cMsgAgent :public itfMessageAgent{
			private:
				//データ関連
				bool IsData;				//データ取得中か？
				std::string DataStr;		//データ自体
				unsigned int DataPos;		//データ位置
				unsigned int DataSize;		//データサイズ
				clock::time_point DataTime;		//データ時間

				//状況
				unsigned char Status;		//カメラステータス
				bool IsErr;					//エラー状態か？
				unsigned char ErrCode;		//エラーコード
				clock::time_point StatusTime;//ステータス時間

				//写真撮影要求フラグ
				bool DoSetTakePicture;
				//指定写真サイズ
				unsigned char PictureSize;   //PC側の写真サイズ指定

				//コマンドリセット要求フラグ
				bool DoCommandReset;
				//パワーリセット要求時フラグ
				bool DoPowerReset;
			public:
				//オートパワーリセット
				modeflags AutoPowerReset;
				//オートライト点灯機能
				modeflags AutoLight;
				//ライトON/OFF
				modeflags Light;
				//連続撮影モード
				modeflags AutoTakePic;
				//ミニパケット送信モード
				modeflags MiniPacketMode;
				//情報取得モード
				modeflags InfoMode;
				//受信データシグナル発信
				boost::signals2::signal<void(std::string,clock::time_point)> signal_newPicture;
			private:
				unsigned int MsgPos;
			public:
				cMsgAgent()
					: DataStr()
					, DataPos(0)
					, DataSize(0)
					, PictureSize(0)
					, DoCommandReset(false)
					, DoPowerReset(false)
					, DoSetTakePicture(false)
					, Status(0x00)
					, IsErr(false)
					, ErrCode(0x00){
				}
				bool listen(datum::time_point Time_, bool Err_, const std::string& Str_)override{
					//カメラのステータス取得
					unsigned char c = static_cast<unsigned char>(Str_[0]);

					//カメラから撮影開始を確認
					//  写真サイズとデータサイズを取り出す
					// 　auto reset とかされてしまうと、カメラの状態が分からないので、とりあえずこのコマンドでカメラの現在状況を必ず撮影状態にしておくことにする
					if(c == 0x00){
						// データが足りない時
						if(Str_.size()<4){
							return true;
						}

						// 写真サイズとデータサイズを取り出す
						IsData = true;
						DataPos = 0;
						DataSize = static_cast<unsigned int>(static_cast<unsigned char>(Str_.at(2))) + (static_cast<unsigned int>(static_cast<unsigned char>(Str_.at(3)) << 8));

						//DataStrをクリア
						DataStr.assign(DataSize, 0x00);

						return false;

					}//カメラから撮影中データが得られたとき
					else if(c == 0x01){
						//データが足りない時
						if(Str_.size()<3)return true;

						//実際にカメラからデータを待っていた時
						// 変更の可能性があるため、ここは保留する(2012.1.27)
						// dataPos が、ここではこれまでにゲットしたデータ数になっているが、ここをどうするか？？
						if(IsData){
							//データサイズ
							DataPos = static_cast<unsigned int>(static_cast<unsigned char>(Str_[1])) + (static_cast<unsigned int>(static_cast<unsigned char>(Str_[2])) << 8);
							//			unsigned int DataBegin=DataPos-(Str_.size()-3);
							unsigned int DataBegin = DataPos;

							//データ書き込み
							for(unsigned int i = 0; i<Str_.size() - 3; ++i){
								if(DataSize <= DataBegin + i)break;
								DataStr.at(DataBegin + i) = Str_.at(i + 3);
							}

							//全てのデータを受信したとき
							//			if(DataSize<=DataPos){
							if(DataSize <= DataBegin + Str_.size() - 3){
								DataTime = Time_;
								signal_setPicture(DataStr, DataTime);
								IsData = false;
							}
							return false;
						}//要求してない時
						else{
							return false;
						}
					}//コマンドリセット要求成功時
					else if(c == 0x60){
						DoCommandReset = false;
						return false;
					}//リセット要求成功時
					else if(c == 0x70){
						DoPowerReset = false;
						return false;
					}
					//オートリセット変更時
					else if((c & 0xFE) == 0x80){
						AutoPowerReset.set_pic(c == 0x80);
						return false;
					}
					//ライト点灯状態変更
					else if((c & 0xFE) == 0x30){
						Light.set_pic(c == 0x30);
						return false;
					}
					//オートライト(自動フラッシュ)変更
					else if((c & 0xFE) == 0x40){
						AutoLight.set_pic(c == 0x40);
						return false;
					}
					//ミニパケット送信モード変更
					else if((c & 0xFE) == 0x50){
						MiniPacketMode.set_pic(c == 0x50);
						return false;
					}
					// 連続撮影モード変更
					else if((c & 0xFE) == 0x10){
						AutoTakePic.set_pic(c == 0x10);
						return false;
					}
					// 内部情報更新
					else if(c == 0xA0){
						// データが足りない時
						if(Str_.size()<4){
							return true;
						}

						Status = static_cast<unsigned char>(Str_[1]);
						IsErr = (static_cast<unsigned char>(Str_[2]) != 0);
						ErrCode = static_cast<unsigned char>(Str_[3]);
						StatusTime = Time_;

						return false;
					}
					// 内部情報送信モード変更
					else if((c & 0xFE) == 0xB0){
						InfoMode.set_pic(c == 0xB0);
						return false;
					}

					return true;
				}
				bool talk(std::string& Str)override{
					Str = "";
					switch(MsgPos){
					case 0:
						++MsgPos;
						//PowerReset要求時
						if(DoPowerReset){
							DoPowerReset = false;
							Str.push_back(static_cast<unsigned char>(0x70));
							return false;
						}
					case 1:
						++MsgPos;
						//AutoPowerReset設定変更時
						if(AutoPowerReset){
							if(AutoPowerReset.get_req())Str.push_back(static_cast<unsigned char>(0x80));
							else Str.push_back(static_cast<unsigned char>(0x81));
							return false;
						}
					case 2:
						++MsgPos;
						//CommandReset要求時
						if(DoCommandReset){
							DoCommandReset = false;
							Str.push_back(static_cast<unsigned char>(0x60));
							return false;
						}
					case 3:
						++MsgPos;
						//オートライト設定時
						if(AutoLight){
							if(AutoLight.get_req())Str.push_back(static_cast<unsigned char>(0x40));
							else Str.push_back(static_cast<unsigned char>(0x41));
							return false;
						}
					case 4:
						++MsgPos;
						//ライトの設定
						if(Light){
							if(Light.get_req())Str.push_back(static_cast<unsigned char>(0x30));
							else Str.push_back(static_cast<unsigned char>(0x31));
							return false;
						}
					case 5:
						++MsgPos;
						//MIniパケットモードの設定
						if(MiniPacketMode){
							if(MiniPacketMode.get_req())Str.push_back(static_cast<unsigned char>(0x50));
							else Str.push_back(static_cast<unsigned char>(0x51));
							return false;
						}
					case 6:
						++MsgPos;
						// 連続撮影モードの設定
						if(AutoTakePic){
							if(AutoTakePic.get_req()){
								Str.push_back(static_cast<unsigned char>(0x10));
								Str.push_back(static_cast<unsigned char>(0x00));// Picture サイズは一番小さいもの
							} else Str.push_back(static_cast<unsigned char>(0x11));
							return false;
						}
					case 7:
						++MsgPos;
						//写真要求モードになっている
						if(DoSetTakePicture){
							DoSetTakePicture = false;
							//PictureSizeを適正値に補正
							if(PictureSize<0)PictureSize = 0;
							else if(PictureSize>2)PictureSize = 2;
							//コマンド0x00送信
							Str.push_back(static_cast<unsigned char>(0x00));
							Str.push_back(PictureSize);
							return false;
						}
					case 8:
						++MsgPos;
						// 内部情報送信モードの設定
						if(InfoMode){
							if(InfoMode.get_req())Str.push_back(static_cast<unsigned char>(0xB0));
							else Str.push_back(static_cast<unsigned char>(0xB1));
							return false;
						}
					default:
						return true;
					}
				}
				void setup_listen(void)override{
					ErrCode = 0x00;
					IsErr = false;
				}
				void setup_talk(void)override{
					MsgPos = 0;
				}
			private:
				hmLib::signals::unique_connections SignalConnections;
				hmLib::inquiries::unique_connections InquiryConnections;

			public:
				void setTakePicture(){
					InfoMode.set_req(true);
					DoSetTakePicture = true;
				}
				void setTakePictureLarge(){
					PictureSize = 2;
					setTakePicture();
				}

				void setCommandReset(){ DoCommandReset = true; }
				void setPowerReset(){ DoPowerReset = true; }

				void setPictureSize(unsigned char Size_){
					if(Size_>2)return;
					PictureSize = Size_;
				}
				unsigned char getPictureSize()const{ return PictureSize; }

				bool getIsData()const{ return IsData; }
				std::pair<unsigned int, unsigned int> getDataPosSize()const{
					return std::make_pair(DataPos, DataSize);
				}
				clock::time_point getDataTime()const{ return DataTime; }

				unsigned char getStatus()const{ return Status; }
				bool getIsErr()const{ return IsErr; }
				unsigned char getErrCode()const{ return ErrCode; }
				clock::time_point getStatusTime()const{ return StatusTime; }
			public:
				boost::signals2::signal<void(const std::string&, clock::time_point)> signal_setPicture;
				boost::signals2::signal<void(const std::string&, clock::time_point)> signal_setLogPicture;

				void slot_setTakePicture(boost::signals2::signal<void(void)>& Signal_){
					SignalConnections(hmLib::signals::connect(Signal_, [&](void)->void{this->setTakePicture(); }));
				}
				void contact_getDoTakePictureFlag(hmLib::inquiries::inquiry<bool>& Inquiry_){
					InquiryConnections(hmLib::inquiries::connect(Inquiry_, DoSetTakePicture));
				}
				void slot_setTakePictureLarge(boost::signals2::signal<void(void)>& Signal_){
					SignalConnections(hmLib::signals::connect(Signal_, [&](void)->void{this->setTakePictureLarge(); }));
				}

				void slot_setCommandReset(boost::signals2::signal<void(void)>& Signal_){
					SignalConnections(hmLib::signals::connect(Signal_, [&](void)->void{this->setCommandReset(); }));
				}
				void contact_getDoCommandResetFlag(hmLib::inquiries::inquiry<bool>& Inquiry_){
					InquiryConnections(hmLib::inquiries::connect(Inquiry_, DoCommandReset));
				}

				void slot_setPowerReset(boost::signals2::signal<void(void)>& Signal_){
					SignalConnections(hmLib::signals::connect(Signal_, [&](void)->void{this->setPowerReset(); }));
				}
				void contact_getDoPowerResetFlag(hmLib::inquiries::inquiry<bool>& Inquiry_){
					InquiryConnections(hmLib::inquiries::connect(Inquiry_, DoPowerReset));
				}

				void slot_setPictureSize(boost::signals2::signal<void(unsigned char)>& Signal_){
					SignalConnections(hmLib::signals::connect(Signal_, [&](unsigned char c)->void{this->setPictureSize(c); }));
				}
				void contact_getPictureSize(hmLib::inquiries::inquiry<unsigned char>& Inquiry_){
					InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->unsigned char{return this->getPictureSize(); }));
				}
				void slot_setPictureSizeLarge(boost::signals2::signal<void(void)>& Signal_){
					SignalConnections(hmLib::signals::connect(Signal_, [&](void)->void{this->setPictureSize(2); }));
				}

				void contact_getIsData(hmLib::inquiries::inquiry<bool>& Inquiry_){
					InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->bool{return this->getIsData(); }));
				}
				void contact_getDataPosSize(hmLib::inquiries::inquiry<std::pair<unsigned int, unsigned int>>& Inquiry_){
					InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->std::pair<unsigned int, unsigned int>{return this->getDataPosSize(); }));
				}
				void contact_getDataTime(hmLib::inquiries::inquiry<clock::time_point>& Inquiry_){
					InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->clock::time_point{return this->getDataTime(); }));
				}

				void contact_getStatus(hmLib::inquiries::inquiry<unsigned char>& Inquiry_){
					InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->unsigned char{return this->getStatus(); }));
				}
				void contact_getStatusTime(hmLib::inquiries::inquiry<clock::time_point>& Inquiry_){
					InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->clock::time_point{return this->getStatusTime(); }));
				}
				void contact_getIsErr(hmLib::inquiries::inquiry<bool>& Inquiry_){
					InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->bool{return this->getIsErr(); }));
				}
				void contact_getErrCode(hmLib::inquiries::inquiry<unsigned char>& Inquiry_){
					InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->unsigned char{return this->getErrCode(); }));
				}
			};
		}
	}
}

#
#endif
