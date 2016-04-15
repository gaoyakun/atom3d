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

#ifndef _OT_OTPOLYLINESPLINE_H_
#define _OT_OTPOLYLINESPLINE_H_

#include "spline.h"
#include "otvector3.h"
#include "otarray.h"
#include "serialiser.h"

namespace opentree
{
  /// \addtogroup Utilities
  /// @{

  /** Polyline spline
   * Consists of a set of points connected by line segments.
   * Spline defined by #size points. If a point between those definition points
   * is needed, then it will be approximated.
   */
  class otPolyLineSpline : public iSpline1D, public iSerialiser
  {
  public:
    ///data structure
    struct Point
    {
      ///Index
      float x;
      ///Value
      float y;
    };

  private:
    ///Array of Points
    otArray<Point> points;

  public:
    /** Creates an empty spline
     * \param size Defines how may definition points will be specified
     */
    otPolyLineSpline(unsigned int size = 0)
    : points(size)
    {
    }
    /// Deletes all points.
    virtual ~otPolyLineSpline()
    {
    }

    /** Returns the y for a given x
     * \param x required coordinate to the point on the spline.
     * \return y approximated coordinate for the specified point.
     */
    float getPoint(const float x);

    /** Adds a new definition point to the spline.
     * Points must be ordered and may not exceed #size!
     * \param x required coordinate to get the y value later.
     * \param y this is the value which will be looked up later.
     */
    void setPoint(const float x, const float y);

    bool operator==(otPolyLineSpline& other);

    const otArray<Point>& getPointList() const
    {
      return points;
    }

    iSerialiser* getSerialiser()
    {
      return this;
    }

    unsigned int getDataLength()
    {
      // 1 byte point count
      // n Points == n * (2 float) => n*2 float => n*8 byte
      return getPointList().getCount() * 8 + 1;
    }

    unsigned int writeData(char* data);

  };
  
  /// @}
}

#endif //_OT_OTPOLYLINESPLINE_H_
