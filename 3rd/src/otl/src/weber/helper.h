/*
  Copyright (c) 2004 Pascal Kirchdorfer, Seth Yastrov

  Based on J. Weber's and J. Penn's Algorithm described in 
  "Creation and Rendering of Realistic Trees"

  The implementation of the algorithm is based on the following two projects:
  Arbaro: http://arbaro.sourceforge.net (c) Wolfram Diestel
  TReal: http://members.chello.nl/~l.vandenheuvel2/TReal/ (c) Ton van den Heuvel

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

#ifndef _WEBER_HELPER_H_
#define _WEBER_HELPER_H_

#include <opentree/weber/webertypes.h>

#include <opentree/utils/otarray.h>
#include <opentree/utils/otmatrix.h>

#include <opentree/ottree.h>
#include <opentree/otstem.h>

namespace opentree
{
  /// \addtogroup Weber_Algorithm
  /// @{

  class otWeberHelper 
  {
  public:

    /// Get the childlevel of the current level.
    static inline Level& nextLevel(TreeData& params, Level& level)
    {
      return params.level[min(level.levelNumber+1,3)];
    }

    /** Rotation about the X and Z axis.
     * Equivalent but way faster than: mtx = mtx * ( xrot * zrot );
     */
    static inline void rotXZDegree(otMatrix& mtx, float x, float z)
    {
      rotXZRad(mtx, x*D2RAD, z*D2RAD);
    }

    /** Rotation about the X and Z axis.
     * Equivalent but way faster than: mtx = mtx * ( xrot * zrot );
     */
    static inline void rotXZRad(otMatrix& mtx, float x, float z)
    {
      float* M = (float*) mtx;

      float sx=sin(x); float cx=cos(x);
      float sz=sin(z); float cz=cos(z);

      float N[8];

      N[0] =  cz;
      N[1] =  sz;

      N[2] =  sz * sx;
      N[3] = -sz * cx;

      N[4] = -cz * sx;
      N[5] =  cz * cx;

      N[6] = sx;
      N[7] = cx;

      float tmp;
      tmp  = M[0]*N[3]  + M[4]*N[5]  + M[8] *N[6];
      M[8] = M[0]*N[2]  + M[4]*N[4]  + M[8] *N[7];
      M[0] = M[0]*N[0]  + M[4]*N[1];
      M[4] = tmp;

      tmp  = M[1]*N[3]  + M[5]*N[5]  + M[9] *N[6];
      M[9] = M[1]*N[2]  + M[5]*N[4]  + M[9] *N[7];
      M[1] = M[1]*N[0]  + M[5]*N[1];
      M[5] = tmp;

      tmp  = M[2]*N[3]  + M[6]*N[5]  + M[10]*N[6];
      M[10]= M[2]*N[2]  + M[6]*N[4]  + M[10]*N[7];
      M[2] = M[2]*N[0]  + M[6]*N[1];
      M[6] = tmp;
    }

    static inline void rotVertical(otMatrix& mtx, otVector3 axis, float angle)
    {
      axis.normalize();
      float x = -axis.Y;
      float y = axis.X;
      float s = sin(angle);
      float c = cos(angle);

      otMatrix m;

      m[0] =  x*x*(1-c) + c;
      m[1] =  x*y*(1-c);
      m[2] = -y*s;
                      
      m[4] =  x*y*(1-c);
      m[5] =  y*y*(1-c) + c;
      m[6] =  x*s;
                      
      m[8] =  y*s;
      m[9] = -x*s;
      m[10] =  c ;

      m.rot(mtx);
      m.translate(mtx[12],mtx[13],mtx[14]);
      mtx = m;
    }
  };

  /// }@

}

#endif //_WEBER_HELPER_H_
