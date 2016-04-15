inline ATOM_Matrix3x3f::ATOM_Matrix3x3f (void)
{
}

inline ATOM_Matrix3x3f::ATOM_Matrix3x3f (float fillval):
  m00(fillval),
  m01(fillval),
  m02(fillval),
  m10(fillval),
  m11(fillval),
  m12(fillval),
  m20(fillval),
  m21(fillval),
  m22(fillval)
{
}

inline ATOM_Matrix3x3f::ATOM_Matrix3x3f (float m00_, float m01_, float m02_, float m10_, float m11_, float m12_, float m20_, float m21_, float m22_):
  m00(m00_),
  m01(m01_),
  m02(m02_),
  m10(m10_),
  m11(m11_),
  m12(m12_),
  m20(m20_),
  m21(m21_),
  m22(m22_)
{
}

inline bool ATOM_Matrix3x3f::almostEqual (const ATOM_Matrix3x3f &other) const
{
  return ATOM_equal (m00, other.m00) && ATOM_equal (m01, other.m01) && ATOM_equal (m02, other.m02) &&
          ATOM_equal (m10, other.m10) && ATOM_equal (m11, other.m11) && ATOM_equal (m12, other.m12) &&
          ATOM_equal (m20, other.m20) && ATOM_equal (m21, other.m21) && ATOM_equal (m22, other.m22);
}

inline ATOM_Vector3f ATOM_Matrix3x3f::getRow (int row) const
{
  return ATOM_Vector3f (m[row*3], m[row*3+1], m[row*3+2]);
}

inline ATOM_Vector3f ATOM_Matrix3x3f::getCol (int col) const
{
  return ATOM_Vector3f (m[col], m[col+3], m[col+6]);
}

inline void ATOM_Matrix3x3f::getRow (int row, ATOM_Vector3f &v) const
{
  v.set (m[row*3], m[row*3+1], m[row*3+2]);
}

inline void ATOM_Matrix3x3f::getCol (int col, ATOM_Vector3f &v) const
{
  v.set (m[col], m[col+3], m[col+6]);
}

inline void ATOM_Matrix3x3f::setRow (int row, const ATOM_Vector3f &v)
{
  m[row*3] = v.x;
  m[row*3+1] = v.y;
  m[row*3+2] = v.z;
}

inline void ATOM_Matrix3x3f::setCol (int col, const ATOM_Vector3f &v)
{
  m[col] = v.x;
  m[col+3] = v.y;
  m[col+6] = v.z;
}

inline void ATOM_Matrix3x3f::setRow (int row, float x, float y, float z)
{
  m[row*3] = x;
  m[row*3+1] = y;
  m[row*3+2] = z;
}

inline void ATOM_Matrix3x3f::setCol (int col, float x, float y, float z)
{
  m[col] = x;
  m[col+3] = y;
  m[col+6] = z;
}

inline void ATOM_Matrix3x3f::fill (float val)
{
  m00 = val;
  m01 = val;
  m02 = val;
  m10 = val;
  m11 = val;
  m12 = val;
  m20 = val;
  m21 = val;
  m22 = val;
}

inline void ATOM_Matrix3x3f::set (float m00_, float m01_, float m02_, float m10_, float m11_, float m12_, float m20_, float m21_, float m22_)
{
  m00 = m00_;
  m01 = m01_;
  m02 = m02_;
  m10 = m10_;
  m11 = m11_;
  m12 = m12_;
  m20 = m20_;
  m21 = m21_;
  m22 = m22_;
}

inline float ATOM_Matrix3x3f::operator () (int row, int col) const
{
  return m[3 * row + col];
}

inline float & ATOM_Matrix3x3f::operator () (int row, int col)
{
  return m[3 * row + col];
}

inline bool ATOM_Matrix3x3f::isNAN (void) const
{
  return ATOM_nan(m[0]) || ATOM_nan(m[1]) || ATOM_nan(m[2]) || ATOM_nan(m[3]) || ATOM_nan(m[4]) || ATOM_nan(m[5]) || ATOM_nan(m[6]) || ATOM_nan(m[7]) || ATOM_nan(m[8]);
}

static inline float DET2 (float e00, float e01, float e10, float e11)
{
  return e00 * e11 - e10 * e01;
}

inline float ATOM_Matrix3x3f::getDeterminant (void) const
{
  return m00 * DET2(m11, m12, m21, m22) - m01 * DET2(m10, m12, m20, m22) + m02 * DET2(m10, m11, m20, m21);
}

inline void ATOM_Matrix3x3f::transpose (void)
{
  float tmp;

  tmp = m10;
  m10 = m01;
  m01 = tmp;

  tmp = m12;
  m12 = m21;
  m21 = tmp;

  tmp = m02;
  m02 = m20;
  m20 = tmp;
}

inline void ATOM_Matrix3x3f::transposeFrom (const ATOM_Matrix3x3f &other)
{
  m00 = other.m00;
  m01 = other.m10;
  m02 = other.m20;
  m10 = other.m01;
  m11 = other.m11;
  m12 = other.m21;
  m20 = other.m02;
  m21 = other.m12;
  m22 = other.m22;
}

inline bool ATOM_Matrix3x3f::invert (void)
{
  float det = getDeterminant ();
  if (ATOM_equal (det, 0.f))
  {
    makeIdentity ();
    return false;
  }

  det = 1.f / det;

  float _00 =  det * DET2(m11, m12, m21, m22);
  float _10 = -det * DET2(m10, m12, m20, m22);
  float _20 =  det * DET2(m10, m11, m20, m21);
  float _01 = -det * DET2(m01, m02, m21, m22);
  float _11 =  det * DET2(m00, m02, m20, m22);
  float _21 = -det * DET2(m00, m01, m20, m21);
  float _02 =  det * DET2(m01, m02, m11, m12);
  float _12 = -det * DET2(m00, m02, m10, m12);
  float _22 =  det * DET2(m00, m01, m10, m11);

  m00 = _00;
  m10 = _10;
  m20 = _20;
  m01 = _01;
  m11 = _11;
  m21 = _21;
  m02 = _02;
  m12 = _12;
  m22 = _22;

  return true;
}

inline bool ATOM_Matrix3x3f::invertFrom (const ATOM_Matrix3x3f &other)
{
  float det = other.getDeterminant ();
  if (ATOM_equal (det, 0.f))
  {
    makeIdentity ();
    return false;
  }

  det = 1.f / det;

  m00 =  det * DET2(other.m11, other.m12, other.m21, other.m22);
  m10 = -det * DET2(other.m10, other.m12, other.m20, other.m22);
  m20 =  det * DET2(other.m10, other.m11, other.m20, other.m21);
  m01 = -det * DET2(other.m01, other.m02, other.m21, other.m22);
  m11 =  det * DET2(other.m00, other.m02, other.m20, other.m22);
  m21 = -det * DET2(other.m00, other.m01, other.m20, other.m21);
  m02 =  det * DET2(other.m01, other.m02, other.m11, other.m12);
  m12 = -det * DET2(other.m00, other.m02, other.m10, other.m12);
  m22 =  det * DET2(other.m00, other.m01, other.m10, other.m11);

  return true;
}

inline bool ATOM_Matrix3x3f::invertTranspose (void)
{
  float det = getDeterminant ();
  if (ATOM_equal (det, 0.f))
  {
    makeIdentity ();
    return false;
  }

  det = 1.f / det;

  float _00 =  det * DET2(m11, m12, m21, m22);
  float _01 = -det * DET2(m10, m12, m20, m22);
  float _02 =  det * DET2(m10, m11, m20, m21);
  float _10 = -det * DET2(m01, m02, m21, m22);
  float _11 =  det * DET2(m00, m02, m20, m22);
  float _12 = -det * DET2(m00, m01, m20, m21);
  float _20 =  det * DET2(m01, m02, m11, m12);
  float _21 = -det * DET2(m00, m02, m10, m12);
  float _22 =  det * DET2(m00, m01, m10, m11);

  m00 = _00;
  m10 = _10;
  m20 = _20;
  m01 = _01;
  m11 = _11;
  m21 = _21;
  m02 = _02;
  m12 = _12;
  m22 = _22;

  return true;
}

inline bool ATOM_Matrix3x3f::invertTransposeFrom (const ATOM_Matrix3x3f &other)
{
  float det = other.getDeterminant ();
  if (ATOM_equal (det, 0.f))
  {
    makeIdentity ();
    return false;
  }

  det = 1.f / det;

  m00 =  det * DET2(other.m11, other.m12, other.m21, other.m22);
  m01 = -det * DET2(other.m10, other.m12, other.m20, other.m22);
  m02 =  det * DET2(other.m10, other.m11, other.m20, other.m21);
  m10 = -det * DET2(other.m01, other.m02, other.m21, other.m22);
  m11 =  det * DET2(other.m00, other.m02, other.m20, other.m22);
  m12 = -det * DET2(other.m00, other.m01, other.m20, other.m21);
  m20 =  det * DET2(other.m01, other.m02, other.m11, other.m12);
  m21 = -det * DET2(other.m00, other.m02, other.m10, other.m12);
  m22 =  det * DET2(other.m00, other.m01, other.m10, other.m11);

  return true;
}

inline ATOM_Matrix3x3f operator >> (const ATOM_Matrix3x3f &m1, const ATOM_Matrix3x3f &m2)
{
  ATOM_Matrix3x3f m;
  m.m00 = m1.m00 * m2.m00 + m1.m10 * m2.m01 + m1.m20 * m2.m02;
  m.m01 = m1.m01 * m2.m00 + m1.m11 * m2.m01 + m1.m21 * m2.m02;
  m.m02 = m1.m02 * m2.m00 + m1.m12 * m2.m01 + m1.m22 * m2.m02;
  m.m10 = m1.m00 * m2.m10 + m1.m10 * m2.m11 + m1.m20 * m2.m12;
  m.m11 = m1.m01 * m2.m10 + m1.m11 * m2.m11 + m1.m21 * m2.m12;
  m.m12 = m1.m02 * m2.m10 + m1.m12 * m2.m11 + m1.m22 * m2.m12;
  m.m20 = m1.m00 * m2.m20 + m1.m10 * m2.m21 + m1.m20 * m2.m22;
  m.m21 = m1.m01 * m2.m20 + m1.m11 * m2.m21 + m1.m21 * m2.m22;
  m.m22 = m1.m02 * m2.m20 + m1.m12 * m2.m21 + m1.m22 * m2.m22;
  return m;
}

inline ATOM_Matrix3x3f operator << (const ATOM_Matrix3x3f &m1, const ATOM_Matrix3x3f &m2)
{
  return operator >> (m2, m1);
}

inline ATOM_Vector3f operator >> (const ATOM_Matrix3x3f &m, const ATOM_Vector3f &v)
{
  return m.transform (v);
}

inline ATOM_Vector3f operator << (const ATOM_Vector3f &v, const ATOM_Matrix3x3f &m)
{
  return m.transform (v);
}

inline ATOM_Matrix3x3f operator + (const ATOM_Matrix3x3f &m1, const ATOM_Matrix3x3f &m2)
{
  ATOM_Matrix3x3f m;
  m.m00 = m1.m00 + m2.m00;
  m.m01 = m1.m01 + m2.m01;
  m.m02 = m1.m02 + m2.m02;
  m.m10 = m1.m10 + m2.m10;
  m.m11 = m1.m11 + m2.m11;
  m.m12 = m1.m12 + m2.m12;
  m.m20 = m1.m20 + m2.m20;
  m.m21 = m1.m21 + m2.m21;
  m.m22 = m1.m22 + m2.m22;
  return m;
}

inline ATOM_Matrix3x3f operator - (const ATOM_Matrix3x3f &m1, const ATOM_Matrix3x3f &m2)
{
  ATOM_Matrix3x3f m;
  m.m00 = m1.m00 - m2.m00;
  m.m01 = m1.m01 - m2.m01;
  m.m02 = m1.m02 - m2.m02;
  m.m10 = m1.m10 - m2.m10;
  m.m11 = m1.m11 - m2.m11;
  m.m12 = m1.m12 - m2.m12;
  m.m20 = m1.m20 - m2.m20;
  m.m21 = m1.m21 - m2.m21;
  m.m22 = m1.m22 - m2.m22;
  return m;
}

inline ATOM_Matrix3x3f operator * (const ATOM_Matrix3x3f &m1, const ATOM_Matrix3x3f &m2)
{
  ATOM_Matrix3x3f m;
  m.m00 = m1.m00 * m2.m00;
  m.m01 = m1.m01 * m2.m01;
  m.m02 = m1.m02 * m2.m02;
  m.m10 = m1.m10 * m2.m10;
  m.m11 = m1.m11 * m2.m11;
  m.m12 = m1.m12 * m2.m12;
  m.m20 = m1.m20 * m2.m20;
  m.m21 = m1.m21 * m2.m21;
  m.m22 = m1.m22 * m2.m22;
  return m;
}

inline ATOM_Matrix3x3f operator * (const ATOM_Matrix3x3f &m, float f)
{
  ATOM_Matrix3x3f mat;
  mat.m00 = m.m00 * f;
  mat.m01 = m.m01 * f;
  mat.m02 = m.m02 * f;
  mat.m10 = m.m10 * f;
  mat.m11 = m.m11 * f;
  mat.m12 = m.m12 * f;
  mat.m20 = m.m20 * f;
  mat.m21 = m.m21 * f;
  mat.m22 = m.m22 * f;
  return m;
}

inline ATOM_Matrix3x3f operator * (float f, const ATOM_Matrix3x3f &m)
{
  return operator * (m, f);
}

inline ATOM_Matrix3x3f operator / (const ATOM_Matrix3x3f &m1, const ATOM_Matrix3x3f &m2)
{
  ATOM_Matrix3x3f m;
  m.m00 = m1.m00 / m2.m00;
  m.m01 = m1.m01 / m2.m01;
  m.m02 = m1.m02 / m2.m02;
  m.m10 = m1.m10 / m2.m10;
  m.m11 = m1.m11 / m2.m11;
  m.m12 = m1.m12 / m2.m12;
  m.m20 = m1.m20 / m2.m20;
  m.m21 = m1.m21 / m2.m21;
  m.m22 = m1.m22 / m2.m22;
  return m;
}

inline ATOM_Matrix3x3f operator / (const ATOM_Matrix3x3f &m1, float f)
{
  ATOM_Matrix3x3f m;
  m.m00 = m.m00 / f;
  m.m01 = m.m01 / f;
  m.m02 = m.m02 / f;
  m.m10 = m.m10 / f;
  m.m11 = m.m11 / f;
  m.m12 = m.m12 / f;
  m.m20 = m.m20 / f;
  m.m21 = m.m21 / f;
  m.m22 = m.m22 / f;
  return m;
}

inline bool operator == (const ATOM_Matrix3x3f &m1, const ATOM_Matrix3x3f &m2)
{
  return m1.m00 == m2.m00 && m1.m01 == m2.m01 && m1.m02 == m2.m02 &&
         m1.m10 == m2.m10 && m1.m11 == m2.m11 && m1.m12 == m2.m12 &&
         m1.m20 == m2.m20 && m1.m21 == m2.m21 && m1.m22 == m2.m22;
}

inline bool operator != (const ATOM_Matrix3x3f &m1, const ATOM_Matrix3x3f &m2)
{
  return ! operator == (m1, m2);
}

inline ATOM_Matrix3x3f & ATOM_Matrix3x3f::operator >>= (const ATOM_Matrix3x3f &other)
{
  *this = (*this >> other);
  return *this;
}

inline ATOM_Matrix3x3f & ATOM_Matrix3x3f::operator <<= (const ATOM_Matrix3x3f &other)
{
  *this = (*this) << other;
  return *this;
}

inline ATOM_Matrix3x3f & ATOM_Matrix3x3f::operator += (const ATOM_Matrix3x3f &other)
{
  m00 += other.m00;
  m01 += other.m01;
  m02 += other.m02;
  m10 += other.m10;
  m11 += other.m11;
  m12 += other.m12;
  m20 += other.m20;
  m21 += other.m21;
  m22 += other.m22;
  return *this;
}

inline ATOM_Matrix3x3f & ATOM_Matrix3x3f::operator -= (const ATOM_Matrix3x3f &other)
{
  m00 -= other.m00;
  m01 -= other.m01;
  m02 -= other.m02;
  m10 -= other.m10;
  m11 -= other.m11;
  m12 -= other.m12;
  m20 -= other.m20;
  m21 -= other.m21;
  m22 -= other.m22;
  return *this;
}

inline ATOM_Matrix3x3f & ATOM_Matrix3x3f::operator *= (const ATOM_Matrix3x3f &other)
{
  m00 *= other.m00;
  m01 *= other.m01;
  m02 *= other.m02;
  m10 *= other.m10;
  m11 *= other.m11;
  m12 *= other.m12;
  m20 *= other.m20;
  m21 *= other.m21;
  m22 *= other.m22;
  return *this;
}

inline ATOM_Matrix3x3f & ATOM_Matrix3x3f::operator *= (float f)
{
  m00 *= f;
  m01 *= f;
  m02 *= f;
  m10 *= f;
  m11 *= f;
  m12 *= f;
  m20 *= f;
  m21 *= f;
  m22 *= f;
  return *this;
}

inline ATOM_Matrix3x3f & ATOM_Matrix3x3f::operator /= (const ATOM_Matrix3x3f &other)
{
  m00 /= other.m00;
  m01 /= other.m01;
  m02 /= other.m02;
  m10 /= other.m10;
  m11 /= other.m11;
  m12 /= other.m12;
  m20 /= other.m20;
  m21 /= other.m21;
  m22 /= other.m22;
  return *this;
}

inline ATOM_Matrix3x3f & ATOM_Matrix3x3f::operator /= (float f)
{
  m00 /= f;
  m01 /= f;
  m02 /= f;
  m10 /= f;
  m11 /= f;
  m12 /= f;
  m20 /= f;
  m21 /= f;
  m22 /= f;
  return *this;
}

inline ATOM_Vector3f ATOM_Matrix3x3f::transform (const ATOM_Vector3f &v) const
{
  float x = v.x * m00 + v.y * m10 + v.z * m20;
  float y = v.x * m01 + v.y * m11 + v.z * m21;
  float z = v.x * m02 + v.y * m12 + v.z * m22;
  return ATOM_Vector3f (x, y, z);
}

inline void ATOM_Matrix3x3f::transformInplace (ATOM_Vector3f &v) const
{
  float x = v.x * m00 + v.y * m10 + v.z * m20;
  float y = v.x * m01 + v.y * m11 + v.z * m21;
  float z = v.x * m02 + v.y * m12 + v.z * m22;
  v.x = x;
  v.y = y;
  v.z = z;
}

inline void ATOM_Matrix3x3f::makeIdentity (void)
{
	m00 = 1.f;
	m01 = 0.f;
	m02 = 0.f;
	m10 = 0.f;
	m11 = 1.f;
	m12 = 0.f;
	m20 = 0.f;
	m21 = 0.f;
	m22 = 1.f;
}

inline ATOM_Matrix3x3f ATOM_Matrix3x3f::getIdentityMatrix (void)
{
	return ATOM_Matrix3x3f (1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f);
}

inline void ATOM_Matrix3x3f::makeRotateX (float angle)
{
  float s, c;
  ATOM_sincos (angle, &s, &c);

  m00 = 1.f;
  m01 = 0.f;
  m02 = 0.f;
  m10 = 0.f;
  m11 = c;
  m12 = s;
  m20 = 0.f;
  m21 = -s;
  m22 = c;
}

inline ATOM_Matrix3x3f ATOM_Matrix3x3f::getRotateXMatrix (float angle)
{
  ATOM_Matrix3x3f m;
  m.makeRotateX (angle);
  return m;
}

inline void ATOM_Matrix3x3f::makeRotateY (float angle)
{
  float s, c;
  ATOM_sincos (angle, &s, &c);

  m00 = c;
  m01 = 0.f;
  m02 = -s;
  m10 = 0.f;
  m11 = 1.f;
  m12 = 0.f;
  m20 = s;
  m21 = 0.f;
  m22 = c;
}

inline ATOM_Matrix3x3f ATOM_Matrix3x3f::getRotateYMatrix (float angle)
{
  ATOM_Matrix3x3f m;
  m.makeRotateY (angle);
  return m;
}

inline void ATOM_Matrix3x3f::makeRotateZ (float angle)
{
  float s, c;
  ATOM_sincos (angle, &s, &c);

  m00 = c;
  m01 = s;
  m02 = 0.f;
  m10 = -s;
  m11 = c;
  m12 = 0.f;
  m20 = 0.f;
  m21 = 0.f;
  m22 = 1.f;
}

inline ATOM_Matrix3x3f ATOM_Matrix3x3f::getRotateZMatrix (float angle)
{
  ATOM_Matrix3x3f m;
  m.makeRotateZ (angle);
  return m;
}

inline void ATOM_Matrix3x3f::makeRotateAngleAxis (float angle, const ATOM_Vector3f &axis)
{
  ATOM_Vector3f nAxis;
  nAxis.normalizeFrom (axis);
  makeRotateAngleNormalizedAxis (angle, nAxis);
}

inline void ATOM_Matrix3x3f::makeRotateAngleNormalizedAxis (float angle, const ATOM_Vector3f &normalizedAxis)
{
  float s, c;
  ATOM_sincos (angle, &s, &c);

  float xx = normalizedAxis.x * normalizedAxis.x;
  float yy = normalizedAxis.y * normalizedAxis.y;
  float zz = normalizedAxis.z * normalizedAxis.z;
  float xy = normalizedAxis.x * normalizedAxis.y;
  float yz = normalizedAxis.y * normalizedAxis.z;
  float zx = normalizedAxis.z * normalizedAxis.x;
  float xs = normalizedAxis.x * s;
  float ys = normalizedAxis.y * s;
  float zs = normalizedAxis.z * s;
  float ic = 1.f - c;

  m00 = ic * xx + c;
  m01 = ic * xy + zs;
  m02 = ic * zx - ys;

  m10 = ic * xy - zs;
  m11 = ic * yy + c;
  m12 = ic * yz + xs;

  m20 = ic * zx + ys;
  m21 = ic * yz - xs;
  m22 = ic * zz + c;
}

inline ATOM_Matrix3x3f ATOM_Matrix3x3f::getRotateAngleAxisMatrix (float angle, const ATOM_Vector3f &axis)
{
  ATOM_Matrix3x3f m;
  m.makeRotateAngleAxis (angle, axis);
  return m;
}

inline ATOM_Matrix3x3f ATOM_Matrix3x3f::getRotateAngleNormalizedAxisMatrix (float angle, const ATOM_Vector3f &normalizedAxis)
{
  ATOM_Matrix3x3f m;
  m.makeRotateAngleNormalizedAxis (angle, normalizedAxis);
  return m;
}

inline void ATOM_Matrix3x3f::makeScale (const ATOM_Vector3f &scale)
{
  m00 = scale.x;
  m01 = 0.f;
  m02 = 0.f;
  m10 = 0.f;
  m11 = scale.y;
  m12 = 0.f;
  m20 = 0.f;
  m21 = 0.f;
  m22 = scale.z;
}

inline ATOM_Matrix3x3f ATOM_Matrix3x3f::getScaleMatrix (const ATOM_Vector3f &scale)
{
  ATOM_Matrix3x3f m;
  m.makeScale (scale);
  return m;
}

inline ATOM_Vector3f & ATOM_Vector3f::operator <<= (const ATOM_Matrix3x3f &m)
{
  m.transformInplace (*this);
  return *this;
}
