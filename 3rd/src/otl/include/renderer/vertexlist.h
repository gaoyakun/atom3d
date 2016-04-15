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

#ifndef _VERTEXLIST_H_
#define _VERTEXLIST_H_

#include <opentree/utils/otarray.h>
#include <opentree/utils/otvertices.h>
#include "vertices.h"

/// \addtogroup Renderer
/// @{

class VertexList : public opentree::otVertices
{
private:
  char* name;

  opentree::otArray<Vertex> vertexarray;

public:
  VertexList() : name(0) { }
  VertexList(const char* name) : name(0) { setName(name); }
  virtual ~VertexList() { delete [] name; }

  const char* getName()
  {
    return name;
  }

  void setName(const char* name)
  {
    delete [] this->name;
    this->name = new char[strlen(name)+1];
    strcpy(this->name,name);
  }

  void addVertices(Vertex* vertices, unsigned int count)
  {
    vertexarray.add(vertices, count);
  }

  const Vertex* getVertices() const
  {
    return vertexarray.getRawArray();
  }

  void reserveVertices(unsigned int count)
  {
    vertexarray.setGrowRate(count);
  }

  void add(int index, float x, float y, float z, float nx, float ny, float nz, 
    float r, float g, float b, float a, float u, float v)
  {
    Vertex vertex;
    
    vertex.x=x; vertex.y=y; vertex.z=z; 
    vertex.nx=nx; vertex.ny=ny; vertex.nz=nz; 
    vertex.r=r; vertex.g=g; vertex.b=b; 
    vertex.u=u; vertex.v=v;

    vertexarray.add(vertex);
  }

  unsigned int getCount()
  {
    return vertexarray.getCount();
  }
};

/// }@

#endif // _VERTEXLIST_H_
