#ifndef HMR_VIEWER_CAMERA_DXCAMERAMUI_INC
#define HMR_VIEWER_CAMERA_DXCAMERAMUI_INC 100
#
#include"hmLibVer.hpp"
#include<boost/format.hpp>
#include<boost/signals2.hpp>
//#include<hmLib/dxImage.hpp>
#include<hmLib_v2/dxObject.hpp>
#include<hmLib_v2/dxArea.hpp>
#include<hmLib/signals.hpp>
#include<hmLib/inquiries.hpp>
#include<HomuraViewer/chrono.hpp>
#include<HomuraViewer/View/DxBUI.hpp>
namespace hmr{
	namespace viewer{
		namespace camera{
			class dxosMUI :public dxosBUI{
			public:
				dxosBUIWaitableBut TakePictureMUIBut;
				dxosBUIWaitableBut CommandResetMUIBut;
				dxosBUIWaitableBut PowerResetMUIBut;
				dxosBUIWaitableBoolBut InfoModeMUIBut;
				dxosBUIWaitableBoolBut AutoTakePictureMUIBut;
				dxosBUIWaitableBoolBut AutoPowerResetMUIBut;
				dxosBUIWaitableBoolBut AutoLightMUIBut;
				dxosBUIWaitableBoolBut LightMUIBut;
				dxosBUIWaitableBoolBut MiniPacketModeMUIBut;

				boost::signals2::signal<void(unsigned char)> signal_setPictureSize;
				hmLib::inquiries::inquiry<unsigned char> inquiry_getPictureSize;

				hmLib::inquiries::inquiry<bool> inquiry_getIsData;
				hmLib::inquiries::inquiry<std::pair<unsigned int, unsigned int>> inquiry_getDataPosSize;
				hmLib::inquiries::inquiry<clock::time_point> inquiry_getDataTime;

				hmLib::inquiries::inquiry<unsigned char> inquiry_getStatus;
				hmLib::inquiries::inquiry<clock::time_point> inquiry_getStatusTime;
				hmLib::inquiries::inquiry<bool> inquiry_getIsErr;
				hmLib::inquiries::inquiry<unsigned char> inquiry_getErrCode;
			public:
				dxosMUI()
					:dxosBUI("Sprite", 55, 130)
					, TakePictureMUIBut(this)
					, CommandResetMUIBut(this)
					, PowerResetMUIBut(this)
					, InfoModeMUIBut(this)
					, AutoTakePictureMUIBut(this)
					, AutoPowerResetMUIBut(this)
					, AutoLightMUIBut(this)
					, LightMUIBut(this)
					, MiniPacketModeMUIBut(this){
				}
			public:
				int normal_draw(dxO& dxo)override{
					try{
						try{
							InfoModeMUIBut.set(Pint(70, 20), "Info");
							dxo.draw(Pint(5, 5), InfoModeMUIBut);
						}
						catch(const hmLib::inquiries::unconnected_exception&){
							dxo.draw(Pint(5, 5), dxoStrP(Pint(70, 20), "Info", getClr(error, strobj)));
						}

						try{
							unsigned char Status = inquiry_getStatus();
							clock::time_point StatusTime = inquiry_getStatusTime();
							std::string StatusStr;
							status Sta;

							if(Status == 0x20 || Status == 0x30){
								auto Range = inquiry_getDataPosSize();

								dxo.draw(Pint(80, 5), dxoProgress(Pint(145, 20), Range.first, Range.second, getClr(normal, butobj)));
								dxo.draw(Pint(80, 5), dxoStrP(Pint(145, 20), (boost::format("%d/%d") % Range.first%Range.second).str(), CLR::White));
							} else{
								//from Sprite.hpp
								//Status = return (0x80 * isPowerReset()) | (0x60 & (mode() << 5)) | (0x1f & id());
								bool IsPowerReset = ((Status & 0x80) != 0);
								unsigned char Mode = ((Status & 0x60) >> 5);
								/* Mode from homuraLib/machine/module/Sprite/command_uart.hpp
									コマンド送信状況　待機:0 送信:1 受信:2
								*/
								unsigned char ID = (Status & 0x1f);
								/* ID from homuraLib/machine/module/Sprite/command.hpp
									ResetCamera = 0x01,
									TakePicture = 0x02,
									GetDataSize = 0x03,
									StopTakePicture = 0x04,
									SavePower = 0x05,
									StopSavePower = 0x06,
									GetData = 0x07,
									SetCompressRate = 0x08,
									SetImageSize = 0x09,
									SetBaudrate = 0x0A
								*/
								StatusStr = (boost::format("%02x:%c") % static_cast<unsigned int>(Status)%(Mode==1?'S':Mode==2?'R':Mode==0?' ':'?')).str();

								if (IsPowerReset) {
									StatusStr += "再起動";
									Sta = error;
								} else if (Mode == 0x00) {
									StatusStr += "待機";
									Sta = normal;
								} else if(ID == 0x01){
									StatusStr += "リセット";
									Sta = active;
								} else if (ID == 0x02) {
									StatusStr += "撮影中";
									Sta = active;
								} else if(ID == 0x03){
									StatusStr += "写真情報";
									Sta = active;
								} else if(ID == 0x04){
									StatusStr += "撮影終了";
									Sta = active;
								} else if(ID == 0x05){
									StatusStr += "省電開始";
									Sta = active;
								} else if (ID == 0x06) {
									StatusStr += "省電終了";
									Sta = active;
								} else if (ID == 0x07) {
									StatusStr += "写真読込";
									Sta = active;
								} else if (ID == 0x08) {
									StatusStr += "圧縮率";
									Sta = active;
								} else if (ID == 0x09) {
									StatusStr += "画素数";
									Sta = active;
								} else if (ID == 0x0A) {
									StatusStr += "Boudrate";
									Sta = active;
								} else if(ID == 0xFF){
									StatusStr += "エラー";

									try{
										if(inquiry_getIsErr()){
											StatusStr += ":" + (boost::format("%02x") % static_cast<unsigned int>(inquiry_getErrCode())).str();
										} else{
											StatusStr += ":------";
										}
									}
									catch(const hmLib::inquiries::unconnected_exception&){
										StatusStr += ":NoCnct";
									}
									Sta = error;
								} else {
									StatusStr += (boost::format("?ID%02x") % static_cast<unsigned int>(ID)).str();
									Sta = error;
								}
								dxo.draw(Pint(80, 5), dxoButIO(Pint(145, 20), StatusStr, getClr(Sta, butobj), true));
							}
						}
						catch(const hmLib::inquiries::unconnected_exception&){
							dxo.draw(Pint(80, 5), dxoStrP(Pint(145, 20), "NoCnct", getClr(error, strobj)));
						}

						try{
							AutoTakePictureMUIBut.set(Pint(70, 20), "AutoT");
							dxo.draw(Pint(5, 30), AutoTakePictureMUIBut);
						}
						catch(const hmLib::inquiries::unconnected_exception&){
							dxo.draw(Pint(5, 30), dxoStrP(Pint(70, 20), "NoCnct", getClr(error, strobj)));
						}

						try{
							TakePictureMUIBut.set(Pint(70, 20), "Take");
							dxo.draw(Pint(80, 30), TakePictureMUIBut);
						}
						catch(const hmLib::inquiries::unconnected_exception&){
							dxo.draw(Pint(80, 30), dxoStrP(Pint(70, 20), "NoCnct", getClr(error, strobj)));
						}

						try{
							unsigned char PicSize = inquiry_getPictureSize();
							if(dxo.draw(Pint(155, 30), dxoButIO(Pint(24, 20), "S", getClr(normal, butobj), PicSize == 0)) == 1){
								signal_setPictureSize(0);
							}
							if(dxo.draw(Pint(155 + 24, 30), dxoButIO(Pint(23, 20), "M", getClr(normal, butobj), PicSize == 1)) == 1){
								signal_setPictureSize(1);
							}
							if(dxo.draw(Pint(155 + 47, 30), dxoButIO(Pint(23, 20), "L", getClr(normal, butobj), PicSize == 2)) == 1){
								signal_setPictureSize(2);
							}
						}
						catch(const hmLib::inquiries::unconnected_exception&){
							dxo.draw(Pint(155, 30), dxoStrP(Pint(70, 20), "NoCnct", getClr(error, strobj)));
						}


					}
					catch(const hmLib::exceptions::exception& Excp){
						if (dxo.draw(Pint(0, 0), dxoButIO(getSize(), std::string("=ERR=") + Excp.what(), getClr(error, butobj), true, CLR::White, ALI::left))==1) {
							printfDx((std::string("=ERR=") + Excp.what() + "\n").c_str());
						}
					}

					return 0;
				}
				int extend_draw(dxO& dxo)override{
					normal_draw(dxo);

					try{
						AutoLightMUIBut.set(Pint(70, 20), "AutoL");
						dxo.draw(Pint(5, 55), AutoLightMUIBut);
					}
					catch(const hmLib::inquiries::unconnected_exception&){
						dxo.draw(Pint(5, 55), dxoStrP(Pint(70, 20), "NoCnct", getClr(error, strobj)));
					}

					try{
						LightMUIBut.set(Pint(70, 20), "Light");
						dxo.draw(Pint(80, 55), LightMUIBut);
					}
					catch(const hmLib::inquiries::unconnected_exception&){
						dxo.draw(Pint(80, 55), dxoStrP(Pint(70, 20), "NoCnct", getClr(error, strobj)));
					}

					try{
						AutoPowerResetMUIBut.set(Pint(70, 20), "AutoR");
						dxo.draw(Pint(5, 80), AutoPowerResetMUIBut);
					}
					catch(const hmLib::inquiries::unconnected_exception&){
						dxo.draw(Pint(5, 80), dxoStrP(Pint(70, 20), "NoCnct", getClr(error, strobj)));
					}

					try{
						CommandResetMUIBut.set(Pint(70, 20), "CmReset");
						dxo.draw(Pint(80, 80), CommandResetMUIBut);
					}
					catch(const hmLib::inquiries::unconnected_exception&){
						dxo.draw(Pint(80, 80), dxoStrP(Pint(70, 20), "NoCnct", getClr(error, strobj)));
					}

					try{
						PowerResetMUIBut.set(Pint(70, 20), "PwReset");
						dxo.draw(Pint(155, 80), PowerResetMUIBut);
					}
					catch(const hmLib::inquiries::unconnected_exception&){
						dxo.draw(Pint(155, 80), dxoStrP(Pint(70, 20), "NoCnct", getClr(error, strobj)));
					}

					try{
						MiniPacketModeMUIBut.set(Pint(70, 20), "MinPac");
						dxo.draw(Pint(5, 105), MiniPacketModeMUIBut);
					}
					catch(const hmLib::inquiries::unconnected_exception&){
						dxo.draw(Pint(5, 105), dxoStrP(Pint(70, 20), "NoCnct", getClr(error, strobj)));
					}

					try{
						if(inquiry_getIsErr()){
							dxo.draw(Pint(80, 105), dxoButIO(Pint(70, 20), (boost::format("%02x") % inquiry_getErrCode()).str(), getClr(error, butobj), true));
						} else{
							dxo.draw(Pint(80, 105), dxoButIO(Pint(70, 20), "No Err", getClr(normal, butobj), true));
						}
					}
					catch(const hmLib::inquiries::unconnected_exception&){
						dxo.draw(Pint(80, 105), dxoStrP(Pint(70, 20), "NoCnct", getClr(error, strobj)));
					}


					return 0;
				}
				status getStatus()const override{
					if(InfoModeMUIBut.Pic.is_connect() && (!InfoModeMUIBut.Pic()))return invalid;
					else if(inquiry_getIsErr.is_connect() && inquiry_getIsErr())return error;
					else return normal;
				}
			};
		}
	}
}
#
#endif
