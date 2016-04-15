#ifndef __ATOM_MATH_DATALIMIT_H
#define __ATOM_MATH_DATALIMIT_H

#include <limits.h>
#include <float.h>

// Data limits
static const signed char ATOM_MaxByteValue = SCHAR_MAX;
static const signed char ATOM_MinByteValue = SCHAR_MIN;
static const unsigned char ATOM_MaxUByteValue = UCHAR_MAX;
static const unsigned char ATOM_MinUByteValue = 0;
static const short ATOM_MaxShortValue = SHRT_MAX;
static const short ATOM_MinShortValue = SHRT_MIN;
static const unsigned short ATOM_MaxUShortValue = USHRT_MAX;
static const unsigned short ATOM_MinUShortValue = 0;
static const int ATOM_MaxIntValue = INT_MAX;
static const int ATOM_MinIntValue = INT_MIN;
static const unsigned int ATOM_MaxUIntValue = UINT_MAX;
static const unsigned int ATOM_MinUIntValue = 0;
static const long ATOM_MaxLongValue = LONG_MAX;
static const long ATOM_MinLongValue = LONG_MIN;
static const unsigned long ATOM_MaxULongValue = ULONG_MAX;
static const unsigned long ATOM_MinULongValue = 0;
static const float ATOM_MaxFloatValue = FLT_MAX;
static const float ATOM_MinFloatValue = FLT_MIN;
static const double ATOM_MaxDoubleValue = DBL_MAX;
static const double ATOM_MinDoubleValue = DBL_MIN;
static const float ATOM_FloatEpsilon = FLT_EPSILON;
static const double ATOM_DoubleEpsilon = DBL_EPSILON;

template <class T> struct ATOM_DataLimit;

template <> 
struct ATOM_DataLimit<signed char> 
{
  static inline signed char max_value () { return ATOM_MaxByteValue;}
  static inline signed char min_value () { return ATOM_MinByteValue;}
};

template <>
struct ATOM_DataLimit<unsigned char> {
  static inline unsigned char max_value () { return ATOM_MaxUByteValue;}
  static inline unsigned char min_value () { return ATOM_MinUByteValue;}
};

template <>
struct ATOM_DataLimit<short> {
  static inline short max_value () { return ATOM_MaxShortValue;}
  static inline short min_value () { return ATOM_MinShortValue;}
};

template <>
struct ATOM_DataLimit<unsigned short> {
  static inline unsigned short max_value () { return ATOM_MaxUShortValue;}
  static inline unsigned short min_value () { return ATOM_MinUShortValue;}
};

template <>
struct ATOM_DataLimit<int> {
  static inline int max_value () { return ATOM_MaxIntValue;}
  static inline int min_value () { return ATOM_MinIntValue;}
};

template <>
struct ATOM_DataLimit<unsigned int> {
  static inline unsigned int max_value () { return ATOM_MaxUIntValue;}
  static inline unsigned int min_value () { return ATOM_MinUIntValue;}
};

template <>
struct ATOM_DataLimit<long> {
  static inline long max_value () { return ATOM_MaxLongValue;}
  static inline long min_value () { return ATOM_MinLongValue;}
};

template <>
struct ATOM_DataLimit<unsigned long> {
  static inline unsigned long max_value () { return ATOM_MaxULongValue;}
  static inline unsigned long min_value () { return ATOM_MinULongValue;}
};

template <>
struct ATOM_DataLimit<float> {
  static inline float max_value () { return ATOM_MaxFloatValue;}
  static inline float min_value () { return ATOM_MinFloatValue;}
};

template <>
struct ATOM_DataLimit<double> {
  static inline double max_value () { return ATOM_MaxDoubleValue;}
  static inline double min_value () { return ATOM_MinDoubleValue;}
};

#endif // __ATOM_MATH_DATALIMIT_H
