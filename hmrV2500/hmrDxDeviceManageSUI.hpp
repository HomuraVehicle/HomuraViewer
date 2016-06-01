#ifndef HMR_DXDEVICEMANAGESUI_INC
#define HMR_DXDEVICEMANAGESUI_INC 101

#include <iostream>
#include <string>
#include "hmLibVer.hpp"
#include <hmLib_v2/hmBasic.hpp>
#include <hmLib_v2/dxObject.hpp>
#include <hmLib_v2/dxArea.hpp>
#include <hmLib_v3_06/signals.hpp>
#include <boost/signals2.hpp>
#include "hmrDxBUI.hpp"
#include "hmrDeviceManage.hpp"
//#include "hmrBaseDX.hpp"

//#define DEBUG_MODE 1

/* ---- hmrDxDeviceManageSUI-----

===hmrDxDeviceManageSUI===
v1_02/131102 hmIto
	�\���t�H�[�}�b�g�𐮗�
v1_01/131019 amby
	DxDeviceManageSUI��Clockmode�ɑΉ�
	����	
v1_00/130928 amby
	DxDeviceManageSUI�̐���J�n
	����
*/
#define HMR_DEVMNGSUI_TIMEBUF_SIZE 15

namespace hmr{
	// �`��N���X static
	class dxosDevMngSUI:public dxosBUI{
		static const unsigned int TimeBuf[HMR_DEVMNGSUI_TIMEBUF_SIZE];	//���Ԕz��
	private:
		int RemLength;
		int SleepLength;
		int roamLength;
		cDevMngMsgAgent::ClockMODE clockMode;
		//bool useMP;

	public:
		// signal �n��̒�`
		boost::signals2::signal<void(bool, int, int)> signal_setSleepMode;
		boost::signals2::signal<void(bool)> signal_exeSleepMode;

		boost::signals2::signal<void(bool, int)> signal_setRoamMode;
		boost::signals2::signal<void(bool)> signal_exeRoamMode;
		
		//boost::signals2::signal<void(bool)> signal_setTurboMode;
		boost::signals2::signal<void(bool)> signal_setNormalMode;

		boost::signals2::signal<void(bool)> signal_setKillMode;
		boost::signals2::signal<void(bool)> signal_exeKillMode;

		boost::signals2::signal<void(bool)> signal_setGettingInfo;
		boost::signals2::signal<void(bool, cDevMngMsgAgent::ClockMODE)> signal_setClockMode;
		

		// inquire �n��̒�`
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

		hmLib::inquiries::inquiry<cDevMngMsgAgent::DevMODE> devMODEInq;
		hmLib::inquiries::inquiry<cDevMngMsgAgent::ClockMODE> clockMODEInq;

		
	public://virtual functions @dxosBUI;
		int normal_draw(dxO& dxo)override{
			// Info �̕`��
			try{
				bool isSending = SendingInfoCommandInq();
				dxRGB intClr;
				if(isSending){ intClr =  getClr(active, butobj);
				}else{intClr =  getClr(normal, butobj);}
				if( dxo.draw(Pint(5, 5),dxoButIO(Pint(60,20), "INFO", intClr, isSending)) == 1){
					signal_setGettingInfo(!isSending);
				}
			}catch(const hmLib::inquiries::unconnected_exception&){				
				dxo.draw(Pint(5, 5), dxoStr("Inquiry::sendingInfoCom �擾���s") );
			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(5, 5), dxoStr( Excp.what()) );
			}

			// ��ԕ\��
			try{
				cDevMngMsgAgent::DevMODE deviceMode = devMODEInq();
				if(deviceMode == cDevMngMsgAgent::DevMODE::NORMAL){
					dxo.draw(Pint(70,5),dxoStrP(Pint(75,20),"NORMAL",getClr(normal,strobj)));
				}else if(deviceMode == cDevMngMsgAgent::DevMODE::SLEEP){
					dxo.draw(Pint(70,5),dxoStrP(Pint(75,20),"SLEEP",getClr(normal,strobj)));
				}else if(deviceMode == cDevMngMsgAgent::DevMODE::ROAM){
					dxo.draw(Pint(70,5),dxoStrP(Pint(75,20),"ROAM",getClr(normal,strobj)));
				}else if(deviceMode == cDevMngMsgAgent::DevMODE::UNKNOWN){
					dxo.draw(Pint(70,5),dxoStrP(Pint(75,20),"UNKNOWN",getClr(normal,strobj)));
				}
			}catch(const hmLib::inquiries::unconnected_exception&){				
				dxo.draw(Pint(70, 5), dxoStr("Inquiry::deviceMode �擾���s") );
			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(70, 5), dxoStr(Excp.what()) );
			}

			try{
				cDevMngMsgAgent::ClockMODE clockMode = clockMODEInq();
				if(clockMode == cDevMngMsgAgent::ClockMODE::NORMALCLOCK){
					dxo.draw(Pint(150,5),dxoStrP(Pint(75,20),"NORMAL",getClr(normal,strobj)));
				}else if(clockMode == cDevMngMsgAgent::ClockMODE::LOWCLOCK){
					dxo.draw(Pint(150,5),dxoStrP(Pint(75,20),"SLOW",getClr(normal,strobj)));
				}else if(clockMode == cDevMngMsgAgent::ClockMODE::HIGHCLOCK){
					dxo.draw(Pint(150,5),dxoStrP(Pint(75,20),"TURBO",getClr(normal,strobj)));
				}else if(clockMode == cDevMngMsgAgent::ClockMODE::UNKNOWNCLOCK){
					dxo.draw(Pint(150,5),dxoStrP(Pint(75,20),"UNKNOWN",getClr(normal,strobj)));
				}
			}catch(const hmLib::inquiries::unconnected_exception&){				
				dxo.draw(Pint(150, 5), dxoStr("Inquiry::clockMode �擾���s") );
			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(150, 5), dxoStr(Excp.what()) );
			}


			return 0;
		}
		int extend_draw(dxO& dxo)override{
			// draw
			normal_draw(dxo);

			// Normal flag �̕`��
			try{
				bool isSending = SendingNormalCommandInq();
				dxRGB intClr;
				if(isSending){ intClr =  getClr(active, butobj);
				}else{intClr =  getClr(normal, butobj);}
				if( dxo.draw(Pint(5, 30),dxoButIO(Pint(105,20), "NORMAL", intClr, isSending)) == 1){
					signal_setNormalMode(!isSending);
				}
			}catch(const hmLib::inquiries::unconnected_exception&){				
				dxo.draw(Pint(5, 30), dxoStr("Inquiry::sendingNormalCom �擾���s") );
			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(5, 30), dxoStr( Excp.what()) );
			}

			// KILL �̕`��
			try{
				bool isSending = SendingKillCommandInq();
				bool isReadytoKill = IsReadyKillInq();

				if(!isReadytoKill){
					dxRGB intClr;
					if(isSending){ intClr =  getClr(active, butobj);
					}else{intClr =  getClr(normal, butobj);}
					if( dxo.draw(Pint(125, 30),dxoButIO(Pint(105,20), "KILL", intClr, isSending)) == 1){
						signal_setKillMode(!isSending);
					}
				}else{
					int ans = dxo.draw(Pint(125, 30),dxoButLRIO(Pint(105,20), "Sure?? Y/N", getClr(normal, butobj), true));
					if( ans == 1){
						signal_exeKillMode(true);
					}else if(ans == 10){
						signal_exeKillMode(false);
					}
				}
			}catch(const hmLib::inquiries::unconnected_exception&){				
				dxo.draw(Pint(125, 30), dxoStr("Inquiry::sendingKILLCom �擾���s") );
			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(125, 30), dxoStr( Excp.what()) );
			}


			// ROAM �̕`��
			try{
				bool isSending = SendingRoamCommandInq();
				bool isReadytoRoam = IsReadyRoamInq();

				if(!isReadytoRoam){
					dxRGB intClr;
					if(isSending){ intClr =  getClr(active, butobj);
					}else{intClr =  getClr(normal, butobj);}
					
					if( dxo.draw(Pint(5, 55),dxoButIO(Pint(70,20), "ROAM", intClr, isSending)) == 1){
						signal_setRoamMode(!isSending, roamLength);
					}
				}else{
					int ans = dxo.draw(Pint(5, 55),dxoButIO(Pint(70,20), "Sure?? Y/N", getClr(normal, butobj), true));
					if( ans == 1){
						signal_exeRoamMode(true);
					}else if(ans == 10){
						signal_exeRoamMode(false);
					}
				}

			}catch(const hmLib::inquiries::unconnected_exception&){				
				dxo.draw(Pint(5, 55), dxoStr("Inquiry::Roam �擾���s") );
			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(5, 55), dxoStr( Excp.what()) );
			}

			// ROAM ���Ԍn�ݒ�
			try{
				bool isSending = SendingRoamCommandInq();
				bool isReadytoRoam = IsReadyRoamInq();
				std::string str;
				//if(useMP) str = "MP";
				//else str = "RF";

				// sleeptime �ݒ�
				if(!isReadytoRoam && !isSending){
					int ans=dxo.draw(Pint(80, 55), dxoButLRIO(Pint(70, 20), hmstd::fStr("%d(s)", roamLength), getClr(normal, butobj), true));
					if(ans==1){
						roamLength += 20;
						if(roamLength >= 300) roamLength = 20;
					}
					else if(ans==10){
						roamLength -= 20;
						if(roamLength < 20) roamLength = 300;
					}

					/*
					ans = dxo.draw(Pint(155,55), dxoButIO(Pint(70, 20), str, getClr(normal, butobj), true));
					if(ans==1){
						useMP = !useMP;
					}
					*/
				}else{
					dxo.draw(Pint(80, 55), dxoButLRIO(Pint(70, 20), hmstd::fStr("%d(s)", roamLength), getClr(normal, butobj), false));
					//dxo.draw(Pint(155, 55), dxoButIO(Pint(70, 20), str, getClr(normal, butobj), false));
				}
			}catch(const hmLib::inquiries::unconnected_exception& ){				
				// Inquire �ɃA�N�Z�X�ł��Ȃ������I�I�@�ł��A����Ȃ̃��C���֐��ɂ͊֌W�Ȃ�������ݏ���
				dxo.draw(Pint(80, 55), dxoStr("Inquiry::isReady to roam �擾���s") );
			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(80, 55), dxoStr( Excp.what()) );
			}

			// ROAM ���[�h�̕`��
			try{
				bool useMP = UseMPInq();
				std::string str;
				if(useMP) str = "MP";
				else str = "RF";
				dxo.draw(Pint(155, 55), dxoStrP(Pint(70,20),str,getClr(normal,strobj)));
			}catch(const hmLib::inquiries::unconnected_exception& ){				
				// Inquire �ɃA�N�Z�X�ł��Ȃ������I�I�@�ł��A����Ȃ̃��C���֐��ɂ͊֌W�Ȃ�������ݏ���
				dxo.draw(Pint(155, 55), dxoStr("Inquiry::UseMP �擾���s") );
			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(155, 55), dxoStr( Excp.what()) );
			}

			// SLEEP �̕`��
			try{
				bool isSending = SendingSleepCommandInq();
				bool isReadytoSleep = IsReadySleepInq();

				if(!isReadytoSleep){
					dxRGB intClr;
					if(isSending){ intClr =  getClr(active, butobj);
					}else{intClr =  getClr(normal, butobj);}
					
					if( dxo.draw(Pint(5, 80),dxoButIO(Pint(70,20), "SLEEP", intClr, isSending)) == 1){
						signal_setSleepMode(!isSending, TimeBuf[SleepLength]-TimeBuf[RemLength], TimeBuf[RemLength]);
					}
				}else{
					int ans = dxo.draw(Pint(5, 80),dxoButLRIO(Pint(70,20), "Sure?? Y/N", getClr(normal, butobj), true));
					if( ans == 1){
						signal_exeSleepMode(true);
					}else if(ans == 10){
						signal_exeSleepMode(false);
					}
				}

			}catch(const hmLib::inquiries::unconnected_exception&){				
				dxo.draw(Pint(5, 80), dxoStr("Inquiry::SLEEP �擾���s") );
			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(5, 80), dxoStr( Excp.what()) );
			}

			// SLEEP ���Ԍn�ݒ�
			try{
				bool isSending = SendingSleepCommandInq();
				bool isReadytoSleep = IsReadySleepInq();

				// sleeptime �ݒ�
				if(!isReadytoSleep && !isSending){
					int ans;

					ans = dxo.draw(Pint(80,80), dxoButLRIO(Pint(70, 20), hmstd::fStr("I:%.1f", TimeBuf[SleepLength]/60.), getClr(normal, butobj), true));
					if(ans==1){
						++SleepLength;
						SleepLength=std::min(SleepLength,HMR_DEVMNGSUI_TIMEBUF_SIZE-1);
					}
					else if(ans==10){
						--SleepLength;
						SleepLength=std::max(SleepLength,1);
					}
					RemLength=std::min(SleepLength-1,RemLength);

					ans=dxo.draw(Pint(155,80), dxoButLRIO(Pint(70, 20), hmstd::fStr("R:%.1f", TimeBuf[RemLength]/60.), getClr(normal, butobj), true));
					if(ans==1){
						++RemLength;
						RemLength=std::min(RemLength,HMR_DEVMNGSUI_TIMEBUF_SIZE-1);
					}
					else if(ans==10){
						--RemLength;
						RemLength=std::max(RemLength,0);
					}

				}else{
					dxo.draw(Pint(80, 80), dxoButLRIO(Pint(70, 20), hmstd::fStr("I:%.1f", TimeBuf[SleepLength]/60.), getClr(normal, butobj), false));
					dxo.draw(Pint(155, 80), dxoButLRIO(Pint(70, 20), hmstd::fStr("R:%.1f", TimeBuf[RemLength]/60.), getClr(normal, butobj), false));
				}
			}catch(const hmLib::inquiries::unconnected_exception& ){				
				// Inquire �ɃA�N�Z�X�ł��Ȃ������I�I�@�ł��A����Ȃ̃��C���֐��ɂ͊֌W�Ȃ�������ݏ���
				dxo.draw(Pint(80, 80), dxoStr("Inquiry::isReady to sleep �擾���s") );
			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(80, 80), dxoStr( Excp.what()) );
			}

			// CLOCK �̕`��
			try{
				bool isSending = SendingClockCommandInq();
				dxRGB intClr;
				if(isSending){ intClr =  getClr(active, butobj);
				}else{intClr =  getClr(normal, butobj);}
				if( dxo.draw(Pint(5, 105),dxoButIO(Pint(70,20), "CLOCK", intClr, isSending)) == 1){
					signal_setClockMode(!isSending, clockMode);
				}
			}catch(const hmLib::inquiries::unconnected_exception&){				
				dxo.draw(Pint(5, 105), dxoStr("Inquiry::sendingClockCom �擾���s") );
			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(5, 105), dxoStr( Excp.what()) );
			}

			// CLOCK �ݒ�
			try{
				bool isSending = SendingClockCommandInq();
				std::string str;
				if(clockMode == cDevMngMsgAgent::ClockMODE::NORMALCLOCK)str = "NORMAL";
				else if(clockMode == cDevMngMsgAgent::ClockMODE::HIGHCLOCK)str = "HIGH";
				else if(clockMode == cDevMngMsgAgent::ClockMODE::LOWCLOCK)str = "LOW";
				
				//  �ݒ�
				if(!isSending){

					int ans=dxo.draw(Pint(80, 105), dxoButLRIO(Pint(70, 20), str, getClr(normal, butobj), true));
					if(ans==1){
						//clockMode = (cDevMngMsgAgent::ClockMODE)(clockMode+1)%cDevMngMsgAgent::ClockMODE::UNKNOWNCLOCK;
						if(clockMode == cDevMngMsgAgent::ClockMODE::NORMALCLOCK)clockMode = cDevMngMsgAgent::ClockMODE::HIGHCLOCK;
						else if(clockMode == cDevMngMsgAgent::ClockMODE::HIGHCLOCK)clockMode = cDevMngMsgAgent::ClockMODE::LOWCLOCK;
						else if(clockMode == cDevMngMsgAgent::ClockMODE::LOWCLOCK)clockMode = cDevMngMsgAgent::ClockMODE::NORMALCLOCK;
					}
					else if(ans==10){
						if(clockMode == cDevMngMsgAgent::ClockMODE::NORMALCLOCK)clockMode = cDevMngMsgAgent::ClockMODE::LOWCLOCK;
						else if(clockMode == cDevMngMsgAgent::ClockMODE::HIGHCLOCK)clockMode = cDevMngMsgAgent::ClockMODE::NORMALCLOCK;
						else if(clockMode == cDevMngMsgAgent::ClockMODE::LOWCLOCK)clockMode = cDevMngMsgAgent::ClockMODE::HIGHCLOCK;
					}

					/*
					ans = dxo.draw(Pint(155,55), dxoButIO(Pint(70, 20), str, getClr(normal, butobj), true));
					if(ans==1){
						useMP = !useMP;
					}
					*/
				}else{
					dxo.draw(Pint(80, 105), dxoButLRIO(Pint(70, 20), str, getClr(normal, butobj), false));
					//dxo.draw(Pint(155, 55), dxoButIO(Pint(70, 20), str, getClr(normal, butobj), false));
				}
			}catch(const hmLib::inquiries::unconnected_exception& ){				
				// Inquire �ɃA�N�Z�X�ł��Ȃ������I�I�@�ł��A����Ȃ̃��C���֐��ɂ͊֌W�Ȃ�������ݏ���
				dxo.draw(Pint(80, 105), dxoStr("Inquiry::sendingClockCom �擾���s") );
			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(80, 105), dxoStr( Excp.what()) );
			}
			
			return 0;

		}
		status getStatus()const override{return normal;}
	public:
		// constructor
		dxosDevMngSUI(void):dxosBUI("HMR Device", 30, 130){
			RemLength =0;
			SleepLength =1;
			roamLength = 20;
			//useMP = true;
			clockMode =  cDevMngMsgAgent::ClockMODE::NORMALCLOCK;
		}
	};		
}

#
#endif
