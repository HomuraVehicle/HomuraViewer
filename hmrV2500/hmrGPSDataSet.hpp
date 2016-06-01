#ifndef HMR_GPSDATASET_INC
#define HMR_GPSDATASET_INC 101
#

/*===========hmrGPSDataSet===========
hmrGPS v1_01/130907 amby
	６２行目のGPGGA変換部分でバグを修正、一応boost::formatがエラーを吐くので、
	エラーを出力として出すことでもみ消すようにした。
*/


#include<boost/format.hpp>
#include<hmLib_v3_06/position.hpp>
#include"hmrData.hpp"
#include"date.hpp"

namespace hmr{
	class GPSDataSet{
	public:
		typedef hmLib::pdouble pdouble;
	private:
		//使用したGPS
		int UseGPS;
		pdouble Pos;
		double	Height;
		clock::time_point Time;
		clock::time_point UTC;
		bool failFlag;
		bool errFlag;
	public:
		GPSDataSet():
			UseGPS(0),
			Pos(0.,0.),
			Height(0.),
			Time(),
			UTC(),
			failFlag(false),
			errFlag(false){
		}
		//コンストラクタ
		GPSDataSet(pdouble _Pos,double _Height,clock::time_point _Time, clock::time_point _UTC,bool _failFlag,bool _errFlag,int _UseGPS):
			UseGPS(_UseGPS),
			Pos(_Pos),
			Height(_Height),
			Time(_Time),
			UTC(_UTC),
			failFlag(_failFlag),
			errFlag(_errFlag){
		}
		//operatorオーバーライド
		bool operator<(const GPSDataSet& _dat)const{return Time<_dat.Time;}
//		const Datum& operator=(const Datum& _dat){return *this;}
		//各データ取得用関数
		int getUseGPS()const{return UseGPS;}
		pdouble getPos()const{return Pos;}
		double getHeight()const{return Height;}
		clock::time_point getTime()const{return Time;}
		clock::time_point getUTC()const{return UTC;}
		bool getErr()const{return errFlag;}
		bool getFail()const{return failFlag;}
		std::string strGPGGA()const{
			// 失敗することが多いので、エラー処理を加えて無視するようにする
			try{
				std::string tmp;
				tmp=boost::format("$GPGGA,").str();
				hmLib::date UTCD;
				UTCD.from_time(getUTC());
				tmp+=(boost::format("%02d%02d%02d.%03d,")%UTCD.Hour%UTCD.Min%UTCD.Sec%UTCD.mSec).str();
				int d=(int)abs(getPos().y);
				int m=(int)abs(getPos().y*60.)-d*60;
				int n=(int)abs(getPos().y*600000.)-d*600000-m*10000;
				tmp+=(boost::format("%02d%02d.%04d,%c,")%d%m%n%(getPos().y>0?'N':'S')).str();
				d=(int)abs(getPos().x);
				m=(int)abs(getPos().x*60.)-d*60;
				n=(int)abs(getPos().x*600000.)-d*600000-m*10000;
				tmp+=(boost::format("%03d%02d.%04d,%c,")%d%m%n%(getPos().x>0?'E':'W')).str();
				tmp+=(boost::format("%d,%02d,%02d.%d,")%(int)getFail()%3%1%0).str();
				d=(int)getHeight();
				m=(int)(getHeight()*10.)-d*10;
				tmp+=(boost::format("%05d,%d,M,")%d%m).str();
				tmp+=(boost::format("%04d.%d,M,%03d.%d,0000*")%0%0%0%0).str();
				char csum=0;
				for(int i=1;i<(int)tmp.size()-1;i++)csum^=tmp[i];
				tmp.push_back(csum);
				tmp.push_back(0x0d);
				tmp.push_back(0x0a);
				return tmp;
			}
			catch(const std::exception& ex ){
				std::string tmp =  ex.what();
				return tmp;
			}
		}
	};
}

#
#endif