/*
// wangjian added for shadow test 
// 2013.5.31	

*/

#ifndef __ATOM_GEOMETRY_LINE3D_H
#define __ATOM_GEOMETRY_LINE3D_H

#pragma once

#include "../ATOM_math.h"

class Line
{
public:
	Line(const ATOM_Vector3f& point, const ATOM_Vector3f& direction);

	const ATOM_Vector3f& GetStartPoint() const;
	const ATOM_Vector3f& GetEndPoint() const;
	ATOM_Vector3f GetPoint(float parameter) const;
	void SetStartPoint(const ATOM_Vector3f& point);

	const ATOM_Vector3f& GetDirection() const;
	void SetDirection(const ATOM_Vector3f& direction);

	static bool CmpLines(const Line& l0, const Line& l1, float epsilon = 1.0e-6f);

private:
#pragma warning(push)
#pragma warning(disable : 4251)
	ATOM_Vector3f _startPoint;
	ATOM_Vector3f _endPoint;
	ATOM_Vector3f _direction;
#pragma warning(pop)
};

#endif