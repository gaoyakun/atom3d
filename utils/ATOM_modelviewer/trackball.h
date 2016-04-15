#ifndef __TRACKBALL_H__
#define __TRACKBALL_H__

#include <ATOM_math.h>

class Trackball
{
public:
	Trackball (int w, int h, float radius = 0.9f)
	{
		reset ();
		_vDown.set(0, 0, 0);
		_vCurrent.set(0, 0, 0);
		_offset[0] = 0;
		_offset[1] = 0;
		setWindow (w, h, radius);
	}

	void reset (void)
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

	void setTranslationRadius (float radius) 
	{ 
		_radiusTranslation = radius; 
	}

	void setWindow (int width, int height, float radius = 0.9f)
	{
		_width = width;
		_height = height;
		_radius = radius;
	}

	void setOffset (int x, int y)
	{
		_offset[0] = x;
		_offset[1] = y;
	}

	void begin (int x, int y)
	{
		if (x >= _offset[0] && x < _offset[0] + _width && y >= _offset[1] && y < _offset[1] + _height)
		{
			_dragged = true;
			_quatDown = _quatNow;
			_vDown = screenToVector ((float)x, (float)y);
		}
	}

	void move (int x, int y)
	{
		if (_dragged)
		{
			_vCurrent = screenToVector ((float)x, (float)y);
			_quatNow = _quatDown >> quatFromBallPoints (_vCurrent, _vDown);
		}
	}

	void end (void)
	{
		_dragged = false;
	}

	ATOM_Matrix4x4f getRotationMatrix (void) const
	{
		ATOM_Matrix4x4f matrix;
		_quatNow.toMatrix (matrix);
		return matrix;
	}

	const ATOM_Matrix4x4f & getTranslationMatrix (void) const
	{
		return _translation;
	}

	const ATOM_Matrix4x4f & getTranslationDeltaMatrix (void) const
	{
		return _translationDelta;
	}

	bool isBeginDragged (void) const
	{
		return _dragged;
	}

	const ATOM_Quaternion &getQuaternionNow (void) const
	{
		return _quatNow;
	}

	void setQuaternionNow (const ATOM_Quaternion &q)
	{
		_quatNow = q;
	}

	static ATOM_Quaternion quatFromBallPoints (const ATOM_Vector3f &from, const ATOM_Vector3f &to)
	{
		float dot = dotProduct (from, to);
		ATOM_Vector3f part = crossProduct(from, to);
		return ATOM_Quaternion (part.x, part.y, part.z, dot);
	}

protected:
	ATOM_Matrix4x4f _rotation;
	ATOM_Matrix4x4f _translation;
	ATOM_Matrix4x4f _translationDelta;

	int _offset[2];
	int _width;
	int _height;
	ATOM_Vector2f _center;
	float _radius;
	float _radiusTranslation;
	ATOM_Quaternion _quatDown;
	ATOM_Quaternion _quatNow;
	bool _dragged;

	int _lastMouse[2];
	ATOM_Vector3f _vDown;
	ATOM_Vector3f _vCurrent;

	ATOM_Vector3f screenToVector (float x, float y)
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
};


#endif // __TRACKBALL_H__
