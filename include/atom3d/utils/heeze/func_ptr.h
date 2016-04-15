/********************************************************************
	created:	2005/08/26
	created:	26:8:2005   20:37
	filename: 	I:\Heeze\function\func_ptr.h
	file path:	I:\Heeze\function
	file base:	func_ptr
	file ext:	h
	author:		Dashi Bai
	E-Mail:		gzxb525@gmail.com
	
	purpose:	
*********************************************************************/


#ifndef HEEZE_FUNC_PTR_H
#define HEEZE_FUNC_PTR_H

#include "type_list.h"
#include "safe_delete.h"
//#include "exception.h"

#ifndef HEEZE_V
#define HEEZE_V( List, INDEX ) typename type_at< typename List, INDEX >::type t##INDEX
#endif

#define HEEZE_CREATE_FUNC_PTR( NUM, VARIBLES, PARAMS )	\
template<typename _TRet, typename _TList>	\
struct func_ptr< _TRet, _TList, NUM >{	\
	struct func_ptr_base{	\
		virtual func_ptr_base* clone() = 0;	\
		virtual _TRet operator() VARIBLES = 0;	\
	};	\
	template<class _TCls, typename _TFunc>	\
	struct mem_func_ptr : public func_ptr_base{	\
		mem_func_ptr( _TCls* po, _TFunc pf ) : m_po(po), m_pf(pf){ }	\
		_TRet operator() VARIBLES {		\
			return (m_po->*m_pf) PARAMS ;		\
		}	\
		func_ptr_base* clone(){		\
			return new mem_func_ptr< _TCls, _TFunc >(m_po, m_pf);	\
		}	\
	protected:	\
		_TFunc m_pf;		\
		_TCls* m_po;		\
	};	\
	template< class _TCls, typename _TFunc>	\
	struct void_mem_func_ptr : public func_ptr_base{		\
		void_mem_func_ptr( _TCls* po, _TFunc pf ) : m_po(po), m_pf(pf){	}		\
		_TRet operator() VARIBLES {		\
			(m_po->*m_pf) PARAMS ;		\
		}		\
		func_ptr_base* clone(){	\
			return new void_mem_func_ptr<_TCls, _TFunc>(m_po, m_pf);	}		\
	protected:	\
		_TFunc m_pf;		\
		_TCls* m_po;		\
	};	\
	template< typename _TFunc >	\
	struct functor_ptr : public func_ptr_base{	\
		functor_ptr( _TFunc f ) : m_f(f) {}	\
		_TRet operator() VARIBLES {			\
			return m_f PARAMS ;		\
		}		\
		func_ptr_base* clone(){	\
			return new functor_ptr<_TFunc>(m_f);	}	\
	protected:	\
		_TFunc m_f;	\
	};	\
	template< typename _TFunc >	\
	struct void_functor_ptr : public func_ptr_base{	\
		void_functor_ptr( _TFunc f ) : m_f(f) {}	\
		_TRet operator() VARIBLES {			\
			m_f PARAMS ;		\
		}		\
		func_ptr_base* clone(){	\
			return new void_functor_ptr<_TFunc>(m_f);	}	\
	protected:	\
		_TFunc m_f;	\
	};	\
	struct void_function_base{		\
		void_function_base() : m_ptr(NULL){}	\
		~void_function_base(){ safe_delete( m_ptr ); }	\
		_TRet operator() VARIBLES {		\
			if( !m_ptr ) throw bad_ptr();	\
			(*m_ptr) PARAMS ;		\
		}		\
	protected:	\
		func_ptr_base* m_ptr;	\
	};	\
	struct function_base{		\
		function_base() : m_ptr(NULL){}	\
		~function_base(){ safe_delete( m_ptr ); }	\
		_TRet operator() VARIBLES {		\
			if( !m_ptr ) throw bad_ptr();	\
			return (*m_ptr) PARAMS ;	\
		}		\
	protected:	\
		func_ptr_base* m_ptr;	\
	};	\
};


HEEZE_NAMESPACE_BEGIN

template<typename _TRet, typename _TList, unsigned int i > struct func_ptr;

/*
//////////////////////////////////////////////////////////////////////////
// JUST FOR TEST
template<typename _TRet, typename _TList>	
struct func_ptr< _TRet, _TList, 0 >{	
	struct func_ptr_base{	
		virtual func_ptr_base* clone() = 0;	
		virtual _TRet operator() ( ) = 0;	
	};	
	template<class _TCls, typename _TFunc>	
	struct mem_func_ptr : public func_ptr_base{	
		mem_func_ptr( _TCls* po, _TFunc pf ) : m_po(po), m_pf(pf){ }	
		_TRet operator() ( ) {		
			return (m_po->*m_pf) (  ) ;		
		}	
		func_ptr_base* clone(){		
			return new mem_func_ptr< _TCls, _TFunc >(m_po, m_pf);	
		}	
	protected:	
		_TFunc m_pf;		
		_TCls* m_po;		
	};	
	template< class _TCls, typename _TFunc>	
	struct void_mem_func_ptr : public func_ptr_base{		
		void_mem_func_ptr( _TCls* po, _TFunc pf ) : m_po(po), m_pf(pf){	}		
		_TRet operator() ( ) {		
			(m_po->*m_pf) ( ) ;		
		}		
		func_ptr_base* clone(){	
			return new void_mem_func_ptr<_TCls, _TFunc>(m_po, m_pf);		
		}		
	protected:	
		_TFunc m_pf;		
		_TCls* m_po;		
	};	
	template< typename _TFunc >
	struct functor_ptr : public func_ptr_base{
		functor_ptr( typename type_select< boost::is_function< _TFunc >::value,	_TFunc,	_TFunc& >::type f ) : m_f(f) {}
		_TRet operator() ( ) {		
			return m_f ( ) ;		
		}		
		func_ptr_base* clone(){	
			return new functor_ptr<_TFunc>(m_f);		
		}
	protected:
		_TFunc m_f;
	};
	template< typename _TFunc >
	struct void_functor_ptr : public func_ptr_base{
		void_functor_ptr( typename type_select< boost::is_function< _TFunc >::value,	_TFunc,	_TFunc& >::type f ) : m_f(f) {}
		_TRet operator() ( ) {		
			m_f ( ) ;		
		}		
		func_ptr_base* clone(){	
			return new void_functor_ptr<_TFunc>(m_f);		
		}
	protected:
		_TFunc m_f;
	};
	struct void_function_base{		
		void_function_base() : m_ptr(NULL){}	
		~void_function_base(){ safe_delete( m_ptr ); }	
		_TRet operator() ( ) {		
			if( !m_ptr ) throw exception(HEEZE_ERROR_FUNCTION_NO_BIND);	
			(*m_ptr) (  ) ;		
		}		
	protected:	
		func_ptr_base* m_ptr;	
	};	
	struct function_base{		
		function_base() : m_ptr(NULL){}	
		~function_base(){ safe_delete( m_ptr ); }	
		_TRet operator() ( ) {	
			if( !m_ptr ) throw exception(HEEZE_ERROR_FUNCTION_NO_BIND);	
			return (*m_ptr) ( ) ;	
		}		
	protected:	
		func_ptr_base* m_ptr;	
	};	
};
*/


HEEZE_CREATE_FUNC_PTR( 0
	, ()
	, () )

HEEZE_CREATE_FUNC_PTR( 1
	, ( HEEZE_V(_TList, 0) )
	, ( t0 ) )

HEEZE_CREATE_FUNC_PTR( 2
	, ( HEEZE_V(_TList, 0), HEEZE_V(_TList, 1) )
	, ( t0, t1 ) )

HEEZE_CREATE_FUNC_PTR( 3
	, ( HEEZE_V(_TList, 0), HEEZE_V(_TList, 1), HEEZE_V(_TList, 2) )
	, ( t0, t1, t2 ) )

HEEZE_CREATE_FUNC_PTR( 4
	, ( HEEZE_V(_TList, 0), HEEZE_V(_TList, 1), HEEZE_V(_TList, 2), HEEZE_V(_TList, 3) )
	, ( t0, t1, t2, t3 ) )

HEEZE_CREATE_FUNC_PTR( 5
	, ( HEEZE_V(_TList, 0), HEEZE_V(_TList, 1), HEEZE_V(_TList, 2), HEEZE_V(_TList, 3)
	,	HEEZE_V(_TList, 4) )
	, ( t0, t1, t2, t3, t4 ) )

HEEZE_CREATE_FUNC_PTR( 6
	, ( HEEZE_V(_TList, 0), HEEZE_V(_TList, 1), HEEZE_V(_TList, 2), HEEZE_V(_TList, 3)
	,	HEEZE_V(_TList, 4), HEEZE_V(_TList, 5) )
	, ( t0, t1, t2, t3, t4, t5 ) )

HEEZE_CREATE_FUNC_PTR( 7
	, ( HEEZE_V(_TList, 0), HEEZE_V(_TList, 1), HEEZE_V(_TList, 2), HEEZE_V(_TList, 3)
	,	HEEZE_V(_TList, 4), HEEZE_V(_TList, 5), HEEZE_V(_TList, 6) )
	, ( t0, t1, t2, t3, t4, t5, t6 ) )

HEEZE_CREATE_FUNC_PTR( 8
	, ( HEEZE_V(_TList, 0), HEEZE_V(_TList, 1), HEEZE_V(_TList, 2), HEEZE_V(_TList, 3)
	,	HEEZE_V(_TList, 4), HEEZE_V(_TList, 5), HEEZE_V(_TList, 6), HEEZE_V(_TList, 7) )
	, ( t0, t1, t2, t3, t4, t5, t6, t7 ) )

HEEZE_CREATE_FUNC_PTR( 9
	, ( HEEZE_V(_TList, 0), HEEZE_V(_TList, 1), HEEZE_V(_TList, 2), HEEZE_V(_TList, 3)
	,	HEEZE_V(_TList, 4), HEEZE_V(_TList, 5), HEEZE_V(_TList, 6), HEEZE_V(_TList, 7) 
	,	HEEZE_V(_TList, 8) )
	, ( t0, t1, t2, t3, t4, t5, t6, t7, t8 ) )

HEEZE_CREATE_FUNC_PTR( 10
	, ( HEEZE_V(_TList, 0), HEEZE_V(_TList, 1), HEEZE_V(_TList, 2), HEEZE_V(_TList, 3)
	,	HEEZE_V(_TList, 4), HEEZE_V(_TList, 5), HEEZE_V(_TList, 6), HEEZE_V(_TList, 7) 
	,	HEEZE_V(_TList, 8), HEEZE_V(_TList, 9) )
	, ( t0, t1, t2, t3, t4, t5, t6, t7, t8, t9 ) )

HEEZE_CREATE_FUNC_PTR( 11
	, ( HEEZE_V(_TList, 0), HEEZE_V(_TList, 1), HEEZE_V(_TList, 2), HEEZE_V(_TList, 3)
	,	HEEZE_V(_TList, 4), HEEZE_V(_TList, 5), HEEZE_V(_TList, 6), HEEZE_V(_TList, 7) 
	,	HEEZE_V(_TList, 8), HEEZE_V(_TList, 9), HEEZE_V(_TList, 10) )
	, ( t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10 ) )

HEEZE_CREATE_FUNC_PTR( 12
	, ( HEEZE_V(_TList, 0), HEEZE_V(_TList, 1), HEEZE_V(_TList, 2), HEEZE_V(_TList, 3)
	,	HEEZE_V(_TList, 4), HEEZE_V(_TList, 5), HEEZE_V(_TList, 6), HEEZE_V(_TList, 7) 
	,	HEEZE_V(_TList, 8), HEEZE_V(_TList, 9), HEEZE_V(_TList, 10), HEEZE_V(_TList, 11) )
	, ( t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11 ) )

HEEZE_CREATE_FUNC_PTR( 13
	, ( HEEZE_V(_TList, 0), HEEZE_V(_TList, 1), HEEZE_V(_TList, 2), HEEZE_V(_TList, 3)
	,	HEEZE_V(_TList, 4), HEEZE_V(_TList, 5), HEEZE_V(_TList, 6), HEEZE_V(_TList, 7) 
	,	HEEZE_V(_TList, 8), HEEZE_V(_TList, 9), HEEZE_V(_TList, 10), HEEZE_V(_TList, 11)
	,	HEEZE_V(_TList, 12) )
	, ( t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12 ) )

HEEZE_CREATE_FUNC_PTR( 14
	, ( HEEZE_V(_TList, 0), HEEZE_V(_TList, 1), HEEZE_V(_TList, 2), HEEZE_V(_TList, 3)
	,	HEEZE_V(_TList, 4), HEEZE_V(_TList, 5), HEEZE_V(_TList, 6), HEEZE_V(_TList, 7) 
	,	HEEZE_V(_TList, 8), HEEZE_V(_TList, 9), HEEZE_V(_TList, 10), HEEZE_V(_TList, 11)
	,	HEEZE_V(_TList, 12), HEEZE_V(_TList, 13) )
	, ( t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13 ) )

HEEZE_CREATE_FUNC_PTR( 15
	, ( HEEZE_V(_TList, 0), HEEZE_V(_TList, 1), HEEZE_V(_TList, 2), HEEZE_V(_TList, 3)
	,	HEEZE_V(_TList, 4), HEEZE_V(_TList, 5), HEEZE_V(_TList, 6), HEEZE_V(_TList, 7) 
	,	HEEZE_V(_TList, 8), HEEZE_V(_TList, 9), HEEZE_V(_TList, 10), HEEZE_V(_TList, 11)
	,	HEEZE_V(_TList, 12), HEEZE_V(_TList, 13), HEEZE_V(_TList, 14) )
	, ( t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14 ) )

HEEZE_CREATE_FUNC_PTR( 16
	, ( HEEZE_V(_TList, 0), HEEZE_V(_TList, 1), HEEZE_V(_TList, 2), HEEZE_V(_TList, 3)
	,	HEEZE_V(_TList, 4), HEEZE_V(_TList, 5), HEEZE_V(_TList, 6), HEEZE_V(_TList, 7) 
	,	HEEZE_V(_TList, 8), HEEZE_V(_TList, 9), HEEZE_V(_TList, 10), HEEZE_V(_TList, 11)
	,	HEEZE_V(_TList, 12), HEEZE_V(_TList, 13), HEEZE_V(_TList, 14), HEEZE_V(_TList, 15) )
	, ( t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15 ) )

HEEZE_CREATE_FUNC_PTR( 17
	, ( HEEZE_V(_TList, 0), HEEZE_V(_TList, 1), HEEZE_V(_TList, 2), HEEZE_V(_TList, 3)
	,	HEEZE_V(_TList, 4), HEEZE_V(_TList, 5), HEEZE_V(_TList, 6), HEEZE_V(_TList, 7) 
	,	HEEZE_V(_TList, 8), HEEZE_V(_TList, 9), HEEZE_V(_TList, 10), HEEZE_V(_TList, 11)
	,	HEEZE_V(_TList, 12), HEEZE_V(_TList, 13), HEEZE_V(_TList, 14), HEEZE_V(_TList, 15)
	,	HEEZE_V(_TList, 16) )
	, ( t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15, t16 ) )

HEEZE_CREATE_FUNC_PTR( 18
	, ( HEEZE_V(_TList, 0), HEEZE_V(_TList, 1), HEEZE_V(_TList, 2), HEEZE_V(_TList, 3)
	,	HEEZE_V(_TList, 4), HEEZE_V(_TList, 5), HEEZE_V(_TList, 6), HEEZE_V(_TList, 7) 
	,	HEEZE_V(_TList, 8), HEEZE_V(_TList, 9), HEEZE_V(_TList, 10), HEEZE_V(_TList, 11)
	,	HEEZE_V(_TList, 12), HEEZE_V(_TList, 13), HEEZE_V(_TList, 14), HEEZE_V(_TList, 15)
	,	HEEZE_V(_TList, 16), HEEZE_V(_TList, 17) )
	, ( t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15, t16, t17 ) )
	
HEEZE_CREATE_FUNC_PTR( 19
	, ( HEEZE_V(_TList, 0), HEEZE_V(_TList, 1), HEEZE_V(_TList, 2), HEEZE_V(_TList, 3)
	,	HEEZE_V(_TList, 4), HEEZE_V(_TList, 5), HEEZE_V(_TList, 6), HEEZE_V(_TList, 7) 
	,	HEEZE_V(_TList, 8), HEEZE_V(_TList, 9), HEEZE_V(_TList, 10), HEEZE_V(_TList, 11)
	,	HEEZE_V(_TList, 12), HEEZE_V(_TList, 13), HEEZE_V(_TList, 14), HEEZE_V(_TList, 15)
	,	HEEZE_V(_TList, 16), HEEZE_V(_TList, 17), HEEZE_V(_TList, 18) )
	, ( t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15, t16, t17, t18 ) )
	
HEEZE_CREATE_FUNC_PTR( 20
	, ( HEEZE_V(_TList, 0), HEEZE_V(_TList, 1), HEEZE_V(_TList, 2), HEEZE_V(_TList, 3)
	,	HEEZE_V(_TList, 4), HEEZE_V(_TList, 5), HEEZE_V(_TList, 6), HEEZE_V(_TList, 7) 
	,	HEEZE_V(_TList, 8), HEEZE_V(_TList, 9), HEEZE_V(_TList, 10), HEEZE_V(_TList, 11)
	,	HEEZE_V(_TList, 12), HEEZE_V(_TList, 13), HEEZE_V(_TList, 14), HEEZE_V(_TList, 15)
	,	HEEZE_V(_TList, 16), HEEZE_V(_TList, 17), HEEZE_V(_TList, 18), HEEZE_V(_TList, 19) )
	, ( t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15, t16, t17, t18, 19 ) )


HEEZE_NAMESPACE_END

#endif	// HEEZE_FUNC_PTR_H

		
		
