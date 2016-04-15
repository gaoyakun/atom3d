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

#ifndef _MESHERFIXEDEVEN_STEM_H_
#define _MESHERFIXEDEVEN_STEM_H_

#include "opentree/utils/otarray.h"
#include "opentree/utils/otvector3.h"

#include "cylmesher.h"
#include "mesher.h"

namespace opentree
{
  class otStem;
  class otVertices;
  class otTriangles;

  /// \addtogroup Mesher
  /// @{

  /** 
   * Creates a mesh for one stem.
   * Meshes stem with a fixed number of evenly distributed circles.
   */
  class MesherFixedEvenStem : public iMesher
  {
  private:
    otStem* stem;

    int vertexOffset;
    int circleRes, curveRes;

    //temporary variables
    otVector3 p,n;
    otVector3 c,z,pos;
    float angle, dSin, dCos;

  public:
    int levelNo;

  private:
    int countVertices();
    int countIndices();
    
    void calcVertices(otVertices* vertices);
    void calcIndices(otTriangles* triangles);
    
    void getCap(otTriangles* triangles, bool end);
    void getCap(otVertices* vertices, bool end);
    
    void getSide(int a, int b, otTriangles* triangles);
    void getCircle(int a, otVertices* vertices);
    
    int getOffset(int i);
    
  public:
    MesherFixedEvenStem();
    MesherFixedEvenStem(const MesherFixedEvenStem& s);
    MesherFixedEvenStem(otStem* s);

    virtual ~MesherFixedEvenStem();

    
    unsigned int getVerticesCount()
    {
      return countVertices();
    }
  
    unsigned int getIndicesCount()
    {
      return countIndices();
    }
    
    void getVertices(otVertices& vertices);
    void getIndices(otTriangles& triangles, unsigned int offset);
    
    void setCurveRes(const int curves);
    void setCircleRes(const int circles);

  };
}

#endif //_MESHERFIXEDEVEN_STEM_H_
