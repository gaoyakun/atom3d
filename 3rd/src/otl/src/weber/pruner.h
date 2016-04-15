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

#ifndef _WEBER_PRUNER_H_
#define _WEBER_PRUNER_H_

#include <opentree/utils/otvector3.h>

#include "stem.h"
#include "stemhelper.h"

namespace opentree
{

  /// \addtogroup Weber_Algorithm
  /// @{

  /** Pruning of a tree, cuts it into shape.
   * \todo Could this be less recursive?
   */
  class otWeberPruner
  {
    otWeberStem* stem;
    TreeData& params;
    Level& level;

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

    bool isInEnvelop(const otVector3& vector)
    {
      float r = sqrt(vector.X*vector.X + vector.Y*vector.Y);
      float ratio = (params.scale_tree - vector.Z)/(params.scale_tree*(1-params.baseSize));
      return ((r/params.scale_tree) < (params.pruneWidth * otWeberStemHelper::getShapeRatio(ratio,8, params)));
    }

  public:
    otWeberPruner(otWeberStem* stem)
    : stem(stem), params(stem->params), level(stem->level)
    {
    }

    ~otWeberPruner() {}

    void pruning() 
    {
      float splitcorr = stem->splitCorrection;
      float origlen = stem->length;

      //Find first segment that's outside the envelop
      int segm = makeSegments(0,level.curveRes);
    
      while (segm >= 0 && stem->length > 0.001*params.scale_tree) 
      {
        //Restore the splitcorrection
        stem->splitCorrection = splitcorr;

        //Length of the stem to the envelop border must be end of stem.
        stem->length -= stem->segmentLength*(level.curveRes-segm+1);

        //Create new segment length
        stem->segmentLength = stem->length/level.curveRes;

        float max_radius = otWeberStemHelper::stemRadius(stem->parent, stem->offset);
        float radius = stem->parent->baseRadius * powf(stem->length/stem->parent->length,params.ratioPower);
        stem->baseRadius = min(radius,max_radius);
      
        if (stem->length > otWeberStemHelper::getMinStemLen()) 
          segm = makeSegments(0,level.curveRes);
      }

      //Apply pruneRatio on corrections.
      stem->length = origlen - (origlen-stem->length)*params.pruneRatio;
      stem->splitCorrection = splitcorr;
    }

    int makeSegments(int start_seg,int end_seg) 
    {
      otMatrix trf = stem->trf;

      for (int s=start_seg; s<=end_seg; s++) 
      {
        float xrot = calcXStemRot(s);
        if (xrot) trf.lrotXDegree(xrot);

        if (level.curveV > 0 && s != 0) 
        {
          otVector3 rot(1,0,0);
          rot.rotZRad(PI+calcVariance(PI));
          trf.lrotateDegree(calcVariance(level.curveV)/level.curveRes, rot);
        }  
      
        otVector3 z = trf.getZ();
        otWeberHelper::rotVertical(trf, z, -calcVerticalAttraction(trf, s));

        trf.thisZtranslate(stem->segmentLength);

        const otVector3 v = trf.getTranslation();
        if(!isInEnvelop(v))
        {
          return s;
        }
        
        if (s<end_seg-1) 
        {
          int ret = makeClones(trf, s);
          if (ret >= 0) return ret;
        }
      }
      return -1;
    }

    int makeClones(otMatrix& trf, int s)
    {
      int seg_splits_eff;
      if (stem->level.levelNumber==0 && s==0 && params.trunk.baseSplits>0) 
      {
        seg_splits_eff = (int)params.trunk.baseSplits;
      }
      else
      {
        seg_splits_eff = (int)(level.segSplits + stem->splitErrorValue + 0.5);
        stem->splitErrorValue -= (float)(seg_splits_eff - level.segSplits);
      }
      
      if (seg_splits_eff>=1)
      {
        float s_angle_delta = 360.0f/(seg_splits_eff+1);
        float s_angle = 0;

        for (int i=0; i<seg_splits_eff; i++) 
        { 
          otWeberStem clone = otWeberStemHelper::clone(stem, trf, true);

          s_angle += s_angle_delta;
          otWeberStemHelper::split(&clone, clone.trf,s_angle,s,true);

          otWeberPruner pruner(&clone);
          int nseg = pruner.makeSegments(s+1,clone.level.curveRes);

          if (nseg>=0) return nseg;
        }

        otWeberStemHelper::split(stem,trf,0,s,true);

      }
      return -1;
    }
  };

  /// }@

}

#endif // _WEBER_PRUNER_H_
