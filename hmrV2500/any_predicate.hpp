#ifndef HMLIB_ANYPREDICATE_INC
#define HMLIB_ANYPREDICATE_INC
#
/*===any_predicate===
叙述関数(boolを返す関数オブジェクト）ならなんでも受け取れるクラス
コピー不可、ムーブ可
*/
#include<memory>
//#include <typeinfo>
namespace hmLib{
	class any_predicate{
		typedef any_predicate my_type;
	public:
		class basic_predicate_adaptor{
		public:
			virtual bool operator()(void)const=0;
//			virtual int type_hash()const=0;
		};
		template<typename predicate_>
		class predicate_adaptor:public basic_predicate_adaptor{
			predicate_ Predicate;
//			static int TypeID;
		public:
			predicate_adaptor(predicate_ Predicate_):Predicate(Predicate_){}
			virtual bool operator()(void)const override{return Predicate();}
//			virtual int type_hash()const override{return TypeID;}
		};
	private:
		std::unique_ptr<basic_predicate_adaptor> pPredicateAdaptor;
	private:
		any_predicate(const my_type&);
	public:
		any_predicate(my_type&& My_):pPredicateAdaptor(std::move(My_.pPredicateAdaptor)){}
		template<typename predicate_> 
		any_predicate(predicate_ Predicate)
			:pPredicateAdaptor(static_cast<basic_predicate_adaptor*>(new predicate_adaptor<predicate_>(std::move(Predicate)))){
		}
		bool operator()(void)const{return (*pPredicateAdaptor)();}
		bool operator<(const my_type& My_)const{return pPredicateAdaptor.get()<My_.pPredicateAdaptor.get();}
//		bool operator<(const my_type& My_)const{return pPredicateAdaptor->type_hash()<My_.pPredicateAdaptor->type_hash();}
	};
//	template<typename predicate_>
//	int any_predicate::predicate_adaptor<predicate_>::TypeID=typeid(predicate_).hash_code();
}
#
#endif
