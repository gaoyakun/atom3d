// 函数指针的容器(全局函数,静态函数,成员函数,仿函数)
// dashi

/************************************************************************
	2005.12.22.
	1.改变了存储方式,用引用计数的办法去存储,因为函数指针无论拷贝多少份,他的内容还是一样的
	2.扩张了调用的方法
		int DoFun(string);
		Function<int, string> f(DoFun);
		1. return f.get()->invoke("效率高,不过麻烦");
		2. return f->invoke("效率高, 稍微有点麻烦, 推荐");
		3. return f("效率相对要低一些,不过方便");
	3.支持的参数由原来的20个下降到3个,最主要还要加些静态检测(判断输入的类型是否匹配),所以先这样用一下
************************************************************************/
#ifndef FUNCTION_H
#define FUNCTION_H

#include "FuncPtr.h"

HEEZE_NAMESPACE_BEGIN

#define HEEZE_MAKE_TYPE_LIST_20 typename heeze::make_type_list<_T1, _T2, _T3, _T4, _T5, _T6, _T7, _T8, _T9, _T10, _T11, _T12, _T13, _T14, _T15, _T16, _T17, _T18, _T19, _T20>::type

template
<	
	typename _TRtn
,	typename _T1  = heeze::null_type, typename _T2  = heeze::null_type, typename _T3  = heeze::null_type
,	typename _T4  = heeze::null_type, typename _T5  = heeze::null_type, typename _T6  = heeze::null_type
,	typename _T7  = heeze::null_type, typename _T8  = heeze::null_type, typename _T9  = heeze::null_type
,	typename _T10 = heeze::null_type, typename _T11 = heeze::null_type, typename _T12 = heeze::null_type
,	typename _T13 = heeze::null_type, typename _T14 = heeze::null_type, typename _T15 = heeze::null_type
,	typename _T16 = heeze::null_type, typename _T17 = heeze::null_type, typename _T18 = heeze::null_type 
,	typename _T19 = heeze::null_type, typename _T20 = heeze::null_type
>
struct Function : public type_select
	< 
		is_void<_TRtn>::value
	,	typename FuncPtrHelper<type_length<HEEZE_MAKE_TYPE_LIST_20>::value, _TRtn, HEEZE_MAKE_TYPE_LIST_20>::VoidFuncBase
	,	typename FuncPtrHelper<type_length<HEEZE_MAKE_TYPE_LIST_20>::value, _TRtn, HEEZE_MAKE_TYPE_LIST_20>::FuncBase
	>::type 
{
	typedef typename HEEZE_MAKE_TYPE_LIST_20 ParamList;		// type list

	enum{ PARAM_LENTH = type_length< ParamList >::value };	// type lenth of type list
	enum{ RTN_IS_VOID = is_void<_TRtn>::value };			// the return type is void

	typedef typename FuncPtrHelper<PARAM_LENTH, _TRtn, ParamList> FuncPtrHelper;
	typedef typename FuncPtrHelper::IFuncPtr IFuncPtr;

	typedef typename type_select< RTN_IS_VOID, typename FuncPtrHelper::VoidFuncBase, typename FuncPtrHelper::FuncBase>::type FuncBase;

	//////////////////////////////////////////////////////////////////////////
	// construction/ destruction
	inline Function()
		: FuncBase(NULL){		
	}

	// member fuction
	template<typename _TCls, typename _TFunc>
		inline Function(_TCls& obj, _TFunc func)
		: FuncBase( new heeze::type_select
			<
				RTN_IS_VOID
			,	FuncPtrHelper::VoidFuncPtr<_TCls, _TFunc>
			,	FuncPtrHelper::FuncPtr<_TCls, _TFunc> 
			>
			::type(&obj, func) ){		
		// TODO:: check _TCls is Class and not pointer, _TFunc is member function
	}
	// functor
	template< typename _TFunc >
		inline Function( _TFunc func )
		: FuncBase( new type_select
			<
				RTN_IS_VOID
			,	FuncPtrHelper::VoidFunctorPtr<_TFunc>
			,	FuncPtrHelper::FunctorPtr<_TFunc> 
			>
			::type(func) ){
	}

	inline Function(const IFuncPtr* pFunc)
		: FuncBase(pFunc){
		if( _pFunc )
			_pFunc->aquire();
	}

	inline ~Function(){
		if( _pFunc)
			_pFunc->release();
	}

	//////////////////////////////////////////////////////////////////////////
	// copy function
	Function( const Function& ref )
		: FuncBase(ref._pFunc){
		if( ref._pFunc )
			ref._pFunc->aquire();
	}

	Function& operator=( const Function& ref ){
		if( _pFunc )
			_pFunc->release();
		if( ref._pFunc )
			ref._pFunc->aquire();
		_pFunc = ref._pFunc;
		return *this;
	}	

	Function& operator=(IFuncPtr* pFunc)
	{
		if( _pFunc )
			_pFunc->release();
		if( pFunc->_pFunc )
			pFunc->_pFunc->aquire();
		_pFunc = pFunc;
		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	// 设置
	template<typename _TCls, typename _TFunc>
		void reset(_TCls& obj, _TFunc func) {	
		if( _pFunc )
			_pFunc->release();
		typedef FuncPtrHelper::VoidFuncPtr<_TCls, _TFunc> VoidFuncPtr;
		typedef FuncPtrHelper::FuncPtr<_TCls, _TFunc> FuncPtr;
		typedef type_select<	RTN_IS_VOID, VoidFuncPtr, FuncPtr>::type NewFuncPtr;
		_pFunc = new NewFuncPtr(&obj, func);
	}

	template< typename _TFunc >
		void reset( _TFunc func ){
		if( _pFunc )
			_pFunc->release();
		typedef FuncPtrHelper::VoidFunctorPtr<_TFunc> VoidFunctorPtr;
		typedef FuncPtrHelper::FunctorPtr<_TFunc> FunctorPtr;
		typedef type_select<RTN_IS_VOID, VoidFunctorPtr, FunctorPtr>::type NewFuncPtr;
		_pFunc = new NewFuncPtr(func);
	}

	void reset(const IFuncPtr* pFunc){
		if( _pFunc )
			_pFunc->release();
		if( pFunc )
			pFunc->aquire();
		_pFunc = pFunc;
	}

	void reset(){
		if( _pFunc )
		{
			_pFunc->release();
			_pFunc = NULL;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// 调用FUNCTION PTR
	inline IFuncPtr* operator->() const{
		assert(_pFunc);	// 函数还未被初始化
		return _pFunc;
	}

	inline IFuncPtr* get() const{
		assert(_pFunc);	// 函数还未被初始化
		return _pFunc;
	}

	//////////////////////////////////////////////////////////////////////////
	// 是否可用
	inline bool enable(){
		return (NULL != _pFunc);
	}
};

HEEZE_NAMESPACE_END

#endif	// FUNCTION_H