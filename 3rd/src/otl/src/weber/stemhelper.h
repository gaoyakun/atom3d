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

#ifndef _WEBER_STEMHELPER_H_
#define _WEBER_STEMHELPER_H_

#include <opentree/weber/webertypes.h>

#include <opentree/utils/otarray.h>
#include <opentree/utils/otmatrix.h>

#include <opentree/ottree.h>
#include <opentree/otstem.h>

#include "stem.h"
#include "helper.h"

namespace opentree
{
  /// \addtogroup Weber_Algorithm
  /// @{

  class otWeberStemHelper 
  {
  public:

    inline static const float getMinStemLen()
    {
      return 1.0f/2000.0f;
    }

    inline static const float getMinStemRadius()
    {
      return getMinStemLen()/10;
    }

    inline static otWeberStem clone(otWeberStem* parent, otMatrix& trf, bool prune=false)
    {
      otWeberStem clone(parent,parent->level,trf,parent->offset);
      clone.parent = parent->parent;
      clone.segmentLength = parent->segmentLength;
      clone.length = parent->length;
      clone.baseRadius = parent->baseRadius;
      clone.splitCorrection = parent->splitCorrection;
      if (!prune)
      {
        clone.lengthChildMax = parent->lengthChildMax;
        clone.substemsPerSegment = parent->substemsPerSegment;
        clone.substemRotangle = parent->substemRotangle+180;
      }

      return clone;
    }

    inline static void split(otWeberStem* splitstem, otMatrix& trf, float s_angle, int nseg, bool prune=false)
    {

      float declination = acos(trf.getZ().Z)*180/PI;
      float split_angle = splitstem->level.splitAngle + calcVariance(splitstem->level.splitAngleV);
      split_angle = max(0.0f, split_angle - declination);

      if (splitstem->level.levelNumber != 0)
        s_angle = 0;

      float split_diverge;
      if (s_angle<=0)
      {
        split_diverge = 0;
      } 
      else if (splitstem->params.trunk.baseSplits>0 && splitstem->level.levelNumber==0 && nseg==0) 
      {
        split_diverge = s_angle + calcVariance(splitstem->level.splitAngleV);
      }
      else
      {
	split_diverge = 20 + .75f * (30 + fabsf(declination - 90.0f)) * pow(calcRandom(), 2);
      }

      if (split_diverge+s_angle == 0)
        trf.lrotXDegree(split_angle); // if split_angle == 0, rotation will be skiped automatically.
      else
        otWeberHelper::rotXZDegree(trf, split_angle, split_diverge+s_angle);

      splitstem->splitCorrection -=  split_angle/(splitstem->level.curveRes-nseg-1);
    }

    static inline float getShapeRatio(float ratio, int shape, const TreeData& par)
    {
      switch (shape)
      {
        case CONICAL: return 0.2f+0.8f*ratio;
        case SPHERICAL: return 0.2f+0.8f*sinf(PI*ratio);
        case HEMISPHERICAL: return 0.2f+0.8f*sinf(0.5f*PI*ratio);
        case CYLINDRICAL: return 1.0f;
        case TAPERED_CYLINDRICAL: return 0.5f+0.5f*ratio;
        case FLAME: return (ratio<=0.7f?ratio/0.7f : (1-ratio)/0.3f);
        case INVERSE_CONICAL: return 1-0.8f*ratio;
        case TEND_FLAME: return (ratio<=0.7f? 0.5f+0.5f*ratio/0.7f : 0.5f+0.5f*(1-ratio)/0.3f);
        case ENVELOPE:
        {
          if (ratio<0 || ratio>1) return 0;
          else if (ratio<(1-par.pruneWidthPeak)) return powf(ratio/(1-par.pruneWidthPeak),par.prunePowerHigh);
          else return powf((1-ratio)/(1-par.pruneWidthPeak),par.prunePowerLow);
        }
      }
      otAssert("otWeberStem::getShapeRatio => Unknown Shape");
      return 0;
    }

    static inline float stemRadius(const otWeberStem* stem, float h)
    {
      Level& level = stem->level;

      float Z = min(h / stem->length, 1.0f); // limited to one.

      float unit_taper=0;
      float radius_z=0;
      
      if (level.taper >= 0 && level.taper < 1)
        unit_taper = level.taper;
      else if (level.taper >= 1 && level.taper < 2)
        unit_taper = 2 - level.taper;
      else if (level.taper >= 2 && level.taper < 3)
        unit_taper = 0;
      
      // purely tapered radius
      float taper_z = stem->baseRadius * (1 - unit_taper * Z);
      
      if (level.taper >= 0 && level.taper < 1)
        radius_z = taper_z;
      else if (level.taper >= 1 && level.taper <= 3)
      {
        float depth, Z3;
        float Z2 = (1 - Z) * stem->length;
        
        if (level.taper < 2 || Z2 < taper_z)
          depth = 1;
        else
          depth = level.taper - 2;
        
        if (level.taper < 2)
          Z3 = Z2;
        else
          Z3 = fabsf(Z2 - 2 * taper_z * (int)(Z2 / (2 * taper_z) + 0.5));
        
        if (level.taper < 2 && Z3 >= taper_z)
          radius_z = taper_z;
        else
          radius_z = (1-depth) * taper_z + depth * sqrtf(max(0.0f,powf(taper_z, 2) - powf(Z3 - taper_z, 2)));
      }
      
      // Flaring
      if (level.levelNumber == 0)
      {
        float y = max(1 - 8 * Z, 0.0f); // "the value of y is limited to a minimum of zero"
        float flare_z = stem->params.flare * (pow(100.0f, y) - 1) / 100.0f + 1.0f;
        radius_z *= flare_z;
      }
      
      return radius_z;
    }

  };

  /// }@

}

#endif //_WEBER_STEMHELPER_H_
