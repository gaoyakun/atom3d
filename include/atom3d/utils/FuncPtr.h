// 函数指针的容器(全局函数,静态函数,成员函数,仿函数)
// dashi
#ifndef FUNC_PTR_H
#define FUNC_PTR_H

#include "heeze/type_list.h"
#include <assert.h>

#define FUNC_V( LIST, INDEX ) typename heeze::type_at< LIST, INDEX >::type t##INDEX
#define FUNC_V1( LIST ) FUNC_V(LIST, 0)
#define FUNC_V2( LIST ) FUNC_V1(LIST), FUNC_V(LIST, 1)
#define FUNC_V3( LIST ) FUNC_V2(LIST), FUNC_V(LIST, 2)
#define FUNC_V4( LIST ) FUNC_V3(LIST), FUNC_V(LIST, 3)
#define FUNC_V5( LIST ) FUNC_V4(LIST), FUNC_V(LIST, 4)
#define FUNC_V6( LIST ) FUNC_V5(LIST), FUNC_V(LIST, 5)
#define FUNC_V7( LIST ) FUNC_V6(LIST), FUNC_V(LIST, 6)
#define FUNC_V8( LIST ) FUNC_V7(LIST), FUNC_V(LIST, 7)
#define FUNC_V9( LIST ) FUNC_V8(LIST), FUNC_V(LIST, 8)

#define FUNC_P1 t0
#define FUNC_P2 FUNC_P1, t1
#define FUNC_P3 FUNC_P2, t2
#define FUNC_P4 FUNC_P3, t3
#define FUNC_P5 FUNC_P4, t4
#define FUNC_P6 FUNC_P5, t5
#define FUNC_P7 FUNC_P6, t6
#define FUNC_P8 FUNC_P7, t7
#define FUNC_P9 FUNC_P8, t8

HEEZE_NAMESPACE_BEGIN

struct IFuncPtrBase {
	inline IFuncPtrBase() 
		: _nRef(1) {
	}
	virtual ~IFuncPtrBase(){
	}
	inline int aquire () { 
		return ++_nRef; 
	}
	inline void release () { 
		if (!--_nRef) 
			delete this; 
	}
private:
	int _nRef;
};

template<int nParamCount, typename _TRtn, typename _TLst>
struct FuncPtrHelper;

template<typename _TRtn, typename _TLst>
struct FuncPtrHelper<0, _TRtn, _TLst>{
	struct IFuncPtr : public IFuncPtrBase{
		virtual _TRtn invoke ( ) = 0;
	};
	template<typename _TCls, typename _TFunc>	// FIXME::MemFuncPtr
	struct FuncPtr : public IFuncPtr{
		inline FuncPtr(_TCls* pObj, _TFunc func) : _pObj(pObj), _Func(func){}
		inline virtual _TRtn invoke ( ) {	
			// FIXME:: _TCls is class, _TFunc is member function, 
			//	只能检测到如此,不能在编译期确定_TFunc是否是_TCls的成员函数
			return (_pObj->*_Func) () ;
		}
	private:
		_TCls* _pObj;
		_TFunc _Func;
	};
	template<typename _TCls, typename _TFunc>	// FIXME::VoidMemFuncPtr
	struct VoidFuncPtr : public IFuncPtr	{
		inline VoidFuncPtr(_TCls* pObj, _TFunc func) : _pObj(pObj), _Func(func){}
		inline virtual _TRtn invoke ( ) {
			// FIXME:: _TCls is class, _TFunc is member function, 
			//	只能检测到如此,不能在编译期确定_TFunc是否是_TCls的成员函数
			(_pObj->*_Func) () ;
		}
	private:
		_TCls* _pObj;
		_TFunc _Func;
	};
	template< typename _TFunc >
	struct FunctorPtr : public IFuncPtr{
		inline FunctorPtr( _TFunc func ) : _Func(func) {}
		inline virtual _TRtn invoke ( ) {	
			// FIXME:: _TFunc is class and have operator() -->_TFunc::operator()
			//			or global function or static function
			return _Func ( ) ;		
		}		
	private:
		_TFunc _Func;
	};
	template< typename _TFunc >
	struct VoidFunctorPtr : public IFuncPtr{
		VoidFunctorPtr( _TFunc func ) : _Func(func) {}
		inline virtual _TRtn invoke ( ) {	
			// FIXME:: _TFunc is class and have operator() -->_TFunc::operator()
			//			or global function or static function
			_Func ( ) ;		
		}		
	private:
		_TFunc _Func;
	};
	struct VoidFuncBase{		
		VoidFuncBase( IFuncPtr* pFunc ) : _pFunc(pFunc){}	
		inline _TRtn operator() () {	
			assert(_pFunc);	// 函数还未被初始化
			_pFunc->invoke();
		}		
	protected:	
		IFuncPtr* _pFunc;
	};	
	struct FuncBase{		
		FuncBase(IFuncPtr* pFunc) : _pFunc(pFunc){}	
		_TRtn operator() () {		
			assert(_pFunc);	// 函数还未被初始化
			return _pFunc->invoke() ;	
		}		
	protected:	
		IFuncPtr* _pFunc;	
	};	
};

template<typename _TRtn, typename _TLst>
struct FuncPtrHelper<1, _TRtn, _TLst>{
	struct IFuncPtr : public IFuncPtrBase{
		virtual _TRtn invoke ( FUNC_V1(_TLst) ) = 0;
	};
	template<typename _TCls, typename _TFunc>
	struct FuncPtr : public IFuncPtr{
		inline FuncPtr(_TCls* pObj, _TFunc func) : _pObj(pObj), _Func(func){}
		inline virtual _TRtn invoke ( FUNC_V1(_TLst) ) {
			return (_pObj->*_Func) (FUNC_P1) ;
		}
	private:
		_TCls* _pObj;
		_TFunc _Func;
	};
	template<typename _TCls, typename _TFunc>
	struct VoidFuncPtr : public IFuncPtr	{
		inline VoidFuncPtr(_TCls* pObj, _TFunc func) : _pObj(pObj), _Func(func){}
		inline virtual _TRtn invoke ( FUNC_V1(_TLst) ) {
			(_pObj->*_Func) (FUNC_P1) ;
		}
	private:
		_TCls* _pObj;
		_TFunc _Func;
	};
	template< typename _TFunc >
	struct FunctorPtr : public IFuncPtr{
		inline FunctorPtr( _TFunc func ) : _Func(func) {}
		inline virtual _TRtn invoke (FUNC_V1(_TLst) ) {		
			return _Func (FUNC_P1 ) ;		
		}		
	private:
		_TFunc _Func;
	};
	template< typename _TFunc >
	struct VoidFunctorPtr : public IFuncPtr{
		VoidFunctorPtr( _TFunc func ) : _Func(func) {}
		inline virtual _TRtn invoke (FUNC_V1(_TLst) ) {		
			_Func (FUNC_P1 ) ;		
		}		
	private:
		_TFunc _Func;
	};
	struct VoidFuncBase{		
		VoidFuncBase( IFuncPtr* pFunc ) : _pFunc(pFunc){}	
		inline _TRtn operator() (FUNC_V1(_TLst)) {	
			assert(_pFunc);
			_pFunc->invoke(FUNC_P1);
		}		
	protected:	
		IFuncPtr* _pFunc;
	};	
	struct FuncBase{		
		FuncBase(IFuncPtr* pFunc) : _pFunc(pFunc){}	
		_TRtn operator() (FUNC_V1(_TLst)) {		
			assert(_pFunc);
			return _pFunc->invoke(FUNC_P1) ;	
		}		
	protected:	
		IFuncPtr* _pFunc;	
	};
};

template<typename _TRtn, typename _TLst>
struct FuncPtrHelper<2, _TRtn, _TLst>{
	struct IFuncPtr : public IFuncPtrBase{
		virtual _TRtn invoke ( FUNC_V2(_TLst) ) = 0;
	};
	template<typename _TCls, typename _TFunc>
	struct FuncPtr : public IFuncPtr{
		inline FuncPtr(_TCls* pObj, _TFunc func) : _pObj(pObj), _Func(func){}
		inline virtual _TRtn invoke ( FUNC_V2(_TLst) ) {
			return (_pObj->*_Func) (FUNC_P2) ;
		}
	private:
		_TCls* _pObj;
		_TFunc _Func;
	};
	template<typename _TCls, typename _TFunc>
	struct VoidFuncPtr : public IFuncPtr	{
		inline VoidFuncPtr(_TCls* pObj, _TFunc func) : _pObj(pObj), _Func(func){}
		inline virtual _TRtn invoke ( FUNC_V2(_TLst) ) {
			(_pObj->*_Func) (FUNC_P2) ;
		}
	private:
		_TCls* _pObj;
		_TFunc _Func;
	};
	template< typename _TFunc >
	struct FunctorPtr : public IFuncPtr{
		inline FunctorPtr( _TFunc func ) : _Func(func) {}
		inline virtual _TRtn invoke (FUNC_V2(_TLst) ) {		
			return _Func (FUNC_P2) ;		
		}		
	private:
		_TFunc _Func;
	};
	template< typename _TFunc >
	struct VoidFunctorPtr : public IFuncPtr{
		VoidFunctorPtr( _TFunc func ) : _Func(func) {}
		inline virtual _TRtn invoke (FUNC_V2(_TLst) ) {		
			_Func (FUNC_P2) ;		
		}		
	private:
		_TFunc _Func;
	};
	struct VoidFuncBase{		
		VoidFuncBase( IFuncPtr* pFunc ) : _pFunc(pFunc){}	
		inline _TRtn operator() (FUNC_V2(_TLst)) {	
			assert(_pFunc);
			_pFunc->invoke(FUNC_P2);
		}		
	protected:	
		IFuncPtr* _pFunc;
	};	
	struct FuncBase{		
		FuncBase(IFuncPtr* pFunc) : _pFunc(pFunc){}	
		_TRtn operator() (FUNC_V2(_TLst)) {		
			assert(_pFunc);
			return _pFunc->invoke(FUNC_P2) ;	
		}		
	protected:	
		IFuncPtr* _pFunc;	
	};
};

template<typename _TRtn, typename _TLst>
struct FuncPtrHelper<3, _TRtn, _TLst>{
	struct IFuncPtr : public IFuncPtrBase{
		virtual _TRtn invoke ( FUNC_V3(_TLst) ) = 0;
	};
	template<typename _TCls, typename _TFunc>
	struct FuncPtr : public IFuncPtr{
		inline FuncPtr(_TCls* pObj, _TFunc func) : _pObj(pObj), _Func(func){}
		inline virtual _TRtn invoke ( FUNC_V3(_TLst) ) {
			return (_pObj->*_Func) (FUNC_P3) ;
		}
	private:
		_TCls* _pObj;
		_TFunc _Func;
	};
	template<typename _TCls, typename _TFunc>
	struct VoidFuncPtr : public IFuncPtr	{
		inline VoidFuncPtr(_TCls* pObj, _TFunc func) : _pObj(pObj), _Func(func){}
		inline virtual _TRtn invoke ( FUNC_V3(_TLst) ) {
			(_pObj->*_Func) (FUNC_P3) ;
		}
	private:
		_TCls* _pObj;
		_TFunc _Func;
	};
	template< typename _TFunc >
	struct FunctorPtr : public IFuncPtr{
		inline FunctorPtr( _TFunc func ) : _Func(func) {}
		inline virtual _TRtn invoke (FUNC_V3(_TLst) ) {		
			return _Func (FUNC_P3 ) ;		
		}		
	private:
		_TFunc _Func;
	};
	template< typename _TFunc >
	struct VoidFunctorPtr : public IFuncPtr{
		VoidFunctorPtr( _TFunc func ) : _Func(func) {}
		inline virtual _TRtn invoke (FUNC_V3(_TLst) ) {		
			_Func (FUNC_P3 ) ;		
		}		
	private:
		_TFunc _Func;
	};
	struct VoidFuncBase{		
		VoidFuncBase( IFuncPtr* pFunc ) : _pFunc(pFunc){}	
		inline _TRtn operator() (FUNC_V3(_TLst)) {	
			assert(_pFunc);
			_pFunc->invoke(FUNC_P3);
		}		
	protected:	
		IFuncPtr* _pFunc;
	};	
	struct FuncBase{		
		FuncBase(IFuncPtr* pFunc) : _pFunc(pFunc){}	
		_TRtn operator() (FUNC_V3(_TLst)) {		
			assert(_pFunc);
			return _pFunc->invoke(FUNC_P3) ;	
		}		
	protected:	
		IFuncPtr* _pFunc;	
	};
};

template<typename _TRtn, typename _TLst>
struct FuncPtrHelper<4, _TRtn, _TLst>{
	struct IFuncPtr : public IFuncPtrBase{
		virtual _TRtn invoke ( FUNC_V4(_TLst) ) = 0;
	};
	template<typename _TCls, typename _TFunc>
	struct FuncPtr : public IFuncPtr{
		inline FuncPtr(_TCls* pObj, _TFunc func) : _pObj(pObj), _Func(func){}
		inline virtual _TRtn invoke ( FUNC_V4(_TLst) ) {
			return (_pObj->*_Func) (FUNC_P4) ;
		}
	private:
		_TCls* _pObj;
		_TFunc _Func;
	};
	template<typename _TCls, typename _TFunc>
	struct VoidFuncPtr : public IFuncPtr	{
		inline VoidFuncPtr(_TCls* pObj, _TFunc func) : _pObj(pObj), _Func(func){}
		inline virtual _TRtn invoke ( FUNC_V4(_TLst) ) {
			(_pObj->*_Func) (FUNC_P4) ;
		}
	private:
		_TCls* _pObj;
		_TFunc _Func;
	};
	template< typename _TFunc >
	struct FunctorPtr : public IFuncPtr{
		inline FunctorPtr( _TFunc func ) : _Func(func) {}
		inline virtual _TRtn invoke (FUNC_V4(_TLst) ) {		
			return _Func (FUNC_P4 ) ;		
		}		
	private:
		_TFunc _Func;
	};
	template< typename _TFunc >
	struct VoidFunctorPtr : public IFuncPtr{
		VoidFunctorPtr( _TFunc func ) : _Func(func) {}
		inline virtual _TRtn invoke (FUNC_V4(_TLst) ) {		
			_Func (FUNC_P4 ) ;		
		}		
	private:
		_TFunc _Func;
	};
	struct VoidFuncBase{		
		VoidFuncBase( IFuncPtr* pFunc ) : _pFunc(pFunc){}	
		inline _TRtn operator() (FUNC_V4(_TLst)) {	
			assert(_pFunc);
			_pFunc->invoke(FUNC_P4);
		}		
	protected:	
		IFuncPtr* _pFunc;
	};	
	struct FuncBase{		
		FuncBase(IFuncPtr* pFunc) : _pFunc(pFunc){}	
		_TRtn operator() (FUNC_V4(_TLst)) {		
			assert(_pFunc);
			return _pFunc->invoke(FUNC_P4) ;	
		}		
	protected:	
		IFuncPtr* _pFunc;	
	};
};

template<typename _TRtn, typename _TLst>
struct FuncPtrHelper<5, _TRtn, _TLst>{
	struct IFuncPtr : public IFuncPtrBase{
		virtual _TRtn invoke ( FUNC_V5(_TLst) ) = 0;
	};
	template<typename _TCls, typename _TFunc>
	struct FuncPtr : public IFuncPtr{
		inline FuncPtr(_TCls* pObj, _TFunc func) : _pObj(pObj), _Func(func){}
		inline virtual _TRtn invoke ( FUNC_V5(_TLst) ) {
			return (_pObj->*_Func) (FUNC_P5) ;
		}
	private:
		_TCls* _pObj;
		_TFunc _Func;
	};
	template<typename _TCls, typename _TFunc>
	struct VoidFuncPtr : public IFuncPtr	{
		inline VoidFuncPtr(_TCls* pObj, _TFunc func) : _pObj(pObj), _Func(func){}
		inline virtual _TRtn invoke ( FUNC_V5(_TLst) ) {
			(_pObj->*_Func) (FUNC_P5) ;
		}
	private:
		_TCls* _pObj;
		_TFunc _Func;
	};
	template< typename _TFunc >
	struct FunctorPtr : public IFuncPtr{
		inline FunctorPtr( _TFunc func ) : _Func(func) {}
		inline virtual _TRtn invoke (FUNC_V5(_TLst) ) {		
			return _Func (FUNC_P5 ) ;		
		}		
	private:
		_TFunc _Func;
	};
	template< typename _TFunc >
	struct VoidFunctorPtr : public IFuncPtr{
		VoidFunctorPtr( _TFunc func ) : _Func(func) {}
		inline virtual _TRtn invoke (FUNC_V5(_TLst) ) {		
			_Func (FUNC_P5 ) ;		
		}		
	private:
		_TFunc _Func;
	};
	struct VoidFuncBase{		
		VoidFuncBase( IFuncPtr* pFunc ) : _pFunc(pFunc){}	
		inline _TRtn operator() (FUNC_V5(_TLst)) {	
			assert(_pFunc);
			_pFunc->invoke(FUNC_P5);
		}		
	protected:	
		IFuncPtr* _pFunc;
	};	
	struct FuncBase{		
		FuncBase(IFuncPtr* pFunc) : _pFunc(pFunc){}	
		_TRtn operator() (FUNC_V5(_TLst)) {		
			assert(_pFunc);
			return _pFunc->invoke(FUNC_P5) ;	
		}		
	protected:	
		IFuncPtr* _pFunc;	
	};
};


HEEZE_NAMESPACE_END

#endif

