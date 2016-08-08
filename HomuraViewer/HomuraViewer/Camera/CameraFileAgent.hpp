
#ifndef HMR_VIEWER_CAMERA_CAMERAFILEAGENT_INC
#define HMR_VIEWER_CAMERA_CAMERAFILEAGENT_INC 101
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
#include <hmLib/signals.hpp>
#include <HomuraViewer/File.hpp>
#include <HomuraViewer/chrono.hpp>
#include <hmLib/coordinates.hpp>

namespace hmr{
	namespace viewer{
		namespace camera{
			// Jpegファイルを一括して保存することの責任クラス
			//  activate の際には PATH を指定すること
			class cFileAgent :public itfFileAgent{
			private:
				bool activeFlag;
				std::string filePath;
				std::string Name;
				std::ofstream fout;

				hmLib::signals::unique_connections SignalConnections;
			public:
				cFileAgent(const std::string& Name_) :activeFlag(false), Name(Name_){}

				bool is_active()const{ return activeFlag; }

			private:
				// log 関数
				void log_writeData(clock::time_point time_, std::string data_){
					// 開いていなければ・・・・
					if(!is_active()) return;

					// 時間を取得
					date Date;
					Date.from_time(time_);
					std::string str1 = (boost::format("%02d%02d%02d_%02d%02d%02d_%03d") % (Date.Year % 100) % Date.Month%Date.Day%Date.Hour%Date.Min%Date.Sec%Date.mSec).str();

					// この時間でファイル名を設定
					std::string str = filePath + str1 + ".jpg";

					// データをログ
					fout.open(str);
					fout << data_;
					fout.close();
				}
			public:
				void activate(const std::string& Path_){
					if(!is_active()){
						activeFlag = true;
						filePath = Path_ + Name + "\\";

						if(!boost::filesystem::exists(boost::filesystem::current_path() / (Path_ + Name))){
							boost::filesystem::create_directory(boost::filesystem::current_path() / (Path_ + Name));
						}
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
					SignalConnections(hmLib::signals::connect(Signal_, [&](const std::string& data_, clock::time_point time_)->void{this->log_writeData(time_, data_); }));
				}

			};
		}
	}

}
#endif









