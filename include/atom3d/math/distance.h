#ifndef __ATOM_MATH_DISTANCE_H
#define __ATOM_MATH_DISTANCE_H

#include "vector3.h"

enum 
{
  ATOM_DISTANCE_PRESULT_INSIDE,
  ATOM_DISTANCE_PRESULT_OUTSIDE,
  ATOM_DISTANCE_PRESULT_EDGE
};

/*************************************************************************************
 *
 *                               Plane to Point
 *
 *************************************************************************************/

inline float distance_Plane_Point ( float pA, float pB, float pC, float pD, const ATOM_Vector3f &point) {
  return pA * point.x + pB * point.y + pC * point.z + pD;
}

/*************************************************************************************
 *
 *                               Point to Point
 *
 *************************************************************************************/

inline float distance_Point_Point ( const ATOM_Vector3f &p1, const ATOM_Vector3f &p2) {
  return (p1 - p2).getLength();
}

/*************************************************************************************
 *
 *                               ATOM_Ray to Point
 *
 *************************************************************************************/

inline int nearest_point_Point_Segment (const ATOM_Vector3f &point,
                                        const ATOM_Vector3f &origin,
                                        const ATOM_Vector3f &n_direction,
                                        float length,
                                        float &percent) {
  percent = dotProduct (point - origin, n_direction);
  if (percent < 0.f)
  {
    percent = 0.f;
    return ATOM_DISTANCE_PRESULT_EDGE;
  }
  else if (percent > length)
  {
    percent = length;
    return ATOM_DISTANCE_PRESULT_EDGE;
  }

  return ATOM_DISTANCE_PRESULT_INSIDE;
}

inline int nearest_point_Point_Line (const ATOM_Vector3f &point,
                                     const ATOM_Vector3f &origin,
                                     const ATOM_Vector3f &n_direction,
                                     ATOM_Vector3f &out) {
  float distance = dotProduct (point - origin, n_direction);
  out = origin + n_direction * distance;
  return ATOM_DISTANCE_PRESULT_INSIDE;
}

#endif // __ATOM_MATH_DISTANCE_H


