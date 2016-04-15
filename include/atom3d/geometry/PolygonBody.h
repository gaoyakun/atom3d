/*
// wangjian added for shadow test 
// 2013.5.31	

*/
#ifndef __ATOM_GEOMETRY_POLYGONBODY_H
#define __ATOM_GEOMETRY_POLYGONBODY_H

#pragma once

#include <vector>


#include "Polygon3d.h"

#include "../ATOM_math.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct sSimpleFrustm
{
	enum Corner
	{
		NearBottomLeft = 0,
		NearBottomRight,
		NearTopRight,
		NearTopLeft,
		FarBottomLeft,
		FarBottomRight,
		FarTopRight,
		FarTopLeft,
		CornerCOUNT
	};
	typedef std::vector<ATOM_Vector3f> tCornerArray;
	tCornerArray _corners;

	const ATOM_Vector3f& GetCorner(Corner corner) const
	{
		return _corners[corner];
	}

	explicit sSimpleFrustm(	ATOM_Vector3f eye, ATOM_Vector3f center, ATOM_Vector3f up, 
							float tanHalfFovY, float aspect, 
							float zNear, float zFar	);
	~sSimpleFrustm();
};

class PolygonBody
{
public:
	typedef std::vector<Polygon3d*> tPolygonArray;
	typedef std::vector<Line*> tEdgeArray;

	PolygonBody();
	~PolygonBody();

	unsigned int GetPolygonCount() const;
	const tPolygonArray& GetPolygonArray() const;
	void Add(Polygon3d* polygon);
	void Add(const Polygon3d::tVertexArray& vertices, const ATOM_Vector3f& normal);

	void AddFrustum( const sSimpleFrustm & frustum	);
	void AddFrustum( ATOM_Vector3f eye, ATOM_Vector3f center, ATOM_Vector3f up, 
					 float fov, float aspect, float zNear, float zFar );

	unsigned int GetVertexCount() const;
	const Polygon3d::tVertexArray& GetVertexArray() const;
	
	void Clip(const Plane& plane);
	void Clip(const ATOM_BBox& boundingBox);

	//void Clip(const ATOM_Frustum3d& frustum);

	void Extrude(const ATOM_Vector3f& direction, const ATOM_BBox& boundingBox);
	void CreateHull(const ATOM_Vector3f& point);
	/*void CreateConvexHull(const ATOM_Vector3f& point);
	void CreateConvexHull();*/
	void CreateUniqueVertexArray(float epsilon = 1.0e-6f);
	void Clear();

private:
	void AddUniqueEdge(tEdgeArray& outEdgeList, const Line& line) const;
	void AddUniqueVertex(const Polygon3d::tVertex& vertex, float epsilon);
	void RemoveVisiblePolygons(const ATOM_Vector3f& point);
	void Intersect(const Line& line, const ATOM_Vector3f& min, const ATOM_Vector3f& max, Polygon3d::tVertexArray& vertices);
	bool Clip(float p, float q, float& u1, float& u2) const;
	void ExtrudePolygons(const ATOM_Vector3f& direction, const ATOM_BBox& boundingBox);
	void CreateConvexHull(const Polygon3d::tVertexArray& vertices);
	
private:
#pragma warning(push)
#pragma warning(disable : 4251)
	tPolygonArray _polygons;
	Polygon3d::tVertexArray _uniqueVertices;
#pragma warning(pop)
};

#endif
