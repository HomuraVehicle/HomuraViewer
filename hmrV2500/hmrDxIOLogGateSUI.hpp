#ifndef HMR_DXIOLOGGATE_SUI_INC
#define HMR_DXIOLOGGATE_SUI_INC 100

#include <iostream>
#include <string>
#include "hmLibVer.hpp"
#include <hmLib_v2/hmBasic.hpp>
#include <hmLib_v2/dxObject.hpp>
#include <hmLib_v2/dxArea.hpp>
#include <hmLib_v3_06/signals.hpp>
#include <hmLib_v3_06/inquiries/inquiry.hpp>
#include <boost/signals2.hpp>
#include "hmrDxBUI.hpp"
//#include "hmrBaseDX.hpp"

//#define DEBUG_MODE 1

/* ---- hmrDxBufGateSUI-----

hmrDxIOLogGateSUI.hpp :v1_00/130615 amby
	DxIOLogGateSUI�̐���J�n
*/


namespace hmr{
	// �`��N���X static
	class dxosIOLogGateSUI:public dxosBUI{
	private:
		//unsigned int inBufSize, outBufSize;

	public:
		// signal �n��̒�`
		boost::signals2::signal<void(void)> signal_require_clearBuf;
		//boost::signals2::signal<void(void)> signal_require_clearOutBuf;

		// inquire �n��̒�`
		hmLib::inquiries::inquiry<unsigned int> BufSizeInq;
		//hmLib::inquiries::inquiry<unsigned int> outBufSizeInq;
	
		hmLib::inquiries::inquiry<bool> isOpenInq;
		hmLib::inquiries::inquiry<bool> canGetInq;
		hmLib::inquiries::inquiry<bool> canSendInq;

	public:
		//virtual int fndraw(dxO& dxf);
		int normal_draw(dxO& dxo)override{
			//int ans;

			/*
			// �������g�𖼏���ď�ԕ\��
			dxo.draw(Pint(5,5),dxoStrP(Pint(70,20),"IOLogGate",getClr(normal,strobj)));
			
			// is Open ��ԕ\��
			try{
				if(isOpenInq()){
					dxo.draw(Pint(80,5),dxoStrP(Pint(140,20),"Opened",getClr(normal,strobj)));
				}else{
					dxo.draw(Pint(80,5),dxoStrP(Pint(140,20),"Closed",getClr(invalid,strobj)));
				}
			}catch(const hmLib::inquiries::unconnected_exception&){				
				dxo.draw(Pint(80, 5), dxoStr("Inquiry::isOpen �擾���s") );
			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(80, 5), dxoStr( Excp.what()) );
			}
			*/
			// can get �n��̕`��
			
			// is Open
			try{
				if(isOpenInq()){
					dxo.draw(Pint(5,5),dxoStrP(Pint(70,20),"Opened",getClr(normal,strobj)));
				}else{
					dxo.draw(Pint(5,5),dxoStrP(Pint(70,20),"Closed",getClr(invalid,strobj)));
				}
			}catch(const hmLib::inquiries::unconnected_exception& ){				
				// Inquire �ɃA�N�Z�X�ł��Ȃ������I�I�@�ł��A����Ȃ̃��C���֐��ɂ͊֌W�Ȃ�������ݏ���
				dxo.draw(Pint(5, 5), dxoStr("Inquiry::isOpen �擾���s") );
			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(5, 5), dxoStr(Excp.what()) );
			}
			// buf size
			try{
				dxo.draw(Pint(120, 5),dxoStrP(Pint(30,20),"Buf:", getClr(normal,strobj)));
				dxo.draw(Pint(155, 5),dxoStrP(Pint(70,20),hmstd::fStr("%d", BufSizeInq()), getClr(normal,strobj)));
			}catch(const hmLib::inquiries::unconnected_exception&){				
				dxo.draw(Pint(80, 5), dxoStr("Inquiry::BufSize �擾���s") );
			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(80, 5), dxoStr( Excp.what()) );
			}

			return 0;
		}

		int extend_draw(dxO& dxo)override{
			// draw
			//normal_draw(dxo);

			// can get �n��̕`��
			 // is Open
			try{
				if(isOpenInq()){
					dxo.draw(Pint(5,5),dxoStrP(Pint(70,20),"Opened",getClr(normal,strobj)));
				}else{
					dxo.draw(Pint(5,5),dxoStrP(Pint(70,20),"Closed",getClr(invalid,strobj)));
				}
			}catch(const hmLib::inquiries::unconnected_exception& ){				
				// Inquire �ɃA�N�Z�X�ł��Ȃ������I�I�@�ł��A����Ȃ̃��C���֐��ɂ͊֌W�Ȃ�������ݏ���
				dxo.draw(Pint(5, 5), dxoStr("Inquiry::isOpen �擾���s") );
			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(5, 5), dxoStr(Excp.what()) );
			}
			 // can send
			try{
				if(canSendInq()){
					dxo.draw(Pint(80,5),dxoStrP(Pint(70,20),"���M��",getClr(normal,strobj)));
				}else{
					dxo.draw(Pint(80,5),dxoStrP(Pint(70,20),"���M�s��",getClr(invalid,strobj)));
				}
			}catch(const hmLib::inquiries::unconnected_exception& ){				
				dxo.draw(Pint(80, 5), dxoStr("Inquiry::canSend �擾���s") );
			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(80, 5), dxoStr( Excp.what()) );
			}
			 // can recv
			try{
				if(canGetInq()){
					dxo.draw(Pint(155,5),dxoStrP(Pint(70,20),"��M��",getClr(normal,strobj)));
				}else{
					dxo.draw(Pint(155,5),dxoStrP(Pint(70,20),"��M�s��",getClr(invalid,strobj)));
				}
			}catch(const hmLib::inquiries::unconnected_exception& ){				
				dxo.draw(Pint(155, 5), dxoStr("Inquiry::canGet �擾���s") );
			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(155, 5), dxoStr( Excp.what()) );
			}


			// inBuf Size �ɂ��Ă̏�ԕ\��
			try{
				dxo.draw(Pint(5, 30),dxoStrP(Pint(70,20),"Buf", getClr(normal,strobj)));
				dxo.draw(Pint(80, 30),dxoStrP(Pint(70,20),hmstd::fStr("%d", BufSizeInq()), getClr(normal,strobj)));
				if( dxo.draw(Pint(155,30),dxoBut(Pint(70,20),"CLR", getClr(normal,strobj))) == 1){
					signal_require_clearBuf();
				}
			}catch(const hmLib::inquiries::unconnected_exception&){				
				dxo.draw(Pint(5, 30), dxoStr("Inquiry::BufSize �擾���s") );
			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(5, 30), dxoStr( Excp.what()) );
			}

			/*
			// outBuf Size �ɂ��Ă̏�ԕ\��
			try{
				dxo.draw(Pint(5, 55),dxoStrP(Pint(70,20),"OutBuf", getClr(normal,strobj)));
				dxo.draw(Pint(80, 55),dxoStrP(Pint(70,20),hmstd::fStr("%d", outBufSizeInq()), getClr(normal,strobj)));
				if( dxo.draw(Pint(155, 55),dxoBut(Pint(70,20),"CLR", getClr(normal,strobj))) == 1){
					signal_require_clearOutBuf();
				}
			}catch(const hmLib::inquiries::unconnected_exception&){				
				dxo.draw(Pint(5, 55), dxoStr("Inquiry::outBufSize �擾���s") );
			}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(5, 55), dxoStr( Excp.what()) );
			}
			*/


			
			return 0;

		}

		status getStatus()const override{
			try{
				if(isOpenInq()){
					return normal;
				}else{
					return invalid;
				}
			}catch(const hmLib::exceptions::exception& ){
				return error;
			}
		}

	public:
		// constructor
		dxosIOLogGateSUI(void):dxosBUI("IOLogGate", 30, 55){}
	
	};		
}

#
#endif
