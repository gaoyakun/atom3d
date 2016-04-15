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

#ifndef _FOREST_IMPOSTER_H_
#define _FOREST_IMPOSTER_H_

#include <renderer/material.h>
#include <renderer/mesh.h>

#include <opentree/mesher/meshtypes.h>

/// \addtogroup Forest
/// @{

class Vertex;
class Triangle;

class Imposter
{
private:
  Mesh* highpoly;
  Mesh* billboard;

  opentree::otMatrix camera;

public:
  Imposter(Mesh* highpoly, Mesh* billboard) 
  : highpoly(highpoly), billboard(billboard)
  {
  }

  ~Imposter() {}

  void update(Renderer* renderer)
  {
    int x, y, w, h;
    renderer->getViewport(x, y, w, h);
    Texture* tex = billboard->getMaterial()->getTexture(0);
    renderer->setViewport(0,0,tex->width, tex->height);
    camera.scale(1,0.75f,0.75f);
    renderer->setCamera(camera);
    renderer->clear(0,0,0,0);
    highpoly->draw(renderer);
    renderer->render2Texture(tex->id,tex->width, tex->height);
    renderer->setViewport(x, y, w, h);
  }

  opentree::otMatrix& getCamera()
  {
    return camera;
  }
};

/// }@

#endif // _FOREST_IMPOSTER_H_
