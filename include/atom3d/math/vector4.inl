inline ATOM_Vector4f::ATOM_Vector4f (void)
{
}

inline ATOM_Vector4f::ATOM_Vector4f (float fillval):
  x(fillval),
  y(fillval),
  z(fillval),
  w(fillval)
{
}

inline ATOM_Vector4f::ATOM_Vector4f (float x_, float y_, float z_, float w_):
  x(x_),
  y(y_),
  z(z_),
  w(w_)
{
}

inline ATOM_Vector4f::ATOM_Vector4f (const ATOM_Vector3f &v):
  x(v.x),
  y(v.y),
  z(v.z),
  w(1.f)
{
}

inline ATOM_Vector4f::ATOM_Vector4f (const ATOM_Vector4f &v):
  x(v.x),
  y(v.y),
  z(v.z),
  w(v.w)
{
}

inline ATOM_Vector4f & ATOM_Vector4f::operator = (const ATOM_Vector4f &v)
{
  x = v.x;
  y = v.y;
  z = v.z;
  w = v.w;
  return *this;
}

inline void ATOM_Vector4f::set (float x_, float y_, float z_, float w_)
{
  x = x_;
  y = y_;
  z = z_;
  w = w_;
}

inline void ATOM_Vector4f::homogeneous (void)
{
  if (!ATOM_equal (w, 0.f))
  {
    x /= w;
    y /= w;
    z /= w;
    w = 1.f;
  }
}

inline bool ATOM_Vector4f::isHomogeneous (void) const
{
  return ATOM_equal (w, 1.f);
}

inline bool ATOM_Vector4f::almostEqual (const ATOM_Vector4f &other) const
{
  return ATOM_equal (x, other.x) && ATOM_equal (y, other.y) && ATOM_equal (z, other.z) && ATOM_equal (w, other.w);
}

inline float ATOM_Vector4f::getLength (void) const
{
  return ATOM_sqrt (x * x + y * y + z * z + w * w);
}

inline float ATOM_Vector4f::getSquaredLength (void) const
{
  return x * x + y * y + z * z + w * w;
}

inline float ATOM_Vector4f::getLength3 (void) const
{
  return ATOM_sqrt (x * x + y * y + z * z);
}

inline float ATOM_Vector4f::getSquaredLength3 (void) const
{
  return x * x + y * y + z * z;
}

inline void ATOM_Vector4f::normalize (void)
{
  float s = 1.f / ATOM_sqrt (x * x + y * y + z * z + w * w);
  x *= s;
  y *= s;
  z *= s;
  w *= s;
}

inline void ATOM_Vector4f::normalizeFrom (const ATOM_Vector4f &v)
{
  float s = 1.f / ATOM_sqrt (v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
  x = v.x * s;
  y = v.y * s;
  z = v.z * s;
  w = v.w * s;
}

inline void ATOM_Vector4f::normalize3 (void)
{
  float s = 1.f / ATOM_sqrt (x * x + y * y + z * z);
  x *= s;
  y *= s;
  z *= s;
}

inline void ATOM_Vector4f::normalize3From (const ATOM_Vector4f &v)
{
  float s = 1.f / ATOM_sqrt (v.x * v.x + v.y * v.y + v.z * v.z);
  x = v.x * s;
  y = v.y * s;
  z = v.z * s;
}

inline void ATOM_Vector4f::normalize3From (const ATOM_Vector3f &v)
{
  float s = 1.f / ATOM_sqrt (v.x * v.x + v.y * v.y + v.z * v.z);
  x = v.x * s;
  y = v.y * s;
  z = v.z * s;
}

inline void ATOM_Vector4f::normalize_lp (void)
{
  float s = ATOM_invsqrt(x * x + y * y + z * z + w * w);
  x *= s;
  y *= s;
  z *= s;
  w *= s;
}

inline void ATOM_Vector4f::normalizeFrom_lp (const ATOM_Vector4f &v)
{
  float s = ATOM_invsqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
  x = v.x * s;
  y = v.y * s;
  z = v.z * s;
  w = v.w * s;
}

inline void ATOM_Vector4f::normalize3_lp (void)
{
  float s = ATOM_invsqrt (x * x + y * y + z * z);
  x *= s;
  y *= s;
  z *= s;
}

inline void ATOM_Vector4f::normalize3From_lp (const ATOM_Vector4f &v)
{
  float s = ATOM_invsqrt (v.x * v.x + v.y * v.y + v.z * v.z);
  x = v.x * s;
  y = v.y * s;
  z = v.z * s;
}

inline void ATOM_Vector4f::normalize3From_lp (const ATOM_Vector3f &v)
{
  float s = ATOM_invsqrt (v.x * v.x + v.y * v.y + v.z * v.z);
  x = v.x * s;
  y = v.y * s;
  z = v.z * s;
}

inline void ATOM_Vector4f::setVector3 (const ATOM_Vector3f &v)
{
	x = v.x;
	y = v.y;
	z = v.z;
}

inline ATOM_Vector3f ATOM_Vector4f::getVector3 (void) const
{
  return ATOM_Vector3f(x, y, z);
}

inline ATOM_Vector4f operator + (const ATOM_Vector4f &v1, const ATOM_Vector4f &v2)
{
  return ATOM_Vector4f(v1.x+v2.x, v1.y+v2.y, v1.z+v2.z, v1.w+v2.w);
}

inline ATOM_Vector4f operator - (const ATOM_Vector4f &v1, const ATOM_Vector4f &v2)
{
  return ATOM_Vector4f(v1.x-v2.x, v1.y-v2.y, v1.z-v2.z, v1.w-v2.w);
}

inline ATOM_Vector4f operator * (const ATOM_Vector4f &v1, const ATOM_Vector4f &v2)
{
  return ATOM_Vector4f(v1.x*v2.x, v1.y*v2.y, v1.z*v2.z, v1.w*v2.w);
}

inline ATOM_Vector4f operator * (const ATOM_Vector4f &v, float f)
{
  return ATOM_Vector4f(v.x*f, v.y*f, v.z*f, v.w*f);
}

inline ATOM_Vector4f operator * (float f, const ATOM_Vector4f &v)
{
  return operator * (v, f);
}

inline ATOM_Vector4f operator / (const ATOM_Vector4f &v1, const ATOM_Vector4f &v2)
{
  return ATOM_Vector4f(v1.x/v2.x, v1.y/v2.y, v1.z/v2.z, v1.w/v2.w);
}

inline ATOM_Vector4f operator / (const ATOM_Vector4f &v, float f)
{
  return ATOM_Vector4f(v.x/f, v.y/f, v.z/f, v.w/f);
}

inline bool operator == (const ATOM_Vector4f &v1, const ATOM_Vector4f &v2)
{
  return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z && v1.w == v2.w;
}

inline bool operator != (const ATOM_Vector4f &v1, const ATOM_Vector4f &v2)
{
  return ! operator == (v1, v2);
}

inline float dotProduct (const ATOM_Vector4f &v1, const ATOM_Vector4f &v2)
{
  return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
}

inline float dotProduct3 (const ATOM_Vector4f &v1, const ATOM_Vector4f &v2)
{
  return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

inline ATOM_Vector4f crossProduct3 (const ATOM_Vector4f &v1, const ATOM_Vector4f &v2)
{
  return ATOM_Vector4f (v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x, 1.f);
}

inline ATOM_Vector4f ATOM_Vector4f::operator - (void) const
{
  return ATOM_Vector4f (-x, -y, -z, -w);
}

inline ATOM_Vector4f & ATOM_Vector4f::operator += (const ATOM_Vector4f &v)
{
  x += v.x;
  y += v.y;
  z += v.z;
  w += v.w;
  return *this;
}

inline ATOM_Vector4f & ATOM_Vector4f::operator -= (const ATOM_Vector4f &v)
{
  x -= v.x;
  y -= v.y;
  z -= v.z;
  w -= v.w;
  return *this;
}

inline ATOM_Vector4f & ATOM_Vector4f::operator *= (const ATOM_Vector4f &v)
{
  x *= v.x;
  y *= v.y;
  z *= v.z;
  w *= v.w;
  return *this;
}

inline ATOM_Vector4f & ATOM_Vector4f::operator *= (float f)
{
  x *= f;
  y *= f;
  z *= f;
  w *= f;
  return *this;
}

inline ATOM_Vector4f & ATOM_Vector4f::operator /= (const ATOM_Vector4f &v)
{
  x /= v.x;
  y /= v.y;
  z /= v.z;
  w /= v.w;
  return *this;
}

inline ATOM_Vector4f & ATOM_Vector4f::operator /= (float f)
{
  x /= f;
  y /= f;
  z /= f;
  w /= f;
  return *this;
}

inline float ATOM_Vector4f::operator [] (int i) const
{
	return xyzw[i];
}

inline float & ATOM_Vector4f::operator [] (int i)
{
	return xyzw[i];
}

