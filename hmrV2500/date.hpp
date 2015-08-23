#ifndef HMLIB_DATE_INC
#define HMLIB_DATE_INC
#
#include<chrono>
namespace hmLib{
	struct date{
		typedef std::chrono::system_clock::time_point time_point;
		unsigned int Year;
		unsigned int Month;
		unsigned int Day;
		unsigned int Hour;
		unsigned int Min;
		unsigned int Sec;
		unsigned int mSec;
	public:
		date():Year(0),Month(0),Day(0),Hour(0),Min(0),Sec(0),mSec(0){}
		date(unsigned int Year_,unsigned int Month_,unsigned int Day_,unsigned int Hour_,unsigned int Min_,unsigned int Sec_,unsigned int mSec_)
			:Year(Year_),Month(Month_),Day(Day_),Hour(Hour_),Min(Min_),Sec(Sec_),mSec(mSec_){
		}
		time_point to_time()const{
			std::tm st;
			st.tm_year=Year;
			st.tm_mon=Month;
			st.tm_mday=Day;
			st.tm_hour=Hour;
			st.tm_min=Min;
			st.tm_sec=Sec;
			return std::chrono::system_clock::from_time_t(std::mktime( &st ))+std::chrono::milliseconds(mSec);
		}
		void from_time(const time_point& Time_){
			std::time_t time = std::chrono::system_clock::to_time_t(Time_);
			std::tm* st = std::localtime(&time);
			Year = st->tm_year;
			Month= st->tm_mon;
			Day  = st->tm_mday;
			Hour = st->tm_hour;
			Min  = st->tm_min;
			Sec  = st->tm_sec;
			mSec = (std::chrono::duration_cast<std::chrono::milliseconds>(Time_.time_since_epoch()).count())%1000;
		}
	};
}
#
#endif
