#ifndef __DEBUGHELP_HASHER_H
#define __DEBUGHELP_HASHER_H

#include <cstddef>
#include <string>

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4503)
#endif

template <class _Key>
struct ATOM_hasher
{
  unsigned operator() (_Key p) const 
  {
    return ((unsigned)p) >> 2;
  }
};

inline unsigned __hash_string(const char* str) 
{
  unsigned long i = 0; 
  for ( ; *str; ++str)
    i = 5 * i + *str;
  return unsigned(i);
}

inline unsigned __hash_string(const wchar_t* str) 
{
  unsigned long i = 0; 
  for ( ; *str; ++str)
    i = 5 * i + *str;
  return unsigned(i);
}

#if defined(__GNUC__)||(_MSC_VER >= 1310)

template <class T, class A>
struct ATOM_hasher<std::basic_string<T, std::char_traits<T>, A> >
{
  unsigned operator()(const T *str) const 
  {
    return __hash_string(str);
  }

  unsigned operator()(const std::basic_string<T, std::char_traits<T>, A> & str) const 
  {
    return __hash_string(str.c_str());
  }
};

#else

struct ATOM_hasher<std::string>
{
  unsigned operator()(const char *str) const 
  {
    return __hash_string(str);
  }

  unsigned operator()(const std::string& str) const 
  {
    return __hash_string(str.c_str());
  }
};

#endif

template <>
struct ATOM_hasher<char*>
{
  unsigned operator()(const char* str) const 
  {
    return __hash_string(str);
  }
};

template <>
struct ATOM_hasher<const char*>
{
  unsigned operator()(const char* str) const 
  {
    return __hash_string(str);
  }
};

template <>
struct ATOM_hasher<char>
{
  unsigned operator()(char x) const 
  {
    return x;
  }
};

template <>
struct ATOM_hasher<unsigned char>
{
  unsigned operator()(unsigned char x) const 
  {
    return x;
  }
};

template <>
struct ATOM_hasher<signed char>
{
  unsigned operator()(unsigned char x) const 
  {
    return x;
  }
};

template <>
struct ATOM_hasher<short>
{
  unsigned operator()(short x) const 
  {
    return x;
  }
};

template <>
struct ATOM_hasher<unsigned short>
{
  unsigned operator()(unsigned short x) const 
  {
    return x;
  }
};

template <>
struct ATOM_hasher<int>
{
  unsigned operator()(int x) const 
  {
    return x;
  }
};

template <>
struct ATOM_hasher<unsigned int>
{
  unsigned operator()(unsigned int x) const 
  {
    return x;
  }
};

template <>
struct ATOM_hasher<long>
{
  unsigned operator()(long x) const 
  {
    return x;
  }
};

template <>
struct ATOM_hasher<unsigned long>
{
  unsigned operator()(unsigned long x) const 
  {
    return x;
  }
};

#if defined(__GNUC__)||(_MSC_VER >= 1310)

template <class T>
struct ATOM_hasher<T*>
{
  unsigned operator() (const T *p) const 
  {
    return ((size_t)p) >> 2;
  }
};

template <class T>
struct ATOM_hasher<const T*>
{
  unsigned operator() (const T *p) const 
  {
    return ((size_t)p) >> 2;
  }
};

#endif

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#endif // __DEBUGHELP_HASHER_H

