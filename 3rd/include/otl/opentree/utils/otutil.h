/*
  Copyright (c) 2004 Pascal Kirchdorfer, Seth Yastrov

  This software is provided 'as-is', without any express or implied warranty. In
  no event will the authors be held liable for any damages arising from the use
  of this software.

  Permission is granted to anyone to use this software for any purpose, including
  commercial applications, and to alter it and redistribute it freely, subject to
  the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim
       that you wrote the original software. If you use this software in a product,
       an acknowledgment in the product documentation would be appreciated but is
       not required.

    2. Altered source versions must be plainly marked as such, and must not be
       misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.
*/

#ifndef _OT_UTIL_H_
#define _OT_UTIL_H_

// All external includes go here, so we have on place were we can override then if needed.
#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <float.h>

namespace opentree
{
  /** \addtogroup Utilities
   *  Collection of utilities needed by opentree.
   */

  /// @{

  #ifndef PI
  #define PI 3.14159265f
  #endif
  #ifndef RAD2D
  #define RAD2D 57.29578f
  #endif
  #ifndef D2RAD
  #define D2RAD 0.017453293f
  #endif

  /** Set Seed for rand() funtion.
   * This is generally often used togetter with time() in order to get
   * a more random behaviour of the app. In our case though we want predictable
   * random values. As long as the random function is seeded with the #same value,
   * the trees should look the same.
   * \param seed Initialization value for the random function.
   */ 
  inline static void setRandomSeed(unsigned int seed)
  {
    srand(seed);
  }

  /** Returns a random number.
   * TODO: make it less random, do we get the same 
   * random number on all platform with the same #seed?
   * \return random value between 0 and 1.
   */
  inline static float calcRandom()
  {
    return (float)rand() / (RAND_MAX + 1.0f);
    //return 0.5f;
  }

  /**
   * Returns a random number.
   * \param variance scope of the random value
   * \param usesigned usage of signed or unsigned values
   * \return Random value between 0 and #variance or -#variance and #variance.
   */
  inline static float calcVariance(float variance, bool usesigned = true)
  {
    variance = fabs(variance);
      
    if (usesigned)
    {
      //random between -1 and 1 multiplied with the variance
      return (2 * calcRandom() - 1) * variance;
    }
    else
    {
      //random between 0 and 1 multiplied with the variance
      return calcRandom() * variance;
    }
  }

  template<class Typ> inline
  const Typ& (min)(const Typ& left, const Typ& right)
  {
    return ((right < left)? right : left);
  }

  template<class Typ> inline
  const Typ& (max)(const Typ& left, const Typ& right)
  {
    return ((right > left)? right : left);
  }

#ifdef _DEBUG
  ///assers if condition X is true and prints Y
  #define otCondAssert(X,Y) if (X) { std::cerr << Y; int _ot_assert_ = 0; _ot_assert_ /= _ot_assert_; }
  ///same as otCondAssert(true, Y). Assers whenever called
  #define otAssert(Y) std::cerr << Y; int _ot_assert_ = 0; _ot_assert_ /= _ot_assert_; 
#else
  ///assers if condition X is true and prints Y
  #define otCondAssert(X,Y) 
  ///same as otCondAssert(true, Y). Assers whenever called
  #define otAssert(Y) 
#endif

  /// @}
}

#endif //_OT_UTIL_H_

