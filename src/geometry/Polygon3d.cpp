#include <algorithm>

#include "Line.h"
#include "Plane_ext.h"
#include "Polygon3d.h"

Polygon3d::Polygon3d()
{
}

Polygon3d::Polygon3d(const tVertex& p0, const tVertex& p1, const tVertex& p2)
{
	_vertices.push_back(p0);
	_vertices.push_back(p1);
	_vertices.push_back(p2);
}

Polygon3d::Polygon3d(const tVertex& p0, const tVertex& p1, const tVertex& p2, const tVertex& p3)
{
	_vertices.push_back(p0);
	_vertices.push_back(p1);
	_vertices.push_back(p2);
	_vertices.push_back(p3);
}

unsigned int Polygon3d::GetVertexCount() const
{
	return static_cast<unsigned int>(_vertices.size());
}

const Polygon3d::tVertexArray& Polygon3d::GetVertexArray() const
{
	return _vertices;
}

void Polygon3d::AddVertex(const tVertex& vertex)
{
	_vertices.push_back(vertex);
}

void Polygon3d::AddUniqueVertex(const tVertex& vertex, float epsilon)
{
	bool addVertex = true;

	for (unsigned int i = 0; i < _vertices.size() && addVertex; ++i)
		addVertex = !(CmpVertices(vertex, _vertices[i], epsilon));

	if (addVertex)
		_vertices.push_back(vertex);
}

void Polygon3d::Clip(const Plane& plane, tVertexArray& intersectionPoints)
{
	// normally should not happen ;-)
	if (_vertices.size() < 3)
		return;

	tVertexArray newVertices;
	
	//the polygon is clipped to the part behind the plane
	for (unsigned int i = 0; i < _vertices.size(); ++i)
	{		
		unsigned int next = (i + 1) % _vertices.size();

		bool currentOut = !plane.IsBehind(_vertices[i]);
		bool nextOut = !plane.IsBehind(_vertices[next]);

		// both outside
		if (currentOut && nextOut)
			continue;

		float param = .0f;
		Line line(_vertices[i], _vertices[next]-_vertices[i]);

		// calculate intersection outside to inside
		if (currentOut)
		{
			if (plane.Intersect(param, line))
			{
				ATOM_Vector3f intersection = line.GetPoint(param);
				newVertices.push_back(intersection);
				intersectionPoints.push_back(intersection);
			}
			newVertices.push_back(_vertices[next]);
			continue;
		}

		// calculate intersection inside to outside
		if (nextOut)
		{
			if (plane.Intersect(param, line))
			{
				ATOM_Vector3f intersection = line.GetPoint(param);
				newVertices.push_back(intersection);
				intersectionPoints.push_back(intersection);
			}
			continue;
		}

		// both inside
		newVertices.push_back(_vertices[next]);
	}

	_vertices.clear();
	
	// leave polygon if degenerated
	if (newVertices.size() > 2)
		_vertices = newVertices;
}

void Polygon3d::ReverseVertices()
{
	if (_vertices.size() > 2)
		std::reverse(_vertices.begin(), _vertices.end());
}

const ATOM_Vector3f& Polygon3d::GetNormal()
{
	return _normal;
}

void Polygon3d::SetNormal(const ATOM_Vector3f& normal)
{
	if (normal.getLength() == 0)
		return;

	_normal = normal;
	_normal.normalize();
}

bool Polygon3d::CmpVertices(const ATOM_Vector3f& v0, const ATOM_Vector3f& v1, float epsilon)
{
	return (CmpCoords(v0.x, v1.x, epsilon)
		&& CmpCoords(v0.y, v1.y, epsilon)
		&& CmpCoords(v0.z, v1.z, epsilon));
}

bool Polygon3d::CmpCoords(float c0, float c1, float epsilon)
{
	if (c0 == c1)
		return true;

	if (((c0 + epsilon) < c1) || ((c0 - epsilon) > c1))
		return false;

	return true;
}
