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
hmrDataから分離

===hmrData===
データ型の管理用ファイル

hmrData:v1_03/130615 hmIto
	Datumハッシュ,Packetハッシュを追加
hmrData v1_02/130410 hmIto
	packet::Timeを復活
	packet/datumに、時間によるoperator<を追加
hmrData:v1_01/130324 hmIto
	packet::Timeを削除
hmrData:v1_00/130310 hmIto
	datum,packetを追加
*/
namespace hmr{
	namespace viewer{
		using clock = std::chrono::system_clock;
		using date = hmLib::date;

		inline std::string time_to_hms(const clock::time_point& time_, unsigned int msno_ = 3){
			// エポックからの経過時間を取得
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
			// エポックからの経過時間を取得
			date Date;
			Date.from_time(time_);
			return (boost::format("%04d/%02d/%02d") % Date.Year%Date.Month%Date.Day).str();
		}
	}
}
#
#endif
