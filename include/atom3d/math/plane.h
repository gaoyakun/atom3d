#ifndef __ATOM_MATH_PLANE_H
#define __ATOM_MATH_PLANE_H

#include "vector3.h"
#include "distance.h"

class ATOM_Plane
{
public:
	ATOM_Plane (void);
	ATOM_Plane (const ATOM_Vector3f &origin, const ATOM_Vector3f &normal);
	ATOM_Plane (const ATOM_Vector3f &p0, const ATOM_Vector3f &p1, const ATOM_Vector3f &p2);

public:
	void  setCoefficients (float a, float b, float c, float d);
	void  setA (float a);
	void  setB (float b);
	void  setC (float c);
	void  setD (float d);
	float getA (void) const;
	float getB (void) const;
	float getC (void) const;
	float getD (void) const;
	int   getNX (void) const;
	int   getNY (void) const;
	int   getNZ (void) const;
	int   getPX (void) const;
	int   getPY (void) const;
	int   getPZ (void) const;
	void  flip (void);
	void  normalize (void);
	void  set (const ATOM_Vector3f &origin, const ATOM_Vector3f &normal);
	void  set (const ATOM_Vector3f &v0, const ATOM_Vector3f &v1, const ATOM_Vector3f &v2);

public:
	ATOM_Vector3f getNormal () const;
	ATOM_Vector3f getNearestPointToPoint (const ATOM_Vector3f &point) const;
	bool isFrontFacingTo (const ATOM_Vector3f &point) const;
	float signedDistanceTo (const ATOM_Vector3f &point) const;

private:
	void calcNPVertices () const;

private:
	float _M_coefficients[4];
	mutable int _M_nVertices[3];
	mutable int _M_pVertices[3];
	mutable bool _M_np_dirty;
};

#include "plane.inl"

#endif // __ATOM_GEOMETRY_PLANE_H


