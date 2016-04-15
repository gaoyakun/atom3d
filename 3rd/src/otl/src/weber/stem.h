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

#ifndef _WEBER_STEM_H_
#define _WEBER_STEM_H_

#include <opentree/weber/webertypes.h>

#include <opentree/utils/otarray.h>
#include <opentree/utils/otmatrix.h>

#include <opentree/ottree.h>
#include <opentree/otstem.h>

#include "segment.h"
#include "stemrelations.h"

namespace opentree
{
  class otWeberTree;
  class otWeberClone;
  class otWeberLeafFactory;

  /// \addtogroup Weber_Algorithm
  /// @{

  class otWeberStem 
  {
  public:
    TreeData& params;
    Level& level;

    otWeberTree* tree;
    otWeberStem* parent;

    otMatrix trf;

    float offset;

    float length;
    float baseRadius;

    float lengthChildMax;
    float substemsPerSegment;
    float substemRotangle;

    float splitCorrection;
    float splitErrorValue;

    float segmentLength;

    otWeberStem(otWeberTree* tree, TreeData& params, int levno, otMatrix trf, float offs)
    : params(params), level(params.level[levno]), tree(tree), trf(trf), offset(offs)
    {
      splitErrorValue = 0;
      splitCorrection = 0;

      parent = 0;
    }

    otWeberStem(otWeberStem* parentStem, Level& level, otMatrix trf, float offs)
    : params(parentStem->params), level(level), trf(trf), offset(offs)
    {
      tree = parentStem->tree;

      splitErrorValue = 0;
      splitCorrection = 0;

      parent = parentStem;
    }

    ~otWeberStem() {}

  };

  /// }@

}

#endif //_WEBERARBARO_STEM_H_
