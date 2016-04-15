/*
  Copyright (c) 2005 Pascal Kirchdorfer, Seth Yastrov

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

#ifndef __SPLINE_H_
#define __SPLINE_H_

#include "otvector3.h"
#include "serialiser.h"

namespace opentree
{

  /// Interface for 1 dimensional splines
  class iSpline1D
  {
  public:
    virtual ~iSpline1D() {}

    /// Returns the point along the spline at i
    virtual float getPoint(const float i) = 0;
    
    // Returns the tangent of the point i
    // virtual float getTangent(float i) = 0;

    virtual iSerialiser* getSerialiser() = 0;
  };

  /// Interface for 3 dimensional splines
  class iSpline3D
  {
  public:
    virtual ~iSpline3D() {}

    /// Returns the point along the spline at i
    virtual otVector3 getPoint(const float i) = 0;
    
    /// Returns a tangent of the point i pointing in the direction of the spline
    virtual otVector3 getTangent(const float i) = 0;
    
    /// Returns the derivative of the spline
    virtual iSpline3D* getDerivative() = 0;

    /// This function searches the position where the spline is the nearest by a point.
    virtual otVector3 findNearest(const otVector3& pos) = 0;

    /// Returns the length of the spline curve.
    virtual float getLength() = 0;

    /// Returns the serialiser class for the implemented spline
    virtual iSerialiser* getSerialiser() = 0;
  };

};

#endif // __SPLINE_H_
