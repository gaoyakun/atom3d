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
#ifdef WIN32
#include <windows.h>
#endif //WIN32

#include <SDL.h>

#include <renderer/sdlframework.h>
#include <renderer/gamelet.h>

SDL_FrameWork::SDL_FrameWork(Gamelet* gamelet) : gamelet(gamelet)
{
  renderer.init();
  gamelet->setRenderer(&renderer);
  gamelet->init();
  running = false;
}

SDL_FrameWork::~SDL_FrameWork()
{
  delete gamelet;
}

int SDL_FrameWork::run()
{
  /* Loop until an SDL_QUIT event is found */
  while (!running)
  {
    processEvents();
    updateTree();
    drawFrame();
  }

  return 0;
}

void SDL_FrameWork::processEvents()
{
  SDL_Event event;
  
  /* Poll for events */
  while (SDL_PollEvent(&event))
  {
    switch (event.type)
    {
      case SDL_MOUSEMOTION:
      {
        unsigned char button = 0;
        
        if (event.motion.state & SDL_BUTTON(SDL_BUTTON_LEFT))
          button += 1;
        if (event.motion.state & SDL_BUTTON(SDL_BUTTON_RIGHT))
          button += 2;
        if (event.motion.state & SDL_BUTTON(SDL_BUTTON_MIDDLE))
          button += 4;
        
        int x = event.motion.xrel;
        int y = event.motion.yrel;

        gamelet->handleEvent(EV_MOUSE, x, y, button);

        break;
      }
      case SDL_KEYDOWN:
      {
        gamelet->handleEvent(EV_KEY, event.key.keysym.sym);
        break;
      }
      case SDL_QUIT:
      {
        gamelet->handleEvent(EV_QUIT);
        running = 1;
        break;
      }
      default:
      {
        break;
      }
    }
  }
}

void SDL_FrameWork::updateTree()
{
}

void SDL_FrameWork::drawFrame()
{
  gamelet->draw();
}

void SDL_FrameWork::quit()
{
  SDL_Event ev;
  ev.type = SDL_QUIT;
  SDL_PushEvent(&ev);
}

extern int OTL_main(int argc, char ** argv);

int main(int argc, char ** argv)
{
  return OTL_main(argc, argv);
}
