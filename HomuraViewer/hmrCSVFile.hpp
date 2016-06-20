#ifndef HMR_CSVFILE_INC
#define HMR_CSVFILE_INC 100
#
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <boost/signals2.hpp>
#include <hmLib/signals.hpp>
#include <hmLib/inquiries.hpp>
#include <hmrVLib/File.hpp>
#include <hmrVLib/Data.hpp>
namespace hmr{
	//CSV形式クラス
	class cCSVFileAgent:public itfFileAgent{
		typedef hmLib::inquiries::inquiry<std::string> inquiry;
		typedef std::pair<std::string, inquiry> pair;
		typedef std::vector<pair> containor;
	public:
		template<class T>
		class cCell{
		private:
			std::string Name;
			void regist_ownself(cCSVFileAgent& File_)const{
				File_.Cells.push_back(pair());
				File_.Cells.back().first = Name;
				hmLib::inquiries::connect(File_.Cells.back().second, [&](void)->std::string{return Inquiry.is_connect()?boost::lexical_cast<std::string>(Inquiry()):""; });
			}
		public:
			cCell(const std::string& Name_) :Name(Name_){}
			hmLib::inquiries::inquiry<T> Inquiry;
			friend class cCSVFileAgent;
		};
		
		template<>
		class cCell<std::string>{
		private:
			std::string Name;
			void regist_ownself(cCSVFileAgent& File_)const{
				File_.Cells.push_back(pair());
				File_.Cells.back().first = Name;
				hmLib::inquiries::connect(File_.Cells.back().second, [&](void)->std::string{return Inquiry.is_connect()?Inquiry():"";});
			}
		public:
			cCell(const std::string& Name_) :Name(Name_){}
			hmLib::inquiries::inquiry<std::string> Inquiry;
			friend class cCSVFileAgent;
		};

		template<>
		class cCell<hmr::clock::time_point>{
		private:
			std::string Name;
			void regist_ownself(cCSVFileAgent& File_)const{
				File_.Cells.push_back(pair());
				File_.Cells.back().first = Name;
				hmLib::inquiries::connect(File_.Cells.back().second, [&](void)->std::string{return Inquiry.is_connect() ? hmr::time_to_hms(Inquiry()) : ""; });
			}
		public:
			cCell(const std::string& Name_) :Name(Name_){}
			hmLib::inquiries::inquiry<hmr::clock::time_point> Inquiry;
			friend class cCSVFileAgent;
		};
	private:
		std::ofstream ofs;
		containor Cells;
		std::string Sep;
		std::string Name;
	private:
		hmLib::signals::unique_connections SignalConnections;
	private:
		// write file関数
		void write(void){
			// 開いてなければ・・・・
			if(!is_active()) return;

			// File に書き込む
			bool Begin=true;
			for(auto itr=std::begin(Cells); itr!=std::end(Cells); ++itr){
				if(Begin){
					Begin=false;
				}else ofs << Sep;

				if(!itr->second.is_connect())continue;

				ofs<<itr->second();
			}

			ofs<<std::endl;
		}
	public:
		cCSVFileAgent(const std::string& Name_,const std::string& Sep_=","):Name(Name_),Sep(Sep_){}
		void activate(const std::string& Path_){
			if(!is_active()){
				ofs.open(Path_ + Name+".csv");
				if(!ofs.is_open())return;
				bool Begin=true;
				for(auto itr=std::begin(Cells);itr!=std::end(Cells);++itr){
					if(Begin){
						Begin=false;
					}else ofs << Sep;
					
					ofs<<itr->first;
				}

				ofs<<std::endl;
			}
		}
		bool is_active() const {return ofs.is_open();}
		void inactivate(){if(is_active())ofs.close();}
		//ラベル名を登録すると、アクセス用inquiryがもらえる
		template<class T>
		void regist(const cCell<T>& Cell_){Cell_.regist_ownself(*this);}
		
		// ファイルを保存するためのslot
		void slot_write(boost::signals2::signal<void(void)>& Signal_){
			SignalConnections(hmLib::signals::connect(Signal_,[&](void)->void{this->write();}));	
		}

	};
}
#
#endif
