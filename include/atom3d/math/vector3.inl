inline ATOM_Vector3f::ATOM_Vector3f (void)
{
}

inline ATOM_Vector3f::ATOM_Vector3f (float fillval):
  x(fillval),
  y(fillval),
  z(fillval)
{
}

inline ATOM_Vector3f::ATOM_Vector3f (float x_, float y_, float z_):
  x(x_),
  y(y_),
  z(z_)
{
}

inline void ATOM_Vector3f::set (float x_, float y_, float z_)
{
  x = x_;
  y = y_;
  z = z_;
}

inline bool ATOM_Vector3f::almostEqual (const ATOM_Vector3f &other) const
{
  return ATOM_equal (x, other.x) && ATOM_equal (y, other.y) && ATOM_equal (z, other.z);
}

inline float ATOM_Vector3f::getLength (void) const
{
  return ATOM_sqrt (x * x + y * y + z * z);
}

inline float ATOM_Vector3f::getSquaredLength (void) const
{
  return x * x + y * y + z * z;
}

inline void ATOM_Vector3f::normalize (void)
{
  float s = 1.f / ATOM_sqrt (x * x + y * y + z * z);
  x *= s;
  y *= s;
  z *= s;
}

inline void ATOM_Vector3f::normalizeFrom (const ATOM_Vector3f &v)
{
  float s = 1.f / ATOM_sqrt (v.x * v.x + v.y * v.y + v.z * v.z);
  x = v.x * s;
  y = v.y * s;
  z = v.z * s;
}

inline void ATOM_Vector3f::normalize_lp (void)
{
  float s = ATOM_invsqrt (x * x + y * y + z * z);
  x *= s;
  y *= s;
  z *= s;
}

inline void ATOM_Vector3f::normalizeFrom_lp (const ATOM_Vector3f &v)
{
  float s = ATOM_invsqrt (v.x * v.x + v.y * v.y + v.z * v.z);
  x = v.x * s;
  y = v.y * s;
  z = v.z * s;
}

inline ATOM_Vector3f operator + (const ATOM_Vector3f &v1, const ATOM_Vector3f &v2)
{
  return ATOM_Vector3f (v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

inline ATOM_Vector3f operator - (const ATOM_Vector3f &v1, const ATOM_Vector3f &v2)
{
  return ATOM_Vector3f (v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

inline ATOM_Vector3f operator * (const ATOM_Vector3f &v1, const ATOM_Vector3f &v2)
{
  return ATOM_Vector3f (v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}

inline ATOM_Vector3f operator * (const ATOM_Vector3f &v, float f)
{
  return ATOM_Vector3f (v.x * f, v.y * f, v.z * f);
}

inline ATOM_Vector3f operator * (float f, const ATOM_Vector3f &v)
{
  return ATOM_Vector3f (v.x * f, v.y * f, v.z * f);
}

inline ATOM_Vector3f operator / (const ATOM_Vector3f &v1, const ATOM_Vector3f &v2)
{
  return ATOM_Vector3f (v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
}

inline ATOM_Vector3f operator / (const ATOM_Vector3f &v, float f)
{
  return ATOM_Vector3f (v.x / f, v.y / f, v.z / f);
}

inline bool operator == (const ATOM_Vector3f &v1, const ATOM_Vector3f &v2)
{
  return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z;
}

inline bool operator != (const ATOM_Vector3f &v1, const ATOM_Vector3f &v2)
{
  return ! operator == (v1, v2);
}

inline float dotProduct (const ATOM_Vector3f &v1, const ATOM_Vector3f &v2)
{
  return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

inline ATOM_Vector3f crossProduct (const ATOM_Vector3f &v1, const ATOM_Vector3f &v2)
{
  return ATOM_Vector3f (v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
}

inline ATOM_Vector3f ATOM_Vector3f::operator - (void) const
{
  return ATOM_Vector3f (-x, -y, -z);
}

inline ATOM_Vector3f & ATOM_Vector3f::operator += (const ATOM_Vector3f &v)
{
  x += v.x;
  y += v.y;
  z += v.z;
  return *this;
}

inline ATOM_Vector3f & ATOM_Vector3f::operator -= (const ATOM_Vector3f &v)
{
  x -= v.x;
  y -= v.y;
  z -= v.z;
  return *this;
}

inline ATOM_Vector3f & ATOM_Vector3f::operator *= (const ATOM_Vector3f &v)
{
  x *= v.x;
  y *= v.y;
  z *= v.z;
  return *this;
}

inline ATOM_Vector3f & ATOM_Vector3f::operator *= (float f)
{
  x *= f;
  y *= f;
  z *= f;
  return *this;
}

inline ATOM_Vector3f & ATOM_Vector3f::operator /= (const ATOM_Vector3f &v)
{
  x /= v.x;
  y /= v.y;
  z /= v.z;
  return *this;
}

inline ATOM_Vector3f & ATOM_Vector3f::operator /= (float f)
{
  x /= f;
  y /= f;
  z /= f;
  return *this;
}

inline float ATOM_Vector3f::operator [] (int i) const
{
	return xyz[i];
}

inline float & ATOM_Vector3f::operator [] (int i)
{
	return xyz[i];
}
