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

#ifndef _WEBER_SEGMENTFACTORY_H_
#define _WEBER_SEGMENTFACTORY_H_

#include <opentree/utils/otvector3.h>

#include "leaffactory.h"
#include "substemfactory.h"
#include "clonefactory.h"

namespace opentree
{

  /// \addtogroup Weber_Algorithm
  /// @{

  class otWeberSegmentFactory
  {
    otWeberStemRelations* stemrelations;
    otWeberStem* stem;
    TreeData& params;
    Level& level;

    /// Calculate down angle
    float calcXStemRot(int s)
    {
      if (s == 0) return 0;

      float delta;

      if (level.curveBack==0) 
      {
        delta = level.curve / level.curveRes;
      } 
      else 
      {
        if (s < (level.curveRes+1)/2) 
        {
	  delta = level.curve*2 / level.curveRes;
        } 
        else 
        {
	  delta = level.curveBack*2 / level.curveRes;
        }
      }
      return delta + stem->splitCorrection;
    }

    float calcVerticalAttraction(otMatrix& trf, int s)
    {
      if (params.attractionUp == 0 || stem->level.levelNumber<2 || s == 0)
        return 0;

      // Weber/Penn do here: cosf(acosf(trf.getY().Z)) which can be simplified:
      return params.attractionUp * acosf(trf.getZ().Z) * trf.getY().Z / level.curveRes;
    }

    void prepareRelations(int start_seg,int end_seg)
    {
      float spliterr = stem->splitErrorValue;

      otWeberSubStemFactory substemfact(stem, stemrelations);
      otWeberLeafFactory leaffact(stem, stemrelations);
      otWeberCloneFactory clonefact(stem, stemrelations);

      unsigned int substems = 0;
      unsigned int leaves = 0;
      unsigned int clones = 0;

      for (int s=start_seg; s<=end_seg; s++) 
      {
        if (level.levelNumber<params.levels-1)
        {
          substems += substemfact.countSubstems(s);
        }
        if (level.levelNumber==params.levels-1 && params.leaves!=0)
        {
          leaves += leaffact.countLeaves(s);
        }
        if (s<end_seg-1)
        {
          clones += clonefact.countClones(s);
        }

      }
      stemrelations->substems.setGrowRate(substems);
      stemrelations->leaves.setGrowRate(leaves);
      stemrelations->clones.setGrowRate(clones);

      stem->splitErrorValue = spliterr;
    }


  public:
    otWeberSegmentFactory(otWeberStem* stem, otWeberStemRelations* stemrelations)
    : stemrelations(stemrelations), stem(stem), params(stem->params), level(stem->level)
    {
    }

    ~otWeberSegmentFactory() {}

    void makeSegments(int start_seg,int end_seg) 
    {
      otMatrix trf = stem->trf;

      otWeberLeafFactory leaffact(stem, stemrelations);
      otWeberSubStemFactory substemfact(stem, stemrelations);
      otWeberCloneFactory clonefact(stem, stemrelations);

      stemrelations->segments.setGrowRate(end_seg-start_seg+1);
      
      prepareRelations(start_seg, end_seg);

      for (int s=start_seg; s<=end_seg; s++) 
      {
        trf.lrotXDegree(calcXStemRot(s));

        // add random rotation about z-axis
        if (level.curveV>0 && s!=0) 
        {
          otVector3 rot(1,0,0);
          rot.rotZRad(PI+calcVariance(PI));
          trf.lrotateDegree(calcVariance(level.curveV)/level.curveRes, rot);
        }  
        
        otVector3 z = trf.getZ();
        otWeberHelper::rotVertical(trf, z, -calcVerticalAttraction(trf, s));

        // segment radius
        float rad1 = otWeberStemHelper::stemRadius(stem, s*stem->segmentLength);

        otWeberSegment* segment = new otWeberSegment(trf,rad1);
        stemrelations->segments.add(segment);

        if (level.levelNumber<params.levels-1)
        {
          substemfact.makeSubstems(s, trf);
        }

        if (level.levelNumber==params.levels-1 && params.leaves!=0)
        {
          leaffact.makeLeaves(s, trf);
        }

        trf.thisZtranslate(stem->segmentLength);

        if (s<end_seg-1)
        {
          clonefact.makeClones(trf,s);
        }
      }
      
      return;
    }
  };

  /// }@

}

#endif // _WEBER_SEGMENTFACTORY_H_
