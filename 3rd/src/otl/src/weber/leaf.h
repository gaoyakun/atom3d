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

#ifndef _WEBER_LEAF_H_
#define _WEBER_LEAF_H_

#include <opentree/weber/webertypes.h>
#include <opentree/utils/otmatrix.h>

#include <opentree/ottree.h>

#include "stem.h"
#include "helper.h"

namespace opentree
{

  /// \addtogroup Weber_Algorithm
  /// @{

  /**
  * A class for the leaves of the tree
  */
  class otWeberLeaf 
  {
  private:
    TreeData* params;

    /** Section 4.9 Leaf Orientation
     * "Left alone, the modeled leaves will generally assume seemingly random
     * orientations. However, in reality, leaves are oriented to face upwards and
     * outwards, presumably to optimize the available direct (sun) and scattered
     * (sky) light. We can use the declination and orientation of each leaf to rotate
     * them toward the light. The necessary rotations are computed based on the
     * current viewing transformation and are applied prior to creating the leaf into
     * the geometric description." - Quote "Creation and Rendering of Realistic Trees"
     */
    void calcLeafOrientation()
    {
      otVector3 pos = trf.getTranslation();
      otVector3 normal = trf.getY();
      
      float theta_pos = atan2(pos.Y,pos.X);
      float theta_bend = theta_pos - atan2(normal.Y,normal.X);

      //global rotation
      trf.grotZRad(params->leafBend * theta_bend);

      normal = trf.getY();

      float phi_bend = atan2f(sqrtf(normal.X*normal.X + normal.Y*normal.Y), normal.Z);

      //local rotation
      trf.lrotXRad(phi_bend * params->leafBend);
    }

    
  public:	
    otMatrix trf;
    float length, width;

    otWeberLeaf() : params(0) {}

    otWeberLeaf(TreeData& params, otMatrix& trf) 
    : params(&params), trf(trf) 
    {
    }
    
    void make()
    {
      length = params->leafScale/sqrt(params->leafQuality);
      width = length * params->leafScaleX;

      if (params->leafBend != 0) 
        calcLeafOrientation();
    }

  };

  /// }@

}

#endif // _WEBER_LEAF_H_
