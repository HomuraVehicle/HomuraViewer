#ifndef HMR_VIEWER_FULLADC_FILEAGENT_INC
#define HMR_VIEWER_FULLADC_FILEAGENT_INC 100
#
#include <fstream>
#include <string>
#include <boost/signals2.hpp>
#include <hmLib/signals.hpp>
#include <HomuraViewer/File/ItfFile.hpp>
#include <HomuraViewer/chrono.hpp>
#include "Data.hpp"
namespace hmr{
	namespace viewer{
		namespace fulladc{
			template<unsigned int adc_num_>
			class cFileAgent :public itfFileAgent{
				constexpr static unsigned int adc_num = adc_num_;
				using this_data_t = data_t<adc_num>;
			private:
				std::ofstream ofs;
				hmLib::signals::unique_connections SignalConnections;
				std::string Name;
				char Separator;
			private:
				// log and write 関数
				void logData(this_data_t Data_){
					if(is_active()){
						ofs << time_to_ymd(Data_.Time);
						for(unsigned int adc_ch = 0; adc_ch < adc_num; ++adc_ch){
							ofs << Separator << Data_.Data[adc_ch];
						}
						ofs << std::endl;
					}
				}
			public:
				cFileAgent(const std::string& Name_, char Separator_)
					: Name(Name_)
					, Separator(Separator_){
				}
				void activate(const std::string& Path_){
					if(!is_active()){
						std::ifstream fin(Path_ + Name);

						//ファイルが存在しない場合
						if(!fin.is_open()){
							ofs.open(Path_ + Name);
							ofs << "Time";
							for(unsigned int adc_ch = 0; adc_ch < adc_num; ++adc_ch){
								ofs << Separator << "ADC" << adc_ch;
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
				void slot_logData(boost::signals2::signal<void(this_data_t)>& Signal_){
					SignalConnections(hmLib::signals::connect(Signal_, [&](this_data_t Data_)->void{this->logData(Data_); }));
				}
			};
		}
	}
}
#
#endif
