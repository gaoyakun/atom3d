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

#include "opentree/utils/otpolylinespline3.h"
#include "opentree/utils/otpolylinespline.h"

namespace opentree
{
  /// \addtogroup Utilities
  /// @{

  otVector3 otPolyLineSpline3::getPoint(const float x)
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
          return points[i-1].y * (1-dist_given_points) + points[i].y * dist_given_points;
        }
      }
    }
    //otAssert("otSpline::getPos: Position out of Range!");
    //I have to return something, that should never be called anyway :)
    return points[0].y;
  }

  float otPolyLineSpline3::getLength()
  {
    if (points.getCount() < 2)
      return 0;
    float length = 0;
    for (unsigned int i=0; i<points.getCount()-1; i++)
    {
      length += (points[i+1].y-points[i].y).length();
    }
    return length;
  }

  otVector3 otPolyLineSpline3::getTangent(const float x)
  {
    if (points[0].x <= x && points[1].x > x)
    {
      return (points[1].y-points[0].y).normalize();
    }
    if (points[points.getCount()-2].x <= x && points[points.getCount()-1].x >= x)
    {
      return (points[points.getCount()-2].y-points[points.getCount()-1].y).normalize();
    }
    for (unsigned int i=0; i<points.getCount()-2; i++)
    {
      if (points[i].x <= x && points[i+1].x > x)
      {
        return ((points[i+1].y-points[i].y).normalize() + (points[i].y-points[i-1].y).normalize()).normalize();
      }
    }

    otAssert("otSpline::getPos: Position out of Range!");
    //I have to return something, this should never be called.
    return points[0].y;
  }
  
  iSpline3D* otPolyLineSpline3::getDerivative()
  {
    unsigned int count = getPointList().getCount();
    otPolyLineSpline3* tangent=new otPolyLineSpline3(count);

    for(unsigned int i=0;i<count-1;i++)
    {
      tangent->setPoint(points[i].x,getTangent(points[i].x));
    }
    return tangent;
  }
   
  iSpline3D* otPolyLineSpline3::getSecondDerivative()
  {
    unsigned int count = getPointList().getCount();
     
    otPolyLineSpline3* secondDerivative=new otPolyLineSpline3(count);
      
    float localResolution = ((points[2].y-points[1].y).length() <  (points[1].y-points[0].y).length()) ? (points[2].y-points[1].y).length() : (points[1].y-points[0].y).length() ;
    secondDerivative->setPoint(points[0].x,((points[2].y-points[1].y).normalize() - (points[1].y-points[0].y).normalize())/localResolution);
     
    for(unsigned int i=1;i<count-2;i++)
    {
      float localResolution = ((points[i+1].y-points[i].y).length() <  (points[i].y-points[i-1].y).length()) ? (points[i+1].y-points[i].y).length() : (points[i].y-points[i-1].y).length() ;
      secondDerivative->setPoint(points[i].x,((points[i+1].y-points[i].y).normalize() - (points[i].y-points[i-1].y).normalize())/localResolution);
    }
    secondDerivative->setPoint(points[count-1].x,points[count-2].y);
    return secondDerivative;
  }

  iSpline3D* otPolyLineSpline3::getNormal()
  {
    unsigned int count = getPointList().getCount();
    
    otPolyLineSpline3* normalSpline = new otPolyLineSpline3(count);
    otPolyLineSpline3* derivativeSpline = (otPolyLineSpline3 *)getDerivative();
    otPolyLineSpline3* secondDerivativeSpline = (otPolyLineSpline3 *)getSecondDerivative();
    
    normalSpline->setPoint(points[0].x,*(new otVector3));
    for(unsigned int i=1;i<count-1;i++)
    {
      otVector3 derivative=derivativeSpline->getPointList()[i].y;
      otVector3 secondDerivative=secondDerivativeSpline->getPointList()[i].y;
      normalSpline->setPoint(points[i].x,(secondDerivative -derivative*((derivative|secondDerivative))).normalize() );
    }
    return normalSpline;
  }

  iSpline1D* otPolyLineSpline3::getCurvature()
  {
    unsigned int count = getPointList().getCount();
    
    otPolyLineSpline* curvatureSpline = new otPolyLineSpline(count);
    
    otPolyLineSpline3* secondDerivativeSpline = (otPolyLineSpline3 *)getSecondDerivative();
    otPolyLineSpline3* normalSpline = (otPolyLineSpline3 *)getNormal();
    
    curvatureSpline->setPoint(points[0].x,0);
    for(unsigned int i=1;i<count-1;i++)
    {
      otVector3 normal=normalSpline->getPointList()[i].y;
      otVector3 secondDerivative=secondDerivativeSpline->getPointList()[i].y;
      curvatureSpline->setPoint(points[i].x,(normal|secondDerivative));
    }
    return curvatureSpline;
  }

  otVector3 otPolyLineSpline3::findNearest(const otVector3& pos)
  {
    float distance = FLT_MAX;
    unsigned int nearest_i = 0;
    for (unsigned int i=0; i<points.getCount()-1; i++)
    {
      float dist1 = (points[i].y-pos).length();
      float dist2 = (points[i+1].y-pos).length();

      if (distance > dist1+dist2)
      {
        distance = dist1+dist2;
        nearest_i = i;
      }
    }

    /*  Vector Math to get the nearest point.
     *
     *        I+1
     *        / \
     *  near>/   \
     *      /     \
     *     I ---- pos
     * 
     *  Nearest point = V(I->POS) dot V(I->I+1) * ( V(I->POS) / len(V(I->POS)) )
     */

    otVector3 I_to_I1 = (points[nearest_i+1].y-points[nearest_i].y).normalize();
    otVector3 I_to_pos = pos-points[nearest_i].y;

    float dist = I_to_pos | I_to_I1;
    return points[nearest_i].y + I_to_I1 * dist;
  }

  void otPolyLineSpline3::setPoint(const float x, const otVector3& y)
  {
    #ifdef _DEBUG
      // Enforcing sorted adding of points.
      for (unsigned int i=0; i<points.getCount(); i++)
      {
        otCondAssert(points[i].x > x, "otSpline::setPoint unsorted adding of point!");
      }
      // Let's start at the beginning
      otCondAssert(points.getCount() == 0 && x > 0, "otSpline::setPoint first point should be 0!");

    #endif

    Point point;
    point.x = x;
    point.y = y;
    points.add(point);
  }

  bool otPolyLineSpline3::operator==(otPolyLineSpline3& other)
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

  unsigned int otPolyLineSpline3::writeData(char* data)
  {
    unsigned sum = 1;

    //let's hope we never have more than 256 points :)
    data[0] = getPointList().getCount();

    for (unsigned int i=0; i<getPointList().getCount(); i++)
    {
      Point p = getPointList().get(i);
      *(float*)(data+sum+ 0) = p.x;
      *(float*)(data+sum+ 4) = p.y.X; 
      *(float*)(data+sum+ 8) = p.y.Y; 
      *(float*)(data+sum+12) = p.y.Z;
      sum += 16;
    }

    return sum;
  }

  /// @}
}

