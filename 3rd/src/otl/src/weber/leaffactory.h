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

#ifndef _WEBER_LEAFFACTORY_H_
#define _WEBER_LEAFFACTORY_H_

#include <opentree/utils/otvector3.h>

#include "tree.h"

namespace opentree
{

  /// \addtogroup Weber_Algorithm
  /// @{

  class otWeberLeafFactory
  {
    otWeberStemRelations* stemrelations;
    otWeberStem* stem;
    TreeData& params;
    Level& level;

    float leavesPerBranch;
    float leavesPerSegment;
    float leavesErrorValue;

    /// Calculate rotation angle
    inline float calcZrot()
    {
      Level& level1 = otWeberHelper::nextLevel(params, level);
      float& rotation = stem->substemRotangle;

      if (level1.rotate>=0) // rotation
      {
        float rotate = level1.rotate + calcVariance(level1.rotateV);
        rotation = fmodf(rotation + rotate, 360);
        return rotation;
      }
      else // alternation
      {
        float rotate = 180 + level1.rotate + calcVariance(level1.rotateV);
        rotation = fmodf(rotation + rotate, 360);
        return rotation;
      }
    }

    /// Calculate down angle
    inline float calcXrot(float offset)
    {
      Level& level1 = params.level[min(stem->level.levelNumber+1,3)];

      if (level1.downAngleV>=0) 
      {
        return level1.downAngle+calcVariance(level1.downAngleV);
      } 
      else 
      {
        float base_len = params.baseSize*params.scale_tree;
        float ratio = (stem->length-offset)/(stem->length - base_len);
        float shaperatio = 1 - 2 * otWeberStemHelper::getShapeRatio(ratio,0, params);
        return level1.downAngle + (level1.downAngleV * shaperatio);
      }  
    }

  public:
    otWeberLeafFactory(otWeberStem* stem, otWeberStemRelations* stemrelations)
    : stemrelations(stemrelations), stem(stem), params(stem->params), level(stem->level)
    {
      leavesErrorValue = 0;
      leavesPerBranch = 0;
      leavesPerSegment = 0;
      if (level.levelNumber == params.levels-1 && stem->parent) 
      {
        float ratio = stem->offset/stem->parent->length;
        ratio = otWeberStemHelper::getShapeRatio(ratio,params.leafShapeRatio, params);
        leavesPerBranch = abs(params.leaves) * params.leafQuality * ratio;
        leavesPerSegment = leavesPerBranch / level.curveRes;
      }
    }

    ~otWeberLeafFactory() {}

    unsigned int countLeaves(int index) 
    {
      if (params.leaves > 0)
      {
        float leaves_eff = (leavesPerSegment + leavesErrorValue+0.5f);
        leavesErrorValue -= float(leaves_eff - leavesPerSegment);
        if (leaves_eff <= 0) 
          return 0;

        return (unsigned int)ceilf(leaves_eff);
      }
      else if (params.leaves<0 && index == level.curveRes-1)
      {
        return (unsigned int)(leavesPerBranch+0.5f);
      }
      return 0;
    }

    void makeLeaves(int index, otMatrix& seg_trf) 
    {
      if (params.leaves > 0)
      {
        makeNormalLeaves(index, seg_trf);
      }
      else if (params.leaves < 0)
      {
        // Create fan only at the last segment
        if (index == level.curveRes-1)
          makeFanLeaves(index, seg_trf);
      }
    }

    /// Create Leaves normally
    void makeNormalLeaves(int index, otMatrix& seg_trf) 
    {
      float leaves_eff = (leavesPerSegment + leavesErrorValue+0.5f);
      leavesErrorValue -= float(leaves_eff - leavesPerSegment);
    
      if (leaves_eff <= 0) 
        return;
    
      float deltaPos;
      float fractionalPos;
      if (level.levelNumber == 0)
      {
        deltaPos = (1.0f - params.baseSize) / (leaves_eff + 1.0f);
        fractionalPos = params.baseSize;
      }
      else
      {
        deltaPos = (1.0f / (leaves_eff + 1.0f));
        fractionalPos = 0;
      }

      stem->tree->leaves += (unsigned int)ceilf(leaves_eff);
      for (int s=0; s<leaves_eff; s++) 
      {
	otMatrix trf = seg_trf;

        fractionalPos += deltaPos;
        float offset = (index + fractionalPos) * stem->segmentLength;

	trf.thisZtranslate(fractionalPos*stem->segmentLength);
        otWeberHelper::rotXZDegree(trf, calcXrot(offset), calcZrot());

	// create new leaf
	otWeberLeaf leaf = otWeberLeaf(params,trf);
	leaf.make();
	stemrelations->leaves.add(leaf);
      }
    }

    /// Create Leaves in a fan at the end of the stem (for palms and such)
    void makeFanLeaves(int index, otMatrix& seg_trf) 
    {
      Level& level1 = otWeberHelper::nextLevel(params, level);
      int cnt = (int)(leavesPerBranch+0.5f);

      otMatrix trf = seg_trf;

      // go to the end of the last segment (end of stem).
      trf.thisZtranslate(stem->segmentLength);

      stem->tree->leaves += cnt;

      float rotate = level1.rotate/cnt;
      for (int s=0; s<cnt; s++) 
      {
	otMatrix trf1 = trf;
        trf1.lrotYDegree(s*rotate + calcVariance(level1.rotateV));
	trf1.lrotXDegree(level1.downAngle + calcVariance(level1.downAngleV));

        otWeberLeaf leaf = otWeberLeaf(params,trf1);
	stemrelations->leaves.add(leaf);
	leaf.make();
      }
    }
  };

  /// }@

}

#endif // _WEBER_LEAFFACTORY_H_
