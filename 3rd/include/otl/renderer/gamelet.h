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

#ifndef _GAMELET_H_
#define _GAMELET_H_

#include <opentree/utils/otvector3.h>

/// \addtogroup Forest
/// @{

class Renderer;

enum Events
{
  EV_MOUSE,
  EV_KEY,
  EV_QUIT
};

class Gamelet
{
protected:
  Renderer* renderer;

public:
  virtual ~Gamelet() {}

  virtual void init() = 0;

  virtual void setRenderer(Renderer* renderer)
  {
    this->renderer = renderer;
  }

  virtual void handleEvent(Events ev, int a=0, int b=0, int c=0) = 0;

  virtual void draw() = 0;
};

/// }@

#endif // _FOREST_GAMELET_H_
