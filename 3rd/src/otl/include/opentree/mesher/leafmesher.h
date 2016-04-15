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

#ifndef _MESHER_LEAF_H_
#define _MESHER_LEAF_H_

#include "mesher.h"
#include "opentree/utils/otutil.h"
#include "opentree/utils/otvector3.h"

namespace opentree
{

  /// \addtogroup Mesher
  /// @{

  class otLeaf;
  class otVertices;
  class otTriangles;
  
  /** 
   * Creates a mesh for one leaf.
   */
  class MesherLeaf : public iMesher
  {
    otLeaf& leaf;
    char leafedges;
    otVector3 leafColor;

  public:
    MesherLeaf(otLeaf& leaf);
    virtual ~MesherLeaf();

    unsigned int getVerticesCount()
    {
      if (leafedges == 1)
        return 1;
      if (leafedges == 3)
        return 6;
      if (leafedges == 4)
        return 8;
      else
      {
        otAssert ("leafedges is not 1, 3 or 4!");
        return 0;
      }
    }

    unsigned int getIndicesCount()
    {
      if (leafedges == 1)
        return 1;
      if (leafedges == 3)
        return 2;
      if (leafedges == 4)
        return 4;
      else
      {
        otAssert ("leafedges is not 1, 3 or 4!");
        return 0;
      }
    }

    //void usePoints() { leafedges = 1; }
    void useTriangles() { leafedges = 3; }
    void useQuads() { leafedges = 4; }

    void getVertices(otVertices& vertices);
    void getIndices(otTriangles& triangles, unsigned int offset);

    void setColor(otVector3 color) {
      leafColor = color;
    }
  };

};

#endif // _MESHER_LEAF_H_
