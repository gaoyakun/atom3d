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

#include "opentree/utils/otpolylinespline.h"

namespace opentree
{
  /// \addtogroup Utilities
  /// @{

  float otPolyLineSpline::getPoint(const float x)
  {
    for (unsigned int i=0; i<points.getCount(); i++)
    {
      // in case we're lucky enough to seatch a point that is a definition point,
      // all the math can be skipped.
      if (points[i].x == x) return points[i].y;
      if (points[i].x > x)
      {
        if (i == points.getCount() || i == 0) // if first or last definition point
        {
          return points[i].y;
        }
        else //approximate point between two definition points.
        {
          float dist_given_points = (x - points[i-1].x) / (points[i].x - points[i-1].x);
          return (1-dist_given_points) * points[i-1].y + dist_given_points * points[i].y;
        }
      }
    }
    otAssert("otPolyLineSpline::getPos: Position out of Range!");
    return -1;
  }

  void otPolyLineSpline::setPoint(const float x, const float y)
  {
    #ifdef _DEBUG
      // Enforcing sorted adding of points.
      for (unsigned int i=0; i<points.getCount(); i++)
      {
        otCondAssert(points[i].x > x, "otPolyLineSpline::setPoint unsorted adding of point!");
      }

      // Check if values are valid numbers.
      if (!(y > 0) && !(y <= 0) )
      {
        otAssert("otPolyLineSpline::setPoint: Bogus y value!");
      }
    #endif

    Point point;
    point.x = x;
    point.y = y;
    points.add(point);
  }

  bool otPolyLineSpline::operator==(otPolyLineSpline& other)
  {
    if (points.getCount() != other.points.getCount()) 
      return false;

    for (unsigned int i = 0; i < points.getCount(); i++)
    {
      if (!(points[i].x == other.points[i].x))
        return false;
      if (!(points[i].y == other.points[i].y))
        return false;
    }
    return true;
  }
  
  unsigned int otPolyLineSpline::writeData(char* data)
  {
    unsigned sum = 1;
    //let's hope we never have more than 256 points :)
    data[0] = getPointList().getCount();

    for (unsigned int i=0; i<getPointList().getCount(); i++)
    {
      otPolyLineSpline::Point p = getPointList().get(i);
      *(float*)(data+sum+0) = p.x;
      *(float*)(data+sum+4) = p.y; 
      sum += 8;
    }

    return sum;
  }

  /// @}
}

