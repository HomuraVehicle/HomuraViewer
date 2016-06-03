#ifndef HMR_CSVFILETOOL_INC
#define HMR_CSVFILETOOL_INC 0
#
/*===hmrCSVFileTool===
hmrCSVFileTool v0_00/131026 iwahori
	便利クラス群
*/
#include "hmLibVer.hpp"
#include <hmLib/inquiries.hpp>
#include "date.hpp"
#include "hmrCSVFile.hpp"
#include <boost/lexical_cast.hpp>
#include <string>
#include "hmrData.hpp"

namespace hmr{
	//signalされたデータを文字列にして一時保管するcontactクラス、上書き防止通知機能付き
	struct signaled_readcheck_str_contact{
		typedef signaled_readcheck_str_contact my_type;
	private:
		std::string Str;
		bool Read;
	public:
		boost::signals2::signal<void(void)> Signal;
	public:
		signaled_readcheck_str_contact():Read(true){}
	private:
		signaled_readcheck_str_contact(const my_type& My_);
		signaled_readcheck_str_contact(my_type&& My_);
		const my_type& operator=(const my_type& My_);
		const my_type& operator=(my_type&& My_);
	private:
		hmLib::signals::unique_connections SignalConnections;
		hmLib::inquiries::unique_connections InquiryConnections;
	public:
		template<typename T>
		void slot(boost::signals2::signal<void(const T)>& Signal_){
			SignalConnections(signals::connect(Signal_,[&](const T val_)->void{
				if(this->Read==false)Signal();
				this->Str=boost::lexical_cast<std::string>(val_);
				this->Read=false;
			}));
		}
		void conatct(hmLib::inquiries::inquiry<const std::string&>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->const std::string&{
				if(this->Read)this->Read=true;
				else this->Str="";
				return this->Str;
			}));
		}
	public:
		static void regist(cCSVFileAgent& Agent_,const std::string& Label_,signaled_readcheck_str_contact& Signaled_){
			Signaled_.conatct(Agent_.regist(Label_));
			Agent_.slot_write(Signaled_.Signal);
		}
	};
	struct string_date{
		typedef std::chrono::system_clock::time_point time_point;
	private:
		std::string stringDate;
		hmLib::inquiries::unique_connections InquiryConnections;
		const std::string& getDate(){
			hmLib::date Date;
			Date.from_time(clock::now());
			stringDate.erase();
			stringDate+=boost::lexical_cast<std::string>(Date.Year);
			stringDate+=".";
			stringDate+=boost::lexical_cast<std::string>(Date.Month);
			stringDate+=".";
			stringDate+=boost::lexical_cast<std::string>(Date.Day);
			stringDate+=".";
			stringDate+=boost::lexical_cast<std::string>(Date.Hour);
			stringDate+=".";
			stringDate+=boost::lexical_cast<std::string>(Date.Min);
			stringDate+=".";
			stringDate+=boost::lexical_cast<std::string>(Date.Sec);
			stringDate+=".";
			stringDate+=boost::lexical_cast<std::string>(Date.mSec);
			return stringDate;
		}
	public:
		string_date():stringDate(){}
		void contact_getTime(hmLib::inquiries::inquiry<const std::string&>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->const std::string&{return this->getDate();}));
		}
	};
	struct string_counter{
	private:
		unsigned int Count;
		hmLib::inquiries::unique_connections InquiryConnections;
		std::string getCount(){return boost::lexical_cast<std::string>(Count++);}
	public:
		string_counter():Count(0){}
		void contact_getCount(hmLib::inquiries::inquiry<const std::string&>& Inquiry_){
			InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->const std::string{return this->getCount();}));
		}
		void reset(unsigned int Count_=0){Count=Count_;}
	};
}

#
#endif