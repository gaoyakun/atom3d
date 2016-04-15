/*
  Copyright (c) 2004-2005 Pascal Kirchdorfer, Seth Yastrov

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

#include "opentree/mesher/cylmesher.h"

#include "opentree/utils/otarray.h"
#include "opentree/utils/otvector3.h"
#include "opentree/utils/ottriangles.h"
#include "opentree/utils/otvertices.h"


namespace opentree
{
  
  /// \addtogroup Mesher
  /// @{

  MesherCyl::MesherCyl() {}

  MesherCyl::~MesherCyl()
  {
  }

  void MesherCyl::getSide(otTriangles& triangles, int segments, int offset_a, int offset_b)
  {
    triangles.add(offset_b+segments-1, offset_b, offset_a);
    for (int i=0; i<segments-1; i++)
    {
        triangles.add(offset_a+i, offset_b+i,   offset_a+i+1);
        triangles.add(offset_b+i, offset_b+i+1, offset_a+i+1);
    }
    triangles.add(offset_a+segments-1,offset_b+segments-1,offset_a);
  }

  void MesherCyl::getCircle(otVertices& vertices, int segments, float radius, otMatrix& trf, otVector3& pos, float v)
  {
    float angle = 2*PI/(segments);
    float dSin = sin(angle);
    float dCos = cos(angle);

    float x=0, y=1;
    float x_temp, y_temp;

    float du=2.0f/segments;
    float u = v/10.0f;

    otVector3 p;
    otVector3 n;
    otVector3 c = otVector3(1, 1, 1);
    // create circle vertices
    for (int  i=0; i<segments; i++)
    {
      x_temp = x;
      y_temp = y;

      n.set(x,y,0);
      trf.applyTo(n);

      p = n;
      p *= radius;
      p += pos;

      vertices.add(p,n,c,u,v);

      u += (i<segments/2.0f)?du:-du;

      x = x_temp*dCos + y_temp*dSin;
      y = y_temp*dCos - x_temp*dSin;
    }
  }

  void MesherCyl::getCap(otTriangles* triangles, int segments, int offset, bool end)
  {
    if (end)
    {
      for (int i = 0; i < segments; i++)
      {
        triangles->add(offset+segments, offset+(i==segments-1?0:i+1), offset+i);
      }
    }
    else 
    {
      for (int i = 0; i < segments; i++)
      {
        triangles->add(offset, offset+i+1, offset+(i==segments-1?1:i+2));
      }
    }
  }

  void MesherCyl::getCap(otVertices* vertices, bool end, int segments, otVector3& pos, otMatrix& t, float radius)
  {
    otVector3 p;
    otVector3 n;
    otVector3 c;

    float angle = 2*PI/(segments);
    float dSin = sin(angle);
    float dCos = cos(angle);

    n.set(0,0,(end?1.0f:-1.0f));
    t.applyTo(n);

    if (!end)
    {
      vertices->add(pos,n,c,0,1);
    }

    float x=0, y=1;
    float x_temp, y_temp;

    for (int i=0; i<segments; i++)
    {
      x_temp = x;
      y_temp = y;

      p.set(x,y,0);
      t.applyTo(p);
      p *= radius;
      p += pos;

      vertices->add(p,n,c,0,0);

      x = x_temp*dCos + y_temp*dSin;
      y = y_temp*dCos - x_temp*dSin;
    }

    if (end)
    {
      vertices->add(pos,n,c,0,1);
    }
  }

}

