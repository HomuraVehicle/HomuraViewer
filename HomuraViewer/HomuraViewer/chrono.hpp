#ifndef HMR_VIEWER_CHRONO_INC
#define HMR_VIEWER_CHRONO_INC 100
#
#include<string>
#include<ctime>
#include<chrono>
#include<boost/format.hpp>
#include<hmLib/date.hpp>
/*
=== chrono ===
hmrData���番��

===hmrData===
�f�[�^�^�̊Ǘ��p�t�@�C��

hmrData:v1_03/130615 hmIto
	Datum�n�b�V��,Packet�n�b�V����ǉ�
hmrData v1_02/130410 hmIto
	packet::Time�𕜊�
	packet/datum�ɁA���Ԃɂ��operator<��ǉ�
hmrData:v1_01/130324 hmIto
	packet::Time���폜
hmrData:v1_00/130310 hmIto
	datum,packet��ǉ�
*/
namespace hmr{
	namespace viewer{
		using clock = std::chrono::system_clock;
		using date = hmLib::date;

		inline std::string time_to_hms(const clock::time_point& time_, unsigned int msno_ = 3){
			// �G�|�b�N����̌o�ߎ��Ԃ��擾
			date Date;
			Date.from_time(time_);
			if(msno_ == 0){
				return (boost::format("%02d:%02d:%02d") % Date.Hour%Date.Min%Date.Sec).str();
			} else if(msno_ == 1){
				return (boost::format("%02d:%02d:%02d.%01d") % Date.Hour%Date.Min%Date.Sec % (Date.mSec / 100)).str();
			} else if(msno_ == 2){
				return (boost::format("%02d:%02d:%02d.%02d") % Date.Hour%Date.Min%Date.Sec % (Date.mSec / 10)).str();
			} else{
				return (boost::format("%02d:%02d:%02d.%03d") % Date.Hour%Date.Min%Date.Sec%Date.mSec).str();
			}
		}
		inline std::string time_to_ymd(const clock::time_point& time_){
			// �G�|�b�N����̌o�ߎ��Ԃ��擾
			date Date;
			Date.from_time(time_);
			return (boost::format("%04d/%02d/%02d") % Date.Year%Date.Month%Date.Day).str();
		}
	}
}
#
#endif
