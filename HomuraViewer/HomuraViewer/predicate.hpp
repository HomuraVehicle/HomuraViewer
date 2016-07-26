#ifndef HMLIB_PREDICATE_INC
#define HMLIB_PREDICATE_INC 100
#
namespace hmLib{
	template<typename t>
	class is_predicate_not{
		t T;
	public:
		is_predicate_not(t&& T_):T(std::move(T_)){}
		bool operator()(void)const{
			return !T();
		}
	};
	template<typename t>
	is_predicate_not<t> predicate_not(t&& T_){return is_predicate_not<t>(std::move(T_));}		
	template<typename t1,typename t2>
	class is_predicate_or{
		t1 T1;
		t2 T2;
	public:
		is_predicate_or(t1&& T1_,t2&& T2_):T1(T1_),T2(T2_){}
		bool operator()(void)const{
			return T1()||T2();
		}
	};
	template<typename t1,typename t2>
	is_predicate_or<t1,t2> predicate_or(t1&& T1_,t2&& T2_){return is_predicate_or<t1,t2>(std::move(T1_),std::move(T2_));}		
	template<typename t1,typename t2>
	class is_predicate_and{
		t1 T1;
		t2 T2;
	public:
		is_predicate_and(t1&& T1_,t2&& T2_):T1(T1_),T2(T2_){}
		bool operator()(void)const{
			return T1()&&T2();
		}
	};
	template<typename t1,typename t2>
	is_predicate_and<t1,t2> predicate_and(t1&& T1_,t2&& T2_){return is_predicate_and<t1,t2>(std::move(T1_),std::move(T2_));}
}
#
#endif
