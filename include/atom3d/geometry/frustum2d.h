#ifndef __ATOM_GEOMETRY_FRUSTUM2D_H
#define __ATOM_GEOMETRY_FRUSTUM2D_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_math.h"

class ATOM_Frustum2d
  {
//        ATOM_STATIC_ASSERT(IsFloat<float>::result);
    typedef ATOM_Vector2f Vector2;
  public:
    inline ATOM_Frustum2d (float originx = 0, float originy = 0, float centerx = 0, float centery = 1, float fov = ATOM_HalfPi);
    inline void Init (float originx, float originy, float centerx, float centery, float fov);
    inline bool IntersectCircle (float ccenterx, float ccentery, float radius) const;
    inline bool IntersectRect (float xmin, float ymin, float xmax, float ymax) const;
  private:
    float _M_fov;
    Vector2 _M_origin;
    Vector2 _M_center;
    Vector2 _M_normal_left;
    Vector2 _M_normal_right;
    Vector2 _M_normal_front;
    Vector2 _M_normal_back;
  };
// implements
  //The 2d point rotate equation
  //Rotate 2d point (x,y) by delta to 2d point (x1, y1)
  //            |cos(delta) -sin(delta)|
  //[x1, y1] =  |                      | * [x, y]
  //            |sin(delta)  cos(delta)|


inline ATOM_Frustum2d::ATOM_Frustum2d (float originx, float originy, float centerx, float centery, float fov) {
  Init (originx, originy, centerx, centery, fov);
}


inline void ATOM_Frustum2d::Init (float originx, float originy, float centerx, float centery, float fov) {
  _M_origin.x = originx;
  _M_origin.y = originy;
  _M_center.x = centerx;
  _M_center.y = centery;
  _M_fov = fov;

  float a = ATOM_HalfPi - fov * 0.5f;
  float s, c;
  ATOM_sincos (a, &s, &c);

  // Calculating back normal is easy
  _M_normal_back = _M_center - _M_origin;
  _M_normal_back.normalize ();

  // Front normal is negative of back normal
  _M_normal_front = -_M_normal_back;

  // Calculating left normal by rotating back normal -(Pi/2 - fov/2)
  _M_normal_left.x = c * _M_normal_back.x + s * _M_normal_back.y;
  _M_normal_left.y = -s * _M_normal_back.x + c * _M_normal_back.y;

  // Calculating right normal by rotating back normal (Pi/2 - fov/2)
  _M_normal_right.x = c * _M_normal_back.x - s * _M_normal_back.y;
  _M_normal_right.y = s * _M_normal_back.x + c * _M_normal_back.y;
}

inline bool ATOM_Frustum2d::IntersectCircle (float ccenterx, float ccentery, float radius) const {
  assert (radius > 0.f);

  // Test against back line
  Vector2 l = Vector2(ccenterx, ccentery) - _M_origin;
  if (l.x * _M_normal_back.x + l.y * _M_normal_back.y <= -radius)
    return false;

  // Test against left line
  if (l.x * _M_normal_left.x + l.y * _M_normal_left.y <= -radius)
    return false;

  // Test against right line
  if (l.x * _M_normal_right.x + l.y * _M_normal_right.y <= -radius)
    return false;

  l = Vector2(ccenterx, ccentery) - _M_center;
  // Test against front line
  if (l.x * _M_normal_front.x + l.y * _M_normal_front.y <= -radius)
    return false;

  return true;
}

inline bool ATOM_Frustum2d::IntersectRect (float xmin, float ymin, float xmax, float ymax) const {
  Vector2 l[4];

  // Test against back line
  l[0] = Vector2(xmin, ymin) - _M_origin;
  l[1] = Vector2(xmin, ymax) - _M_origin;
  l[2] = Vector2(xmax, ymin) - _M_origin;
  l[3] = Vector2(xmax, ymax) - _M_origin;
  if (l[0].x * _M_normal_back.x + l[0].y * _M_normal_back.y <= 0 &&
      l[1].x * _M_normal_back.x + l[1].y * _M_normal_back.y <= 0 &&
      l[2].x * _M_normal_back.x + l[2].y * _M_normal_back.y <= 0 &&
      l[3].x * _M_normal_back.x + l[3].y * _M_normal_back.y <= 0)
    return false;

  // Test against left line
  if (l[0].x * _M_normal_left.x + l[0].y * _M_normal_left.y <= 0 &&
      l[1].x * _M_normal_left.x + l[1].y * _M_normal_left.y <= 0 &&
      l[2].x * _M_normal_left.x + l[2].y * _M_normal_left.y <= 0 &&
      l[3].x * _M_normal_left.x + l[3].y * _M_normal_left.y <= 0)
    return false;

  // Test against right line
  if (l[0].x * _M_normal_right.x + l[0].y * _M_normal_right.y <= 0 &&
      l[1].x * _M_normal_right.x + l[1].y * _M_normal_right.y <= 0 &&
      l[2].x * _M_normal_right.x + l[2].y * _M_normal_right.y <= 0 &&
      l[3].x * _M_normal_right.x + l[3].y * _M_normal_right.y <= 0)
    return false;

  // Test against front line
  l[0] = Vector2(xmin, ymin) - _M_center;
  l[1] = Vector2(xmin, ymax) - _M_center;
  l[2] = Vector2(xmax, ymin) - _M_center;
  l[3] = Vector2(xmax, ymax) - _M_center;
  if (l[0].x * _M_normal_front.x + l[0].y * _M_normal_front.y <= 0 &&
      l[1].x * _M_normal_front.x + l[1].y * _M_normal_front.y <= 0 &&
      l[2].x * _M_normal_front.x + l[2].y * _M_normal_front.y <= 0 &&
      l[3].x * _M_normal_front.x + l[3].y * _M_normal_front.y <= 0)
    return false;

  return true;
}

#endif // __ATOM_GEOMETRY_FRUSTUM2D_H
