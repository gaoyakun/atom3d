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

#ifndef _WEBER_TRUNKFACTORY_H_
#define _WEBER_TRUNKFACTORY_H_

#include <opentree/weber/webertypes.h>
#include <opentree/utils/otarray.h>
#include <opentree/utils/otmatrix.h>

#include "helper.h"

namespace opentree
{
  /// \addtogroup Weber_Algorithm
  /// @{

  class otWeberTrunkFactory 
  {
  private:
    otWeberTree* tree;

    Level& level;

    float rotation;

    otMatrix trf;

    /// Calculate down angle
    inline float calcDownAngle()
    {
      return level.downAngle+calcVariance(level.downAngleV);
    }

    /// Calculate rotation angle
    inline float calcRotAngle()
    {
      if (level.rotate>=0) // rotation
      {
        float rotate = level.rotate + calcVariance(level.rotateV);
        rotation = fmodf(rotation + rotate, 360);
        return rotation;
      }
      else // alternation
      {
        float rotate = 180 + level.rotate + calcVariance(level.rotateV);
        rotation = fmodf(rotation + rotate, 360);
        return rotation;
      }
    }


  public:
    otWeberTrunkFactory(otWeberTree* tree, Level& level)
    : tree(tree), level(level), rotation(0)
    {
    }
    
    ~otWeberTrunkFactory()
    {
    }

    otWeberStem create()
    {
      otWeberHelper::rotXZDegree(trf, calcDownAngle(), calcRotAngle());

      float angle = calcVariance(360);
      float dist = calcVariance(tree->params.trunk.dist);
      trf.translate(otVector3(dist*sin(angle), dist*cos(angle),0));

      return otWeberStem(tree,tree->params,0,trf,0);
    }

  };

/// }@

}

#endif // _WEBER_TRUNKFACTORY_H_
