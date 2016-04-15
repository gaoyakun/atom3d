/*
// wangjian added for shadow test 
// 2013.5.31	

*/
#ifndef __ATOM_GEOMETRY_POLYGON3D_H
#define __ATOM_GEOMETRY_POLYGON3D_H

#pragma once

#include <vector>
//#include "Plane_ext.h"
#include "../ATOM_math.h"

class Line;
class Plane;

class Polygon3d
{
public:
	enum VertexOrder
	{
		ccw = 0,
		cw
	};

	typedef ATOM_Vector3f tVertex;
	typedef std::vector<tVertex> tVertexArray;
	
	Polygon3d();
	Polygon3d(const tVertex& p0, const tVertex& p1, const tVertex& p2);
	Polygon3d(const tVertex& p0, const tVertex& p1, const tVertex& p2, const tVertex& p3);

	unsigned int GetVertexCount() const;
	const tVertexArray& GetVertexArray() const;
	void AddVertex(const tVertex& vertex);
	void AddUniqueVertex(const tVertex& vertex, float epsilon = 1.0e-6f);

	void Clip(const Plane& plane, tVertexArray& intersectionPoints);
	void ReverseVertices();

	const ATOM_Vector3f& GetNormal();
	void SetNormal(const ATOM_Vector3f& normal);

	static bool CmpVertices(const ATOM_Vector3f& v0, const ATOM_Vector3f& v1, float epsilon = 1.0e-6f);
	static bool CmpCoords(float c0, float c1, float epsilon = 1.0e-6f);

private:
#pragma warning(push)
#pragma warning(disable : 4251)
	tVertexArray _vertices;
	ATOM_Vector3f _normal;
#pragma warning(pop)
};

#endif