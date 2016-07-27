#ifndef HMRVLIB_CONSTNAMEFILEAGENT_INC
#define HMRVLIB_CONSTNAMEFILEAGENT_INC 100
#
#include <string>
#include <utility>
#include <fstream>
#include <boost/signals2.hpp>
#include <hmLib/signals.hpp>
#include <HomuraViewer/chrono.hpp>
#include "ItfFile.hpp"

namespace hmr{
	namespace viewer{
		template<typename T>
		class cConstNameFileAgent :public itfFileAgent{
		private:
			std::ofstream ofs;
			hmLib::signals::unique_connections SignalConnections;
			std::string Name;
			char Separator;
		private:
			// log and write 関数
			void log_writeData(clock::time_point time_, double data_){
				if(is_active()){
					ofs << time_to_ymd(time_) << Separator << time_to_hms(time_) << Separator << data_ << std::endl;
				}
			}

		public:
			cConstNameFileAgent(const std::string& Name_, char Separator_)
				: Name(Name_)
				, Separator(Separator_){}

			void activate(const std::string& Path_){
				if(!is_active()){
					std::ifstream fin(Path_ + Name);

					//ファイルが存在しない場合
					if(!fin.is_open()){
						ofs.open(Path_ + Name);
						ofs << "Time" << Separator << "Data" << std::endl;
					} else{
						fin.close();
						ofs.open(Path_ + Name, std::ios_base::out | std::ios_base::app);
					}
				}
			}
			bool is_active() const{ return ofs.is_open(); }
			void inactivate(){
				if(is_active()){
					ofs.close();
				}
			}

			// Log 用の signal slot
		public:
			void slot_log_writeData(boost::signals2::signal<void(double, clock::time_point)>& Signal_){
				SignalConnections(hmLib::signals::connect(Signal_, [&](double data_, clock::time_point time_)->void{this->log_writeData(time_, data_); }));
			}
		};
		template<typename T1, typename T2>
		class cConstNameFileAgent< std::pair<T1, T2> > :public itfFileAgent{
			using my_tuple = std::pair<T1, T2>;
		private:
			std::ofstream ofs;
			hmLib::signals::unique_connections SignalConnections;
			std::string Name;
			char Separator;
		private:
			// log and write 関数
			void log_writeData(clock::time_point time_, T1 data1_, T2 data2_){
				if(is_active()){
					ofs << time_to_ymd(time_) << Separator << time_to_hms(time_);
					ofs << Separator << data1_;
					ofs << Separator << data2_;
					ofs << std::endl;
				}
			}
		public:
			cConstNameFileAgent(const std::string& Name_, char Separator_)
				: Name(Name_)
				, Separator(Separator_){}

			void activate(const std::string& Path_){
				if(!is_active()){
					std::ifstream fin(Path_ + Name);

					//ファイルが存在しない場合
					if(!fin.is_open()){
						ofs.open(Path_ + Name);
						ofs << "Date" << Separator << "Time" << Separator << "Data1" << Separator << "Data2";
						ofs << std::endl;
					} else{
						fin.close();
						ofs.open(Path_ + Name, std::ios_base::out | std::ios_base::app);
					}
				}
			}
			bool is_active() const{ return ofs.is_open(); }
			void inactivate(){
				if(is_active()){
					ofs.close();
				}
			}

			// Log 用の signal slot
		public:
			void slot_log_writeData(boost::signals2::signal<void(T1, T2, clock::time_point)>& Signal_){
				SignalConnections(hmLib::signals::connect(Signal_, [&](T1 data1_, T2 data2_, clock::time_point time_)->void{this->log_writeData(time_, data1_, data2_); }));
			}
		};
		template<typename T, typename... others>
		class cConstNameFileAgent< std::tuple<T, others...> > :public itfFileAgent{
			using my_tuple = std::tuple<T, others...>;
		private:
			std::ofstream ofs;
			hmLib::signals::unique_connections SignalConnections;
			std::string Name;
			char Separator;
		private:
			template<unsigned int pos>
			void write_tuple_item(const my_tuple& data_){
				if(pos >= )return;
				ofs << Separator << std::get<pos>(data_);
				write_tuple_item<pos + 1>(data_);
			}
			template<>
			void write_tuple_item<std::tuple_size<my_tuple>::value>(const my_tuple& data_){}
			// log and write 関数
			void log_writeData(clock::time_point time_, my_tuple data_){
				if(is_active()){
					ofs << time_to_ymd(time_) << Separator << time_to_hms(time_);
					write_tuple_item(data_, 0);
					ofs << std::endl;
				}
			}

		public:
			cConstNameFileAgent(const std::string& Name_, char Separator_)
				: Name(Name_)
				, Separator(Separator_){}

			void activate(const std::string& Path_){
				if(!is_active()){
					std::ifstream fin(Path_ + Name);

					//ファイルが存在しない場合
					if(!fin.is_open()){
						ofs.open(Path_ + Name);
						ofs << "Date" << Separator << "Time";
						for(unsigned int Cnt = 0; Cnt < std::tuple_size<my_tuple>::value; ++Cnt){
							ofs << Separator << "Data" << Cnt;
						}
						ofs << std::endl;
					} else{
						fin.close();
						ofs.open(Path_ + Name, std::ios_base::out | std::ios_base::app);
					}
				}
			}
			bool is_active() const{ return ofs.is_open(); }
			void inactivate(){
				if(is_active()){
					ofs.close();
				}
			}

			// Log 用の signal slot
		public:
			void slot_log_writeData(boost::signals2::signal<void(my_tuple, clock::time_point)>& Signal_){
				SignalConnections(hmLib::signals::connect(Signal_, [&](my_tuple data_, clock::time_point time_)->void{this->log_writeData(time_, data_); }));
			}
		};
	}
}
#endif
