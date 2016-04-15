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

#ifndef _OT_TRIANGLES_H_
#define _OT_TRIANGLES_H_

namespace opentree
{
  class otTriangle
  {
  public:
    int v1, v2, v3;

    otTriangle(unsigned int V1, unsigned int V2, unsigned int V3) 
    {
      v1=V1; v2=V2; v3=V3; 
    }

    otTriangle(const otTriangle& v) 
    {
      v1=v.v1; v2=v.v2; v3=v.v3; 
    }

    otTriangle operator+ (int n) 
    {
      return otTriangle(v1+n, v2+n, v3+n); 
    }

    otTriangle& operator+= (int n) 
    {
      v1+=n; v2+=n; v3+=n;
      return *this; 
    }
  };

  class otTriangles
  {
  private:
    int counter;

    virtual void addTriangle(int v1, int v2, int v3) = 0;

  public:
    otTriangles() : counter(0) {}
    virtual ~otTriangles() {}

    void add(otTriangle t)
    {
      addTriangle(t.v1, t.v2, t.v3);
      counter++;
    }

    void add(int v1, int v2, int v3)
    {
      addTriangle(v1, v2, v3);
      counter++;
    }

    int getCount()
    { 
      return counter; 
    }
  };
}

#endif //_OT_TRIANGLES_H_
