#ifndef HMR_DXKEYBOARD_INC
#define HMR_DXKEYBOARD_INC 101
#
#include<map>

#include<boost/signals2.hpp>
#include<hmLib/inquiries.hpp>
#include"hmLibVer.hpp"
#include<hmLib_v2/dxBasic.hpp>
#include"any_predicate.hpp"
namespace hmr{
	namespace viewer{
		class is_key_pushed{
		private:
			KEY::KEYNAME KeyName;
		public:
			is_key_pushed(KEY::KEYNAME KeyName_) :KeyName(KeyName_){}
			bool operator()(void)const{ return dx::getKey(KeyName) == 1; }
		};
		class have_key_pushed{
		private:
			KEY::KEYNAME KeyName;
		public:
			have_key_pushed(KEY::KEYNAME KeyName_) :KeyName(KeyName_){}
			bool operator()(void)const{ return dx::getKey(KeyName) > 0; }
		};
		class is_key_pulled{
		private:
			KEY::KEYNAME KeyName;
		public:
			is_key_pulled(KEY::KEYNAME KeyName_) :KeyName(KeyName_){}
			bool operator()(void)const{ return dx::getKey(KeyName) == -1; }
		};

		class cDxKeyboard{
			typedef boost::signals2::signal<void(void)> signal_t;
		private:
			std::map<hmLib::any_predicate, std::unique_ptr<signal_t>> SignalMap;
			hmLib::inquiries::unique_connections InquiryConnections;
		public:
			void work(){
				unsigned int Cnt = 0;
				for(auto itr = std::begin(SignalMap); itr != std::end(SignalMap); ++itr){
					if((itr->first)()){
						(*(itr->second))();
					}
					++Cnt;
				}
			}
			signal_t& signal(hmLib::any_predicate&& AnyPredicate_){
				auto itr = SignalMap.find(AnyPredicate_);
				if(itr != SignalMap.end())return *(itr->second);
				std::unique_ptr<signal_t> Ptr(new signal_t());
				return *(SignalMap[std::move(AnyPredicate_)] = std::move(Ptr));
			}
			void contact(KEY::KEYNAME Key_, hmLib::inquiries::inquiry<int> Inquiry_){
				InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->int{return dx::getKey(Key_); }));
			}
		};
	}
}
#
#endif
