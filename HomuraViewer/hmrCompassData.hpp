#ifndef HMR_COMPASS_DATA_INC
#define HMR_COMPASS_DATA_INC 100
#
/*==========hmrCompass============
hmrCompassData v1_00/130705 amby
	�R���p�X�̃f�[�^��ێ����āA�f�[�^�ɑ΂���C���^�[�t�F�[�X��^����N���X

*/
#include "hmLibVer.hpp"
#include<boost/signals2.hpp>
#include<hmLib/signals.hpp>
#include <hmLib/inquiries.hpp>
#include <hmLib/coordinates.hpp>
#include "hmrData.hpp"

namespace hmr{
	class cCompass{
		typedef hmLib::coordinates3D::position position;
		typedef hmLib::coordinates3D::polar polar;
	private:
		polar Polar;
		clock::time_point Time;

		hmLib::signals::unique_connections SignalConnections;
		hmLib::inquiries::unique_connections InquiryConnections;

	public:
		// constractor
		cCompass():Time(), Polar(){};

		// transfer functions
		static position transPolar_to_Pos(polar pol){
			return position(pol);
		}
		static polar transPos_to_Polar(position pos){
			return polar(pos);
		}

	private:
		// slot �p�̊֐�
		void setCompassData(clock::time_point time, position compassDat){
			Time = time;
			Polar = transPos_to_Polar(compassDat);
		}


	public:
		// ���͗p�֐��̒�
		// �@slot 
		void slot_addCompassData(boost::signals2::signal<void(position,clock::time_point)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](position pos,clock::time_point time)->void{setCompassData(time, pos);}));		
		}

		// �o�͗p�֐��̒�
		// �@contact 
		void contact_getPositon(hmLib::inquiries::inquiry<position>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->position{return this->transPolar_to_Pos(Polar);}));
		}

		void contact_getPolar(hmLib::inquiries::inquiry<polar>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,Polar));
		}

		void contact_getTime(hmLib::inquiries::inquiry<clock::time_point>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,Time));
		}

		void contact_getYawAngle(hmLib::inquiries::inquiry<double>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->double{return this->Polar.phi;}));
		}
	};
}
#
#endif
