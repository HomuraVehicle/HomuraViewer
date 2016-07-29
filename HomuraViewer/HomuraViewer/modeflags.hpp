#ifndef HMR_MODEFLAGS_INC
#define HMR_MODEFLAGS_INC 100
#
#include "hmLibVer.hpp"
#include <cstdint>
#include <functional>
#include <boost/signals2.hpp>
#include <hmLib/signals.hpp>
#include <hmLib/inquiries.hpp>
namespace hmr{
	namespace viewer{
		struct modeflags{
		private:
			bool Pic;
			bool Req;
		public:
			modeflags():Pic(false),Req(false){}
			operator bool()const{ return Pic != Req; }
			bool get_req()const{ return Req; }
			void set_pic(bool Pic_){ Pic = Pic_; }
		public:
			hmLib::inquiries::connection contact_getPic(hmLib::inquiries::inquiry<bool>& Inquiry_){
				return hmLib::inquiries::connect(Inquiry_, [&](void)->bool{return this->Pic; });
			}
			hmLib::inquiries::connection contact_getReq(hmLib::inquiries::inquiry<bool>& Inquiry_){
				return hmLib::inquiries::connect(Inquiry_, [&](void)->bool{return this->Req; });
			}
			boost::signals2::connection slot_setReq(boost::signals2::signal<void(bool)>& Signal_){
				return hmLib::signals::connect(Signal_, [&](bool Flag)->void{Req = Flag; });
			}
		};
	}
}
#
#endif
