inline ATOM_Matrix4x4f::ATOM_Matrix4x4f (void)
{
}

inline ATOM_Matrix4x4f::ATOM_Matrix4x4f (float fillval):
  m00(fillval),
  m01(fillval),
  m02(fillval),
  m03(fillval),
  m10(fillval),
  m11(fillval),
  m12(fillval),
  m13(fillval),
  m20(fillval),
  m21(fillval),
  m22(fillval),
  m23(fillval),
  m30(fillval),
  m31(fillval),
  m32(fillval),
  m33(fillval)
{
}

inline ATOM_Matrix4x4f::ATOM_Matrix4x4f (const ATOM_Matrix4x4f &other)
{
  memcpy (this, &other, sizeof(ATOM_Matrix4x4f));
}

inline ATOM_Matrix4x4f::ATOM_Matrix4x4f (float m00_, float m01_, float m02_, float m03_,
                               float m10_, float m11_, float m12_, float m13_,
                               float m20_, float m21_, float m22_, float m23_,
                               float m30_, float m31_, float m32_, float m33_):
  m00(m00_),
  m01(m01_),
  m02(m02_),
  m03(m03_),
  m10(m10_),
  m11(m11_),
  m12(m12_),
  m13(m13_),
  m20(m20_),
  m21(m21_),
  m22(m22_),
  m23(m23_),
  m30(m30_),
  m31(m31_),
  m32(m32_),
  m33(m33_)
{
}

inline ATOM_Matrix4x4f::ATOM_Matrix4x4f (const ATOM_Matrix3x3f &upper3):
  m00(upper3.m00),
  m01(upper3.m01),
  m02(upper3.m02),
  m03(0.f),
  m10(upper3.m10),
  m11(upper3.m11),
  m12(upper3.m12),
  m13(0.f),
  m20(upper3.m20),
  m21(upper3.m21),
  m22(upper3.m22),
  m23(0.f),
  m30(0.f),
  m31(0.f),
  m32(0.f),
  m33(1.f)
{
}

inline ATOM_Matrix4x4f::ATOM_Matrix4x4f (const ATOM_Matrix3x3f &upper3, const ATOM_Vector3f &translation):
  m00(upper3.m00),
  m01(upper3.m01),
  m02(upper3.m02),
  m03(0.f),
  m10(upper3.m10),
  m11(upper3.m11),
  m12(upper3.m12),
  m13(0.f),
  m20(upper3.m20),
  m21(upper3.m21),
  m22(upper3.m22),
  m23(0.f),
  m30(translation.x),
  m31(translation.y),
  m32(translation.z),
  m33(1.f)
{
}

inline ATOM_Matrix4x4f & ATOM_Matrix4x4f::operator = (const ATOM_Matrix4x4f &other)
{
  memcpy (this, &other, sizeof(ATOM_Matrix4x4f));
  return *this;
}

inline bool ATOM_Matrix4x4f::almostEqual (const ATOM_Matrix4x4f &other) const
{
  return ATOM_equal (m00, other.m00) && ATOM_equal (m01, other.m01) && ATOM_equal (m02, other.m02) && ATOM_equal(m03, other.m03) &&
         ATOM_equal (m10, other.m10) && ATOM_equal (m11, other.m11) && ATOM_equal (m12, other.m12) && ATOM_equal(m13, other.m13) &&
         ATOM_equal (m20, other.m20) && ATOM_equal (m21, other.m21) && ATOM_equal (m22, other.m22) && ATOM_equal(m23, other.m23) &&
         ATOM_equal (m30, other.m30) && ATOM_equal (m31, other.m31) && ATOM_equal (m32, other.m32) && ATOM_equal(m33, other.m33);
}

inline ATOM_Vector4f ATOM_Matrix4x4f::getRow (int row) const
{
  return ATOM_Vector4f(m[row*4], m[row*4+1], m[row*4+2], m[row*4+3]);
}

inline ATOM_Vector4f ATOM_Matrix4x4f::getCol (int col) const
{
  return ATOM_Vector4f (m[col], m[col+4], m[col+8], m[col+12]);
}

inline void ATOM_Matrix4x4f::getRow (int row, ATOM_Vector4f &v) const
{
  v.set (m[row*4], m[row*4+1], m[row*4+2], m[row*4+3]);
}

inline void ATOM_Matrix4x4f::getCol (int col, ATOM_Vector4f &v) const
{
  v.set (m[col], m[col+4], m[col+8], m[col+12]);
}

inline void ATOM_Matrix4x4f::setRow (int row, const ATOM_Vector4f &v)
{
  m[row*4] = v.x;
  m[row*4+1] = v.y;
  m[row*4+2] = v.z;
  m[row*4+3] = v.w;
}

inline void ATOM_Matrix4x4f::setCol (int col, const ATOM_Vector4f &v)
{
  m[col] = v.x;
  m[col+4] = v.y;
  m[col+8] = v.z;
  m[col+12] = v.w;
}

inline void ATOM_Matrix4x4f::setRow (int row, float x, float y, float z, float w)
{
  m[row*4] = x;
  m[row*4+1] = y;
  m[row*4+2] = z;
  m[row*4+3] = w;
}

inline void ATOM_Matrix4x4f::setCol (int col, float x, float y, float z, float w)
{
  m[col] = x;
  m[col+4] = y;
  m[col+8] = z;
  m[col+12] = w;
}

inline void ATOM_Matrix4x4f::setRow (int row, const ATOM_Vector3f &v, float f)
{
  m[row*4] = v.x;
  m[row*4+1] = v.y;
  m[row*4+2] = v.z;
  m[row*4+3] = f;
}

inline void ATOM_Matrix4x4f::setCol (int col, const ATOM_Vector3f &v, float f)
{
  m[col] = v.x;
  m[col+4] = v.y;
  m[col+8] = v.z;
  m[col+12] = f;
}

inline ATOM_Vector3f ATOM_Matrix4x4f::getRow3 (int row) const
{
  return ATOM_Vector3f (m[row*4], m[row*4+1], m[row*4+2]);
}

inline ATOM_Vector3f ATOM_Matrix4x4f::getCol3 (int col) const
{
  return ATOM_Vector3f (m[col], m[col+4], m[col+8]);
}

inline void ATOM_Matrix4x4f::getRow3 (int row, ATOM_Vector3f &v) const
{
  v.set (m[row*4], m[row*4+1], m[row*4+2]);
}

inline void ATOM_Matrix4x4f::getCol3 (int col, ATOM_Vector3f &v) const
{
  v.set (m[col], m[col+4], m[col+8]);
}

inline void ATOM_Matrix4x4f::setRow3 (int row, const ATOM_Vector3f &v)
{
  m[row*4] = v.x;
  m[row*4+1] = v.y;
  m[row*4+2] = v.z;
}

inline void ATOM_Matrix4x4f::setCol3 (int col, const ATOM_Vector3f &v)
{
  m[col] = v.x;
  m[col+4] = v.y;
  m[col+8] = v.z;
}

inline void ATOM_Matrix4x4f::setRow3 (int row, float x, float y, float z)
{
  m[row*4] = x;
  m[row*4+1] = y;
  m[row*4+2] = z;
}

inline void ATOM_Matrix4x4f::setCol3 (int col, float x, float y, float z)
{
  m[col] = x;
  m[col+4] = y;
  m[col+8] = z;
}

inline void ATOM_Matrix4x4f::fill (float val)
{
  for (int i = 0; i < 16; ++i)
  {
	  m[i] = val;
  }
}

inline void ATOM_Matrix4x4f::set (float m00_, float m01_, float m02_, float m03_,
                             float m10_, float m11_, float m12_, float m13_,
                             float m20_, float m21_, float m22_, float m23_,
                             float m30_, float m31_, float m32_, float m33_)
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
  m30 = m30_;
  m31 = m31_;
  m32 = m32_;
  m33 = m33_;
}

inline ATOM_Matrix3x3f ATOM_Matrix4x4f::getUpper3 (void) const
{
  return ATOM_Matrix3x3f (m00, m01, m02, m10, m11, m12, m20, m21, m22);
}

inline void ATOM_Matrix4x4f::setUpper3 (const ATOM_Matrix3x3f &upper3)
{
  m00 = upper3.m00;
  m01 = upper3.m01;
  m02 = upper3.m02;
  m10 = upper3.m10;
  m11 = upper3.m11;
  m12 = upper3.m12;
  m20 = upper3.m20;
  m21 = upper3.m21;
  m22 = upper3.m22;
}

inline float ATOM_Matrix4x4f::operator () (int row, int col) const
{
  return m[4 * row + col];
}

inline float & ATOM_Matrix4x4f::operator () (int row, int col)
{
  return m[4 * row + col];
}

inline bool ATOM_Matrix4x4f::isNAN (void) const
{
  return ATOM_nan(m[0]) || ATOM_nan(m[1]) || ATOM_nan(m[2]) || ATOM_nan(m[3]) || 
         ATOM_nan(m[4]) || ATOM_nan(m[5]) || ATOM_nan(m[6]) || ATOM_nan(m[7]) || 
         ATOM_nan(m[8]) || ATOM_nan(m[9]) || ATOM_nan(m[10]) || ATOM_nan(m[11]) ||
         ATOM_nan(m[12]) || ATOM_nan(m[13]) || ATOM_nan(m[14]) || ATOM_nan(m[15]);
}

inline void ATOM_Matrix4x4f::decompose (ATOM_Vector3f &translate, ATOM_Matrix4x4f &rotation, ATOM_Vector3f &scale) const
{
	scale.x = ATOM_sqrt(m00 * m00 + m01 * m01 + m02 * m02);
	scale.y = ATOM_sqrt(m10 * m10 + m11 * m11 + m12 * m12);
	scale.z = ATOM_sqrt(m20 * m20 + m21 * m21 + m22 * m22);

	rotation.m00 = m00 / scale.x;
	rotation.m01 = m01 / scale.x;
	rotation.m02 = m02 / scale.x;
	rotation.m10 = m10 / scale.y;
	rotation.m11 = m11 / scale.y;
	rotation.m12 = m12 / scale.y;
	rotation.m20 = m20 / scale.z;
	rotation.m21 = m21 / scale.z;
	rotation.m22 = m22 / scale.z;

	translate.x = m30;
	translate.y = m31;
	translate.z = m32;
}

inline void ATOM_Matrix4x4f::decompose (ATOM_Vector3f &translate, ATOM_Matrix3x3f &rotation, ATOM_Vector3f &scale) const
{
	scale.x = ATOM_sqrt(m00 * m00 + m01 * m01 + m02 * m02);
	scale.y = ATOM_sqrt(m10 * m10 + m11 * m11 + m12 * m12);
	scale.z = ATOM_sqrt(m20 * m20 + m21 * m21 + m22 * m22);

	rotation.m00 = m00 / scale.x;
	rotation.m01 = m01 / scale.x;
	rotation.m02 = m02 / scale.x;
	rotation.m10 = m10 / scale.y;
	rotation.m11 = m11 / scale.y;
	rotation.m12 = m12 / scale.y;
	rotation.m20 = m20 / scale.z;
	rotation.m21 = m21 / scale.z;
	rotation.m22 = m22 / scale.z;

	translate.x = m30;
	translate.y = m31;
	translate.z = m32;
}

inline void ATOM_Matrix4x4f::toEulerXYZ (float &x, float &y, float &z) const
{
  y = ATOM_asin (m20);

  if (y < ATOM_HalfPi)
  {
	  if (y > -ATOM_HalfPi)
	  {
		  x = ATOM_atan2 (-m21, m22);
		  z = ATOM_atan2 (-m10, m00);
	  }
	  else
	  {
		  float t = ATOM_atan2 (m01, m11);
		  z = 0.f;
		  x = z - t;
	  }
  }
  else
  {
	  float t = ATOM_atan2 (m01, m11);
	  z = 0.f;
	  x = t - z;
  }
}

inline float ATOM_Matrix4x4f::getDeterminant (void) const
{
  return (m00 * m11 - m01 * m10) * (m22 * m33 - m23 * m32)
        -(m00 * m12 - m02 * m10) * (m21 * m33 - m23 * m31)
        +(m00 * m13 - m03 * m10) * (m21 * m32 - m22 * m31)
        +(m01 * m12 - m02 * m11) * (m20 * m33 - m23 * m30)
        -(m01 * m13 - m03 * m11) * (m20 * m32 - m22 * m30)
        +(m02 * m13 - m03 * m12) * (m20 * m31 - m21 * m30);
}

inline void ATOM_Matrix4x4f::transpose (void)
{
  float tmp;

  tmp = m01;
  m01 = m10;
  m10 = tmp;

  tmp = m02;
  m02 = m20;
  m20 = tmp;

  tmp = m03;
  m03 = m30;
  m30 = tmp;

  tmp = m12;
  m12 = m21;
  m21 = tmp;

  tmp = m13;
  m13 = m31;
  m31 = tmp;

  tmp = m23;
  m23 = m32;
  m32 = tmp;
}

inline void ATOM_Matrix4x4f::transposeFrom (const ATOM_Matrix4x4f &other)
{
  m00 = other.m00;
  m01 = other.m10;
  m02 = other.m20;
  m03 = other.m30;
  m10 = other.m01;
  m11 = other.m11;
  m12 = other.m21;
  m13 = other.m31;
  m20 = other.m02;
  m21 = other.m12;
  m22 = other.m22;
  m23 = other.m32;
  m30 = other.m03;
  m31 = other.m13;
  m32 = other.m23;
  m33 = other.m33;
}

inline void ATOM_Matrix4x4f::invert (void)
{
#if 0
    float* src = m;

    __m128 minor0, minor1, minor2, minor3;
    __m128 row0, row1, row2, row3;
    __m128 det, tmp1;

	tmp1 = _mm_set_ps (src[5], src[4], src[1], src[0]);
	row1 = _mm_set_ps (src[13], src[12], src[9], src[8]);
    //tmp1 = _mm_loadh_pi(_mm_loadl_pi(tmp1, (__m64*)(src)), (__m64*)(src+ 4));
    //row1 = _mm_loadh_pi(_mm_loadl_pi(row1, (__m64*)(src+8)), (__m64*)(src+12));

    row0 = _mm_shuffle_ps(tmp1, row1, 0x88);
    row1 = _mm_shuffle_ps(row1, tmp1, 0xDD);

	tmp1 = _mm_set_ps (src[7], src[6], src[3], src[2]);
	row3 = _mm_set_ps (src[15], src[14], src[11], src[10]);
    //tmp1 = _mm_loadh_pi(_mm_loadl_pi(tmp1, (__m64*)(src+ 2)), (__m64*)(src+ 6));
    //row3 = _mm_loadh_pi(_mm_loadl_pi(row3, (__m64*)(src+10)), (__m64*)(src+14));

    row2 = _mm_shuffle_ps(tmp1, row3, 0x88);
    row3 = _mm_shuffle_ps(row3, tmp1, 0xDD);

    tmp1 = _mm_mul_ps(row2, row3);
    tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);

    minor0 = _mm_mul_ps(row1, tmp1);
    minor1 = _mm_mul_ps(row0, tmp1);

    tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);

    minor0 = _mm_sub_ps(_mm_mul_ps(row1, tmp1), minor0);
    minor1 = _mm_sub_ps(_mm_mul_ps(row0, tmp1), minor1);
    minor1 = _mm_shuffle_ps(minor1, minor1, 0x4E);

    tmp1 = _mm_mul_ps(row1, row2);
    tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);

    minor0 = _mm_add_ps(_mm_mul_ps(row3, tmp1), minor0);
    minor3 = _mm_mul_ps(row0, tmp1);

    tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
    
    minor0 = _mm_sub_ps(minor0, _mm_mul_ps(row3, tmp1));
    minor3 = _mm_sub_ps(_mm_mul_ps(row0, tmp1), minor3);
    minor3 = _mm_shuffle_ps(minor3, minor3, 0x4E);

    tmp1 = _mm_mul_ps(_mm_shuffle_ps(row1, row1, 0x4E), row3);
    tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
    row2 = _mm_shuffle_ps(row2, row2, 0x4E);

    minor0 = _mm_add_ps(_mm_mul_ps(row2, tmp1), minor0);
    minor2 = _mm_mul_ps(row0, tmp1);

    tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);

    minor0 = _mm_sub_ps(minor0, _mm_mul_ps(row2, tmp1));
    minor2 = _mm_sub_ps(_mm_mul_ps(row0, tmp1), minor2);
    minor2 = _mm_shuffle_ps(minor2, minor2, 0x4E);

    tmp1 = _mm_mul_ps(row0, row1);
    tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);

    minor2 = _mm_add_ps(_mm_mul_ps(row3, tmp1), minor2);
    minor3 = _mm_sub_ps(_mm_mul_ps(row2, tmp1), minor3);

    tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);

    minor2 = _mm_sub_ps(_mm_mul_ps(row3, tmp1), minor2);
    minor3 = _mm_sub_ps(minor3, _mm_mul_ps(row2, tmp1));

    tmp1 = _mm_mul_ps(row0, row3);
    tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);

    minor1 = _mm_sub_ps(minor1, _mm_mul_ps(row2, tmp1));
    minor2 = _mm_add_ps(_mm_mul_ps(row1, tmp1), minor2);

    tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);

    minor1 = _mm_add_ps(_mm_mul_ps(row2, tmp1), minor1);
    minor2 = _mm_sub_ps(minor2, _mm_mul_ps(row1, tmp1));

    tmp1 = _mm_mul_ps(row0, row2);
    tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);

    minor1 = _mm_add_ps(_mm_mul_ps(row3, tmp1), minor1);
    minor3 = _mm_sub_ps(minor3, _mm_mul_ps(row1, tmp1));

    tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);

    minor1 = _mm_sub_ps(minor1, _mm_mul_ps(row3, tmp1));
    minor3 = _mm_add_ps(_mm_mul_ps(row1, tmp1), minor3);

    det = _mm_mul_ps(row0, minor0);
    det = _mm_add_ps(_mm_shuffle_ps(det, det, 0x4E), det);
    det = _mm_add_ss(_mm_shuffle_ps(det, det, 0xB1), det);
    tmp1 = _mm_rcp_ss(det);

    det = _mm_sub_ss(_mm_add_ss(tmp1, tmp1), _mm_mul_ss(det, _mm_mul_ss(tmp1, tmp1)));
    det = _mm_shuffle_ps(det, det, 0x00);

    minor0 = _mm_mul_ps(det, minor0);
    _mm_storel_pi((__m64*)(src), minor0);
    _mm_storeh_pi((__m64*)(src+2), minor0);

    minor1 = _mm_mul_ps(det, minor1);
    _mm_storel_pi((__m64*)(src+4), minor1);
    _mm_storeh_pi((__m64*)(src+6), minor1);

    minor2 = _mm_mul_ps(det, minor2);
    _mm_storel_pi((__m64*)(src+ 8), minor2);
    _mm_storeh_pi((__m64*)(src+10), minor2);

    minor3 = _mm_mul_ps(det, minor3);
    _mm_storel_pi((__m64*)(src+12), minor3);
    _mm_storeh_pi((__m64*)(src+14), minor3);
#else
	float v0 = m20 * m31 - m21 * m30;
	float v1 = m20 * m32 - m22 * m30;
	float v2 = m20 * m33 - m23 * m30;
	float v3 = m21 * m32 - m22 * m31;
	float v4 = m21 * m33 - m23 * m31;
	float v5 = m22 * m33 - m23 * m32;
	
	float t00 =  (v5 * m11 - v4 * m12 + v3 * m13);
	float t10 = -(v5 * m10 - v2 * m12 + v1 * m13);
	float t20 =  (v4 * m10 - v2 * m11 + v0 * m13);
	float t30 = -(v3 * m10 - v1 * m11 + v0 * m12);
	
	float invDet = 1.f / (t00 * m00 + t10 * m01 + t20 * m02 + t30 * m03);
	float d00 = t00 * invDet;
	float d10 = t10 * invDet;
	float d20 = t20 * invDet;
	float d30 = t30 * invDet;
	
	float d01 = -(v5 * m01 - v4 * m02 + v3 * m03) * invDet;
	float d11 =  (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
	float d21 = -(v4 * m00 - v2 * m01 + v0 * m03) * invDet;
	float d31 =  (v3 * m00 - v1 * m01 + v0 * m02) * invDet;
	
	v0 = m10 * m31 - m11 * m30;
	v1 = m10 * m32 - m12 * m30;
	v2 = m10 * m33 - m13 * m30;
	v3 = m11 * m32 - m12 * m31;
	v4 = m11 * m33 - m13 * m31;
	v5 = m12 * m33 - m13 * m32;
	
	float d02 =  (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
	float d12 = -(v5 * m00 - v2 * m02 + v1 * m03) * invDet;
	float d22 =  (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
	float d32 = -(v3 * m00 - v1 * m01 + v0 * m02) * invDet;
	
	v0 = m21 * m10 - m20 * m11;
	v1 = m22 * m10 - m20 * m12;
	v2 = m23 * m10 - m20 * m13;
	v3 = m22 * m11 - m21 * m12;
	v4 = m23 * m11 - m21 * m13;
	v5 = m23 * m12 - m22 * m13;
	
	float d03 = -(v5 * m01 - v4 * m02 + v3 * m03) * invDet;
	float d13 =  (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
	float d23 = -(v4 * m00 - v2 * m01 + v0 * m03) * invDet;
	float d33 =  (v3 * m00 - v1 * m01 + v0 * m02) * invDet;
	
	set (
		d00, d01, d02, d03,
		d10, d11, d12, d13,
		d20, d21, d22, d23,
		d30, d31, d32, d33);
#endif
}

inline void ATOM_Matrix4x4f::invertFrom (const ATOM_Matrix4x4f &other)
{
  *this = other;
  invert ();
}

inline void ATOM_Matrix4x4f::invertAffine (void)
{
  float s = getDeterminant ();
  s = 1.f / s;

  float _m00 = s * ((m11 * m22) - (m12 * m21));
  float _m01 = s * ((m21 * m02) - (m22 * m01));
  float _m02 = s * ((m01 * m12) - (m02 * m11));
  float _m10 = s * ((m12 * m20) - (m10 * m22));
  float _m11 = s * ((m22 * m00) - (m20 * m02));
  float _m12 = s * ((m02 * m10) - (m00 * m12));
  float _m20 = s * ((m10 * m21) - (m11 * m20));
  float _m21 = s * ((m20 * m01) - (m21 * m00));
  float _m22 = s * ((m00 * m11) - (m01 * m10));
  float _m30 = s * (m10*(m22*m31 - m21*m32) + m11*(m20*m32 - m22*m30) + m12*(m21*m30 - m20*m31));
  float _m31 = s * (m20*(m02*m31 - m01*m32) + m21*(m00*m32 - m02*m30) + m22*(m01*m30 - m00*m31));
  float _m32 = s * (m30*(m02*m11 - m01*m12) + m31*(m00*m12 - m02*m10) + m32*(m01*m10 - m00*m11));

  m00 = _m00;
  m01 = _m01;
  m02 = _m02;
  m03 = 0.f;
  m10 = _m10;
  m11 = _m11;
  m12 = _m12;
  m13 = 0.f;
  m20 = _m20;
  m21 = _m21;
  m22 = _m22;
  m23 = 0.f;
  m30 = _m30;
  m31 = _m31;
  m32 = _m32;
  m33 = 1.f;
}

inline void ATOM_Matrix4x4f::invertAffineFrom (const ATOM_Matrix4x4f &other)
{
  *this = other;
  invertAffine ();
}

inline void ATOM_Matrix4x4f::invertTranspose (void)
{
  invert ();
  transpose ();
}

inline void ATOM_Matrix4x4f::invertTransposeFrom (const ATOM_Matrix4x4f &other)
{
  *this = other;
  invertTranspose ();
}

inline void ATOM_Matrix4x4f::invertTransposeAffine (void)
{
  float s = getDeterminant ();
  s = 1.f / s;

  float _m00 = s * ((m11 * m22) - (m12 * m21));
  float _m10 = s * ((m21 * m02) - (m22 * m01));
  float _m20 = s * ((m01 * m12) - (m02 * m11));
  float _m01 = s * ((m12 * m20) - (m10 * m22));
  float _m11 = s * ((m22 * m00) - (m20 * m02));
  float _m21 = s * ((m02 * m10) - (m00 * m12));
  float _m02 = s * ((m10 * m21) - (m11 * m20));
  float _m12 = s * ((m20 * m01) - (m21 * m00));
  float _m22 = s * ((m00 * m11) - (m01 * m10));
  float _m03 = s * (m10*(m22*m31 - m21*m32) + m11*(m20*m32 - m22*m30) + m12*(m21*m30 - m20*m31));
  float _m13 = s * (m20*(m02*m31 - m01*m32) + m21*(m00*m32 - m02*m30) + m22*(m01*m30 - m00*m31));
  float _m23 = s * (m30*(m02*m11 - m01*m12) + m31*(m00*m12 - m02*m10) + m32*(m01*m10 - m00*m11));

  m00 = _m00;
  m01 = _m01;
  m02 = _m02;
  m03 = _m03;
  m10 = _m10;
  m11 = _m11;
  m12 = _m12;
  m13 = _m13;
  m20 = _m20;
  m21 = _m21;
  m22 = _m22;
  m23 = _m23;
  m30 = 0.f;
  m31 = 0.f;
  m32 = 0.f;
  m33 = 1.f;
}

inline void ATOM_Matrix4x4f::invertTransposeAffineFrom (const ATOM_Matrix4x4f &other)
{
  *this = other;
  invertTransposeAffine ();
}

inline ATOM_Matrix4x4f operator >> (const ATOM_Matrix4x4f &m1, const ATOM_Matrix4x4f &m2)
{
  ATOM_Matrix4x4f m;

  m.m00 = m2.m00 * m1.m00 + m2.m01 * m1.m10 + m2.m02 * m1.m20 + m2.m03 * m1.m30;
  m.m01 = m2.m00 * m1.m01 + m2.m01 * m1.m11 + m2.m02 * m1.m21 + m2.m03 * m1.m31;
  m.m02 = m2.m00 * m1.m02 + m2.m01 * m1.m12 + m2.m02 * m1.m22 + m2.m03 * m1.m32;
  m.m03 = m2.m00 * m1.m03 + m2.m01 * m1.m13 + m2.m02 * m1.m23 + m2.m03 * m1.m33;
  m.m10 = m2.m10 * m1.m00 + m2.m11 * m1.m10 + m2.m12 * m1.m20 + m2.m13 * m1.m30;
  m.m11 = m2.m10 * m1.m01 + m2.m11 * m1.m11 + m2.m12 * m1.m21 + m2.m13 * m1.m31;
  m.m12 = m2.m10 * m1.m02 + m2.m11 * m1.m12 + m2.m12 * m1.m22 + m2.m13 * m1.m32;
  m.m13 = m2.m10 * m1.m03 + m2.m11 * m1.m13 + m2.m12 * m1.m23 + m2.m13 * m1.m33;
  m.m20 = m2.m20 * m1.m00 + m2.m21 * m1.m10 + m2.m22 * m1.m20 + m2.m23 * m1.m30;
  m.m21 = m2.m20 * m1.m01 + m2.m21 * m1.m11 + m2.m22 * m1.m21 + m2.m23 * m1.m31;
  m.m22 = m2.m20 * m1.m02 + m2.m21 * m1.m12 + m2.m22 * m1.m22 + m2.m23 * m1.m32;
  m.m23 = m2.m20 * m1.m03 + m2.m21 * m1.m13 + m2.m22 * m1.m23 + m2.m23 * m1.m33;
  m.m30 = m2.m30 * m1.m00 + m2.m31 * m1.m10 + m2.m32 * m1.m20 + m2.m33 * m1.m30;
  m.m31 = m2.m30 * m1.m01 + m2.m31 * m1.m11 + m2.m32 * m1.m21 + m2.m33 * m1.m31;
  m.m32 = m2.m30 * m1.m02 + m2.m31 * m1.m12 + m2.m32 * m1.m22 + m2.m33 * m1.m32;
  m.m33 = m2.m30 * m1.m03 + m2.m31 * m1.m13 + m2.m32 * m1.m23 + m2.m33 * m1.m33;

  return m;
}

inline ATOM_Matrix4x4f operator << (const ATOM_Matrix4x4f &m1, const ATOM_Matrix4x4f &m2)
{
  return operator >> (m2, m1);
}

inline ATOM_Vector4f operator >> (const ATOM_Matrix4x4f &m, const ATOM_Vector4f &v)
{
  return m.transformPoint (v);
}

inline ATOM_Vector4f operator << (const ATOM_Vector4f &v, const ATOM_Matrix4x4f &m)
{
  return m.transformPoint (v);
}

inline ATOM_Vector3f operator >> (const ATOM_Matrix4x4f &m, const ATOM_Vector3f &v)
{
  return m.transformPoint (v);
}

inline ATOM_Vector3f operator << (const ATOM_Vector3f &v, const ATOM_Matrix4x4f &m)
{
  return m.transformPoint (v);
}

inline ATOM_Matrix4x4f operator + (const ATOM_Matrix4x4f &m1, const ATOM_Matrix4x4f &m2)
{
  return ATOM_Matrix4x4f(
	  m1.m00+m2.m00, m1.m01+m2.m01, m1.m02+m2.m02, m1.m03+m2.m03,
	  m1.m10+m2.m10, m1.m11+m2.m11, m1.m12+m2.m12, m1.m13+m2.m13,
	  m1.m20+m2.m20, m1.m21+m2.m21, m1.m22+m2.m22, m1.m23+m2.m23,
	  m1.m30+m2.m30, m1.m31+m2.m31, m1.m32+m2.m32, m1.m33+m2.m33
	  );
}

inline ATOM_Matrix4x4f operator - (const ATOM_Matrix4x4f &m1, const ATOM_Matrix4x4f &m2)
{
  return ATOM_Matrix4x4f(
	  m1.m00-m2.m00, m1.m01-m2.m01, m1.m02-m2.m02, m1.m03-m2.m03,
	  m1.m10-m2.m10, m1.m11-m2.m11, m1.m12-m2.m12, m1.m13-m2.m13,
	  m1.m20-m2.m20, m1.m21-m2.m21, m1.m22-m2.m22, m1.m23-m2.m23,
	  m1.m30-m2.m30, m1.m31-m2.m31, m1.m32-m2.m32, m1.m33-m2.m33
	  );
}

inline ATOM_Matrix4x4f operator * (const ATOM_Matrix4x4f &m1, const ATOM_Matrix4x4f &m2)
{
  return ATOM_Matrix4x4f(
	  m1.m00*m2.m00, m1.m01*m2.m01, m1.m02*m2.m02, m1.m03*m2.m03,
	  m1.m10*m2.m10, m1.m11*m2.m11, m1.m12*m2.m12, m1.m13*m2.m13,
	  m1.m20*m2.m20, m1.m21*m2.m21, m1.m22*m2.m22, m1.m23*m2.m23,
	  m1.m30*m2.m30, m1.m31*m2.m31, m1.m32*m2.m32, m1.m33*m2.m33
	  );
}

inline ATOM_Matrix4x4f operator * (const ATOM_Matrix4x4f &m, float f)
{
  return ATOM_Matrix4x4f(
	  m.m00*f, m.m01*f, m.m02*f, m.m03*f,
	  m.m10*f, m.m11*f, m.m12*f, m.m13*f,
	  m.m20*f, m.m21*f, m.m22*f, m.m23*f,
	  m.m30*f, m.m31*f, m.m32*f, m.m33*f
	  );
}

inline ATOM_Matrix4x4f operator * (float f, const ATOM_Matrix4x4f &m)
{
  return operator * (m, f);
}

inline ATOM_Matrix4x4f operator / (const ATOM_Matrix4x4f &m1, const ATOM_Matrix4x4f &m2)
{
  return ATOM_Matrix4x4f(
	  m1.m00/m2.m00, m1.m01/m2.m01, m1.m02/m2.m02, m1.m03/m2.m03,
	  m1.m10/m2.m10, m1.m11/m2.m11, m1.m12/m2.m12, m1.m13/m2.m13,
	  m1.m20/m2.m20, m1.m21/m2.m21, m1.m22/m2.m22, m1.m23/m2.m23,
	  m1.m30/m2.m30, m1.m31/m2.m31, m1.m32/m2.m32, m1.m33/m2.m33
	  );
}

inline ATOM_Matrix4x4f operator / (const ATOM_Matrix4x4f &m, float f)
{
  return operator * (m, 1.f/f);
}

inline bool operator == (const ATOM_Matrix4x4f &m1, const ATOM_Matrix4x4f &m2)
{
  return m1.m00 == m2.m00 && m1.m01 == m2.m01 && m1.m02 == m2.m02 && m1.m03 == m2.m03 &&
         m1.m10 == m2.m10 && m1.m11 == m2.m11 && m1.m12 == m2.m12 && m1.m13 == m2.m13 &&
         m1.m20 == m2.m20 && m1.m21 == m2.m21 && m1.m22 == m2.m22 && m1.m23 == m2.m23 &&
         m1.m30 == m2.m30 && m1.m31 == m2.m31 && m1.m32 == m2.m32 && m1.m33 == m2.m33;
}

inline bool operator != (const ATOM_Matrix4x4f &m1, const ATOM_Matrix4x4f &m2)
{
  return ! operator == (m1, m2);
}

inline ATOM_Matrix4x4f & ATOM_Matrix4x4f::operator >>= (const ATOM_Matrix4x4f &other)
{
  *this = (*this >> other);
  return *this;
}

inline ATOM_Matrix4x4f & ATOM_Matrix4x4f::operator <<= (const ATOM_Matrix4x4f &other)
{
  *this = (*this) << other;
  return *this;
}

inline ATOM_Matrix4x4f & ATOM_Matrix4x4f::operator += (const ATOM_Matrix4x4f &other)
{
  m00 += other.m00;
  m01 += other.m01;
  m02 += other.m02;
  m03 += other.m03;
  m10 += other.m10;
  m11 += other.m11;
  m12 += other.m12;
  m13 += other.m13;
  m20 += other.m20;
  m21 += other.m21;
  m22 += other.m22;
  m23 += other.m23;
  m30 += other.m30;
  m31 += other.m31;
  m32 += other.m32;
  m33 += other.m33;
  return *this;
}

inline ATOM_Matrix4x4f & ATOM_Matrix4x4f::operator -= (const ATOM_Matrix4x4f &other)
{
  m00 -= other.m00;
  m01 -= other.m01;
  m02 -= other.m02;
  m03 -= other.m03;
  m10 -= other.m10;
  m11 -= other.m11;
  m12 -= other.m12;
  m13 -= other.m13;
  m20 -= other.m20;
  m21 -= other.m21;
  m22 -= other.m22;
  m23 -= other.m23;
  m30 -= other.m30;
  m31 -= other.m31;
  m32 -= other.m32;
  m33 -= other.m33;
  return *this;
}

inline ATOM_Matrix4x4f & ATOM_Matrix4x4f::operator *= (const ATOM_Matrix4x4f &other)
{
  m00 *= other.m00;
  m01 *= other.m01;
  m02 *= other.m02;
  m03 *= other.m03;
  m10 *= other.m10;
  m11 *= other.m11;
  m12 *= other.m12;
  m13 *= other.m13;
  m20 *= other.m20;
  m21 *= other.m21;
  m22 *= other.m22;
  m23 *= other.m23;
  m30 *= other.m30;
  m31 *= other.m31;
  m32 *= other.m32;
  m33 *= other.m33;
  return *this;
}

inline ATOM_Matrix4x4f & ATOM_Matrix4x4f::operator *= (float f)
{
  m00 *= f;
  m01 *= f;
  m02 *= f;
  m03 *= f;
  m10 *= f;
  m11 *= f;
  m12 *= f;
  m13 *= f;
  m20 *= f;
  m21 *= f;
  m22 *= f;
  m23 *= f;
  m30 *= f;
  m31 *= f;
  m32 *= f;
  m33 *= f;
  return *this;
}

inline ATOM_Matrix4x4f & ATOM_Matrix4x4f::operator /= (const ATOM_Matrix4x4f &other)
{
  m00 /= other.m00;
  m01 /= other.m01;
  m02 /= other.m02;
  m03 /= other.m03;
  m10 /= other.m10;
  m11 /= other.m11;
  m12 /= other.m12;
  m13 /= other.m13;
  m20 /= other.m20;
  m21 /= other.m21;
  m22 /= other.m22;
  m23 /= other.m23;
  m30 /= other.m30;
  m31 /= other.m31;
  m32 /= other.m32;
  m33 /= other.m33;
  return *this;
}

inline ATOM_Matrix4x4f & ATOM_Matrix4x4f::operator /= (float f)
{
  operator *= (1.f/f);
  return *this;
}

inline ATOM_Vector3f ATOM_Matrix4x4f::transformPoint (const ATOM_Vector3f &v) const
{
  float x = v.x * m00 + v.y * m10 + v.z * m20 + m30;
  float y = v.x * m01 + v.y * m11 + v.z * m21 + m31;
  float z = v.x * m02 + v.y * m12 + v.z * m22 + m32;
  return ATOM_Vector3f (x, y, z);
}

inline ATOM_Vector3f ATOM_Matrix4x4f::transformVector (const ATOM_Vector3f &v) const
{
  float x = v.x * m00 + v.y * m10 + v.z * m20;
  float y = v.x * m01 + v.y * m11 + v.z * m21;
  float z = v.x * m02 + v.y * m12 + v.z * m22;
  return ATOM_Vector3f (x, y, z);
}

inline ATOM_Vector4f ATOM_Matrix4x4f::transformPoint (const ATOM_Vector4f &v) const
{
  return ATOM_Vector4f(
	  v.x * m00 + v.y * m10 + v.z * m20 + v.w * m30,
	  v.x * m01 + v.y * m11 + v.z * m21 + v.w * m31,
	  v.x * m02 + v.y * m12 + v.z * m22 + v.w * m32,
	  v.x * m03 + v.y * m13 + v.z * m23 + v.w * m33
	  );
}

inline ATOM_Vector4f ATOM_Matrix4x4f::transformVector (const ATOM_Vector4f &v) const
{
  return ATOM_Vector4f(
	  v.x * m00 + v.y * m10 + v.z * m20,
	  v.x * m01 + v.y * m11 + v.z * m21,
	  v.x * m02 + v.y * m12 + v.z * m22,
	  v.x * m03 + v.y * m13 + v.z * m23
	  );
}

inline void ATOM_Matrix4x4f::transformPointInplace (ATOM_Vector3f &v) const
{
  float x = v.x * m00 + v.y * m10 + v.z * m20 + m30;
  float y = v.x * m01 + v.y * m11 + v.z * m21 + m31;
  float z = v.x * m02 + v.y * m12 + v.z * m22 + m32;
  v.x = x;
  v.y = y;
  v.z = z;
}

inline void ATOM_Matrix4x4f::transformVectorInplace (ATOM_Vector3f &v) const
{
  float x = v.x * m00 + v.y * m10 + v.z * m20;
  float y = v.x * m01 + v.y * m11 + v.z * m21;
  float z = v.x * m02 + v.y * m12 + v.z * m22;
  v.x = x;
  v.y = y;
  v.z = z;
}

inline void ATOM_Matrix4x4f::transformPointInplace (ATOM_Vector4f &v) const
{
  v = transformPoint (v);
}

inline void ATOM_Matrix4x4f::transformVectorInplace (ATOM_Vector4f &v) const
{
  v = transformVector (v);
}

inline ATOM_Vector3f ATOM_Matrix4x4f::transformPointAffine (const ATOM_Vector3f &v) const
{
  return transformPoint (v);
}

inline ATOM_Vector3f ATOM_Matrix4x4f::transformVectorAffine (const ATOM_Vector3f &v) const
{
  return transformVector (v);
}

inline ATOM_Vector4f ATOM_Matrix4x4f::transformPointAffine (const ATOM_Vector4f &v) const
{
  return ATOM_Vector4f(
	  v.x * m00 + v.y * m10 + v.z * m20 + m30,
	  v.x * m01 + v.y * m11 + v.z * m21 + m31,
	  v.x * m02 + v.y * m12 + v.z * m22 + m32,
	  v.x * m03 + v.y * m13 + v.z * m23 + m33
	  );
}

inline ATOM_Vector4f ATOM_Matrix4x4f::transformVectorAffine (const ATOM_Vector4f &v) const
{
  return ATOM_Vector4f(
	  v.x * m00 + v.y * m10 + v.z * m20,
	  v.x * m01 + v.y * m11 + v.z * m21,
	  v.x * m02 + v.y * m12 + v.z * m22,
	  0.f
	  );
}

inline void ATOM_Matrix4x4f::transformPointAffineInplace (ATOM_Vector3f &v) const
{
  transformPointInplace (v);
}

inline void ATOM_Matrix4x4f::transformVectorAffineInplace (ATOM_Vector3f &v) const
{
  transformVectorInplace (v);
}

inline void ATOM_Matrix4x4f::transformPointAffineInplace (ATOM_Vector4f &v) const
{
  v = transformPointAffine (v);
}

inline void ATOM_Matrix4x4f::transformVectorAffineInplace (ATOM_Vector4f &v) const
{
  v = transformVectorAffine (v);
}

inline void ATOM_Matrix4x4f::makeIdentity (void)
{
  memcpy (m, ATOM_Matrix4x4f::getIdentityMatrix().m, sizeof(ATOM_Matrix4x4f));
}

inline const ATOM_Matrix4x4f &ATOM_Matrix4x4f::getIdentityMatrix (void)
{
  static const ATOM_Matrix4x4f identMatrix(1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f);
  return identMatrix;
}

inline void ATOM_Matrix4x4f::makeRotateAngleAxis (float angle, const ATOM_Vector3f &axis)
{
  setUpper3 (ATOM_Matrix3x3f::getRotateAngleAxisMatrix (angle, axis));

  m03 = 0.f;
  m13 = 0.f;
  m23 = 0.f;
  m30 = 0.f;
  m31 = 0.f;
  m32 = 0.f;
  m33 = 1.f;
}

inline void ATOM_Matrix4x4f::makeRotateAngleNormalizedAxis (float angle, const ATOM_Vector3f &normalizedAxis)
{
  setUpper3 (ATOM_Matrix3x3f::getRotateAngleNormalizedAxisMatrix (angle, normalizedAxis));

  m03 = 0.f;
  m13 = 0.f;
  m23 = 0.f;
  m30 = 0.f;
  m31 = 0.f;
  m32 = 0.f;
  m33 = 1.f;
}

inline ATOM_Matrix4x4f ATOM_Matrix4x4f::getRotateMatrixAngleAxis (float angle, const ATOM_Vector3f &axis)
{
  ATOM_Matrix4x4f m;
  m.makeRotateAngleAxis (angle, axis);
  return m;
}

inline ATOM_Matrix4x4f ATOM_Matrix4x4f::getRotateMatrixAngleNormalizedAxis (float angle, const ATOM_Vector3f &normalizedAxis)
{
  ATOM_Matrix4x4f m;
  m.makeRotateAngleNormalizedAxis (angle, normalizedAxis);
  return m;
}

inline void ATOM_Matrix4x4f::makeScale (const ATOM_Vector3f &scale)
{
  setUpper3 (ATOM_Matrix3x3f::getScaleMatrix (scale));

  m03 = 0.f;
  m13 = 0.f;
  m23 = 0.f;
  m30 = 0.f;
  m31 = 0.f;
  m32 = 0.f;
  m33 = 1.f;
}

inline void ATOM_Matrix4x4f::makeTranslate (const ATOM_Vector3f &translation)
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
  m30 = translation.x;
  m31 = translation.y;
  m32 = translation.z;
  m33 = 1.f;
}

inline ATOM_Matrix4x4f ATOM_Matrix4x4f::getTranslateMatrix (const ATOM_Vector3f &translation)
{
  ATOM_Matrix4x4f m;
  m.makeTranslate (translation);
  return m;
}

inline ATOM_Matrix4x4f ATOM_Matrix4x4f::getScaleMatrix (const ATOM_Vector3f &scale)
{
  ATOM_Matrix4x4f m;
  m.makeScale (scale);
  return m;
}

inline ATOM_Vector4f & ATOM_Vector4f::operator <<= (const ATOM_Matrix4x4f &m)
{
  m.transformPointInplace (*this);
  return *this;
}

inline ATOM_Vector3f & ATOM_Vector3f::operator <<= (const ATOM_Matrix4x4f &m)
{
  ATOM_Vector4f v(*this);
  m.transformPointInplace (v);
  v /= v.w;
  x = v.x;
  y = v.y;
  z = v.z;
  return *this;
}

inline void ATOM_Matrix4x4f::makeRotateX (float angle)
{
  setUpper3 (ATOM_Matrix3x3f::getRotateXMatrix (angle));

  m03 = 0.f;
  m13 = 0.f;
  m23 = 0.f;
  m30 = 0.f;
  m31 = 0.f;
  m32 = 0.f;
  m33 = 1.f;
}

inline ATOM_Matrix4x4f ATOM_Matrix4x4f::getRotateXMatrix (float angle)
{
  ATOM_Matrix4x4f m;
  m.makeRotateX (angle);
  return m;
}

inline void ATOM_Matrix4x4f::makeRotateY (float angle)
{
  setUpper3 (ATOM_Matrix3x3f::getRotateYMatrix (angle));

  m03 = 0.f;
  m13 = 0.f;
  m23 = 0.f;
  m30 = 0.f;
  m31 = 0.f;
  m32 = 0.f;
  m33 = 1.f;
}

inline ATOM_Matrix4x4f ATOM_Matrix4x4f::getRotateYMatrix (float angle)
{
  ATOM_Matrix4x4f m;
  m.makeRotateY (angle);
  return m;
}

inline void ATOM_Matrix4x4f::makeRotateZ (float angle)
{
  setUpper3 (ATOM_Matrix3x3f::getRotateZMatrix (angle));

  m03 = 0.f;
  m13 = 0.f;
  m23 = 0.f;
  m30 = 0.f;
  m31 = 0.f;
  m32 = 0.f;
  m33 = 1.f;
}

inline ATOM_Matrix4x4f ATOM_Matrix4x4f::getRotateZMatrix (float angle)
{
  ATOM_Matrix4x4f m;
  m.makeRotateZ (angle);
  return m;
}

inline void ATOM_Matrix4x4f::makeLookatLH (const ATOM_Vector3f &eye, const ATOM_Vector3f &to, const ATOM_Vector3f &up)
{
  ATOM_Vector3f zaxis = to - eye;
  zaxis.normalize ();
  ATOM_Vector3f xaxis = crossProduct (up, zaxis);
  xaxis.normalize ();
  ATOM_Vector3f yaxis = crossProduct (zaxis, xaxis);

  setRow (0, xaxis, 0.f);
  setRow (1, yaxis, 0.f);
  setRow (2, zaxis, 0.f);
  setRow (3, eye, 1.f);
}

inline void ATOM_Matrix4x4f::makeLookatRH (const ATOM_Vector3f &eye, const ATOM_Vector3f &to, const ATOM_Vector3f &up)
{
  ATOM_Vector3f zaxis = eye - to;
  zaxis.normalize ();
  ATOM_Vector3f xaxis = crossProduct (up, zaxis);
  xaxis.normalize ();
  ATOM_Vector3f yaxis = crossProduct (zaxis, xaxis);

  setRow (0, xaxis, 0.f);
  setRow (1, yaxis, 0.f);
  setRow (2, zaxis, 0.f);
  setRow (3, eye, 1.f);
}

inline ATOM_Matrix4x4f ATOM_Matrix4x4f::getLookatLHMatrix (const ATOM_Vector3f &eye, const ATOM_Vector3f &to, const ATOM_Vector3f &up)
{
  ATOM_Matrix4x4f m;
  m.makeLookatLH (eye, to, up);
  return m;
}

inline ATOM_Matrix4x4f ATOM_Matrix4x4f::getLookatRHMatrix (const ATOM_Vector3f &eye, const ATOM_Vector3f &to, const ATOM_Vector3f &up)
{
  ATOM_Matrix4x4f m;
  m.makeLookatRH (eye, to, up);
  return m;
}

inline void ATOM_Matrix4x4f::makePerspectiveFovLH (float fovY, float aspect, float znear, float zfar)
{
  float h = 1.f / ATOM_tan(fovY * 0.5f);
  float w = h / aspect;
  m00 = w;
  m01 = 0.f;
  m02 = 0.f;
  m03 = 0.f;
  m10 = 0.f;
  m11 = h;
  m12 = 0.f;
  m13 = 0.f;
  m20 = 0.f;
  m21 = 0.f;
  m22 = zfar / (zfar - znear);
  m23 = 1.f;
  m30 = 0.f;
  m31 = 0.f;
  m32 = -znear * m22;
  m33 = 0.f;
}

inline void ATOM_Matrix4x4f::makePerspectiveFovRH (float fovY, float aspect, float znear, float zfar)
{
  float h = 1.f / ATOM_tan(fovY * 0.5f);
  float w = h / aspect;
  m00 = w;
  m01 = 0.f;
  m02 = 0.f;
  m03 = 0.f;
  m10 = 0.f;
  m11 = h;
  m12 = 0.f;
  m13 = 0.f;
  m20 = 0.f;
  m21 = 0.f;
  m22 = zfar / (znear - zfar);
  m23 = -1.f;
  m30 = 0.f;
  m31 = 0.f;
  m32 = znear * m22;
  m33 = 0.f;
}

inline ATOM_Matrix4x4f ATOM_Matrix4x4f::getPerspectiveFovLHMatrix (float fovY, float aspect, float znear, float zfar)
{
  ATOM_Matrix4x4f m;
  m.makePerspectiveFovLH (fovY, aspect, znear, zfar);
  return m;
}

inline ATOM_Matrix4x4f ATOM_Matrix4x4f::getPerspectiveFovRHMatrix (float fovY, float aspect, float znear, float zfar)
{
  ATOM_Matrix4x4f m;
  m.makePerspectiveFovRH (fovY, aspect, znear, zfar);
  return m;
}

inline void ATOM_Matrix4x4f::makePerspectiveFrustumLH (float l, float r, float b, float t, float n, float f)
{
  m00 = 2.f * n / (r - l);
  m01 = 0.f;
  m02 = 0.f;
  m03 = 0.f;
  m10 = 0.f;
  m11 = 2.f * n / (t - b);
  m12 = 0.f;
  m13 = 0.f;
  m20 = (l + r) / (l - r);
  m21 = (t + b) / (b - t);
  m22 = f / (f - n);
  m23 = 1.f;
  m30 = 0.f;
  m31 = 0.f;
  m32 = n * f / (n - f);
  m33 = 0.f;
}

inline void ATOM_Matrix4x4f::makePerspectiveFrustumRH (float l, float r, float b, float t, float n, float f)
{
  m00 = 2.f * n / (r - l);
  m01 = 0.f;
  m02 = 0.f;
  m03 = 0.f;
  m10 = 0.f;
  m11 = 2.f * n / (t - b);
  m12 = 0.f;
  m13 = 0.f;
  m20 = (l + r) / (r - l);
  m21 = (t + b) / (t - b);
  m22 = f / (n - f);
  m23 = -1.f;
  m30 = 0.f;
  m31 = 0.f;
  m32 = n * f / (n - f);
  m33 = 0.f;
}

inline ATOM_Matrix4x4f ATOM_Matrix4x4f::getPerspectiveFrustumLHMatrix (float l, float r, float b, float t, float n, float f)
{
  ATOM_Matrix4x4f m;
  m.makePerspectiveFrustumLH (l, r, b, t, n, f);
  return m;
}

inline ATOM_Matrix4x4f ATOM_Matrix4x4f::getPerspectiveFrustumRHMatrix (float l, float r, float b, float t, float n, float f)
{
  ATOM_Matrix4x4f m;
  m.makePerspectiveFrustumRH (l, r, b, t, n, f);
  return m;
}

inline void ATOM_Matrix4x4f::makeOrthoLH (float w, float h, float znear, float zfar)
{
  m00 = 2.f / w;
  m01 = 0.f;
  m02 = 0.f;
  m03 = 0.f;
  m10 = 0.f;
  m11 = 2.f / h;
  m12 = 0.f;
  m13 = 0.f;
  m20 = 0.f;
  m21 = 0.f;
  m22 = 1.f / (zfar - znear);
  m23 = 0.f;
  m30 = 0.f;
  m31 = 0.f;
  m32 = -znear * m22;
  m33 = 1.f;
}

inline void ATOM_Matrix4x4f::makeOrthoRH (float w, float h, float znear, float zfar)
{
  m00 = 2.f / w;
  m01 = 0.f;
  m02 = 0.f;
  m03 = 0.f;
  m10 = 0.f;
  m11 = 2.f / h;
  m12 = 0.f;
  m13 = 0.f;
  m20 = 0.f;
  m21 = 0.f;
  m22 = 1.f / (znear - zfar);
  m23 = 0.f;
  m30 = 0.f;
  m31 = 0.f;
  m32 = znear * m22;
  m33 = 1.f;
}

inline ATOM_Matrix4x4f ATOM_Matrix4x4f::getOrthoLHMatrix (float w, float h, float znear, float zfar)
{
  ATOM_Matrix4x4f m;
  m.makeOrthoLH (w, h, znear, zfar);
  return m;
}

inline ATOM_Matrix4x4f ATOM_Matrix4x4f::getOrthoRHMatrix (float w, float h, float znear, float zfar)
{
  ATOM_Matrix4x4f m;
  m.makeOrthoRH (w, h, znear, zfar);
  return m;
}

inline void ATOM_Matrix4x4f::makeOrthoFrustumLH (float l, float r, float b, float t, float n, float f)
{
  m00 = 2.f / (r - l);
  m01 = 0.f;
  m02 = 0.f;
  m03 = 0.f;
  m10 = 0.f;
  m11 = 2.f / (t - b);
  m12 = 0.f;
  m13 = 0.f;
  m20 = 0.f;
  m21 = 0.f;
  m22 = 1.f / (f - n);
  m23 = 0.f;
  m30 = (l + r) / (l - r);
  m31 = (t + b) / (b - t);
  m32 = n / (n - f);
  m33 = 1.f;
}

inline void ATOM_Matrix4x4f::makeOrthoFrustumRH (float l, float r, float b, float t, float n, float f)
{
  m00 = 2.f / (r - l);
  m01 = 0.f;
  m02 = 0.f;
  m03 = 0.f;
  m10 = 0.f;
  m11 = 2.f / (t - b);
  m12 = 0.f;
  m13 = 0.f;
  m20 = 0.f;
  m21 = 0.f;
  m22 = 1.f / (n - f);
  m23 = 0.f;
  m30 = (l + r) / (l - r);
  m31 = (t + b) / (b - t);
  m32 = n / (n - f);
  m33 = 1.f;
}

inline ATOM_Matrix4x4f ATOM_Matrix4x4f::getOrthoFrustumLHMatrix (float l, float r, float b, float t, float n, float f)
{
  ATOM_Matrix4x4f m;
  m.makeOrthoFrustumLH (l, r, b, t, n, f);
  return m;
}

inline ATOM_Matrix4x4f ATOM_Matrix4x4f::getOrthoFrustumRHMatrix (float l, float r, float b, float t, float n, float f)
{
  ATOM_Matrix4x4f m;
  m.makeOrthoFrustumRH (l, r, b, t, n, f);
  return m;
}

inline void ATOM_Matrix4x4f::makeBillboard (const ATOM_Vector3f &eye, const ATOM_Vector3f &to, const ATOM_Vector3f &up)
{
  ATOM_Vector3f zaxis = eye - to;
  zaxis.normalize ();
  ATOM_Vector3f yaxis = up;
  yaxis.normalize ();
  ATOM_Vector3f xaxis = crossProduct (yaxis, zaxis);
  zaxis = crossProduct (xaxis, yaxis);

  setRow (0, xaxis, 0.f);
  setRow (1, yaxis, 0.f);
  setRow (2, zaxis, 0.f);
  setRow (3, eye, 1.f);
}

inline void ATOM_Matrix4x4f::makeReflection (const ATOM_Vector4f &plane)
{
	m00 = 1.f - 2.f * plane.x * plane.x;
	m10 = -2 * plane.x * plane.y;
	m20 = -2 * plane.x * plane.z;
	m30 = -2 * plane.x * plane.w;
	m01 = m10;
	m11 = 1 - 2 * plane.y * plane.y;
	m21 = -2 * plane.y * plane.z;
	m31 = -2 * plane.y * plane.w;
	m02 = m20;
	m12 = m21;
	m22 = 1 - 2 * plane.z * plane.z;
	m32 = -2 * plane.z * plane.w;
	m03 = 0.f;
	m13 = 0.f;
	m23 = 0.f;
	m33 = 1.f;
}

inline void ATOM_Matrix4x4f::decomposeLookatLH (ATOM_Vector3f &eye, ATOM_Vector3f &to, ATOM_Vector3f &up) const
{
  eye = getRow3 (3);
  up = getRow3 (1);
  to = eye + getRow3 (2);
}

inline void ATOM_Matrix4x4f::decomposeLookatRH (ATOM_Vector3f &eye, ATOM_Vector3f &to, ATOM_Vector3f &up) const
{
  eye = getRow3 (3);
  up = getRow3 (1);
  to = eye - getRow3(2);
}

inline ATOM_Matrix4x4f ATOM_Matrix4x4f::getBillboardMatrix (const ATOM_Vector3f &eye, const ATOM_Vector3f &to, const ATOM_Vector3f &up)
{
  ATOM_Matrix4x4f m;
  m.makeBillboard (eye, to, up);
  return m;
}

inline ATOM_Matrix4x4f ATOM_Matrix4x4f::getReflectionMatrix (const ATOM_Vector4f &plane)
{
  ATOM_Matrix4x4f m;
  m.makeReflection (plane);
  return m;
}


