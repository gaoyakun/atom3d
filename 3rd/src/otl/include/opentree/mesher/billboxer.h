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

#ifndef _MESHER_BILLBOX_H_
#define _MESHER_BILLBOX_H_

#include "opentree/opentree.h"
#include "opentree/utils/otbox3.h"
#include "opentree/utils/ottriangles.h"
#include "opentree/utils/otvertices.h"

namespace opentree
{

  /// \addtogroup Mesher
  /// @{

  /** !!NOT IMPLEMENTED!! Box of Billboards
   * Creates a box of billboards around the mesh.
   */
  class MesherBillbox : public iMesher
  {
    otBox3 box;

  public:
    MesherBillbox(Vertex* vertices, unsigned int count)
    {
      for (unsigned int i = 0; i < count; i++)
      {
        box.extend(otVector3(vertices[i].x, vertices[i].y, vertices[i].z));
      }
    }
    ~MesherBillbox() {}

    unsigned int getVerticesCount()
    {
      return 14;
    }

    void getVertices(otVertices& vertices)
    {
      otVector3 center = box.getCenter();

      float width = box.getSize().Y/2;
      float length = box.getSize().X/2;
      float height = box.getSize().Z/2;

      otVector3 c(0,0.6f,0);

      otVector3 v0 = otVector3(-length,-height,-width);
      otVector3 v1 = otVector3(-length,-height, width);
      otVector3 v2 = otVector3( length,-height,-width);
      otVector3 v3 = otVector3( length,-height, width);
      otVector3 v4 = otVector3(-length, height,-width);
      otVector3 v5 = otVector3(-length, height, width);
      otVector3 v6 = otVector3( length, height,-width);
      otVector3 v7 = otVector3( length, height, width);

      otVector3 n0 = v0; n0.normalize();
      otVector3 n1 = v1; n1.normalize();
      otVector3 n2 = v2; n2.normalize();
      otVector3 n3 = v3; n3.normalize();
      otVector3 n4 = v4; n4.normalize();
      otVector3 n5 = v5; n5.normalize();
      otVector3 n6 = v6; n6.normalize();
      otVector3 n7 = v7; n7.normalize();

      vertices.add(otVertex(center+v0,n0,c,0   ,0     ));
      vertices.add(otVertex(center+v1,n1,c,0   ,0.333f));
      vertices.add(otVertex(center+v2,n2,c,0.5f,0     ));
      vertices.add(otVertex(center+v3,n3,c,0.5f,0.333f));
      vertices.add(otVertex(center+v1,n1,c,0.5f,0.666f));
      vertices.add(otVertex(center+v6,n6,c,1   ,0     ));
      vertices.add(otVertex(center+v7,n7,c,1   ,0.333f));
      vertices.add(otVertex(center+v5,n5,c,1   ,0.666f));
      vertices.add(otVertex(center+v1,n1,c,1   ,1     ));
      vertices.add(otVertex(center+v6,n6,c,1.5f,0.333f));
      vertices.add(otVertex(center+v4,n4,c,1.5f,0.666f));
      vertices.add(otVertex(center+v0,n0,c,1.5f,1     ));
      vertices.add(otVertex(center+v6,n6,c,2   ,0.666f));
      vertices.add(otVertex(center+v2,n2,c,2   ,1     ));
    }

    unsigned int getIndicesCount()
    {
      return 12;
    }

    void getIndices(otTriangles& triangles)
    {
      triangles.add(otTriangle( 0, 3, 1));
      triangles.add(otTriangle( 0, 2, 3));
                               
      triangles.add(otTriangle( 2, 6, 3));
      triangles.add(otTriangle( 2, 5, 6));
                               
      triangles.add(otTriangle( 3, 7, 4));
      triangles.add(otTriangle( 3, 6, 7));
                               
      triangles.add(otTriangle( 6,10, 7));
      triangles.add(otTriangle( 6, 9,10));
                               
      triangles.add(otTriangle( 7,11, 8));
      triangles.add(otTriangle( 7,10,11));
                               
      triangles.add(otTriangle(10,13,11));
      triangles.add(otTriangle(10,12,13));
    }

    const otBox3& getBox() const
    {
      return box;
    }
  };

};

#endif // _MESHER_BILLBOX_H_
