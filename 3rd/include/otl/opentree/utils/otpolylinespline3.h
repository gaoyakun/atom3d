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

#ifndef _OT_OTPOLYLINESPLINE3_H_
#define _OT_OTPOLYLINESPLINE3_H_

#include "spline.h"
#include "otvector3.h"
#include "otarray.h"
#include "serialiser.h"

namespace opentree
{
  /// \addtogroup Utilities
  /// @{

  /** Polyline 3d spline.
   * Consists of a set of points connected by linear segments.
   * Spline defined by #size points. If a point between those definition points
   * is needed, then it will be approximated.
   * Actually one could claim it's a 4d spline as we use a 4th value to order the
   * splines, this is required so the spline can have any desired form in the 3d
   * space.
   */
  class otPolyLineSpline3 : public iSpline3D, public iSerialiser
  {
  public:
    ///3d vector data structure
    class Point
    {
    public:
      ///index
      float x;
      ///3d vector
      otVector3 y;
    };

  private:
    ///Array of points
    otArray<Point> points;

  public:
    /** Creates an empty spline
     * \param size Defines how may definition points will be specified
     */
    otPolyLineSpline3(unsigned int size = 0)
    : points(size)
    {
    }

    /// Deletes all points.
    virtual ~otPolyLineSpline3()
    {
    }

    /** Returns the y for a given x
     * \param x required coordinate to the point on the spline.
     * \return y approximated coordinate for the specified point.
     */
    otVector3 getPoint(const float x);

    /** Returns the length of the spline curve
     * \return distance between the first and last point following the spline curve.
     */
    float getLength();

    /** Returns the slope at a given point on the spline
     * \param x required coordinate to the point on the spline.
     * \return y approximated slope vector (normal vector).
     */
    otVector3 getTangent(const float x);
    
    iSpline3D* getDerivative();

    iSpline3D* getSecondDerivative();
    
    iSpline3D* getNormal();

    iSpline1D* getCurvature();
    
    /** Nearest point
     * This function searches the position where the spline is the
     * nearest by a point
     * \param pos Point near the spline.
     * \return Point on the spline which is the closest to [pos]
     */
    otVector3 findNearest(const otVector3& pos);

    /** Adds a new definition point to the spline.
     * Points must be ordered and may not exceed #size!
     * \param x index.
     * \param y Approximated position vector.
     */
    void setPoint(const float x, const otVector3& y);
    
    bool operator==(otPolyLineSpline3& other);

    iSerialiser* getSerialiser()
    {
      return this;
    }

    unsigned int getDataLength()
    {
      // 1 byte point count
      // n Points == n * (1 float + 3 otVector) => n*4 float => n*16 byte
      return getPointList().getCount() * 16 + 1;
    }

    unsigned int writeData(char* data);

    const otArray<Point>& getPointList() const
    {
      return points;
    }
  };

  /// @}
}

#endif //_OT_OTPOLYLINESPLINE3_H_

