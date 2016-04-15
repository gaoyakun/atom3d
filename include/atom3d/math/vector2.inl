inline ATOM_Vector2f::ATOM_Vector2f (void)
{
}

inline ATOM_Vector2f::ATOM_Vector2f (float fillval):
  x(fillval),
  y(fillval)
{
}

inline ATOM_Vector2f::ATOM_Vector2f (float x_, float y_):
  x(x_),
  y(y_)
{
}

inline void ATOM_Vector2f::set (float x_, float y_)
{
  x = x_;
  y = y_;
}

inline bool ATOM_Vector2f::almostEqual (const ATOM_Vector2f &other) const
{
  return ATOM_equal (x, other.x) && ATOM_equal (y, other.y);
}

inline float ATOM_Vector2f::getLength (void) const
{
  return ATOM_sqrt (x * x + y * y);
}

inline float ATOM_Vector2f::getSquaredLength (void) const
{
  return x * x + y * y;
}

inline void ATOM_Vector2f::normalize (void)
{
  float s = 1.f / ATOM_sqrt (x * x + y * y);
  x *= s;
  y *= s;
}

inline void ATOM_Vector2f::normalizeFrom (const ATOM_Vector2f &v)
{
  float s = 1.f / ATOM_sqrt (v.x * v.x + v.y * v.y);
  x = v.x * s;
  y = v.y * s;
}

inline void ATOM_Vector2f::normalize_lp (void)
{
  float s = ATOM_invsqrt (x * x + y * y);
  x *= s;
  y *= s;
}

inline void ATOM_Vector2f::normalizeFrom_lp (const ATOM_Vector2f &v)
{
  float s = ATOM_invsqrt (v.x * v.x + v.y * v.y);
  x = v.x * s;
  y = v.y * s;
}

inline ATOM_Vector2f operator + (const ATOM_Vector2f &v1, const ATOM_Vector2f &v2)
{
  return ATOM_Vector2f (v1.x + v2.x, v1.y + v2.y);
}

inline ATOM_Vector2f operator - (const ATOM_Vector2f &v1, const ATOM_Vector2f &v2)
{
  return ATOM_Vector2f (v1.x - v2.x, v1.y - v2.y);
}

inline ATOM_Vector2f operator * (const ATOM_Vector2f &v1, const ATOM_Vector2f &v2)
{
  return ATOM_Vector2f (v1.x * v2.x, v1.y * v2.y);
}

inline ATOM_Vector2f operator * (const ATOM_Vector2f &v, float f)
{
  return ATOM_Vector2f (v.x * f, v.y * f);
}

inline ATOM_Vector2f operator * (float f, const ATOM_Vector2f &v)
{
  return ATOM_Vector2f (v.x * f, v.y * f);
}

inline ATOM_Vector2f operator / (const ATOM_Vector2f &v1, const ATOM_Vector2f &v2)
{
  return ATOM_Vector2f (v1.x / v2.x, v1.y / v2.y);
}

inline ATOM_Vector2f operator / (const ATOM_Vector2f &v, float f)
{
  return ATOM_Vector2f (v.x / f, v.y / f);
}

inline bool operator == (const ATOM_Vector2f &v1, const ATOM_Vector2f &v2)
{
  return v1.x == v2.x && v1.y == v2.y;
}

inline bool operator != (const ATOM_Vector2f &v1, const ATOM_Vector2f &v2)
{
  return ! operator == (v1, v2);
}

inline float dotProduct (const ATOM_Vector2f &v1, const ATOM_Vector2f &v2)
{
  return v1.x * v2.x + v1.y * v2.y;
}

inline ATOM_Vector2f ATOM_Vector2f::operator - (void) const
{
  return ATOM_Vector2f (-x, -y);
}

inline ATOM_Vector2f & ATOM_Vector2f::operator += (const ATOM_Vector2f &v)
{
  x += v.x;
  y += v.y;
  return *this;
}

inline ATOM_Vector2f & ATOM_Vector2f::operator -= (const ATOM_Vector2f &v)
{
  x -= v.x;
  y -= v.y;
  return *this;
}

inline ATOM_Vector2f & ATOM_Vector2f::operator *= (const ATOM_Vector2f &v)
{
  x *= v.x;
  y *= v.y;
  return *this;
}

inline ATOM_Vector2f & ATOM_Vector2f::operator *= (float f)
{
  x *= f;
  y *= f;
  return *this;
}

inline ATOM_Vector2f & ATOM_Vector2f::operator /= (const ATOM_Vector2f &v)
{
  x /= v.x;
  y /= v.y;
  return *this;
}

inline ATOM_Vector2f & ATOM_Vector2f::operator /= (float f)
{
  x /= f;
  y /= f;
  return *this;
}

inline float ATOM_Vector2f::operator [] (int i) const
{
	return xy[i];
}

inline float & ATOM_Vector2f::operator [] (int i)
{
	return xy[i];
}

