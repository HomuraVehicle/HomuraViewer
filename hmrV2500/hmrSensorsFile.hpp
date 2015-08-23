
#ifndef HMR_SENSORSFILE_INC
#define HMR_SENSORSFILE_INC 101
#

/*===hmrSensorsFile===
Sensor ファイル関係のクラス
　ログはスロットを通して行うことにする

 とりあえず、複雑なデータ構造を持たないセンサ類のログをまとめてとる
 　スロットを多用しているので、簡単にプログラムをいじって拡張できればよいと考えて、動的な汎用性は求めなかった

hmrSensorsFile v1_01/130907 amby
	関数の引数でスロットを区別していたので、それをテンプレートの形に変更	

hmrSensorsFile v1_00/130720 amby
	作成開始
*/

#include <iostream>
#include <string>
#include <fstream>
#include <array>
#include <boost/signals2.hpp>
#include <hmLib_v3_05/signals.hpp>
#include "hmrItfFile.hpp"
#include"hmrData.hpp"


namespace hmr{
	class cSensorsFileAgent:public itfFileAgent{
	public:
		typedef enum{THERMO = 0, INF_THERMO=1, HUMID=2, H2S=3, CO2=4, SENSOR_NUM=5} SENSOR_NAME;
		//SENSOR_NAME S_NAME;
	private:
		// Dataが入っていれば true, でなければ false
		typedef std::pair<bool, double> sensorData;
		// バッテリそれぞれの要素
		typedef std::array<sensorData, (int)SENSOR_NAME::SENSOR_NUM> sensorDataSet;
		
		sensorDataSet sBuf;
		clock::time_point time;
		
		std::ofstream ofs;
		hmLib::signals::unique_connections SignalConnections;


		// log 関数
		template<SENSOR_NAME sName>
		void logData(clock::time_point time_, double data_){
			// 開いていなければ・・・・
			if(!is_active()) return;

			// Dataが詰まっていれば
			if(sBuf.at(sName).first){
				writeFile();
			}
			
			// Logを始める
			time = time_;
			sBuf.at(sName).second = data_;
			sBuf.at(sName).first = true;

			return;	
		}

		// write file関数
		void writeFile(void){
			// 開いてなければ・・・・
			if(!is_active()) return;

			// File に書き込む
			ofs << hmr::time_to_hms(time) ;
			for(int i = 0; i< SENSOR_NUM; i++){
				// write
				if( sBuf.at(i).first ){
					ofs << FILE_DEVIDE_CHARS << sBuf.at(i).second;
				}else{
					ofs << FILE_DEVIDE_CHARS;
				}

				//Flag clear
				sBuf.at(i).first = false;
			}
			ofs << std::endl;

			return;
		}


	public:
		cSensorsFileAgent(){
			for(int i=0;i<SENSOR_NUM;i++){
				sBuf.at(i).first = false;
				sBuf.at(i).second = 0.;
			}
		}
		void activate(const std::string& Path_){
			if(!is_active()){
				ofs.open(Path_ + "sensors.txt");
				ofs << FILE_COMMENTOUT_CHARS << " This is the Sensors data log file" << std::endl;
				ofs << FILE_COMMENTOUT_CHARS << " Time" << FILE_DEVIDE_CHARS << " Thermo "
					<< FILE_DEVIDE_CHARS << " InfThermo " <<  FILE_DEVIDE_CHARS << " Humid "  
					<< FILE_DEVIDE_CHARS << " H2S " <<  FILE_DEVIDE_CHARS << " CO2 " 
					<< std::endl;
			}
		}
		bool is_active() const {return ofs.is_open();}
		void inactivate(){
			if(is_active()){
				ofs << FILE_COMMENTOUT_CHARS << "EOF" << std::endl;
				ofs.close();
			}
		}
	
	// Log 用の signal slot
	public:
		// Logとして書き込む時のslot
		template <SENSOR_NAME sName>
		void slot_logData(boost::signals2::signal<void(double, clock::time_point)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](double data_, clock::time_point time_)->void{this->logData<sName>(time_, data_);}));	
		}

		// InfThermo用
		void slot_logInfThermoData(boost::signals2::signal<void(double, double,double, clock::time_point)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](double data_,double d1, double d2, clock::time_point time_)->void{this->logData<INF_THERMO>(time_, data_);}));
		}

		// ファイルを保存するためのslot
		void slot_writeData(boost::signals2::signal<void(void)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](void)->void{this->writeFile();}));	
		}


		/*
		template<typename iterator>
		unsigned int get_rangesize(iterator begin,iterator end){return end-begin;}
		template<unsigned int No>
		void output_no(void){
			unsigned int Values[No];

			std::cout<<No<<std::endl;
		}
		output_no<45>();

		get_rangesize<std::vector<int>::iterator>(Begin,End);
		*/
	
	};
}
#endif









