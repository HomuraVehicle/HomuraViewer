#ifndef HMR_DXIODISPLAY_INC
#define HMR_DXIODISPLAY_INC 100
#

#include <hmLib/config_vc.h> // std::min, std::max�@�́@windows�}�N���ł���`����Ă���A�����I�ɕ������Ă��Ȃ��Ƃ���

#include <iostream>
#include <string>
#include <vector>

#include "hmLibVer.hpp"

#include <hmLib_v2/hmBasic.hpp>
#include <hmLib_v2/dxObject.hpp>
#include <hmLib_v2/dxArea.hpp>
#include <hmLib_v2/dxColorSet.hpp>

#include "IO/DxGateSwitcher.hpp"
#include "IO/DxIO_v2.hpp"

/* === hmrDxIODisplay.hpp ===   

IO�֘A�̕`��p�̃y�[�W�����

hmrDxIODisplay:v1_00/130602 amby

============================= */

namespace hmr{
	namespace viewer{
/*
		// Sub Page
		class dxosIOSubPage :public dxReObject, public hmoBox{
		private:
			dxRGB Clr;

		public:
			// Gait switcher 
			hmr::dxosGateSwitcher DxosGateSwitcher;

			// �`��̂��߂̗v�f���`
			hmr::dxosStartControl DxSendStartButt;
			hmr::dxosBufControl DxReadBufCtr;
			hmr::dxosBufControl DxSendBufCtr;
			hmr::dxosTimeIntervalControl DxSendIntervalCtr;
			hmr::dxosTimeOutControl DxSendTimeOutCtr;
			hmr::dxosSyncControl DxSyncCheckBox;
			hmr::dxosSelectDuplex DxSelectDuplex;

		public:
			// constructor
			dxosIOSubPage(Pint Size_, const dxColor& Clr_)
				:hmoBox(Size_)
				, Clr(Clr_)
				, DxosGateSwitcher(CLR::SoftOrenge)
				, DxSendStartButt(Pint(220, 40), Clr, "Send Start")
				, DxReadBufCtr(Pint(200, 30), Clr, "Read Buf Clear")
				, DxSendBufCtr(Pint(200, 30), Clr, "Send Buf Clear")
				, DxSendIntervalCtr(Pint(200, 30), Clr, "Interval")
				, DxSendTimeOutCtr(Pint(200, 30), Clr, "TimeOut")
				, DxSyncCheckBox(Pint(200, 20), Clr)
				, DxSelectDuplex(Pint(220, 20), Clr){}

		protected:
			virtual int redraw(dxO& dxo){
				dxo.draw(Pint(0, 0), DxosGateSwitcher);

				dxo.draw(Pint(10, 300), DxSendStartButt);
				dxo.draw(Pint(10, 350), DxSelectDuplex);

				dxo.draw(Pint(30, 420), DxSendIntervalCtr);
				dxo.draw(Pint(30, 380), DxSendTimeOutCtr);
				dxo.draw(Pint(30, 455), DxSyncCheckBox);


				dxo.draw(Pint(30, 500), DxSendBufCtr);
				dxo.draw(Pint(30, 540), DxReadBufCtr);






				return 0;
			}
		};

		// Main Page
		template <typename iterator>
		class dxosIOMainPage :public dxReObject, public hmoBox{
		private:
			dxRGB Clr;

		public:
			// �`��̂��߂̗v�f���`
			// IO View�@�N���X�̒�`
			hmr::dxosIO<iterator> DxIO;
			// IO send box �N���X�̒�`
			hmr::dxosIOSendBox DxSendBox;
			// IO setFlag �N���X�̒�`
			hmr::dxosSelectIOPaint DxSelectIOPaint;

		public:
			// constructor
			//  ���̃R���X�g���N�^�́A�K���@dx::ini()�@�̌�ɌĂԂ��ƁI�I
			dxosIOMainPage(Pint Size_, const dxColor& Clr_)
				:hmoBox(Size_)
				, Clr(Clr_)
				, DxIO(Pint(720, 690), Clr)
				, DxSendBox(Pint(640, 30), Clr)
				, DxSelectIOPaint(Pint(80, 30)){
				// initialize
				//  �K�� dx::ini() �̌�ɏ����������Ă��
				DxSendBox.ini();
			}
		protected:
			virtual int redraw(dxO& dxo){
				dxo.draw(Pint(80, 0), DxSendBox);
				dxo.draw(Pint(0, 30), DxIO);
				dxo.draw(Pint(0, 0), DxSelectIOPaint);
				return 0;
			}
		};
	*/
		// Main Page
		template <typename iterator>
		class dxosIOMainPage2 :public dxReObject, public hmoBox{
		private:
			dxRGB Clr;

		public:
			// �`��̂��߂̗v�f���`
			// IO View�@�N���X�̒�`
			io::dxosIO2<iterator> DxIO;
			// IO send box �N���X�̒�`
			io::dxosIOSendBox2 DxSendBox;
			// IO setFlag �N���X�̒�`
			io::dxosSelectIOPaint DxSelectIOPaint;

		public:
			// constructor
			//  ���̃R���X�g���N�^�́A�K���@dx::ini()�@�̌�ɌĂԂ��ƁI�I
			dxosIOMainPage2(Pint Size_, const dxColor& Clr_)
				:hmoBox(Size_)
				, Clr(Clr_)
				, DxIO(Pint(720, 690), Clr)
				, DxSendBox(Pint(640, 30), Clr)
				, DxSelectIOPaint(Pint(80, 30)){
				// initialize
				//  �K�� dx::ini() �̌�ɏ����������Ă��
				DxSendBox.ini();
			}

			/*
			// initialize �֐��@
			//  �K�� dx::ini() �̌�ɏ����������Ă��
			void initialize(){
				DxSendBox.ini();
			}
			*/

		protected:
			virtual int redraw(dxO& dxo){
				dxo.draw(Pint(80, 0), DxSendBox);
				dxo.draw(Pint(0, 30), DxIO);
				dxo.draw(Pint(0, 0), DxSelectIOPaint);
				return 0;
			}
		};
	}
}
#
#endif


