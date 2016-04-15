#ifndef __ATOM_SCRIPT_VAR_H
#define __ATOM_SCRIPT_VAR_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_kernel.h"
#include "basedefs.h"

#pragma warning(push)
#pragma warning(disable:4995)

class ATOM_ScriptVariant
{
public:
  enum {
    TYPE_NONE,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_BOOL,
    TYPE_STRING,
    TYPE_POINTER,
    TYPE_ARRAY
  };

public:
  ATOM_ScriptVariant (void);
  ATOM_ScriptVariant (long long val);
  ATOM_ScriptVariant (float val);
  ATOM_ScriptVariant (bool val);
  ATOM_ScriptVariant (const char *val);
  ATOM_ScriptVariant (void *val);
  ATOM_ScriptVariant (const ATOM_ScriptVariant &rhs);
  ATOM_ScriptVariant (const ATOM_ScriptVariant *array, int count);
  ~ATOM_ScriptVariant (void);
  void swap (ATOM_ScriptVariant &rhs);
  ATOM_ScriptVariant & operator = (const ATOM_ScriptVariant &rhs);

public:
  void reset (void);
  int getType (void) const;
  long long asInteger (void) const;
  float asFloat (void) const;
  bool asBool (void) const;
  const char *asString (void) const;
  void *asPointer (void) const;
  ATOM_ScriptVariant *asArray (void) const;
  int getArraySize (void) const;

private:
  int _M_type;
  int _M_arraysize;
  union
  {
    long long i;
    float f;
    bool b;
    char *s;
    void *p;
  };
};

template <class T> struct ATOM_ScriptVariantCast;

#define DECL_VARIANT_CAST(type, castfunc) \
template <> struct ATOM_ScriptVariantCast<type> { \
type operator () (const ATOM_ScriptVariant &var) { return (type)(var.castfunc()); } \
};

DECL_VARIANT_CAST(char, asInteger)
DECL_VARIANT_CAST(signed char, asInteger)
DECL_VARIANT_CAST(unsigned char, asInteger)
DECL_VARIANT_CAST(short, asInteger)
DECL_VARIANT_CAST(unsigned short, asInteger)
DECL_VARIANT_CAST(int, asInteger)
DECL_VARIANT_CAST(unsigned int, asInteger)
DECL_VARIANT_CAST(long, asInteger)
DECL_VARIANT_CAST(unsigned long, asInteger)
DECL_VARIANT_CAST(float, asFloat)
DECL_VARIANT_CAST(double, asFloat)
DECL_VARIANT_CAST(bool, asBool)
DECL_VARIANT_CAST(const char*, asString)
#ifdef ATOM_STL_ALLOCATOR
DECL_VARIANT_CAST(ATOM_STRING, asString)
#endif
DECL_VARIANT_CAST(std::string, asString)
DECL_VARIANT_CAST(ATOM_ScriptVariant*, asArray)

template <class T> struct ATOM_ScriptVariantCast<T*> {
  T * operator () (const ATOM_ScriptVariant &var) { return (T*)var.asPointer(); }
};

inline ATOM_ScriptVariant::ATOM_ScriptVariant (void) {
  _M_type = TYPE_NONE;
  _M_arraysize = 0;
}

inline ATOM_ScriptVariant::ATOM_ScriptVariant (long long val) {
  _M_type = TYPE_INT;
  i = val;
  _M_arraysize = 0;
}

inline ATOM_ScriptVariant::ATOM_ScriptVariant (float val) {
  _M_type = TYPE_FLOAT;
  f = val;
  _M_arraysize = 0;
}

inline ATOM_ScriptVariant::ATOM_ScriptVariant (bool val) {
  _M_type = TYPE_BOOL;
  b = val;
  _M_arraysize = 0;
}

inline ATOM_ScriptVariant::ATOM_ScriptVariant (const char *val) {
  _M_arraysize = 0;
  _M_type = TYPE_STRING;
  if (val)
  {
    s = (char*) ATOM_MALLOC (strlen(val) + 1);
    strcpy (s, val);
  }
  else
  {
    s = 0;
  }
}

inline ATOM_ScriptVariant::ATOM_ScriptVariant (void *val) {
  _M_type = TYPE_POINTER;
  p = val;
  _M_arraysize = 0;
}

inline ATOM_ScriptVariant::ATOM_ScriptVariant (const ATOM_ScriptVariant *array, int count) {
  _M_type = TYPE_ARRAY;
  _M_arraysize = count;
  p = 0;
  if (count > 0)
  {
    ATOM_ScriptVariant *v = ATOM_NEW_ARRAY(ATOM_ScriptVariant, count);
    for (int i = 0; i < count; ++i)
    {
      v[i] = array[i];
    }
    p = v;
  }
}

inline ATOM_ScriptVariant::ATOM_ScriptVariant (const ATOM_ScriptVariant &rhs) {
  _M_type = rhs._M_type;
  switch (_M_type)
  {
  case TYPE_INT:
    i = rhs.i;
    break;
  case TYPE_FLOAT:
    f = rhs.f;
    break;
  case TYPE_BOOL:
    b = rhs.b;
    break;
  case TYPE_STRING:
    if (rhs.s)
    {
      s = (char*) ATOM_MALLOC (strlen(rhs.s) + 1);
      strcpy (s, rhs.s);
    }
    else
    {
      s = 0;
    }
    break;
  case TYPE_POINTER:
    p = rhs.p;
    break;
  case TYPE_ARRAY:
    {
      _M_arraysize = rhs._M_arraysize;
      p = 0;
      if (_M_arraysize)
      {
        ATOM_ScriptVariant *v = ATOM_NEW_ARRAY(ATOM_ScriptVariant, _M_arraysize);
        ATOM_ScriptVariant *s = (ATOM_ScriptVariant*)rhs.p;
        for (int i = 0; i < _M_arraysize; ++i)
        {
          v[i] = s[i];
        }
        p = v;
      }
      break;
    }
    break;
  default:
    break;
  }
}

inline ATOM_ScriptVariant::~ATOM_ScriptVariant (void) {
  reset ();
}

inline void ATOM_ScriptVariant::swap (ATOM_ScriptVariant &rhs) {
  std::swap (_M_type, rhs._M_type);
  std::swap (i, rhs.i);
}

inline ATOM_ScriptVariant & ATOM_ScriptVariant::operator = (const ATOM_ScriptVariant &rhs) {
  ATOM_ScriptVariant tmp(rhs);
  swap (tmp);
  return *this;
}

inline int ATOM_ScriptVariant::getType (void) const {
  return _M_type;
}

inline long long ATOM_ScriptVariant::asInteger (void) const {
  return i;
}

inline float ATOM_ScriptVariant::asFloat (void) const {
  return f;
}

inline bool ATOM_ScriptVariant::asBool (void) const {
  return b;
}

inline const char *ATOM_ScriptVariant::asString (void) const {
  return s;
}

inline void *ATOM_ScriptVariant::asPointer (void) const {
  return p;
}

inline ATOM_ScriptVariant *ATOM_ScriptVariant::asArray (void) const {
  return (ATOM_ScriptVariant*)p;
}

inline int ATOM_ScriptVariant::getArraySize (void) const {
  return _M_arraysize;
}

inline void ATOM_ScriptVariant::reset (void) {
  if (_M_type == TYPE_STRING)
  {
    ATOM_FREE (s);
  }
  else if (_M_type == TYPE_ARRAY)
  {
    ATOM_DELETE((ATOM_ScriptVariant *)p);
  }
  _M_type = TYPE_NONE;
}

#pragma warning(pop)

typedef ATOM_VECTOR<ATOM_ScriptVariant> ATOM_ScriptArguments;

class ATOM_ScriptArgs: public ATOM_ScriptArguments
{
public:
	ATOM_ScriptArgs(void)
	{
	}

	explicit ATOM_ScriptArgs(const ATOM_ScriptVariant& firstArg)
		: ATOM_ScriptArguments(1, firstArg)
	{
	}

	ATOM_ScriptArgs& operator()(const ATOM_ScriptVariant& newArg)
	{
		this->push_back (newArg);
		return *this;
	}
};

#endif // __ATOM_SCRIPT_VAR_H
