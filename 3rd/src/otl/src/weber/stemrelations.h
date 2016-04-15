/*
  Copyright (c) 2004 Pascal Kirchdorfer, Seth Yastrov

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

#ifndef _WEBER_STEMRELATIONS_H_
#define _WEBER_STEMRELATIONS_H_

#include <opentree/weber/webertypes.h>

#include <opentree/utils/otarray.h>
#include <opentree/utils/otmatrix.h>

#include <opentree/ottree.h>
#include <opentree/otstem.h>

#include "segment.h"
#include "stem.h"
#include "leaf.h"

namespace opentree
{
  class otWeberTree;

  /// \addtogroup Weber_Algorithm
  /// @{

  class otWeberStemRelations
  {
  public:
    /// Array of segments forming this stem
    otArray<otWeberSegment*> segments;
    /// Array of this stem's clones (for splitting)
    otArray<otWeberStemRelations*> clones;      
    /// Array of this stem's substems
    otArray<otWeberStemRelations*> substems;    
    /// Array of this stem's  leaves
    otArray<otWeberLeaf> leaves;      

    otWeberTree* tree;
    otWeberStem* parent;
    otWeberStem* clonedFrom;

    int level;
    float offset;

    otWeberStemRelations(int level, float offset)
    : level(level), offset(offset) {}
    
    ~otWeberStemRelations() 
    {
      clones.delAll();
      substems.delAll();
      segments.delAll();
      //leaves.delAll();
    }
  };

  /// }@

}

#endif // _WEBER_STEMRELATIONS_H_
