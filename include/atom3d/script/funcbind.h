#ifndef __ATOM_SCRIPT_FUNCTBIND_H
#define __ATOM_SCRIPT_FUNCTBIND_H

#if _MSC_VER > 1000
# pragma once
#endif

#pragma warning(push)
#pragma warning(disable:4995)

#include <string>
#include <typeinfo>
#include <stdexcept>
#include "../ATOM_utils.h"
#include "callback.h"

template <class T>
struct ATOM_RemoveReferenceQualifier
{
  typedef typename ATOM_RemoveConstVolatile<typename ATOM_RemoveReference<T>::type>::type type;
};

template <class T> struct __ATOM_ParamHelper__;
template <class T> struct __ATOM_ReturnHelper__;
template <class T> struct __ATOM_HelpHelper__;

struct ATOM_InstanceOp
{
  virtual ATOM_InstanceOp *clone (void) const = 0;
  virtual void destroy (void) = 0;
  virtual unsigned getSize () const = 0;
  virtual void construct (void *p) const = 0;
  virtual void destruct (void *p) const = 0;
  virtual void copy_construct (void *dst, const void *src) const = 0;
  virtual void copy (void *dst, const void *src) const = 0;
};

template <class T, bool copyable>
struct ATOM_CopyconstructHelperT
{
  void copy_construct (void *dst, const void *src) const { new(dst) T(*((const T*)src)); }
};

template <class T>
struct ATOM_CopyconstructHelperT<T, false>
{
  void copy_construct (void *dst, const void *src) const { throw std::runtime_error("Couldn't copy noncopyable class");; }
};

template <class T, bool copyable, bool abstract>
struct ATOM_InstanceOpHelperT
{
  void construct (void *p) const { new(p) T; }
  void destruct (void *p) const { ((T*)p)->~T(); }
  void copy_construct (void *dst, const void *src) const { ATOM_CopyconstructHelperT<T, copyable>().copy_construct(dst, src); }
  void copy (void *dst, const void *src) const { if (dst != src) { destruct(dst); copy_construct (dst, src); } }
};

template <class T, bool copyable>
struct ATOM_InstanceOpHelperT<T, copyable, true>
{
  void construct (void *p) const { throw std::runtime_error("Couldn't instantiate abstract class"); }
  void destruct (void *p) const { ((T*)p)->~T(); }
  void copy_construct (void *dst, const void *src) const { throw std::runtime_error("Couldn't instantiate abstract class"); }
  void copy (void *dst, const void *src) const { throw std::runtime_error("Couldn't instantiate abstract class"); }
};

template <class T, bool copyable>
struct ATOM_InstanceOpT: public ATOM_InstanceOp
{
  typedef ATOM_InstanceOpHelperT<T, copyable, ATOM_IsAbstract<T>::result> helper_type;
  typedef ATOM_InstanceOpT<T, copyable> thistype;

  virtual ATOM_InstanceOp *clone (void) const { return ATOM_NEW(thistype); }
  virtual void destroy (void) { ATOM_DELETE(this); }
  virtual unsigned getSize () const { return sizeof(T); }
  virtual void construct (void *p) const { helper_type().construct(p); }
  virtual void destruct (void *p) const { helper_type().destruct(p); }
  virtual void copy_construct (void *dst, const void *src) const { helper_type().copy_construct(dst, src); }
  virtual void copy (void *dst, const void *src) const { helper_type().copy(dst, src); }
};

template <class T>
struct ATOM_InstanceOpTCopyable: public ATOM_InstanceOpT<T, true>
{
	typedef ATOM_InstanceOpT<T, true> basetype;
	typedef typename basetype::helper_type helper_type;
};

template <class T>
struct ATOM_InstanceOpTNonCopyable: public ATOM_InstanceOpT<T, false>
{
	typedef ATOM_InstanceOpT<T, false> basetype;
	typedef typename basetype::helper_type helper_type;
};

template <class T> struct ATOM_ReturnTraitsT
{
  static const bool isRegistered = false;
  static inline const char *getClassName (T*) { return 0; }
};

template <class T> 
struct __ATOM_HelpHelper__
{
  const char *operator () () const {
    return typeid(T).name();
  }
};
/*
template <> struct __ATOM_HelpHelper__<char>
{
  const char *operator () () const {
    return "char";
  }
};

template <> struct __ATOM_HelpHelper__<signed char>
{
  const char *operator () () const {
    return "signed char";
  }
};

template <> struct __ATOM_HelpHelper__<unsigned char>
{
  const char *operator () () const {
    return "unsigned char";
  }
};

template <> struct __ATOM_HelpHelper__<short>
{
  const char *operator () () const {
    return "short";
  }
};

template <> struct __ATOM_HelpHelper__<unsigned short>
{
  const char *operator () () const {
    return "unsigned short";
  }
};

template <> struct __ATOM_HelpHelper__<int>
{
  const char *operator () () const {
    return "int";
  }
};

template <> struct __ATOM_HelpHelper__<unsigned int>
{
  const char * operator () () const {
    return "unsigned int";
  }
};

template <> struct __ATOM_HelpHelper__<long>
{
  const char *operator () () const {
    return "long";
  }
};

template <> struct __ATOM_HelpHelper__<unsigned long>
{
  const char *operator () () const {
    return "unsigned long";
  }
};

template <> struct __ATOM_HelpHelper__<long long>
{
  const char *operator () () const {
    return "long long";
  }
};

template <> struct __ATOM_HelpHelper__<unsigned long long>
{
  const char * operator () () const {
    return "unsigned long long";
  }
};

template <> struct __ATOM_HelpHelper__<float>
{
  const char *operator () () const {
    return "float";
  }
};

template <> struct __ATOM_HelpHelper__<double>
{
  const char *operator () () const {
    return "double";
  }
};

template <> struct __ATOM_HelpHelper__<bool>
{
  const char *operator () () const {
    return "bool";
  }
};

template <> struct __ATOM_HelpHelper__<const char*>
{
  const char *operator () () const {
    return "const char*";
  }
};

template <> struct __ATOM_HelpHelper__<ATOM_ScriptVariant>
{
  const char *operator () () const {
    return "variant";
  }
};
*/
  template <class Alloc> struct __ATOM_HelpHelper__<std::basic_string<char, std::char_traits<char>, Alloc> >
{
  typedef std::basic_string<char, std::char_traits<char>, Alloc> string_type;
  const char *operator () () const {
    return "string";
  }
};
/*
template <class T> struct __ATOM_HelpHelper__<T*>
{
  const char *operator () () const {
    return "pointer";
  }
};
*/
template <> struct __ATOM_ParamHelper__<char>
{
  char operator () (ATOM_Script *scp, int index) {
    return static_cast<char> (scp->getParameteri (index));
  }
};

template <> struct __ATOM_ParamHelper__<signed char>
{
  signed char operator () (ATOM_Script *scp, int index) {
    return static_cast<signed char> (scp->getParameteri (index));
  }
};

template <> struct __ATOM_ParamHelper__<unsigned char>
{
  unsigned char operator () (ATOM_Script *scp, int index) {
    return static_cast<unsigned char> (scp->getParameteri (index));
  }
};

template <> struct __ATOM_ParamHelper__<short>
{
  short operator () (ATOM_Script *scp, int index) {
    return static_cast<short> (scp->getParameteri (index));
  }
};

template <> struct __ATOM_ParamHelper__<unsigned short>
{
  unsigned short operator () (ATOM_Script *scp, int index) {
    return static_cast<unsigned short> (scp->getParameteri (index));
  }
};

template <> struct __ATOM_ParamHelper__<int>
{
  int operator () (ATOM_Script *scp, int index) {
    return scp->getParameteri (index);
  }
};

template <> struct __ATOM_ParamHelper__<unsigned int>
{
  unsigned int operator () (ATOM_Script *scp, int index) {
    return scp->getParameterui (index);
  }
};

template <> struct __ATOM_ParamHelper__<long>
{
  long operator () (ATOM_Script *scp, int index) {
    return scp->getParameteri (index);
  }
};

template <> struct __ATOM_ParamHelper__<unsigned long>
{
  unsigned long operator () (ATOM_Script *scp, int index) {
    return scp->getParameterui (index);
  }
};

template <> struct __ATOM_ParamHelper__<long long>
{
  long long operator () (ATOM_Script *scp, int index) {
    return scp->getParameteri (index);
  }
};

template <> struct __ATOM_ParamHelper__<unsigned long long>
{
  unsigned long long operator () (ATOM_Script *scp, int index) {
    return scp->getParameterui (index);
  }
};

template <> struct __ATOM_ParamHelper__<float>
{
  float operator () (ATOM_Script *scp, int index) {
    return scp->getParameterf (index);
  }
};

template <> struct __ATOM_ParamHelper__<double>
{
  double operator () (ATOM_Script *scp, int index) {
    return scp->getParameterf (index);
  }
};

template <> struct __ATOM_ParamHelper__<bool>
{
  bool operator () (ATOM_Script *scp, int index) {
    return scp->getParameterb (index);
  }
};

template <> struct __ATOM_ParamHelper__<const char*>
{
  const char *operator () (ATOM_Script *scp, int index) {
    return scp->getParameters (index);
  }
};

template <> struct __ATOM_ParamHelper__<ATOM_ScriptVariant>
{
  ATOM_ScriptVariant operator () (ATOM_Script *scp, int index) {
    return scp->getParameterv (index);
  }
};

  template <class Alloc> struct __ATOM_ParamHelper__<std::basic_string<char, std::char_traits<char>, Alloc> >
{
  typedef std::basic_string<char, std::char_traits<char>, Alloc> string_type;
  string_type operator () (ATOM_Script *scp, int index) {
    return scp->getParameters (index);
  }
};

template <class T> struct __ATOM_ParamHelper__<T*>
{
  T *operator () (ATOM_Script *scp, int index) {
    return static_cast<T*>(scp->getParameterp (index));
  }
};

namespace
{
  template <class T>
  struct __FunctionParamHelper__
  {
    T operator () (ATOM_Script *scp, int index) {
      int type = scp->getParameterType (index);
      if (type == ATOM_Script::TYPE_STRING)
      {
        const char *fn = scp->getParameters (index);
        return (T)scp->allocDynamicFunction(fn, scp, &ATOM_ScriptCallbackBridge::SetCallback, &ATOM_ScriptCallbackT<T>::execute);
      }
      else if (type == ATOM_Script::TYPE_POINTER)
      {
        return (T)scp->getParameterp (index);
      }
      else if (type == ATOM_Script::TYPE_NIL)
      {
        return (T)0;
      }
      else
      {
        throw std::runtime_error ("Callback function parameter must be string or pointer or nil");
      }
    }
  };
}

template <class Tr> 
struct __ATOM_ParamHelper__<Tr (*)(void)>
  : public __FunctionParamHelper__<void (*)(void)>
{
};

template <class Tr, class T1> 
struct __ATOM_ParamHelper__<Tr (*)(T1)>
  : public __FunctionParamHelper__<Tr (*)(T1)>
{
};

template <class Tr, class T1, class T2> 
struct __ATOM_ParamHelper__<Tr (*)(T1, T2)>
  : public __FunctionParamHelper__<Tr (*)(T1, T2)>
{
};

template <class Tr, class T1, class T2, class T3> 
struct __ATOM_ParamHelper__<Tr (*)(T1, T2, T3)>
  : public __FunctionParamHelper__<Tr (*)(T1, T2, T3)>
{
};

template <class Tr, class T1, class T2, class T3, class T4> 
struct __ATOM_ParamHelper__<Tr (*)(T1, T2, T3, T4)>
  : public __FunctionParamHelper__<Tr (*)(T1, T2, T3, T4)>
{
};

template <class Tr, class T1, class T2, class T3, class T4, class T5> 
struct __ATOM_ParamHelper__<Tr (*)(T1, T2, T3, T4, T5)>
  : public __FunctionParamHelper__<Tr (*)(T1, T2, T3, T4, T5)>
{
};

template <class Tr, class T1, class T2, class T3, class T4, class T5, class T6> 
struct __ATOM_ParamHelper__<Tr (*)(T1, T2, T3, T4, T5, T6)>
  : public __FunctionParamHelper__<Tr (*)(T1, T2, T3, T4, T5, T6)>
{
};

template <class Tr, class T1, class T2, class T3, class T4, class T5, class T6, class T7> 
struct __ATOM_ParamHelper__<Tr (*)(T1, T2, T3, T4, T5, T6, T7)>
  : public __FunctionParamHelper__<Tr (*)(T1, T2, T3, T4, T5, T6, T7)>
{
};

template <class Tr, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8> 
struct __ATOM_ParamHelper__<Tr (*)(T1, T2, T3, T4, T5, T6, T7, T8)>
  : public __FunctionParamHelper__<Tr (*)(T1, T2, T3, T4, T5, T6, T7, T8)>
{
};

template <class Tr, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9> 
struct __ATOM_ParamHelper__<Tr (*)(T1, T2, T3, T4, T5, T6, T7, T8, T9)>
  : public __FunctionParamHelper__<Tr (*)(T1, T2, T3, T4, T5, T6, T7, T8, T9)>
{
};

template <class Tr, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10> 
struct __ATOM_ParamHelper__<Tr (*)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10)>
  : public __FunctionParamHelper__<Tr (*)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10)>
{
};

template <class Tr, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11> 
struct __ATOM_ParamHelper__<Tr (*)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11)>
  : public __FunctionParamHelper__<Tr (*)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11)>
{
};

template <class Tr, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12> 
struct __ATOM_ParamHelper__<Tr (*)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12)>
  : public __FunctionParamHelper__<Tr (*)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12)>
{
};

template <class Tr, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13> 
struct __ATOM_ParamHelper__<Tr (*)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13)>
  : public __FunctionParamHelper__<Tr (*)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13)>
{
};

template <class Tr, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14> 
struct __ATOM_ParamHelper__<Tr (*)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14)>
  : public __FunctionParamHelper__<Tr (*)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14)>
{
};

template <class Tr, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15> 
struct __ATOM_ParamHelper__<Tr (*)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15)>
  : public __FunctionParamHelper__<Tr (*)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15)>
{
};

template <class T, bool IntType>
struct __ParamHelperHelper__
{
  T operator () (ATOM_Script *scp, int index) {
    return (T)scp->getParameteri (index);
  }
};

template <class T>
struct __ParamHelperHelper__<T, false>
{
  T operator () (ATOM_Script *scp, int index) {
    return T(*static_cast<T*>(scp->getParameterp (index)));
  }
};

template <class T> struct __ATOM_ParamHelper__: public __ParamHelperHelper__<T, ATOM_IsConvertible<T,int>::result>
{
};

template <> struct __ATOM_ReturnHelper__<char>
{
  int operator () (ATOM_Script *scp, char ret, ATOM_DereferenceFunc deref) {
    return scp->setReturni (ret);
  }
};

template <> struct __ATOM_ReturnHelper__<signed char>
{
  int operator () (ATOM_Script *scp, signed char ret, ATOM_DereferenceFunc deref) {
    return scp->setReturni (ret);
  }
};

template <> struct __ATOM_ReturnHelper__<unsigned char>
{
  int operator () (ATOM_Script *scp, unsigned char ret, ATOM_DereferenceFunc deref) {
    return scp->setReturni (ret);
  }
};

template <> struct __ATOM_ReturnHelper__<short>
{
  int operator () (ATOM_Script *scp, short ret, ATOM_DereferenceFunc deref) {
    return scp->setReturni (ret);
  }
};

template <> struct __ATOM_ReturnHelper__<unsigned short>
{
  int operator () (ATOM_Script *scp, unsigned short ret, ATOM_DereferenceFunc deref) {
    return scp->setReturni (ret);
  }
};

template <> struct __ATOM_ReturnHelper__<int>
{
  int operator () (ATOM_Script *scp, int ret, ATOM_DereferenceFunc deref) {
    return scp->setReturni (ret);
  }
};

template <> struct __ATOM_ReturnHelper__<unsigned int>
{
  int operator () (ATOM_Script *scp, unsigned int ret, ATOM_DereferenceFunc deref) {
    return scp->setReturnui (ret);
  }
};

template <> struct __ATOM_ReturnHelper__<long>
{
  int operator () (ATOM_Script *scp, long ret, ATOM_DereferenceFunc deref) {
    return scp->setReturni (ret);
  }
};

template <> struct __ATOM_ReturnHelper__<unsigned long>
{
  int operator () (ATOM_Script *scp, unsigned long ret, ATOM_DereferenceFunc deref) {
    return scp->setReturnui (ret);
  }
};

template <> struct __ATOM_ReturnHelper__<long long>
{
  int operator () (ATOM_Script *scp, long long ret, ATOM_DereferenceFunc deref) {
    return scp->setReturni (ret);
  }
};

template <> struct __ATOM_ReturnHelper__<unsigned long long>
{
  int operator () (ATOM_Script *scp, unsigned long long ret, ATOM_DereferenceFunc deref) {
    return scp->setReturnui (ret);
  }
};

template <> struct __ATOM_ReturnHelper__<float>
{
  int operator () (ATOM_Script *scp, float ret, ATOM_DereferenceFunc deref) {
    return scp->setReturnf (ret);
  }
};

template <> struct __ATOM_ReturnHelper__<double>
{
  int operator () (ATOM_Script *scp, double ret, ATOM_DereferenceFunc deref) {
    return scp->setReturnf (ret);
  }
};

template <> struct __ATOM_ReturnHelper__<bool>
{
  int operator () (ATOM_Script *scp, bool ret, ATOM_DereferenceFunc deref) {
    return scp->setReturnb (ret);
  }
};

template <> struct __ATOM_ReturnHelper__<const char*>
{
  int operator () (ATOM_Script *scp, const char *ret, ATOM_DereferenceFunc deref) {
    return scp->setReturns (ret);
  }
};

template <> struct __ATOM_ReturnHelper__<ATOM_ScriptVariant>
{
  int operator () (ATOM_Script *scp, const ATOM_ScriptVariant &ret, ATOM_DereferenceFunc deref) {
    return scp->setReturnv (ret, deref);
  }
};

  template <class Alloc> struct __ATOM_ReturnHelper__<std::basic_string<char, std::char_traits<char>, Alloc> >
{
  typedef std::basic_string<char, std::char_traits<char>, Alloc> string_type;
  int operator () (ATOM_Script *scp, const string_type &ret, ATOM_DereferenceFunc deref) {
    return scp->setReturns (ret.c_str());
  }
};

template <class T> struct __ATOM_ReturnHelper__<T*>
{
  int operator () (ATOM_Script *scp, T *ret, ATOM_DereferenceFunc deref) {
    return scp->setReturnp (ret, deref);
  }
};

template <class T> struct __ATOM_ReturnHelper__<const T*>
{
  int operator () (ATOM_Script *scp, const T *ret, ATOM_DereferenceFunc deref) {
    return scp->setReturnp (const_cast<T*>(ret), deref);
  }
};

template <class T> struct __ATOM_ReturnHelper__<T&>
{
  int operator () (ATOM_Script *scp, T &ret, ATOM_DereferenceFunc deref) {
    return scp->setReturnp (&ret, 0);
  }
  int operator () (ATOM_Script *scp, const T &ret, ATOM_DereferenceFunc deref) {
    return scp->setReturnp (const_cast<T*>(&ret), 0);
  }
};

template <class T> struct __ATOM_ReturnHelper__<const T&>
{
  int operator () (ATOM_Script *scp, const T &ret, ATOM_DereferenceFunc deref) {
    return scp->setReturnp (const_cast<T*>(&ret), 0);
  }
};

template <class T, bool IsInt>
struct __ReturnHelperHelper__
{
  int operator () (ATOM_Script *scp, const T &ret, ATOM_DereferenceFunc deref) {
    return scp->setReturni (int(ret));
  }
};

template <class T>
struct __ReturnHelperHelper__<T, false>
{
  int operator () (ATOM_Script *scp, const T &ret, ATOM_DereferenceFunc deref) {
    ATOM_InstanceOpTCopyable<T> op;
    return scp->setReturnpi (const_cast<T*>(&ret), &op);
  }
};

template <class T> struct __ATOM_ReturnHelper__: public __ReturnHelperHelper__<T, ATOM_IsConvertible<T, int>::result>
{
};

template <class T> struct __Param__: public __ATOM_ParamHelper__<typename ATOM_RemoveReferenceQualifier<T>::type> {};
template <class T> struct __Param__<T*>: public __ATOM_ParamHelper__<typename ATOM_RemoveReferenceQualifier<T>::type*> {};
template <> struct __Param__<const char*>: public __ATOM_ParamHelper__<const char*> {};
template <class T> struct __Help__: public __ATOM_HelpHelper__<typename ATOM_RemoveReferenceQualifier<T>::type> {};
template <class T> struct __Help__<T*>: public __ATOM_HelpHelper__<typename ATOM_RemoveReferenceQualifier<T>::type*> {};
template <> struct __Help__<const char*>: public __ATOM_HelpHelper__<const char*> {};
template <class T> struct __Return__: public __ATOM_ReturnHelper__<typename ATOM_RemoveReferenceQualifier<T>::type> {};
template <class T> struct __Return__<T*>: public __ATOM_ReturnHelper__<T*> {};
template <class T> struct __Return__<T&>: public __ATOM_ReturnHelper__<T&> {};
template <class T> struct __Return__<const T&>: public __ATOM_ReturnHelper__<T&> {};
template <> struct __Return__<const char*>: public __ATOM_ReturnHelper__<const char*> {};

struct ATOM_FunctionCallerBase {
  const char *_name;
  const char *_purename;
  ATOM_DereferenceFunc _deref;
  ATOM_FunctionCallerBase (const char *name, ATOM_DereferenceFunc deref): _name(name), _deref(deref) { 
    const char *p = strrchr (_name, '.');
    _purename = p ? p+1 : _name;
  }
  virtual int invoke (ATOM_Script *scp, void *context) = 0;
  virtual ~ATOM_FunctionCallerBase () {}
  virtual void helpstr (char *buffer) {}
};

struct ATOM_CommandCallerBase {
  const char *_name;
  const char *_purename;
  ATOM_DereferenceFunc _deref;
  bool _isAttrib;
  ATOM_CommandCallerBase (const char *name, ATOM_DereferenceFunc deref): _name(name), _deref(deref), _isAttrib(false) {
    const char *p = strrchr (_name, '.');
    _purename = p ? p+1 : _name;
  }
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) = 0;
  virtual ~ATOM_CommandCallerBase () {}
  virtual void helpstr (char *buffer) {}
};

struct ATOM_IndexCommandCallerBase {
  ATOM_DereferenceFunc _deref;
  ATOM_IndexCommandCallerBase (ATOM_DereferenceFunc deref): _deref(deref) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) = 0;
  virtual ~ATOM_IndexCommandCallerBase () {}
};

struct ATOM_NewIndexCommandCallerBase {
  ATOM_NewIndexCommandCallerBase () {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) = 0;
  virtual ~ATOM_NewIndexCommandCallerBase () {}
};

struct ATOM_AttributeAccessorBase: public ATOM_CommandCallerBase {
  ATOM_AttributeAccessorBase (const char *name, ATOM_DereferenceFunc deref): ATOM_CommandCallerBase(name, deref) {_isAttrib = true;}
  virtual int invokeWrite (void *pc, ATOM_Script *scp, void *context) = 0;
};

template <class T> struct ATOM_FunctionCallerBaseT: public ATOM_FunctionCallerBase {
  T func;
  ATOM_FunctionCallerBaseT (const char *name, ATOM_DereferenceFunc deref, T p): ATOM_FunctionCallerBase(name, deref), func(p) {}
};

template <class T> struct ATOM_CommandCallerBaseT: public ATOM_CommandCallerBase {
  T func;
  ATOM_CommandCallerBaseT (const char *name, ATOM_DereferenceFunc deref, T p): ATOM_CommandCallerBase(name, deref), func(p) {}
};

template <class T> struct ATOM_IndexCommandCallerBaseT: public ATOM_IndexCommandCallerBase {
  T func;
  ATOM_IndexCommandCallerBaseT (ATOM_DereferenceFunc deref, T p): ATOM_IndexCommandCallerBase(deref), func(p) {}
};

template <class T> struct ATOM_NewIndexCommandCallerBaseT: public ATOM_NewIndexCommandCallerBase {
  T func;
  ATOM_NewIndexCommandCallerBaseT (T p): ATOM_NewIndexCommandCallerBase(), func(p) {}
};

template <class TRead> struct ATOM_AttributeAccessorReadonlyBaseT: public ATOM_AttributeAccessorBase {
  TRead funcRead;
  ATOM_AttributeAccessorReadonlyBaseT (const char *name, ATOM_DereferenceFunc deref, TRead r): ATOM_AttributeAccessorBase(name, deref), funcRead(r) {}
};

template <class TRead, class TWrite> struct ATOM_AttributeAccessorBaseT: public ATOM_AttributeAccessorBase {
  TRead funcRead;
  TWrite funcWrite;
  ATOM_AttributeAccessorBaseT (const char *name, ATOM_DereferenceFunc deref, TRead r, TWrite w): ATOM_AttributeAccessorBase(name, deref), funcRead(r), funcWrite(w) {}
};

template <class T> struct ATOM_ConstantAccessorT: public ATOM_FunctionCallerBase {
  T value;
  ATOM_ConstantAccessorT (const char *name, ATOM_DereferenceFunc deref, T val): ATOM_FunctionCallerBase(name, deref), value(val) {}
  virtual int invoke (ATOM_Script *scp, void *context) {
    return __Return__<T>() (scp, value, this->_deref);
  }
};

template <class Tc> struct ATOM_FunctionCaller;
template <class Tc> struct ATOM_CommandCaller;
template <class Tc> struct ATOM_IndexCommandCaller;
template <class Tc> struct ATOM_NewIndexCommandCaller;
template <class Tc, class Tr> struct ATOM_AttributeAccessor;
template <class Tc> struct ATOM_AttributeAccessorReadonly;

#define __S_PARAM(t, i) __Param__< t > () (scp, i)

template <>
struct ATOM_FunctionCaller<void (*)(void)>: public ATOM_FunctionCallerBaseT<void (*)(void)>
{
  typedef void return_type;
  typedef void (*TFunc)(void);
  typedef ATOM_FunctionCallerBaseT<TFunc> base_type;
  ATOM_FunctionCaller (const char *name, TFunc func): base_type (name, 0, func) {}
  virtual int invoke (ATOM_Script *scp, void *context) {
    func ();
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (void)", this->_name);
  }
};

template <class T1>
struct ATOM_FunctionCaller<void (*)(T1)>: public ATOM_FunctionCallerBaseT<void (*)(T1)>
{
  typedef void return_type;
  typedef void (*TFunc)(T1);
  typedef ATOM_FunctionCallerBaseT<TFunc> base_type;
  ATOM_FunctionCaller (const char *name, TFunc func): base_type (name, 0, func) {}
  virtual int invoke (ATOM_Script *scp, void *context) {
    func (__S_PARAM(T1, 0));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s)", this->_name, __Help__<T1>()());
  }
};

template <class T1, class T2>
struct ATOM_FunctionCaller<void (*)(T1, T2)>: public ATOM_FunctionCallerBaseT<void (*)(T1, T2)>
{
  typedef void return_type;
  typedef void (*TFunc)(T1, T2);
  typedef ATOM_FunctionCallerBaseT<TFunc> base_type;
  ATOM_FunctionCaller (const char *name, TFunc func): base_type (name, 0, func) {}
  virtual int invoke (ATOM_Script *scp, void *context) {
    func (__S_PARAM(T1, 0), __S_PARAM(T2, 1));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s)", this->_name, __Help__<T1>()(), __Help__<T2>()());
  }
};

template <class T1, class T2, class T3>
struct ATOM_FunctionCaller<void (*)(T1, T2, T3)>: public ATOM_FunctionCallerBaseT<void (*)(T1, T2, T3)>
{
  typedef void return_type;
  typedef void (*TFunc)(T1, T2, T3);
  typedef ATOM_FunctionCallerBaseT<TFunc> base_type;
  ATOM_FunctionCaller (const char *name, TFunc func): base_type (name, 0, func) {}
  virtual int invoke (ATOM_Script *scp, void *context) {
    func (__S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s)", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()());
  }
};

template <class T1, class T2, class T3, class T4>
struct ATOM_FunctionCaller<void (*)(T1, T2, T3, T4)>: public ATOM_FunctionCallerBaseT<void (*)(T1, T2, T3, T4)>
{
  typedef void return_type;
  typedef void (*TFunc)(T1, T2, T3, T4);
  typedef ATOM_FunctionCallerBaseT<TFunc> base_type;
  ATOM_FunctionCaller (const char *name, TFunc func): base_type (name, 0, func) {}
  virtual int invoke (ATOM_Script *scp, void *context) {
    func (__S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2), __S_PARAM(T4, 3));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s,%s)", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()());
  }
};

template <class T1, class T2, class T3, class T4, class T5>
struct ATOM_FunctionCaller<void (*)(T1, T2, T3, T4, T5)>: public ATOM_FunctionCallerBaseT<void (*)(T1, T2, T3, T4, T5)>
{
  typedef void return_type;
  typedef void (*TFunc)(T1, T2, T3, T4, T5);
  typedef ATOM_FunctionCallerBaseT<TFunc> base_type;
  ATOM_FunctionCaller (const char *name, TFunc func): base_type (name, 0, func) {}
  virtual int invoke (ATOM_Script *scp, void *context) {
    func (__S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2), __S_PARAM(T4, 3), __S_PARAM(T5, 4));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s,%s,%s)", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()());
  }
};

template <class T1, class T2, class T3, class T4, class T5, class T6>
struct ATOM_FunctionCaller<void (*)(T1, T2, T3, T4, T5, T6)>: public ATOM_FunctionCallerBaseT<void (*)(T1, T2, T3, T4, T5, T6)>
{
  typedef void return_type;
  typedef void (*TFunc)(T1, T2, T3, T4, T5, T6);
  typedef ATOM_FunctionCallerBaseT<TFunc> base_type;
  ATOM_FunctionCaller (const char *name, TFunc func): base_type (name, 0, func) {}
  virtual int invoke (ATOM_Script *scp, void *context) {
    func (__S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2), __S_PARAM(T4, 3), __S_PARAM(T5, 4), 
          __S_PARAM(T6, 5));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s,%s,%s,%s)", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()());
  }
};

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7>
struct ATOM_FunctionCaller<void (*)(T1, T2, T3, T4, T5, T6, T7)>: public ATOM_FunctionCallerBaseT<void (*)(T1, T2, T3, T4, T5, T6, T7)>
{
  typedef void return_type;
  typedef void (*TFunc)(T1, T2, T3, T4, T5, T6, T7);
  typedef ATOM_FunctionCallerBaseT<TFunc> base_type;
  ATOM_FunctionCaller (const char *name, TFunc func): base_type (name, 0, func) {}
  virtual int invoke (ATOM_Script *scp, void *context) {
    func (__S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2), __S_PARAM(T4, 3), __S_PARAM(T5, 4), 
          __S_PARAM(T6, 5), __S_PARAM(T7, 6));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s,%s,%s,%s,%s)", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()());
  }
};

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
struct ATOM_FunctionCaller<void (*)(T1, T2, T3, T4, T5, T6, T7, T8)>: public ATOM_FunctionCallerBaseT<void (*)(T1, T2, T3, T4, T5, T6, T7, T8)>
{
  typedef void return_type;
  typedef void (*TFunc)(T1, T2, T3, T4, T5, T6, T7, T8);
  typedef ATOM_FunctionCallerBaseT<TFunc> base_type;
  ATOM_FunctionCaller (const char *name, TFunc func): base_type (name, 0, func) {}
  virtual int invoke (ATOM_Script *scp, void *context) {
    func (__S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2), __S_PARAM(T4, 3), __S_PARAM(T5, 4),
          __S_PARAM(T6, 5), __S_PARAM(T7, 6), __S_PARAM(T8, 7));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s,%s,%s,%s,%s,%s)", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()());
  }
};

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
struct ATOM_FunctionCaller<void (*)(T1, T2, T3, T4, T5, T6, T7, T8, T9)>: public ATOM_FunctionCallerBaseT<void (*)(T1, T2, T3, T4, T5, T6, T7, T8, T9)>
{
  typedef void return_type;
  typedef void (*TFunc)(T1, T2, T3, T4, T5, T6, T7, T8, T9);
  typedef ATOM_FunctionCallerBaseT<TFunc> base_type;
  ATOM_FunctionCaller (const char *name, TFunc func): base_type (name, 0, func) {}
  virtual int invoke (ATOM_Script *scp, void *context) {
    func (__S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2), __S_PARAM(T4, 3), __S_PARAM(T5, 4),
          __S_PARAM(T6, 5), __S_PARAM(T7, 6), __S_PARAM(T8, 7), __S_PARAM(T9, 8));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s,%s,%s,%s,%s,%s,%s)", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()());
  }
};

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10>
struct ATOM_FunctionCaller<void (*)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10)>: public ATOM_FunctionCallerBaseT<void (*)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10)>
{
  typedef void return_type;
  typedef void (*TFunc)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10);
  typedef ATOM_FunctionCallerBaseT<TFunc> base_type;
  ATOM_FunctionCaller (const char *name, TFunc func): base_type (name, 0, func) {}
  virtual int invoke (ATOM_Script *scp, void *context) {
    func (__S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2), __S_PARAM(T4, 3), __S_PARAM(T5, 4),
          __S_PARAM(T6, 5), __S_PARAM(T7, 6), __S_PARAM(T8, 7), __S_PARAM(T9, 8), __S_PARAM(T10, 9));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()());
  }
};

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11>
struct ATOM_FunctionCaller<void (*)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11)>: public ATOM_FunctionCallerBaseT<void (*)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11)>
{
  typedef void return_type;
  typedef void (*TFunc)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11);
  typedef ATOM_FunctionCallerBaseT<TFunc> base_type;
  ATOM_FunctionCaller (const char *name, TFunc func): base_type (name, 0, func) {}
  virtual int invoke (ATOM_Script *scp, void *context) {
    func (__S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2), __S_PARAM(T4, 3), __S_PARAM(T5, 4),
          __S_PARAM(T6, 5), __S_PARAM(T7, 6), __S_PARAM(T8, 7), __S_PARAM(T9, 8), __S_PARAM(T10, 9),
          __S_PARAM(T11, 10));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()(), __Help__<T11>()());
  }
};

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12>
struct ATOM_FunctionCaller<void (*)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12)>: public ATOM_FunctionCallerBaseT<void (*)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12)>
{
  typedef void return_type;
  typedef void (*TFunc)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12);
  typedef ATOM_FunctionCallerBaseT<TFunc> base_type;
  ATOM_FunctionCaller (const char *name, TFunc func): base_type (name, 0, func) {}
  virtual int invoke (ATOM_Script *scp, void *context) {
    func (__S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2), __S_PARAM(T4, 3), __S_PARAM(T5, 4),
          __S_PARAM(T6, 5), __S_PARAM(T7, 6), __S_PARAM(T8, 7), __S_PARAM(T9, 8), __S_PARAM(T10, 9),
          __S_PARAM(T11, 10), __S_PARAM(T12, 11));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()(), __Help__<T11>()(),
      __Help__<T12>()());
  }
};

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13>
struct ATOM_FunctionCaller<void (*)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13)>: public ATOM_FunctionCallerBaseT<void (*)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13)>
{
  typedef void return_type;
  typedef void (*TFunc)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13);
  typedef ATOM_FunctionCallerBaseT<TFunc> base_type;
  ATOM_FunctionCaller (const char *name, TFunc func): base_type (name, 0, func) {}
  virtual int invoke (ATOM_Script *scp, void *context) {
    func (__S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2), __S_PARAM(T4, 3), __S_PARAM(T5, 4),
          __S_PARAM(T6, 5), __S_PARAM(T7, 6), __S_PARAM(T8, 7), __S_PARAM(T9, 8), __S_PARAM(T10, 9),
          __S_PARAM(T11, 10), __S_PARAM(T12, 11), __S_PARAM(T13, 12));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()(), __Help__<T11>()(),
      __Help__<T12>()(), __Help__<T13>()());
  }
};

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14>
struct ATOM_FunctionCaller<void (*)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14)>: public ATOM_FunctionCallerBaseT<void (*)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14)>
{
  typedef void return_type;
  typedef void (*TFunc)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14);
  typedef ATOM_FunctionCallerBaseT<TFunc> base_type;
  ATOM_FunctionCaller (const char *name, TFunc func): base_type (name, 0, func) {}
  virtual int invoke (ATOM_Script *scp, void *context) {
    func (__S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2), __S_PARAM(T4, 3), __S_PARAM(T5, 4),
          __S_PARAM(T6, 5), __S_PARAM(T7, 6), __S_PARAM(T8, 7), __S_PARAM(T9, 8), __S_PARAM(T10, 9),
          __S_PARAM(T11, 10), __S_PARAM(T12, 11), __S_PARAM(T13, 12), __S_PARAM(T14, 13));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()(), __Help__<T11>()(),
      __Help__<T12>()(), __Help__<T13>()(), __Help__<T14>()());
  }
};

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15>
struct ATOM_FunctionCaller<void (*)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15)>: public ATOM_FunctionCallerBaseT<void (*)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15)>
{
  typedef void return_type;
  typedef void (*TFunc)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15);
  typedef ATOM_FunctionCallerBaseT<TFunc> base_type;
  ATOM_FunctionCaller (const char *name, TFunc func): base_type (name, 0, func) {}
  virtual int invoke (ATOM_Script *scp, void *context) {
    func (__S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2), __S_PARAM(T4, 3), __S_PARAM(T5, 4),
          __S_PARAM(T6, 5), __S_PARAM(T7, 6), __S_PARAM(T8, 7), __S_PARAM(T9, 8), __S_PARAM(T10, 9),
          __S_PARAM(T11, 10), __S_PARAM(T12, 11), __S_PARAM(T13, 12), __S_PARAM(T14, 13), __S_PARAM(T15, 14));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()(), __Help__<T11>()(),
      __Help__<T12>()(), __Help__<T13>()(), __Help__<T14>()(), __Help__<T15>()());
  }
};

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16>
struct ATOM_FunctionCaller<void (*)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16)>: public ATOM_FunctionCallerBaseT<void (*)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16)>
{
  typedef void return_type;
  typedef void (*TFunc)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16);
  typedef ATOM_FunctionCallerBaseT<TFunc> base_type;
  ATOM_FunctionCaller (const char *name, TFunc func): base_type (name, 0, func) {}
  virtual int invoke (ATOM_Script *scp, void *context) {
    func (__S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2), __S_PARAM(T4, 3), __S_PARAM(T5, 4),
          __S_PARAM(T6, 5), __S_PARAM(T7, 6), __S_PARAM(T8, 7), __S_PARAM(T9, 8), __S_PARAM(T10, 9),
          __S_PARAM(T11, 10), __S_PARAM(T12, 11), __S_PARAM(T13, 12), __S_PARAM(T14, 13), __S_PARAM(T15, 14), __S_PARAM(T16, 15));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()(), __Help__<T11>()(),
      __Help__<T12>()(), __Help__<T13>()(), __Help__<T14>()(), __Help__<T15>()(), __Help__<T16>()());
  }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <class Tr>
struct ATOM_FunctionCaller<Tr (*)(void)>: public ATOM_FunctionCallerBaseT<Tr (*)(void)>
{
  typedef Tr return_type;
  typedef Tr (*TFunc)(void);
  typedef ATOM_FunctionCallerBaseT<TFunc> base_type;
  ATOM_FunctionCaller (const char *name, ATOM_DereferenceFunc deref, TFunc func): base_type (name, deref, func) {}
  virtual int invoke (ATOM_Script *scp, void *context) {
    return __Return__<Tr> () (scp, this->func (), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (void)", __Help__<Tr>()(), this->_name);
  }
};

template <class Tr, class T1>
struct ATOM_FunctionCaller<Tr (*)(T1)>: public ATOM_FunctionCallerBaseT<Tr (*)(T1)>
{
  typedef Tr return_type;
  typedef Tr (*TFunc)(T1);
  typedef ATOM_FunctionCallerBaseT<TFunc> base_type;
  ATOM_FunctionCaller (const char *name, ATOM_DereferenceFunc deref, TFunc func): base_type (name, deref, func) {}
  virtual int invoke (ATOM_Script *scp, void *context) {
    return __Return__<Tr> () (scp, func (__S_PARAM(T1, 0)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s)", __Help__<Tr>()(), this->_name, __Help__<T1>()());
  }
};

template <class Tr, class T1, class T2>
struct ATOM_FunctionCaller<Tr (*)(T1, T2)>: public ATOM_FunctionCallerBaseT<Tr (*)(T1, T2)>
{
  typedef Tr return_type;
  typedef Tr (*TFunc)(T1, T2);
  typedef ATOM_FunctionCallerBaseT<TFunc> base_type;
  ATOM_FunctionCaller (const char *name, ATOM_DereferenceFunc deref, TFunc func): base_type (name, deref, func) {}
  virtual int invoke (ATOM_Script *scp, void *context) {
    return __Return__<Tr> () (scp, func (__S_PARAM(T1, 0), __S_PARAM(T2, 1)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s)", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()());
  }
};

template <class Tr, class T1, class T2, class T3>
struct ATOM_FunctionCaller<Tr (*)(T1, T2, T3)>: public ATOM_FunctionCallerBaseT<Tr (*)(T1, T2, T3)>
{
  typedef Tr return_type;
  typedef Tr (*TFunc)(T1, T2, T3);
  typedef ATOM_FunctionCallerBaseT<TFunc> base_type;
  ATOM_FunctionCaller (const char *name, ATOM_DereferenceFunc deref, TFunc func): base_type (name, deref, func) {}
  virtual int invoke (ATOM_Script *scp, void *context) {
    return __Return__<Tr> () (scp, func (__S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s)", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()());
  }
};

template <class Tr, class T1, class T2, class T3, class T4>
struct ATOM_FunctionCaller<Tr (*)(T1, T2, T3, T4)>: public ATOM_FunctionCallerBaseT<Tr (*)(T1, T2, T3, T4)>
{
  typedef Tr return_type;
  typedef Tr (*TFunc)(T1, T2, T3, T4);
  typedef ATOM_FunctionCallerBaseT<TFunc> base_type;
  ATOM_FunctionCaller (const char *name, ATOM_DereferenceFunc deref, TFunc func): base_type (name, deref, func) {}
  virtual int invoke (ATOM_Script *scp, void *context) {
    return __Return__<Tr> () (scp, func (__S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2), __S_PARAM(T4, 3)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s,%s)", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()());
  }
};

template <class Tr, class T1, class T2, class T3, class T4, class T5>
struct ATOM_FunctionCaller<Tr (*)(T1, T2, T3, T4, T5)>: public ATOM_FunctionCallerBaseT<Tr (*)(T1, T2, T3, T4, T5)>
{
  typedef Tr return_type;
  typedef Tr (*TFunc)(T1, T2, T3, T4, T5);
  typedef ATOM_FunctionCallerBaseT<TFunc> base_type;
  ATOM_FunctionCaller (const char *name, ATOM_DereferenceFunc deref, TFunc func): base_type (name, deref, func) {}
  virtual int invoke (ATOM_Script *scp, void *context) {
    return __Return__<Tr> () (scp, func (__S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2), __S_PARAM(T4, 3), __S_PARAM(T5, 4)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s,%s,%s)", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()());
  }
};

template <class Tr, class T1, class T2, class T3, class T4, class T5, class T6>
struct ATOM_FunctionCaller<Tr (*)(T1, T2, T3, T4, T5, T6)>: public ATOM_FunctionCallerBaseT<Tr (*)(T1, T2, T3, T4, T5, T6)>
{
  typedef Tr return_type;
  typedef Tr (*TFunc)(T1, T2, T3, T4, T5, T6);
  typedef ATOM_FunctionCallerBaseT<TFunc> base_type;
  ATOM_FunctionCaller (const char *name, ATOM_DereferenceFunc deref, TFunc func): base_type (name, deref, func) {}
  virtual int invoke (ATOM_Script *scp, void *context) {
    return __Return__<Tr> () (scp, func (__S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2), __S_PARAM(T4, 3), __S_PARAM(T5, 4), 
                                                  __S_PARAM(T6, 5)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s,%s,%s,%s)", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()());
  }
};

template <class Tr, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
struct ATOM_FunctionCaller<Tr (*)(T1, T2, T3, T4, T5, T6, T7)>: public ATOM_FunctionCallerBaseT<Tr (*)(T1, T2, T3, T4, T5, T6, T7)>
{
  typedef Tr return_type;
  typedef Tr (*TFunc)(T1, T2, T3, T4, T5, T6, T7);
  typedef ATOM_FunctionCallerBaseT<TFunc> base_type;
  ATOM_FunctionCaller (const char *name, ATOM_DereferenceFunc deref, TFunc func): base_type (name, deref, func) {}
  virtual int invoke (ATOM_Script *scp, void *context) {
    return __Return__<Tr> () (scp, func (__S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2), __S_PARAM(T4, 3), __S_PARAM(T5, 4), 
                                                  __S_PARAM(T6, 5), __S_PARAM(T7, 6)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s,%s,%s,%s,%s)", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()());
  }
};

template <class Tr, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
struct ATOM_FunctionCaller<Tr (*)(T1, T2, T3, T4, T5, T6, T7, T8)>: public ATOM_FunctionCallerBaseT<Tr (*)(T1, T2, T3, T4, T5, T6, T7, T8)>
{
  typedef Tr return_type;
  typedef Tr (*TFunc)(T1, T2, T3, T4, T5, T6, T7, T8);
  typedef ATOM_FunctionCallerBaseT<TFunc> base_type;
  ATOM_FunctionCaller (const char *name, ATOM_DereferenceFunc deref, TFunc func): base_type (name, deref, func) {}
  virtual int invoke (ATOM_Script *scp, void *context) {
    return __Return__<Tr> () (scp, func (__S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2), __S_PARAM(T4, 3), __S_PARAM(T5, 4),
                                                  __S_PARAM(T6, 5), __S_PARAM(T7, 6), __S_PARAM(T8, 7)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s,%s,%s,%s,%s,%s)", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()());
  }
};

template <class Tr, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
struct ATOM_FunctionCaller<Tr (*)(T1, T2, T3, T4, T5, T6, T7, T8, T9)>: public ATOM_FunctionCallerBaseT<Tr (*)(T1, T2, T3, T4, T5, T6, T7, T8, T9)>
{
  typedef Tr return_type;
  typedef Tr (*TFunc)(T1, T2, T3, T4, T5, T6, T7, T8, T9);
  typedef ATOM_FunctionCallerBaseT<TFunc> base_type;
  ATOM_FunctionCaller (const char *name, ATOM_DereferenceFunc deref, TFunc func): base_type (name, deref, func) {}
  virtual int invoke (ATOM_Script *scp, void *context) {
    return __Return__<Tr> () (scp, func (__S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2), __S_PARAM(T4, 3), __S_PARAM(T5, 4),
                                                  __S_PARAM(T6, 5), __S_PARAM(T7, 6), __S_PARAM(T8, 7), __S_PARAM(T9, 8)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s,%s,%s,%s,%s,%s,%s)", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()());
  }
};

template <class Tr, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10>
struct ATOM_FunctionCaller<Tr (*)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10)>: public ATOM_FunctionCallerBaseT<Tr (*)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10)>
{
  typedef Tr return_type;
  typedef Tr (*TFunc)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10);
  typedef ATOM_FunctionCallerBaseT<TFunc> base_type;
  ATOM_FunctionCaller (const char *name, ATOM_DereferenceFunc deref, TFunc func): base_type (name, deref, func) {}
  virtual int invoke (ATOM_Script *scp, void *context) {
    return __Return__<Tr> () (scp, func (__S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2), __S_PARAM(T4, 3), __S_PARAM(T5, 4),
                                                  __S_PARAM(T6, 5), __S_PARAM(T7, 6), __S_PARAM(T8, 7), __S_PARAM(T9, 8), __S_PARAM(T10, 9)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()());
  }
};

template <class Tr, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11>
struct ATOM_FunctionCaller<Tr (*)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11)>: public ATOM_FunctionCallerBaseT<Tr (*)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11)>
{
  typedef Tr return_type;
  typedef Tr (*TFunc)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11);
  typedef ATOM_FunctionCallerBaseT<TFunc> base_type;
  ATOM_FunctionCaller (const char *name, ATOM_DereferenceFunc deref, TFunc func): base_type (name, deref, func) {}
  virtual int invoke (ATOM_Script *scp, void *context) {
    return __Return__<Tr> () (scp, func (__S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2), __S_PARAM(T4, 3), __S_PARAM(T5, 4),
                                         __S_PARAM(T6, 5), __S_PARAM(T7, 6), __S_PARAM(T8, 7), __S_PARAM(T9, 8), __S_PARAM(T10, 9),
                                         __S_PARAM(T11, 10)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()(), __Help__<T11>()());
  }
};

template <class Tr, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12>
struct ATOM_FunctionCaller<Tr (*)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12)>: public ATOM_FunctionCallerBaseT<Tr (*)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12)>
{
  typedef Tr return_type;
  typedef Tr (*TFunc)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12);
  typedef ATOM_FunctionCallerBaseT<TFunc> base_type;
  ATOM_FunctionCaller (const char *name, ATOM_DereferenceFunc deref, TFunc func): base_type (name, deref, func) {}
  virtual int invoke (ATOM_Script *scp, void *context) {
    return __Return__<Tr> () (scp, func (__S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2), __S_PARAM(T4, 3), __S_PARAM(T5, 4),
                                         __S_PARAM(T6, 5), __S_PARAM(T7, 6), __S_PARAM(T8, 7), __S_PARAM(T9, 8), __S_PARAM(T10, 9),
                                         __S_PARAM(T11, 10), __S_PARAM(T12, 11)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()(), __Help__<T11>()(),
      __Help__<T12>()());
  }
};

template <class Tr, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13>
struct ATOM_FunctionCaller<Tr (*)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13)>: public ATOM_FunctionCallerBaseT<Tr (*)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13)>
{
  typedef Tr return_type;
  typedef Tr (*TFunc)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13);
  typedef ATOM_FunctionCallerBaseT<TFunc> base_type;
  ATOM_FunctionCaller (const char *name, ATOM_DereferenceFunc deref, TFunc func): base_type (name, deref, func) {}
  virtual int invoke (ATOM_Script *scp, void *context) {
    return __Return__<Tr> () (scp, func (__S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2), __S_PARAM(T4, 3), __S_PARAM(T5, 4),
                                         __S_PARAM(T6, 5), __S_PARAM(T7, 6), __S_PARAM(T8, 7), __S_PARAM(T9, 8), __S_PARAM(T10, 9),
                                         __S_PARAM(T11, 10), __S_PARAM(T12, 11), __S_PARAM(T13, 12)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()(), __Help__<T11>()(),
      __Help__<T12>()(), __Help__<T13>()());
  }
};

template <class Tr, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14>
struct ATOM_FunctionCaller<Tr (*)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14)>: public ATOM_FunctionCallerBaseT<Tr (*)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14)>
{
  typedef Tr return_type;
  typedef Tr (*TFunc)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14);
  typedef ATOM_FunctionCallerBaseT<TFunc> base_type;
  ATOM_FunctionCaller (const char *name, ATOM_DereferenceFunc deref, TFunc func): base_type (name, deref, func) {}
  virtual int invoke (ATOM_Script *scp, void *context) {
    return __Return__<Tr> () (scp, func (__S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2), __S_PARAM(T4, 3), __S_PARAM(T5, 4),
                                         __S_PARAM(T6, 5), __S_PARAM(T7, 6), __S_PARAM(T8, 7), __S_PARAM(T9, 8), __S_PARAM(T10, 9),
                                         __S_PARAM(T11, 10), __S_PARAM(T12, 11), __S_PARAM(T13, 12), __S_PARAM(T14, 13)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()(), __Help__<T11>()(),
      __Help__<T12>()(), __Help__<T13>()(), __Help__<T14>()());
  }
};

template <class Tr, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15>
struct ATOM_FunctionCaller<Tr (*)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15)>: public ATOM_FunctionCallerBaseT<Tr (*)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15)>
{
  typedef Tr return_type;
  typedef Tr (*TFunc)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15);
  typedef ATOM_FunctionCallerBaseT<TFunc> base_type;
  ATOM_FunctionCaller (const char *name, ATOM_DereferenceFunc deref, TFunc func): base_type (name, deref, func) {}
  virtual int invoke (ATOM_Script *scp, void *context) {
    return __Return__<Tr> () (scp, func (__S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2), __S_PARAM(T4, 3), __S_PARAM(T5, 4),
                                         __S_PARAM(T6, 5), __S_PARAM(T7, 6), __S_PARAM(T8, 7), __S_PARAM(T9, 8), __S_PARAM(T10, 9),
                                         __S_PARAM(T11, 10), __S_PARAM(T12, 11), __S_PARAM(T13, 12), __S_PARAM(T14, 13), __S_PARAM(T15, 14)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()(), __Help__<T11>()(),
      __Help__<T12>()(), __Help__<T13>()(), __Help__<T14>()(), __Help__<T15>()());
  }
};

template <class Tr, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16>
struct ATOM_FunctionCaller<Tr (*)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16)>: public ATOM_FunctionCallerBaseT<Tr (*)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16)>
{
  typedef Tr return_type;
  typedef Tr (*TFunc)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16);
  typedef ATOM_FunctionCallerBaseT<TFunc> base_type;
  ATOM_FunctionCaller (const char *name, ATOM_DereferenceFunc deref, TFunc func): base_type (name, deref, func) {}
  virtual int invoke (ATOM_Script *scp, void *context) {
    return __Return__<Tr> () (scp, func (__S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2), __S_PARAM(T4, 3), __S_PARAM(T5, 4),
                                         __S_PARAM(T6, 5), __S_PARAM(T7, 6), __S_PARAM(T8, 7), __S_PARAM(T9, 8), __S_PARAM(T10, 9),
                                         __S_PARAM(T11, 10), __S_PARAM(T12, 11), __S_PARAM(T13, 12), __S_PARAM(T14, 13), __S_PARAM(T15, 14), __S_PARAM(T16, 15)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()(), __Help__<T11>()(),
      __Help__<T12>()(), __Help__<T13>()(), __Help__<T14>()(), __Help__<T15>()(), __Help__<T16>()());
  }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <class TcR, class TrR, class TcW, class TrW>
struct ATOM_AttributeAccessor<TrR (*)(TcR*), void (*)(TcW*, TrW)>: public ATOM_AttributeAccessorBaseT<TrR (*)(TcR*), void (*)(TcW*, TrW)>
{
  typedef TrR (*read_func)(TcR*);
  typedef void (*write_func)(TcW*, TrW);
  typedef ATOM_AttributeAccessorBaseT<read_func, write_func> base_type;
  ATOM_AttributeAccessor (const char *name, ATOM_DereferenceFunc deref, read_func r, write_func w): base_type(name, deref, r, w) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<TrR>() (scp, funcRead ((TcR*)pc), this->_deref);
  }
  virtual int invokeWrite (void *pc, ATOM_Script *scp, void *context) {
    funcWrite ((TcW*)pc, __S_PARAM(TrW, 0));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s", __Help__<TrR>()(), this->_name);
  }
};

template <class TcR, class TrR, class TcW, class TrW>
struct ATOM_AttributeAccessor<TrR (*)(TcR*), void (TcW::*)(TrW)>: public ATOM_AttributeAccessorBaseT<TrR (*)(TcR*), void (TcW::*)(TrW)>
{
  typedef TrR (*read_func)(TcR*);
  typedef void (TcW::*write_func)(TrW);
  typedef ATOM_AttributeAccessorBaseT<read_func, write_func> base_type;
  ATOM_AttributeAccessor (const char *name, ATOM_DereferenceFunc deref, read_func r, write_func w): base_type(name, deref, r, w) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<TrR>() (scp, funcRead ((TcR*)pc), this->_deref);
  }
  virtual int invokeWrite (void *pc, ATOM_Script *scp, void *context) {
    (((TcW*)pc)->*(this->funcWrite)) (__S_PARAM(TrW, 0));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s", __Help__<TrR>()(), this->_name);
  }
};

template <class TcR, class TrR, class TcW, class TrW>
struct ATOM_AttributeAccessor<TrR (*)(TcR*), void (TcW::*)(TrW) const>: public ATOM_AttributeAccessorBaseT<TrR (*)(TcR*), void (TcW::*)(TrW) const>
{
  typedef TrR (*read_func)(TcR*);
  typedef void (TcW::*write_func)(TrW) const;
  typedef ATOM_AttributeAccessorBaseT<read_func, write_func> base_type;
  ATOM_AttributeAccessor (const char *name, ATOM_DereferenceFunc deref, read_func r, write_func w): base_type(name, deref, r, w) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<TrR>() (scp, funcRead ((TcR*)pc), this->_deref);
  }
  virtual int invokeWrite (void *pc, ATOM_Script *scp, void *context) {
    (((const TcW*)pc)->*(this->funcWrite)) (__S_PARAM(TrW, 0));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s", __Help__<TrR>()(), this->_name);
  }
};

template <class TcR, class TrR, class TcW, class TrW>
struct ATOM_AttributeAccessor<TrR (TcR::*)(void), void (*)(TcW*, TrW)>: public ATOM_AttributeAccessorBaseT<TrR (TcR::*)(void), void (*)(TcW*, TrW)>
{
  typedef TrR (TcR::*read_func)(void);
  typedef void (*write_func)(TcW*, TrW);
  typedef ATOM_AttributeAccessorBaseT<read_func, write_func> base_type;
  ATOM_AttributeAccessor (const char *name, ATOM_DereferenceFunc deref, read_func r, write_func w): base_type(name, deref, r, w) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<TrR>() (scp, (((TcR*)pc)->*(this->funcRead))(), this->_deref);
  }
  virtual int invokeWrite (void *pc, ATOM_Script *scp, void *context) {
    funcWrite ((TcW*)pc, __S_PARAM(TrW, 0));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s", __Help__<TrR>()(), this->_name);
  }
};

template <class TcR, class TrR, class TcW, class TrW>
struct ATOM_AttributeAccessor<TrR (TcR::*)(void) const, void (*)(TcW*, TrW)>: public ATOM_AttributeAccessorBaseT<TrR (TcR::*)(void) const, void (*)(TcW*, TrW)>
{
  typedef TrR (TcR::*read_func)(void) const;
  typedef void (*write_func)(TcW*, TrW);
  typedef ATOM_AttributeAccessorBaseT<read_func, write_func> base_type;
  ATOM_AttributeAccessor (const char *name, ATOM_DereferenceFunc deref, read_func r, write_func w): base_type(name, deref, r, w) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<TrR>() (scp, (((TcR*)pc)->*(this->funcRead))(), this->_deref);
  }
  virtual int invokeWrite (void *pc, ATOM_Script *scp, void *context) {
    funcWrite ((TcW*)pc, __S_PARAM(TrW, 0));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s", __Help__<TrR>()(), this->_name);
  }
};

template <class TcR, class TrR, class TcW, class TrW>
struct ATOM_AttributeAccessor<TrR (TcR::*)(void), void (TcW::*)(TrW)>: public ATOM_AttributeAccessorBaseT<TrR (TcR::*)(void), void (TcW::*)(TrW)>
{
  typedef TrR (TcR::*read_func)(void);
  typedef void (TcW::*write_func)(TrW);
  typedef ATOM_AttributeAccessorBaseT<read_func, write_func> base_type;
  ATOM_AttributeAccessor (const char *name, ATOM_DereferenceFunc deref, read_func r, write_func w): base_type(name, deref, r, w) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<TrR>() (scp, (((TcR*)pc)->*(this->funcRead))(), this->_deref);
  }
  virtual int invokeWrite (void *pc, ATOM_Script *scp, void *context) {
    (((TcW*)pc)->*(this->funcWrite)) (__S_PARAM(TrW, 0));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s", __Help__<TrR>()(), this->_name);
  }
};

template <class TcR, class TrR, class TcW, class TrW>
struct ATOM_AttributeAccessor<TrR (TcR::*)(void) const, void (TcW::*)(TrW)>: public ATOM_AttributeAccessorBaseT<TrR (TcR::*)(void) const, void (TcW::*)(TrW)>
{
  typedef TrR (TcR::*read_func)(void) const;
  typedef void (TcW::*write_func)(TrW);
  typedef ATOM_AttributeAccessorBaseT<read_func, write_func> base_type;
  ATOM_AttributeAccessor (const char *name, ATOM_DereferenceFunc deref, read_func r, write_func w): base_type(name, deref, r, w) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<TrR>() (scp, (((TcR*)pc)->*(this->funcRead))(), this->_deref);
  }
  virtual int invokeWrite (void *pc, ATOM_Script *scp, void *context) {
    (((TcW*)pc)->*(this->funcWrite)) (__S_PARAM(TrW, 0));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s", __Help__<TrR>()(), this->_name);
  }
};

template <class Tc, class Tr>
struct ATOM_AttributeAccessorReadonly<Tr (*)(Tc*)>: public ATOM_AttributeAccessorReadonlyBaseT<Tr (*)(Tc*)>
{
  typedef Tr (*read_func)(Tc*);
  typedef ATOM_AttributeAccessorReadonlyBaseT<read_func> base_type;
  ATOM_AttributeAccessorReadonly (const char *name, ATOM_DereferenceFunc deref, read_func r): base_type(name, deref, r) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>() (scp, funcRead((Tc*)pc), this->_deref);
  }
  virtual int invokeWrite (void *pc, ATOM_Script *scp, void *context) {
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s", __Help__<Tr>()(), this->_name);
  }
};

template <class Tc, class Tr>
struct ATOM_AttributeAccessorReadonly<Tr (Tc::*)(void)>: public ATOM_AttributeAccessorReadonlyBaseT<Tr (Tc::*)(void)>
{
  typedef Tr (Tc::*read_func)(void);
  typedef ATOM_AttributeAccessorReadonlyBaseT<read_func> base_type;
  ATOM_AttributeAccessorReadonly (const char *name, ATOM_DereferenceFunc deref, read_func r): base_type(name, deref, r) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>() (scp, (((Tc*)pc)->*(this->funcRead))(), this->_deref);
  }
  virtual int invokeWrite (void *pc, ATOM_Script *scp, void *context) {
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s", __Help__<Tr>()(), this->_name);
  }
};

template <class Tc, class Tr>
struct ATOM_AttributeAccessorReadonly<Tr (Tc::*)(void) const>: public ATOM_AttributeAccessorReadonlyBaseT<Tr (Tc::*)(void) const>
{
  typedef Tr (Tc::*read_func)(void) const;
  typedef ATOM_AttributeAccessorReadonlyBaseT<read_func> base_type;
  ATOM_AttributeAccessorReadonly (const char *name, ATOM_DereferenceFunc deref, read_func r): base_type(name, deref, r) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>() (scp, (((Tc*)pc)->*(this->funcRead))(), this->_deref);
  }
  virtual int invokeWrite (void *pc, ATOM_Script *scp, void *context) {
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s", __Help__<Tr>()(), this->_name);
  }
};

template <class Tc, class Tr>
struct ATOM_NewIndexCommandCaller<void (*)(Tc*, int, Tr)>: public ATOM_NewIndexCommandCallerBaseT<void (*)(Tc*, int, Tr)>
{
  typedef void return_type;
  typedef ATOM_NewIndexCommandCallerBaseT<void (*)(Tc*, int, Tr)> base_type;
  ATOM_NewIndexCommandCaller (void (*p)(Tc*, int, Tr)): base_type(p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    func ((Tc*)pc, __S_PARAM(int, 0), __S_PARAM(Tr, 1));
    return 0;
  }
};

template <class Tc, class Tr>
struct ATOM_NewIndexCommandCaller<void (Tc::*)(int, Tr)>: public ATOM_NewIndexCommandCallerBaseT<void (Tc::*)(int, Tr)>
{
  typedef void return_type;
  typedef ATOM_NewIndexCommandCallerBaseT<void (Tc::*)(int, Tr)> base_type;
  ATOM_NewIndexCommandCaller (void (Tc::*p)(int, Tr)): base_type(p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    (((Tc*)pc)->*(this->func)) (__S_PARAM(int, 0), __S_PARAM(Tr, 1));
    return 0;
  }
};

template <class Tc, class Tr>
struct ATOM_NewIndexCommandCaller<void (Tc::*)(int, Tr) const>: public ATOM_NewIndexCommandCallerBaseT<void (Tc::*)(int, Tr) const>
{
  typedef void return_type;
  typedef ATOM_NewIndexCommandCallerBaseT<void (Tc::*)(int, Tr) const> base_type;
  ATOM_NewIndexCommandCaller (void (Tc::*p)(int, Tr) const): base_type(p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    (((const Tc*)pc)->*(this->func)) (__S_PARAM(int, 0), __S_PARAM(Tr, 1));
    return 0;
  }
};


template <class Tc, class Tr>
struct ATOM_IndexCommandCaller<Tr (*)(Tc*, int)>: public ATOM_IndexCommandCallerBaseT<Tr (*)(Tc*, int)>
{
  typedef Tr return_type;
  typedef ATOM_IndexCommandCallerBaseT<Tr (*)(Tc*, int)> base_type;
  ATOM_IndexCommandCaller (ATOM_DereferenceFunc deref, Tr (*p)(Tc*, int)): base_type(deref, p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>() (scp, func ((Tc*)pc, __S_PARAM(int, 0)), this->_deref);
  }
};

template <class Tc, class Tr>
struct ATOM_IndexCommandCaller<Tr (Tc::*)(int)>: public ATOM_IndexCommandCallerBaseT<Tr (Tc::*)(int)>
{
  typedef Tr return_type;
  typedef ATOM_IndexCommandCallerBaseT<Tr (Tc::*)(int)> base_type;
  ATOM_IndexCommandCaller (ATOM_DereferenceFunc deref, Tr (Tc::*p)(int)): base_type(deref, p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>() (scp, (((Tc*)pc)->*(this->func)) (__S_PARAM(int, 0)), this->_deref);
  }
};

template <class Tc, class Tr>
struct ATOM_IndexCommandCaller<Tr (Tc::*)(int) const>: public ATOM_IndexCommandCallerBaseT<Tr (Tc::*)(int) const>
{
  typedef Tr return_type;
  typedef ATOM_IndexCommandCallerBaseT<Tr (Tc::*)(int) const> base_type;
  ATOM_IndexCommandCaller (ATOM_DereferenceFunc deref, Tr (Tc::*p)(int) const): base_type(deref, p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>() (scp, (((const Tc*)pc)->*(this->func)) (__S_PARAM(int, 0)), this->_deref);
  }
};

template <class Tc>
struct ATOM_CommandCaller<void (*)(Tc*)>: public ATOM_CommandCallerBaseT<void (*)(Tc*)>
{
  typedef void return_type;
  typedef ATOM_CommandCallerBaseT<void (*)(Tc*)> base_type;
  ATOM_CommandCaller (const char *name, void (*p)(Tc*)):base_type(name,0,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    func ((Tc*)pc);
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (void)", this->_name);
  }
};

template <class Tc, class T1>
struct ATOM_CommandCaller<void (*)(Tc*, T1)>: public ATOM_CommandCallerBaseT<void (*)(Tc*, T1)>
{
  typedef void return_type;
  typedef ATOM_CommandCallerBaseT<void (*)(Tc*, T1)> base_type;
  ATOM_CommandCaller (const char *name, void (*p)(Tc*, T1)):base_type(name,0,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    func ((Tc*)pc, __S_PARAM(T1, 0));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s)", this->_name, __Help__<T1>()());
  }
};

template <class Tc, class T1, class T2>
struct ATOM_CommandCaller<void (*)(Tc*, T1, T2)>: public ATOM_CommandCallerBaseT<void (*)(Tc*, T1, T2)>
{
  typedef void return_type;
  typedef ATOM_CommandCallerBaseT<void (*)(Tc*, T1, T2)> base_type;
  ATOM_CommandCaller (const char *name, void (*p)(Tc*, T1, T2)):base_type(name,0,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    func ((Tc*)pc, __S_PARAM(T1, 0), __S_PARAM(T2, 1));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s)", this->_name, __Help__<T1>()(), __Help__<T2>()());
  }
};

template <class Tc, class T1, class T2, class T3>
struct ATOM_CommandCaller<void (*)(Tc*, T1, T2, T3)>: public ATOM_CommandCallerBaseT<void (*)(Tc*, T1, T2, T3)>
{
  typedef void return_type;
  typedef ATOM_CommandCallerBaseT<void (*)(Tc*, T1, T2, T3)> base_type;
  ATOM_CommandCaller (const char *name, void (*p)(Tc*, T1, T2, T3)):base_type(name,0,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    func ((Tc*)pc, __S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s)", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()());
  }
};

template <class Tc, class T1, class T2, class T3, class T4>
struct ATOM_CommandCaller<void (*)(Tc*, T1, T2, T3, T4)>: public ATOM_CommandCallerBaseT<void (*)(Tc*, T1, T2, T3, T4)>
{
  typedef void return_type;
  typedef ATOM_CommandCallerBaseT<void (*)(Tc*, T1, T2, T3, T4)> base_type;
  ATOM_CommandCaller (const char *name, void (*p)(Tc*, T1, T2, T3, T4)):base_type(name,0,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    func ((Tc*)pc, __S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2), __S_PARAM(T4, 3));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s,%s)", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()());
  }
};

template <class Tc, class T1, class T2, class T3, class T4, class T5>
struct ATOM_CommandCaller<void (*)(Tc*, T1, T2, T3, T4, T5)>: public ATOM_CommandCallerBaseT<void (*)(Tc*, T1, T2, T3, T4, T5)>
{
  typedef void return_type;
  typedef ATOM_CommandCallerBaseT<void (*)(Tc*, T1, T2, T3, T4, T5)> base_type;
  ATOM_CommandCaller (const char *name, void (*p)(Tc*, T1, T2, T3, T4, T5)):base_type(name,0,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    func ((Tc*)pc, __S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2), __S_PARAM(T4, 3), __S_PARAM(T5, 4));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s,%s,%s)", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()());
  }
};

template <class Tc, class T1, class T2, class T3, class T4, class T5, class T6>
struct ATOM_CommandCaller<void (*)(Tc*, T1, T2, T3, T4, T5, T6)>: public ATOM_CommandCallerBaseT<void (*)(Tc*, T1, T2, T3, T4, T5, T6)>
{
  typedef void return_type;
  typedef ATOM_CommandCallerBaseT<void (*)(Tc*, T1, T2, T3, T4, T5, T6)> base_type;
  ATOM_CommandCaller (const char *name, void (*p)(Tc*, T1, T2, T3, T4, T5, T6)):base_type(name,0,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    func ((Tc*)pc, __S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2), __S_PARAM(T4, 3), __S_PARAM(T5, 4), 
                   __S_PARAM(T6, 5));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s,%s,%s,%s)", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()());
  }
};

template <class Tc, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
struct ATOM_CommandCaller<void (*)(Tc*, T1, T2, T3, T4, T5, T6, T7)>: public ATOM_CommandCallerBaseT<void (*)(Tc*, T1, T2, T3, T4, T5, T6, T7)>
{
  typedef void return_type;
  typedef ATOM_CommandCallerBaseT<void (*)(Tc*, T1, T2, T3, T4, T5, T6, T7)> base_type;
  ATOM_CommandCaller (const char *name, void (*p)(Tc*, T1, T2, T3, T4, T5, T6, T7)):base_type(name,0,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    func ((Tc*)pc, __S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2), __S_PARAM(T4, 3), __S_PARAM(T5, 4), 
                   __S_PARAM(T6, 5), __S_PARAM(T7, 6));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s,%s,%s,%s,%s)", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()());
  }
};

template <class Tc, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
struct ATOM_CommandCaller<void (*)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8)>: public ATOM_CommandCallerBaseT<void (*)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8)>
{
  typedef void return_type;
  typedef ATOM_CommandCallerBaseT<void (*)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8)> base_type;
  ATOM_CommandCaller (const char *name, void (*p)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8)):base_type(name,0,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    func ((Tc*)pc, __S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2), __S_PARAM(T4, 3), __S_PARAM(T5, 4), 
                   __S_PARAM(T6, 5), __S_PARAM(T7, 6), __S_PARAM(T8, 7));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s,%s,%s,%s,%s,%s)", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()());
  }
};

template <class Tc, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
struct ATOM_CommandCaller<void (*)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9)>: public ATOM_CommandCallerBaseT<void (*)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9)>
{
  typedef void return_type;
  typedef ATOM_CommandCallerBaseT<void (*)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9)> base_type;
  ATOM_CommandCaller (const char *name, void (*p)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9)):base_type(name,0,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    func ((Tc*)pc, __S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2), __S_PARAM(T4, 3), __S_PARAM(T5, 4), 
                   __S_PARAM(T6, 5), __S_PARAM(T7, 6), __S_PARAM(T8, 7), __S_PARAM(T9, 8));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s,%s,%s,%s,%s,%s,%s)", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()());
  }
};

template <class Tc, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10>
struct ATOM_CommandCaller<void (*)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10)>: public ATOM_CommandCallerBaseT<void (*)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10)>
{
  typedef void return_type;
  typedef ATOM_CommandCallerBaseT<void (*)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10)> base_type;
  ATOM_CommandCaller (const char *name, void (*p)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10)):base_type(name,0,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    func ((Tc*)pc, __S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2), __S_PARAM(T4, 3), __S_PARAM(T5, 4), 
                   __S_PARAM(T6, 5), __S_PARAM(T7, 6), __S_PARAM(T8, 7), __S_PARAM(T9, 8), __S_PARAM(T10, 9));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()());
  }
};

template <class Tc, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11>
struct ATOM_CommandCaller<void (*)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11)>: public ATOM_CommandCallerBaseT<void (*)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11)>
{
  typedef void return_type;
  typedef ATOM_CommandCallerBaseT<void (*)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11)> base_type;
  ATOM_CommandCaller (const char *name, void (*p)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11)):base_type(name,0,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    func ((Tc*)pc, __S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2), __S_PARAM(T4, 3), __S_PARAM(T5, 4), 
                   __S_PARAM(T6, 5), __S_PARAM(T7, 6), __S_PARAM(T8, 7), __S_PARAM(T9, 8), __S_PARAM(T10, 9),
                   __S_PARAM(T11, 10));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()(), __Help__<T11>()());
  }
};

template <class Tc, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12>
struct ATOM_CommandCaller<void (*)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12)>: public ATOM_CommandCallerBaseT<void (*)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12)>
{
  typedef void return_type;
  typedef ATOM_CommandCallerBaseT<void (*)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12)> base_type;
  ATOM_CommandCaller (const char *name, void (*p)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12)):base_type(name,0,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    func ((Tc*)pc, __S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2), __S_PARAM(T4, 3), __S_PARAM(T5, 4), 
                   __S_PARAM(T6, 5), __S_PARAM(T7, 6), __S_PARAM(T8, 7), __S_PARAM(T9, 8), __S_PARAM(T10, 9),
                   __S_PARAM(T11, 10), __S_PARAM(T12, 11));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()(), __Help__<T11>()(),
      __Help__<T12>()());
  }
};

template <class Tc, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13>
struct ATOM_CommandCaller<void (*)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13)>: public ATOM_CommandCallerBaseT<void (*)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13)>
{
  typedef void return_type;
  typedef ATOM_CommandCallerBaseT<void (*)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13)> base_type;
  ATOM_CommandCaller (const char *name, void (*p)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13)):base_type(name,0,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    func ((Tc*)pc, __S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2), __S_PARAM(T4, 3), __S_PARAM(T5, 4), 
                   __S_PARAM(T6, 5), __S_PARAM(T7, 6), __S_PARAM(T8, 7), __S_PARAM(T9, 8), __S_PARAM(T10, 9),
                   __S_PARAM(T11, 10), __S_PARAM(T12, 11), __S_PARAM(T13, 12));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()(), __Help__<T11>()(),
      __Help__<T12>()(), __Help__<T13>()());
  }
};

template <class Tc, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14>
struct ATOM_CommandCaller<void (*)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14)>: public ATOM_CommandCallerBaseT<void (*)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14)>
{
  typedef void return_type;
  typedef ATOM_CommandCallerBaseT<void (*)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14)> base_type;
  ATOM_CommandCaller (const char *name, void (*p)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14)):base_type(name,0,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    func ((Tc*)pc, __S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2), __S_PARAM(T4, 3), __S_PARAM(T5, 4), 
                   __S_PARAM(T6, 5), __S_PARAM(T7, 6), __S_PARAM(T8, 7), __S_PARAM(T9, 8), __S_PARAM(T10, 9),
                   __S_PARAM(T11, 10), __S_PARAM(T12, 11), __S_PARAM(T13, 12), __S_PARAM(T14, 13));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()(), __Help__<T11>()(),
      __Help__<T12>()(), __Help__<T13>()(), __Help__<T14>()());
  }
};

template <class Tc, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15>
struct ATOM_CommandCaller<void (*)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15)>: public ATOM_CommandCallerBaseT<void (*)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15)>
{
  typedef void return_type;
  typedef ATOM_CommandCallerBaseT<void (*)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15)> base_type;
  ATOM_CommandCaller (const char *name, void (*p)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15)):base_type(name,0,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    func ((Tc*)pc, __S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2), __S_PARAM(T4, 3), __S_PARAM(T5, 4), 
                   __S_PARAM(T6, 5), __S_PARAM(T7, 6), __S_PARAM(T8, 7), __S_PARAM(T9, 8), __S_PARAM(T10, 9),
                   __S_PARAM(T11, 10), __S_PARAM(T12, 11), __S_PARAM(T13, 12), __S_PARAM(T14, 13), __S_PARAM(T15, 14));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()(), __Help__<T11>()(),
      __Help__<T12>()(), __Help__<T13>()(), __Help__<T14>()(), __Help__<T15>()());
  }
};

template <class Tc, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16>
struct ATOM_CommandCaller<void (*)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16)>: public ATOM_CommandCallerBaseT<void (*)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16)>
{
  typedef void return_type;
  typedef ATOM_CommandCallerBaseT<void (*)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16)> base_type;
  ATOM_CommandCaller (const char *name, void (*p)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16)):base_type(name,0,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    func ((Tc*)pc, __S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2), __S_PARAM(T4, 3), __S_PARAM(T5, 4), 
                   __S_PARAM(T6, 5), __S_PARAM(T7, 6), __S_PARAM(T8, 7), __S_PARAM(T9, 8), __S_PARAM(T10, 9),
                   __S_PARAM(T11, 10), __S_PARAM(T12, 11), __S_PARAM(T13, 12), __S_PARAM(T14, 13), __S_PARAM(T15, 14), __S_PARAM(T16, 15));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()(), __Help__<T11>()(),
      __Help__<T12>()(), __Help__<T13>()(), __Help__<T14>()(), __Help__<T15>()(), __Help__<T16>()());
  }
};

template <class Tc, class Tr>
struct ATOM_CommandCaller<Tr (*)(Tc*)>: public ATOM_CommandCallerBaseT<Tr (*)(Tc*)>
{
  typedef Tr return_type;
  typedef ATOM_CommandCallerBaseT<Tr (*)(Tc*)> base_type;
  ATOM_CommandCaller (const char *name, ATOM_DereferenceFunc deref, Tr (*p)(Tc*)):base_type(name,deref,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>() (scp, func ((Tc*)pc), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (void)", __Help__<Tr>()(), this->_name);
  }
};

template <class Tc, class Tr, class T1>
struct ATOM_CommandCaller<Tr (*)(Tc*, T1)>: public ATOM_CommandCallerBaseT<Tr (*)(Tc*, T1)>
{
  typedef Tr return_type;
  typedef ATOM_CommandCallerBaseT<Tr (*)(Tc*, T1)> base_type;
  ATOM_CommandCaller (const char *name, ATOM_DereferenceFunc deref, Tr (*p)(Tc*, T1)):base_type(name,deref,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>() (scp, func ((Tc*)pc, __S_PARAM(T1, 0)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s)", __Help__<Tr>()(), this->_name, __Help__<T1>()());
  }
};

template <class Tc, class Tr, class T1, class T2>
struct ATOM_CommandCaller<Tr (*)(Tc*, T1, T2)>: public ATOM_CommandCallerBaseT<Tr (*)(Tc*, T1, T2)>
{
  typedef Tr return_type;
  typedef ATOM_CommandCallerBaseT<Tr (*)(Tc*, T1, T2)> base_type;
  ATOM_CommandCaller (const char *name, ATOM_DereferenceFunc deref, Tr (*p)(Tc*, T1, T2)):base_type(name,deref,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>() (scp, func ((Tc*)pc, __S_PARAM(T1, 0), __S_PARAM(T2, 1)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s)", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()());
  }
};

template <class Tc, class Tr, class T1, class T2, class T3>
struct ATOM_CommandCaller<Tr (*)(Tc*, T1, T2, T3)>: public ATOM_CommandCallerBaseT<Tr (*)(Tc*, T1, T2, T3)>
{
  typedef Tr return_type;
  typedef ATOM_CommandCallerBaseT<Tr (*)(Tc*, T1, T2, T3)> base_type;
  ATOM_CommandCaller (const char *name, ATOM_DereferenceFunc deref, Tr (*p)(Tc*, T1, T2, T3)):base_type(name,deref,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>() (scp, func ((Tc*)pc, __S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s)", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()());
  }
};

template <class Tc, class Tr, class T1, class T2, class T3, class T4>
struct ATOM_CommandCaller<Tr (*)(Tc*, T1, T2, T3, T4)>: public ATOM_CommandCallerBaseT<Tr (*)(Tc*, T1, T2, T3, T4)>
{
  typedef Tr return_type;
  typedef ATOM_CommandCallerBaseT<Tr (*)(Tc*, T1, T2, T3, T4)> base_type;
  ATOM_CommandCaller (const char *name, ATOM_DereferenceFunc deref, Tr (*p)(Tc*, T1, T2, T3, T4)):base_type(name,deref,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>() (scp, func ((Tc*)pc, __S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2), __S_PARAM(T4, 3)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s,%s)", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()());
  }
};

template <class Tc, class Tr, class T1, class T2, class T3, class T4, class T5>
struct ATOM_CommandCaller<Tr (*)(Tc*, T1, T2, T3, T4, T5)>: public ATOM_CommandCallerBaseT<Tr (*)(Tc*, T1, T2, T3, T4, T5)>
{
  typedef Tr return_type;
  typedef ATOM_CommandCallerBaseT<Tr (*)(Tc*, T1, T2, T3, T4, T5)> base_type;
  ATOM_CommandCaller (const char *name, ATOM_DereferenceFunc deref, Tr (*p)(Tc*, T1, T2, T3, T4, T5)):base_type(name,deref,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>() (scp, func ((Tc*)pc, __S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2), __S_PARAM(T4, 3), __S_PARAM(T5, 4)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s,%s,%s)", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()());
  }
};

template <class Tc, class Tr, class T1, class T2, class T3, class T4, class T5, class T6>
struct ATOM_CommandCaller<Tr (*)(Tc*, T1, T2, T3, T4, T5, T6)>: public ATOM_CommandCallerBaseT<Tr (*)(Tc*, T1, T2, T3, T4, T5, T6)>
{
  typedef Tr return_type;
  typedef ATOM_CommandCallerBaseT<Tr (*)(Tc*, T1, T2, T3, T4, T5, T6)> base_type;
  ATOM_CommandCaller (const char *name, ATOM_DereferenceFunc deref, Tr (*p)(Tc*, T1, T2, T3, T4, T5, T6)):base_type(name,deref,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>() (scp, func ((Tc*)pc, __S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2), __S_PARAM(T4, 3), __S_PARAM(T5, 4), 
                                            __S_PARAM(T6, 5)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s,%s,%s,%s)", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()());
  }
};

template <class Tc, class Tr, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
struct ATOM_CommandCaller<Tr (*)(Tc*, T1, T2, T3, T4, T5, T6, T7)>: public ATOM_CommandCallerBaseT<Tr (*)(Tc*, T1, T2, T3, T4, T5, T6, T7)>
{
  typedef Tr return_type;
  typedef ATOM_CommandCallerBaseT<Tr (*)(Tc*, T1, T2, T3, T4, T5, T6, T7)> base_type;
  ATOM_CommandCaller (const char *name, ATOM_DereferenceFunc deref, Tr (*p)(Tc*, T1, T2, T3, T4, T5, T6, T7)):base_type(name,deref,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>() (scp, func ((Tc*)pc, __S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2), __S_PARAM(T4, 3), __S_PARAM(T5, 4), 
                                            __S_PARAM(T6, 5), __S_PARAM(T7, 6)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s,%s,%s,%s,%s)", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()());
  }
};

template <class Tc, class Tr, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
struct ATOM_CommandCaller<Tr (*)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8)>: public ATOM_CommandCallerBaseT<Tr (*)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8)>
{
  typedef Tr return_type;
  typedef ATOM_CommandCallerBaseT<Tr (*)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8)> base_type;
  ATOM_CommandCaller (const char *name, ATOM_DereferenceFunc deref, Tr (*p)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8)):base_type(name,deref,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>() (scp, func ((Tc*)pc, __S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2), __S_PARAM(T4, 3), __S_PARAM(T5, 4), 
                                            __S_PARAM(T6, 5), __S_PARAM(T7, 6), __S_PARAM(T8, 7)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s,%s,%s,%s,%s,%s)", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()());
  }
};

template <class Tc, class Tr, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
struct ATOM_CommandCaller<Tr (*)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9)>: public ATOM_CommandCallerBaseT<Tr (*)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9)>
{
  typedef Tr return_type;
  typedef ATOM_CommandCallerBaseT<Tr (*)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9)> base_type;
  ATOM_CommandCaller (const char *name, ATOM_DereferenceFunc deref, Tr (*p)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9)):base_type(name,deref,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>() (scp, func ((Tc*)pc, __S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2), __S_PARAM(T4, 3), __S_PARAM(T5, 4), 
                                            __S_PARAM(T6, 5), __S_PARAM(T7, 6), __S_PARAM(T8, 7), __S_PARAM(T9, 8)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s,%s,%s,%s,%s,%s,%s)", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()());
  }
};

template <class Tc, class Tr, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10>
struct ATOM_CommandCaller<Tr (*)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10)>: public ATOM_CommandCallerBaseT<Tr (*)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10)>
{
  typedef Tr return_type;
  typedef ATOM_CommandCallerBaseT<Tr (*)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10)> base_type;
  ATOM_CommandCaller (const char *name, ATOM_DereferenceFunc deref, Tr (*p)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10)):base_type(name,deref,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>() (scp, func ((Tc*)pc, __S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2), __S_PARAM(T4, 3), __S_PARAM(T5, 4), 
                                            __S_PARAM(T6, 5), __S_PARAM(T7, 6), __S_PARAM(T8, 7), __S_PARAM(T9, 8), __S_PARAM(T10, 9)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()());
  }
};

template <class Tc, class Tr, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11>
struct ATOM_CommandCaller<Tr (*)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11)>: public ATOM_CommandCallerBaseT<Tr (*)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11)>
{
  typedef Tr return_type;
  typedef ATOM_CommandCallerBaseT<Tr (*)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11)> base_type;
  ATOM_CommandCaller (const char *name, ATOM_DereferenceFunc deref, Tr (*p)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11)):base_type(name,deref,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>() (scp, func ((Tc*)pc, __S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2), __S_PARAM(T4, 3), __S_PARAM(T5, 4), 
                                            __S_PARAM(T6, 5), __S_PARAM(T7, 6), __S_PARAM(T8, 7), __S_PARAM(T9, 8), __S_PARAM(T10, 9),
                                            __S_PARAM(T11, 10)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()(), __Help__<T11>()());
  }
};

template <class Tc, class Tr, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12>
struct ATOM_CommandCaller<Tr (*)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12)>: public ATOM_CommandCallerBaseT<Tr (*)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12)>
{
  typedef Tr return_type;
  typedef ATOM_CommandCallerBaseT<Tr (*)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12)> base_type;
  ATOM_CommandCaller (const char *name, ATOM_DereferenceFunc deref, Tr (*p)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12)):base_type(name,deref,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>() (scp, func ((Tc*)pc, __S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2), __S_PARAM(T4, 3), __S_PARAM(T5, 4), 
                                            __S_PARAM(T6, 5), __S_PARAM(T7, 6), __S_PARAM(T8, 7), __S_PARAM(T9, 8), __S_PARAM(T10, 9),
                                            __S_PARAM(T11, 10), __S_PARAM(T12, 11)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()(), __Help__<T11>()(),
      __Help__<T12>()());
  }
};

template <class Tc, class Tr, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13>
struct ATOM_CommandCaller<Tr (*)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13)>: public ATOM_CommandCallerBaseT<Tr (*)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13)>
{
  typedef Tr return_type;
  typedef ATOM_CommandCallerBaseT<Tr (*)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13)> base_type;
  ATOM_CommandCaller (const char *name, ATOM_DereferenceFunc deref, Tr (*p)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13)):base_type(name,deref,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>() (scp, func ((Tc*)pc, __S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2), __S_PARAM(T4, 3), __S_PARAM(T5, 4), 
                                            __S_PARAM(T6, 5), __S_PARAM(T7, 6), __S_PARAM(T8, 7), __S_PARAM(T9, 8), __S_PARAM(T10, 9),
                                            __S_PARAM(T11, 10), __S_PARAM(T12, 11), __S_PARAM(T13, 12)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()(), __Help__<T11>()(),
      __Help__<T12>()(), __Help__<T13>()());
  }
};

template <class Tc, class Tr, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14>
struct ATOM_CommandCaller<Tr (*)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14)>: public ATOM_CommandCallerBaseT<Tr (*)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14)>
{
  typedef Tr return_type;
  typedef ATOM_CommandCallerBaseT<Tr (*)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14)> base_type;
  ATOM_CommandCaller (const char *name, ATOM_DereferenceFunc deref, Tr (*p)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14)):base_type(name,deref,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>() (scp, func ((Tc*)pc, __S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2), __S_PARAM(T4, 3), __S_PARAM(T5, 4), 
                                            __S_PARAM(T6, 5), __S_PARAM(T7, 6), __S_PARAM(T8, 7), __S_PARAM(T9, 8), __S_PARAM(T10, 9),
                                            __S_PARAM(T11, 10), __S_PARAM(T12, 11), __S_PARAM(T13, 12), __S_PARAM(T14, 13)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()(), __Help__<T11>()(),
      __Help__<T12>()(), __Help__<T13>()(), __Help__<T14>()());
  }
};

template <class Tc, class Tr, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15>
struct ATOM_CommandCaller<Tr (*)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15)>: public ATOM_CommandCallerBaseT<Tr (*)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15)>
{
  typedef Tr return_type;
  typedef ATOM_CommandCallerBaseT<Tr (*)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15)> base_type;
  ATOM_CommandCaller (const char *name, ATOM_DereferenceFunc deref, Tr (*p)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15)):base_type(name,deref,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>() (scp, func ((Tc*)pc, __S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2), __S_PARAM(T4, 3), __S_PARAM(T5, 4), 
                                            __S_PARAM(T6, 5), __S_PARAM(T7, 6), __S_PARAM(T8, 7), __S_PARAM(T9, 8), __S_PARAM(T10, 9),
                                            __S_PARAM(T11, 10), __S_PARAM(T12, 11), __S_PARAM(T13, 12), __S_PARAM(T14, 13), __S_PARAM(T15, 14)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()(), __Help__<T11>()(),
      __Help__<T12>()(), __Help__<T13>()(), __Help__<T14>()(), __Help__<T15>()());
  }
};

template <class Tc, class Tr, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16>
struct ATOM_CommandCaller<Tr (*)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16)>: public ATOM_CommandCallerBaseT<Tr (*)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16)>
{
  typedef Tr return_type;
  typedef ATOM_CommandCallerBaseT<Tr (*)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16)> base_type;
  ATOM_CommandCaller (const char *name, ATOM_DereferenceFunc deref, Tr (*p)(Tc*, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16)):base_type(name,deref,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>() (scp, func ((Tc*)pc, __S_PARAM(T1, 0), __S_PARAM(T2, 1), __S_PARAM(T3, 2), __S_PARAM(T4, 3), __S_PARAM(T5, 4), 
                                            __S_PARAM(T6, 5), __S_PARAM(T7, 6), __S_PARAM(T8, 7), __S_PARAM(T9, 8), __S_PARAM(T10, 9),
                                            __S_PARAM(T11, 10), __S_PARAM(T12, 11), __S_PARAM(T13, 12), __S_PARAM(T14, 13), __S_PARAM(T15, 14), __S_PARAM(T16, 15)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()(), __Help__<T11>()(),
      __Help__<T12>()(), __Help__<T13>()(), __Help__<T14>()(), __Help__<T15>()(), __Help__<T16>()());
  }
};

template <class Tc> 
struct ATOM_CommandCaller<void (Tc::*)(void)>: public ATOM_CommandCallerBaseT<void (Tc::*)(void)>
{
  typedef void return_type;
  typedef ATOM_CommandCallerBaseT<void (Tc::*)(void)> base_type;
  ATOM_CommandCaller (const char *name, void (Tc::*p)(void)):base_type(name,0,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    (((Tc*)pc)->*(this->func)) ();
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (void)", this->_name);
  }
};

template <class Tc> 
struct ATOM_CommandCaller<void (Tc::*)(void) const>: public ATOM_CommandCallerBaseT<void (Tc::*)(void) const>
{
  typedef void return_type;
  typedef ATOM_CommandCallerBaseT<void (Tc::*)(void) const> base_type;
  ATOM_CommandCaller (const char *name, void (Tc::*p)(void) const):base_type(name,0,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    (((const Tc*)pc)->*(this->func)) ();
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (void) const", this->_name);
  }
};

template <class Tc, class T1> 
struct ATOM_CommandCaller<void (Tc::*)(T1)>: public ATOM_CommandCallerBaseT<void (Tc::*)(T1)>
{
  typedef void return_type;
  typedef ATOM_CommandCallerBaseT<void (Tc::*)(T1)> base_type;
  ATOM_CommandCaller (const char *name, void (Tc::*p)(T1)):base_type(name,0,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    (((Tc*)pc)->*(this->func)) (__S_PARAM(T1,0));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s)", this->_name, __Help__<T1>()());
  }
};

template <class Tc, class T1> 
struct ATOM_CommandCaller<void (Tc::*)(T1) const>: public ATOM_CommandCallerBaseT<void (Tc::*)(T1) const>
{
  typedef void return_type;
  typedef ATOM_CommandCallerBaseT<void (Tc::*)(T1) const> base_type;
  ATOM_CommandCaller (const char *name, void (Tc::*p)(T1) const):base_type(name,0,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    (((const Tc*)pc)->*(this->func)) (__S_PARAM(T1,0));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s) const", this->_name, __Help__<T1>()());
  }
};

template <class Tc, class T1, class T2> 
struct ATOM_CommandCaller<void (Tc::*)(T1,T2)>: public ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2)>
{
  typedef void return_type;
  typedef ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2)> base_type;
  ATOM_CommandCaller (const char *name, void (Tc::*p)(T1,T2)):base_type(name,0,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    (((Tc*)pc)->*(this->func)) (__S_PARAM(T1,0), __S_PARAM(T2,1));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s)", this->_name, __Help__<T1>()(), __Help__<T2>()());
  }
};

template <class Tc, class T1, class T2> 
struct ATOM_CommandCaller<void (Tc::*)(T1,T2) const>: public ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2) const>
{
  typedef void return_type;
  typedef ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2) const> base_type;
  ATOM_CommandCaller (const char *name, void (Tc::*p)(T1,T2) const):base_type(name,0,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    (((const Tc*)pc)->*(this->func)) (__S_PARAM(T1,0), __S_PARAM(T2,1));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s) const", this->_name, __Help__<T1>()(), __Help__<T2>()());
  }
};

template <class Tc, class T1, class T2, class T3> 
struct ATOM_CommandCaller<void (Tc::*)(T1,T2,T3)>: public ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3)>
{
  typedef void return_type;
  typedef ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3)> base_type;
  ATOM_CommandCaller (const char *name, void (Tc::*p)(T1,T2,T3)):base_type(name,0,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    (((Tc*)pc)->*(this->func)) (__S_PARAM(T1,0), __S_PARAM(T2,1), __S_PARAM(T3,2));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s)", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()());
  }
};

template <class Tc, class T1, class T2, class T3> 
struct ATOM_CommandCaller<void (Tc::*)(T1,T2,T3) const>: public ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3) const>
{
  typedef void return_type;
  typedef ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3) const> base_type;
  ATOM_CommandCaller (const char *name, void (Tc::*p)(T1,T2,T3) const):base_type(name,0,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    (((const Tc*)pc)->*(this->func)) (__S_PARAM(T1,0), __S_PARAM(T2,1), __S_PARAM(T3,2));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s) const", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()());
  }
};

template <class Tc, class T1, class T2, class T3, class T4> 
struct ATOM_CommandCaller<void (Tc::*)(T1,T2,T3,T4)>: public ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3,T4)>
{
  typedef void return_type;
  typedef ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3,T4)> base_type;
  ATOM_CommandCaller (const char *name, void (Tc::*p)(T1,T2,T3,T4)):base_type(name,0,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    (((Tc*)pc)->*(this->func)) (__S_PARAM(T1,0), __S_PARAM(T2,1), __S_PARAM(T3,2), __S_PARAM(T4,3));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s,%s)", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()());
  }
};

template <class Tc, class T1, class T2, class T3, class T4> 
struct ATOM_CommandCaller<void (Tc::*)(T1,T2,T3,T4) const>: public ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3,T4) const>
{
  typedef void return_type;
  typedef ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3,T4) const> base_type;
  ATOM_CommandCaller (const char *name, void (Tc::*p)(T1,T2,T3,T4) const):base_type(name,0,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    (((const Tc*)pc)->*(this->func)) (__S_PARAM(T1,0), __S_PARAM(T2,1), __S_PARAM(T3,2), __S_PARAM(T4,3));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s,%s) const", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()());
  }
};

template <class Tc, class T1, class T2, class T3, class T4, class T5> 
struct ATOM_CommandCaller<void (Tc::*)(T1,T2,T3,T4,T5)>: public ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3,T4,T5)>
{
  typedef void return_type;
  typedef ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3,T4,T5)> base_type;
  ATOM_CommandCaller (const char *name, void (Tc::*p)(T1,T2,T3,T4,T5)):base_type(name,0,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    (((Tc*)pc)->*(this->func)) (__S_PARAM(T1,0), __S_PARAM(T2,1), __S_PARAM(T3,2), __S_PARAM(T4,3), __S_PARAM(T5,4));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s,%s,%s)", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()());
  }
};

template <class Tc, class T1, class T2, class T3, class T4, class T5> 
struct ATOM_CommandCaller<void (Tc::*)(T1,T2,T3,T4,T5) const>: public ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3,T4,T5) const>
{
  typedef void return_type;
  typedef ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3,T4,T5) const> base_type;
  ATOM_CommandCaller (const char *name, void (Tc::*p)(T1,T2,T3,T4,T5) const):base_type(name,0,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    (((const Tc*)pc)->*(this->func)) (__S_PARAM(T1,0), __S_PARAM(T2,1), __S_PARAM(T3,2), __S_PARAM(T4,3), __S_PARAM(T5,4));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s,%s,%s) const", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()());
  }
};

template <class Tc, class T1, class T2, class T3, class T4, class T5, class T6> 
struct ATOM_CommandCaller<void (Tc::*)(T1,T2,T3,T4,T5,T6)>: public ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3,T4,T5,T6)>
{
  typedef void return_type;
  typedef ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3,T4,T5,T6)> base_type;
  ATOM_CommandCaller (const char *name, void (Tc::*p)(T1,T2,T3,T4,T5,T6)):base_type(name,0,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    (((Tc*)pc)->*(this->func)) (__S_PARAM(T1,0), __S_PARAM(T2,1), __S_PARAM(T3,2), __S_PARAM(T4,3), __S_PARAM(T5,4),
                                __S_PARAM(T6,5));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s,%s,%s,%s)", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()());
  }
};

template <class Tc, class T1, class T2, class T3, class T4, class T5, class T6> 
struct ATOM_CommandCaller<void (Tc::*)(T1,T2,T3,T4,T5,T6) const>: public ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3,T4,T5,T6) const>
{
  typedef void return_type;
  typedef ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3,T4,T5,T6) const> base_type;
  ATOM_CommandCaller (const char *name, void (Tc::*p)(T1,T2,T3,T4,T5,T6) const):base_type(name,0,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    (((const Tc*)pc)->*(this->func)) (__S_PARAM(T1,0), __S_PARAM(T2,1), __S_PARAM(T3,2), __S_PARAM(T4,3), __S_PARAM(T5,4),
                                __S_PARAM(T6,5));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s,%s,%s,%s) const", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()());
  }
};

template <class Tc, class T1, class T2, class T3, class T4, class T5, class T6, class T7> 
struct ATOM_CommandCaller<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7)>: public ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7)>
{
  typedef void return_type;
  typedef ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7)> base_type;
  ATOM_CommandCaller (const char *name, void (Tc::*p)(T1,T2,T3,T4,T5,T6,T7)):base_type(name,0,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    (((Tc*)pc)->*(this->func)) (__S_PARAM(T1,0), __S_PARAM(T2,1), __S_PARAM(T3,2), __S_PARAM(T4,3), __S_PARAM(T5,4),
                                __S_PARAM(T6,5), __S_PARAM(T7,6));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s,%s,%s,%s,%s)", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()());
  }
};

template <class Tc, class T1, class T2, class T3, class T4, class T5, class T6, class T7> 
struct ATOM_CommandCaller<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7) const>: public ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7) const>
{
  typedef void return_type;
  typedef ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7) const> base_type;
  ATOM_CommandCaller (const char *name, void (Tc::*p)(T1,T2,T3,T4,T5,T6,T7) const):base_type(name,0,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    (((const Tc*)pc)->*(this->func)) (__S_PARAM(T1,0), __S_PARAM(T2,1), __S_PARAM(T3,2), __S_PARAM(T4,3), __S_PARAM(T5,4),
                                __S_PARAM(T6,5), __S_PARAM(T7,6));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s,%s,%s,%s,%s) const", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()());
  }
};

template <class Tc, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8> 
struct ATOM_CommandCaller<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8)>: public ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8)>
{
  typedef void return_type;
  typedef  ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8)> base_type;
  ATOM_CommandCaller (const char *name, void (Tc::*p)(T1,T2,T3,T4,T5,T6,T7,T8)):base_type(name,0,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    (((Tc*)pc)->*(this->func)) (__S_PARAM(T1,0), __S_PARAM(T2,1), __S_PARAM(T3,2), __S_PARAM(T4,3), __S_PARAM(T5,4),
                                __S_PARAM(T6,5), __S_PARAM(T7,6), __S_PARAM(T8,7));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s,%s,%s,%s,%s,%s)", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()());
  }
};

template <class Tc, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8> 
struct ATOM_CommandCaller<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8) const>: public ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8) const>
{
  typedef void return_type;
  typedef  ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8) const> base_type;
  ATOM_CommandCaller (const char *name, void (Tc::*p)(T1,T2,T3,T4,T5,T6,T7,T8) const):base_type(name,0,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    (((const Tc*)pc)->*(this->func)) (__S_PARAM(T1,0), __S_PARAM(T2,1), __S_PARAM(T3,2), __S_PARAM(T4,3), __S_PARAM(T5,4),
                                __S_PARAM(T6,5), __S_PARAM(T7,6), __S_PARAM(T8,7));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s,%s,%s,%s,%s,%s) const", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()());
  }
};

template <class Tc, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9> 
struct ATOM_CommandCaller<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9)>: public ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9)>
{
  typedef void return_type;
  typedef  ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9)> base_type;
  ATOM_CommandCaller (const char *name, void (Tc::*p)(T1,T2,T3,T4,T5,T6,T7,T8,T9)):base_type(name,0,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    (((Tc*)pc)->*(this->func)) (__S_PARAM(T1,0), __S_PARAM(T2,1), __S_PARAM(T3,2), __S_PARAM(T4,3), __S_PARAM(T5,4),
                                __S_PARAM(T6,5), __S_PARAM(T7,6), __S_PARAM(T8,7), __S_PARAM(T9,8));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s,%s,%s,%s,%s,%s,%s)", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()());
  }
};

template <class Tc, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9> 
struct ATOM_CommandCaller<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9) const>: public ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9) const>
{
  typedef void return_type;
  typedef  ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9) const> base_type;
  ATOM_CommandCaller (const char *name, void (Tc::*p)(T1,T2,T3,T4,T5,T6,T7,T8,T9) const):base_type(name,0,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    (((const Tc*)pc)->*(this->func)) (__S_PARAM(T1,0), __S_PARAM(T2,1), __S_PARAM(T3,2), __S_PARAM(T4,3), __S_PARAM(T5,4),
                                __S_PARAM(T6,5), __S_PARAM(T7,6), __S_PARAM(T8,7), __S_PARAM(T9,8));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s,%s,%s,%s,%s,%s,%s) const", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()());
  }
};

template <class Tc, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10> 
struct ATOM_CommandCaller<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10)>: public ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10)>
{
  typedef void return_type;
  typedef ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10)> base_type;
  ATOM_CommandCaller (const char *name, void (Tc::*p)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10)):base_type(name,0,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    (((Tc*)pc)->*(this->func)) (__S_PARAM(T1,0), __S_PARAM(T2,1), __S_PARAM(T3,2), __S_PARAM(T4,3), __S_PARAM(T5,4),
                                __S_PARAM(T6,5), __S_PARAM(T7,6), __S_PARAM(T8,7), __S_PARAM(T9,8), __S_PARAM(T10,9));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()());
  }
};

template <class Tc, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10> 
struct ATOM_CommandCaller<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10) const>: public ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10) const>
{
  typedef void return_type;
  typedef ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10) const> base_type;
  ATOM_CommandCaller (const char *name, void (Tc::*p)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10) const):base_type(name,0,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    (((const Tc*)pc)->*(this->func)) (__S_PARAM(T1,0), __S_PARAM(T2,1), __S_PARAM(T3,2), __S_PARAM(T4,3), __S_PARAM(T5,4),
                                __S_PARAM(T6,5), __S_PARAM(T7,6), __S_PARAM(T8,7), __S_PARAM(T9,8), __S_PARAM(T10,9));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s) const", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()());
  }
};

template <class Tc, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11> 
struct ATOM_CommandCaller<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11)>: public ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11)>
{
  typedef void return_type;
  typedef ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11)> base_type;
  ATOM_CommandCaller (const char *name, void (Tc::*p)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11)):base_type(name,0,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    (((Tc*)pc)->*(this->func)) (__S_PARAM(T1,0), __S_PARAM(T2,1), __S_PARAM(T3,2), __S_PARAM(T4,3), __S_PARAM(T5,4),
                                __S_PARAM(T6,5), __S_PARAM(T7,6), __S_PARAM(T8,7), __S_PARAM(T9,8), __S_PARAM(T10,9),
                                __S_PARAM(T11,10));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()(), __Help__<T11>()());
  }
};

template <class Tc, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11> 
struct ATOM_CommandCaller<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11) const>: public ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11) const>
{
  typedef void return_type;
  typedef ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11) const> base_type;
  ATOM_CommandCaller (const char *name, void (Tc::*p)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11) const):base_type(name,0,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    (((const Tc*)pc)->*(this->func)) (__S_PARAM(T1,0), __S_PARAM(T2,1), __S_PARAM(T3,2), __S_PARAM(T4,3), __S_PARAM(T5,4),
                                __S_PARAM(T6,5), __S_PARAM(T7,6), __S_PARAM(T8,7), __S_PARAM(T9,8), __S_PARAM(T10,9),
                                __S_PARAM(T11,10));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s) const", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()(), __Help__<T11>()());
  }
};

template <class Tc, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12> 
struct ATOM_CommandCaller<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12)>: public ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12)>
{
  typedef void return_type;
  typedef ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12)> base_type;
  ATOM_CommandCaller (const char *name, void (Tc::*p)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12)):base_type(name,0,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    (((Tc*)pc)->*(this->func)) (__S_PARAM(T1,0), __S_PARAM(T2,1), __S_PARAM(T3,2), __S_PARAM(T4,3), __S_PARAM(T5,4),
                                __S_PARAM(T6,5), __S_PARAM(T7,6), __S_PARAM(T8,7), __S_PARAM(T9,8), __S_PARAM(T10,9),
                                __S_PARAM(T11,10), __S_PARAM(T12,11));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()(), __Help__<T11>()(),
      __Help__<T12>()());
  }
};

template <class Tc, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12> 
struct ATOM_CommandCaller<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12) const>: public ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12) const>
{
  typedef void return_type;
  typedef ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12) const> base_type;
  ATOM_CommandCaller (const char *name, void (Tc::*p)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12) const):base_type(name,0,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    (((const Tc*)pc)->*(this->func)) (__S_PARAM(T1,0), __S_PARAM(T2,1), __S_PARAM(T3,2), __S_PARAM(T4,3), __S_PARAM(T5,4),
                                __S_PARAM(T6,5), __S_PARAM(T7,6), __S_PARAM(T8,7), __S_PARAM(T9,8), __S_PARAM(T10,9),
                                __S_PARAM(T11,10), __S_PARAM(T12,11));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s) const", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()(), __Help__<T11>()(),
      __Help__<T12>()());
  }
};

template <class Tc, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13> 
struct ATOM_CommandCaller<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13)>: public ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13)>
{
  typedef void return_type;
  typedef ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13)> base_type;
  ATOM_CommandCaller (const char *name, void (Tc::*p)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13)):base_type(name,0,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    (((Tc*)pc)->*(this->func)) (__S_PARAM(T1,0), __S_PARAM(T2,1), __S_PARAM(T3,2), __S_PARAM(T4,3), __S_PARAM(T5,4),
                                __S_PARAM(T6,5), __S_PARAM(T7,6), __S_PARAM(T8,7), __S_PARAM(T9,8), __S_PARAM(T10,9),
                                __S_PARAM(T11,10), __S_PARAM(T12,11), __S_PARAM(T13,12));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()(), __Help__<T11>()(),
      __Help__<T12>()(), __Help__<T13>()());
  }
};

template <class Tc, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13> 
struct ATOM_CommandCaller<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13) const>: public ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13) const>
{
  typedef void return_type;
  typedef ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13) const> base_type;
  ATOM_CommandCaller (const char *name, void (Tc::*p)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13) const):base_type(name,0,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    (((const Tc*)pc)->*(this->func)) (__S_PARAM(T1,0), __S_PARAM(T2,1), __S_PARAM(T3,2), __S_PARAM(T4,3), __S_PARAM(T5,4),
                                __S_PARAM(T6,5), __S_PARAM(T7,6), __S_PARAM(T8,7), __S_PARAM(T9,8), __S_PARAM(T10,9),
                                __S_PARAM(T11,10), __S_PARAM(T12,11), __S_PARAM(T13,12));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s) const", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()(), __Help__<T11>()(),
      __Help__<T12>()(), __Help__<T13>()());
  }
};

template <class Tc, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14> 
struct ATOM_CommandCaller<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14)>: public ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14)>
{
  typedef void return_type;
  typedef ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14)> base_type;
  ATOM_CommandCaller (const char *name, void (Tc::*p)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14)):base_type(name,0,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    (((Tc*)pc)->*(this->func)) (__S_PARAM(T1,0), __S_PARAM(T2,1), __S_PARAM(T3,2), __S_PARAM(T4,3), __S_PARAM(T5,4),
                                __S_PARAM(T6,5), __S_PARAM(T7,6), __S_PARAM(T8,7), __S_PARAM(T9,8), __S_PARAM(T10,9),
                                __S_PARAM(T11,10), __S_PARAM(T12,11), __S_PARAM(T13,12), __S_PARAM(T14,13));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()(), __Help__<T11>()(),
      __Help__<T12>()(), __Help__<T13>()(), __Help__<T14>()());
  }
};

template <class Tc, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14> 
struct ATOM_CommandCaller<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14) const>: public ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14) const>
{
  typedef void return_type;
  typedef ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14) const> base_type;
  ATOM_CommandCaller (const char *name, void (Tc::*p)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14) const):base_type(name,0,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    (((const Tc*)pc)->*(this->func)) (__S_PARAM(T1,0), __S_PARAM(T2,1), __S_PARAM(T3,2), __S_PARAM(T4,3), __S_PARAM(T5,4),
                                __S_PARAM(T6,5), __S_PARAM(T7,6), __S_PARAM(T8,7), __S_PARAM(T9,8), __S_PARAM(T10,9),
                                __S_PARAM(T11,10), __S_PARAM(T12,11), __S_PARAM(T13,12), __S_PARAM(T14,13));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s) const", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()(), __Help__<T11>()(),
      __Help__<T12>()(), __Help__<T13>()(), __Help__<T14>()());
  }
};

template <class Tc, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15> 
struct ATOM_CommandCaller<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15)>: public ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15)>
{
  typedef void return_type;
  typedef ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15)> base_type;
  ATOM_CommandCaller (const char *name, void (Tc::*p)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15)):base_type(name,0,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    (((Tc*)pc)->*(this->func)) (__S_PARAM(T1,0), __S_PARAM(T2,1), __S_PARAM(T3,2), __S_PARAM(T4,3), __S_PARAM(T5,4),
                                __S_PARAM(T6,5), __S_PARAM(T7,6), __S_PARAM(T8,7), __S_PARAM(T9,8), __S_PARAM(T10,9),
                                __S_PARAM(T11,10), __S_PARAM(T12,11), __S_PARAM(T13,12), __S_PARAM(T14,13), __S_PARAM(T15,14));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()(), __Help__<T11>()(),
      __Help__<T12>()(), __Help__<T13>()(), __Help__<T14>()(), __Help__<T15>()());
  }
};

template <class Tc, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15> 
struct ATOM_CommandCaller<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15) const>: public ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15) const>
{
  typedef void return_type;
  typedef ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15) const> base_type;
  ATOM_CommandCaller (const char *name, void (Tc::*p)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15) const):base_type(name,0,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    (((const Tc*)pc)->*(this->func)) (__S_PARAM(T1,0), __S_PARAM(T2,1), __S_PARAM(T3,2), __S_PARAM(T4,3), __S_PARAM(T5,4),
                                __S_PARAM(T6,5), __S_PARAM(T7,6), __S_PARAM(T8,7), __S_PARAM(T9,8), __S_PARAM(T10,9),
                                __S_PARAM(T11,10), __S_PARAM(T12,11), __S_PARAM(T13,12), __S_PARAM(T14,13), __S_PARAM(T15,14));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s) const", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()(), __Help__<T11>()(),
      __Help__<T12>()(), __Help__<T13>()(), __Help__<T14>()(), __Help__<T15>()());
  }
};

template <class Tc, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16> 
struct ATOM_CommandCaller<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16)>: public ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16)>
{
  typedef void return_type;
  typedef ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16)> base_type;
  ATOM_CommandCaller (const char *name, void (Tc::*p)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16)):base_type(name,0,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    (((Tc*)pc)->*(this->func)) (__S_PARAM(T1,0), __S_PARAM(T2,1), __S_PARAM(T3,2), __S_PARAM(T4,3), __S_PARAM(T5,4),
                                __S_PARAM(T6,5), __S_PARAM(T7,6), __S_PARAM(T8,7), __S_PARAM(T9,8), __S_PARAM(T10,9),
                                __S_PARAM(T11,10), __S_PARAM(T12,11), __S_PARAM(T13,12), __S_PARAM(T14,13), __S_PARAM(T15,14), __S_PARAM(T16,15));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()(), __Help__<T11>()(),
      __Help__<T12>()(), __Help__<T13>()(), __Help__<T14>()(), __Help__<T15>()(), __Help__<T16>()());
  }
};

template <class Tc, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16> 
struct ATOM_CommandCaller<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16) const>: public ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16) const>
{
  typedef void return_type;
  typedef ATOM_CommandCallerBaseT<void (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16) const> base_type;
  ATOM_CommandCaller (const char *name, void (Tc::*p)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16) const):base_type(name,0,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    (((const Tc*)pc)->*(this->func)) (__S_PARAM(T1,0), __S_PARAM(T2,1), __S_PARAM(T3,2), __S_PARAM(T4,3), __S_PARAM(T5,4),
                                __S_PARAM(T6,5), __S_PARAM(T7,6), __S_PARAM(T8,7), __S_PARAM(T9,8), __S_PARAM(T10,9),
                                __S_PARAM(T11,10), __S_PARAM(T12,11), __S_PARAM(T13,12), __S_PARAM(T14,13), __S_PARAM(T15,14), __S_PARAM(T16,15));
    return 0;
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "void %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s) const", this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()(), __Help__<T11>()(),
      __Help__<T12>()(), __Help__<T13>()(), __Help__<T14>()(), __Help__<T15>()(), __Help__<T16>()());
  }
};

template <class Tc, class Tr> 
struct ATOM_CommandCaller<Tr (Tc::*)(void)>: public ATOM_CommandCallerBaseT<Tr (Tc::*)(void)>
{
  typedef Tr return_type;
  typedef ATOM_CommandCallerBaseT<Tr (Tc::*)(void)> base_type;
  ATOM_CommandCaller (const char *name, ATOM_DereferenceFunc deref, Tr (Tc::*p)(void)):base_type(name,deref,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>()(scp, (((Tc*)pc)->*(this->func)) (), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (void)", __Help__<Tr>()(), this->_name);
  }
};

template <class Tc, class Tr> 
struct ATOM_CommandCaller<Tr (Tc::*)(void) const>: public ATOM_CommandCallerBaseT<Tr (Tc::*)(void) const>
{
  typedef Tr return_type;
  typedef ATOM_CommandCallerBaseT<Tr (Tc::*)(void) const> base_type;
  ATOM_CommandCaller (const char *name, ATOM_DereferenceFunc deref, Tr (Tc::*p)(void) const):base_type(name,deref,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>()(scp, (((const Tc*)pc)->*(this->func)) (), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (void) const", __Help__<Tr>()(), this->_name);
  }
};

template <class Tc, class Tr, class T1> 
struct ATOM_CommandCaller<Tr (Tc::*)(T1)>: public ATOM_CommandCallerBaseT<Tr (Tc::*)(T1)>
{
  typedef Tr return_type;
  typedef ATOM_CommandCallerBaseT<Tr (Tc::*)(T1)> base_type;
  ATOM_CommandCaller (const char *name, ATOM_DereferenceFunc deref, Tr (Tc::*p)(T1)):base_type(name,deref,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>()(scp, (((Tc*)pc)->*(this->func)) (__S_PARAM(T1,0)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s)", __Help__<Tr>()(), this->_name, __Help__<T1>()());
  }
};

template <class Tc, class Tr, class T1> 
struct ATOM_CommandCaller<Tr (Tc::*)(T1) const>: public ATOM_CommandCallerBaseT<Tr (Tc::*)(T1) const>
{
  typedef Tr return_type;
  typedef ATOM_CommandCallerBaseT<Tr (Tc::*)(T1) const> base_type;
  ATOM_CommandCaller (const char *name, ATOM_DereferenceFunc deref, Tr (Tc::*p)(T1) const):base_type(name,deref,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>()(scp, (((const Tc*)pc)->*(this->func)) (__S_PARAM(T1,0)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s) const", __Help__<Tr>()(), this->_name, __Help__<T1>()());
  }
};

template <class Tc, class Tr, class T1, class T2> 
struct ATOM_CommandCaller<Tr (Tc::*)(T1,T2)>: public ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2)>
{
  typedef Tr return_type;
  typedef ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2)> base_type;
  ATOM_CommandCaller (const char *name, ATOM_DereferenceFunc deref, Tr (Tc::*p)(T1,T2)):base_type(name,deref,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>()(scp, (((Tc*)pc)->*(this->func)) (__S_PARAM(T1,0),__S_PARAM(T2,1)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s)", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()());
  }
};

template <class Tc, class Tr, class T1, class T2> 
struct ATOM_CommandCaller<Tr (Tc::*)(T1,T2) const>: public ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2) const>
{
  typedef Tr return_type;
  typedef ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2) const> base_type;
  ATOM_CommandCaller (const char *name, ATOM_DereferenceFunc deref, Tr (Tc::*p)(T1,T2) const):base_type(name,deref,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>()(scp, (((const Tc*)pc)->*(this->func)) (__S_PARAM(T1,0),__S_PARAM(T2,1)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s) const", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()());
  }
};

template <class Tc, class Tr, class T1, class T2, class T3> 
struct ATOM_CommandCaller<Tr (Tc::*)(T1,T2,T3)>: public ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3)>
{
  typedef Tr return_type;
  typedef ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3)> base_type;
  ATOM_CommandCaller (const char *name, ATOM_DereferenceFunc deref, Tr (Tc::*p)(T1,T2,T3)):base_type(name,deref,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>()(scp, (((Tc*)pc)->*(this->func)) (__S_PARAM(T1,0),__S_PARAM(T2,1),__S_PARAM(T3,2)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s)", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()());
  }
};

template <class Tc, class Tr, class T1, class T2, class T3> 
struct ATOM_CommandCaller<Tr (Tc::*)(T1,T2,T3) const>: public ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3) const>
{
  typedef Tr return_type;
  typedef ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3) const> base_type;
  ATOM_CommandCaller (const char *name, ATOM_DereferenceFunc deref, Tr (Tc::*p)(T1,T2,T3) const):base_type(name,deref,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>()(scp, (((const Tc*)pc)->*(this->func)) (__S_PARAM(T1,0),__S_PARAM(T2,1),__S_PARAM(T3,2)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s) const", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()());
  }
};

template <class Tc, class Tr, class T1, class T2, class T3, class T4> 
struct ATOM_CommandCaller<Tr (Tc::*)(T1,T2,T3,T4)>: public ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3,T4)>
{
  typedef Tr return_type;
  typedef ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3,T4)> base_type;
  ATOM_CommandCaller (const char *name, ATOM_DereferenceFunc deref, Tr (Tc::*p)(T1,T2,T3,T4)):base_type(name,deref,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>()(scp, (((Tc*)pc)->*(this->func)) (__S_PARAM(T1,0),__S_PARAM(T2,1),__S_PARAM(T3,2),__S_PARAM(T4,3)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s,%s)", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()());
  }
};

template <class Tc, class Tr, class T1, class T2, class T3, class T4> 
struct ATOM_CommandCaller<Tr (Tc::*)(T1,T2,T3,T4) const>: public ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3,T4) const>
{
  typedef Tr return_type;
  typedef ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3,T4) const> base_type;
  ATOM_CommandCaller (const char *name, ATOM_DereferenceFunc deref, Tr (Tc::*p)(T1,T2,T3,T4) const):base_type(name,deref,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>()(scp, (((const Tc*)pc)->*(this->func)) (__S_PARAM(T1,0),__S_PARAM(T2,1),__S_PARAM(T3,2),__S_PARAM(T4,3)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s,%s) const", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()());
  }
};

template <class Tc, class Tr, class T1, class T2, class T3, class T4, class T5> 
struct ATOM_CommandCaller<Tr (Tc::*)(T1,T2,T3,T4,T5)>: public ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3,T4,T5)>
{
  typedef Tr return_type;
  typedef ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3,T4,T5)> base_type;
  ATOM_CommandCaller (const char *name, ATOM_DereferenceFunc deref, Tr (Tc::*p)(T1,T2,T3,T4,T5)):base_type(name,deref,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>()(scp, (((Tc*)pc)->*(this->func)) (__S_PARAM(T1,0),__S_PARAM(T2,1),__S_PARAM(T3,2),__S_PARAM(T4,3),
                                                               __S_PARAM(T5,4)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s,%s,%s)", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()());
  }
};

template <class Tc, class Tr, class T1, class T2, class T3, class T4, class T5> 
struct ATOM_CommandCaller<Tr (Tc::*)(T1,T2,T3,T4,T5) const>: public ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3,T4,T5) const>
{
  typedef Tr return_type;
  typedef ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3,T4,T5) const> base_type;
  ATOM_CommandCaller (const char *name, ATOM_DereferenceFunc deref, Tr (Tc::*p)(T1,T2,T3,T4,T5) const):base_type(name,deref,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>()(scp, (((const Tc*)pc)->*(this->func)) (__S_PARAM(T1,0),__S_PARAM(T2,1),__S_PARAM(T3,2),__S_PARAM(T4,3),
                                                               __S_PARAM(T5,4)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s,%s,%s) const", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()());
  }
};

template <class Tc, class Tr, class T1, class T2, class T3, class T4, class T5, class T6> 
struct ATOM_CommandCaller<Tr (Tc::*)(T1,T2,T3,T4,T5,T6)>: public ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3,T4,T5,T6)>
{
  typedef Tr return_type;
  typedef ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3,T4,T5,T6)> base_type;
  ATOM_CommandCaller (const char *name, ATOM_DereferenceFunc deref, Tr (Tc::*p)(T1,T2,T3,T4,T5,T6)):base_type(name,deref,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>()(scp, (((Tc*)pc)->*(this->func)) (__S_PARAM(T1,0),__S_PARAM(T2,1),__S_PARAM(T3,2),__S_PARAM(T4,3),
                                                               __S_PARAM(T5,4),__S_PARAM(T6,5)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s,%s,%s,%s)", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()());
  }
};

template <class Tc, class Tr, class T1, class T2, class T3, class T4, class T5, class T6> 
struct ATOM_CommandCaller<Tr (Tc::*)(T1,T2,T3,T4,T5,T6) const>: public ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3,T4,T5,T6) const>
{
  typedef Tr return_type;
  typedef ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3,T4,T5,T6) const> base_type;
  ATOM_CommandCaller (const char *name, ATOM_DereferenceFunc deref, Tr (Tc::*p)(T1,T2,T3,T4,T5,T6) const):base_type(name,deref,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>()(scp, (((const Tc*)pc)->*(this->func)) (__S_PARAM(T1,0),__S_PARAM(T2,1),__S_PARAM(T3,2),__S_PARAM(T4,3),
                                                               __S_PARAM(T5,4),__S_PARAM(T6,5)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s,%s,%s,%s) const", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()());
  }
};

template <class Tc, class Tr, class T1, class T2, class T3, class T4, class T5, class T6, class T7> 
struct ATOM_CommandCaller<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7)>: public ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7)>
{
  typedef Tr return_type;
  typedef ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7)> base_type;
  ATOM_CommandCaller (const char *name, ATOM_DereferenceFunc deref, Tr (Tc::*p)(T1,T2,T3,T4,T5,T6,T7)):base_type(name,deref,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>()(scp, (((Tc*)pc)->*(this->func)) (__S_PARAM(T1,0),__S_PARAM(T2,1),__S_PARAM(T3,2),__S_PARAM(T4,3),
                                                               __S_PARAM(T5,4),__S_PARAM(T6,5),__S_PARAM(T7,6)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s,%s,%s,%s,%s)", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()());
  }
};

template <class Tc, class Tr, class T1, class T2, class T3, class T4, class T5, class T6, class T7> 
struct ATOM_CommandCaller<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7) const>: public ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7) const>
{
  typedef Tr return_type;
  typedef ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7) const> base_type;
  ATOM_CommandCaller (const char *name, ATOM_DereferenceFunc deref, Tr (Tc::*p)(T1,T2,T3,T4,T5,T6,T7) const):base_type(name,deref,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>()(scp, (((const Tc*)pc)->*(this->func)) (__S_PARAM(T1,0),__S_PARAM(T2,1),__S_PARAM(T3,2),__S_PARAM(T4,3),
                                                               __S_PARAM(T5,4),__S_PARAM(T6,5),__S_PARAM(T7,6)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s,%s,%s,%s,%s) const", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()());
  }
};

template <class Tc, class Tr, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8> 
struct ATOM_CommandCaller<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8)>: public ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8)>
{
  typedef Tr return_type;
  typedef ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8)> base_type;
  ATOM_CommandCaller (const char *name, ATOM_DereferenceFunc deref, Tr (Tc::*p)(T1,T2,T3,T4,T5,T6,T7,T8)):base_type(name,deref,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>()(scp, (((Tc*)pc)->*(this->func)) (__S_PARAM(T1,0),__S_PARAM(T2,1),__S_PARAM(T3,2),__S_PARAM(T4,3),
                                                               __S_PARAM(T5,4),__S_PARAM(T6,5),__S_PARAM(T7,6),__S_PARAM(T8,7)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s,%s,%s,%s,%s,%s)", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()());
  }
};

template <class Tc, class Tr, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8> 
struct ATOM_CommandCaller<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8) const>: public ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8) const>
{
  typedef Tr return_type;
  typedef ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8) const> base_type;
  ATOM_CommandCaller (const char *name, ATOM_DereferenceFunc deref, Tr (Tc::*p)(T1,T2,T3,T4,T5,T6,T7,T8) const):base_type(name,deref,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>()(scp, (((const Tc*)pc)->*(this->func)) (__S_PARAM(T1,0),__S_PARAM(T2,1),__S_PARAM(T3,2),__S_PARAM(T4,3),
                                                               __S_PARAM(T5,4),__S_PARAM(T6,5),__S_PARAM(T7,6),__S_PARAM(T8,7)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s,%s,%s,%s,%s,%s) const", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()());
  }
};

template <class Tc, class Tr, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9> 
struct ATOM_CommandCaller<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9)>: public ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9)>
{
  typedef Tr return_type;
  typedef ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9)> base_type;
  ATOM_CommandCaller (const char *name, ATOM_DereferenceFunc deref, Tr (Tc::*p)(T1,T2,T3,T4,T5,T6,T7,T8,T9)):base_type(name,deref,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>()(scp, (((Tc*)pc)->*(this->func)) (__S_PARAM(T1,0),__S_PARAM(T2,1),__S_PARAM(T3,2),__S_PARAM(T4,3),
                                                               __S_PARAM(T5,4),__S_PARAM(T6,5),__S_PARAM(T7,6),__S_PARAM(T8,7),
                                                               __S_PARAM(T9,8)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s,%s,%s,%s,%s,%s,%s)", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()());
  }
};

template <class Tc, class Tr, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9> 
struct ATOM_CommandCaller<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9) const>: public ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9) const>
{
  typedef Tr return_type;
  typedef ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9) const> base_type;
  ATOM_CommandCaller (const char *name, ATOM_DereferenceFunc deref, Tr (Tc::*p)(T1,T2,T3,T4,T5,T6,T7,T8,T9) const):base_type(name,deref,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>()(scp, (((const Tc*)pc)->*(this->func)) (__S_PARAM(T1,0),__S_PARAM(T2,1),__S_PARAM(T3,2),__S_PARAM(T4,3),
                                                               __S_PARAM(T5,4),__S_PARAM(T6,5),__S_PARAM(T7,6),__S_PARAM(T8,7),
                                                               __S_PARAM(T9,8)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s,%s,%s,%s,%s,%s,%s) const", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()());
  }
};

template <class Tc, class Tr, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10> 
struct ATOM_CommandCaller<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10)>: public ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10)>
{
  typedef Tr return_type;
  typedef ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10)> base_type;
  ATOM_CommandCaller (const char *name, ATOM_DereferenceFunc deref, Tr (Tc::*p)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10)):base_type(name,deref,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>()(scp, (((Tc*)pc)->*(this->func)) (__S_PARAM(T1,0),__S_PARAM(T2,1),__S_PARAM(T3,2),__S_PARAM(T4,3),
                                                               __S_PARAM(T5,4),__S_PARAM(T6,5),__S_PARAM(T7,6),__S_PARAM(T8,7),
                                                               __S_PARAM(T9,8),__S_PARAM(T10,9)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()());
  }
};

template <class Tc, class Tr, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10> 
struct ATOM_CommandCaller<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10) const>: public ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10) const>
{
  typedef Tr return_type;
  typedef ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10) const> base_type;
  ATOM_CommandCaller (const char *name, ATOM_DereferenceFunc deref, Tr (Tc::*p)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10) const):base_type(name,deref,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>()(scp, (((const Tc*)pc)->*(this->func)) (__S_PARAM(T1,0),__S_PARAM(T2,1),__S_PARAM(T3,2),__S_PARAM(T4,3),
                                                               __S_PARAM(T5,4),__S_PARAM(T6,5),__S_PARAM(T7,6),__S_PARAM(T8,7),
                                                               __S_PARAM(T9,8),__S_PARAM(T10,9)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s) const", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()());
  }
};

template <class Tc, class Tr, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11> 
struct ATOM_CommandCaller<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11)>: public ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11)>
{
  typedef Tr return_type;
  typedef ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11)> base_type;
  ATOM_CommandCaller (const char *name, ATOM_DereferenceFunc deref, Tr (Tc::*p)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11)):base_type(name,deref,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>()(scp, (((Tc*)pc)->*(this->func)) (__S_PARAM(T1,0),__S_PARAM(T2,1),__S_PARAM(T3,2),__S_PARAM(T4,3),
                                                             __S_PARAM(T5,4),__S_PARAM(T6,5),__S_PARAM(T7,6),__S_PARAM(T8,7),
                                                             __S_PARAM(T9,8),__S_PARAM(T10,9),__S_PARAM(T11,10)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()(), __Help__<T11>()());
  }
};

template <class Tc, class Tr, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11> 
struct ATOM_CommandCaller<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11) const>: public ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11) const>
{
  typedef Tr return_type;
  typedef ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11) const> base_type;
  ATOM_CommandCaller (const char *name, ATOM_DereferenceFunc deref, Tr (Tc::*p)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11) const):base_type(name,deref,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>()(scp, (((const Tc*)pc)->*(this->func)) (__S_PARAM(T1,0),__S_PARAM(T2,1),__S_PARAM(T3,2),__S_PARAM(T4,3),
                                                               __S_PARAM(T5,4),__S_PARAM(T6,5),__S_PARAM(T7,6),__S_PARAM(T8,7),
                                                               __S_PARAM(T9,8),__S_PARAM(T10,9),__S_PARAM(T11,10)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s) const", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()(), __Help__<T11>()());
  }
};

template <class Tc, class Tr, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12> 
struct ATOM_CommandCaller<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12)>: public ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12)>
{
  typedef Tr return_type;
  typedef ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12)> base_type;
  ATOM_CommandCaller (const char *name, ATOM_DereferenceFunc deref, Tr (Tc::*p)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12)):base_type(name,deref,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>()(scp, (((Tc*)pc)->*(this->func)) (__S_PARAM(T1,0),__S_PARAM(T2,1),__S_PARAM(T3,2),__S_PARAM(T4,3),
                                                             __S_PARAM(T5,4),__S_PARAM(T6,5),__S_PARAM(T7,6),__S_PARAM(T8,7),
                                                             __S_PARAM(T9,8),__S_PARAM(T10,9),__S_PARAM(T11,10),__S_PARAM(T12,11)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()(), __Help__<T11>()(),
      __Help__<T12>()());
  }
};

template <class Tc, class Tr, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12> 
struct ATOM_CommandCaller<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12) const>: public ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12) const>
{
  typedef Tr return_type;
  typedef ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12) const> base_type;
  ATOM_CommandCaller (const char *name, ATOM_DereferenceFunc deref, Tr (Tc::*p)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12) const):base_type(name,deref,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>()(scp, (((const Tc*)pc)->*(this->func)) (__S_PARAM(T1,0),__S_PARAM(T2,1),__S_PARAM(T3,2),__S_PARAM(T4,3),
                                                               __S_PARAM(T5,4),__S_PARAM(T6,5),__S_PARAM(T7,6),__S_PARAM(T8,7),
                                                               __S_PARAM(T9,8),__S_PARAM(T10,9),__S_PARAM(T11,10),__S_PARAM(T12,11)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s) const", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()(), __Help__<T11>()(),
      __Help__<T12>()());
  }
};

template <class Tc, class Tr, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13> 
struct ATOM_CommandCaller<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13)>: public ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13)>
{
  typedef Tr return_type;
  typedef ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13)> base_type;
  ATOM_CommandCaller (const char *name, ATOM_DereferenceFunc deref, Tr (Tc::*p)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13)):base_type(name,deref,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>()(scp, (((Tc*)pc)->*(this->func)) (__S_PARAM(T1,0),__S_PARAM(T2,1),__S_PARAM(T3,2),__S_PARAM(T4,3),
                                                             __S_PARAM(T5,4),__S_PARAM(T6,5),__S_PARAM(T7,6),__S_PARAM(T8,7),
                                                             __S_PARAM(T9,8),__S_PARAM(T10,9),__S_PARAM(T11,10),__S_PARAM(T12,11),
                                                             __S_PARAM(T13,12)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()(), __Help__<T11>()(),
      __Help__<T12>()(), __Help__<T13>()());
  }
};

template <class Tc, class Tr, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13> 
struct ATOM_CommandCaller<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13) const>: public ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13) const>
{
  typedef Tr return_type;
  typedef ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13) const> base_type;
  ATOM_CommandCaller (const char *name, ATOM_DereferenceFunc deref, Tr (Tc::*p)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13) const):base_type(name,deref,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>()(scp, (((const Tc*)pc)->*(this->func)) (__S_PARAM(T1,0),__S_PARAM(T2,1),__S_PARAM(T3,2),__S_PARAM(T4,3),
                                                               __S_PARAM(T5,4),__S_PARAM(T6,5),__S_PARAM(T7,6),__S_PARAM(T8,7),
                                                               __S_PARAM(T9,8),__S_PARAM(T10,9),__S_PARAM(T11,10),__S_PARAM(T12,11),
                                                               __S_PARAM(T13,12)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s) const", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()(), __Help__<T11>()(),
      __Help__<T12>()(), __Help__<T13>()());
  }
};

template <class Tc, class Tr, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14> 
struct ATOM_CommandCaller<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14)>: public ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14)>
{
  typedef Tr return_type;
  typedef ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14)> base_type;
  ATOM_CommandCaller (const char *name, ATOM_DereferenceFunc deref, Tr (Tc::*p)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14)):base_type(name,deref,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>()(scp, (((Tc*)pc)->*(this->func)) (__S_PARAM(T1,0),__S_PARAM(T2,1),__S_PARAM(T3,2),__S_PARAM(T4,3),
                                                             __S_PARAM(T5,4),__S_PARAM(T6,5),__S_PARAM(T7,6),__S_PARAM(T8,7),
                                                             __S_PARAM(T9,8),__S_PARAM(T10,9),__S_PARAM(T11,10),__S_PARAM(T12,11),
                                                             __S_PARAM(T13,12),__S_PARAM(T14,13)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()(), __Help__<T11>()(),
      __Help__<T12>()(), __Help__<T13>()(), __Help__<T14>()());
  }
};

template <class Tc, class Tr, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14> 
struct ATOM_CommandCaller<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14) const>: public ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14) const>
{
  typedef Tr return_type;
  typedef ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14) const> base_type;
  ATOM_CommandCaller (const char *name, ATOM_DereferenceFunc deref, Tr (Tc::*p)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14) const):base_type(name,deref,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>()(scp, (((const Tc*)pc)->*(this->func)) (__S_PARAM(T1,0),__S_PARAM(T2,1),__S_PARAM(T3,2),__S_PARAM(T4,3),
                                                               __S_PARAM(T5,4),__S_PARAM(T6,5),__S_PARAM(T7,6),__S_PARAM(T8,7),
                                                               __S_PARAM(T9,8),__S_PARAM(T10,9),__S_PARAM(T11,10),__S_PARAM(T12,11),
                                                               __S_PARAM(T13,12),__S_PARAM(T14,13)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s) const", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()(), __Help__<T11>()(),
      __Help__<T12>()(), __Help__<T13>()(), __Help__<T14>()());
  }
};

template <class Tc, class Tr, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15> 
struct ATOM_CommandCaller<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15)>: public ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15)>
{
  typedef Tr return_type;
  typedef ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15)> base_type;
  ATOM_CommandCaller (const char *name, ATOM_DereferenceFunc deref, Tr (Tc::*p)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15)):base_type(name,deref,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>()(scp, (((Tc*)pc)->*(this->func)) (__S_PARAM(T1,0),__S_PARAM(T2,1),__S_PARAM(T3,2),__S_PARAM(T4,3),
                                                             __S_PARAM(T5,4),__S_PARAM(T6,5),__S_PARAM(T7,6),__S_PARAM(T8,7),
                                                             __S_PARAM(T9,8),__S_PARAM(T10,9),__S_PARAM(T11,10),__S_PARAM(T12,11),
                                                             __S_PARAM(T13,12),__S_PARAM(T14,13),__S_PARAM(T15,14)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()(), __Help__<T11>()(),
      __Help__<T12>()(), __Help__<T13>()(), __Help__<T14>()(), __Help__<T15>()());
  }
};

template <class Tc, class Tr, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15> 
struct ATOM_CommandCaller<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15) const>: public ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15) const>
{
  typedef Tr return_type;
  typedef ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15) const> base_type;
  ATOM_CommandCaller (const char *name, ATOM_DereferenceFunc deref, Tr (Tc::*p)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15) const):base_type(name,deref,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>()(scp, (((const Tc*)pc)->*(this->func)) (__S_PARAM(T1,0),__S_PARAM(T2,1),__S_PARAM(T3,2),__S_PARAM(T4,3),
                                                               __S_PARAM(T5,4),__S_PARAM(T6,5),__S_PARAM(T7,6),__S_PARAM(T8,7),
                                                               __S_PARAM(T9,8),__S_PARAM(T10,9),__S_PARAM(T11,10),__S_PARAM(T12,11),
                                                               __S_PARAM(T13,12),__S_PARAM(T14,13),__S_PARAM(T15,14)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s) const", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()(), __Help__<T11>()(),
      __Help__<T12>()(), __Help__<T13>()(), __Help__<T14>()(), __Help__<T15>()());
  }
};

template <class Tc, class Tr, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16> 
struct ATOM_CommandCaller<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16)>: public ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16)>
{
  typedef Tr return_type;
  typedef ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16)> base_type;
  ATOM_CommandCaller (const char *name, ATOM_DereferenceFunc deref, Tr (Tc::*p)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16)):base_type(name,deref,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>()(scp, (((Tc*)pc)->*(this->func)) (__S_PARAM(T1,0),__S_PARAM(T2,1),__S_PARAM(T3,2),__S_PARAM(T4,3),
                                                             __S_PARAM(T5,4),__S_PARAM(T6,5),__S_PARAM(T7,6),__S_PARAM(T8,7),
                                                             __S_PARAM(T9,8),__S_PARAM(T10,9),__S_PARAM(T11,10),__S_PARAM(T12,11),
                                                             __S_PARAM(T13,12),__S_PARAM(T14,13),__S_PARAM(T15,14),__S_PARAM(T16,15)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()(), __Help__<T11>()(),
      __Help__<T12>()(), __Help__<T13>()(), __Help__<T14>()(), __Help__<T15>()(), __Help__<T16>()());
  }
};

template <class Tc, class Tr, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16> 
struct ATOM_CommandCaller<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16) const>: public ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16) const>
{
  typedef Tr return_type;
  typedef ATOM_CommandCallerBaseT<Tr (Tc::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16) const> base_type;
  ATOM_CommandCaller (const char *name, ATOM_DereferenceFunc deref, Tr (Tc::*p)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16) const):base_type(name,deref,p) {}
  virtual int invoke (void *pc, ATOM_Script *scp, void *context) {
    return __Return__<Tr>()(scp, (((const Tc*)pc)->*(this->func)) (__S_PARAM(T1,0),__S_PARAM(T2,1),__S_PARAM(T3,2),__S_PARAM(T4,3),
                                                               __S_PARAM(T5,4),__S_PARAM(T6,5),__S_PARAM(T7,6),__S_PARAM(T8,7),
                                                               __S_PARAM(T9,8),__S_PARAM(T10,9),__S_PARAM(T11,10),__S_PARAM(T12,11),
                                                               __S_PARAM(T13,12),__S_PARAM(T14,13),__S_PARAM(T15,14),__S_PARAM(T16,15)), this->_deref);
  }
  virtual void helpstr (char *buffer) {
    sprintf (buffer, "%s %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s) const", __Help__<Tr>()(), this->_name, __Help__<T1>()(), __Help__<T2>()(), __Help__<T3>()(), __Help__<T4>()(),
      __Help__<T5>()(), __Help__<T6>()(), __Help__<T7>()(), __Help__<T8>()(), __Help__<T9>()(), __Help__<T10>()(), __Help__<T11>()(),
      __Help__<T12>()(), __Help__<T13>()(), __Help__<T14>()(), __Help__<T15>()(), __Help__<T16>()());
  }
};

namespace internal
{
  template <class T, class R>
  struct FunctionCallerImpl
  {
    ATOM_FunctionCallerBase * operator () (const char *name, T func, ATOM_DereferenceFunc deref) {
      return ATOM_NEW(ATOM_FunctionCaller<T>, name, deref, func);
    }
  };

  template <class T>
  struct FunctionCallerImpl<T, void>
  {
    ATOM_FunctionCallerBase * operator () (const char *name, T func, ATOM_DereferenceFunc deref) {
      return ATOM_NEW(ATOM_FunctionCaller<T>, name, func);
    }
  };
} // namespace internal

template <class T>
ATOM_FunctionCallerBase *createFunctionCaller (const char *name, T func) {
  typedef typename ATOM_FunctionCaller<T>::return_type ReturnType;
  return internal::FunctionCallerImpl<T, ReturnType>()(name, func, (ATOM_DereferenceFunc)0);
  //return ATOM_NEW(ATOM_FunctionCaller<T>, name, func);
}

template <class T>
ATOM_FunctionCallerBase *createFunctionCallerRef (const char *name, T func, ATOM_DereferenceFunc deref) {
  typedef typename ATOM_FunctionCaller<T>::return_type ReturnType;
  static const bool isVoid = ATOM_IsSame<ReturnType, void>::result;
  ATOM_STATIC_ASSERT(!isVoid);
  return internal::FunctionCallerImpl<T, ReturnType>()(name, func, deref);
}

namespace internal
{
  template <class T, class R>
  struct CommandCallerImpl
  {
    ATOM_CommandCallerBase * operator () (const char *name, T func, ATOM_DereferenceFunc deref) {
      return ATOM_NEW(ATOM_CommandCaller<T>, name, deref, func);
    }
  };

  template <class T>
  struct CommandCallerImpl<T, void>
  {
    ATOM_CommandCallerBase * operator () (const char *name, T func, ATOM_DereferenceFunc deref) {
      return ATOM_NEW(ATOM_CommandCaller<T>, name, func);
    }
  };
} // namespace internal

template <class T>
ATOM_CommandCallerBase *createCommandCaller (const char *name, T func) {
  typedef typename ATOM_CommandCaller<T>::return_type ReturnType;
  return internal::CommandCallerImpl<T, ReturnType>()(name, func, (ATOM_DereferenceFunc)0);
}

template <class T>
ATOM_CommandCallerBase *createCommandCallerRef (const char *name, T func, ATOM_DereferenceFunc deref) {
  typedef typename ATOM_CommandCaller<T>::return_type ReturnType;
  static const bool isVoid = ATOM_IsSame<ReturnType, void>::result;
  ATOM_STATIC_ASSERT(!isVoid);
  return internal::CommandCallerImpl<T, ReturnType>()(name, func, deref);
}

template <class T>
ATOM_IndexCommandCallerBase *createIndexCommandCaller (T func) {
  return ATOM_NEW(ATOM_IndexCommandCaller<T>, (ATOM_DereferenceFunc)0, func);
}

template <class T>
ATOM_IndexCommandCallerBase *createIndexCommandCallerRef (T func, ATOM_DereferenceFunc deref) {
  return ATOM_NEW(ATOM_IndexCommandCaller<T>, deref, func);
}

template <class T>
ATOM_NewIndexCommandCallerBase *createNewIndexCommandCaller (T func) {
  return ATOM_NEW(ATOM_NewIndexCommandCaller<T>, func);
}

template <class Tr, class Tw>
ATOM_AttributeAccessorBase *createAttribAccessor (const char *name, Tr r, Tw w) {
  typedef ATOM_AttributeAccessor<Tr, Tw> AttribType;
  return ATOM_NEW(AttribType, name, (ATOM_DereferenceFunc)0, r, w);
}

template <class Tr, class Tw>
ATOM_AttributeAccessorBase *createAttribAccessorRef (const char *name, Tr r, Tw w, ATOM_DereferenceFunc deref) {
  typedef ATOM_AttributeAccessor<Tr, Tw> AttribType;
  return ATOM_NEW(AttribType, name, deref, r, w);
}

template <class Tr>
ATOM_AttributeAccessorBase *createAttribAccessorReadonly (const char *name, Tr r) {
  return ATOM_NEW(ATOM_AttributeAccessorReadonly<Tr>, name, (ATOM_DereferenceFunc)0, r);
}

template <class Tr>
ATOM_AttributeAccessorBase *createAttribAccessorReadonlyRef (const char *name, Tr r, ATOM_DereferenceFunc deref ) {
  return ATOM_NEW(ATOM_AttributeAccessorReadonly<Tr>, name, deref, r);
}

template <class T>
ATOM_FunctionCallerBase *createConstantAccessor (const char *name, T val) {
  return ATOM_NEW(ATOM_ConstantAccessorT<T>, name, (ATOM_DereferenceFunc)0, val);
}

template <class T>
ATOM_FunctionCallerBase *createConstantAccessorRef (const char *name, T val, ATOM_DereferenceFunc deref) {
  return ATOM_NEW(ATOM_ConstantAccessorT<T>, name, deref, val);
}

#pragma warning(pop)

#endif // __ATOM_SCRIPT_FUNCTBIND_H


