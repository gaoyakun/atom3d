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

#include <renderer/sdlframework.h>

#include "forestgamelet.h"
#include "generator.h"
#include "loader.h"
#include "xmlparserforest.h"

ForestGamelet::ForestGamelet()
{
}

ForestGamelet::~ForestGamelet()
{
}

void ForestGamelet::init()
{
  Loader loader(forest, renderer);

  xmlParserForest parser(&loader);
  parser.parse("data/forest-xml/demoforest.xml");

  camera.grotXDegree(90);
  camera.grotZDegree(180);
  camera.translate(0,-10,-20);
  renderer->setPerspective(60.0f, 1.5f, .25f, 1000.0f * .25f);
}

void ForestGamelet::handleEvent(Events ev, int a, int b, int c)
{
  switch (ev)
  {
    case EV_MOUSE:
    {
      int x = a;
      int y = b;

      if (c & 1)
      {
        camera.lrotZDegree((float)x);
        camera.grotXDegree((float)y);
      }
      if (c & 2)
      {
        camera.translate((float)x/2.0f,(float)-y/2.0f,0);
      }
      if (c & 4)
      {
        camera.translate(0,0,(float)x+y);
      }
      break;
    }
    case EV_KEY:
    {
      if (a == 27)
      {
        SDL_FrameWork::quit();
      }
      if (a == 'w')
      {
        renderer->showWireFrame(true);
        renderer->showTextures(false);
      }
      if (a == 't')
      {
        renderer->showWireFrame(false);
        renderer->showTextures(true);
      }
      break;
    }
    case EV_QUIT:
    {
      // We quit now... who cares :)
      break;
    }
    default:
    {
      break;
    }
  }
}

void ForestGamelet::draw()
{
  renderer->clear(0.4f,0.2f,0.9f,0);
  unsigned int vertcnt = 0, polycnt = 0;
  forest.draw(renderer, camera, vertcnt, polycnt);
  renderer->updateFPS("Forest v0.1", vertcnt, polycnt);
  renderer->blit();
}
