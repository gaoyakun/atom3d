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

#ifndef _OT_LEAF_H_
#define _OT_LEAF_H_

#include "opentree/opentree.h"
#include "opentree/utils/otmatrix.h"

namespace opentree
{
  /// \addtogroup Tree_Structure
  /// @{

  /** Leaf class
   * Defines one Leaf.
   */
  class otLeaf 
  {
  public:
    ///Position and Rotation of this leaf.
    otMatrix transform;
    /// Length and Width of this leaf.
    float length, width;

  public:
    /// Default ctor for block allocation
    otLeaf() {}

    /// Creates a new leaf
    otLeaf(const otMatrix& t, float length, float width)
    : transform(t), length(length), width(width)
    {
    }

    /// Copy ctor
    otLeaf(const otLeaf& l)
    : transform(l.transform), length(l.length), width(l.width) 
    {
    }

    /// Destructs a leaf
    ~otLeaf() {}
  };

  /// @}

};

#endif //_WEBER_LEAF_H_
