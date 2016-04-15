#ifndef __ATOM_GEOMETRY_INTERSECTION_H
#define __ATOM_GEOMETRY_INTERSECTION_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_math.h"
#include "aabb.h"
#include "plane.h"
#include "frustum3d.h"

enum {
  IRESULT_INSIDE,
  IRESULT_OUTSIDE,
  IRESULT_INTERSECTED,
  IRESULT_LYINGON,
  IRESULT_SURFACE,
  IRESULT_EMBEDDED,
  IRESULT_POINT,
  IRESULT_EDGE
};

#ifndef ATOM_INTERSECT_EPSILON
# define ATOM_INTERSECT_EPSILON 0.0000001
#endif // INTERSECT_EPSILON

#ifndef ATOM_IS_ZERO
# define ATOM_IS_ZERO(f) ((f) < (ATOM_INTERSECT_EPSILON) && (f) > -(ATOM_INTERSECT_EPSILON))
#endif // ATOM_IS_ZERO

#ifndef ATOM_POSITIVE
# define ATOM_POSITIVE(f) ((f) >= (ATOM_INTERSECT_EPSILON))
#endif // ATOM_POSITIVE

#ifndef ATOM_NEGATIVE
# define ATOM_NEGATIVE(f) ((f) <= -(ATOM_INTERSECT_EPSILON))
#endif // ATOM_NEGATIVE

/*************************************************************************************
 *
 *                               AABB vs AABB
 *
 *************************************************************************************/

inline int intersect_test_AABB_AABB ( const ATOM_Vector3f &center1, 
                                      const ATOM_Vector3f &extends1,
                                      const ATOM_Vector3f &center2, 
                                      const ATOM_Vector3f &extends2) {
  if (center1.x + extends1.x <= center2.x - extends2.x ||
      center1.x - extends1.x >= center2.x + extends2.x ||
      center1.y + extends1.y <= center2.y - extends2.y ||
      center1.y - extends1.y >= center2.y + extends2.y ||
      center1.z + extends1.z <= center2.z - extends2.z ||
      center1.z - extends1.z >= center2.z + extends2.z)
    return IRESULT_OUTSIDE;

  return IRESULT_INTERSECTED;
}

/*************************************************************************************
 *
 *                               AABB vs ATOM_Frustum3d
 *
 *************************************************************************************/

inline int intersect_test_AABB_Frustum3d (const ATOM_Vector3f &center, 
                                          const ATOM_Vector3f &extends,
                                          const ATOM_Frustum3d &frustum) 
  {
	  bool bad_intesect = false;
  for (int i = 0; i < 6; ++i) 
  {
    const Plane &p = frustum[i];	

    if (p.getA() * (center.x + p.getPX() * extends.x) +
        p.getB() * (center.y + p.getPY() * extends.y) +
        p.getC() * (center.z + p.getPZ() * extends.z) +
        p.getD() < 0)
      return IRESULT_OUTSIDE;

    if (p.getA() * (center.x + p.getNX() * extends.x) +
        p.getB() * (center.y + p.getNY() * extends.y) +
        p.getC() * (center.z + p.getNZ() * extends.z) +
        p.getD() < 0)
      bad_intesect  = true;
  }

    if( bad_intesect )
    {
	    return IRESULT_INTERSECTED;
    }

  return IRESULT_INSIDE;
}

inline int intersect_test_AABB_Frustum3d (const ATOM_Vector3f &center, 
                                          const ATOM_Vector3f &extends,
                                          const ATOM_Frustum3d &frustum,
                                          int in_mask,
                                          int * out_mask) {
  int i, k, result = IRESULT_INSIDE;
  *out_mask = 0;

  for (i = 0, k = 1; k <= in_mask; k += k, ++i) 
  {
    if (k & in_mask)
    {
      const Plane &p = frustum[i];

      if (p.getA() * (center.x + p.getPX() * extends.x) +
          p.getB() * (center.y + p.getPY() * extends.y) +
          p.getC() * (center.z + p.getPZ() * extends.z) +
          p.getD() < 0)
        return IRESULT_OUTSIDE;

      if (p.getA() * (center.x + p.getNX() * extends.x) +
          p.getB() * (center.y + p.getNY() * extends.y) +
          p.getC() * (center.z + p.getNZ() * extends.z) +
          p.getD() < 0)
      {
        *out_mask |= k;
        result = IRESULT_INTERSECTED;
      }
    }
  }

  return result;
}

/*************************************************************************************
 *
 *                               AABB vs ATOM_Ray
 *
 *************************************************************************************/

inline bool intersect_test_AABB_Line (const ATOM_Vector3f &center, 
                                      const ATOM_Vector3f &extends,
                                      const ATOM_Vector3f &origin,
                                      const ATOM_Vector3f &direction) {
  float AWdU[3], AWxDdU[3];
  ATOM_Vector3f WxD = crossProduct(direction, origin - center);

  AWdU[1] = ATOM_abs(direction.y);
  AWdU[2] = ATOM_abs(direction.z);
  AWxDdU[0] = ATOM_abs(WxD.x);
  if ( AWxDdU[0] > extends.y * AWdU[2] + extends.z * AWdU[1] )
    return false;

  AWdU[0] = ATOM_abs (direction.x);
  AWxDdU[1] = ATOM_abs(WxD.y);
  if ( AWxDdU[1] > extends.x * AWdU[2] + extends.z * AWdU[0] )
    return false;

  AWxDdU[2] = ATOM_abs(WxD.z);
  if ( AWxDdU[2] > extends.x * AWdU[1] + extends.y * AWdU[0] )
    return false;

  return true;
}

inline bool intersect_test_AABB_Ray ( const ATOM_Vector3f &center, 
                                      const ATOM_Vector3f &extends,
                                      const ATOM_Vector3f &origin,
                                      const ATOM_Vector3f &direction) {
  float WdU[3], AWdU[3], DdU[3], ADdU[3], AWxDdU[3];

  ATOM_Vector3f diff = origin - center;

  WdU[0] = direction.x;
  AWdU[0] = ATOM_abs(WdU[0]);
  DdU[0] = diff.x;
  ADdU[0] = ATOM_abs(DdU[0]);
  if ( ADdU[0] > extends.x && DdU[0] * WdU[0] >= 0.f )
    return false;

  WdU[1] = direction.y;
  AWdU[1] = ATOM_abs(WdU[1]);
  DdU[1] = diff.y;
  ADdU[1] = ATOM_abs(DdU[1]);
  if ( ADdU[1] > extends.y && DdU[1] * WdU[1] >= 0.f )
    return false;

  WdU[2] = direction.z;
  AWdU[2] = ATOM_abs(WdU[2]);
  DdU[2] = diff.z;
  ADdU[2] = ATOM_abs(DdU[2]);
  if ( ADdU[2] > extends.z && DdU[2] * WdU[2] >= 0.f )
    return false;

  ATOM_Vector3f WxD = crossProduct(direction, diff);

  AWxDdU[0] = ATOM_abs(WxD.x);
  if ( AWxDdU[0] > extends.y * AWdU[2] + extends.z * AWdU[1] )
    return false;

  AWxDdU[1] = ATOM_abs(WxD.y);
  if ( AWxDdU[1] > extends.x * AWdU[2] + extends.z * AWdU[0] )
    return false;

  AWxDdU[2] = ATOM_abs(WxD.z);
  if ( AWxDdU[2] > extends.x * AWdU[1] + extends.y * AWdU[0] )
    return false;

  return true;
}

inline bool intersect_test_AABB_Segment ( const ATOM_Vector3f &center, 
                                          const ATOM_Vector3f &extends,
                                          const ATOM_Vector3f &origin,
                                          const ATOM_Vector3f &direction) {
  float AWdU[3], ADdU[3], AWxDdU[3], Rhs;
  ATOM_Vector3f sdir = 0.5f * direction;
  ATOM_Vector3f scen = origin + sdir;

  ATOM_Vector3f diff = scen - center;

  AWdU[0] = ATOM_abs(sdir.x);
  ADdU[0] = ATOM_abs(diff.x);
  Rhs = extends.x + AWdU[0];
  if ( ADdU[0] > Rhs )
    return false;

  AWdU[1] = ATOM_abs(sdir.y);
  ADdU[1] = ATOM_abs(diff.y);
  Rhs = extends.y + AWdU[1];
  if ( ADdU[1] > Rhs )
    return false;

  AWdU[2] = ATOM_abs(sdir.z);
  ADdU[2] = ATOM_abs(diff.z);
  Rhs = extends.z + AWdU[2];
  if ( ADdU[2] > Rhs )
    return false;

  ATOM_Vector3f WxD = crossProduct(sdir, diff);

  AWxDdU[0] = ATOM_abs(WxD.x);
  if ( AWxDdU[0] > extends.y * AWdU[2] + extends.z * AWdU[1] )
    return false;

  AWxDdU[1] = ATOM_abs(WxD.y);
  if ( AWxDdU[1] > extends.x * AWdU[2] + extends.z * AWdU[0] )
    return false;

  AWxDdU[2] = ATOM_abs(WxD.z);
  if ( AWxDdU[2] > extends.x * AWdU[1] + extends.y * AWdU[0] )
    return false;

  return true;
}

namespace internal
{
  inline bool _clip (float fDenom, float fNumer, float& rfT0, float& rfT1)
  {
    if (fDenom > 0.f)
    {
      if ( fNumer > fDenom * rfT1 )
        return false;
      if ( fNumer > fDenom * rfT0 )
        rfT0 = fNumer/fDenom;
      return true;
    }
    else if ( fDenom < 0.f )
    {
      if ( fNumer > fDenom * rfT0 )
        return false;
      if ( fNumer > fDenom * rfT1 )
        rfT1 = fNumer/fDenom;
      return true;
    }
    else
    {
      return fNumer <= 0.f;
    }
  }

  inline bool _find_intersection (const ATOM_Vector3f& origin,
                                  const ATOM_Vector3f& direction, 
                                  const ATOM_Vector3f& extends, 
                                  float& rfT0,
                                  float& rfT1)
  {
      float SaveT0 = rfT0, SaveT1 = rfT1;

      bool notEntirelyClipped =
          _clip (+direction.x, -origin.x - extends.x, rfT0, rfT1) &&
          _clip (-direction.x, +origin.x - extends.x, rfT0, rfT1) &&
          _clip (+direction.y, -origin.y - extends.y, rfT0, rfT1) &&
          _clip (-direction.y, +origin.y - extends.y, rfT0, rfT1) &&
          _clip (+direction.z, -origin.z - extends.z, rfT0, rfT1) &&
          _clip (-direction.z, +origin.z - extends.z, rfT0, rfT1);

      return notEntirelyClipped && ( rfT0 != SaveT0 || rfT1 != SaveT1 );
  }
} // namespace internal

inline int intersection_find_AABB_Segment ( const ATOM_Vector3f &center, 
                                            const ATOM_Vector3f &extends,
                                            const ATOM_Vector3f &origin,
                                            const ATOM_Vector3f &direction,
                                            float distance[2]) {
  float T0 = 0.f;
  float T1 = 1.f;

  if (internal::_find_intersection (origin - center, direction, extends, T0, T1))
  {
    if ( T0 > 0.f )
    {
      if ( T1 < 1.f )
      {
        distance[0] = T0;
        distance[1] = T1;
        return 2;
      }
      else
      {
        distance[0] = T0;
        return 1;
      }
    }
    else
    {
      if ( T1 < 1.f )
      {
        distance[0] = T1;
        return 1;
      }
      else
      {
        return 0;
      }
    }
  }

  return 0;
}

inline int intersection_find_AABB_Ray ( const ATOM_Vector3f &center, 
                                        const ATOM_Vector3f &extends,
                                        const ATOM_Vector3f &origin,
                                        const ATOM_Vector3f &direction,
                                        float distance[2]) {
  float T0 = 0.f;
  float T1 = ATOM_DataLimit<float>::max_value ();

  if (internal::_find_intersection (origin - center, direction, extends, T0, T1))
  {
    if ( T0 > 0.f )
    {
      distance[0] = T0;
      distance[1] = T1;
      return 2;
    }
    else
    {
      distance[0] = T1;
      return 1;
    }
  }

  return 0;
}

inline int intersection_find_AABB_Line (const ATOM_Vector3f &center, 
                                        const ATOM_Vector3f &extends,
                                        const ATOM_Vector3f &origin,
                                        const ATOM_Vector3f &direction,
                                        float distance[2]) {
  float T0 = -ATOM_DataLimit<float>::max_value ();
  float T1 = ATOM_DataLimit<float>::max_value ();

  if (internal::_find_intersection (origin - center, direction, extends, T0, T1))
  {
    if ( T0 != T1 )
    {
      distance[0] = T0;
      distance[1] = T1;
      return 2;
    }
    else
    {
      distance[0] = T0;
      return 1;
    }
  }

  return 0;
}

/*************************************************************************************
 *
 *                               Plane vs ATOM_Ray
 *
 *************************************************************************************/

inline int intersect_test_Plane_Line (float plnA, float plnB, float plnC, float plnD,
                                      const ATOM_Vector3f &origin,
                                      const ATOM_Vector3f &n_direction) {
  float f = plnA * n_direction.x + plnB * n_direction.y + plnC * n_direction.z;
  if (ATOM_equal (f, 0.f))
  {
    float d = plnA * origin.x + plnB * origin.y + plnC * origin.z + plnD;
    if (ATOM_equal(d, 0.f))
      return IRESULT_LYINGON;
    else
      return IRESULT_OUTSIDE;
  }
  return IRESULT_INTERSECTED;
}

inline int intersect_test_Plane_Ray ( float plnA, float plnB, float plnC, float plnD,
                                      const ATOM_Vector3f &origin,
                                      const ATOM_Vector3f &n_direction) {
  float f = plnA * n_direction.x + plnB * n_direction.y + plnC * n_direction.z;
  float d = plnA * origin.x + plnB * origin.y + plnC * origin.z + plnD;

  if (ATOM_equal (f, 0.f))
  {
    if (ATOM_equal (d, 0.f))
      return IRESULT_LYINGON;
    else
      return IRESULT_OUTSIDE;
  }

  if (ATOM_less(f, 0.f))
    return ATOM_greater(d, 0.f) ? IRESULT_INTERSECTED : IRESULT_OUTSIDE;
  else
    return ATOM_less (d, 0.f) ? IRESULT_INTERSECTED : IRESULT_OUTSIDE;
}

inline int intersect_test_Plane_Segment ( float plnA, float plnB, float plnC, float plnD,
                                          const ATOM_Vector3f &origin,
                                          const ATOM_Vector3f &direction) {
  ATOM_Vector3f endpoint = origin + direction;
  float f0 = plnA * origin.x + plnB * origin.y + plnC * origin.z + plnD;
  float f1 = plnA * endpoint.x + plnB * endpoint.y + plnC * endpoint.z + plnD;

  if (ATOM_equal(f0, 0.f))
  {
    if (ATOM_equal (f1, 0.f))
      return IRESULT_LYINGON;
    else
      return IRESULT_INTERSECTED;
  }

  if (ATOM_greater(f0, 0.f))
    return ATOM_greater(f1, 0.f) ? IRESULT_OUTSIDE : IRESULT_INTERSECTED;
  else
    return ATOM_less(f1, 0.f) ? IRESULT_OUTSIDE : IRESULT_INTERSECTED;
}

inline bool intersection_find_Plane_Line (float plnA, float plnB, float plnC, float plnD,
                                          const ATOM_Vector3f &origin,
                                          const ATOM_Vector3f &n_direction,
                                          float & distance) {
  float f0 = plnA * origin.x + plnB * origin.y + plnC * origin.z + plnD;

  if (ATOM_equal (f0, 0.f))
  {
    distance = 0.f;
    return true;
  }

  float f1 = plnA * n_direction.x + plnB * n_direction.y + plnC * n_direction.z;

  if (ATOM_equal (f1, 0.f))
    return false;

  distance = -f0 / f1;
  return true;
}

inline bool intersection_find_Plane_Ray ( float plnA, float plnB, float plnC, float plnD,
                                          const ATOM_Vector3f &origin,
                                          const ATOM_Vector3f &n_direction,
                                          float & distance) {
  float f0 = plnA * origin.x + plnB * origin.y + plnC * origin.z + plnD;

  if (ATOM_equal (f0, 0.f))
  {
    distance = 0.f;
    return true;
  }

  float f1 = plnA * n_direction.x + plnB * n_direction.y + plnC * n_direction.z;

  if (ATOM_equal (f1, 0.f))
    return false;

  if ((ATOM_greater (f0, 0.f) && ATOM_less (f1, 0.f)) || (ATOM_less(f0, 0.f) && ATOM_greater (f1, 0.f)))
  {
    distance = -f0 / f1;
    return true;
  }

  return false;
}

inline bool intersection_find_Plane_Segment ( float plnA, float plnB, float plnC, float plnD,
                                              const ATOM_Vector3f &origin,
                                              const ATOM_Vector3f &direction,
                                              float & distance) {
  float f0 = plnA * origin.x + plnB * origin.y + plnC * origin.z + plnD;

  if (ATOM_equal (f0, 0.f))
  {
    distance = 0.f;
    return true;
  }

  ATOM_Vector3f endpoint = origin + direction;
  float f1 = plnA * endpoint.x + plnB * endpoint.y + plnC * endpoint.z + plnD;

  if ((ATOM_greater(f0, 0.f) && ATOM_greater (f1, 0.f)) || (ATOM_less (f0, 0.f) && ATOM_less (f1, 0.f)))
    return false;

  distance = direction.getLength() * f0 / (f0 - f1);
  return true;
}

/*************************************************************************************
 *
 *                               Half-Space vs ATOM_Ray
 *
 *************************************************************************************/

inline int intersect_test_HSP_Line (float plnA, float plnB, float plnC, float plnD,
                                    const ATOM_Vector3f &origin,
                                    const ATOM_Vector3f &n_direction) {
  float f = plnA * n_direction.x + plnB * n_direction.y + plnC * n_direction.z;
  if (ATOM_equal (f, 0.f))
  {
    float d = plnA * origin.x + plnB * origin.y + plnC * origin.z + plnD;
    if (ATOM_equal (d, 0.f))
      return IRESULT_LYINGON;
    else if (ATOM_greater(d, 0.f))
      return IRESULT_INSIDE;
    else // if (ATOM_NEGATIVE (d))
      return IRESULT_OUTSIDE;
  }
  return IRESULT_INTERSECTED;
}

inline int intersect_test_HSP_Ray ( float plnA, float plnB, float plnC, float plnD,
                                    const ATOM_Vector3f &origin,
                                    const ATOM_Vector3f &n_direction) {
  float f = plnA * n_direction.x + plnB * n_direction.y + plnC * n_direction.z;
  float d = plnA * origin.x + plnB * origin.y + plnC * origin.z + plnD;

  if (ATOM_equal (f, 0.f))
  {
    if (ATOM_equal (d, 0.f))
      return IRESULT_LYINGON;
    else if (ATOM_greater(d, 0.f))
      return IRESULT_INSIDE;
    else // if (ATOM_NEGATIVE(d))
      return IRESULT_OUTSIDE;
  }
  else if (ATOM_less (f, 0.f))
    return ATOM_greater(d, 0.f) ? IRESULT_INTERSECTED : IRESULT_OUTSIDE;
  else
    return ATOM_less(d, 0.f) ? IRESULT_INTERSECTED : IRESULT_INSIDE;
}

inline int intersect_test_HSP_Segment ( float plnA, float plnB, float plnC, float plnD,
                                        const ATOM_Vector3f &origin,
                                        const ATOM_Vector3f &direction) {
  ATOM_Vector3f endpoint = origin + direction;
  float f0 = plnA * origin.x + plnB * origin.y + plnC * origin.z + plnD;
  float f1 = plnA * endpoint.x + plnB * endpoint.y + plnC * endpoint.z + plnD;

  if (ATOM_equal (f0, 0.f))
  {
    if (ATOM_equal (f1, 0.f))
      return IRESULT_LYINGON;
    else if (ATOM_greater (f1, 0.f))
      return IRESULT_INSIDE;
    else // if (ATOM_NEGATIVE(f1))
      return IRESULT_OUTSIDE;
  }
  else if (ATOM_greater(f0, 0.f))
    return ATOM_greater(f1, 0.f) ? IRESULT_INSIDE : IRESULT_INTERSECTED;
  else // if (ATOM_NEGATIVE(f0))
    return ATOM_less(f1, 0.f) ? IRESULT_OUTSIDE : IRESULT_INTERSECTED;
}

#endif // __ATOM_GEOMETRY_INTERSECTION_H
