inline ATOM_Matrix3x4f::ATOM_Matrix3x4f (void)
{
}

inline ATOM_Matrix3x4f::ATOM_Matrix3x4f (const ATOM_Matrix3x4f &other)
{
	m00 = other.m00;
	m01 = other.m01;
	m02 = other.m02;
	m03 = other.m03;
	m10 = other.m10;
	m11 = other.m11;
	m12 = other.m12;
	m13 = other.m13;
	m20 = other.m20;
	m21 = other.m21;
	m22 = other.m22;
	m23 = other.m23;
}

inline ATOM_Matrix3x4f::ATOM_Matrix3x4f (float fillval)
{
	fill (fillval);
}

inline ATOM_Matrix3x4f::ATOM_Matrix3x4f (	float m00_, float m01_, float m02_, float m03_,
				float m10_, float m11_, float m12_, float m13_,
				float m20_, float m21_, float m22_, float m23_)
{
	set (m00_, m01_, m02_, m03_, m10_, m11_, m12_, m13_, m20_, m21_, m22_, m23_);
}

inline ATOM_Matrix3x4f::ATOM_Matrix3x4f (const ATOM_Matrix3x3f &upper3)
{
	set (upper3);
}

inline ATOM_Matrix3x4f::ATOM_Matrix3x4f (const ATOM_Matrix3x3f &upper3, const ATOM_Vector3f &translation)
{
	set (upper3, translation);
}

inline ATOM_Matrix3x4f::ATOM_Matrix3x4f (const ATOM_Matrix4x4f &other)
{
	set (other);
}

inline ATOM_Matrix3x4f & ATOM_Matrix3x4f::operator = (const ATOM_Matrix3x4f &other)
{
	memcpy (m, other.m, sizeof(float) * 3 * 4);
	return *this;
}

inline ATOM_Matrix3x4f & ATOM_Matrix3x4f::operator = (const ATOM_Matrix4x4f &other)
{
	set (other);
	return *this;
}

inline bool ATOM_Matrix3x4f::almostEqual (const ATOM_Matrix3x4f &other) const
{
	return	ATOM_equal (m00, other.m00) && ATOM_equal (m01, other.m01) && ATOM_equal (m02, other.m02) && ATOM_equal(m03, other.m03) &&
			ATOM_equal (m10, other.m10) && ATOM_equal (m11, other.m11) && ATOM_equal (m12, other.m12) && ATOM_equal(m13, other.m13) &&
			ATOM_equal (m20, other.m20) && ATOM_equal (m21, other.m21) && ATOM_equal (m22, other.m22) && ATOM_equal(m23, other.m23);
}

inline ATOM_Vector4f ATOM_Matrix3x4f::getRow (int row) const
{
	return ATOM_Vector4f(m[row*4], m[row*4+1], m[row*4+2], m[row*4+3]);
}

inline ATOM_Vector3f ATOM_Matrix3x4f::getCol (int col) const
{
  return ATOM_Vector3f (m[col], m[col+4], m[col+8]);
}

inline void ATOM_Matrix3x4f::setRow (int row, const ATOM_Vector4f &v)
{
  m[row*4] = v.x;
  m[row*4+1] = v.y;
  m[row*4+2] = v.z;
  m[row*4+3] = v.w;
}

inline void ATOM_Matrix3x4f::setCol (int col, const ATOM_Vector3f &v)
{
	m[col] = v.x;
	m[col+4] = v.y;
	m[col+8] = v.z;
}

inline void ATOM_Matrix3x4f::fill (float fillval)
{
	m00 = fillval;
	m01 = fillval;
	m02 = fillval;
	m03 = fillval;
	m10 = fillval;
	m11 = fillval;
	m12 = fillval;
	m13 = fillval;
	m20 = fillval;
	m21 = fillval;
	m22 = fillval;
	m23 = fillval;
}

inline void ATOM_Matrix3x4f::set (	float m00_, float m01_, float m02_, float m03_,
							float m10_, float m11_, float m12_, float m13_,
							float m20_, float m21_, float m22_, float m23_)
{
	m00 = m00_;
	m01 = m01_;
	m02 = m02_;
	m03 = m03_;
	m10 = m10_;
	m11 = m11_;
	m12 = m12_;
	m13 = m13_;
	m20 = m20_;
	m21 = m21_;
	m22 = m22_;
	m23 = m23_;
}

inline void ATOM_Matrix3x4f::set (const ATOM_Matrix4x4f &m44)
{
	m00 = m44.m00;
	m01 = m44.m10;
	m02 = m44.m20;
	m03 = m44.m30;
	m10 = m44.m01;
	m11 = m44.m11;
	m12 = m44.m21;
	m13 = m44.m31;
	m20 = m44.m02;
	m21 = m44.m12;
	m22 = m44.m22;
	m23 = m44.m32;
}

inline void ATOM_Matrix3x4f::set (const ATOM_Matrix3x3f &m33)
{
	m00 = m33.m00;
	m01 = m33.m10;
	m02 = m33.m20;
	m03 = 0.f;
	m10 = m33.m01;
	m11 = m33.m11;
	m12 = m33.m21;
	m13 = 0.f;
	m20 = m33.m02;
	m21 = m33.m12;
	m22 = m33.m22;
	m23 = 0.f;
}

inline void ATOM_Matrix3x4f::set (const ATOM_Matrix3x3f &m33, const ATOM_Vector3f &translation)
{
	m00 = m33.m00;
	m01 = m33.m10;
	m02 = m33.m20;
	m03 = translation.x;
	m10 = m33.m01;
	m11 = m33.m11;
	m12 = m33.m21;
	m13 = translation.y;
	m20 = m33.m02;
	m21 = m33.m12;
	m22 = m33.m22;
	m23 = translation.z;
}

inline ATOM_Matrix3x3f ATOM_Matrix3x4f::getUpper3 (void) const
{
	return ATOM_Matrix3x3f (m00, m10, m20, m01, m11, m21, m02, m12, m22);
}

inline void ATOM_Matrix3x4f::setUpper3 (const ATOM_Matrix3x3f &upper3)
{
	m00 = upper3.m00;
	m01 = upper3.m10;
	m02 = upper3.m20;
	m10 = upper3.m01;
	m11 = upper3.m11;
	m12 = upper3.m21;
	m20 = upper3.m02;
	m21 = upper3.m12;
	m22 = upper3.m22;
}

inline float ATOM_Matrix3x4f::operator () (int row, int col) const
{
	return m[row * 4 + col];
}

inline float & ATOM_Matrix3x4f::operator () (int row, int col)
{
	return m[row * 4 + col];
}

inline bool ATOM_Matrix3x4f::isNAN (void) const
{
	return ATOM_nan (m00) || ATOM_nan (m01) || ATOM_nan (m02) || ATOM_nan (m03)
		|| ATOM_nan (m10) || ATOM_nan (m11) || ATOM_nan (m12) || ATOM_nan (m13)
		|| ATOM_nan (m20) || ATOM_nan (m21) || ATOM_nan (m22) || ATOM_nan (m23);
}

inline void ATOM_Matrix3x4f::toMatrix44 (ATOM_Matrix4x4f &m44) const
{
	m44.m00 = m00;
	m44.m01 = m10;
	m44.m02 = m20;
	m44.m03 = 0.f;
	m44.m10 = m01;
	m44.m11 = m11;
	m44.m12 = m21;
	m44.m13 = 0.f;
	m44.m20 = m02;
	m44.m21 = m12;
	m44.m22 = m22;
	m44.m23 = 0.f;
	m44.m30 = m03;
	m44.m31 = m13;
	m44.m32 = m23;
	m44.m33 = 1.f;
}

inline ATOM_Matrix3x4f operator >> (const ATOM_Matrix3x4f &m1, const ATOM_Matrix3x4f &m2)
{
	ATOM_Matrix3x4f result;
	result.m00 = m1.m00 * m2.m00 + m1.m01 * m2.m10 + m1.m02 * m2.m20;
	result.m01 = m1.m00 * m2.m01 + m1.m01 * m2.m11 + m1.m02 * m2.m21;
	result.m02 = m1.m00 * m2.m02 + m1.m01 * m2.m12 + m1.m02 * m2.m22;
	result.m03 = m1.m00 * m2.m03 + m1.m01 * m2.m13 + m1.m02 * m2.m23 + m1.m03;
	result.m10 = m1.m10 * m2.m00 + m1.m11 * m2.m10 + m1.m12 * m2.m20;
	result.m11 = m1.m10 * m2.m01 + m1.m11 * m2.m11 + m1.m12 * m2.m21;
	result.m12 = m1.m10 * m2.m02 + m1.m11 * m2.m12 + m1.m12 * m2.m22;
	result.m13 = m1.m10 * m2.m03 + m1.m11 * m2.m13 + m1.m12 * m2.m23 + m1.m13;
	result.m20 = m1.m20 * m2.m00 + m1.m21 * m2.m10 + m1.m22 * m2.m20;
	result.m21 = m1.m20 * m2.m01 + m1.m21 * m2.m11 + m1.m22 * m2.m21;
	result.m22 = m1.m20 * m2.m02 + m1.m21 * m2.m12 + m1.m22 * m2.m22;
	result.m23 = m1.m20 * m2.m03 + m1.m21 * m2.m13 + m1.m22 * m2.m23 + m1.m23;
	return result;
}

inline ATOM_Matrix3x4f operator << (const ATOM_Matrix3x4f &m1, const ATOM_Matrix3x4f &m2)
{
	return operator >> (m2, m1);
}

inline ATOM_Vector4f operator >> (const ATOM_Matrix3x4f &m, const ATOM_Vector4f &v)
{
	return ATOM_Vector4f (	m.m00 * v.x + m.m01 * v.y + m.m02 * v.z + m.m03 * v.w,
							m.m10 * v.x + m.m11 * v.y + m.m12 * v.z + m.m13 * v.w,
							m.m20 * v.x + m.m21 * v.y + m.m22 * v.z + m.m23 * v.w,
							v.w
						);
}

inline ATOM_Vector4f operator << (const ATOM_Vector4f &v, const ATOM_Matrix3x4f &m)
{
	return operator >> (m, v);
}

inline ATOM_Vector3f operator >> (const ATOM_Matrix3x4f &m, const ATOM_Vector3f &v)
{
	return ATOM_Vector3f (	m.m00 * v.x + m.m01 * v.y + m.m02 * v.z + m.m03,
							m.m10 * v.x + m.m11 * v.y + m.m12 * v.z + m.m13,
							m.m20 * v.x + m.m21 * v.y + m.m22 * v.z + m.m23
						);
}

inline ATOM_Vector3f operator << (const ATOM_Vector3f &v, const ATOM_Matrix3x4f &m)
{
	return operator >> (m, v);
}

inline ATOM_Matrix3x4f operator + (const ATOM_Matrix3x4f &m1, const ATOM_Matrix3x4f &m2)
{
	ATOM_Matrix3x4f result;
	result.m00 = m1.m00 + m2.m00;
	result.m01 = m1.m01 + m2.m01;
	result.m02 = m1.m02 + m2.m02;
	result.m03 = m1.m03 + m2.m03;
	result.m10 = m1.m10 + m2.m10;
	result.m11 = m1.m11 + m2.m11;
	result.m12 = m1.m12 + m2.m12;
	result.m13 = m1.m13 + m2.m13;
	result.m20 = m1.m20 + m2.m20;
	result.m21 = m1.m21 + m2.m21;
	result.m22 = m1.m22 + m2.m22;
	result.m23 = m1.m23 + m2.m23;
	return result;
}

inline ATOM_Matrix3x4f operator - (const ATOM_Matrix3x4f &m1, const ATOM_Matrix3x4f &m2)
{
	ATOM_Matrix3x4f result;
	result.m00 = m1.m00 - m2.m00;
	result.m01 = m1.m01 - m2.m01;
	result.m02 = m1.m02 - m2.m02;
	result.m03 = m1.m03 - m2.m03;
	result.m10 = m1.m10 - m2.m10;
	result.m11 = m1.m11 - m2.m11;
	result.m12 = m1.m12 - m2.m12;
	result.m13 = m1.m13 - m2.m13;
	result.m20 = m1.m20 - m2.m20;
	result.m21 = m1.m21 - m2.m21;
	result.m22 = m1.m22 - m2.m22;
	result.m23 = m1.m23 - m2.m23;
	return result;
}

inline ATOM_Matrix3x4f operator * (const ATOM_Matrix3x4f &m1, const ATOM_Matrix3x4f &m2)
{
	ATOM_Matrix3x4f result;
	result.m00 = m1.m00 * m2.m00;
	result.m01 = m1.m01 * m2.m01;
	result.m02 = m1.m02 * m2.m02;
	result.m03 = m1.m03 * m2.m03;
	result.m10 = m1.m10 * m2.m10;
	result.m11 = m1.m11 * m2.m11;
	result.m12 = m1.m12 * m2.m12;
	result.m13 = m1.m13 * m2.m13;
	result.m20 = m1.m20 * m2.m20;
	result.m21 = m1.m21 * m2.m21;
	result.m22 = m1.m22 * m2.m22;
	result.m23 = m1.m23 * m2.m23;
	return result;
}

inline ATOM_Matrix3x4f operator * (const ATOM_Matrix3x4f &m, float f)
{
	ATOM_Matrix3x4f result;
	result.m00 = m.m00 * f;
	result.m01 = m.m01 * f;
	result.m02 = m.m02 * f;
	result.m03 = m.m03 * f;
	result.m10 = m.m10 * f;
	result.m11 = m.m11 * f;
	result.m12 = m.m12 * f;
	result.m13 = m.m13 * f;
	result.m20 = m.m20 * f;
	result.m21 = m.m21 * f;
	result.m22 = m.m22 * f;
	result.m23 = m.m23 * f;
	return result;
}

inline ATOM_Matrix3x4f operator * (float f, const ATOM_Matrix3x4f &m)
{
	return operator * (m, f);
}

inline ATOM_Matrix3x4f operator / (const ATOM_Matrix3x4f &m1, const ATOM_Matrix3x4f &m2)
{
	ATOM_Matrix3x4f result;
	result.m00 = m1.m00 / m2.m00;
	result.m01 = m1.m01 / m2.m01;
	result.m02 = m1.m02 / m2.m02;
	result.m03 = m1.m03 / m2.m03;
	result.m10 = m1.m10 / m2.m10;
	result.m11 = m1.m11 / m2.m11;
	result.m12 = m1.m12 / m2.m12;
	result.m13 = m1.m13 / m2.m13;
	result.m20 = m1.m20 / m2.m20;
	result.m21 = m1.m21 / m2.m21;
	result.m22 = m1.m22 / m2.m22;
	result.m23 = m1.m23 / m2.m23;
	return result;
}

inline ATOM_Matrix3x4f operator / (const ATOM_Matrix3x4f &m1, float f)
{
	ATOM_Matrix3x4f result;
	result.m00 = m1.m00 / f;
	result.m01 = m1.m01 / f;
	result.m02 = m1.m02 / f;
	result.m03 = m1.m03 / f;
	result.m10 = m1.m10 / f;
	result.m11 = m1.m11 / f;
	result.m12 = m1.m12 / f;
	result.m13 = m1.m13 / f;
	result.m20 = m1.m20 / f;
	result.m21 = m1.m21 / f;
	result.m22 = m1.m22 / f;
	result.m23 = m1.m23 / f;
	return result;
}

inline bool operator == (const ATOM_Matrix3x4f &m1, const ATOM_Matrix3x4f &m2)
{
	return m1.m00 == m2.m00 && m1.m01 == m2.m01 && m1.m02 == m2.m02 && m1.m03 == m2.m03
		&& m1.m10 == m2.m10 && m1.m11 == m2.m11 && m1.m12 == m2.m12 && m1.m13 == m2.m13
		&& m1.m20 == m2.m20 && m1.m21 == m2.m21 && m1.m22 == m2.m22 && m1.m23 == m2.m23;
}

inline bool operator != (const ATOM_Matrix3x4f &m1, const ATOM_Matrix3x4f &m2)
{
	return ! operator == (m1, m2);
}


inline ATOM_Matrix3x4f & ATOM_Matrix3x4f::operator >>= (const ATOM_Matrix3x4f &other)
{
	*this = operator >> (*this, other);
	return *this;
}

inline ATOM_Matrix3x4f & ATOM_Matrix3x4f::operator <<= (const ATOM_Matrix3x4f &other)
{
	*this = operator << (*this, other);
	return *this;
}

inline ATOM_Matrix3x4f & ATOM_Matrix3x4f::operator += (const ATOM_Matrix3x4f &other)
{
	*this = operator + (*this, other);
	return *this;
}

inline ATOM_Matrix3x4f & ATOM_Matrix3x4f::operator -= (const ATOM_Matrix3x4f &other)
{
	*this = operator - (*this, other);
	return *this;
}

inline ATOM_Matrix3x4f & ATOM_Matrix3x4f::operator *= (const ATOM_Matrix3x4f &other)
{
	*this = operator * (*this, other);
	return *this;
}

inline ATOM_Matrix3x4f & ATOM_Matrix3x4f::operator *= (float f)
{
	*this = operator * (*this, f);
	return *this;
}

inline ATOM_Matrix3x4f & ATOM_Matrix3x4f::operator /= (const ATOM_Matrix3x4f &other)
{
	*this = operator / (*this, other);
	return *this;
}

inline ATOM_Matrix3x4f & ATOM_Matrix3x4f::operator /= (float f)
{
	*this = operator / (*this, f);
	return *this;
}


inline ATOM_Vector3f ATOM_Matrix3x4f::transformPoint (const ATOM_Vector3f &v) const
{
	return ATOM_Vector3f (	m00 * v.x + m01 * v.y + m02 * v.z + m03,
							m10 * v.x + m11 * v.y + m12 * v.z + m13,
							m20 * v.x + m21 * v.y + m22 * v.z + m23
						);
}

inline ATOM_Vector3f ATOM_Matrix3x4f::transformVector(const ATOM_Vector3f &v) const
{
	return ATOM_Vector3f (	m00 * v.x + m01 * v.y + m02 * v.z,
							m10 * v.x + m11 * v.y + m12 * v.z,
							m20 * v.x + m21 * v.y + m22 * v.z
						);
}

inline ATOM_Vector4f ATOM_Matrix3x4f::transformPoint (const ATOM_Vector4f &v) const
{
	return ATOM_Vector4f (	m00 * v.x + m01 * v.y + m02 * v.z + m03 * v.w,
							m10 * v.x + m11 * v.y + m12 * v.z + m13 * v.w,
							m20 * v.x + m21 * v.y + m22 * v.z + m23 * v.w,
							v.w
						);
}

inline ATOM_Vector3f ATOM_Matrix3x4f::transformPointAffine (const ATOM_Vector3f &v) const
{
	return transformPoint (v);
}

inline ATOM_Vector3f ATOM_Matrix3x4f::transformVectorAffine (const ATOM_Vector3f &v) const
{
	return transformVector (v);
}

inline ATOM_Vector4f ATOM_Matrix3x4f::transformPointAffine (const ATOM_Vector4f &v) const
{
	return transformPoint (v);
}

inline void ATOM_Matrix3x4f::transformPointInplace (ATOM_Vector3f &v) const
{
	v = transformPoint (v);
}

inline void ATOM_Matrix3x4f::transformPointInplace (ATOM_Vector4f &v) const
{
	v = transformPoint (v);
}

inline void ATOM_Matrix3x4f::transformVectorInplace (ATOM_Vector3f &v) const
{
	v = transformVector (v);
}

inline void ATOM_Matrix3x4f::transformPointAffineInplace (ATOM_Vector3f &v) const
{
	v = transformPointAffine (v);
}

inline void ATOM_Matrix3x4f::transformPointAffineInplace (ATOM_Vector4f &v) const
{
	v = transformPointAffine (v);
}

inline void ATOM_Matrix3x4f::transformVectorAffineInplace (ATOM_Vector3f &v) const
{
	v = transformVectorAffine (v);
}

inline void ATOM_Matrix3x4f::makeIdentity (void)
{
	m00 = 1.f;
	m01 = 0.f;
	m02 = 0.f;
	m03 = 0.f;
	m10 = 0.f;
	m11 = 1.f;
	m12 = 0.f;
	m13 = 0.f;
	m20 = 0.f;
	m21 = 0.f;
	m22 = 1.f;
	m23 = 0.f;
}

inline const ATOM_Matrix3x4f &ATOM_Matrix3x4f::getIdentityMatrix (void)
{
  static const ATOM_Matrix3x4f identMatrix(1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f);
  return identMatrix;
}

