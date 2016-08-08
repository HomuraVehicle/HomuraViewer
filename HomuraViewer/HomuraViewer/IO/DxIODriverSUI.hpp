#ifndef HMR_VIEWER_IO_DXIODRIVERSUI_INC
#define HMR_VIEWER_IO_DXIODRIVERSUI_INC 100

#include <iostream>
#include <string>
#include "hmLibVer.hpp"
#include <hmLib_v2/hmBasic.hpp>
#include <hmLib_v2/dxObject.hpp>
#include <hmLib_v2/dxArea.hpp>
#include <hmLib/signals.hpp>
#include <boost/signals2.hpp>
#include <HomuraViewer/View/DxBUI.hpp>

/* ---- hmrDxIOSUI-----

hmrDxIOSUI.hpp :v1_00/130615 amby
	DxIOSUI�̐���J�n
*/

namespace hmr{
	namespace viewer{
		namespace io{
			// �`��N���X static
			class dxosIODriverSUI :public dxosBUI{
			private:
				//unsigned int inBufSize, outBufSize;

			public:
				// inquire �n��̒�`
				hmLib::inquiries::inquiry<bool> isOpenInq;
				hmLib::inquiries::inquiry<bool> canGetInq;
				hmLib::inquiries::inquiry<bool> canSendInq;

			public:
				//virtual int fndraw(dxO& dxf);
				int normal_draw(dxO& dxo)override{
					//int ans;

					/*
					// �������g�𖼏���ď�ԕ\��
					dxo.draw(Pint(5,5),dxoStrP(Pint(70,20),"IO",getClr(normal,strobj)));

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
							dxo.draw(Pint(5, 5), dxoStrP(Pint(70, 20), "Opened", getClr(normal, strobj)));
						} else{
							dxo.draw(Pint(5, 5), dxoStrP(Pint(70, 20), "Closed", getClr(invalid, strobj)));
						}
					}
					catch(const hmLib::inquiries::unconnected_exception&){
						// Inquire �ɃA�N�Z�X�ł��Ȃ������I�I�@�ł��A����Ȃ̃��C���֐��ɂ͊֌W�Ȃ�������ݏ���
						dxo.draw(Pint(5, 5), dxoStr("Inquiry::isOpen �擾���s"));
					}
					catch(const hmLib::exceptions::exception& Excp){
						dxo.draw(Pint(5, 5), dxoStr(Excp.what()));
					}
					// can send
					try{
						if(canSendInq()){
							dxo.draw(Pint(80, 5), dxoStrP(Pint(70, 20), "���M��", getClr(normal, strobj)));
						} else{
							dxo.draw(Pint(80, 5), dxoStrP(Pint(70, 20), "���M�s��", getClr(invalid, strobj)));
						}
					}
					catch(const hmLib::inquiries::unconnected_exception&){
						dxo.draw(Pint(80, 5), dxoStr("Inquiry::canSend �擾���s"));
					}
					catch(const hmLib::exceptions::exception& Excp){
						dxo.draw(Pint(80, 5), dxoStr(Excp.what()));
					}
					// can recv
					try{
						if(canGetInq()){
							dxo.draw(Pint(155, 5), dxoStrP(Pint(70, 20), "��M��", getClr(normal, strobj)));
						} else{
							dxo.draw(Pint(155, 5), dxoStrP(Pint(70, 20), "��M�s��", getClr(invalid, strobj)));
						}
					}
					catch(const hmLib::inquiries::unconnected_exception&){
						dxo.draw(Pint(155, 5), dxoStr("Inquiry::canGet �擾���s"));
					}
					catch(const hmLib::exceptions::exception& Excp){
						dxo.draw(Pint(155, 5), dxoStr(Excp.what()));
					}

					return 0;
				}

				int extend_draw(dxO& dxo)override{
					// draw
					normal_draw(dxo);

					/*
					// can get �n��̕`��
					// is Open
					try{
					if(isOpenInq()){
					dxo.draw(Pint(5,30),dxoStrP(Pint(70,20),"Opened",getClr(normal,strobj)));
					}else{
					dxo.draw(Pint(5,30),dxoStrP(Pint(70,20),"Closed",getClr(invalid,strobj)));
					}
					}catch(const hmLib::inquiries::unconnected_exception& ){
					// Inquire �ɃA�N�Z�X�ł��Ȃ������I�I�@�ł��A����Ȃ̃��C���֐��ɂ͊֌W�Ȃ�������ݏ���
					dxo.draw(Pint(5, 30), dxoStr("Inquiry::isOpen �擾���s") );
					}catch(const hmLib::exceptions::exception& Excp){
					dxo.draw(Pint(5, 30), dxoStr( Excp.what()) );
					}
					// can send
					try{
					if(canSendInq()){
					dxo.draw(Pint(80,30),dxoStrP(Pint(70,20),"���M��",getClr(normal,strobj)));
					}else{
					dxo.draw(Pint(80,30),dxoStrP(Pint(70,20),"���M�s��",getClr(invalid,strobj)));
					}
					}catch(const hmLib::inquiries::unconnected_exception& ){
					dxo.draw(Pint(80, 30), dxoStr("Inquiry::canSend �擾���s") );
					}catch(const hmLib::exceptions::exception& Excp){
					dxo.draw(Pint(80, 30), dxoStr( Excp.what()) );
					}
					// can recv
					try{
					if(canGetInq()){
					dxo.draw(Pint(155,30),dxoStrP(Pint(70,20),"��M��",getClr(normal,strobj)));
					}else{
					dxo.draw(Pint(155,30),dxoStrP(Pint(70,20),"��M�s��",getClr(invalid,strobj)));
					}
					}catch(const hmLib::inquiries::unconnected_exception& ){
					dxo.draw(Pint(155, 30), dxoStr("Inquiry::canGet �擾���s") );
					}catch(const hmLib::exceptions::exception& Excp){
					dxo.draw(Pint(155, 30), dxoStr( Excp.what()) );
					}
					*/

					return 0;

				}

				status getStatus()const override{
					try{
						if(isOpenInq()){
							return normal;
						} else{
							return invalid;
						}
					}
					catch(const hmLib::exceptions::exception&){
						return error;
					}
				}

			public:
				// constructor
				dxosIODriverSUI(void) :dxosBUI("IO", 30, 30){}

			};
		}
	}
}

#
#endif
