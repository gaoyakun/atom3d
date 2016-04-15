#include <limits>

#include "Line.h"
#include "Plane_ext.h"
#include "PolygonBody.h"

//#include "../Qhull/source/include/cpp/qhull.h"
//#include "../Qhull/source/include/cpp/QhullFacetList.h"
//#include "../Qhull/source/include/cpp/QhullVertexSet.h"

#include "dbghlp/logger.h"

//=====================================================

sSimpleFrustm::sSimpleFrustm(	ATOM_Vector3f eye, ATOM_Vector3f center, ATOM_Vector3f up, 
								float tanHalfFovY, float aspect, 
								float zNear, float zFar )
{
	if (_corners.empty())
		for (unsigned int i = 0; i < sSimpleFrustm::CornerCOUNT; ++i)
			_corners.push_back(ATOM_Vector3f(.0, .0, .0));

	ATOM_Vector3f viewVec(center - eye);
	viewVec.normalize();

	ATOM_Vector3f leftVec( crossProduct(up, viewVec) );
	leftVec.normalize();

	ATOM_Vector3f upVec(crossProduct(viewVec, leftVec));

	const float nearHalfHeight = tanHalfFovY * zNear;
	const float nearHalfWidth = nearHalfHeight * aspect;

	const float farHalfHeight = tanHalfFovY * zFar;
	const float farHalfWidth = farHalfHeight * aspect;

	const ATOM_Vector3f nearCenter(eye + viewVec * zNear);

	_corners[sSimpleFrustm::NearBottomRight]  = ATOM_Vector3f(nearCenter + (leftVec * nearHalfWidth) - (upVec * nearHalfHeight));
	_corners[sSimpleFrustm::NearTopRight]     = ATOM_Vector3f(nearCenter + (leftVec * nearHalfWidth) + (upVec * nearHalfHeight));
	_corners[sSimpleFrustm::NearTopLeft]	  = ATOM_Vector3f(nearCenter - (leftVec * nearHalfWidth) + (upVec * nearHalfHeight));
	_corners[sSimpleFrustm::NearBottomLeft]   = ATOM_Vector3f(nearCenter - (leftVec * nearHalfWidth) - (upVec * nearHalfHeight));

	const ATOM_Vector3f farCenter(eye + viewVec * zFar);

	_corners[sSimpleFrustm::FarBottomRight]  = ATOM_Vector3f(farCenter + (leftVec * farHalfWidth) - (upVec * farHalfHeight));
	_corners[sSimpleFrustm::FarTopRight]	 = ATOM_Vector3f(farCenter + (leftVec * farHalfWidth) + (upVec * farHalfHeight));
	_corners[sSimpleFrustm::FarTopLeft]	     = ATOM_Vector3f(farCenter - (leftVec * farHalfWidth) + (upVec * farHalfHeight));
	_corners[sSimpleFrustm::FarBottomLeft]	 = ATOM_Vector3f(farCenter - (leftVec * farHalfWidth) - (upVec * farHalfHeight));
}

sSimpleFrustm::~sSimpleFrustm()
{
	_corners.clear();
}

//========================================================================================================================================//


PolygonBody::PolygonBody() :
	_polygons(0),
	_uniqueVertices(0)
{
}

PolygonBody::~PolygonBody()
{
	Clear();
}

unsigned int PolygonBody::GetPolygonCount() const
{
	return static_cast<unsigned int>(_polygons.size());
}

const PolygonBody::tPolygonArray& PolygonBody::GetPolygonArray() const
{
	return _polygons;
}

void PolygonBody::Add(Polygon3d* polygon)
{
	if (polygon)
		_polygons.push_back(polygon);
}

void PolygonBody::Add(const Polygon3d::tVertexArray& vertices, const ATOM_Vector3f& normal)
{
	// we need at least 3 vertices
	if (vertices.size() < 3)
		return;

	Polygon3d* polygon = new Polygon3d();
	
	for (unsigned int i = 0; i < vertices.size(); ++i)
		polygon->AddUniqueVertex(vertices[i]);

	if (polygon->GetVertexCount() < 3)
	{
		delete polygon;
		return;
	}

	// check normal direction
	Plane polygonPlane(	polygon->GetVertexArray()[0], 
						polygon->GetVertexArray()[1], 
						polygon->GetVertexArray()[2],
						Polygon3d::ccw);

	if (dotProduct( polygonPlane.GetNormal(), normal ) < .0)
		polygon->ReverseVertices();

	_polygons.push_back(polygon);
}

void PolygonBody::AddFrustum( const sSimpleFrustm & frustum	)
{
	// near polygon ccw
	Add(new Polygon3d(	frustum.GetCorner(sSimpleFrustm::NearBottomLeft),
		frustum.GetCorner(sSimpleFrustm::NearBottomRight),
		frustum.GetCorner(sSimpleFrustm::NearTopRight),
		frustum.GetCorner(sSimpleFrustm::NearTopLeft)));

	// far polygon ccw
	Add(new Polygon3d(	frustum.GetCorner(sSimpleFrustm::FarTopLeft),
		frustum.GetCorner(sSimpleFrustm::FarTopRight),
		frustum.GetCorner(sSimpleFrustm::FarBottomRight),
		frustum.GetCorner(sSimpleFrustm::FarBottomLeft)));

	// left polygon ccw
	Add(new Polygon3d(	frustum.GetCorner(sSimpleFrustm::NearBottomLeft),
		frustum.GetCorner(sSimpleFrustm::NearTopLeft),
		frustum.GetCorner(sSimpleFrustm::FarTopLeft),
		frustum.GetCorner(sSimpleFrustm::FarBottomLeft)));

	// right polygon ccw
	Add(new Polygon3d(	frustum.GetCorner(sSimpleFrustm::NearBottomRight),
		frustum.GetCorner(sSimpleFrustm::FarBottomRight),
		frustum.GetCorner(sSimpleFrustm::FarTopRight),
		frustum.GetCorner(sSimpleFrustm::NearTopRight)));

	// bottom polygon ccw
	Add(new Polygon3d(	frustum.GetCorner(sSimpleFrustm::FarBottomLeft),
		frustum.GetCorner(sSimpleFrustm::FarBottomRight),
		frustum.GetCorner(sSimpleFrustm::NearBottomRight),
		frustum.GetCorner(sSimpleFrustm::NearBottomLeft)));

	// top polygon ccw
	Add(new Polygon3d(	frustum.GetCorner(sSimpleFrustm::FarTopRight),
		frustum.GetCorner(sSimpleFrustm::FarTopLeft),
		frustum.GetCorner(sSimpleFrustm::NearTopLeft),
		frustum.GetCorner(sSimpleFrustm::NearTopRight)));
}

void PolygonBody::AddFrustum(	ATOM_Vector3f eye, ATOM_Vector3f center, ATOM_Vector3f up, 
								float tanHalfFovY, float aspect, 
								float zNear, float zFar	)
{
#ifdef _DEBUG
	//ATOM_LOGGER::log ("%s\n", __FUNCTION__);
#endif

	sSimpleFrustm frustum(eye,center,up,tanHalfFovY,aspect,zNear,zFar);
	AddFrustum(frustum);
}

unsigned int PolygonBody::GetVertexCount() const
{
	return static_cast<unsigned int>(_uniqueVertices.size());
}

const Polygon3d::tVertexArray& PolygonBody::GetVertexArray() const
{
	return _uniqueVertices;
}

void PolygonBody::Clip(const Plane& plane)
{
	Polygon3d::tVertexArray intersectPoints;

	for (tPolygonArray::iterator polygon = _polygons.begin(); polygon != _polygons.end();)
	{
		(*polygon)->Clip(plane, intersectPoints);

		// all vertices were clipped
		if (!(*polygon)->GetVertexCount())
		{
			delete (*polygon);
			polygon = _polygons.erase(polygon);
		}
		else
			polygon++;
	}

	// add closing polygon with all 
	if (intersectPoints.size())
		Add(intersectPoints, plane.GetNormal());
}

ATOM_Vector4f GetPlaneEquation( const ATOM_BBox& boundingBox, int plane )
{
	ATOM_Vector4f equation;

	const ATOM_Vector3f & _minimum = boundingBox.getMin();
	const ATOM_Vector3f & _maximum = boundingBox.getMax();

	switch(plane)
	{
	case 0 : //Front
		equation = ATOM_Vector4f(.0, .0, 1.0, abs(_maximum.z));
		break;
	case 1 : //Back
		equation = ATOM_Vector4f(.0, .0, -1.0, abs(_minimum.z));
		break;
	case 2 : //Bottom
		equation = ATOM_Vector4f(.0, -1.0, .0, abs(_minimum.y));
		break;
	case 3 : //Top
		equation = ATOM_Vector4f(.0, 1.0, .0, abs(_maximum.y));
		break;
	case 4 : //Left
		equation = ATOM_Vector4f(-1.0, .0, .0, abs(_minimum.x));
		break;
	case 5 : //Right
		equation = ATOM_Vector4f(1.0, .0, .0, abs(_maximum.x));
		break;
	}

	return equation;
}
void PolygonBody::Clip(const ATOM_BBox& boundingBox)
{
#ifdef _DEBUG
	///ATOM_LOGGER::log ("%s\n", __FUNCTION__);
#endif
	for (unsigned int i = 0; i < 6; ++i)
		Clip( Plane( GetPlaneEquation( boundingBox, i ) ) );
}

//void PolygonBody::Clip(const sSimpleFrustm& frustum)
//{
//	for (unsigned int i = 0; i < 6; ++i)
//	{
//		const ATOM_Plane& plane = frustum.GetPlaneHessNorm(static_cast<Frustum::FrustumPlane>(i));
//		Clip(Plane(-plane.GetNormal(), plane.GetDistance()));
//	}
//}

void PolygonBody::Extrude(const ATOM_Vector3f& direction, const ATOM_BBox& boundingBox)
{
#ifdef _DEBUG
	//ATOM_LOGGER::log ("%s\n", __FUNCTION__);
#endif

	CreateUniqueVertexArray();

	const ATOM_Vector3f& maximum = boundingBox.getMax();
	const ATOM_Vector3f& minimum = boundingBox.getMin();

	unsigned int size = GetVertexCount();

	for (unsigned int i = 0; i < size; ++i)
		Intersect( Line(_uniqueVertices[i], direction), minimum, maximum, _uniqueVertices);

	// only for debugging
	ExtrudePolygons(direction, boundingBox);
}

// this function is only for debugging purposes and extrudes every polygonvertex into direction
void PolygonBody::ExtrudePolygons(const ATOM_Vector3f& direction, const ATOM_BBox& boundingBox)
{
	Polygon3d* topPoly = new Polygon3d();

	const ATOM_Vector3f& maximum = boundingBox.getMax();
	const ATOM_Vector3f& minimum = boundingBox.getMin();

	for (unsigned int i = 0; i < GetPolygonCount(); ++i)
	{
		Polygon3d::tVertexArray& vertices = const_cast<Polygon3d::tVertexArray&>(_polygons[i]->GetVertexArray());
		unsigned int size = _polygons[i]->GetVertexCount();

		Polygon3d::tVertexArray uniqueVertices;

		for (unsigned int j = 0; j < size; ++j)
		{
			unsigned int oldSz = _polygons[i]->GetVertexCount();

			Intersect(Line(vertices[j], direction), minimum, maximum, vertices);

			unsigned int newSz = _polygons[i]->GetVertexCount();
			
			if (newSz > oldSz)
				topPoly->AddVertex(_polygons[i]->GetVertexArray()[newSz-1]);
		}
	}

	_polygons.push_back(topPoly);

}

void PolygonBody::Intersect(const Line& line, const ATOM_Vector3f& min, const ATOM_Vector3f& max, Polygon3d::tVertexArray& vertices)
{
	const ATOM_Vector3f& dir = line.GetDirection();
	const ATOM_Vector3f& point = line.GetStartPoint();

	float t1 = .0f;
	float t2 = std::numeric_limits<float>::infinity();

	bool intersect =
		Clip(-dir.x, point.x-min.x, t1, t2) && Clip(dir.x, max.x-point.x, t1, t2) &&
		Clip(-dir.y, point.y-min.y, t1, t2) && Clip(dir.y, max.y-point.y, t1, t2) &&
		Clip(-dir.z, point.z-min.z, t1, t2) && Clip(dir.z, max.z-point.z, t1, t2);

	if (!intersect)
		return;

	ATOM_Vector3f newPoint;
	intersect = false;
	
	if (t1 >= .0)
	{
		newPoint = point + t1 * dir;
		intersect = true;
	}

	if (t2 >= .0)
	{
		newPoint = point + t2 * dir;
		intersect = true;
	}

	if (intersect)
		vertices.push_back(newPoint);
}

bool PolygonBody::Clip(float p, float q, float& u1, float& u2) const
{
	if (p < .0f)
	{
		float r = q / p;
		if (r > u2)
			return false;
		else
		{
			if (r > u1)
			{
				u1 = r;
			}
			return true;
		}
	}
	else
	{
		if (p > .0f)
		{
			float r = q / p;
			if (r < u1)
				return false;
			else
			{
				if (r < u2)
				{
					u2 = r;
				}
				return true;
			}
		}
		else
			return q >= .0f;
	}
}

void PolygonBody::RemoveVisiblePolygons(const ATOM_Vector3f& point)
{
	for (tPolygonArray::iterator polygon = _polygons.begin(); polygon != _polygons.end();)
	{
		// delete degenerated polygons
		if ((*polygon)->GetVertexCount() < 3)
		{
			delete (*polygon);
			polygon = _polygons.erase(polygon);
			continue;
		}
		
		// determine polygonPlane and delete polygon if its visible
		Plane polygonPlane(	(*polygon)->GetVertexArray()[0],
							(*polygon)->GetVertexArray()[1],
							(*polygon)->GetVertexArray()[2],
							Polygon3d::ccw);

		if (polygonPlane.GetDistance(point) > .0f)
		{
			delete (*polygon);
			polygon = _polygons.erase(polygon);
			continue;
		}
		polygon++;
	}	
}

void PolygonBody::CreateHull(const ATOM_Vector3f& point)
{
	RemoveVisiblePolygons(point);

	tEdgeArray uniqueEdges;

	// build array with unique edges of all polygons
	for (unsigned int i = 0; i < _polygons.size(); ++i)
	{
		unsigned int size = _polygons[i]->GetVertexCount();
		const Polygon3d::tVertexArray& vertices = _polygons[i]->GetVertexArray();

		// add edge from last to first
		AddUniqueEdge(uniqueEdges, Line(vertices[size-1], vertices[0] - vertices[size-1]));
		
		// iterate over the rest
		for (unsigned int j = 0; j < size-1; ++j)
			AddUniqueEdge(uniqueEdges, Line(vertices[j], vertices[j+1] - vertices[j]));
	}

	// build a triangle with the point and each of the unique edges
	for (unsigned int i = 0; i < uniqueEdges.size(); ++i)
	{
		Line* line = uniqueEdges[i];

		Add(new Polygon3d(line->GetStartPoint(), line->GetEndPoint(), point));

		delete uniqueEdges[i];
	}
}

//void PolygonBody::CreateConvexHull(const ATOM_Vector3f& point)
//{
//	CreateUniqueVertexArray();
//
//	_uniqueVertices.push_back(point);
//
//	CreateConvexHull(_uniqueVertices);
//}


//void PolygonBody::CreateConvexHull()
//{
//	CreateUniqueVertexArray();
//
//	CreateConvexHull(_uniqueVertices);
//}

//void PolygonBody::CreateConvexHull(const Polygon3d::tVertexArray& vertices)
//{
//	if (vertices.size() < 5)
//		return;
//
//	orgQhull::RboxPoints rPoints;
//	rPoints.setDimension(3);
//
//	for (unsigned int i = 0; i < vertices.size(); ++i)
//	{
//		rPoints.append(static_cast<double>(vertices[i].x));
//		rPoints.append(static_cast<double>(vertices[i].y));
//		rPoints.append(static_cast<double>(vertices[i].z));
//	}
//
//	Clear();
//	
//	// QJn: joggle each input coordniate by a random number in the range [-n,n]
//	// Qt: triangulate result
//	orgQhull::Qhull convexHull(rPoints, "QJ0.0001Qt");
//
//	std::vector<orgQhull::QhullFacet> triangles = convexHull.facetList().toStdVector();
//
//	for (unsigned int i = 0; i < triangles.size(); ++i)
//	{
//		orgQhull::QhullPoint first = triangles[i].vertices()[0].point();
//		orgQhull::QhullPoint second = triangles[i].vertices()[1].point();
//		orgQhull::QhullPoint third = triangles[i].vertices()[2].point();
//
//		double* nCoords = triangles[i].hyperplane().coordinates();
//		ATOM_Vector3f normal = ATOM_Vector3f(	static_cast<float>(nCoords[0]), 
//												static_cast<float>(nCoords[1]), 
//												static_cast<float>(nCoords[2])	);
//		normal.normalize();
//
//		Polygon3d::tVertex v0(static_cast<float>(first[0]), static_cast<float>(first[1]), static_cast<float>(first[2]));
//		Polygon3d::tVertex v1(static_cast<float>(second[0]), static_cast<float>(second[1]), static_cast<float>(second[2]));
//		Polygon3d::tVertex v2(static_cast<float>(third[0]), static_cast<float>(third[1]), static_cast<float>(third[2]));
//		
//		Plane plane(v0, v1, v2, Polygon3d::ccw);
//	
//		Polygon3d* polygon =  new Polygon3d();
//		polygon->AddVertex(v0);
//
//		if ( dotProduct( normal, plane.GetNormal() ) > .0f )
//		{
//			polygon->AddVertex(v1);
//			polygon->AddVertex(v2);
//		}
//		else
//		{
//			polygon->AddVertex(v2);
//			polygon->AddVertex(v1);
//		}
//		polygon->SetNormal(normal);
//
//		_polygons.push_back(polygon);
//	}
//}

void PolygonBody::AddUniqueEdge(tEdgeArray& outEdgeList, const Line& line) const
{
	bool addEdge = true;

	for (unsigned int i = 0; i < outEdgeList.size() && addEdge; ++i)
		addEdge = !(Line::CmpLines(*outEdgeList[i], line));

	if (addEdge)
		outEdgeList.push_back(new Line(line.GetStartPoint(), line.GetDirection()));
}

void PolygonBody::CreateUniqueVertexArray(float epsilon)
{
	_uniqueVertices.clear();

	for (unsigned int i = 0; i < _polygons.size(); ++i)
	{
		unsigned int size = _polygons[i]->GetVertexCount();
		const Polygon3d::tVertexArray& vertices = _polygons[i]->GetVertexArray();

		for (unsigned int j = 0; j < size; ++j)
			AddUniqueVertex(vertices[j], epsilon);
	}
}

void PolygonBody::AddUniqueVertex(const Polygon3d::tVertex& vertex, float epsilon)
{
	bool addVertex = true;

	for (unsigned int i = 0; i < _uniqueVertices.size() && addVertex; ++i)
		addVertex = !(Polygon3d::CmpVertices(_uniqueVertices[i], vertex, epsilon));

	if (addVertex)
		_uniqueVertices.push_back(vertex);
}

void PolygonBody::Clear()
{
	for (unsigned int i = 0; i < GetPolygonCount(); ++i)
		delete _polygons[i];

	_polygons.clear();
	_uniqueVertices.clear();
}
