/********************************************************************
	created:	2005/08/23
	created:	23:8:2005   12:19
	filename: 	I:\Heeze\Util\type_list.h
	file path:	I:\Heeze\Util
	file base:	type_list
	file ext:	h
	author:		Dashi Bai
	email:		gzxb525@gmail.com
	
	purpose:	
*********************************************************************/


#ifndef HEEZE_TYPE_LIST_H
#define HEEZE_TYPE_LIST_H

#include "mydm.h"

HEEZE_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////
// type_list
template <class _T, class _U>
struct type_list{
	typedef _T _Head;
	typedef _U _Tail;
};

//////////////////////////////////////////////////////////////////////////
// make_type_list_impl
//namespace Private{

template
<	
	typename _T1  = null_type, typename _T2  = null_type, typename _T3  = null_type
,	typename _T4  = null_type, typename _T5  = null_type, typename _T6  = null_type
,	typename _T7  = null_type, typename _T8  = null_type, typename _T9  = null_type
,	typename _T10 = null_type, typename _T11 = null_type, typename _T12 = null_type
,	typename _T13 = null_type, typename _T14 = null_type, typename _T15 = null_type
,	typename _T16 = null_type, typename _T17 = null_type, typename _T18 = null_type 
,	typename _T19 = null_type, typename _T20 = null_type, typename _T21 = null_type
,	typename _T22 = null_type, typename _T23 = null_type, typename _T24 = null_type
> 
struct make_type_list
{	
private:
	typedef typename make_type_list
		<	
			_T2 , _T3 , _T4  
		,	_T5 , _T6 , _T7  
		,	_T8 , _T9 , _T10 
		,	_T11, _T12, _T13
		,	_T14, _T15, _T16 
		,	_T17, _T18, _T19
		,	_T20, _T21, _T22 
		,	_T23, _T24
		>
		::type _TailType;
public:
	typedef type_list<_T1, _TailType> type;
};

template<>
struct make_type_list
	<
		null_type, null_type, null_type
	,	null_type, null_type, null_type
	,	null_type, null_type, null_type
	,	null_type, null_type, null_type
	,	null_type, null_type, null_type
	,	null_type, null_type, null_type
	,	null_type, null_type, null_type
	,	null_type, null_type, null_type
	>
{
	typedef null_type type;
};

//////////////////////////////////////////////////////////////////////////
// type_at
// input: type list and the index which the class in the type list
// return: the class
template <class _TList, unsigned int index> struct type_at;

template <class _T, class _U>
struct type_at<type_list<_T, _U>, 0>{
	typedef _T type;
};

template <class _T, class _U, unsigned int i>
struct type_at<type_list<_T, _U>, i>{
	typedef typename type_at<_U, i - 1>::type type;
};


//////////////////////////////////////////////////////////////////////////
// type_list_length
//namespace Private{
template <class _TList>
struct type_length{
	enum {value = 1 + type_length<_TList::_Tail>::value};
};

template <>
struct type_length<null_type>{
	enum {value = 0};
};


//////////////////////////////////////////////////////////////////////////
// type_select
template< bool b, typename _T, typename _U >
struct type_select{
	typedef _U type;
};

template< typename _T, typename _U >
struct type_select< true, _T, _U >{
	typedef _T type;
};

//////////////////////////////////////////////////////////////////////////
// is_same_type
template<typename T1, typename T2>
struct is_same_type{
	enum{ value = 0 };
};


template<typename T1>
struct is_same_type<T1, T1>{
	enum{ value = 1	};
};

//////////////////////////////////////////////////////////////////////////
// is_void
template<typename _T>
struct is_void{
	enum{ value = 0	};
};

template<>
struct is_void<void>{
	enum{ value = 1	};
};

HEEZE_NAMESPACE_END

#endif	// HEEZE_TYPE_LIST_H