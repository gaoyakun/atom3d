/*
// wangjian added for shadow test 
// 2013.5.31	

*/

#ifndef __ATOM_GEOMETRY_PLANE3D_H
#define __ATOM_GEOMETRY_PLANE3D_H

#pragma once

#include "Polygon3d.h"

#include "../ATOM_math.h"
//#include "Line.h"


class Line;

class Plane
{
public:
	Plane(const ATOM_Vector4f& equation);
	Plane(const ATOM_Vector3f& normal, float distance);
	Plane(const ATOM_Vector3f& v0, const ATOM_Vector3f& v1, const ATOM_Vector3f& v2, Polygon3d::VertexOrder order);
	Plane(const ATOM_Vector3f& point, const ATOM_Vector3f& normal);

	const ATOM_Vector4f& GetEquation();
	void SetEquation(const ATOM_Vector4f& equation);

	const ATOM_Vector3f& GetNormal() const;
	float GetDistance() const;
	float GetDistance(const ATOM_Vector3f& point) const;
	
	bool IsBehind(const ATOM_Vector3f& point) const;
	bool Intersect(float& param, const Line& line, float epsilon = 1.0e-6f) const;
	void Transform(const ATOM_Matrix4x4f& matrix);

private:
	void CalcHessNorm();

private:
	float _distance;
#pragma warning(push)
#pragma warning(disable : 4251)
	ATOM_Vector3f _normal;
	ATOM_Vector4f _equation;
#pragma warning(pop)

};

#endif