#ifndef __ATOM_UTILS_TYPETRAITS_H
#define __ATOM_UTILS_TYPETRAITS_H

#include <cstddef>
#include "config.h"
#include "intlogic.h"
#include "falsetrue.h"
#include "staticassertion.h"

#define ATOM_TYPETRAITS_DECL(classname, testtype, testresult) \
  template <class testtype> struct classname { \
    static const bool result = testresult; \
  }; 

#define ATOM_TYPETRAITS_DECL_SPEC(classname, spectype, testresult) \
  template <> struct classname<spectype> { \
    static const bool result = testresult; \
  };

#define ATOM_TYPETRAITS_DECL_PARTIAL_SPEC1_2(classdecl1, classdecl2, classname, spectype, testresult) \
  template <classdecl1, classdecl2> struct classname<spectype> { \
    static const bool result = testresult; \
  };

#define ATOM_TYPETRAITS_DECL_PARTIAL_SPEC1_1(classdecl, classname, spectype, testresult) \
  template <classdecl> struct classname<spectype> { \
    static const bool result = testresult; \
  };

#define ATOM_TYPETRAITS_DECL_SPEC_CONST(classname, spectype, testresult) \
  ATOM_TYPETRAITS_DECL_SPEC(classname, spectype, testresult) \
  ATOM_TYPETRAITS_DECL_SPEC(classname, spectype const, testresult)

#define ATOM_TYPETRAITS_DECL_SPEC_VOLATILE(classname, spectype, testresult) \
  ATOM_TYPETRAITS_DECL_SPEC(classname, spectype, testresult) \
  ATOM_TYPETRAITS_DECL_SPEC(classname, spectype volatile, testresult)

#define ATOM_TYPETRAITS_DECL_SPEC_CONST_VOLATILE(classname, spectype, testresult) \
  ATOM_TYPETRAITS_DECL_SPEC(classname, spectype, testresult) \
  ATOM_TYPETRAITS_DECL_SPEC(classname, spectype const, testresult) \
  ATOM_TYPETRAITS_DECL_SPEC(classname, spectype volatile, testresult) \
  ATOM_TYPETRAITS_DECL_SPEC(classname, spectype const volatile, testresult)

  // same type test
  template <class T, class U>
  struct ATOM_IsSame
  {
    static const bool result = false;
  };

  template <class T>
  struct ATOM_IsSame<T, T>
  {
    static const bool result = true;
  };

  // array type test

  ATOM_TYPETRAITS_DECL(ATOM_IsArray, T, false)
  #ifndef ATOM_COMPILER_NO_ARRAYTYPE_SPEC
  ATOM_TYPETRAITS_DECL_PARTIAL_SPEC1_2(class T, std::size_t N, ATOM_IsArray, T[N], true)
  ATOM_TYPETRAITS_DECL_PARTIAL_SPEC1_2(class T, std::size_t N, ATOM_IsArray, T const[N], true)
  ATOM_TYPETRAITS_DECL_PARTIAL_SPEC1_2(class T, std::size_t N, ATOM_IsArray, T volatile[N], true)
  ATOM_TYPETRAITS_DECL_PARTIAL_SPEC1_2(class T, std::size_t N, ATOM_IsArray, T const volatile[N], true)
  # if !(defined(__BORLANDC__) && (__BORLANDC__ < 0x600))
  ATOM_TYPETRAITS_DECL_PARTIAL_SPEC1_1(class T,ATOM_IsArray,T[],true)
  ATOM_TYPETRAITS_DECL_PARTIAL_SPEC1_1(class T,ATOM_IsArray,T const[],true)
  ATOM_TYPETRAITS_DECL_PARTIAL_SPEC1_1(class T,ATOM_IsArray,T volatile[],true)
  ATOM_TYPETRAITS_DECL_PARTIAL_SPEC1_1(class T,ATOM_IsArray,T const volatile[],true)
  # endif
  #endif

  // remove const volatile modifier

  namespace internal
  {
    template <class T> struct CVTraitsImpl {};

    template <class T>
    struct CVTraitsImpl<T*>
    {
      static const bool is_const = false;
      static const bool is_volatile = false;
      typedef T unqualifiered_type;
    };

    template <class T>
    struct CVTraitsImpl<const T*>
    {
      static const bool is_const = true;
      static const bool is_volatile = false;
      typedef T unqualifiered_type;
    };

    template <class T>
    struct CVTraitsImpl<volatile T*>
    {
      static const bool is_const = false;
      static const bool is_volatile = true;
      typedef T unqualifiered_type;
    };

    template <class T>
    struct CVTraitsImpl<const volatile T*>
    {
      static const bool is_const = true;
      static const bool is_volatile = true;
      typedef T unqualifiered_type;
    };
  }

  template <class T>
  struct ATOM_RemoveConstVolatile
  {
    typedef typename internal::CVTraitsImpl<T*>::unqualifiered_type type;
  };

  template <class T>
  struct ATOM_RemoveConstVolatile<T&>
  {
    typedef T& type;
  };

  #ifndef ATOM_COMPILER_NO_ARRAYTYPE_SPEC
  template <class T, std::size_t N>
  struct ATOM_RemoveConstVolatile<T const[N]>
  {
    typedef T type[N];
  };

  template <class T, std::size_t N>
  struct ATOM_RemoveConstVolatile<T volatile[N]>
  {
    typedef T type[N];
  };

  template <class T, std::size_t N>
  struct ATOM_RemoveConstVolatile<T const volatile[N]>
  {
    typedef T type[N];
  };
  #endif

  // add reference 

  namespace internal
  {
    template <class T> struct AddReferenceImpl { typedef T & type; };
    template <class T> struct AddReferenceImpl<T&> { typedef T& type; };
    template <> struct AddReferenceImpl<void> { typedef void type; };
    template <> struct AddReferenceImpl<void const> { typedef void const type; };
    template <> struct AddReferenceImpl<void volatile> { typedef void volatile type; };
    template <> struct AddReferenceImpl<void const volatile> { typedef void const volatile type; };
  }

  template <class T>
  struct ATOM_AddReference
  {
    typedef typename internal::AddReferenceImpl<T>::type type;
  };

  // Remove reference

  template <class T>
  struct ATOM_RemoveReference
  {
    typedef T type;
  };

  template <class T>
  struct ATOM_RemoveReference<T &>
  {
    typedef T type;
  };

  // test for conversion

  namespace internal
  {
    template <class From, class To>
    struct IsConvertibleBasicImpl
    {
      static ATOM_no_type check (...);
      static ATOM_yes_type check (To);
      static From from;
      static const bool value = sizeof(check(from)) == sizeof(ATOM_yes_type);
    };

  #if !defined(__BORLANDC__) || __BORLANDC__ > 0x551
    template <class From, class To>
    struct IsConvertibleImpl
    {
      typedef typename ATOM_AddReference<From>::type ref_type;
      static const bool value =
      ATOM_IntAnd<IsConvertibleBasicImpl<ref_type,To>::value,
          ATOM_IntNot<ATOM_IsArray<To>::result>::value>::value;
    };
  #endif

    template<> struct IsConvertibleImpl<void,void> { static const bool value = true; };
    template<> struct IsConvertibleImpl<void,void const> { static const bool value = true; };
    template<> struct IsConvertibleImpl<void,void volatile> { static const bool value = true; };
    template<> struct IsConvertibleImpl<void,void const volatile> { static const bool value = true; };
    template<> struct IsConvertibleImpl<void const,void> { static const bool value = true; };
    template<> struct IsConvertibleImpl<void const,void const> { static const bool value = true; };
    template<> struct IsConvertibleImpl<void const,void volatile> { static const bool value = true; };
    template<> struct IsConvertibleImpl<void const,void const volatile> { static const bool value = true; };
    template<> struct IsConvertibleImpl<void volatile,void> { static const bool value = true; };
    template<> struct IsConvertibleImpl<void volatile,void const> { static const bool value = true; };
    template<> struct IsConvertibleImpl<void volatile,void volatile> { static const bool value = true; };
    template<> struct IsConvertibleImpl<void volatile,void const volatile> { static const bool value = true; };
    template<> struct IsConvertibleImpl<void const volatile,void> { static const bool value = true; };
    template<> struct IsConvertibleImpl<void const volatile,void const> { static const bool value = true; };
    template<> struct IsConvertibleImpl<void const volatile,void volatile> { static const bool value = true; };
    template<> struct IsConvertibleImpl<void const volatile,void const volatile> { static const bool value = true; };
    template<class To> struct IsConvertibleImpl<void, To> { static const bool value = false; };
    template<class To> struct IsConvertibleImpl<void const, To> { static const bool value = false; };
    template<class To> struct IsConvertibleImpl<void volatile, To> { static const bool value = false; };
    template<class To> struct IsConvertibleImpl<void const volatile, To> { static const bool value = false; };
    template<class From> struct IsConvertibleImpl<From, void> { static const bool value = false; };
    template<class From> struct IsConvertibleImpl<From, void const> { static const bool value = false; };
    template<class From> struct IsConvertibleImpl<From, void volatile> { static const bool value = false; };
    template<class From> struct IsConvertibleImpl<From, void const volatile> { static const bool value = false; };
  }

  template <class From, class To> struct ATOM_IsConvertible { static const bool result = internal::IsConvertibleImpl<From, To>::value; };

  #ifdef __GNUC__
  template <> struct ATOM_IsConvertible<float,char> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<float const,char> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<float volatile,char> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<float const volatile,char> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<float,signed char> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<float const,signed char> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<float volatile,signed char> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<float const volatile,signed char> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<float,unsigned char> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<float const,unsigned char> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<float volatile,unsigned char> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<float const volatile,unsigned char> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<float,signed short> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<float const,signed short> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<float volatile,signed short> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<float const volatile,signed short> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<float,unsigned short> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<float const,unsigned short> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<float volatile,unsigned short> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<float const volatile,unsigned short> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<float,signed int> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<float const,signed int> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<float volatile,signed int> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<float const volatile,signed int> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<float,unsigned int> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<float const,unsigned int> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<float volatile,unsigned int> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<float const volatile,unsigned int> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<float,signed long> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<float const,signed long> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<float volatile,signed long> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<float const volatile,signed long> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<float,unsigned long> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<float const,unsigned long> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<float volatile,unsigned long> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<float const volatile,unsigned long> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<float,signed long long> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<float const,signed long long> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<float volatile,signed long long> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<float const volatile,signed long long> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<float,unsigned long long> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<float const,unsigned long long> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<float volatile,unsigned long long> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<float const volatile,unsigned long long> { static const bool result = true; };

  template <> struct ATOM_IsConvertible<double,char> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<double const,char> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<double volatile,char> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<double const volatile,char> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<double,signed char> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<double const,signed char> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<double volatile,signed char> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<double const volatile,signed char> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<double,unsigned char> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<double const,unsigned char> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<double volatile,unsigned char> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<double const volatile,unsigned char> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<double,signed short> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<double const,signed short> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<double volatile,signed short> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<double const volatile,signed short> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<double,unsigned short> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<double const,unsigned short> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<double volatile,unsigned short> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<double const volatile,unsigned short> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<double,signed int> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<double const,signed int> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<double volatile,signed int> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<double const volatile,signed int> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<double,unsigned int> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<double const,unsigned int> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<double volatile,unsigned int> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<double const volatile,unsigned int> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<double,signed long> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<double const,signed long> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<double volatile,signed long> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<double const volatile,signed long> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<double,unsigned long> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<double const,unsigned long> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<double volatile,unsigned long> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<double const volatile,unsigned long> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<double,signed long long> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<double const,signed long long> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<double volatile,signed long long> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<double const volatile,signed long long> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<double,unsigned long long> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<double const,unsigned long long> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<double volatile,unsigned long long> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<double const volatile,unsigned long long> { static const bool result = true; };

  template <> struct ATOM_IsConvertible<long double,char> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<long double const,char> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<long double volatile,char> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<long double const volatile,char> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<long double,signed char> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<long double const,signed char> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<long double volatile,signed char> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<long double const volatile,signed char> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<long double,unsigned char> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<long double const,unsigned char> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<long double volatile,unsigned char> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<long double const volatile,unsigned char> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<long double,signed short> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<long double const,signed short> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<long double volatile,signed short> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<long double const volatile,signed short> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<long double,unsigned short> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<long double const,unsigned short> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<long double volatile,unsigned short> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<long double const volatile,unsigned short> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<long double,signed int> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<long double const,signed int> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<long double volatile,signed int> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<long double const volatile,signed int> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<long double,unsigned int> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<long double const,unsigned int> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<long double volatile,unsigned int> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<long double const volatile,unsigned int> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<long double,signed long> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<long double const,signed long> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<long double volatile,signed long> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<long double const volatile,signed long> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<long double,unsigned long> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<long double const,unsigned long> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<long double volatile,unsigned long> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<long double const volatile,unsigned long> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<long double,signed long long> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<long double const,signed long long> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<long double volatile,signed long long> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<long double const volatile,signed long long> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<long double,unsigned long long> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<long double const,unsigned long long> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<long double volatile,unsigned long long> { static const bool result = true; };
  template <> struct ATOM_IsConvertible<long double const volatile,unsigned long long> { static const bool result = true; };
  #endif

  // void type test

  ATOM_TYPETRAITS_DECL(ATOM_IsVoid, T, false)
  ATOM_TYPETRAITS_DECL_SPEC(ATOM_IsVoid, void, true)
  ATOM_TYPETRAITS_DECL_SPEC(ATOM_IsVoid, void const, true)
  ATOM_TYPETRAITS_DECL_SPEC(ATOM_IsVoid, void volatile, true)
  ATOM_TYPETRAITS_DECL_SPEC(ATOM_IsVoid, void const volatile, true)

  // integer type test

  ATOM_TYPETRAITS_DECL(ATOM_IsIntegral, T, false)
  ATOM_TYPETRAITS_DECL_SPEC_CONST_VOLATILE(ATOM_IsIntegral, char, true)
  ATOM_TYPETRAITS_DECL_SPEC_CONST_VOLATILE(ATOM_IsIntegral, bool, true)
  ATOM_TYPETRAITS_DECL_SPEC_CONST_VOLATILE(ATOM_IsIntegral, signed char, true)
  ATOM_TYPETRAITS_DECL_SPEC_CONST_VOLATILE(ATOM_IsIntegral, signed short, true)
  ATOM_TYPETRAITS_DECL_SPEC_CONST_VOLATILE(ATOM_IsIntegral, signed int, true)
  ATOM_TYPETRAITS_DECL_SPEC_CONST_VOLATILE(ATOM_IsIntegral, signed long, true)
  ATOM_TYPETRAITS_DECL_SPEC_CONST_VOLATILE(ATOM_IsIntegral, unsigned char, true)
  ATOM_TYPETRAITS_DECL_SPEC_CONST_VOLATILE(ATOM_IsIntegral, unsigned short, true)
  ATOM_TYPETRAITS_DECL_SPEC_CONST_VOLATILE(ATOM_IsIntegral, unsigned int, true)
  ATOM_TYPETRAITS_DECL_SPEC_CONST_VOLATILE(ATOM_IsIntegral, unsigned long, true)

  #if (defined(_MSC_VER) && (_MSC_VER == 1200)) || \
  (defined(__BORLANDC__) && (__BORLANDC__ == 0x600) && (_MSC_VER == 1200))
  ATOM_TYPETRAITS_DECL_SPEC_CONST_VOLATILE(ATOM_IsIntegral, unsigned __int8, true)
  ATOM_TYPETRAITS_DECL_SPEC_CONST_VOLATILE(ATOM_IsIntegral, __int8, true)
  ATOM_TYPETRAITS_DECL_SPEC_CONST_VOLATILE(ATOM_IsIntegral, unsigned __int16, true)
  ATOM_TYPETRAITS_DECL_SPEC_CONST_VOLATILE(ATOM_IsIntegral, __int16, true)
  ATOM_TYPETRAITS_DECL_SPEC_CONST_VOLATILE(ATOM_IsIntegral, unsigned __int32, true)
  ATOM_TYPETRAITS_DECL_SPEC_CONST_VOLATILE(ATOM_IsIntegral, __int32, true)
  # ifdef __BORLANDC__
  ATOM_TYPETRAITS_DECL_SPEC_CONST_VOLATILE(ATOM_IsIntegral, unsigned __int64,true)
  ATOM_TYPETRAITS_DECL_SPEC_CONST_VOLATILE(ATOM_IsIntegral, __int64, true)
  # endif
  #endif

  # if defined(ATOM_COMPILER_HAS_LONG_LONG)
  ATOM_TYPETRAITS_DECL_SPEC_CONST_VOLATILE(ATOM_IsIntegral, unsigned long long, true)
  ATOM_TYPETRAITS_DECL_SPEC_CONST_VOLATILE(ATOM_IsIntegral, long long, true)
  #elif defined(ATOM_COMPILER_HAS_MS_INT64)
  ATOM_TYPETRAITS_DECL_SPEC_CONST_VOLATILE(ATOM_IsIntegral, unsigned __int64, true)
  ATOM_TYPETRAITS_DECL_SPEC_CONST_VOLATILE(ATOM_IsIntegral, __int64, true)
  #endif

  // float type test

  ATOM_TYPETRAITS_DECL(ATOM_IsFloat, T, false)
  ATOM_TYPETRAITS_DECL_SPEC_CONST_VOLATILE(ATOM_IsFloat, float, true)
  ATOM_TYPETRAITS_DECL_SPEC_CONST_VOLATILE(ATOM_IsFloat, double, true)
  ATOM_TYPETRAITS_DECL_SPEC_CONST_VOLATILE(ATOM_IsFloat, long double, true)

  // number type test

  namespace internal
  {
    template <class T>
    struct IsNumberImpl
    {
      static const bool value = ATOM_IntOr<ATOM_IsIntegral<T>::result, ATOM_IsFloat<T>::result>::value;
    };
  }

  ATOM_TYPETRAITS_DECL(ATOM_IsNumber, T, internal::IsNumberImpl<T>::value)

  // pointer type test

  namespace internal
  {
    template <class T> struct IsPointerHelper { static const bool value = false; };
    template <class T> struct IsPointerHelper<T*> { static const bool value = true; };
    template <class T> struct IsPointerHelper<T* const> { static const bool value = true; };
    template <class T> struct IsPointerHelper<T* volatile> { static const bool value = true; };
    template <class T> struct IsPointerHelper<T* const volatile> { static const bool value = true; };

    template <class T>
    struct IsPointerImpl
    {
      static const bool value = IsPointerHelper<T>::value;
    };
  }

  ATOM_TYPETRAITS_DECL (ATOM_IsPointer, T, internal::IsPointerImpl<T>::value)
  ATOM_TYPETRAITS_DECL_PARTIAL_SPEC1_1 (class T, ATOM_IsPointer, T&, false)
  #ifdef ATOM_COMPILER_HAS_QUALIFIERS_ON_REFERENCE
  ATOM_TYPETRAITS_DECL_PARTIAL_SPEC1_1 (class T, ATOM_IsPointer, T& const, false)
  ATOM_TYPETRAITS_DECL_PARTIAL_SPEC1_1 (class T, ATOM_IsPointer, T& volatile, false)
  ATOM_TYPETRAITS_DECL_PARTIAL_SPEC1_1 (class T, ATOM_IsPointer, T& const volatile, false)
  #endif

  // reference type test

  ATOM_TYPETRAITS_DECL (ATOM_IsReference, T, false)
  ATOM_TYPETRAITS_DECL_PARTIAL_SPEC1_1 (class T, ATOM_IsReference, T&, true)

  #if defined(__BORLANDC__) && !defined(__COMO__) && (__BORLANDC__ < 0x600)
  ATOM_TYPETRAITS_DECL_PARTIAL_SPEC1_1 (class T, ATOM_IsReference, T& const, true)
  ATOM_TYPETRAITS_DECL_PARTIAL_SPEC1_1 (class T, ATOM_IsReference, T& volatile, true)
  ATOM_TYPETRAITS_DECL_PARTIAL_SPEC1_1 (class T, ATOM_IsReference, T& const volatile, true)
  #endif

  #if defined(__GNUC__) && (__GNUC__ < 3)
  ATOM_TYPETRAITS_DECL_PARTIAL_SPEC1_1(class T, ATOM_IsReference, T const, IsReference<T>::value)
  ATOM_TYPETRAITS_DECL_PARTIAL_SPEC1_1(class T, ATOM_IsReference, T volatile, IsReference<T>::value)
  ATOM_TYPETRAITS_DECL_PARTIAL_SPEC1_1(class T, ATOM_IsReference, T const volatile, IsReference<T>::value)
  ATOM_TYPETRAITS_DECL_PARTIAL_SPEC1_2(class T, unsigned long N, ATOM_IsReference, T[N], false)
  ATOM_TYPETRAITS_DECL_PARTIAL_SPEC1_2(class T, unsigned long N, ATOM_IsReference, const T[N], false)
  ATOM_TYPETRAITS_DECL_PARTIAL_SPEC1_2(class T, unsigned long N, ATOM_IsReference, volatile T[N], false)
  ATOM_TYPETRAITS_DECL_PARTIAL_SPEC1_2(class T, unsigned long N, ATOM_IsReference, const volatile T[N], false)
  #endif

  // function type test

  namespace internal
  {
    template <class R>
    struct IsFunctionPtrHelper
    {
      static const bool value = false;
    };

    template <class R>
    struct IsFunctionPtrHelper<R (*)()> { static const bool value = true; };

    template <class R,class T0>
    struct IsFunctionPtrHelper<R (*)(T0)> { static const bool value = true; };

    template <class R,class T0,class T1>
    struct IsFunctionPtrHelper<R (*)(T0,T1)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2>
    struct IsFunctionPtrHelper<R (*)(T0,T1,T2)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3>
    struct IsFunctionPtrHelper<R (*)(T0,T1,T2,T3)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4>
    struct IsFunctionPtrHelper<R (*)(T0,T1,T2,T3,T4)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5>
    struct IsFunctionPtrHelper<R (*)(T0,T1,T2,T3,T4,T5)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6>
    struct IsFunctionPtrHelper<R (*)(T0,T1,T2,T3,T4,T5,T6)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7>
    struct IsFunctionPtrHelper<R (*)(T0,T1,T2,T3,T4,T5,T6,T7)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8>
    struct IsFunctionPtrHelper<R (*)(T0,T1,T2,T3,T4,T5,T6,T7,T8)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9>
    struct IsFunctionPtrHelper<R (*)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10>
    struct IsFunctionPtrHelper<R (*)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11>
    struct IsFunctionPtrHelper<R (*)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12>
    struct IsFunctionPtrHelper<R (*)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12,class T13>
    struct IsFunctionPtrHelper<R (*)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12,class T13,class T14>
    struct IsFunctionPtrHelper<R (*)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12,class T13,class T14,class T15>
    struct IsFunctionPtrHelper<R (*)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12,class T13,class T14,class T15,class T16>
    struct IsFunctionPtrHelper<R (*)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12,class T13,class T14,class T15,class T16,class T17>
    struct IsFunctionPtrHelper<R (*)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12,class T13,class T14,class T15,class T16,class T17,class T18>
    struct IsFunctionPtrHelper<R (*)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17,T18)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12,class T13,class T14,class T15,class T16,class T17,class T18,class T19>
    struct IsFunctionPtrHelper<R (*)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17,T18,T19)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12,class T13,class T14,class T15,class T16,class T17,class T18,class T19,class T20>
    struct IsFunctionPtrHelper<R (*)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17,T18,T19,T20)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12,class T13,class T14,class T15,class T16,class T17,class T18,class T19,class T20,class T21>
    struct IsFunctionPtrHelper<R (*)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17,T18,T19,T20,T21)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12,class T13,class T14,class T15,class T16,class T17,class T18,class T19,class T20,class T21,class T22>
    struct IsFunctionPtrHelper<R (*)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17,T18,T19,T20,T21,T22)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12,class T13,class T14,class T15,class T16,class T17,class T18,class T19,class T20,class T21,class T22,class T23>
    struct IsFunctionPtrHelper<R (*)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17,T18,T19,T20,T21,T22,T23)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12,class T13,class T14,class T15,class T16,class T17,class T18,class T19,class T20,class T21,class T22,class T23,class T24>
    struct IsFunctionPtrHelper<R (*)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17,T18,T19,T20,T21,T22,T23,T24)> { static const bool value = true; };

  #if defined(WIN32) && !defined(__INTEL_COMPILER) && !defined(_M_IA64) && !defined(_M_AMD64)

    template <class R>
    struct IsFunctionPtrHelper<R (__stdcall *)()> { static const bool value = true; };

    template <class R,class T0>
    struct IsFunctionPtrHelper<R (__stdcall *)(T0)> { static const bool value = true; };

    template <class R,class T0,class T1>
    struct IsFunctionPtrHelper<R (__stdcall *)(T0,T1)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2>
    struct IsFunctionPtrHelper<R (__stdcall *)(T0,T1,T2)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3>
    struct IsFunctionPtrHelper<R (__stdcall *)(T0,T1,T2,T3)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4>
    struct IsFunctionPtrHelper<R (__stdcall *)(T0,T1,T2,T3,T4)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5>
    struct IsFunctionPtrHelper<R (__stdcall *)(T0,T1,T2,T3,T4,T5)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6>
    struct IsFunctionPtrHelper<R (__stdcall *)(T0,T1,T2,T3,T4,T5,T6)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7>
    struct IsFunctionPtrHelper<R (__stdcall *)(T0,T1,T2,T3,T4,T5,T6,T7)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8>
    struct IsFunctionPtrHelper<R (__stdcall *)(T0,T1,T2,T3,T4,T5,T6,T7,T8)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9>
    struct IsFunctionPtrHelper<R (__stdcall *)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10>
    struct IsFunctionPtrHelper<R (__stdcall *)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11>
    struct IsFunctionPtrHelper<R (__stdcall *)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12>
    struct IsFunctionPtrHelper<R (__stdcall *)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12,class T13>
    struct IsFunctionPtrHelper<R (__stdcall *)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12,class T13,class T14>
    struct IsFunctionPtrHelper<R (__stdcall *)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12,class T13,class T14,class T15>
    struct IsFunctionPtrHelper<R (__stdcall *)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12,class T13,class T14,class T15,class T16>
    struct IsFunctionPtrHelper<R (__stdcall *)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12,class T13,class T14,class T15,class T16,class T17>
    struct IsFunctionPtrHelper<R (__stdcall *)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12,class T13,class T14,class T15,class T16,class T17,class T18>
    struct IsFunctionPtrHelper<R (__stdcall *)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17,T18)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12,class T13,class T14,class T15,class T16,class T17,class T18,class T19>
    struct IsFunctionPtrHelper<R (__stdcall *)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17,T18,T19)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12,class T13,class T14,class T15,class T16,class T17,class T18,class T19,class T20>
    struct IsFunctionPtrHelper<R (__stdcall *)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17,T18,T19,T20)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12,class T13,class T14,class T15,class T16,class T17,class T18,class T19,class T20,class T21>
    struct IsFunctionPtrHelper<R (__stdcall *)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17,T18,T19,T20,T21)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12,class T13,class T14,class T15,class T16,class T17,class T18,class T19,class T20,class T21,class T22>
    struct IsFunctionPtrHelper<R (__stdcall *)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17,T18,T19,T20,T21,T22)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12,class T13,class T14,class T15,class T16,class T17,class T18,class T19,class T20,class T21,class T22,class T23>
    struct IsFunctionPtrHelper<R (__stdcall *)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17,T18,T19,T20,T21,T22,T23)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12,class T13,class T14,class T15,class T16,class T17,class T18,class T19,class T20,class T21,class T22,class T23,class T24>
    struct IsFunctionPtrHelper<R (__stdcall *)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17,T18,T19,T20,T21,T22,T23,T24)> { static const bool value = true; };

    template <class R>
    struct IsFunctionPtrHelper<R (__fastcall *)()> { static const bool value = true; };

    template <class R,class T0>
    struct IsFunctionPtrHelper<R (__fastcall *)(T0)> { static const bool value = true; };

    template <class R,class T0,class T1>
    struct IsFunctionPtrHelper<R (__fastcall *)(T0,T1)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2>
    struct IsFunctionPtrHelper<R (__fastcall *)(T0,T1,T2)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3>
    struct IsFunctionPtrHelper<R (__fastcall *)(T0,T1,T2,T3)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4>
    struct IsFunctionPtrHelper<R (__fastcall *)(T0,T1,T2,T3,T4)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5>
    struct IsFunctionPtrHelper<R (__fastcall *)(T0,T1,T2,T3,T4,T5)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6>
    struct IsFunctionPtrHelper<R (__fastcall *)(T0,T1,T2,T3,T4,T5,T6)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7>
    struct IsFunctionPtrHelper<R (__fastcall *)(T0,T1,T2,T3,T4,T5,T6,T7)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8>
    struct IsFunctionPtrHelper<R (__fastcall *)(T0,T1,T2,T3,T4,T5,T6,T7,T8)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9>
    struct IsFunctionPtrHelper<R (__fastcall *)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10>
    struct IsFunctionPtrHelper<R (__fastcall *)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11>
    struct IsFunctionPtrHelper<R (__fastcall *)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12>
    struct IsFunctionPtrHelper<R (__fastcall *)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12,class T13>
    struct IsFunctionPtrHelper<R (__fastcall *)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12,class T13,class T14>
    struct IsFunctionPtrHelper<R (__fastcall *)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12,class T13,class T14,class T15>
    struct IsFunctionPtrHelper<R (__fastcall *)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12,class T13,class T14,class T15,class T16>
    struct IsFunctionPtrHelper<R (__fastcall *)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12,class T13,class T14,class T15,class T16,class T17>
    struct IsFunctionPtrHelper<R (__fastcall *)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12,class T13,class T14,class T15,class T16,class T17,class T18>
    struct IsFunctionPtrHelper<R (__fastcall *)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17,T18)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12,class T13,class T14,class T15,class T16,class T17,class T18,class T19>
    struct IsFunctionPtrHelper<R (__fastcall *)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17,T18,T19)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12,class T13,class T14,class T15,class T16,class T17,class T18,class T19,class T20>
    struct IsFunctionPtrHelper<R (__fastcall *)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17,T18,T19,T20)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12,class T13,class T14,class T15,class T16,class T17,class T18,class T19,class T20,class T21>
    struct IsFunctionPtrHelper<R (__fastcall *)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17,T18,T19,T20,T21)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12,class T13,class T14,class T15,class T16,class T17,class T18,class T19,class T20,class T21,class T22>
    struct IsFunctionPtrHelper<R (__fastcall *)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17,T18,T19,T20,T21,T22)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12,class T13,class T14,class T15,class T16,class T17,class T18,class T19,class T20,class T21,class T22,class T23>
    struct IsFunctionPtrHelper<R (__fastcall *)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17,T18,T19,T20,T21,T22,T23)> { static const bool value = true; };

    template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12,class T13,class T14,class T15,class T16,class T17,class T18,class T19,class T20,class T21,class T22,class T23,class T24>
    struct IsFunctionPtrHelper<R (__fastcall *)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17,T18,T19,T20,T21,T22,T23,T24)> { static const bool value = true; };

#endif

    struct false_result
    {
      template <class T> struct result_
      {
        static const bool value = false;
      };
    };

    template <bool is_ref = true>
    struct IsFunctionChooser: false_result
    {
    };

    template <>
    struct IsFunctionChooser<false>
    {
      template <class T>
      struct result_: IsFunctionPtrHelper<T>
      {
      };
    };

    template <class T>
    struct IsFunctionImpl: IsFunctionChooser<ATOM_IsReference<T>::result>::template result_<T>
    {
    };
  } // namespace internal

  ATOM_TYPETRAITS_DECL (ATOM_IsFunction, T, internal::IsFunctionImpl<T>::value)

  // member pointer test
  ATOM_TYPETRAITS_DECL(ATOM_IsMemberPointer, T, false)
  ATOM_TYPETRAITS_DECL_PARTIAL_SPEC1_2(class T, class U, ATOM_IsMemberPointer, U T::*, true)

  // member function test

  namespace internal
  {
    template <class T>
    struct IsMemFunPointerImpl { static const bool value = false; };

    template <class R, class T  >
    struct IsMemFunPointerImpl<R (T::*)() > { static const bool value = true; };
    template <class R, class T  >
    struct IsMemFunPointerImpl<R (T::*)() const > { static const bool value = true; };
    template <class R, class T  >
    struct IsMemFunPointerImpl<R (T::*)() volatile > { static const bool value = true; };
    template <class R, class T  >
    struct IsMemFunPointerImpl<R (T::*)() const volatile > { static const bool value = true; };

    template <class R, class T ,  class T0>
    struct IsMemFunPointerImpl<R (T::*)( T0) > { static const bool value = true; };
    template <class R, class T ,  class T0>
    struct IsMemFunPointerImpl<R (T::*)( T0) const > { static const bool value = true; };
    template <class R, class T ,  class T0>
    struct IsMemFunPointerImpl<R (T::*)( T0) volatile > { static const bool value = true; };
    template <class R, class T ,  class T0>
    struct IsMemFunPointerImpl<R (T::*)( T0) const volatile > { static const bool value = true; };

    template <class R, class T ,  class T0 , class T1>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1) > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1) const > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1) volatile > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1) const volatile > { static const bool value = true; };

    template <class R, class T ,  class T0 , class T1 , class T2>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2) > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2) const > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2) volatile > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2) const volatile > { static const bool value = true; };

    template <class R, class T ,  class T0 , class T1 , class T2 , class T3>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3) > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3) const > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3) volatile > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3) const volatile > { static const bool value = true; };

    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4) > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4) const > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4) volatile > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4) const volatile > { static const bool value = true; };

    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5) > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5) const > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5) volatile > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5) const volatile > { static const bool value = true; };

    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6) > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6) const > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6) volatile > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6) const volatile > { static const bool value = true; };

    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7) > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7) const > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7) volatile > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7) const volatile > { static const bool value = true; };

    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8) > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8) const > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8) volatile > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8) const volatile > { static const bool value = true; };

    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9) > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9) const > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9) volatile > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9) const volatile > { static const bool value = true; };

    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10) > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10) const > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10) volatile > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10) const volatile > { static const bool value = true; };

    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11) > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11) const > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11) volatile > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11) const volatile > { static const bool value = true; };

    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12) > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12) const > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12) volatile > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12) const volatile > { static const bool value = true; };

    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13) > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13) const > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13) volatile > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13) const volatile > { static const bool value = true; };

    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14) > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14) const > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14) volatile > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14) const volatile > { static const bool value = true; };

    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15) > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15) const > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15) volatile > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15) const volatile > { static const bool value = true; };

    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16) > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16) const > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16) volatile > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16) const volatile > { static const bool value = true; };

    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17) > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17) const > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17) volatile > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17) const volatile > { static const bool value = true; };

    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18) > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18) const > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18) volatile > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18) const volatile > { static const bool value = true; };

    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19) > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19) const > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19) volatile > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19) const volatile > { static const bool value = true; };

    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19 , class T20>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19 , T20) > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19 , class T20>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19 , T20) const > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19 , class T20>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19 , T20) volatile > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19 , class T20>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19 , T20) const volatile > { static const bool value = true; };

    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19 , class T20 , class T21>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19 , T20 , T21) > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19 , class T20 , class T21>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19 , T20 , T21) const > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19 , class T20 , class T21>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19 , T20 , T21) volatile > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19 , class T20 , class T21>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19 , T20 , T21) const volatile > { static const bool value = true; };

    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19 , class T20 , class T21 , class T22>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19 , T20 , T21 , T22) > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19 , class T20 , class T21 , class T22>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19 , T20 , T21 , T22) const > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19 , class T20 , class T21 , class T22>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19 , T20 , T21 , T22) volatile > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19 , class T20 , class T21 , class T22>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19 , T20 , T21 , T22) const volatile > { static const bool value = true; };

    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19 , class T20 , class T21 , class T22 , class T23>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19 , T20 , T21 , T22 , T23) > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19 , class T20 , class T21 , class T22 , class T23>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19 , T20 , T21 , T22 , T23) const > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19 , class T20 , class T21 , class T22 , class T23>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19 , T20 , T21 , T22 , T23) volatile > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19 , class T20 , class T21 , class T22 , class T23>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19 , T20 , T21 , T22 , T23) const volatile > { static const bool value = true; };

    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19 , class T20 , class T21 , class T22 , class T23 , class T24>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19 , T20 , T21 , T22 , T23 , T24) > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19 , class T20 , class T21 , class T22 , class T23 , class T24>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19 , T20 , T21 , T22 , T23 , T24) const > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19 , class T20 , class T21 , class T22 , class T23 , class T24>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19 , T20 , T21 , T22 , T23 , T24) volatile > { static const bool value = true; };
    template <class R, class T ,  class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19 , class T20 , class T21 , class T22 , class T23 , class T24>
    struct IsMemFunPointerImpl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19 , T20 , T21 , T22 , T23 , T24) const volatile > { static const bool value = true; };

  } // namespace internal

  ATOM_TYPETRAITS_DECL (ATOM_IsMemberFunction, T, internal::IsMemFunPointerImpl<T>::value)

  // class type test

  namespace internal
  {
  #if defined(__GNUC__)  && !defined(__EDG_VERSION__)
    template <class U> ATOM_yes_type IsClassTester(void(U::*)(void));
    template <class U> ATOM_no_type IsClassTester(...);

    template <class T>
    struct IsClassImpl
    {
      static const bool value = sizeof(IsClassTester<T>(0)) == sizeof(ATOM_yes_type);
    };
  #else
    template <class T>
    struct IsClassImpl
    {
      template <class U> static ATOM_yes_type IsClassTester(void(U::*)(void));
      template <class U> static ATOM_no_type IsClassTester(...);
      static const bool value = sizeof(IsClassTester<T>(0)) == sizeof(ATOM_yes_type);
    };
  #endif
  } // namespace internal

  #ifdef __EDG_VERSION__
  template <class T>
  struct ATOM_IsClass
  {
    static const bool result = internal::IsClassImpl<typename ATOM_RemoveConstVolatile<T>::type>::value;
  };
  #else
  template <class T>
  struct ATOM_IsClass
  {
    static const bool result = internal::IsClassImpl<T>::value;
  };
  #endif

  // test for enumeration type

  namespace internal
  {
    struct IntConvertible
    {
      IntConvertible(int);
    };

    template <bool IsNumberOrReference = true>
    struct IsEnumHelper
    {
      template <class T>
      struct type
      {
        static const bool result = false;
      };
    };

    template <>
    struct IsEnumHelper<false>
    {
      template <class T>
      struct type: ATOM_IsConvertible<T, IntConvertible>
      {
      };
    };

    template <class T>
    struct IsEnumImpl
    {
      typedef ATOM_AddReference<T> ar;
      typedef typename ar::type r_type;
  #ifdef __GNUC__
      static const bool selector = ATOM_IntOr<ATOM_IsNumber<T>::result,
                                                          ATOM_IsReference<T>::result,
                                                          ATOM_IsFunction<T>::result,
                                                          ATOM_IsClass<T>::result>::value;
  #else
      static const bool selector = ATOM_IntOr<ATOM_IsNumber<T>::result,
                                                          ATOM_IsReference<T>::result,
                                                          ATOM_IsClass<T>::result>::value;
  #endif
      typedef IsEnumHelper<selector> se;
      typedef typename se::template type<r_type> helper;
      static const bool value = helper::result;
    };

    template <>
    struct IsEnumImpl<void>
    {
      static const bool value = false;
    };

    template <>
    struct IsEnumImpl<void const>
    {
      static const bool value = false;
    };

    template <>
    struct IsEnumImpl<void volatile>
    {
      static const bool value = false;
    };

    template <>
    struct IsEnumImpl<void const volatile>
    {
      static const bool value = false;
    };
  } // namespace internal

  template <class T>
  struct ATOM_IsEnum
  {
    static const bool result = internal::IsEnumImpl<T>::value;
  };

  // test for scalar type

  template <class T>
  struct ATOM_IsScalar
  {
    static const bool result = ATOM_IntOr<ATOM_IsNumber<T>::result, ATOM_IsEnum<T>::result, ATOM_IsPointer<T>::result, ATOM_IsMemberPointer<T>::result>::value;
  };

  template <> struct ATOM_IsScalar<void> { static const bool result = false; };
  template <> struct ATOM_IsScalar<void const> { static const bool result = false; };
  template <> struct ATOM_IsScalar<void volatile> { static const bool result = false; };
  template <> struct ATOM_IsScalar<void const volatile> { static const bool result = false; };

  // test for POD type
  template <class T>
  struct ATOM_IsPOD
  {
    static const bool result = ATOM_IntOr<ATOM_IsScalar<T>::result, ATOM_IsVoid<T>::result>::value;
  };

  #ifndef ATOM_COMPILER_NO_ARRAYTYPE_SPEC
  template <class T, std::size_t sz>
  struct ATOM_IsPOD<T[sz]>: ATOM_IsPOD<T>
  {
  };
  #endif

  // test for class derivation

  namespace internal
  {
    template <class B, class D>
    struct BDhelper
    {
      template <class T> static ATOM_yes_type check(D const volatile *, T);
      static ATOM_no_type check(B const volatile *, int);
    };

    template <class B, class D>
    struct IsBaseAndDerivedImpl2
    {
      struct Host
      {
        operator B const volatile *() const;
        operator D const volatile *();
      };
      static const bool value = sizeof(BDhelper<B,D>::check(Host(),0)) == sizeof(ATOM_yes_type);
    };

    template <class B, class D>
    struct IsBaseAndDerivedImpl3
    {
      static const bool value = false;
    };

    template <bool ic1, bool ic2, bool iss>
    struct IsBaseAndDerivedSelect
    {
      template <class T, class U>
      struct rebind
      {
        typedef IsBaseAndDerivedImpl3<T, U> type;
      };
    };

    template <>
    struct IsBaseAndDerivedSelect<true, true, false>
    {
      template <class T, class U>
      struct rebind
      {
        typedef IsBaseAndDerivedImpl2<T, U> type;
      };
    };

    template <class B, class D>
    struct IsBaseAndDerivedImpl
    {
      typedef typename ATOM_RemoveConstVolatile<B>::type ncvB;
      typedef typename ATOM_RemoveConstVolatile<D>::type ncvD;
      typedef IsBaseAndDerivedSelect< ATOM_IsClass<B>::result,
                                      ATOM_IsClass<D>::result,
                                      ATOM_IsSame<B, D>::result> selector;
      typedef typename selector::template rebind<ncvB, ncvD> binder;
      typedef typename binder::type bound_type;
      static const bool value = bound_type::value;
    };
  } // namespace internal

  template <class Base, class Derived>
  struct ATOM_IsBaseAndDerived
  {
    static const bool result = internal::IsBaseAndDerivedImpl<Base, Derived>::value;
  };

  template <class Base, class Derived>
  struct ATOM_IsBaseAndDerived<Base&, Derived>
  {
    static const bool result = false;
  };

  template <class Base, class Derived>
  struct ATOM_IsBaseAndDerived<Base, Derived&>
  {
    static const bool result = false;
  };

  template <class Base, class Derived>
  struct ATOM_IsBaseAndDerived<Base&, Derived&>
  {
    static const bool result = false;
  };

  namespace internal
  {
    template <class T>
    struct IsAbstractImp2
    {
      template <class U> static ATOM_no_type check_sig(U (*)[1]);
      template <class U> static ATOM_yes_type check_sig(...);
      ATOM_STATIC_ASSERT(sizeof(T) != 0);

      static const unsigned s1 = sizeof(check_sig<T>(0));
      static const bool value = (s1 == sizeof(ATOM_yes_type));
    };

    template <bool v>
    struct IsAbstractSelect
    {
      template <class T>
      struct rebind
      {
        typedef IsAbstractImp2<T> type;
      };
    };

    template <>
    struct IsAbstractSelect<false>
    {
      template <class T>
      struct rebind
      {
        typedef ATOM_FalseType type;
      };
    };

    template <class T>
    struct IsAbstractImp
    {
      typedef IsAbstractSelect<ATOM_IsClass<T>::result> selector;
      typedef typename selector::template rebind<T> binder;
      typedef typename binder::type type;
      static const bool result = type::value;
    };

  } // namespace internal

  template <class T>
  struct ATOM_IsAbstract
  {
    static const bool result = internal::IsAbstractImp<T>::result;
  };

#endif // __ATOM_UTILS_TYPETRAITS_H
