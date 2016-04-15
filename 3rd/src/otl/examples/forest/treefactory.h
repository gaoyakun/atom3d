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

#ifndef _FOREST_TREEFACTORY_H_
#define _FOREST_TREEFACTORY_H_

#include "lodlevel.h"
#include "imposter.h"

#include <opentree/ottree.h>
#include <opentree/utils/otarray.h>
#include <opentree/utils/otmatrix.h>

/// \addtogroup Forest
/// @{

class TreeObject;
class Renderer;

class TreeFactory
{
private:
  opentree::otTree* ot_tree;
  opentree::otArray<Mesh*> meshes;
  opentree::otArray<LodLevel*> lods;

  char* name;

public:
  TreeFactory() : name(0) {}

  ~TreeFactory() 
  {
    delete [] name;
    meshes.delAll();
    lods.delAll();
  }

  const char* getName()
  {
    return name;
  }

  void setName(const char* name)
  {
    this->name = new char[strlen(name)+1];
    strcpy(this->name,name);
  }

  TreeObject* Create();

  void setTree(opentree::otTree* tree)
  {
    ot_tree = tree;
  }

  opentree::otTree* getTree()
  {
    return ot_tree;
  }

  void addMesh(Mesh* mesh, float from, float to)
  {
    meshes.add(mesh);
    LodLevel* lod = new LodLevel(mesh, from, to);
    lods.add(lod);
  }

  Mesh* findMesh(const char* name)
  {
    for (unsigned int i = 0; i < meshes.getCount(); i++)
    {
      if (!strcmp(meshes.get(i)->getName(),name))
        return meshes.get(i);
    }
    return 0;
  }

  void draw(Renderer* renderer, opentree::otMatrix& camera, opentree::otMatrix& trf, unsigned int& vertcnt, unsigned int& polycnt)
  {
    for (unsigned int i = 0; i < lods.getCount(); i++)
    {
      if (lods.get(i)->isInRange((camera * trf).getTranslation().length()))
      {
        if (lods.get(i)->getMesh()->alignToCamera)
        {
          opentree::otMatrix m;
          m.grotXDegree(90);
          m.grotZDegree(180);
          opentree::otVector3 translation = (camera * trf).getTranslation();
          float v = m.getZ().normalize() | camera.getZ().normalize();
          float v2 = m.getZ().normalize() | camera.getY().normalize();
          float v3 = m.getX().normalize() | camera.getX().normalize();
          m.lrotXRad(acos(v)*(v2/fabsf(v2))*(v3/fabsf(v3)));
          m.translate(translation);
          renderer->setCamera(m);
        }
        else
        {
          renderer->setCamera(camera * trf);
        }
        lods.get(i)->getMesh()->draw(renderer, vertcnt, polycnt);
      }
    }

  }

};

/// }@

#endif // _FOREST_TREEFACTORY_H_
