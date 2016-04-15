/********************************************************************
	created:	2005/08/26
	created:	26:8:2005   20:33
	filename: 	I:\Heeze\function\function.h
	file path:	I:\Heeze\function
	file base:	function
	file ext:	h
	author:		Dashi Bai
	E-Mail:		gzxb525@gmail.com
	
	purpose:	
		不需要BOOST,但是降低了初始化FUNCTOR时的效率
*********************************************************************/


#ifndef HEEZE_FUNCTION_H
#define HEEZE_FUNCTION_H

#include "func_ptr.h"


HEEZE_NAMESPACE_BEGIN


#define HEEZE_TYPE_LIST_20 make_type_list<_T1, _T2, _T3, _T4, _T5, _T6, _T7, _T8, _T9, _T10, _T11, _T12, _T13, _T14, _T15, _T16, _T17, _T18, _T19, _T20>::type

template
<	
	typename _TRet
,	typename _T1  = null_type, typename _T2  = null_type, typename _T3  = null_type
,	typename _T4  = null_type, typename _T5  = null_type, typename _T6  = null_type
,	typename _T7  = null_type, typename _T8  = null_type, typename _T9  = null_type
,	typename _T10 = null_type, typename _T11 = null_type, typename _T12 = null_type
,	typename _T13 = null_type, typename _T14 = null_type, typename _T15 = null_type
,	typename _T16 = null_type, typename _T17 = null_type, typename _T18 = null_type 
,	typename _T19 = null_type, typename _T20 = null_type
>
struct function : 
	public type_select
	<
		is_void<_TRet>::value
	,	typename func_ptr
		<
			_TRet
		,	typename HEEZE_TYPE_LIST_20
		,	type_length< typename HEEZE_TYPE_LIST_20 >::value
		>
		::void_function_base
	,	typename func_ptr
		<
			_TRet
		,	typename HEEZE_TYPE_LIST_20
		,	type_length< typename HEEZE_TYPE_LIST_20 >::value
		>
		::function_base
	>
	::type
{	
	typedef typename HEEZE_TYPE_LIST_20 _List;
	typedef _TRet _Ret;
	typedef function< _Ret, _T1, _T2, _T3, _T4, _T5, _T6, _T7, _T8, _T9, _T10, _T11, _T12, _T13, _T14, _T15, _T16, _T17, _T18, _T19, _T20 > _Function; 
	enum{ _IsVoid = is_void<_TRet>::value };
	enum{ _ListLength = type_length< _List >::value };
	typedef func_ptr<_TRet, _List, _ListLength > _FuncPtr;

	friend struct _Function;

	//////////////////////////////////////////////////////////////////////////
	// 
	function(){
	}

	//////////////////////////////////////////////////////////////////////////
	// 
	bool usable(){
		return (NULL != m_ptr);
	}


	//////////////////////////////////////////////////////////////////////////
	// bind member function
	template<class _TCls, typename _TFunc>
	function( _TCls& obj, _TFunc func){
		typedef type_select
		<
			_IsVoid
		,	_FuncPtr::void_mem_func_ptr<_TCls, _TFunc>
		,	_FuncPtr::mem_func_ptr<_TCls, _TFunc> 
		>
		::type _MemFuncPtr;

		safe_delete(m_ptr);
		m_ptr = new _MemFuncPtr(&obj, func);
	}

	template<class _TCls, typename _TFunc>
	void bind( _TCls& obj, _TFunc func){
		typedef type_select
			<
				_IsVoid
			,	_FuncPtr::void_mem_func_ptr<_TCls, _TFunc>
			,	_FuncPtr::mem_func_ptr<_TCls, _TFunc> 
			>
			::type _MemFuncPtr;

		safe_delete(m_ptr);
		m_ptr = new _MemFuncPtr(&obj, func);
	}

	//////////////////////////////////////////////////////////////////////////
	// bind functor
	template< typename _TFunc >
	function( _TFunc func ){
		typedef _TFunc _Func;
		typedef type_select
			<
				_IsVoid
			,	_FuncPtr::void_functor_ptr<_Func>
			,	_FuncPtr::functor_ptr<_Func> 
			>
			::type _FunctorPtr; 

		safe_delete(m_ptr);
		m_ptr = new _FunctorPtr(func);
	}

	template< typename _TFunc >
	void bind( _TFunc func ){
		typedef _TFunc _Func;
		typedef type_select
			<
				_IsVoid
			,	_FuncPtr::void_functor_ptr<_Func>
			,	_FuncPtr::functor_ptr<_Func> 
			>
			::type _FunctorPtr; 

		safe_delete(m_ptr);
		m_ptr = new _FunctorPtr(func);
	}

	//////////////////////////////////////////////////////////////////////////
	// copy function
	function ( const _Function& func ){
		safe_delete( m_ptr );
		m_ptr = func.m_ptr->clone();
	}

	_Function& operator=( const _Function& func ){
		safe_delete( m_ptr );
		m_ptr = func.m_ptr->clone();
		return *this;
	}	
};


HEEZE_NAMESPACE_END

#endif	// HEEZE_FUNCTION_H

		
		
