#ifndef ATOM_GEOMETRY_TRANSFORM_H_
#define ATOM_GEOMETRY_TRANSFORM_H_

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_math.h"

//template<typename T> class TransformOrtho;
//typedef TransformOrtho<float>	TransformOrthof;
//typedef TransformOrtho<double>	TransformOrthod;
	
//==========================================================================
//This is a class which defines a transformation from one coordinate system to another. 
//The two coordinate systems are refered to as 'other' and 'this'. 
//The transform defines a transformation from 'other' to 'this'. 

class ATOM_Transform
{
public:
  typedef ATOM_Matrix3x3f Matrix33;
  typedef ATOM_Matrix4x4f	Matrix44;
  typedef ATOM_Vector3f	Vector3;

private:
	Matrix44	_Mo2t;

public:
	virtual ~ATOM_Transform (void) {}
	virtual void reset();
	virtual const Matrix44 & getO2T() const;
	virtual Vector3 getOrigin() const;
	virtual void setO2T(const Matrix44 &Mo2t);
	virtual void setOrigin(const Vector3 &V);
	virtual void translate(const Vector3 &V);
public:
	ATOM_Transform() { reset(); }
	ATOM_Transform (const Matrix44 &Mo2t) { setO2T(Mo2t); }
};
typedef ATOM_Transform	ATOM_Transformf;

//template class Transform<float>;
//template class Transform<double>;
//template class Transform<long double>;

#endif// ATOM_GEOMETRY_TRANSFORM_H_
