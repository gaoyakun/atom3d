#ifndef __ATOM_MATH_GLOBALS_H
#define __ATOM_MATH_GLOBALS_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <math.h>
#include <float.h>
#if ATOM3D_COMPILER_MSVC
#include <intrin.h>
#endif
#include "defs.h"

#if ATOM3D_COMPILER_MSVC
#	pragma intrinsic(_mm_cvtt_ss2si)
#	pragma intrinsic(_mm_load_ss)
#	pragma intrinsic(_mm_loadu_ps)
#	pragma intrinsic(_mm_cvttps_epi32)
#endif

#define ATOM_max2(a, b) ((a) > (b) ? (a) : (b))
#define ATOM_max3(a, b, c) ATOM_max2(ATOM_max2(a,b),c)
#define ATOM_min2(a, b) ((a) < (b) ? (a) : (b))
#define ATOM_min3(a, b, c) ATOM_min2(ATOM_min2(a,b),c)

static const float ATOM_Pi = 3.14159265359f;
static const float ATOM_SqrPi = ATOM_Pi * ATOM_Pi;
static const float ATOM_TwoPi = ATOM_Pi * 2.f;
static const float ATOM_InvTwoPi = 1.f / ATOM_TwoPi;
static const float ATOM_HalfPi = ATOM_Pi * 0.5f;
static const float ATOM_Epsilon = 0.00001f;

inline bool ATOM_equal (float v1, float v2)
{
  float f = v1 - v2;
  return f < ATOM_Epsilon && f > -ATOM_Epsilon;
}

inline bool ATOM_less (float v1, float v2)
{
  return v1 <= v2 - ATOM_Epsilon;
}

inline bool ATOM_greater (float v1, float v2)
{
  return v1 >= v2 + ATOM_Epsilon;
}

inline bool ATOM_lessequal (float v1, float v2)
{
  return v1 < v2 + ATOM_Epsilon;
}

inline bool ATOM_greaterequal (float v1, float v2)
{
  return v1 > v2 - ATOM_Epsilon;
}

inline float ATOM_exp (float f)
{
	return expf (f);
}

inline float ATOM_sqrt (float f)
{
  return sqrtf (f);
}

inline float ATOM_sqr (float f)
{
  return f * f;
}

inline float ATOM_invsqrt (float f)
{
  float xhalf = 0.5f * f;
  int i = (int&)f;
  i = 0x5f3759df - (i >> 1);
  f = (float&)i;
  return f * (1.5f - xhalf * f * f);
}

inline float ATOM_abs (float f)
{
  ((unsigned&)f) &= 0x7FFFFFFF;
  return f;
}

inline int ATOM_ftol (float f)
{
#if ATOM3D_COMPILER_MSVC
  // require SSE
  return _mm_cvtt_ss2si(_mm_load_ss(&f));
#else
  return int(f);
#endif
}

#if ATOM3D_COMPILER_MSVC
inline __m128i ATOM_ftol4 (float a, float b, float c, float d)
{
  // require SSE2
  return _mm_cvttps_epi32 (_mm_set_ps(d, c, b, a));
}

inline __m128i ATOM_ftol4 (const float *f)
{
	// require SSE2

	return _mm_cvttps_epi32 (_mm_loadu_ps(f));
}
#endif

inline void ATOM_ftol4 (float a, float b, float c, float d, int *i)
{
	// require SSE2
#if !ATOM_COMPILER_MSVC
	i[0] = ATOM_ftol (a);
	i[1] = ATOM_ftol (b);
	i[2] = ATOM_ftol (c);
	i[3] = ATOM_ftol (d);
#else
	__m128i r = _mm_cvttps_epi32 (_mm_set_ps(d, c, b, a));
	i[0] = r.m128i_i32[0];
	i[1] = r.m128i_i32[1];
	i[2] = r.m128i_i32[2];
	i[3] = r.m128i_i32[3];
#endif
}

inline float ATOM_fastsin_0_Pi (float f)
{
  const float B = 4.f / ATOM_Pi;
  const float C = -4.f / ATOM_SqrPi;
  float y = B * f + C * f * ATOM_abs(f);
  const float P = 0.225f;
  return  P * (y * ATOM_abs(y) - y) + y;
}

inline float ATOM_nfastsin_0_Pi_lp (float f)
{
  const float B = 4.f / ATOM_Pi;
  const float C = -4.f / ATOM_SqrPi;
  return B * f + C * f * ATOM_abs(f);
}

inline float ATOM_fastsin (float f)
{
  unsigned &i = (unsigned&)f;
  unsigned nb = i & 0x80000000;
  i &= 0x7FFFFFFF;

  float d = f * ATOM_InvTwoPi;
  f = ATOM_TwoPi * (d - ATOM_ftol (d));
  if (f > ATOM_Pi)
  {
	f -= ATOM_Pi;
	nb ^= 0x80000000;
  }
  float ret = ATOM_fastsin_0_Pi (f);
  ((unsigned&)ret) |= nb;
  return ret;
}

inline float ATOM_fastsin_lp (float f)
{
  unsigned &i = (unsigned&)f;
  unsigned nb = i & 0x80000000;
  i &= 0x7FFFFFFF;

  float d = f * ATOM_InvTwoPi;
  f = ATOM_TwoPi * (d - ATOM_ftol (d));
  if (f > ATOM_Pi)
  {
	f -= ATOM_Pi;
	nb ^= 0x80000000;
  }
  float ret = ATOM_nfastsin_0_Pi_lp (f);
  ((unsigned&)ret) |= nb;
  return ret;
}

inline float ATOM_fastcos (float f)
{
	return ATOM_fastsin (f + ATOM_HalfPi);
}

inline float ATOM_fastcos_lp (float f)
{
	return ATOM_fastsin_lp (f + ATOM_HalfPi);
}

inline float ATOM_sin_0_halfpi (float f)
{
	float s, t;

	s = f * f;
	t = -2.39e-08f;
	t *= s;
	t += 2.7526e-06f;
	t *= s;
	t += -1.98409e-04f;
	t *= s;
	t += 8.3333315e-03f;
	t *= s;
	t += -1.666666664e-01f;
	t *= s;
	t += 1.f;
	t *= f;
	return t;
}

inline float ATOM_sin (float f)
{
  return sinf (f);
}

inline float ATOM_cos (float f)
{
  return cosf (f);
}

inline float ATOM_tan (float f)
{
  return tanf (f);
}

inline float ATOM_log2 (float f)
{
  static const float LOG2 = logf(2.f);
  return logf (f) / LOG2;
}

inline float ATOM_log10 (float f)
{
  return log10f (f);
}

inline float ATOM_log (float f)
{
  return logf (f);
}

inline void ATOM_sincos (float f, float *sinValue, float *cosValue)
{
#if ATOM3D_COMPILER_MSVC && !defined(_M_IA64) && !defined(_M_AMD64)
  __asm
  {
    mov eax, sinValue
    mov edx, cosValue
    fld f
    fsincos
    fstp dword ptr [edx]
    fstp dword ptr [eax]
  }
#else
  *sinValue = ATOM_sin(f);
  *cosValue = ATOM_cos(f);
#endif
}

inline float ATOM_atan_positive (float y, float x)
{
	float a, d, s, t;
	if (y > x)
	{
		a = -x / y;
		d = ATOM_HalfPi;
	}
	else
	{
		a = y / x;
		d = 0.f;
	}
	s = a * a;
	t = s * 0.0028662257f;
	t += -0.0161657367f;
	t *= s;
	t += 0.0429096138f;
	t *= s;
	t += -0.0752896400f;
	t *= s;
	t += 0.1065626393f;
	t *= s;
	t += -0.1420889944f;
	t *= s;
	t += 0.1999355085f;
	t *= s;
	t += -0.3333314528f;
	t *= s;
	t += 1.f;
	t *= a;
	t += d;
	return t;
}

inline float ATOM_atan (float f)
{
  return atanf (f);
}

inline float ATOM_atan2 (float y, float x)
{
  return atan2f (y, x);
}

inline float ATOM_asin (float f)
{
  return asinf (f);
}

inline float ATOM_acos (float f)
{
  return acosf (f);
}

inline float ATOM_pow (float a, float b)
{
  return powf (a, b);
}

inline float ATOM_floor (float f)
{
  return floorf (f);
}

inline float ATOM_ceil (float f)
{
  return ceilf (f);
}

inline float ATOM_mod (float x, float y)
{
  return x - ATOM_floor (x / y) * y;
}

inline bool ATOM_nan (float f)
{
  return _isnan (f) != 0;
}

inline bool ATOM_ispo2 (unsigned n)
{
  return (n & (n-1)) == 0;
}

inline unsigned ATOM_nextpo2 (unsigned n)
{
	--n;
	n |= n >> 1;
	n |= n >> 2;
	n |= n >> 4;
	n |= n >> 8;
	n |= n >> 16;
	return ++n;
}

inline float ATOM_saturate (float f)
{
	if (f < 0.f)
	{
		f = 0.f;
	}
	else if (f > 1.f)
	{
		f = 1.f;
	}
	return f;
}

inline float ATOM_randomf (float f1, float f2)
{
	return f1 + (f2 - f1) * (float(rand()) / float(RAND_MAX));
}

template <class T>
inline T ATOM_randomT (const T &val1, const T &val2)
{
	return val1 + (val2 - val1) * (float(rand()) / float(RAND_MAX));
}

inline short ATOM_float_to_half (float f)
{
	int i = (int&)f;
    //
    // Our floating point number, f, is represented by the bit
    // pattern in integer i.  Disassemble that bit pattern into
    // the sign, s, the exponent, e, and the significand, m.
    // Shift s into the position where it will go in in the
    // resulting half number.
    // Adjust e, accounting for the different exponent bias
    // of float and half (127 versus 15).
    //

    register int s =  (i >> 16) & 0x00008000;
    register int e = ((i >> 23) & 0x000000ff) - (127 - 15);
    register int m =   i        & 0x007fffff;

    //
    // Now reassemble s, e and m into a half:
    //

    if (e <= 0)
    {
	if (e < -10)
	{
	    //
	    // E is less than -10.  The absolute value of f is
	    // less than HALF_MIN (f may be a small normalized
	    // float, a denormalized float or a zero).
	    //
	    // We convert f to a half zero with the same sign as f.
	    //

	    return s;
	}

	//
	// E is between -10 and 0.  F is a normalized float
	// whose magnitude is less than HALF_NRM_MIN.
	//
	// We convert f to a denormalized half.
	//

	//
	// Add an explicit leading 1 to the significand.
	// 

	m = m | 0x00800000;

	//
	// Round to m to the nearest (10+e)-bit value (with e between
	// -10 and 0); in case of a tie, round to the nearest even value.
	//
	// Rounding may cause the significand to overflow and make
	// our number normalized.  Because of the way a half's bits
	// are laid out, we don't have to treat this case separately;
	// the code below will handle it correctly.
	// 

	int t = 14 - e;
	int a = (1 << (t - 1)) - 1;
	int b = (m >> t) & 1;

	m = (m + a + b) >> t;

	//
	// Assemble the half from s, e (zero) and m.
	//

	return s | m;
    }
    else if (e == 0xff - (127 - 15))
    {
	if (m == 0)
	{
	    //
	    // F is an infinity; convert f to a half
	    // infinity with the same sign as f.
	    //

	    return s | 0x7c00;
	}
	else
	{
	    //
	    // F is a NAN; we produce a half NAN that preserves
	    // the sign bit and the 10 leftmost bits of the
	    // significand of f, with one exception: If the 10
	    // leftmost bits are all zero, the NAN would turn 
	    // into an infinity, so we have to set at least one
	    // bit in the significand.
	    //

	    m >>= 13;
	    return s | 0x7c00 | m | (m == 0);
	}
    }
    else
    {
	//
	// E is greater than zero.  F is a normalized float.
	// We try to convert f to a normalized half.
	//

	//
	// Round to m to the nearest 10-bit value.  In case of
	// a tie, round to the nearest even value.
	//

	m = m + 0x00000fff + ((m >> 13) & 1);

	if (m & 0x00800000)
	{
	    m =  0;		// overflow in significand,
	    e += 1;		// adjust exponent
	}

	//
	// Handle exponent overflow
	//

	if (e > 30)
	{
	    return s | 0x7c00;	// if this returns, the half becomes an
	}   			// infinity with the same sign as f.

	//
	// Assemble the half from s, e and m.
	//

	return s | (e << 10) | (m >> 13);
    }
}


#endif // __ATOM_MATH_GLOBALS_H
