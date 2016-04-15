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

#ifndef _WEBER_SUBSTEMFACTORY_H_
#define _WEBER_SUBSTEMFACTORY_H_

#include <opentree/weber/webertypes.h>
#include <opentree/utils/otarray.h>
#include <opentree/utils/otmatrix.h>

#include "stemfactory.h"

namespace opentree
{
  /// \addtogroup Weber_Algorithm
  /// @{

  class otWeberSubStemFactory 
  {
  private:
    otWeberStemRelations* stemrelations;
    otWeberStem* stem;
    TreeData& params;
    Level& level;

    float substemErrorValue;

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
    otWeberSubStemFactory(otWeberStem* stem, otWeberStemRelations* stemrelations)
    :  stemrelations(stemrelations), stem(stem), params(stem->params), level(stem->level)
    {
      substemErrorValue = 0;
    }
    
    ~otWeberSubStemFactory()
    {
    }

    unsigned int countSubstems(int index) 
    {
      float subst_per_segm = stem->substemsPerSegment;

      int substems_eff = (int)(subst_per_segm + substemErrorValue+0.5);
      substemErrorValue -= float(substems_eff - subst_per_segm);
    
      if (substems_eff <= 0) return 0;
      return substems_eff;
    }

    /// create substems for a segment
    void makeSubstems(int index, otMatrix seg_trf) 
    {
      if (level.levelNumber == 0)
      {
        if ((index+1)*stem->segmentLength <= params.baseSize*stem->length)
          return;
      }

      int substems_eff = (int)(stem->substemsPerSegment + substemErrorValue+0.5);
      substemErrorValue -= float(substems_eff - stem->substemsPerSegment);
    
      if (substems_eff <= 0) return;
    
      float deltaPos;
      float fractionalPos;
      if (level.levelNumber == 0)
      {
        deltaPos = (1.0f - params.baseSize) / (substems_eff + 1.0f);
        fractionalPos = params.baseSize;
      }
      else
      {
        deltaPos = (1.0f / (substems_eff + 1.0f));
        fractionalPos = 0;
      }

      for (int s=0; s<substems_eff; s++) 
      {
        otMatrix trf = seg_trf;

        fractionalPos += deltaPos;
        float offset = (index + fractionalPos) * stem->segmentLength;

        // Don't go over the parent's length.
        if (trf.getTranslation().Z + fractionalPos*stem->segmentLength <= stem->length)
          trf.thisZtranslate(fractionalPos*stem->segmentLength);

        otWeberHelper::rotXZDegree(trf, calcXrot(offset), calcZrot());
      
        // create new substem
        Level& level1 = otWeberHelper::nextLevel(params, level);
        otWeberStem substem = otWeberStem(stem,level1,trf,offset);
        otWeberStemRelations* relation = new otWeberStemRelations(stem->level.levelNumber+1, offset/stem->length);
        
        otWeberStemFactory stemfact(&substem, relation);
        if (stemfact.make()) 
          stemrelations->substems.add(relation);
        else
          delete relation;
      }
    }

  };

/// }@

}

#endif // _WEBER_SUBSTEMFACTORY_H_
