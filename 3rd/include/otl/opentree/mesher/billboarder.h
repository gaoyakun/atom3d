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

#ifndef _MESHER_BILLBOARD_H_
#define _MESHER_BILLBOARD_H_

#include "opentree/opentree.h"
#include "opentree/utils/otbox3.h"
#include "opentree/utils/otmatrix.h"
#include "opentree/utils/otvector3.h"
#include "opentree/utils/ottriangles.h"
#include "opentree/utils/otvertices.h"

namespace opentree
{

  /// \addtogroup Mesher
  /// @{

  /** simple Billboards
   * Creates a simlpe billboards at the center of the mesh.
   */
  class MesherBillboard : public iMesher
  {
    otMatrix& trf;
    otBox3 t_box;

  public:
    MesherBillboard(const otVector3* vertices, unsigned int count, otMatrix& trf)
    : trf(trf)
    {
      otBox3 box;
      for (unsigned int i = 0; i < count; i++)
      {
        box.extend(vertices[i]);
        t_box.extend(trf.apply(vertices[i]));
      }
      t_box.setCenter(box.getCenter());
    }

    ~MesherBillboard() {}

    unsigned int getVerticesCount()
    {
      return 4;
    }
      
    void getVertices(otVertices& vertices)
    {
      otVector3 center = t_box.getCenter();

      float length = t_box.getSize().X/2;
      float width = t_box.getSize().Z/2;

      otVector3 c(0,0.6f,0);

      otVector3 v1 = center + trf*(otVector3(-length,0,-width));
      otVector3 v2 = center + trf*(otVector3( length,0,-width));
      otVector3 v3 = center + trf*(otVector3(-length,0, width));
      otVector3 v4 = center + trf*(otVector3( length,0, width));

      otVector3 normal = trf.apply(otVector3(0,-1,0));

      vertices.add(otVertex(v1,normal,c,0,0));
      vertices.add(otVertex(v2,normal,c,1,0));
      vertices.add(otVertex(v3,normal,c,0,1));
      vertices.add(otVertex(v4,normal,c,1,1));
    }

    unsigned int getIndicesCount()
    {
      return 4;
    }

    void getIndices(otTriangles& triangles, unsigned int offset)
    {
      triangles.add(otTriangle(0,1,2)+offset);
      triangles.add(otTriangle(1,3,2)+offset);
    }

    const otBox3& getBox() const
    {
      return t_box;
    }
  };

};

#endif // _MESHER_BILLBOARD_H_
