#ifndef HMR_VIEWER_IO_DXBUFGATESUI_INC
#define HMR_VIEWER_IO_DXBUFGATESUI_INC 100

#include <iostream>
#include <string>
#include "hmLibVer.hpp"
#include <hmLib_v2/hmBasic.hpp>
#include <hmLib_v2/dxObject.hpp>
#include <hmLib_v2/dxArea.hpp>
#include <hmLib/signals.hpp>
#include <boost/signals2.hpp>
#include <HomuraViewer/View/DxBUI.hpp>
//#include "hmrBaseDX.hpp"

//#define DEBUG_MODE 1

/* ---- hmrDxBufGateSUI-----

hmrDxBufGateSUI.hpp :v1_00/130615 amby
	DxBufGateSUI�̐���J�n
*/


namespace hmr{
	namespace viewer{
		namespace io{
			// �`��N���X static
			class dxosBufGateSUI :public dxosBUI{
			private:
				//unsigned int inBufSize, outBufSize;

			public:
				// signal �n��̒�`
				boost::signals2::signal<void(void)> signal_require_clearInBuf;
				boost::signals2::signal<void(void)> signal_require_clearOutBuf;

				// inquire �n��̒�`
				hmLib::inquiries::inquiry<unsigned int> inBufSizeInq;
				hmLib::inquiries::inquiry<unsigned int> outBufSizeInq;

				hmLib::inquiries::inquiry<bool> isOpenInq;
				hmLib::inquiries::inquiry<bool> canGetInq;
				hmLib::inquiries::inquiry<bool> canSendInq;

			public:
				//virtual int fndraw(dxO& dxf);
				int normal_draw(dxO& dxo)override{
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

					// out Buf
					try{
						dxo.draw(Pint(80, 5), dxoStrP(Pint(20, 20), "O:", getClr(normal, strobj)));
						dxo.draw(Pint(100, 5), dxoStrP(Pint(50, 20), hmstd::fStr("%d", outBufSizeInq()), getClr(outBufSizeInq() < 200 ? normal : error, strobj)));
					}
					catch(const hmLib::inquiries::unconnected_exception&){
						dxo.draw(Pint(80, 5), dxoStr("Inquiry::outBufSize �擾���s"));
					}
					catch(const hmLib::exceptions::exception& Excp){
						dxo.draw(Pint(80, 5), dxoStr(Excp.what()));
					}

					// inBuf Size �ɂ��Ă̏�ԕ\��
					try{
						dxo.draw(Pint(155, 5), dxoStrP(Pint(20, 20), "I:", getClr(normal, strobj)));
						dxo.draw(Pint(175, 5), dxoStrP(Pint(50, 20), hmstd::fStr("%d", inBufSizeInq()), getClr(inBufSizeInq() < 200 ? normal : error, strobj)));
					}
					catch(const hmLib::inquiries::unconnected_exception&){
						dxo.draw(Pint(155, 5), dxoStr("Inquiry::inBufSize �擾���s"));
					}
					catch(const hmLib::exceptions::exception& Excp){
						dxo.draw(Pint(155, 5), dxoStr(Excp.what()));
					}


					return 0;
				}

				int extend_draw(dxO& dxo)override{
					// draw
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

					// outBuf Size �ɂ��Ă̏�ԕ\��
					try{
						dxo.draw(Pint(5, 30), dxoStrP(Pint(70, 20), "OutBuf", getClr(normal, strobj)));
						dxo.draw(Pint(80, 30), dxoStrP(Pint(70, 20), hmstd::fStr("%d", outBufSizeInq()), getClr(outBufSizeInq() < 200 ? normal : error, strobj)));
						if(dxo.draw(Pint(155, 30), dxoBut(Pint(70, 20), "CLR", getClr(normal, strobj))) == 1){
							signal_require_clearOutBuf();
						}
					}
					catch(const hmLib::inquiries::unconnected_exception&){
						dxo.draw(Pint(5, 30), dxoStr("Inquiry::outBufSize �擾���s"));
					}
					catch(const hmLib::exceptions::exception& Excp){
						dxo.draw(Pint(5, 30), dxoStr(Excp.what()));
					}

					// inBuf Size �ɂ��Ă̏�ԕ\��
					try{
						dxo.draw(Pint(5, 55), dxoStrP(Pint(70, 20), "InBuf", getClr(normal, strobj)));
						dxo.draw(Pint(80, 55), dxoStrP(Pint(70, 20), hmstd::fStr("%d", inBufSizeInq()), getClr(inBufSizeInq() < 200 ? normal : error, strobj)));
						if(dxo.draw(Pint(155, 55), dxoBut(Pint(70, 20), "CLR", getClr(normal, strobj))) == 1){
							signal_require_clearInBuf();
						}
					}
					catch(const hmLib::inquiries::unconnected_exception&){
						dxo.draw(Pint(5, 55), dxoStr("Inquiry::inBufSize �擾���s"));
					}
					catch(const hmLib::exceptions::exception& Excp){
						dxo.draw(Pint(5, 55), dxoStr(Excp.what()));
					}




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
				dxosBufGateSUI(void) :dxosBUI("BufGate", 30, 80){}

			};
		}
	}
}

#
#endif
