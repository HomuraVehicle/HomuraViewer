
#ifndef HMRVLIB_UNISENSORFILEAGENT_INC
#define HMRVLIB_UNISENSORFILEAGENT_INC 100
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
		namespace file{
			/*
			以下の三関数が実装されているdata_t型クラスで利用可能
			data_t::time()
			data_t::raw_value()
			data_t::value()
			signalで受け取ると、date, time, rawvalue, valueの順でchar separated 形式で出力する
			*/
			template<typename data_t>
			class cUniSensorFileAgent :public itfFileAgent{
			private:
				std::ofstream ofs;
				std::string Name;
				std::string Separator;
				hmLib::signals::unique_connections SignalConnections;
			private:
				// log and write 関数
				void log_writeData(const data_t& data_){
					if(is_active()){
						ofs << time_to_ymd(data_.time()) << Separator
							<< time_to_hms(data_.time()) << Separator
							<< data_.raw_value() << Separator
							<< data_.value() << std::endl;
					}
				}
			public:
				cUniSensorFileAgent(const std::string& Name_, std::string Separator_)
					: Name(Name_)
					, Separator(Separator_){}
				void activate(const std::string& Path_){
					if(!is_active()){
						std::ifstream fin(Path_ + Name);

						//ファイルが存在しない場合
						if(!fin.is_open()){
							ofs.open(Path_ + Name);
							ofs << "Date" << Separator << "Time" << Separator << "RawValue" << Separator << "Value" << std::endl;
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
			public:
				// Log 用の signal slot
				void slot_log_writeData(boost::signals2::signal<void(data_t)>& Signal_){
					SignalConnections(hmLib::signals::connect(Signal_, [&](data_t Data_)->void{this->log_writeData(Data_); }));
				}
			};
		}
	}
}
#
#endif









