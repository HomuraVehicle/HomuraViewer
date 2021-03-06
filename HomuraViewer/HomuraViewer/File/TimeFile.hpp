#ifndef HMR_VIEWER_TIMEFILE_INC
#define HMR_VIEWER_TIMEFILE_INC 100
#
#include<boost/format.hpp>
#include<string>
#include <HomuraViewer/date.hpp>
#include <HomuraViewer/chrono.hpp>
#include"hmrItfFile.hpp"
namespace hmr{
	namespace viewer{
		class cTimeFile :public itfFile{
			bool Active;
			std::string Path;
			std::vector<itfFileAgent*> FileAgents;
		public:
			void activate(const std::string& Path_)override{
				if(is_active())return;

				hmLib::date Date;
				Date.from_time(clock::now());

				Path = Path_ + (boost::format("%02d%02d%02d_%02d%02d") % (Date.Year % 100) % Date.Month%Date.Day%Date.Hour%Date.Min).str();

				for(auto itr = std::begin(FileAgents); itr != std::end(FileAgents); ++itr){
					itr->activate(Path);
				}
			}
			bool is_active()const override{ return Active; }
			void inactivate()override{
				if(!is_active())return;

				for(auto itr = std::begin(FileAgents); itr != std::end(FileAgents); ++itr){
					itr->inactivate(Path);
				}
			}
			void regist(itfFileAgent* pAgent_)override{
				FileAgents.push_back(pAgent_);
			}
		};
	}
}
#
#endif
