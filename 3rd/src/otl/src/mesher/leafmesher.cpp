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

#include "opentree/mesher/leafmesher.h"

#include "opentree/opentree.h"
#include "opentree/utils/otarray.h"
#include "opentree/utils/otmatrix.h"
#include "opentree/utils/ottriangles.h"
#include "opentree/utils/otvertices.h"

namespace opentree
{

  /// \addtogroup Mesher
  /// @{

  MesherLeaf::MesherLeaf(otLeaf& leaf) : leaf(leaf), leafedges(4),
    leafColor(otVector3(0,0.6f,0))
  {}

  MesherLeaf::~MesherLeaf() {}
  
  void MesherLeaf::getVertices(otVertices& vertices)
  {
    float width = leaf.width;
    float length = leaf.length;

    if (leafedges == 1)
    {
      //otVector3 v1(-length,0,0);
      //otVector3 normal = leaf.transform.apply(otVector3(0,1,0));
      //vertices.add(otVertex(leaf.transform*v1,normal,leafColor,0,0));
    }
    else if (leafedges == 3)
    {
      otVector3 v1(0,0,-width);
      otVector3 v2(length*1.5f,0,0);
      otVector3 v3(0,0,width);

      otVector3 normal = -leaf.transform.apply(otVector3(0,1,0));

      vertices.add(otVertex(leaf.transform*v1,normal,leafColor,0,1));
      vertices.add(otVertex(leaf.transform*v2,normal,leafColor,0.5f,0));
      vertices.add(otVertex(leaf.transform*v3,normal,leafColor,1,1));

      normal *= -1;

      vertices.add(otVertex(leaf.transform*v1,normal,leafColor,1,1));
      vertices.add(otVertex(leaf.transform*v2,normal,leafColor,0.5f,0));
      vertices.add(otVertex(leaf.transform*v3,normal,leafColor,0,1));
    }
    else if (leafedges == 4)
    {
      otVector3 v1(0,0,0);
      otVector3 v2(0,0,width);
      otVector3 v3(length,0,0);
      otVector3 v4(length,0,width);

      otVector3 normal = leaf.transform.apply(otVector3(0,1,0));

      vertices.add(otVertex(leaf.transform*v1,normal,leafColor,0,0));
      vertices.add(otVertex(leaf.transform*v2,normal,leafColor,1,0));
      vertices.add(otVertex(leaf.transform*v3,normal,leafColor,0,1));
      vertices.add(otVertex(leaf.transform*v4,normal,leafColor,1,1));

      normal *= -1;

      vertices.add(otVertex(leaf.transform*v1,normal,leafColor,0,0));
      vertices.add(otVertex(leaf.transform*v2,normal,leafColor,1,0));
      vertices.add(otVertex(leaf.transform*v3,normal,leafColor,0,1));
      vertices.add(otVertex(leaf.transform*v4,normal,leafColor,1,1));
    }
  }

  void MesherLeaf::getIndices(otTriangles& triangles, unsigned int offset)
  {
    if (leafedges == 1)
    {
      //offset += triangles.getCount();
      //triangles.add(otTriangle(0,1,2)+offset*3);
    }
    else if (leafedges == 3)
    {
      offset += triangles.getCount() * 3;
      triangles.add(otTriangle(0,1,2)+offset);
      triangles.add(otTriangle(3,5,4)+offset);
    }
    else if (leafedges == 4)
    {
      offset += triangles.getCount() * 2;
      triangles.add(otTriangle(0,1,2)+offset);
      triangles.add(otTriangle(1,3,2)+offset);

      triangles.add(otTriangle(4,6,5)+offset);
      triangles.add(otTriangle(5,6,7)+offset);
    }
  }

};

