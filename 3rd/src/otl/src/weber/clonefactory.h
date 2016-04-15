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

#ifndef _WEBER_CLONEFACTORY_H_
#define _WEBER_CLONEFACTORY_H_

#include <opentree/weber/webertypes.h>
#include <opentree/utils/otarray.h>
#include <opentree/utils/otmatrix.h>

#include "stem.h"
#include "stemhelper.h"

namespace opentree
{
  /// \addtogroup Weber_Algorithm
  /// @{

  class otWeberCloneFactory 
  {
  private:
    otWeberStemRelations* stemrelations;
    otWeberStem* stem;
    TreeData& params;
    Level& level;

    void makeSegments(otWeberStem* clone, otWeberStemRelations* relation, int start_seg);

  public:
    otWeberCloneFactory(otWeberStem* stem, otWeberStemRelations* stemrelations)
    : stemrelations(stemrelations), stem(stem), params(stem->params), level(stem->level)
    {
    }
    
    ~otWeberCloneFactory()
    {
    }

    unsigned int countClones(int nseg) 
    {
      int seg_splits_eff;
      if (stem->level.levelNumber==0 && nseg==0 && params.trunk.baseSplits>0) 
      {
        seg_splits_eff = (int)params.trunk.baseSplits;
      }
      else
      {
        seg_splits_eff = (int)(level.segSplits + stem->splitErrorValue + 0.5);
        stem->splitErrorValue -= (float)(seg_splits_eff - level.segSplits);
      }
      
      if (seg_splits_eff<1)
        return 0;
      
      return seg_splits_eff;
    }

    void makeClones(otMatrix& trf,int nseg) 
    {
      int seg_splits_eff;
      if (stem->level.levelNumber==0 && nseg==0 && params.trunk.baseSplits>0) 
      {
        seg_splits_eff = (int)params.trunk.baseSplits;
      }
      else
      {
        seg_splits_eff = (int)(level.segSplits + stem->splitErrorValue + 0.5);
        stem->splitErrorValue -= (float)(seg_splits_eff - level.segSplits);
      }
      
      if (seg_splits_eff<1) return;
      
      float s_angle_delta = 360.0f/(seg_splits_eff+1);
      float s_angle = 0;
      
      // creating clones
      for (int i=0; i<seg_splits_eff; i++) 
      { 
        otWeberStem clone = otWeberStemHelper::clone(stem, trf);

        s_angle += s_angle_delta;
        otWeberStemHelper::split(&clone, clone.trf,s_angle,nseg);
        clone.substemsPerSegment /= (seg_splits_eff+1);

        otWeberStemRelations* relation = new otWeberStemRelations(clone.level.levelNumber, clone.offset / stem->length);
        stemrelations->clones.add(relation);
        
        makeSegments(&clone, relation, nseg+1);
      }

      otWeberStemHelper::split(stem, trf,0,nseg);
      stem->substemsPerSegment /= (seg_splits_eff+1);

      return;
    }

  };

/// }@

}

#endif // _WEBER_CLONEFACTORY_H_
