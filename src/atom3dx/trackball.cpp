#include "StdAfx.h"
#include "trackball.h"

ATOMX_Trackball::ATOMX_Trackball (int w, int h, float radius)
{
	reset ();
	_vDown.set(0, 0, 0);
	_vCurrent.set(0, 0, 0);
	_offset[0] = 0;
	_offset[1] = 0;
	setWindow (w, h, radius);
}

void ATOMX_Trackball::reset (void)
{
	_quatDown.identity ();
	_quatNow.identity ();
	_rotation.makeIdentity ();
	_translation.makeIdentity ();
	_translationDelta.makeIdentity ();
	_dragged = false;
	_radiusTranslation = 1.f;
	_radius = 1.f;
}

void ATOMX_Trackball::setTranslationRadius (float radius) 
{ 
	_radiusTranslation = radius; 
}

void ATOMX_Trackball::setWindow (int width, int height, float radius)
{
	_width = width;
	_height = height;
	_radius = radius;
}

void ATOMX_Trackball::setOffset (int x, int y)
{
	_offset[0] = x;
	_offset[1] = y;
}

void ATOMX_Trackball::begin (int x, int y)
{
	if (x >= _offset[0] && x < _offset[0] + _width && y >= _offset[1] && y < _offset[1] + _height)
	{
		_dragged = true;
		_quatDown = _quatNow;
		_vDown = screenToVector ((float)x, (float)y);
	}
}

void ATOMX_Trackball::move (int x, int y)
{
	if (_dragged)
	{
		_vCurrent = screenToVector ((float)x, (float)y);
		_quatNow = _quatDown >> quatFromBallPoints (_vCurrent, _vDown);
	}
}

void ATOMX_Trackball::end (void)
{
	_dragged = false;
}

ATOM_Matrix4x4f ATOMX_Trackball::getRotationMatrix (void) const
{
	ATOM_Matrix4x4f matrix;
	_quatNow.toMatrix (matrix);
	return matrix;
}

const ATOM_Matrix4x4f & ATOMX_Trackball::getTranslationMatrix (void) const
{
	return _translation;
}

const ATOM_Matrix4x4f & ATOMX_Trackball::getTranslationDeltaMatrix (void) const
{
	return _translationDelta;
}

bool ATOMX_Trackball::isBeginDragged (void) const
{
	return _dragged;
}

const ATOM_Quaternion &ATOMX_Trackball::getQuaternionNow (void) const
{
	return _quatNow;
}

void ATOMX_Trackball::setQuaternionNow (const ATOM_Quaternion &q)
{
	_quatNow = q;
}

ATOM_Quaternion ATOMX_Trackball::quatFromBallPoints (const ATOM_Vector3f &from, const ATOM_Vector3f &to)
{
	float dot = dotProduct (from, to);
	ATOM_Vector3f part = crossProduct(from, to);
	return ATOM_Quaternion (part.x, part.y, part.z, dot);
}

ATOM_Vector3f ATOMX_Trackball::screenToVector (float x, float y)
{
	float X = -(x - _offset[0] - _width/2) / (_radius * _width / 2);
	float Y = (y - _offset[1] - _height/2) / (_radius * _height / 2);
	float Z = 0.f;
	float mag = X * X + Y * Y;
	if (mag > 1.f)
	{
		float scale = 1.f / ATOM_sqrt(mag);
		X *= scale;
		Y *= scale;
	}
	else
	{
		Z = ATOM_sqrt (1.f - mag);
	}
	return ATOM_Vector3f(X, Y, Z);
}
