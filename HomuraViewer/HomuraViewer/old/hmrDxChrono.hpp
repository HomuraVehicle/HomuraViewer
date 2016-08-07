#ifndef HMR_DXCHRONOSUI_INC
#define HMR_DXCHRONOSUI_INC 100

#include "hmLibVer.hpp"
#include <iostream>
#include <string>
#include <chrono>
#include <boost/signals2.hpp>
#include <hmLib/signals.hpp>
#include <hmLib/inquiries.hpp>
#include <hmLib_v2/hmBasic.hpp>
#include <hmLib_v2/dxObject.hpp>
#include <hmLib_v2/dxArea.hpp>

#include "hmrDxBUI.hpp"

//#include "hmrBaseDX.hpp"

//#define DEBUG_MODE 1

/* ---- hmrDxBufGateSUI-----

hmrDxVMCSUI.hpp :v1_00/130615 amby
DxVMCSUI�̐���J�n
*/


namespace hmr {
	namespace viewer{
		// �`��N���X static
		class dxosChronoSUI :public dxosBUI{
		private:
			//�@�̂̎��Ԃ̂�����L�^���Ă���
			std::chrono::milliseconds MachineDifTime;
		public:
			//�����擾��v������
			boost::signals2::signal<void(void)> signal_requestMachineTime;

			//�@�̎��Ԃ̂�����擾����
			void slot_getMachineDifTime(boost::signals2::signal<void(std::chrono::milliseconds)>& Signal_){
				hmLib::signals::connect(Signal_, [&](std::chrono::milliseconds MachineDifTime_)->void {MachineDifTime = MachineDifTime_; });
			}

			//�����␳���\���ǂ������m�F����
			hmLib::inquiries::inquiry<bool> inquiry_can_correctMachineTime;

			//�����␳��v������
			boost::signals2::signal<void(void)> signal_correctMachineTime;

		public:
			//virtual int fndraw(dxO& dxf);
			int normal_draw(dxO& dxo)override{
				//�����\��
				try{
					bool CanCorrect = inquiry_can_correctMachineTime();
					std::string Str;
					dxRGB Clr;

					auto Time = clock::now() + MachineDifTime;
					if(CanCorrect){
						Str = time_to_ymd(Time) + " " + time_to_hms(Time);
						Clr = getClr(normal, butobj);
					} else{
						Str = "���v��";
						Clr = getClr(normal, butobj);
					}

					if(dxo.draw(Pint(5, 5), dxoBut(Pint(225, 20), Str, Clr)) < 0){
						signal_requestMachineTime();
					}
				}
				catch(const hmLib::inquiries::unconnected_exception&){
					dxo.draw(Pint(5, 5), dxoStr("errUC"));
				}
				catch(const hmLib::exceptions::exception& Excp){
					dxo.draw(Pint(5, 5), dxoStr(Excp.what()));
				}

				return 0;
			}
			int extend_draw(dxO& dxo)override{
				normal_draw(dxo);
				//�␳
				try{
					bool CanCorrect = inquiry_can_correctMachineTime();
					std::string Str;
					dxRGB Clr;

					if(CanCorrect){
						Str = "�␳";
						Clr = getClr(normal, butobj);
					} else{
						Str = "�␳";
						Clr = getClr(invalid, butobj);
					}

					if(dxo.draw(Pint(5, 30), dxoBut(Pint(45, 20), Str, Clr), CanCorrect) < 0){
						signal_correctMachineTime();
					}
				}
				catch(const hmLib::inquiries::unconnected_exception&){
					dxo.draw(Pint(5, 30), dxoStr("errUC"));
				}
				catch(const hmLib::exceptions::exception& Excp){
					dxo.draw(Pint(5, 30), dxoStr(Excp.what()));
				}

				//�������b�\��
				try{
					bool CanCorrect = inquiry_can_correctMachineTime();
					std::string Str;
					dxRGB Clr;

					auto Time = clock::now() + MachineDifTime;
					if(CanCorrect){
						Str = (boost::format("%.3lf") % (MachineDifTime.count() / 1000.)).str();
					} else{
						Str = "---";
					}

					if(dxo.draw(Pint(55, 30), dxoStrP(Pint(175, 20), Str)) < 0){
						signal_requestMachineTime();
					}
				}
				catch(const hmLib::inquiries::unconnected_exception&){
					dxo.draw(Pint(55, 30), dxoStr("errUC"));
				}
				catch(const hmLib::exceptions::exception& Excp){
					dxo.draw(Pint(55, 30), dxoStr(Excp.what()));
				}

				return 0;
			}
			status getStatus()const override{
				return normal;
			}
		public:
			// constructor
			dxosChronoSUI(void) :dxosBUI("Chrono", 30, 55){}
		};
	}
}

#
#endif
