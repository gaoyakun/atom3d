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

#ifndef _OT_MESHER_H_
#define _OT_MESHER_H_

/** \addtogroup Mesher
 *  Collection of Mesher which create the a mesh from its given input.
 */

/// @{

namespace opentree
{
  class otVertices;
  class otTriangles;
  
  class iMesher
  {
  public:
    virtual ~iMesher() {}
    virtual unsigned int getVerticesCount() = 0;
    virtual void getVertices(otVertices& vertices) = 0;
    virtual unsigned int getIndicesCount() = 0;
    virtual void getIndices(otTriangles& triangles, unsigned int offset) = 0;
  };
};

/// @}

#endif // _OT_MESHER_H_
