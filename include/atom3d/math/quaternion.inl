FORCEINLINE ATOM_Quaternion::ATOM_Quaternion (void)
{
}

FORCEINLINE ATOM_Quaternion::ATOM_Quaternion (float x_, float y_, float z_, float w_)
	:
x(x_), y(y_), z(z_), w(w_)
{
}

FORCEINLINE void ATOM_Quaternion::set (float x_, float y_, float z_, float w_)
{
	x = x_;
	y = y_;
	z = z_;
	w = w_;
}

FORCEINLINE void ATOM_Quaternion::normalize (void)
{
	float k = ATOM_invsqrt(x*x + y*y + z*z + w*w);
	x *= k;
	y *= k;
	z *= k;
	w *= k;
}

FORCEINLINE void ATOM_Quaternion::normalizeFrom (const ATOM_Quaternion &other)
{
	x = other.x;
	y = other.y;
	z = other.z;
	w = other.w;

	normalize ();
}

FORCEINLINE void ATOM_Quaternion::identity (void)
{
	set (0.f, 0.f, 0.f, 1.f);
}

FORCEINLINE void ATOM_Quaternion::conjugate (void)
{
	x = -x;
	y = -y;
	z = -z;
}

FORCEINLINE void ATOM_Quaternion::conjugateFrom (const ATOM_Quaternion &other)
{
	x = -other.x;
	y = -other.y;
	z = -other.z;
	w = other.w;
}

FORCEINLINE void ATOM_Quaternion::toAxisAngle (float &angle, ATOM_Vector3f &axis) const
{
	float scale = ATOM_sqrt(x*x + y*y + z*z);
	if (scale > ATOM_Epsilon)
	{
		angle = 2 * ATOM_acos(w);
		axis.set (x/scale, y/scale, z/scale);
	}
	else
	{
		angle = 0.f;
		axis.set (1.f, 0.f, 0.f);
	}
}

FORCEINLINE void ATOM_Quaternion::fromAxisAngle (float angle, const ATOM_Vector3f &axis)
{
	angle *= 0.5f;
	float s, c;
	ATOM_sincos (angle, &s, &c);
	x = axis.x * s;
	y = axis.y * s;
	z = axis.z * s;
	w = c;
}

FORCEINLINE void ATOM_Quaternion::fromMatrix (const ATOM_Matrix3x3f &matrix)
{
#if 1
	float trace = matrix.m00 + matrix.m11 + matrix.m22 + 1.f;
	if ( trace > 1.0f)
	{
		float root = ATOM_sqrt(trace);
		w = 0.5f * root;
		root = 0.5f / root;
		x = ( matrix.m12 - matrix.m21 ) * root;
		y = ( matrix.m20 - matrix.m02 ) * root;
		z = ( matrix.m01 - matrix.m10 ) * root;
	}
	else
	{
		int maxi = 0;
		float maxdiag = matrix.m00;
		for (int i = 1; i < 3; ++i)
		{
			if ( matrix(i, i) > maxdiag )
			{
				maxi = i;
				maxdiag = matrix(i, i);
			}
		}

		float S;
		switch( maxi )
		{
		case 0:
			S = 2.0f * ATOM_sqrt(1.0f + matrix.m00 - matrix.m11 - matrix.m22);
			x = 0.25f * S;
			y = ( matrix.m01 + matrix.m10 ) / S;
			z = ( matrix.m02 + matrix.m20 ) / S;
			w = ( matrix.m12 - matrix.m21 ) / S;
			break;
		case 1:
			S = 2.0f * ATOM_sqrt(1.0f + matrix.m11 - matrix.m00 - matrix.m22);
			x = ( matrix.m01 + matrix.m10 ) / S;
			y = 0.25f * S;
			z = ( matrix.m12 + matrix.m21 ) / S;
			w = ( matrix.m20 - matrix.m02 ) / S;
			break;
		case 2:
			S = 2.0f * sqrt(1.0f + matrix.m22 - matrix.m00 - matrix.m11);
			x = ( matrix.m02 + matrix.m20 ) / S;
			y = ( matrix.m12 + matrix.m21 ) / S;
			z = 0.25f * S;
			w = ( matrix.m01 - matrix.m10 ) / S;
			break;
		}
	}
#else
	float trace = matrix(0, 0) + matrix(1, 1) + matrix(2, 2);
	float root;

	if (trace > 0.f)
	{
		root = ATOM_sqrt(trace + 1.f);
		w = 0.5f * root;
		root = 0.5f / root;
		x = (matrix(2, 1) - matrix(1, 2)) * root;
		y = (matrix(0, 2) - matrix(2, 0)) * root;
		z = (matrix(1, 0) - matrix(0, 1)) * root;
	}
	else
	{
		static const unsigned next[3] = { 1, 2, 0 };
		unsigned i = 0;
		if (matrix(1, 1) > matrix(0, 0))
		{
			i = 1;
		}
		if (matrix(2, 2) > matrix(i, i))
		{
			i = 2;
		}

		unsigned j = next[i];
		unsigned k = next[j];
		root = ATOM_sqrt (matrix(i, i) - matrix(j, j) - matrix(k, k) + 1.f);

		float *quat[3] = { &x, &y, &z };

		*quat[i] = 0.5f * root;
		root = 0.5f / root;
		w = (matrix(k, j) - matrix(j, k)) * root;
		*quat[j] = (matrix(j, i) + matrix(i, j)) * root;
		*quat[k] = (matrix(k, i) + matrix(i, k)) * root;
	}
#endif
}

FORCEINLINE void ATOM_Quaternion::toMatrix (ATOM_Matrix3x3f &matrix) const
{
	float xx2 = x * x * 2.f;
	float yy2 = y * y * 2.f;
	float zz2 = z * z * 2.f;
	float xy2 = x * y * 2.f;
	float yz2 = y * z * 2.f;
	float zx2 = z * x * 2.f;
	float xw2 = x * w * 2.f;
	float yw2 = y * w * 2.f;
	float zw2 = z * w * 2.f;

	matrix(0, 0) = 1.f - yy2 - zz2;
	matrix(0, 1) = xy2 + zw2;
	matrix(0, 2) = zx2 - yw2;
	matrix(1, 0) = xy2 - zw2;
	matrix(1, 1) = 1.f - xx2 - zz2;
	matrix(1, 2) = yz2 + xw2;
	matrix(2, 0) = zx2 + yw2;
	matrix(2, 1) = yz2 - xw2;
	matrix(2, 2) = 1.f - xx2 - yy2;
}

FORCEINLINE void ATOM_Quaternion::fromMatrix (const ATOM_Matrix4x4f &matrix)
{
#if 1
	float trace = matrix.m00 + matrix.m11 + matrix.m22 + 1.f;
	if ( trace > 1.0f)
	{
		float root = ATOM_sqrt(trace);
		w = 0.5f * root;
		root = 0.5f / root;
		x = ( matrix.m12 - matrix.m21 ) * root;
		y = ( matrix.m20 - matrix.m02 ) * root;
		z = ( matrix.m01 - matrix.m10 ) * root;
	}
	else
	{
		int maxi = 0;
		float maxdiag = matrix.m00;
		for (int i = 1; i < 3; ++i)
		{
			if ( matrix(i, i) > maxdiag )
			{
				maxi = i;
				maxdiag = matrix(i, i);
			}
		}

		float S;
		switch( maxi )
		{
		case 0:
			S = 2.0f * ATOM_sqrt(1.0f + matrix.m00 - matrix.m11 - matrix.m22);
			x = 0.25f * S;
			y = ( matrix.m01 + matrix.m10 ) / S;
			z = ( matrix.m02 + matrix.m20 ) / S;
			w = ( matrix.m12 - matrix.m21 ) / S;
			break;
		case 1:
			S = 2.0f * ATOM_sqrt(1.0f + matrix.m11 - matrix.m00 - matrix.m22);
			x = ( matrix.m01 + matrix.m10 ) / S;
			y = 0.25f * S;
			z = ( matrix.m12 + matrix.m21 ) / S;
			w = ( matrix.m20 - matrix.m02 ) / S;
			break;
		case 2:
			S = 2.0f * sqrt(1.0f + matrix.m22 - matrix.m00 - matrix.m11);
			x = ( matrix.m02 + matrix.m20 ) / S;
			y = ( matrix.m12 + matrix.m21 ) / S;
			z = 0.25f * S;
			w = ( matrix.m01 - matrix.m10 ) / S;
			break;
		}
	}
#else
	float trace = matrix(0, 0) + matrix(1, 1) + matrix(2, 2);
	float root;

	if (trace > 0.f)
	{
		root = ATOM_sqrt(trace + 1.f);
		w = 0.5f * root;
		root = 0.5f / root;
		x = (matrix(2, 1) - matrix(1, 2)) * root;
		y = (matrix(0, 2) - matrix(2, 0)) * root;
		z = (matrix(1, 0) - matrix(0, 1)) * root;
	}
	else
	{
		static const unsigned next[3] = { 1, 2, 0 };
		unsigned i = 0;
		if (matrix(1, 1) > matrix(0, 0))
		{
			i = 1;
		}
		if (matrix(2, 2) > matrix(i, i))
		{
			i = 2;
		}

		unsigned j = next[i];
		unsigned k = next[j];
		root = ATOM_sqrt (matrix(i, i) - matrix(j, j) - matrix(k, k) + 1.f);

		float *quat[3] = { &x, &y, &z };

		*quat[i] = 0.5f * root;
		root = 0.5f / root;
		w = (matrix(k, j) - matrix(j, k)) * root;
		*quat[j] = (matrix(j, i) + matrix(i, j)) * root;
		*quat[k] = (matrix(k, i) + matrix(i, k)) * root;
	}
#endif
}

FORCEINLINE void ATOM_Quaternion::toMatrix (ATOM_Matrix4x4f &matrix) const
{
	float xx2 = x * x * 2.f;
	float yy2 = y * y * 2.f;
	float zz2 = z * z * 2.f;
	float xy2 = x * y * 2.f;
	float yz2 = y * z * 2.f;
	float zx2 = z * x * 2.f;
	float xw2 = x * w * 2.f;
	float yw2 = y * w * 2.f;
	float zw2 = z * w * 2.f;

	matrix(0, 0) = 1.f - yy2 - zz2;
	matrix(0, 1) = xy2 + zw2;
	matrix(0, 2) = zx2 - yw2;
	matrix(0, 3) = 0.f;
	matrix(1, 0) = xy2 - zw2;
	matrix(1, 1) = 1.f - xx2 - zz2;
	matrix(1, 2) = yz2 + xw2;
	matrix(1, 3) = 0.f;
	matrix(2, 0) = zx2 + yw2;
	matrix(2, 1) = yz2 - xw2;
	matrix(2, 2) = 1.f - xx2 - yy2;
	matrix(2, 3) = 0.f;
	matrix(3, 0) = 0.f;
	matrix(3, 1) = 0.f;
	matrix(3, 2) = 0.f;
	matrix(3, 3) = 1.f;
}

FORCEINLINE ATOM_Vector3f ATOM_Quaternion::toEulerXYZ (void) const
{
	return ATOM_Vector3f (
		ATOM_atan(2.f * (w * x + y * z) / (1.f - 2.f * (x * x + y * y))),
		ATOM_asin(2.f * (w * y - z * x)),
		ATOM_atan(2.f * (w * z + x * y) / (1.f - 2.f * (y * y + z * z)))
		);
}

FORCEINLINE void ATOM_Quaternion::fromEulerXYZ (const ATOM_Vector3f &eulerXYZ)
{
	float cx2, sx2, cy2, sy2, cz2, sz2;
	ATOM_sincos(eulerXYZ.x * 0.5f, &sx2, &cx2);
	ATOM_sincos(eulerXYZ.y * 0.5f, &sy2, &cy2);
	ATOM_sincos(eulerXYZ.z * 0.5f, &sz2, &cz2);

	x = sx2 * cy2 * cz2 - cx2 * sy2 * sz2;
	y =	cx2 * sy2 * cz2 + sx2 * cy2 * sz2;
	z =	cx2 * cy2 * sz2 - sx2 * sy2 * cz2;
	w =	cx2 * cy2 * cz2 + sx2 * sy2 * sz2;
}

FORCEINLINE void ATOM_Quaternion::setRotationArc (const ATOM_Vector3f &from, const ATOM_Vector3f &to)
{
	ATOM_Vector3f v = crossProduct (from, to);
	float d = dotProduct (from, to);
	float s = ATOM_sqrt((1.f + d) * 2.f);
	set (v.x/s, v.y/s, v.z/s, s * 0.5f);
}

FORCEINLINE void ATOM_Quaternion::rotateVector (const ATOM_Vector3f &from, ATOM_Vector3f &to) const
{
	ATOM_Quaternion q(from.x, from.y, from.z, 1.f);
	ATOM_Quaternion c;
	c.conjugateFrom (*this);

	q = (*this) >> q >> c;
	to.set (q.x, q.y, q.z);
}

FORCEINLINE ATOM_Quaternion ATOM_Quaternion::lerp (const ATOM_Quaternion &q1, const ATOM_Quaternion &q2, float t)
{

	return ATOM_Quaternion (
		q1.x + (q2.x-q1.x) * t,
		q1.y + (q2.y-q1.y) * t,
		q1.z + (q2.z-q1.z) * t,
		q1.w + (q2.w-q1.w) * t);
}

FORCEINLINE ATOM_Quaternion ATOM_Quaternion::fastSlerp (const ATOM_Quaternion &q1, const ATOM_Quaternion &q2, float t)
{
#if 0
    float fCos= q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;
	ATOM_Quaternion rkT;

	if (fCos < 0.f)
	{
		fCos = -fCos;
		rkT.set (-q2.x, -q2.y, -q2.z, -q2.w);
	}
	else
	{
		rkT = q2;
	}

	ATOM_Quaternion r;
	float fCoeff0, fCoeff1;
	if (ATOM_abs(fCos) < 1.f - 0.001f)
	{
		float fSin = ATOM_sqrt(1.f - fCos * fCos);
		float fAngle = ATOM_atan2(fSin, fCos);
		float fInvSin = 1.f / fSin;
		fCoeff0 = ATOM_sin((1.f - t) * fAngle) * fInvSin;
		fCoeff1 = ATOM_sin(t * fAngle) * fInvSin;
	}
	else
	{
		fCoeff0 = 1.f - t;
		fCoeff1 = t;
	}
	r.x = fCoeff0 * q1.x + fCoeff1 * rkT.x;
	r.y = fCoeff0 * q1.y + fCoeff1 * rkT.y;
	r.z = fCoeff0 * q1.z + fCoeff1 * rkT.z;
	r.w = fCoeff0 * q1.w + fCoeff1 * rkT.w;
	r.normalize ();
	return r;
#else
	float cosom, absCosom, sinom, sinSqr, omega, scale0, scale1;

    cosom = q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;
    absCosom = ATOM_abs( cosom );
    if ( ( 1.0f - absCosom ) > 1e-3f ) 
	{
        sinSqr = 1.0f - absCosom * absCosom;
        sinom = 1.f/ATOM_sqrt(sinSqr);
        omega = ATOM_atan_positive( sinSqr * sinom, absCosom );
        scale0 = ATOM_sin_0_halfpi( ( 1.0f - t ) * omega ) * sinom;
        scale1 = ATOM_sin_0_halfpi( t * omega ) * sinom;
    } 
	else 
	{
        scale0 = 1.0f - t;
        scale1 = t;
    }
    scale1 = ( cosom >= 0.0f ) ? scale1 : -scale1;

	return ATOM_Quaternion (scale0 * q1.x + scale1 * q2.x, scale0 * q1.y + scale1 * q2.y, scale0 * q1.z + scale1 * q2.z, scale0 * q1.w + scale1 * q2.w);
#endif
}

FORCEINLINE ATOM_Quaternion ATOM_Quaternion::slerp (const ATOM_Quaternion &q1, const ATOM_Quaternion &q2, float t)
{
#if 1
    float dot, epsilon, temp, theta, u;

    epsilon = 1.0f;
    temp = 1.0f - t;
    u = t;
    dot = q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;
    if ( dot < 0.0f )
    {
        epsilon = -1.0f;
        dot = -dot;
    }
    if( 1.0f - dot > 0.001f )
    {
        theta = ATOM_acos(dot);
        temp  = ATOM_sin(theta * temp) / ATOM_sin(theta);
        u = ATOM_sin(theta * u) / ATOM_sin(theta);
    }
	ATOM_Quaternion result;
    result.x = temp * q1.x + epsilon * u * q2.x;
    result.y = temp * q1.y + epsilon * u * q2.y;
    result.z = temp * q1.z + epsilon * u * q2.z;
    result.w = temp * q1.w + epsilon * u * q2.w;
    return result;
#else
	ATOM_Quaternion result;
	float X, Y, Z, W;

	float cosOmega = q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z;

	if ( cosOmega < 0.0f ) 
	{
		X = -q2.x;
		Y = -q2.y;
		Z = -q2.z;
		W = -q2.w;
		cosOmega = -cosOmega;
	}
	else
	{
		X = q2.x;
		Y = q2.y;
		Z = q2.z;
		W = q2.w;
	}

	float k0, k1;
	if ( cosOmega > 0.99999f ) 
	{
		k0 = 1.0f - t;
		k1 = t;
	}
	else 
	{
		float sinOmega = ATOM_sqrt( 1.0f - cosOmega*cosOmega );
		float omega = ATOM_atan2( sinOmega, cosOmega );
		float invSinOmega = 1.0f/sinOmega;

		k0 = ATOM_sin((1.0f - t) * omega) * invSinOmega;
		k1 = ATOM_sin(t*omega) * invSinOmega;
	}

	result.x = q1.x * k0 + q2.x * k1;
	result.y = q1.y * k0 + q2.y * k1;
	result.z = q1.z * k0 + q2.z * k1;
	result.w = q1.w * k0 + q2.w * k1;

	return result;
#endif
}

FORCEINLINE ATOM_Quaternion & ATOM_Quaternion::operator >>= (const ATOM_Quaternion &other)
{
	*this = (*this) >> other;
}

FORCEINLINE ATOM_Quaternion & ATOM_Quaternion::operator <<= (const ATOM_Quaternion &other)
{
	*this = (*this) << other;
}

FORCEINLINE ATOM_Quaternion operator >> (const ATOM_Quaternion &q1, const ATOM_Quaternion &q2)
{
	ATOM_Quaternion ret;
	ret.w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;
	ret.x = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y;
	ret.y = q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x;
	ret.z = q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w;

	return ret;
}

FORCEINLINE ATOM_Quaternion operator << (const ATOM_Quaternion &q1, const ATOM_Quaternion &q2)
{
	return operator >> (q2, q1);
}

FORCEINLINE bool operator == (const ATOM_Quaternion &q1, const ATOM_Quaternion &q2)
{
	return q1.x == q2.x && q1.y == q2.y && q1.z == q2.z && q1.w == q2.w;
}

FORCEINLINE bool operator != (const ATOM_Quaternion &q1, const ATOM_Quaternion &q2)
{
	return ! operator == (q1, q2);
}
