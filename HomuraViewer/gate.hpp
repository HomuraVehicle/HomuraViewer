#ifndef HMR_GATE_INC
#define HMR_GATE_INC 105
#
/*===gate===
受信送信を簡易で行うクラス

gate v1_05/130421 hmIto
	exceptionsに対応
gate v1_04/130329 hmIto
	一部streambuf_interfaceの純粋仮想関数が継承されていなかった致命的な問題を修正
gate v1_03/130328 hmIto
	streambuf_interfaceを継承する形に修正
gate:v1_02/130324 hmIto
	iostreamをinclude
	basic_gatestreambufの初期化にポインタではなく参照を渡していた致命的な問題を修正
*/
#include<iostream>
#include<hmLib/exceptions.hpp>
#include<hmLib/streambuf_interface.hpp>
namespace hmr{
	class gate_not_opened_exception:public hmLib::io_exceptions::not_opened{
		typedef hmLib::io_exceptions::not_opened base_type;
	public:
		explicit gate_not_opened_exception(const std::string& Message_):base_type(Message_.c_str()){}
		explicit gate_not_opened_exception(const char* Message_):base_type(Message_){}
	};
	class gate_opened_exception:public hmLib::io_exceptions::opened{
		typedef hmLib::io_exceptions::opened base_type;
	public:
		explicit gate_opened_exception(const std::string& Message_):base_type(Message_.c_str()){}
		explicit gate_opened_exception(const char* Message_):base_type(Message_){}
	};
	template<typename _Elem, typename _Traits=std::char_traits<_Elem> >
	class basic_gate{
	public:
		typedef std::streamsize size_type;
	public://gate
		//受信可能状態かの確認
		virtual bool can_get()=0;
		//受信可能データの有無
		virtual bool empty()=0;
		//複数byte受信　受信文字アドレスと、受信文字数が引数　実際の受信文字数が戻り値
		virtual size_type get(_Elem* str_,const size_type& size_)=0;
		//送信可能状態かの確認
		virtual bool can_put()=0;
		//送信可能データの有無
		virtual bool full()=0;
		//複数byte送信　送信文字アドレスと、送信文字数が引数　実際の送信文字数が戻り値
		virtual size_type put(const _Elem* str_,const size_type& size_)=0;
	};
	template<typename _Elem, typename _Traits=std::char_traits<_Elem> >
	class basic_gatestreambuf:public basic_streambuf_interface<_Elem,_Traits>{
	private:
		typedef basic_gate<_Elem,_Traits> my_gate;
	protected:
		my_gate* gate;
		_Elem buf;
		bool bufflag;
	public://標準関数
		basic_gatestreambuf():gate(/*C++0x_nullptr/*/NULL/**/),bufflag(false){return;}
		basic_gatestreambuf(my_gate* gate_):gate(gate_),bufflag(false){return;}
		~basic_gatestreambuf(){close();}
		bool is_open()const{return gate!=/*C++0x_nullptr/*/NULL/**/;}
		bool open(my_gate* gate_){
			close();
			gate=gate_;
			return false;
		}
		bool close(){
			gate=/*C++0x_nullptr/*/NULL/**/;
			bufflag=false;
			return false;
		}
	protected://common virtual functions
		pos_type iobuf_seekpos(pos_type abspos,std::ios_base::openmode which=std::ios_base::in|std::ios_base::out)override{
			hmLib_throw(io_exceptions::io_exception,"basic_gatestreambuf can't do seek");
			return 0;
		}
		pos_type iobuf_seekoff(off_type relpos,std::ios_base::seekdir dir,std::ios_base::openmode which=std::ios_base::in|std::ios_base::out)override{
			hmLib_throw(io_exceptions::io_exception,"basic_gatestreambuf can't do seek");
			return 0;
		}
	protected://streambuf_interface ibuf
		_Elem ibuf_overflow()override{
			if(bufflag){
				bufflag=false;
				return buf;
			}

			_Elem c;
			typename my_gate::size_type size=1;
			gate->get(&c,size);
			return c;
		}
		streamsize ibuf_gets(_Elem* str,streamsize size)override{
			if(bufflag){
				bufflag=0;
				str[0]=buf;
				return static_cast<streamsize>(gate->get(str+1,size-1));
			}
			return static_cast<streamsize>(gate->get(str,size));
		}
		bool ibuf_unget(_Elem c)override{
			hmLib_assert(!bufflag,io_exceptions::io_exception,"unget cannot use two times.");
			buf=c;
			bufflag=true;
			return false;
		}
		_Elem ibuf_peek()override{
			_Elem ans=ibuf_overflow();
			ibuf_unget(ans);
			return ans;
		}
	protected://streambuf_interface obuf
		bool obuf_overflow(_Elem c)override{
			typename my_gate::size_type size=1;
			gate->put(&c,size);
			return false;
		}
		streamsize obuf_puts(const _Elem* str,streamsize size)override{
			gate->put(str,size);
			return size;
		}
		bool obuf_flush()override{return false;}
	public://gate functions
		bool fail(){return (gate->can_get() || gate->can_put());}
		bool pfail(){return gate->can_put();}
		bool gfail(){return gate->can_get();}
		bool empty(){return gate->empty();}
		bool full(){return gate->full();}
	};
	template<typename _Elem, typename _Traits=std::char_traits<_Elem> >
	class basic_igatestream:public std::basic_istream<_Elem,_Traits>{
	private:
		typedef basic_gate<_Elem,_Traits> my_gate;
		typedef basic_gatestreambuf<_Elem,_Traits> my_gatestreambuf;
	private:
		my_gatestreambuf* streambuf;
	public:
		basic_igatestream():std::basic_istream<_Elem,_Traits>(streambuf=new my_gatestreambuf()){return;}
		basic_igatestream(my_gate& gate_):std::basic_istream<_Elem,_Traits>(streambuf=new my_gatestreambuf(&gate_)){return;}
		~basic_igatestream(){
			close();
			delete streambuf;
		}
	public:
		void open(my_gate& gate_){streambuf->open(&gate_);}
		void close(){streambuf->close();}
		bool is_open(){return streambuf->is_open();}
		bool gfail(){return streambuf->gfail();}
		bool empty(){return streambuf->empty();}
		bool eof(){return empty();}
	};
	template<typename _Elem, typename _Traits=std::char_traits<_Elem> >
	class basic_ogatestream:public std::basic_ostream<_Elem,_Traits>{
	private:
		typedef basic_gate<_Elem,_Traits> my_gate;
		typedef basic_gatestreambuf<_Elem,_Traits> my_gatestreambuf;
	private:
		my_gatestreambuf* streambuf;
	public:
		basic_ogatestream():std::basic_ostream<_Elem,_Traits>(streambuf=new my_gatestreambuf()){return;}
		basic_ogatestream(my_gate& gate_):std::basic_ostream<_Elem,_Traits>(streambuf=new my_gatestreambuf(&gate_)){return;}
		~basic_ogatestream(){
			close();
			delete streambuf;
		}
		void open(my_gate& gate_){streambuf->open(&gate_);}
		void close(){streambuf->close();}
		bool is_open(){return streambuf->is_open();}
	public:
		bool pfail(){return streambuf->pfail();}
		bool full(){return streambuf->full();}
	};
	template<typename _Elem, typename _Traits=std::char_traits<_Elem> >
#pragma warning(push)
#pragma warning(disable:4250)
	class basic_gatestream:public std::basic_iostream<_Elem,_Traits>{
	private:
		typedef basic_gate<_Elem,_Traits> my_gate;
		typedef basic_gatestreambuf<_Elem,_Traits> my_gatestreambuf;
	private:
		my_gatestreambuf* streambuf;
	public:
		basic_gatestream():std::basic_iostream<_Elem,_Traits>(streambuf=new my_gatestreambuf()){return;}
		basic_gatestream(my_gate& gate_):std::basic_iostream<_Elem,_Traits>(streambuf=new my_gatestreambuf(&gate_)){return;}
		~basic_gatestream(){
			close();
			delete streambuf;
		}
		void open(my_gate& gate_){streambuf->open(&gate_);}
		void close(){streambuf->close();}
		bool is_open(){return streambuf->is_open();}
	public:
		bool pfail(){return streambuf->pfail();}
		bool full(){return streambuf->full();}
		bool gfail(){return streambuf->gfail();}
		bool empty(){return streambuf->empty();}
		bool eof(){return empty();}
	};
#pragma warning(pop)
	typedef basic_gate<char,std::char_traits<char> > gate;
	typedef basic_gatestreambuf<char,std::char_traits<char> > gatestreambuf;
	typedef basic_igatestream<char,std::char_traits<char> > igatestream;
	typedef basic_ogatestream<char,std::char_traits<char> > ogatestream;
	typedef basic_gatestream<char,std::char_traits<char> > gatestream;
}
#
#endif
