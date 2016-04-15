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

#ifndef _WEBER_STEMFACTORY_H_
#define _WEBER_STEMFACTORY_H_

#include <opentree/weber/webertypes.h>
#include <opentree/utils/otarray.h>
#include <opentree/utils/otmatrix.h>

#include "pruner.h"

namespace opentree
{
  /// \addtogroup Weber_Algorithm
  /// @{

  class otWeberStemFactory 
  {
  private:
    otWeberStemRelations* stemrelations;
    otWeberStem* stem;
    TreeData& par;
    Level& lpar;

    void makeSegments();

  public:
    otWeberStemFactory(otWeberStem* stem, otWeberStemRelations* stemrelations)
    : stemrelations(stemrelations), stem(stem), par(stem->params), lpar(stem->level)
    {
    }
    
    ~otWeberStemFactory()
    {
    }

    bool make() 
    {
      stem->length = stemLength();
      stem->segmentLength = stem->length/lpar.curveRes;
      stem->baseRadius = stemBaseRadius();
    	
      if (stem->length <= otWeberStemHelper::getMinStemLen() || 
        stem->baseRadius <= otWeberStemHelper::getMinStemRadius())
        return false;

      if (stem->level.levelNumber>0 && par.pruneRatio > 0) 
      {
        otWeberPruner pruner(stem);
        pruner.pruning();
      }
    	
      if (stem->length <= otWeberStemHelper::getMinStemLen() || 
        stem->baseRadius <= otWeberStemHelper::getMinStemRadius())
        return false;

	  if (1 || lpar.levelNumber < par.levels - 1)
	  {
		  prepareSubstemParams();

		  makeSegments();
	  }

      return true;
    }

    void prepareSubstemParams() 
    {
      Level& level1 = otWeberHelper::nextLevel(par, lpar);
    
      // maximum length of a substem
      stem->lengthChildMax = level1.length+calcVariance(level1.lengthV);
    
      // actual number of substems and substems per segment
      float substem_cnt;
      if (stem->level.levelNumber==0) 
      {
        substem_cnt = level1.branches / (1-par.baseSize);
      } 
      else if (stem->level.levelNumber==1) 
      {
        float childlen = stem->length / stem->parent->length / stem->parent->lengthChildMax;
        substem_cnt = (level1.branches * (0.2f + 0.8f * childlen));
      } 
      else 
      {
        substem_cnt = (level1.branches * (1.0f - 0.5f * stem->offset/stem->parent->length));
      }
      stem->substemsPerSegment = substem_cnt / lpar.curveRes;
      stem->substemRotangle = 0;
    }

    float stemBaseRadius() 
    {
      if (stem->level.levelNumber == 0)  // trunk
      {
        return stem->length * par.ratio * par.trunk.scale; 
      }
      else 
      {
        // max radius is the radius of the parent at offset
        float max_radius = otWeberStemHelper::stemRadius(stem->parent, stem->offset);
        float radius = stem->parent->baseRadius * pow(stem->length/stem->parent->length,par.ratioPower);
        return min(radius,max_radius);
      }
    }

    float stemLength()
    {
      if (stem->level.levelNumber == 0) // trunk
      {
        return (lpar.length + calcVariance(lpar.lengthV)) * par.scale_tree;
      }
      else if (stem->level.levelNumber == 1) // first level
      {
        float parlen = stem->parent->length;
        float baselen = par.baseSize*par.scale_tree;
        float ratio  = (parlen-stem->offset)/(parlen-baselen);
        return parlen * stem->parent->lengthChildMax * 
          otWeberStemHelper::getShapeRatio(ratio, par.shape, par);
      }
      else // higher levels
      {
        return stem->parent->lengthChildMax*(stem->parent->length-0.6f*stem->offset);
      }
    }
  };

/// }@

}

#endif // _WEBER_STEMFACTORY_H_
