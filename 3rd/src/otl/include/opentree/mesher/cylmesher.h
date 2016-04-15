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

#include "opentree/opentree.h"
#include "opentree/utils/otarray.h"
#include "opentree/utils/otvector3.h"

#ifndef _MESHER_CYL_H_
#define _MESHER_CYL_H_

namespace opentree
{
  class otStem;
  class otVertices;
  class otTriangles;

  /// \addtogroup Mesher
  /// @{

  /** 
   * Creates a mesh for one stem.
   */
  class MesherCyl
  {
  private:

  public:
    MesherCyl();
    virtual ~MesherCyl();
    
    void getSide(otTriangles& triangles, int segments, int offset_a, int offset_b);
    void getCircle(otVertices& vertices, int segments, float radius, otMatrix& trf, otVector3& pos, float v);
    
    void getCap(otTriangles* triangles, int segments, int offset, bool end);
    void getCap(otVertices* vertices, bool end, int segments, otVector3& pos, otMatrix& t, float radius);
    
  };
}

#endif //_MESHER_CYL_H_

