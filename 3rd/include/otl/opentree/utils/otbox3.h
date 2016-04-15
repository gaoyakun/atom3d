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

#ifndef _OT_OTBOX3_H_
#define _OT_OTBOX3_H_

#include "otvector3.h"

namespace opentree
{
  /// \addtogroup Utilities
  /// @{

  /** 3D box
   * The box is specified through its maximum and minimum vector.
   */
  class otBox3
  {
  private:
    /// Position of the edge with the smallest value.
    otVector3 min_v;
    /// Position of the edge with the biggest value.
    otVector3 max_v;
  public:
    otBox3() : min_v(0,0,0), max_v(0,0,0) {}
    ~otBox3() {}

    /** Translates the center of the box to the specified position.
     * \param center position of the new center
     */
    void setCenter(otVector3 center)
    {
      otVector3 old_center = (max_v+min_v)/2;
      max_v += center - old_center;
      min_v += center - old_center;
    }

    /** Check if the position is inside the box.
     * \param pos Vector which is compared against the box
     * \return true if within the box
     */
    bool isInBox(otVector3 pos) const
    {
      return ( min_v.X < pos.X && pos.X < max_v.X &&
               min_v.Y < pos.Y && pos.Y < max_v.Y &&
               min_v.Z < pos.Z && pos.Z < max_v.Z );
    }

    /// Returns the dimension of the box.
    otVector3 getSize() const
    {
      return max_v-min_v;
    }

    /// Returns the center of the box.
    otVector3 getCenter() const
    {
      return (max_v+min_v)/2;
    }

    /// Extends the box so the pos fits inside the box.
    void extend(otVector3 pos)
    {
      if (min_v.X > pos.X) min_v.X = pos.X;
      if (pos.X > max_v.X) max_v.X = pos.X;
      if (min_v.Y > pos.Y) min_v.Y = pos.Y;
      if (pos.Y > max_v.Y) max_v.Y = pos.Y;
      if (min_v.Z > pos.Z) min_v.Z = pos.Z;
      if (pos.Z > max_v.Z) max_v.Z = pos.Z;
    }
  };

  /// @}
}

#endif //_OT_OTBOX3_H_
