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

#ifndef __MESH_H_
#define __MESH_H_

#include <renderer/renderer.h>
#include <renderer/texture.h>

#include "vertexlist.h"
#include "tangentlist.h"
#include "material.h"

#include <opentree/mesher/meshtypes.h>
#include <opentree/utils/otvector3.h>

/// \addtogroup Renderer
/// @{

class Vertex;
class Triangle;
class Tangent;
class Material;

class Mesh
{
public:
  VertexList* vertexlist;

  IndexList* indexlist;

  TangentList* tangentlist;

  Material* material;

  char* name;

public:
  bool hasTransparency;
  bool hasAlphaTransparency;
  bool hasBinaryTransparency;
  bool alignToCamera;

public:
  Mesh() : tangentlist(0) {}
  ~Mesh() {}

  const char* getName()
  {
    return name;
  }

  void setName(const char* name)
  {
    this->name = new char[strlen(name)+1];
    strcpy(this->name,name);
  }

  void setVertices(VertexList* vlist)
  {
    vertexlist = vlist;
  }

  void setIndices(IndexList* ilist)
  {
    indexlist = ilist;
  }

  void setTangents(TangentList* tlist)
  {
    tangentlist = tlist;
  }

  void createTangents()
  {
    tangentlist->reserveTangents(vertexlist->getCount());

    for (unsigned int i = 0; i < vertexlist->getCount(); i++)
      tangentlist->add(0,0,0,0,0,0);

    for (size_t a = 0; a < indexlist->getCount(); a++)
    {
      const Triangle& triangle = indexlist->getTriangles()[a];
      
      const Vertex& v1 = vertexlist->getVertices()[triangle.v1];
      const Vertex& v2 = vertexlist->getVertices()[triangle.v2];
      const Vertex& v3 = vertexlist->getVertices()[triangle.v3];

      //Calculate normal tangent
      opentree::otVector3 s1(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
      opentree::otVector3 s2(v3.x - v2.x, v3.y - v2.y, v3.z - v2.z);
      opentree::otVector3 s3(v3.x - v1.x, v3.y - v1.y, v3.z - v1.z);

      opentree::otVector3 v1_normal = ((-s1) & s3).normalize();
      opentree::otVector3 v2_normal = (s1 & s2).normalize();
      opentree::otVector3 v3_normal = ((-s3) & (-s2)).normalize();

      float t1, t2, t3;

      //Calculate s tangent
      t1 = v1.v - v2.v;
      t2 = v3.v - v2.v;
      t3 = v3.v - v1.v;
      opentree::otVector3 v1_sTan =-(s1 * t3 - s3 * t1);
      opentree::otVector3 v2_sTan = (s1 * t2 - s2 * t1);
      opentree::otVector3 v3_sTan = (s3 * t2 - s2 * t3);
      v1_sTan.normalize();
      v2_sTan.normalize();
      v3_sTan.normalize();

      //Calculate t tangent
      t1 = v1.u - v2.u;
      t2 = v3.u - v2.u;
      t3 = v3.u - v1.u;
      opentree::otVector3 v1_tTan =-(s1 * t3 - s3 * t1);
      opentree::otVector3 v2_tTan = (s1 * t2 - s2 * t1);
      opentree::otVector3 v3_tTan = (s3 * t2 - s2 * t3);
      v1_tTan.normalize();
      v2_tTan.normalize();
      v3_tTan.normalize();

      if (((v1_sTan & v1_tTan) | v1_normal) < 0.0f)
      {
        v1_sTan = -v1_sTan;
        v1_tTan = -v1_tTan;
      }

      if (((v2_sTan & v2_tTan) | v2_normal) < 0.0f)
      {
        v2_sTan = -v2_sTan;
        v2_tTan = -v2_tTan;
      }

      if (((v3_sTan & v3_tTan) | v3_normal) < 0.0f)
      {
        v3_sTan = -v3_sTan;
        v3_tTan = -v3_tTan;
      }

      Tangent& tan1 = tangentlist->get(triangle.v1);
      Tangent& tan2 = tangentlist->get(triangle.v2);
      Tangent& tan3 = tangentlist->get(triangle.v3);

      tan1.nsx += v1_sTan.X;  tan1.nsy += v1_sTan.Y;  tan1.nsz += v1_sTan.Z;
      tan1.ntx += v1_tTan.X;  tan1.nty += v1_tTan.Y;  tan1.ntz += v1_tTan.Z;

      tan2.nsx += v2_sTan.X;  tan2.nsy += v2_sTan.Y;  tan2.nsz += v2_sTan.Z;
      tan2.ntx += v2_tTan.X;  tan2.nty += v2_tTan.Y;  tan2.ntz += v2_tTan.Z;

      tan3.nsx += v3_sTan.X;  tan3.nsy += v3_sTan.Y;  tan3.nsz += v3_sTan.Z;
      tan3.ntx += v3_tTan.X;  tan3.nty += v3_tTan.Y;  tan3.ntz += v3_tTan.Z;
    }

    for (size_t j = 0; j < vertexlist->getCount(); j++)
    {
      Tangent& tan = tangentlist->get(j);

      opentree::otVector3 sTan(tan.nsx, tan.nsy, tan.nsz);
      opentree::otVector3 tTan(tan.ntx, tan.nty, tan.ntz);

      sTan.normalize(); tTan.normalize();

      tan.nsx = sTan.X;  tan.nsy = sTan.Y;  tan.nsz = sTan.Z;
      tan.ntx = tTan.X;  tan.nty = tTan.Y;  tan.ntz = tTan.Z;
    }
  }

  void draw(Renderer* renderer, unsigned int& vertcnt, unsigned int& polycnt)
  {
    vertcnt += indexlist->getCount()*3;
    polycnt += indexlist->getCount();
    draw(renderer);
  }

  void draw(Renderer* renderer)
  {
    renderer->enableTransparency(hasTransparency);
    renderer->enableAlphaTest(hasAlphaTransparency);

    material->setToRenderer(renderer);
    material->enableShader(renderer);
    renderer->setupArray(vertexlist->getVertices(), (tangentlist)?tangentlist->getTangents():0);
    renderer->drawMesh(indexlist->getCount(), indexlist->getTriangles());
    material->disableShader();
  }

  void setMaterial(Material* mat)
  {
    material = mat;
  }
  
  Material* getMaterial()
  {
    return material;
  }
};

/// }@

#endif // _FOREST_MESH_H_
