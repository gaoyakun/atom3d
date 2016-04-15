#ifndef __ATOM_GEOMETRY_TRIANGLE_H
#define __ATOM_GEOMETRY_TRIANGLE_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_math.h"
#include "plane.h"
#include "intersection.h"
#include "distance.h"

class Triangle
  {
    typedef ATOM_Vector3f Vec3;

  public:
    inline Triangle ();
    inline Triangle (const Vec3 &v0, const Vec3 &v1, const Vec3 &v2);
    inline Vec3 & vertex (int index);
    inline const Vec3 & vertex (int index) const;

  public:
    inline Vec3     getCenterOfMass () const;
    inline ATOM_Plane getPlane () const;
    inline float        getArea () const;
    inline bool     pointInside (const Vec3 &point) const;
    inline int      getNearestPointOnPerimeterToPoint (const Vec3 &point, Vec3 &nearest, int &p1, int &p2) const;
    inline int      intersectMovingUnitSphere (const Vec3 &origin, const Vec3 &n_direction, float &distance, ATOM_Plane &collisionPlane) const;

  private:
    Vec3 _M_points[3];
  };

// inline member functions
inline Triangle::Triangle () {
}

inline Triangle::Triangle (const Vec3 &v0, const Vec3 &v1, const Vec3 &v2) {
  _M_points[0] = v0;
  _M_points[1] = v1;
  _M_points[2] = v2;
}

inline Triangle::Vec3 & Triangle::vertex (int index) {
  return _M_points[index];
}

inline const Triangle::Vec3 & Triangle::vertex (int index) const {
  return _M_points[index];
}

inline Triangle::Vec3 Triangle::getCenterOfMass () const {
  return (_M_points[0] + _M_points[1] + _M_points[2]) / static_cast<float>(3);
}

inline ATOM_Plane Triangle::getPlane () const {
  ATOM_Plane result;
  Vec3 normal = crossProduct (_M_points[1] - _M_points[0], _M_points[2] - _M_points[0]);
  normal.normalize();
  result.setA (normal.x);
  result.setB (normal.y);
  result.setC (normal.z);
  result.setD (- dotProduct (normal, _M_points[0]));
  return result;
}

inline float Triangle::getArea () const {
  return crossProduct(_M_points[1] - _M_points[0], _M_points[2] - _M_points[0]).getLength() * static_cast<float>(0.5);
}

inline bool Triangle::pointInside (const Vec3 &point) const {
  int pos = 0, neg = 0;
  Vec3 center = getCenterOfMass ();
  Vec3 normal = crossProduct (_M_points[1] - _M_points[0], _M_points[2] - _M_points[0]);
  normal.normalize();
  for (int i = 0; i < 3; ++i)
  {
    int j = (i + 1) % 3;
    Vec3 n = crossProduct (_M_points[j] - _M_points[i], normal);
    float halfPlane = dotProduct (point, n) - dotProduct (_M_points[i], n);

    if (ATOM_greater(halfPlane, 0.f))
      pos++;
    else if (ATOM_less (halfPlane, 0.f))
      neg++;
  }
  return !pos || !neg;
}

inline int Triangle::getNearestPointOnPerimeterToPoint (const Vec3 &point, Vec3 &nearest, int &p1, int &p2) const {
	  bool found = false;
	  float nearest_dist= static_cast<float>(0);
	  int result;
  float percent;
  Vec3 p;
	  int first, second;

	  for (int i = 0; i < 3; ++i)
	  {
		  int j = (i + 1) % 3;
    Vec3 n_direction = _M_points[j] - _M_points[i];
    float length = n_direction.getLength();
    n_direction /= length;

    int edgeFlag = nearest_point_Point_Segment (point, _M_points[i], n_direction, length, percent);
    p = edgeFlag == PRESULT_INSIDE ? _M_points[i] + n_direction * percent : ((percent < 0) ? _M_points[i] : _M_points[j]);
		  float dist = distance_Point_Point(p, point);

		  if (!found || dist < nearest_dist)
		  {
			  nearest_dist = dist;
			  nearest = p;
			  result = edgeFlag;
			  first = i;
			  second = j;
			  found = true;
		  }
	  }

	  if (result == PRESULT_EDGE)
  {
		  for (int i = 0; i < 3; ++i)
		  {
			  if (nearest == _M_points[i])
			  {
				  p1 = i - 1;
				  p2 = i + 1;

				  if (p1 < 0) 
          p1 = 2;

				  if (p2 > 2) 
          p2 = 0;

				  break;
			  }
		  }
  }
	  else if (result == PRESULT_INSIDE)
	  {
		  p1 = first;
		  p2 = second;
	  }

	  return result;
}

namespace internal
{
  inline bool _sphereIntersection( const ATOM_Vector3f &center, 
                            const ATOM_Vector3f &origin,
                            const ATOM_Vector3f &n_direction,
                            float &time) {
    ATOM_Vector3f q = center - origin;
    float c = q.getLength();
    float v = dotProduct (q, n_direction);
    float d = static_cast<float>(1) - (c * c - v * v);

    if (d < static_cast<float>(0))
    {
      time = static_cast<float>(0);
      return false;
    }

    time = v - static_cast<float>(ATOM_sqrt (d));
    return true;
  }
} // namespace internal

inline int Triangle::intersectMovingUnitSphere (const Vec3 &origin, const Vec3 &n_direction, float &distance, ATOM_Plane &collisionPlane) const {
  ATOM_Plane plane = getPlane ();
  float distOriginToTriPlane = distance_Plane_Point (plane.getA(), plane.getB(), plane.getC(), plane.getD(), origin);

  // Back face culling
  //if (distOriginToTriPlane < static_cast<float>(0))
  //  return IRESULT_OUTSIDE;

  Vec3 planeNormal(plane.getA(), plane.getB(), plane.getC());
  Vec3 sphereIntersectPoint = origin - planeNormal;

  if (!intersection_find_Plane_Line (plane.getA(), plane.getB(), plane.getC(), plane.getD(), sphereIntersectPoint, n_direction, distance))
    return IRESULT_OUTSIDE;

  Vec3 innerMostPoint;
  bool embedded = true;

  // Embedded test
  if (ATOM_greater(distOriginToTriPlane - static_cast<float>(1), 0.f))
  {
    embedded = false;
  }
  else
  {
    Vec3 v = origin - planeNormal * distOriginToTriPlane;

    // Test if point inside.
    int pos = 0, neg = 0;
    Vec3 center = getCenterOfMass ();
    for (int i = 0; i < 3; ++i)
    {
      int j = (i + 1) % 3;
      Vec3 n = crossProduct (_M_points[j] - _M_points[i], planeNormal);
      float halfPlane = dotProduct (v, n) - dotProduct (_M_points[i], n);

      if (ATOM_greater(halfPlane, 0.f))
        pos++;
      else if (ATOM_less (halfPlane, 0.f))
        neg++;
    }

    if (pos && neg)
    {
      // point not inside
      int e0, e1;
      getNearestPointOnPerimeterToPoint (v, innerMostPoint, e0, e1);

      if (ATOM_greater((innerMostPoint - origin).getLength() - static_cast<float>(1), 0.f))
        embedded = false;
    }

    if (embedded)
      innerMostPoint = v;
  }

  if (embedded)
  {
    distance = distance_Point_Point (origin, innerMostPoint) - static_cast<float>(1);
    collisionPlane.set (innerMostPoint, origin - innerMostPoint);
    return IRESULT_EMBEDDED;
  }
  else
  {
    Vec3 planeIntersectPoint = sphereIntersectPoint + n_direction * distance;

    if (pointInside (planeIntersectPoint))
    {
      if (ATOM_less (distance * static_cast<float>(0.5), 0.f))
        return IRESULT_OUTSIDE;
    
      collisionPlane = plane;
      return IRESULT_SURFACE;
    }
    else
    {
      int e0, e1;
      Vec3 v;
      int edgeFlag = getNearestPointOnPerimeterToPoint (planeIntersectPoint, v, e0, e1);

      if (edgeFlag == PRESULT_EDGE)
      {
        if (!internal::_sphereIntersection (origin, v, -n_direction, distance))
          return IRESULT_OUTSIDE;

        sphereIntersectPoint = v - n_direction * distance;
        collisionPlane.set (v, origin - sphereIntersectPoint);
        Vec3 atSphere = origin - v;

        if (dotProduct (n_direction, atSphere) > 0)
          return IRESULT_OUTSIDE;

        if (collisionPlane.getA() * plane.getA() + collisionPlane.getB() * plane.getB() +
            collisionPlane.getC() * plane.getC() < 0)
          return IRESULT_OUTSIDE;

        if (distance_Plane_Point (collisionPlane.getA(), collisionPlane.getB(), collisionPlane.getC(),
                                  collisionPlane.getD(), _M_points[e0]) > 0)
          return IRESULT_OUTSIDE;

        if (distance_Plane_Point (collisionPlane.getA(), collisionPlane.getB(), collisionPlane.getC(),
                                  collisionPlane.getD(), _M_points[e1]) > 0)
          return IRESULT_OUTSIDE;

        return IRESULT_POINT;
      }
      else
      {
        Vec3 edgeNormal = _M_points[e1] - _M_points[e0];
        edgeNormal.normalize ();
        ATOM_Plane bisectionPlane;
        bisectionPlane.set(_M_points[e1], crossProduct(n_direction, edgeNormal));
        Vec3 discCenter = bisectionPlane.getNearestPointToPoint (origin);

        if (ATOM_greater(distance_Point_Point (origin, discCenter) - static_cast<float>(1), 0.f))
          return IRESULT_OUTSIDE;

        Vec3 toSphereNormal = _M_points[e1] - _M_points[e0], edgePoint;
        toSphereNormal.normalize();
        nearest_point_Point_Line (discCenter, _M_points[e0], toSphereNormal, edgePoint);
        toSphereNormal = discCenter - edgePoint;
        toSphereNormal.normalize ();

        float t;
        if (!internal::_sphereIntersection (origin, edgePoint, toSphereNormal, t))
          return IRESULT_OUTSIDE;

        sphereIntersectPoint = edgePoint + toSphereNormal * t;
        Vec3 edgeVector = crossProduct (crossProduct (_M_points[e1] - _M_points[e0], n_direction), _M_points[e1] - _M_points[e0]);
        edgeVector.normalize ();
        ATOM_Plane edgePlane;
        edgePlane.set (_M_points[e0], edgeVector);

        intersection_find_Plane_Line (edgePlane.getA(), edgePlane.getB(), edgePlane.getC(), edgePlane.getD(),
                                      sphereIntersectPoint, n_direction, t);

        Vec3 vt = sphereIntersectPoint + n_direction * t;
        if (dotProduct ((_M_points[e0] - vt), (_M_points[e1] - vt)) > 0)
          return IRESULT_OUTSIDE;

        vt = origin - sphereIntersectPoint;
        vt.normalize ();
        collisionPlane.set (v, vt);
        if (distance_Plane_Point (collisionPlane.getA(), collisionPlane.getB(), collisionPlane.getC(),
                                  collisionPlane.getD(), getCenterOfMass()) > 0)
          return IRESULT_OUTSIDE;

        if (collisionPlane.getA() * plane.getA() + collisionPlane.getB() * plane.getB() + collisionPlane.getD() * plane.getD() < 0)
          return IRESULT_OUTSIDE;

        intersection_find_Plane_Line (edgePlane.getA(), edgePlane.getB(), edgePlane.getC(), edgePlane.getD(),
                                      sphereIntersectPoint, n_direction, distance);

        if (distance < static_cast<float>(-1))
          return IRESULT_OUTSIDE;

        return IRESULT_EDGE;
      }
    }
  }
}

#endif // __ATOM_GEOMETRY_TRIANGLE_H


