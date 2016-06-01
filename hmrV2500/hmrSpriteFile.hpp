
#ifndef HMR_SPRITEFILE_INC
#define HMR_SPRITEFILE_INC 101
#

/*===hmrSpriteFile===
 Sprite ファイル関係のクラス
	　
	 １、各jpegファイルを作るクラス
	 ２、全体を統括するクラス
	 　　１のクラスを子クラスとして持ってそれを制御する

	の二段構成にする
	それぞれは　itfFileAgent の形式に従う


===hmrSpriteFile===
v1_01/140320 hmIto
	名前を付けれるように修正
v1_00/130907 amby
	作成開始

*/

#include <string>
#include <fstream>
#include <boost/signals2.hpp>
#include <boost/filesystem.hpp>
#include <hmLib_v3_06/signals.hpp>
#include "hmrItfFile.hpp"
#include "hmrData.hpp"
#include "coordinates.hpp"

namespace hmr{
	// それぞれのjpegファイルを作ることの責任クラス
	class cEachSpriteFileAgent:public itfFileAgent{
	private:
		//std::string data;
		//clock::time_point time;
		std::ofstream ofs;
		
	public:
		bool is_active()const{return ofs.is_open();}
		// log 関数
		void log_writeData(clock::time_point time_, std::string& data_){
			// 開いていなければ・・・・
			if(!is_active()) return;

			// File に出力
			ofs << data_;

			//ofs << hmr::time_to_hms(time_) ;
			//ofs << FILE_COMMENTOUT_CHARS << pos_.x << FILE_COMMENTOUT_CHARS << pos_.y << FILE_COMMENTOUT_CHARS << pos_.z;
			//ofs << 	FILE_COMMENTOUT_CHARS << por_.theta << FILE_COMMENTOUT_CHARS << por_.phi;
			//ofs << std::endl;	
			return;	
		}

	public:
		cEachSpriteFileAgent(){}
		void activate(const std::string& Path_){
			if(!is_active()){
				ofs.open(Path_, std::ios::out | std::ios::binary | std::ios::trunc);
				//ofs << FILE_COMMENTOUT_CHARS << " This is the Accele data log file" << std::endl;
				//ofs << FILE_COMMENTOUT_CHARS << " Time" << FILE_DEVIDE_CHARS << "Position[x, y, z]" << FILE_DEVIDE_CHARS << "Polar[theta, phi]" << std::endl;
			}
		}
		void inactivate(){
			if(is_active()){
				//ofs << FILE_COMMENTOUT_CHARS << "EOF" << std::endl;
				ofs.close();
			}
		}
	};

	// Jpegファイルを一括して保存することの責任クラス
	//  activate の際には PATH を指定すること
	class cSpriteFileAgent:public itfFileAgent{
	private:

		bool activeFlag;
		std::string filePath;
		std::string Name;
		
		hmLib::signals::unique_connections SignalConnections;
		cEachSpriteFileAgent eachSpriteFA;


	public:
		cSpriteFileAgent(const std::string& Name_) :eachSpriteFA(), activeFlag(false), Name(Name_){
		}

		bool is_active()const{return activeFlag;}

	private:
		// log 関数
		void log_writeData(clock::time_point time_, std::string data_){
			// 開いていなければ・・・・
			if(!is_active()) return;

			// File に出力
			if(!eachSpriteFA.is_active()){
				// 時間を取得
				hmr::date Date;
				Date.from_time(time_);
				std::string str1 = (boost::format("%02d%02d%02d_%02d%02d%02d_%03d")%(Date.Year%100)%Date.Month%Date.Day%Date.Hour%Date.Min%Date.Sec%Date.mSec).str();

				// この時間でファイル名を設定
				std::string str = filePath + str1 + ".jpg";
				eachSpriteFA.activate(str);
			
				// データをログ
				eachSpriteFA.log_writeData(time_, data_);

				// inactivate
				eachSpriteFA.inactivate();
			}
			return;	
		}


	public:
		void activate(const std::string& Path_){
			if(!is_active()){
				activeFlag = true;
				filePath = Path_ + Name + "\\";
				boost::filesystem::create_directory(boost::filesystem::current_path()/(Path_ + Name));
			}
		}
		void inactivate(){
			if(is_active()){
				activeFlag = false;
			}
		}
	
	// Log 用の signal slot
	public:
		// Logとして書き込む時のslot
		void slot_log_writeData(boost::signals2::signal<void(const std::string&, clock::time_point)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](const std::string& data_, clock::time_point time_)->void{this->log_writeData(time_, data_);}));	
		}
	
	};


}
#endif









