#include "Line.h"
#include "Polygon3d.h"

Line::Line(const ATOM_Vector3f& point, const ATOM_Vector3f& direction) :
	_startPoint(point),
	_direction(direction),
	_endPoint(point + direction)
{
}

const ATOM_Vector3f& Line::GetStartPoint() const
{
	return _startPoint;
}

const ATOM_Vector3f& Line::GetEndPoint() const
{
	return _endPoint;
}

ATOM_Vector3f Line::GetPoint(float parameter) const
{
	return ATOM_Vector3f(_startPoint + parameter * _direction);
}

void Line::SetStartPoint(const ATOM_Vector3f& point)
{
	_startPoint = point;
	_endPoint = _startPoint + _direction;
}

const ATOM_Vector3f& Line::GetDirection() const
{
	return _direction;
}

void Line::SetDirection(const ATOM_Vector3f& direction)
{
	_direction = direction;
	_endPoint = _startPoint + _direction;
}

bool Line::CmpLines(const Line& l0, const Line& l1, float epsilon)
{
	if (Polygon3d::CmpVertices(l0.GetStartPoint(), l1.GetStartPoint(), epsilon) &&
		Polygon3d::CmpVertices(l0.GetEndPoint(), l1.GetEndPoint(), epsilon))
		return true;

	if (Polygon3d::CmpVertices(l0.GetStartPoint(), l1.GetEndPoint(), epsilon) &&
		Polygon3d::CmpVertices(l0.GetEndPoint(), l1.GetStartPoint(), epsilon))
		return true;

	return false;
}
