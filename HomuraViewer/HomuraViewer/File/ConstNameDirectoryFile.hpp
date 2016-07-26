#ifndef HMRVLIB_CONSTNAMEDIRECTORYFILE_INC
#define HMRVLIB_CONSTNAMEDIRECTORYFILE_INC 100
#

/*===hmrWholeFile===
ほむらの毎回のデータファイル関係のクラス
ここで、それぞれのファイルをまとめてディレクトリにぶち込む

こいつはディレクトリという意味でまた、ファイルシステムを継承している

hmrWholeFile v1_00/130914 amby
作成開始
*/

#include <string>
#include <fstream>
#include <boost/signals2.hpp>
#include <boost/filesystem.hpp>
#include <hmLib/signals.hpp>
#include "ItfFile.hpp"


namespace hmr{
	class cConstNameDirectoryFile :public itfFile{
		//typedef std::pair<itfFileAgent*, std::string> FileAgentSet;
		typedef std::vector<itfFileAgent*> FileAgentBuf;

	private:
		std::ofstream ofs;
		hmLib::signals::unique_connections SignalConnections;

		FileAgentBuf fAgents;
		bool activateFlag;
		std::string Name;

	public:
		// signal 系列の定義
		boost::signals2::signal<void(bool)> signal_isActivate;

		cConstNameDirectoryFile(std::string Name_)
			: Name(Name_)
			, activateFlag(false){
		}

		void regist(itfFileAgent* file_){
			fAgents.push_back(file_);
			return;
		}

		void activate(const std::string& Path_){
			// Passを指定してしまう
			std::string FullPath = Path_ + Name;

			if(!boost::filesystem::exists(boost::filesystem::current_path() / FullPath)){
				boost::filesystem::create_directory(boost::filesystem::current_path() / FullPath);
			}
			
			if(!is_active()){
				for(auto itr = fAgents.begin(); itr != fAgents.end(); ++itr){
					(*itr)->activate(FullPath + "\\");
				}

				activateFlag = true;
				signal_isActivate(true);
			}
		}

		bool is_active() const{ return activateFlag; }
		void inactivate(){
			if(is_active()){
				for(auto itr = fAgents.begin(); itr != fAgents.end(); ++itr){
					(*itr)->inactivate();
				}
				activateFlag = false;
				signal_isActivate(false);
			}
		}

		// activate用, inactivate用 の signal slot
	public:
		void slot_activate(boost::signals2::signal<void(const std::string&)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_, [&](const std::string& path_)->void{this->activate(path_); }));
		}
		void slot_inactivate(boost::signals2::signal<void()>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_, [&]()->void{this->inactivate(); }));
		}
	};
}
#endif

#pragma once
