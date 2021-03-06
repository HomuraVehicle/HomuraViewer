#ifndef HMR_DXDEVICEMANAGESUI_INC
#define HMR_DXDEVICEMANAGESUI_INC 101
#
#include <iostream>
#include <string>
#include "hmLibVer.hpp"
#include <hmLib_v2/hmBasic.hpp>
#include <hmLib_v2/dxObject.hpp>
#include <hmLib_v2/dxArea.hpp>
#include <hmLib/signals.hpp>
#include <boost/signals2.hpp>
#include <HomuraViewer/View/DxBUI.hpp>
#include "DeviceManageMsgAgent.hpp"
//#include "hmrBaseDX.hpp"

//#define DEBUG_MODE 1

/* ---- hmrDxDeviceManageSUI-----

===hmrDxDeviceManageSUI===
v1_02/131102 hmIto
	表示フォーマットを整理
v1_01/131019 amby
	DxDeviceManageSUIのClockmodeに対応
	実装	
v1_00/130928 amby
	DxDeviceManageSUIの製作開始
	実装
*/
#define HMR_DEVMNGSUI_TIMEBUF_SIZE 15

namespace hmr{
	namespace viewer{
		namespace devmng{
			// 描画クラス static
			class dxosSUI :public dxosBUI{
				static constexpr unsigned int TimeBuf[HMR_DEVMNGSUI_TIMEBUF_SIZE] = {30,60,120,300,600,900,1200,1800,2700,3600,7200,10800,14400,18000,28800};
			private:
				int RemLength;
				int SleepLength;
				int roamLength;
				cMsgAgent::ClockMODE clockMode;

			public:
				// signal 系列の定義
				boost::signals2::signal<void(bool, int, int)> signal_setSleepMode;
				boost::signals2::signal<void(bool)> signal_exeSleepMode;

				boost::signals2::signal<void(bool, int)> signal_setRoamMode;
				boost::signals2::signal<void(bool)> signal_exeRoamMode;

				//boost::signals2::signal<void(bool)> signal_setTurboMode;
				boost::signals2::signal<void(bool)> signal_setNormalMode;

				boost::signals2::signal<void(bool)> signal_setKillMode;
				boost::signals2::signal<void(bool)> signal_exeKillMode;

				boost::signals2::signal<void(bool)> signal_setGettingInfo;
				boost::signals2::signal<void(bool, cMsgAgent::ClockMODE)> signal_setClockMode;


				// inquire 系列の定義
				hmLib::inquiries::inquiry<bool> SendingSleepCommandInq;
				hmLib::inquiries::inquiry<bool> IsReadySleepInq;

				hmLib::inquiries::inquiry<bool> SendingRoamCommandInq;
				hmLib::inquiries::inquiry<bool> IsReadyRoamInq;
				hmLib::inquiries::inquiry<bool> UseMPInq;

				//hmLib::inquiries::inquiry<bool> SendingTurboCommandInq;
				hmLib::inquiries::inquiry<bool> SendingNormalCommandInq;

				hmLib::inquiries::inquiry<bool> SendingKillCommandInq;
				hmLib::inquiries::inquiry<bool> IsReadyKillInq;

				hmLib::inquiries::inquiry<bool> SendingInfoCommandInq;
				hmLib::inquiries::inquiry<bool> SendingClockCommandInq;

				hmLib::inquiries::inquiry<cMsgAgent::DevMODE> devMODEInq;
				hmLib::inquiries::inquiry<cMsgAgent::ClockMODE> clockMODEInq;


			public://virtual functions @dxosBUI;
				int normal_draw(dxO& dxo)override{
					// Info の描画
					try{
						bool isSending = SendingInfoCommandInq();
						dxRGB intClr;
						if(isSending){
							intClr = getClr(active, butobj);
						} else{ intClr = getClr(normal, butobj); }
						if(dxo.draw(Pint(5, 5), dxoButIO(Pint(60, 20), "INFO", intClr, isSending)) == 1){
							signal_setGettingInfo(!isSending);
						}
					}
					catch(const hmLib::inquiries::unconnected_exception&){
						dxo.draw(Pint(5, 5), dxoStr("Inquiry::sendingInfoCom 取得失敗"));
					}
					catch(const hmLib::exceptions::exception& Excp){
						dxo.draw(Pint(5, 5), dxoStr(Excp.what()));
					}

					// 状態表示
					try{
						cMsgAgent::DevMODE deviceMode = devMODEInq();
						if(deviceMode == cMsgAgent::DevMODE::NORMAL){
							dxo.draw(Pint(70, 5), dxoStrP(Pint(75, 20), "NORMAL", getClr(normal, strobj)));
						} else if(deviceMode == cMsgAgent::DevMODE::SLEEP){
							dxo.draw(Pint(70, 5), dxoStrP(Pint(75, 20), "SLEEP", getClr(normal, strobj)));
						} else if(deviceMode == cMsgAgent::DevMODE::ROAM){
							dxo.draw(Pint(70, 5), dxoStrP(Pint(75, 20), "ROAM", getClr(normal, strobj)));
						} else if(deviceMode == cMsgAgent::DevMODE::UNKNOWN){
							dxo.draw(Pint(70, 5), dxoStrP(Pint(75, 20), "UNKNOWN", getClr(normal, strobj)));
						}
					}
					catch(const hmLib::inquiries::unconnected_exception&){
						dxo.draw(Pint(70, 5), dxoStr("Inquiry::deviceMode 取得失敗"));
					}
					catch(const hmLib::exceptions::exception& Excp){
						dxo.draw(Pint(70, 5), dxoStr(Excp.what()));
					}

					try{
						cMsgAgent::ClockMODE clockMode = clockMODEInq();
						if(clockMode == cMsgAgent::ClockMODE::NORMALCLOCK){
							dxo.draw(Pint(150, 5), dxoStrP(Pint(75, 20), "NORMAL", getClr(normal, strobj)));
						} else if(clockMode == cMsgAgent::ClockMODE::LOWCLOCK){
							dxo.draw(Pint(150, 5), dxoStrP(Pint(75, 20), "SLOW", getClr(normal, strobj)));
						} else if(clockMode == cMsgAgent::ClockMODE::HIGHCLOCK){
							dxo.draw(Pint(150, 5), dxoStrP(Pint(75, 20), "TURBO", getClr(normal, strobj)));
						} else if(clockMode == cMsgAgent::ClockMODE::UNKNOWNCLOCK){
							dxo.draw(Pint(150, 5), dxoStrP(Pint(75, 20), "UNKNOWN", getClr(normal, strobj)));
						}
					}
					catch(const hmLib::inquiries::unconnected_exception&){
						dxo.draw(Pint(150, 5), dxoStr("Inquiry::clockMode 取得失敗"));
					}
					catch(const hmLib::exceptions::exception& Excp){
						dxo.draw(Pint(150, 5), dxoStr(Excp.what()));
					}


					return 0;
				}
				int extend_draw(dxO& dxo)override{
					// draw
					normal_draw(dxo);

					// Normal flag の描画
					try{
						bool isSending = SendingNormalCommandInq();
						dxRGB intClr;
						if(isSending){
							intClr = getClr(active, butobj);
						} else{ intClr = getClr(normal, butobj); }
						if(dxo.draw(Pint(5, 30), dxoButIO(Pint(105, 20), "NORMAL", intClr, isSending)) == 1){
							signal_setNormalMode(!isSending);
						}
					}
					catch(const hmLib::inquiries::unconnected_exception&){
						dxo.draw(Pint(5, 30), dxoStr("Inquiry::sendingNormalCom 取得失敗"));
					}
					catch(const hmLib::exceptions::exception& Excp){
						dxo.draw(Pint(5, 30), dxoStr(Excp.what()));
					}

					// KILL の描画
					try{
						bool isSending = SendingKillCommandInq();
						bool isReadytoKill = IsReadyKillInq();

						if(!isReadytoKill){
							dxRGB intClr;
							if(isSending){
								intClr = getClr(active, butobj);
							} else{ intClr = getClr(normal, butobj); }
							if(dxo.draw(Pint(125, 30), dxoButIO(Pint(105, 20), "KILL", intClr, isSending)) == 1){
								signal_setKillMode(!isSending);
							}
						} else{
							int ans = dxo.draw(Pint(125, 30), dxoButLRIO(Pint(105, 20), "Sure?? Y/N", getClr(normal, butobj), true));
							if(ans == 1){
								signal_exeKillMode(true);
							} else if(ans == 10){
								signal_exeKillMode(false);
							}
						}
					}
					catch(const hmLib::inquiries::unconnected_exception&){
						dxo.draw(Pint(125, 30), dxoStr("Inquiry::sendingKILLCom 取得失敗"));
					}
					catch(const hmLib::exceptions::exception& Excp){
						dxo.draw(Pint(125, 30), dxoStr(Excp.what()));
					}


					// ROAM の描画
					try{
						bool isSending = SendingRoamCommandInq();
						bool isReadytoRoam = IsReadyRoamInq();

						if(!isReadytoRoam){
							dxRGB intClr;
							if(isSending){
								intClr = getClr(active, butobj);
							} else{ intClr = getClr(normal, butobj); }

							if(dxo.draw(Pint(5, 55), dxoButIO(Pint(70, 20), "ROAM", intClr, isSending)) == 1){
								signal_setRoamMode(!isSending, roamLength);
							}
						} else{
							int ans = dxo.draw(Pint(5, 55), dxoButIO(Pint(70, 20), "Sure?? Y/N", getClr(normal, butobj), true));
							if(ans == 1){
								signal_exeRoamMode(true);
							} else if(ans == 10){
								signal_exeRoamMode(false);
							}
						}

					}
					catch(const hmLib::inquiries::unconnected_exception&){
						dxo.draw(Pint(5, 55), dxoStr("Inquiry::Roam 取得失敗"));
					}
					catch(const hmLib::exceptions::exception& Excp){
						dxo.draw(Pint(5, 55), dxoStr(Excp.what()));
					}

					// ROAM 時間系設定
					try{
						bool isSending = SendingRoamCommandInq();
						bool isReadytoRoam = IsReadyRoamInq();
						std::string str;
						//if(useMP) str = "MP";
						//else str = "RF";

						// sleeptime 設定
						if(!isReadytoRoam && !isSending){
							int ans = dxo.draw(Pint(80, 55), dxoButLRIO(Pint(70, 20), hmstd::fStr("%d(s)", roamLength), getClr(normal, butobj), true));
							if(ans == 1){
								roamLength += 20;
								if(roamLength >= 300) roamLength = 20;
							} else if(ans == 10){
								roamLength -= 20;
								if(roamLength < 20) roamLength = 300;
							}

							/*
							ans = dxo.draw(Pint(155,55), dxoButIO(Pint(70, 20), str, getClr(normal, butobj), true));
							if(ans==1){
							useMP = !useMP;
							}
							*/
						} else{
							dxo.draw(Pint(80, 55), dxoButLRIO(Pint(70, 20), hmstd::fStr("%d(s)", roamLength), getClr(normal, butobj), false));
							//dxo.draw(Pint(155, 55), dxoButIO(Pint(70, 20), str, getClr(normal, butobj), false));
						}
					}
					catch(const hmLib::inquiries::unconnected_exception&){
						// Inquire にアクセスできなかった！！　でも、そんなのメイン関数には関係ないからもみ消す
						dxo.draw(Pint(80, 55), dxoStr("Inquiry::isReady to roam 取得失敗"));
					}
					catch(const hmLib::exceptions::exception& Excp){
						dxo.draw(Pint(80, 55), dxoStr(Excp.what()));
					}

					// ROAM モードの描画
					try{
						bool useMP = UseMPInq();
						std::string str;
						if(useMP) str = "MP";
						else str = "RF";
						dxo.draw(Pint(155, 55), dxoStrP(Pint(70, 20), str, getClr(normal, strobj)));
					}
					catch(const hmLib::inquiries::unconnected_exception&){
						// Inquire にアクセスできなかった！！　でも、そんなのメイン関数には関係ないからもみ消す
						dxo.draw(Pint(155, 55), dxoStr("Inquiry::UseMP 取得失敗"));
					}
					catch(const hmLib::exceptions::exception& Excp){
						dxo.draw(Pint(155, 55), dxoStr(Excp.what()));
					}

					// SLEEP の描画
					try{
						bool isSending = SendingSleepCommandInq();
						bool isReadytoSleep = IsReadySleepInq();

						if(!isReadytoSleep){
							dxRGB intClr;
							if(isSending){
								intClr = getClr(active, butobj);
							} else{ intClr = getClr(normal, butobj); }

							if(dxo.draw(Pint(5, 80), dxoButIO(Pint(70, 20), "SLEEP", intClr, isSending)) == 1){
								signal_setSleepMode(!isSending, TimeBuf[SleepLength] - TimeBuf[RemLength], TimeBuf[RemLength]);
							}
						} else{
							int ans = dxo.draw(Pint(5, 80), dxoButLRIO(Pint(70, 20), "Sure?? Y/N", getClr(normal, butobj), true));
							if(ans == 1){
								signal_exeSleepMode(true);
							} else if(ans == 10){
								signal_exeSleepMode(false);
							}
						}

					}
					catch(const hmLib::inquiries::unconnected_exception&){
						dxo.draw(Pint(5, 80), dxoStr("Inquiry::SLEEP 取得失敗"));
					}
					catch(const hmLib::exceptions::exception& Excp){
						dxo.draw(Pint(5, 80), dxoStr(Excp.what()));
					}

					// SLEEP 時間系設定
					try{
						bool isSending = SendingSleepCommandInq();
						bool isReadytoSleep = IsReadySleepInq();

						// sleeptime 設定
						if(!isReadytoSleep && !isSending){
							int ans;

							ans = dxo.draw(Pint(80, 80), dxoButLRIO(Pint(70, 20), hmstd::fStr("I:%.1f", TimeBuf[SleepLength] / 60.), getClr(normal, butobj), true));
							if(ans == 1){
								++SleepLength;
								SleepLength = std::min(SleepLength, HMR_DEVMNGSUI_TIMEBUF_SIZE - 1);
							} else if(ans == 10){
								--SleepLength;
								SleepLength = std::max(SleepLength, 1);
							}
							RemLength = std::min(SleepLength - 1, RemLength);

							ans = dxo.draw(Pint(155, 80), dxoButLRIO(Pint(70, 20), hmstd::fStr("R:%.1f", TimeBuf[RemLength] / 60.), getClr(normal, butobj), true));
							if(ans == 1){
								++RemLength;
								RemLength = std::min(RemLength, HMR_DEVMNGSUI_TIMEBUF_SIZE - 1);
							} else if(ans == 10){
								--RemLength;
								RemLength = std::max(RemLength, 0);
							}

						} else{
							dxo.draw(Pint(80, 80), dxoButLRIO(Pint(70, 20), hmstd::fStr("I:%.1f", TimeBuf[SleepLength] / 60.), getClr(normal, butobj), false));
							dxo.draw(Pint(155, 80), dxoButLRIO(Pint(70, 20), hmstd::fStr("R:%.1f", TimeBuf[RemLength] / 60.), getClr(normal, butobj), false));
						}
					}
					catch(const hmLib::inquiries::unconnected_exception&){
						// Inquire にアクセスできなかった！！　でも、そんなのメイン関数には関係ないからもみ消す
						dxo.draw(Pint(80, 80), dxoStr("Inquiry::isReady to sleep 取得失敗"));
					}
					catch(const hmLib::exceptions::exception& Excp){
						dxo.draw(Pint(80, 80), dxoStr(Excp.what()));
					}

					// CLOCK の描画
					try{
						bool isSending = SendingClockCommandInq();
						dxRGB intClr;
						if(isSending){
							intClr = getClr(active, butobj);
						} else{ intClr = getClr(normal, butobj); }
						if(dxo.draw(Pint(5, 105), dxoButIO(Pint(70, 20), "CLOCK", intClr, isSending)) == 1){
							signal_setClockMode(!isSending, clockMode);
						}
					}
					catch(const hmLib::inquiries::unconnected_exception&){
						dxo.draw(Pint(5, 105), dxoStr("Inquiry::sendingClockCom 取得失敗"));
					}
					catch(const hmLib::exceptions::exception& Excp){
						dxo.draw(Pint(5, 105), dxoStr(Excp.what()));
					}

					// CLOCK 設定
					try{
						bool isSending = SendingClockCommandInq();
						std::string str;
						if(clockMode == cMsgAgent::ClockMODE::NORMALCLOCK)str = "NORMAL";
						else if(clockMode == cMsgAgent::ClockMODE::HIGHCLOCK)str = "HIGH";
						else if(clockMode == cMsgAgent::ClockMODE::LOWCLOCK)str = "LOW";

						//  設定
						if(!isSending){

							int ans = dxo.draw(Pint(80, 105), dxoButLRIO(Pint(70, 20), str, getClr(normal, butobj), true));
							if(ans == 1){
								//clockMode = (cMsgAgent::ClockMODE)(clockMode+1)%cMsgAgent::ClockMODE::UNKNOWNCLOCK;
								if(clockMode == cMsgAgent::ClockMODE::NORMALCLOCK)clockMode = cMsgAgent::ClockMODE::HIGHCLOCK;
								else if(clockMode == cMsgAgent::ClockMODE::HIGHCLOCK)clockMode = cMsgAgent::ClockMODE::LOWCLOCK;
								else if(clockMode == cMsgAgent::ClockMODE::LOWCLOCK)clockMode = cMsgAgent::ClockMODE::NORMALCLOCK;
							} else if(ans == 10){
								if(clockMode == cMsgAgent::ClockMODE::NORMALCLOCK)clockMode = cMsgAgent::ClockMODE::LOWCLOCK;
								else if(clockMode == cMsgAgent::ClockMODE::HIGHCLOCK)clockMode = cMsgAgent::ClockMODE::NORMALCLOCK;
								else if(clockMode == cMsgAgent::ClockMODE::LOWCLOCK)clockMode = cMsgAgent::ClockMODE::HIGHCLOCK;
							}

							/*
							ans = dxo.draw(Pint(155,55), dxoButIO(Pint(70, 20), str, getClr(normal, butobj), true));
							if(ans==1){
							useMP = !useMP;
							}
							*/
						} else{
							dxo.draw(Pint(80, 105), dxoButLRIO(Pint(70, 20), str, getClr(normal, butobj), false));
							//dxo.draw(Pint(155, 55), dxoButIO(Pint(70, 20), str, getClr(normal, butobj), false));
						}
					}
					catch(const hmLib::inquiries::unconnected_exception&){
						// Inquire にアクセスできなかった！！　でも、そんなのメイン関数には関係ないからもみ消す
						dxo.draw(Pint(80, 105), dxoStr("Inquiry::sendingClockCom 取得失敗"));
					}
					catch(const hmLib::exceptions::exception& Excp){
						dxo.draw(Pint(80, 105), dxoStr(Excp.what()));
					}

					return 0;

				}
				status getStatus()const override{ return normal; }
			public:
				// constructor
				dxosSUI(void) :dxosBUI("HMR Device", 30, 130){
					RemLength = 0;
					SleepLength = 1;
					roamLength = 20;
					//useMP = true;
					clockMode = cMsgAgent::ClockMODE::NORMALCLOCK;
				}
			};
		}
	}
}

#
#endif
