
#ifndef HMR_BATTERYFILE_INC
#define HMR_BATTERYFILE_INC 100
#

/*===hmrBatteryFile===
Battery ファイル関係のクラス
　ログはスロットを通して行うことにする

hmrBatteryFile v1_01/130907 amby
　　これまで、バッテリ数のシグナルとスロットをつなぐところで、スロットとして同じ関数を引数の値だけバッテリによって変化させることで対応していた。　
  　　このような形：SignalConnections(hmLib::signals::connect(Signal_,[&](clock::time_point time_, double data_)->void{this->logData(time_, data_, num_);}));	

	しかし、結局関数はポインタでは一つしか用意されないので、ポインタではこれらの引数の違いを区別することができないのかな？？
	よって、無理やりテンプレート関数に変更

hmrBatteryFile v1_00/130720 amby
	作成開始
*/

#include <array>
#include <string>
#include <fstream>
#include <boost/signals2.hpp>
#include <hmLib/signals.hpp>
#include <hmrVLib/File.hpp>
#include <hmrVLib/Data.hpp>


namespace hmr{
	template<unsigned int BatteryNum_>
	class cBatteryFileAgent:public itfFileAgent{
	private:
		// Dataが入っていれば true, でなければ false
		typedef std::pair<bool, double> battData;
		// バッテリそれぞれの要素
		typedef std::array<battData, BatteryNum_> battDataSet;
		
		battDataSet bBuf;
		clock::time_point time;
		
		std::ofstream ofs;
		hmLib::signals::unique_connections SignalConnections;


		// log 関数
		template<unsigned int BattNo>
		void logData(clock::time_point time_, double data_){
			// 開いていなければ・・・・
			if(!is_active()) return;

			// Dataが詰まっていれば
			if(bBuf.at(BattNo).first){
				writeFile();
			}
			
			// Logを始める
			time = time_;
			bBuf.at(BattNo).second = data_;
			bBuf.at(BattNo).first = true;

			return;	
		}

		// write file関数
		void writeFile(void){
			// 開いてなければ・・・・
			if(!is_active()) return;

			// File に書き込む
			ofs << hmr::time_to_hms(time) ;
			for(int i = 0; i< BatteryNum_; i++){
				// write
				if( bBuf.at(i).first ){
					ofs << FILE_DEVIDE_CHARS << bBuf.at(i).second;
				}else{
					ofs << FILE_DEVIDE_CHARS;
				}

				//Flag clear
				bBuf.at(i).first = false;
			}
			ofs << std::endl;

			return;
		}


	public:
		cBatteryFileAgent(){
			for(int i=0;i<BatteryNum_;i++){
				bBuf.at(i).first = false;
				bBuf.at(i).second = 0.;
			}
		}
		void activate(const std::string& Path_){
			if(!is_active()){
				ofs.open(Path_ + "Battery.txt");
				ofs << FILE_COMMENTOUT_CHARS << " This is the Battery data log file" << std::endl;
				ofs << FILE_COMMENTOUT_CHARS << " Time" << FILE_DEVIDE_CHARS << "Data.... [" << BatteryNum_ << "]" << std::endl;
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
		template <unsigned int BattNo>
		void slot_logData(boost::signals2::signal<void(clock::time_point, double)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](clock::time_point time_, double data_)->void{this->logData<BattNo>(time_, data_);}));	
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









