#ifndef HMR_DATA_INC
#define HMR_DATA_INC 102
#
#include<hmLib/config_vc.h>
#include<sstream>
#include<string>
#include<ctime>
#include<chrono>
#include<vector>
#include<boost/format.hpp>
#include"hmrVMC1.h"
/*===hmrData===
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
	typedef std::chrono::system_clock clock;
	struct date{
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
		clock::time_point to_time()const{
			std::tm st;
			st.tm_year=Year-1900;
			st.tm_mon=Month-1;
			st.tm_mday=Day;
			st.tm_hour=Hour;
			st.tm_min=Min;
			st.tm_sec=Sec;
			return clock::from_time_t(std::mktime( &st ))+std::chrono::milliseconds(mSec);
		}
		void from_time(const clock::time_point& Time_){
			std::time_t time = clock::to_time_t(Time_);
			std::tm* st = std::localtime(&time);
			Year = st->tm_year+1900;
			Month= st->tm_mon+1;
			Day  = st->tm_mday;
			Hour = st->tm_hour;
			Min  = st->tm_min;
			Sec  = st->tm_sec;
			mSec = (std::chrono::duration_cast<std::chrono::milliseconds>(Time_.time_since_epoch()).count())%1000;
		}
	};
	inline std::string time_to_hms(const clock::time_point& time_,unsigned int msno_=3){
		// �G�|�b�N����̌o�ߎ��Ԃ��擾
		date Date;
		Date.from_time(time_);

		std::stringstream sout;
		if(msno_==0){
			sout<<boost::format("%02d:%02d;%02d")%Date.Hour%Date.Min%Date.Sec;
		}else if(msno_==1){
			sout<<boost::format("%02d:%02d;%02d.%01d")%Date.Hour%Date.Min%Date.Sec%(Date.mSec/100);
		}else if(msno_==2){
			sout<<boost::format("%02d:%02d;%02d.%02d")%Date.Hour%Date.Min%Date.Sec%(Date.mSec/10);
		}else{
			sout<<boost::format("%02d:%02d;%02d.%03d")%Date.Hour%Date.Min%Date.Sec%Date.mSec;
		}
		return sout.str();
	}
	inline std::string time_to_ymd(const clock::time_point& time_) {
		// �G�|�b�N����̌o�ߎ��Ԃ��擾
		date Date;
		Date.from_time(time_);

		std::stringstream sout;
		sout<<boost::format("%04d/%02d/%02d")%Date.Year%Date.Month%Date.Day;

		return sout.str();
	}
	template<typename object>
	class no_generator{
	public:
		typedef unsigned int no_type;
	private:
		static unsigned int No;
	public:
		static unsigned int no(void){return ++No;}
	};
	template<typename object>
	unsigned int no_generator<object>::No=0;

	struct datum{
		typedef no_generator<datum> no_gen;
		typedef no_gen::no_type no_type;
		typedef vmc1_did_t id_type;
		typedef vmc1_dsize_t size_type;
		typedef clock::time_point time_point;
	public:
		//�f�[�^�ԍ�
		no_type No;
		//�f�[�^����
		time_point Time;
		//�f�[�^ID
		id_type ID;
		// �f�[�^��
		size_type Size;
		//������
		//	���ӁF�����ɂ͏����ȃR�}���h�̂ݓ���B�h�c�C�r�������̃f�[�^�͊܂܂�Ȃ�
		std::string Str;
		//�G���[�t���O
		bool ErrFlag;
	public:
		datum():No(0){}
	public:
		bool operator<(const datum& My_)const{return No<My_.No;}
		bool operator==(const datum& My_)const{return No == My_.No;}
		bool operator!=(const datum& My_)const{return No != My_.No;}

		
//		bool operator<(const time_point& Time_)const{return Time<Time_;}
//		bool operator==(const time_point& Time_)const{return Time == Time_;}
//		bool operator!=(const time_point& Time_)const{return Time != Time_;}

	};
	struct packet{
		typedef no_generator<packet> no_gen;
		typedef no_gen::no_type no_type;
		typedef clock::time_point time_point;
		typedef unsigned int no_type;
	public:
		//�ԍ�
		no_type No;
		//�p�P�b�g����
		time_point Time;
		//�f�[�^
		std::vector<datum> Data;
		//�G���[�p�P�b�g�t���O
		bool ErrFlag;
	public:
		packet():No(0){}
	public:
		bool operator<(const packet& My_)const{return No<My_.No;}
		bool operator==(const packet& My_)const{return No == My_.No;}

//		bool operator<(const time_point& Time_)const{return Time<Time_;}
//		bool operator==(const time_point& Time_)const{return Time == Time_;}
	};
}
#
#endif
