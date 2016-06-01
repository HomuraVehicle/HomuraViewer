#ifndef HMR_FLAGIRL_INC
#define HMR_FLAGIRL_INC
#
namespace hmr{
	class flagirl{
		bool real;
		bool ideal;
		bool lack;
	public:
		flagirl():real(false),ideal(false),lack(false){}
	public:
		void set_pic(bool mode_){real=mode_;}
		void set_request(bool mode_){ideal=mode_;}
		bool pic(void)const{return real;}
		bool request(void)const{return ideal;}
		void setup_talk(void){lack=(real!=ideal);}
		bool talk(void){bool tmp=lack;lack=false;return tmp;}
		bool check_talk(void)const{return lack;}
	};
}
#
#endif
