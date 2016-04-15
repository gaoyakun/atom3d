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

#ifndef _FOREST_FOREST_H_
#define _FOREST_FOREST_H_

#include "treeobject.h"
#include "treeparams.h"

#include <opentree/utils/otarray.h>

/** \addtogroup Forest
 * Forest Walkthrough Application.
 * Main demo application meant to show all features.
 */

/// @{

class Renderer;

class Forest
{
private:
  opentree::otArray<TreeObject*> trees;
  opentree::otArray<TreeFactory*> factories;
  opentree::otArray<Texture*> textures;
  opentree::otArray<TreeParams*> treeparams;
  opentree::otArray<VertexList*> vertexlist;

public:
  Forest() {}
  ~Forest() { trees.delAll(); }

  void addTree(TreeObject* obj)
  {
    trees.add(obj);
  }

  void draw(Renderer* renderer, opentree::otMatrix& cam, unsigned int& vertcnt, unsigned int& polycnt)
  {
    for (unsigned int i=0; i < trees.getCount(); i++)
    {
      trees.get(i)->draw(renderer, cam, vertcnt, polycnt);
    }
  }

  void addFactory(TreeFactory* fact)
  {
    factories.add(fact);
  }

  TreeFactory* findFactory(const char* name)
  {
    for (unsigned int i=0; i < factories.getCount(); i++)
    {
      if (!strcmp(factories.get(i)->getName(), name))
        return factories.get(i);
    }
    return 0;
  }

  void addObject(TreeObject* fact)
  {
    trees.add(fact);
  }

  TreeObject* findObject(const char* name)
  {
    for (unsigned int i=0; i < trees.getCount(); i++)
    {
      if (!strcmp(trees.get(i)->getName(), name))
        return trees.get(i);
    }
    return 0;
  }

  void addTexture(Texture* texture)
  {
    textures.add(texture);
  }

  Texture* findTexture(const char* name)
  {
    for (unsigned int i=0; i < textures.getCount(); i++)
    {
      if (!strcmp(textures.get(i)->getName(), name))
        return textures.get(i);
    }
    return 0;
  }

  void addTreeParams(TreeParams* params)
  {
    treeparams.add(params);
  }

  TreeParams* findTreeParams(const char* name)
  {
    for (unsigned int i=0; i < treeparams.getCount(); i++)
    {
      if (!strcmp(treeparams.get(i)->getName(), name))
        return treeparams.get(i);
    }
    return 0;
  }

  void addVertexList(VertexList* vlist)
  {
    vertexlist.add(vlist);
  }

  VertexList* findVertexList(const char* name)
  {
    for (unsigned int i=0; i < vertexlist.getCount(); i++)
    {
      if (!strcmp(vertexlist.get(i)->getName(), name))
        return vertexlist.get(i);
    }
    return 0;
  }
};

/// }@

#endif // _FOREST_FOREST_H_
