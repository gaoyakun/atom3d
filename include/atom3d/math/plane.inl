inline ATOM_Plane::ATOM_Plane (void) 
{
}

inline ATOM_Plane::ATOM_Plane (const ATOM_Vector3f &origin, const ATOM_Vector3f &normal) 
{
	set (origin, normal);
}

inline ATOM_Plane::ATOM_Plane (const ATOM_Vector3f &p0, const ATOM_Vector3f &p1, const ATOM_Vector3f &p2) 
{
	set (p0, p1, p2);
}

inline void ATOM_Plane::setCoefficients (float a, float b, float c, float d) 
{
	_M_coefficients[0] = a;
	_M_coefficients[1] = b;
	_M_coefficients[2] = c;
	_M_coefficients[3] = d;
	_M_np_dirty = true;
}

inline void ATOM_Plane::setA (float a) 
{
	_M_coefficients[0] = a;
	_M_np_dirty = true;
}

inline void ATOM_Plane::setB (float b) 
{
	_M_coefficients[1] = b;
	_M_np_dirty = true;
}

inline void ATOM_Plane::setC (float c) 
{
	_M_coefficients[2] = c;
	_M_np_dirty = true;
}

inline void ATOM_Plane::setD (float d) 
{
	_M_coefficients[3] = d;
	_M_np_dirty = true;
}

inline float ATOM_Plane::getA (void) const 
{
	return _M_coefficients[0];
}

inline float ATOM_Plane::getB (void) const 
{
	return _M_coefficients[1];
}

inline float ATOM_Plane::getC (void) const 
{
	return _M_coefficients[2];
}

inline float ATOM_Plane::getD (void) const 
{
	return _M_coefficients[3];
}

inline int ATOM_Plane::getNX (void) const 
{
	if (_M_np_dirty)
	{
		_M_np_dirty = false;
		calcNPVertices ();
	}
	return _M_nVertices[0];
}

inline int ATOM_Plane::getNY (void) const 
{
	if (_M_np_dirty)
	{
		_M_np_dirty = false;
		calcNPVertices ();
	}
	return _M_nVertices[1];
}

inline int ATOM_Plane::getNZ (void) const 
{
	if (_M_np_dirty)
	{
		_M_np_dirty = false;
		calcNPVertices ();
	}
	return _M_nVertices[2];
}

inline int ATOM_Plane::getPX (void) const 
{
	if (_M_np_dirty)
	{
		_M_np_dirty = false;
		calcNPVertices ();
	}
	return _M_pVertices[0];
}

inline int ATOM_Plane::getPY (void) const 
{
	if (_M_np_dirty)
	{
		_M_np_dirty = false;
		calcNPVertices ();
	}
	return _M_pVertices[1];
}

inline int ATOM_Plane::getPZ () const 
{
	if (_M_np_dirty)
	{
		_M_np_dirty = false;
		calcNPVertices ();
	}
	return _M_pVertices[2];
}

inline void ATOM_Plane::flip (void) 
{
	_M_coefficients[0] = -_M_coefficients[0];
	_M_coefficients[1] = -_M_coefficients[1];
	_M_coefficients[2] = -_M_coefficients[2];
	_M_coefficients[3] = -_M_coefficients[3];
}

inline void ATOM_Plane::normalize (void) 
{
	float len = ATOM_sqrt(_M_coefficients[0] * _M_coefficients[0] +
						  _M_coefficients[1] * _M_coefficients[1] +
						  _M_coefficients[2] * _M_coefficients[2]);

	if (!ATOM_equal (len, 0.f))
	{
		_M_coefficients[0] /= len;
		_M_coefficients[1] /= len;
		_M_coefficients[2] /= len;
		_M_coefficients[3] /= len;
	}
}

namespace internal 
{
	static inline int sign (float value) 
	{
		return value > 0.f ? 1 : -1;
	}
} // namespace internal

inline void ATOM_Plane::calcNPVertices (void) const 
{
	_M_pVertices[0] = internal::sign (_M_coefficients[0]);
	_M_pVertices[1] = internal::sign (_M_coefficients[1]);
	_M_pVertices[2] = internal::sign (_M_coefficients[2]);
	_M_nVertices[0] = -_M_pVertices[0];
	_M_nVertices[1] = -_M_pVertices[1];
	_M_nVertices[2] = -_M_pVertices[2];
}

inline ATOM_Vector3f ATOM_Plane::getNearestPointToPoint (const ATOM_Vector3f &point) const 
{
	return point - ATOM_Vector3f(getA(),getB(),getC()) * distance_Plane_Point (getA(), getB(), getC(), getD(), point);
}

inline void ATOM_Plane::set (const ATOM_Vector3f &origin, const ATOM_Vector3f &normal) 
{
	ATOM_Vector3f n = normal;
	n.normalize ();
	setA (n.x);
	setB (n.y);
	setC (n.z);
	setD (-dotProduct (n, origin));
}

inline void ATOM_Plane::set (const ATOM_Vector3f &v0, const ATOM_Vector3f &v1, const ATOM_Vector3f &v2) 
{
	ATOM_Vector3f n = crossProduct (v1 - v0, v2 - v0);
	n.normalize();
	setA (n.x);
	setB (n.y);
	setC (n.z);
	setD (-dotProduct (n, v0));
}

inline bool ATOM_Plane::isFrontFacingTo (const ATOM_Vector3f &point) const 
{
	return dotProduct (ATOM_Vector3f(getA(), getB(), getC()), point) <= 0.f;
}

inline float ATOM_Plane::signedDistanceTo (const ATOM_Vector3f &point) const 
{
	return dotProduct (ATOM_Vector3f(getA(), getB(), getC()), point) + getD();
}

inline ATOM_Vector3f ATOM_Plane::getNormal () const 
{
	return ATOM_Vector3f(getA(), getB(), getC());
}
