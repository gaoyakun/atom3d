#include "Line.h"
#include "Plane_ext.h"

Plane::Plane(const ATOM_Vector4f& equation) :
	_normal(ATOM_Vector3f(equation.x,equation.y,equation.z)),
	_distance(equation.w)
{
	_normal.normalize();
}

Plane::Plane(const ATOM_Vector3f& normal, float distance) :
	_normal(normal),
	_distance(distance)
{
	_normal.normalize();
}

Plane::Plane(const ATOM_Vector3f& v0, const ATOM_Vector3f& v1, const ATOM_Vector3f& v2, Polygon3d::VertexOrder order)
{
	switch(order)
	{
		case Polygon3d::ccw:
			_normal = crossProduct(v1-v0, v2-v0);
			_normal.normalize();
			break;
		case Polygon3d::cw:
			_normal = crossProduct(v2-v0, v1-v0);
			_normal.normalize();
			break;
	}
	_distance = dotProduct(v0, _normal);
}

Plane::Plane(const ATOM_Vector3f& point, const ATOM_Vector3f& normal)
{
	_normal = normal;
	_normal.normalize();

	_distance = dotProduct(_normal, point);
}

const ATOM_Vector4f& Plane::GetEquation()
{
	_equation = ATOM_Vector4f(_normal.x,_normal.y,_normal.z, _distance);
	return _equation;
}

void Plane::SetEquation(const ATOM_Vector4f& equation)
{
	_normal = ATOM_Vector3f(equation.x,equation.y,equation.z);
	_normal.normalize();
	_distance = equation.w;
}

const ATOM_Vector3f& Plane::GetNormal() const
{
	return _normal;
}

float Plane::GetDistance() const
{
	return _distance;
}

float Plane::GetDistance(const ATOM_Vector3f& point) const
{
	return dotProduct(point, _normal) - _distance;
}

bool Plane::IsBehind(const ATOM_Vector3f& point) const
{
	bool behind = false;

	if (GetDistance(point) < .0)
		behind = true;

	return behind;
}

bool Plane::Intersect(float& param, const Line& line, float epsilon) const
{
	float prod = dotProduct(_normal, line.GetDirection());
	
	// line and plane closely parallel
	if (abs(prod) < epsilon)
	{
		param = .0f;
		return false;
	}

	param = (_distance - dotProduct(_normal, line.GetStartPoint())) / prod;

	// intersection outside of line
	if(param < -epsilon || param > (1.0f + epsilon))
	{
		param = .0;
		return false;
	}

	return true;
}

void Plane::Transform(const ATOM_Matrix4x4f& matrix)
{
	ATOM_Vector4f transEquation(_normal.x,_normal.y,_normal.z, -_distance);
	transEquation = matrix >> transEquation;

	_normal = ATOM_Vector3f(transEquation.x,transEquation.y,transEquation.z);
	_distance = -transEquation.w;

	CalcHessNorm();
}

void Plane::CalcHessNorm()
{
	float len = _normal.getLength();
	
	_normal = _normal / len;
	_distance = _distance / len;
}

