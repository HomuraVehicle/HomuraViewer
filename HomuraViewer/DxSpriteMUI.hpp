#ifndef HMR_DXSPRITEMUI_INC
#define HMR_DXSPRITEMUI_INC 100
#
#include"hmLibVer.hpp"
#include<boost/format.hpp>
#include<boost/signals2.hpp>
//#include<hmLib/dxImage.hpp>
#include<hmLib_v2/dxObject.hpp>
#include<hmLib_v2/dxArea.hpp>
#include<hmLib/signals.hpp>
#include<hmLib/inquiries.hpp>
#include"hmrData.hpp"
#include"hmrDxTools.hpp"
#include"hmrDxBUI.hpp"
namespace hmr{
	class dxosSpriteMUI:public dxosBUI{
	public:
		dxosBUIWaitableBut TakePictureMUIBut;
		dxosBUIWaitableBut CommandResetMUIBut;
		dxosBUIWaitableBut PowerResetMUIBut;
		dxosBUIWaitableBoolBut InfoModeMUIHead;
		dxosBUIWaitableBoolBut AutoTakePictureMUIBut;	
		dxosBUIWaitableBoolBut AutoPowerResetMUIBut;
		dxosBUIWaitableBoolBut AutoLightMUIBut;
		dxosBUIWaitableBoolBut LightMUIBut;
		dxosBUIWaitableBoolBut MiniPacketModeMUIBut;

		boost::signals2::signal<void(unsigned char)> signal_setPictureSize;
		hmLib::inquiries::inquiry<unsigned char> inquiry_getPictureSize;

		hmLib::inquiries::inquiry<bool> inquiry_getIsData;
		hmLib::inquiries::inquiry<std::pair<unsigned int,unsigned int>> inquiry_getDataPosSize;
		hmLib::inquiries::inquiry<clock::time_point> inquiry_getDataTime;

		hmLib::inquiries::inquiry<unsigned char> inquiry_getStatus;
		hmLib::inquiries::inquiry<clock::time_point> inquiry_getStatusTime;
		hmLib::inquiries::inquiry<bool> inquiry_getIsErr;
		hmLib::inquiries::inquiry<unsigned char> inquiry_getErrCode;
	public:
		dxosSpriteMUI()
			:dxosBUI("Sprite",55,130)
			,TakePictureMUIBut(this)
			,CommandResetMUIBut(this)
			,PowerResetMUIBut(this)
			,InfoModeMUIHead(this)
			,AutoTakePictureMUIBut(this)
			,AutoPowerResetMUIBut(this)
			,AutoLightMUIBut(this)
			,LightMUIBut(this)
			,MiniPacketModeMUIBut(this){
		}
	public:
		int normal_draw(dxO& dxo)override{
			try{
				try{
					InfoModeMUIHead.set(Pint(70,20),"Info");
					dxo.draw(Pint(5,5),InfoModeMUIHead);
				}catch(const hmLib::inquiries::unconnected_exception&){
					dxo.draw(Pint(5,5),dxoStrP(Pint(70,20),"Info",getClr(error,strobj)));
				}

				try{
					unsigned char Status=inquiry_getStatus();
					clock::time_point StatusTime=inquiry_getStatusTime();
					std::string StatusStr;
					status Sta;
					bool isPowerReset=Status&0x80;
					unsigned char SpriteCommandID=Status&0x1F;
					bool IsErr=inquiry_getIsErr();
					
					if(IsErr){
						StatusStr="エラー";

						try{
							if(inquiry_getIsErr()){
								StatusStr+=":"+(boost::format("%02x")%static_cast<unsigned int>(inquiry_getErrCode())).str();
							}else{
								StatusStr+=":------";
							}
						}catch(const hmLib::inquiries::unconnected_exception&){
							StatusStr+=":NoCnct";
						}
						Sta=error;
						dxo.draw(Pint(80,5),dxoButIO(Pint(145,20),StatusStr,getClr(Sta,butobj),true));
					}else{
						if(isPowerReset){
							StatusStr="再起動";
							Sta=error;
							dxo.draw(Pint(80,5),dxoButIO(Pint(145,20),StatusStr,getClr(Sta,butobj),true));
						}else{
							if(SpriteCommandID==0x00){
								StatusStr="待機";
								Sta=normal;
								dxo.draw(Pint(80,5),dxoButIO(Pint(145,20),StatusStr,getClr(Sta,butobj),true));
							}else if(SpriteCommandID==0x01){
								StatusStr="リセット";
								Sta=active;
								dxo.draw(Pint(80,5),dxoButIO(Pint(145,20),StatusStr,getClr(Sta,butobj),true));							
							}else if(SpriteCommandID==0x02){
								StatusStr="撮影";
								Sta=active;
								dxo.draw(Pint(80,5),dxoButIO(Pint(145,20),StatusStr,getClr(Sta,butobj),true));							
							}else if(SpriteCommandID==0x03){
								StatusStr="Data取得";
								Sta=active;
								dxo.draw(Pint(80,5),dxoButIO(Pint(145,20),StatusStr,getClr(Sta,butobj),true));							
							}else if(SpriteCommandID==0x04){
								StatusStr="撮影終了";
								Sta=active;
								dxo.draw(Pint(80,5),dxoButIO(Pint(145,20),StatusStr,getClr(Sta,butobj),true));							
							}else if(SpriteCommandID==0x05){
								StatusStr="SavePower";
								Sta=active;
								dxo.draw(Pint(80,5),dxoButIO(Pint(145,20),StatusStr,getClr(Sta,butobj),true));							
							}else if(SpriteCommandID==0x06){
								StatusStr="StopSavePower";
								Sta=active;
								dxo.draw(Pint(80,5),dxoButIO(Pint(145,20),StatusStr,getClr(Sta,butobj),true));							
							}else if(SpriteCommandID==0x07){
								auto Range=inquiry_getDataPosSize();

								dxo.draw(Pint(80,5),dxoProgress(Pint(145,20),Range.first,Range.second,getClr(normal,butobj)));
								dxo.draw(Pint(80,5),dxoStrP(Pint(145,20),(boost::format("%d/%d")%Range.first%Range.second).str(),CLR::White));
							}else if(SpriteCommandID==0x08){
								StatusStr="圧縮率設定";
								Sta=active;
								dxo.draw(Pint(80,5),dxoButIO(Pint(145,20),StatusStr,getClr(Sta,butobj),true));							
							}else if(SpriteCommandID==0x09){
								StatusStr="Size変更";
								Sta=active;
								dxo.draw(Pint(80,5),dxoButIO(Pint(145,20),StatusStr,getClr(Sta,butobj),true));							
							}else if(SpriteCommandID==0x0A){
								StatusStr="Baudrate設定";
								Sta=active;
								dxo.draw(Pint(80,5),dxoButIO(Pint(145,20),StatusStr,getClr(Sta,butobj),true));							
							}
						}
					}

/*					if(Status==0x20||Status==0x30){
						auto Range=inquiry_getDataPosSize();

						dxo.draw(Pint(80,5),dxoProgress(Pint(145,20),Range.first,Range.second,getClr(normal,butobj)));
						dxo.draw(Pint(80,5),dxoStrP(Pint(145,20),(boost::format("%d/%d")%Range.first%Range.second).str(),CLR::White));
					}else{

						if(Status==0x00){
							StatusStr="待機";
							Sta=normal;
						}else if(Status==0x10){
							StatusStr="Size変更";
							Sta=active;
						}else if(Status==0x11){
							StatusStr="Size確定";
							Sta=active;
						}else if(Status==0x12){
							StatusStr="撮影";
							Sta=active;
						}else if(Status==0x13){
							StatusStr="Data取得";
							Sta=active;
						}else if(Status==0x40){
							StatusStr="リセット";
							Sta=active;
						}else if(Status==0xF0){
							StatusStr="再起動";
							Sta=error;
						}else if(Status==0xFF){
							StatusStr="エラー";

							try{
								if(inquiry_getIsErr()){
									StatusStr+=":"+(boost::format("%02x")%static_cast<unsigned int>(inquiry_getErrCode())).str();
								}else{
									StatusStr+=":------";
								}
							}catch(const hmLib::inquiries::unconnected_exception&){
								StatusStr+=":NoCnct";
							}
							Sta=error;
						}
						dxo.draw(Pint(80,5),dxoButIO(Pint(145,20),StatusStr,getClr(Sta,butobj),true));
					}*/
				}catch(const hmLib::inquiries::unconnected_exception&){
					dxo.draw(Pint(80,5),dxoStrP(Pint(145,20),"NoCnct",getClr(error,strobj)));
				}

				try{
					AutoTakePictureMUIBut.set(Pint(70,20),"AutoT");
					dxo.draw(Pint(5,30),AutoTakePictureMUIBut);
				}catch(const hmLib::inquiries::unconnected_exception&){
					dxo.draw(Pint(5,30),dxoStrP(Pint(70,20),"NoCnct",getClr(error,strobj)));
				}

				try{
					TakePictureMUIBut.set(Pint(70,20),"Take");
					dxo.draw(Pint(80,30),TakePictureMUIBut);
				}catch(const hmLib::inquiries::unconnected_exception&){
					dxo.draw(Pint(80,30),dxoStrP(Pint(70,20),"NoCnct",getClr(error,strobj)));
				}

				try{
					unsigned char PicSize=inquiry_getPictureSize();
					if(dxo.draw(Pint(155,30),dxoButIO(Pint(24,20),"S",getClr(normal,butobj),PicSize==0))==1){
						signal_setPictureSize(0);
					}
					if(dxo.draw(Pint(155+24,30),dxoButIO(Pint(23,20),"M",getClr(normal,butobj),PicSize==1))==1){
						signal_setPictureSize(1);
					}
					if(dxo.draw(Pint(155+47,30),dxoButIO(Pint(23,20),"L",getClr(normal,butobj),PicSize==2))==1){
						signal_setPictureSize(2);
					}
				}catch(const hmLib::inquiries::unconnected_exception&){
					dxo.draw(Pint(155,30),dxoStrP(Pint(70,20),"NoCnct",getClr(error,strobj)));
				}


			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(0,0),dxoButIO(getSize(),std::string("=ERR=")+Excp.what(),getClr(error,butobj),true,CLR::White,ALI::left));
			}

			return 0;
		}
		int extend_draw(dxO& dxo)override{
			normal_draw(dxo);

			try{
				AutoLightMUIBut.set(Pint(70,20),"AutoL");
				dxo.draw(Pint(5,55),AutoLightMUIBut);
			}catch(const hmLib::inquiries::unconnected_exception&){
				dxo.draw(Pint(5,55),dxoStrP(Pint(70,20),"NoCnct",getClr(error,strobj)));
			}

			try{
				LightMUIBut.set(Pint(70,20),"Light");
				dxo.draw(Pint(80,55),LightMUIBut);
			}catch(const hmLib::inquiries::unconnected_exception&){
				dxo.draw(Pint(80,55),dxoStrP(Pint(70,20),"NoCnct",getClr(error,strobj)));
			}

			try{
				AutoPowerResetMUIBut.set(Pint(70,20),"AutoR");
				dxo.draw(Pint(5,80),AutoPowerResetMUIBut);
			}catch(const hmLib::inquiries::unconnected_exception&){
				dxo.draw(Pint(5,80),dxoStrP(Pint(70,20),"NoCnct",getClr(error,strobj)));
			}

			try{
				CommandResetMUIBut.set(Pint(70,20),"CmReset");
				dxo.draw(Pint(80,80),CommandResetMUIBut);
			}catch(const hmLib::inquiries::unconnected_exception&){
				dxo.draw(Pint(80,80),dxoStrP(Pint(70,20),"NoCnct",getClr(error,strobj)));
			}

			try{
				PowerResetMUIBut.set(Pint(70,20),"PwReset");
				dxo.draw(Pint(155,80),PowerResetMUIBut);
			}catch(const hmLib::inquiries::unconnected_exception&){
				dxo.draw(Pint(155,80),dxoStrP(Pint(70,20),"NoCnct",getClr(error,strobj)));
			}

			try{
				MiniPacketModeMUIBut.set(Pint(70,20),"MinPac");
				dxo.draw(Pint(5,105),MiniPacketModeMUIBut);
			}catch(const hmLib::inquiries::unconnected_exception&){
				dxo.draw(Pint(5,105),dxoStrP(Pint(70,20),"NoCnct",getClr(error,strobj)));
			}

			try{
				if(inquiry_getIsErr()){
					dxo.draw(Pint(80,105),dxoButIO(Pint(70,20),(boost::format("%02x")%inquiry_getErrCode()).str(),getClr(error,butobj),true));
				}else{
					dxo.draw(Pint(80,105),dxoButIO(Pint(70,20),"No Err",getClr(normal,butobj),true));
				}
			}catch(const hmLib::inquiries::unconnected_exception&){
				dxo.draw(Pint(80,105),dxoStrP(Pint(70,20),"NoCnct",getClr(error,strobj)));
			}

			return 0;
		}
		status getStatus()const override{
			if(InfoModeMUIHead.Pic.is_connect() && (!InfoModeMUIHead.Pic()))return invalid;
			else if(inquiry_getIsErr.is_connect() && inquiry_getIsErr())return error;
			else return normal;
		}
	};
}
#
#endif
