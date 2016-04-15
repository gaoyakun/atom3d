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

#ifndef _INDEXLIST_H_
#define _INDEXLIST_H_

#include <opentree/utils/otarray.h>
#include <opentree/utils/ottriangles.h>
#include "triangles.h"

/// \addtogroup Renderer
/// @{

class IndexList : public opentree::otTriangles
{
private:
  char* name;

  opentree::otArray<Triangle> indexarray;

public:
  IndexList() : name(0) { }
  IndexList(const char* name) : name(0) { setName(name); }
  virtual ~IndexList() { delete [] name; }

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

  const Triangle* getTriangles() const
  {
    return indexarray.getRawArray();
  }

  void reserveTriangles(unsigned int count)
  {
    indexarray.setGrowRate(count);
  }

  void addTriangle(int v1, int v2, int v3)
  {
    Triangle triangle;
    
    triangle.v1=v1; triangle.v2=v2; triangle.v3=v3; 

    indexarray.add(triangle);
  }

  unsigned int getCount()
  {
    return indexarray.getCount();
  }
};

/// }@

#endif // _INDEXLIST_H_
