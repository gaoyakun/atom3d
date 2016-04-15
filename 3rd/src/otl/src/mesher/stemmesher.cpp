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

#include "opentree/mesher/stemmesher.h"
#include "opentree/otstem.h"
#include "opentree/utils/otarray.h"
#include "opentree/utils/otvector3.h"
#include "opentree/utils/ottriangles.h"
#include "opentree/utils/otvertices.h"

#include "opentree/mesher/cylmesher.h"
#include "opentree/mesher/mesher.h"

namespace opentree
{

  /// \addtogroup Mesher
  /// @{

  MesherStem::MesherStem() {}

  MesherStem::MesherStem(const MesherStem& s)
  : stem(s.stem), lod(s.lod), levelNo(s.levelNo)
  {
  }

  MesherStem::MesherStem(otStem* s)
  : stem(s), lod(lod), levelNo(s->levelNo)
  {
    vertexOffset = 0;
    c.set(0.4f,0.3f,0);

    circleRes=0; curveRes=0;
  }

  MesherStem::~MesherStem()
  {
  }

  void MesherStem::getVertices(otVertices& vertices)
  { 
    calcVertices(&vertices);
  }
  void MesherStem::getIndices(otTriangles& triangles, unsigned int offset)
  {
    vertexOffset += offset;
    calcIndices(&triangles);
  }

  /// Calculate curveres and circleres by #lod
  /// \param lod value between 0 and 1
  void MesherStem::setLOD(const float lod)
  {
    setCurveRes((int)stem->getPosSpline()->getLength()*2);
    setCircleRes((int)(28*lod/(levelNo+2)));
  }

  void MesherStem::setCurveRes(const int curves)
  {
    curveRes=curves;

    otStem* parent = stem;
    while (parent->parent && parent->levelNo == parent->parent->levelNo) // is clone
      parent = parent->parent;

    float thislen = stem->getPosSpline()->getLength();
    float parentlen = parent->getPosSpline()->getLength();
    curveRes = int(curveRes * thislen / parentlen + 0.4999f);
  }

  void MesherStem::setCircleRes(const int circles)
  {
    circleRes = circles;
    angle = 2*PI/(circleRes);
    dSin = sin(angle);
    dCos = cos(angle);
  }
  
  
  int MesherStem::countVertices()
  {
    if (circleRes >= 2 && curveRes >= 0)
    {
      if (circleRes == 2) circleRes = 3;
      if (curveRes == 0) curveRes = 1;

      //cap begin
      int vertex = circleRes+1;

      //side stem
      vertex += (curveRes+1) * circleRes;

      //cap end
      vertex += circleRes+1;

      return vertex;
    }
    return 0;
  }

  int MesherStem::countIndices()
  {
    if (circleRes >= 2 && curveRes > 0)
    {
      if (circleRes == 2) circleRes = 3;
      if (curveRes == 0) curveRes = 1;

      //cap begin
      int triangle = circleRes;
      //side cap<->stem
      triangle += 2 * circleRes;

      //side stem
      triangle += 2 * (curveRes) * circleRes;

      //side cap<->stem
      triangle += 2 * circleRes;

      //cap end
      triangle += circleRes;

      return triangle;
    }
    return 0;
  }

  void MesherStem::calcVertices(otVertices* vertices)
  {
    if (circleRes >= 2 && curveRes >= 0)
    {
      if (circleRes == 2) circleRes = 3;
      if (curveRes == 0) curveRes = 1;

      vertexOffset = vertices->getCount();

      getCap(vertices, false);
      for (int j = 0; j<=curveRes; j++)
      {
        getCircle(j, vertices);
      }
      getCap(vertices, true);
    }
  }

  void MesherStem::calcIndices(otTriangles* triangles)
  {
    if (circleRes >= 2 && curveRes > 0)
    {
      if (circleRes == 2) circleRes = 3;
      if (curveRes == 0) curveRes = 1;

      getCap(triangles, false);
      for (int j = -1; j<curveRes+1; j++)
      {
        getSide(j, j+1, triangles);
      }
      getCap(triangles, true);
    }
  }

  void MesherStem::getCap(otTriangles* triangles, bool end)
  {
    if (end)
    {
      int o = vertexOffset + (curveRes+1) * circleRes + (circleRes+1);
      MesherCyl cylmesh;
      cylmesh.getCap(triangles, circleRes, o, end);
    }
    else 
    {
      int o = vertexOffset;
      MesherCyl cylmesh;
      cylmesh.getCap(triangles, circleRes, o, end);
    }
  }

  void MesherStem::getCap(otVertices* vertices, bool end)
  {
    float spline_pos = float((end?curveRes:0))/(curveRes);
    pos = stem->getPosSpline()->getPoint(spline_pos);
    z = stem->getPosSpline()->getTangent(spline_pos);

    otMatrix t;
    otVector3 xaxis(z & otVector3(0,0,1));
    t.lrotateRad(asin(xaxis.length()),xaxis);

    float radius = stem->getRadSpline()->getPoint(spline_pos);

    MesherCyl cylmesh;
    cylmesh.getCap(vertices, end, circleRes, pos, t, radius);
  }

  void MesherStem::getSide(int a, int b, otTriangles* triangles)
  {
    int o1 = getOffset(a);
    int o2 = getOffset(b);

    MesherCyl cylmesh;
    cylmesh.getSide(*triangles, circleRes, o1, o2);
  }

  void MesherStem::getCircle(int a, otVertices* vertices)
  {
    float spline_pos = float(a)/(curveRes);
    pos = stem->getPosSpline()->getPoint(spline_pos);
    z = stem->getPosSpline()->getTangent(spline_pos);

    otMatrix t;
    otVector3 xaxis(z & otVector3(0,0,1));
    t.lrotateRad(asin(xaxis.length()),xaxis);

    if (a == 0 && stem->parent)
    {
      pos = stem->parent->getPosSpline()->findNearest(pos);
    }

    float radius = stem->getRadSpline()->getPoint(spline_pos);

    float zpos = (z | pos)/(20*PI);
    if (radius)
      zpos = zpos/radius;
    else 
      zpos = 2*zpos/stem->getRadSpline()->getPoint(float(a-1)/(curveRes));

    float v = zpos * 10;

    // create circle vertices
    MesherCyl cylmesh;
    cylmesh.getCircle(*vertices, circleRes, radius, t, pos, v);
  }

  int MesherStem::getOffset(int i)
  {
    return vertexOffset + (i) * circleRes +(circleRes+1);
  }

}

