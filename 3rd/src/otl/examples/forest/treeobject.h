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

#ifndef _FOREST_TREEOBJECT_H_
#define _FOREST_TREEOBJECT_H_

#include "treefactory.h"

#include <opentree/utils/otmatrix.h>
#include <opentree/utils/otutil.h>

/// \addtogroup Forest
/// @{

class Renderer;

class TreeObject
{
  friend class TreeFactory;
private:
  TreeFactory* factory;
  opentree::otMatrix trf;
  char* name;

  TreeObject(TreeFactory* factory) : factory(factory)
  {
  }

  bool isVisible(opentree::otMatrix& camera)
  {
    // Simple frustum culling, see if it's inside the view cone
    opentree::otVector3 dist = (camera*trf).getTranslation();
    float angle = -dist.Z / dist.length();
    return angle > cos(55*D2RAD);
  }

public:
  ~TreeObject() {}

  const char* getName()
  {
    return name;
  }

  void setName(const char* name)
  {
    this->name = new char[strlen(name)+1];
    strcpy(this->name,name);
  }

  void setTransformation(opentree::otMatrix& trf)
  {
    this->trf = trf;
  }

  void draw(Renderer* renderer, opentree::otMatrix& camera, unsigned int& vertcnt, unsigned int& polycnt)
  {
    if(isVisible(camera))
    {
      factory->draw(renderer, camera, trf, vertcnt, polycnt);
    }
  }
};

/// }@

#endif // _FOREST_TREEOBJECT_H_
