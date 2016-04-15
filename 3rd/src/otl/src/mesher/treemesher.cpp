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

#include "opentree/mesher/treemesher.h"

#include "opentree/utils/otvertices.h"
#include "opentree/utils/ottriangles.h"

#include "opentree/ottree.h"
#include "opentree/otleaf.h"

namespace opentree
{

  /// \addtogroup Mesher
  /// @{

  unsigned int MesherTree::countStems(otStem* stem)
  {
    unsigned int count = stem->getChildCount();
    for (unsigned int i = 0; i<stem->getChildCount(); i++)
    {
      if (stem->getChild(i)->getChildCount())
        count += countStems(stem->getChild(i));
    }
    return count;
  }

  void MesherTree::parseStems(otStem* stem)
  {
    MesherStem stemmesher(stem);
    stems.add(stemmesher);

    for (unsigned int i = 0; i<stem->getChildCount(); i++)
    {
      parseStems(stem->getChild(i));
    }
  }

  MesherTree::MesherTree(otTree* t) : tree(t), leafedges(4)
  { 
    // There is nothing slower than memory allocation.
    // So we first count all Stems.
    unsigned int count = tree->getTrunkCount();

    unsigned int i;
    for (i = 0; i<tree->getTrunkCount(); i++)
    {
      count += countStems(tree->getTrunk(i));
    }
    // And then allocate them all at once.
    stems.setGrowRate(count);
    for (i = 0; i<tree->getTrunkCount(); i++)
    {
      parseStems(tree->getTrunk(i));
    }

    usePseudoSelfShadowing = true;
  }

  MesherTree::~MesherTree()
  {
  }

  void MesherTree::getVerticesCount(int level, int* vertex)
  {
    for (unsigned int i=0; i< stems.getCount(); i++)
    {
      if (stems.get(i).levelNo == level)
        *vertex += stems.get(i).getVerticesCount();
    }
  }

  void MesherTree::getIndicesCount(int level, int* triangle)
  {
    *triangle = 0;
    for (unsigned int i=0; i< stems.getCount(); i++)
    {
      if (stems.get(i).levelNo == level)
        *triangle += stems.get(i).getIndicesCount();
    }
  }

  void MesherTree::getVertices(int level, otVertices& vertices)
  {
    for (unsigned int i=0; i< stems.getCount(); i++)
    {
      if (stems.get(i).levelNo == level)
        stems.get(i).getVertices(vertices);
    }
  }

  void MesherTree::getIndices(int level, otTriangles& triangles, unsigned int offset)
  {
    for (unsigned int i=0; i< stems.getCount(); i++)
    {
      if (stems.get(i).levelNo == level)
        stems.get(i).getIndices(triangles, offset);
    }
  }

  void MesherTree::setCurveRes(int level, int res)
  {
    for (unsigned int i=0; i< stems.getCount(); i++)
    {
      if (stems.get(i).levelNo == level)
        stems.get(i).setCurveRes(res);
    }
  }
  void MesherTree::setCircleRes(int level, int res)
  {
    for (unsigned int i=0; i< stems.getCount(); i++)
    {
      if (stems.get(i).levelNo == level)
        stems.get(i).setCircleRes(res);
    }
  }

  //------------------------------------------------------------------

  void MesherTree::getLeavesVerticesCount(int* vertex)
  {
    if (tree->getLeafCount() == 0)
      return;
    // All leaves are the same.
    MesherLeaf leafmesher(tree->getLeaf(0));
    if (leafedges == 1)
    {
      //leafmesher.usePoints();
    }
    else if (leafedges == 3)
      leafmesher.useTriangles();
    else if (leafedges == 4)
      leafmesher.useQuads();
    *vertex += tree->getLeafCount() * leafmesher.getVerticesCount();
  }

  void MesherTree::getLeavesIndicesCount(int* triangle)
  {
    if (tree->getLeafCount() == 0)
      return;
    // All leaves are the same.
    MesherLeaf leafmesher(tree->getLeaf(0));
    if (leafedges == 1)
    {
      //leafmesher.usePoints();
    }
    else if (leafedges == 3)
      leafmesher.useTriangles();
    else if (leafedges == 4)
      leafmesher.useQuads();
    *triangle += tree->getLeafCount() * leafmesher.getIndicesCount();
  }

  void MesherTree::getLeavesVertices(otVertices& vertices)
  {
    otVector3 shadowCenter;
    float maxDistance = 0.0f;
    if (usePseudoSelfShadowing)
    {
      // Calculate shadow center by averaging the positions of all leaves.
      // This is rather simple - shadow center could be calculated per stem.
      for (unsigned int i = 0; i < tree->getLeafCount(); i++)
      {
        otVector3 leafPos = tree->getLeaf(i).transform.getTranslation();
        shadowCenter += leafPos;
      }

      shadowCenter /= (float)tree->getLeafCount();

      // Now find the maximum distance
      for (unsigned int i = 0; i < tree->getLeafCount(); i++)
      {
        otVector3 leafPos = tree->getLeaf(i).transform.getTranslation();
        float distance = (leafPos - shadowCenter).length();
        if (distance > maxDistance)
          maxDistance = distance;
      }

    }

    for (unsigned int i = 0; i < tree->getLeafCount(); i++)
    {
      MesherLeaf leafmesher(tree->getLeaf(i));
      if (leafedges == 1)
      {
        //leafmesher.usePoints();
      }
      else if (leafedges == 3)
        leafmesher.useTriangles();
      else if (leafedges == 4)
        leafmesher.useQuads();

      if (usePseudoSelfShadowing)
      {
        otVector3 leafPos = tree->getLeaf(i).transform.getTranslation();
        float dist = (leafPos - shadowCenter).length();
        leafmesher.setColor(otVector3(dist/maxDistance,
          dist/maxDistance, dist/maxDistance));
      }

      leafmesher.getVertices(vertices);
    }
  }

  void MesherTree::getLeavesIndices(otTriangles& triangles, unsigned int offset)
  {
    for (unsigned int i = 0; i < tree->getLeafCount(); i++)
    {
      MesherLeaf leafmesher(tree->getLeaf(i));
      if (leafedges == 1)
      {
        //leafmesher.usePoints();
      }
      else if (leafedges == 3)
        leafmesher.useTriangles();
      else if (leafedges == 4)
        leafmesher.useQuads();

      leafmesher.getIndices(triangles, offset);
    }
  }

  void MesherTree::setLeavesPseudoSelfShadowing(bool selfShadowing)
  {
    usePseudoSelfShadowing = selfShadowing;
  }

  bool MesherTree::getLeavesPseudoSelfShadowing()
  {
    return usePseudoSelfShadowing;
  }
};
