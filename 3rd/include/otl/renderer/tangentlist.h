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

#ifndef _TANGENTLIST_H_
#define _TANGENTLIST_H_

#include <opentree/utils/otarray.h>
#include <opentree/utils/otvertices.h>
#include "tangents.h"

/// \addtogroup Renderer
/// @{

class TangentList
{
private:
  char* name;

  opentree::otArray<Tangent> tangentarray;

public:
  TangentList() : name(0) { }
  TangentList(const char* name) : name(0) { setName(name); }
  virtual ~TangentList() { delete [] name; }

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

  void addTangents(Tangent* tangents, unsigned int count)
  {
    tangentarray.add(tangents, count);
  }

  const Tangent* getTangents() const
  {
    return tangentarray.getRawArray();
  }

  Tangent& get(unsigned int idx)
  {
    return tangentarray[idx];
  }

  void reserveTangents(unsigned int count)
  {
    tangentarray.setGrowRate(count);
  }

  void add(float nsx, float nsy, float nsz, float ntx, float nty, float ntz)
  {
    Tangent tangent;
    
    tangent.nsx=nsx; tangent.nsy=nsy; tangent.nsz=nsz; 
    tangent.ntx=ntx; tangent.nty=nty; tangent.ntz=ntz; 

    tangentarray.add(tangent);
  }

  unsigned int getCount()
  {
    return tangentarray.getCount();
  }
};

/// }@

#endif // _TANGENTLIST_H_
