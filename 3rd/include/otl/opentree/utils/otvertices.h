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

#ifndef _OT_VERTICES_H_
#define _OT_VERTICES_H_

#include "otvector3.h"

namespace opentree
{

  class otVertex
  {
  public:
    ///position
    float x, y, z;
    ///normal
    float nx, ny, nz;
    ///color
    float r, g, b;
    ///texture
    float u, v;

    otVertex(float X, float Y, float Z, float nX, float nY, float nZ, 
      float R, float G, float B, float U, float V)
    {
      x=X; y=Y; z=Z;
      nx=nX; ny=nY; nz=nZ;
      r=R; g=G; b=B;
      u=U; v=V;
    }

    otVertex(const otVector3& p, const otVector3& n, const otVector3& c, float U, float V)
    { 
      x=p.X; y=p.Y; z=p.Z;
      nx=n.X; ny=n.Y; nz=n.Z;
      r=c.X; g=c.Y; b=c.Z;
      u=U; v=V;
    }

    otVertex(const otVector3& p, const otVector3& n)
    { 
      x=p.X; y=p.Y; z=p.Z;
      nx=n.X; ny=n.Y; nz=n.Z;
      r=1; g=1; b=1;
      u=0; v=0;
    }
  };

  class otVertices
  {
  private:
    unsigned int counter;

    virtual void add(int index, float x, float y, float z, float nx, float ny,
      float nz, float r, float g, float b, float a, float u, float v) = 0;

  public:
    otVertices() : counter(0) {}
    virtual ~otVertices() {}

    inline void add(otVertex v)
    {
      add(counter, v.x, v.y, v.z, v.nx, v.ny, v.nz, v.r, v.g, v.b, 1, v.u, v.v);
      counter++;
    }

    inline void add(const otVector3& p, const otVector3& n, const otVector3& c, float u, float v)
    {
      add(counter, p.X, p.Y, p.Z, n.X, n.Y, n.Z, c.X, c.Y, c.Z, 1, u, v);
      counter++;
    }

    inline unsigned int getCount() const
    {
      return counter;
    }
  };

}

#endif //_OT_VERTICES_H_
