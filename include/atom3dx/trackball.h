#ifndef __ATOM3DX_TRACKBALL_H
#define __ATOM3DX_TRACKBALL_H

#include "../atom3d/ATOM_math.h"
#include "basedefs.h"

class ATOMX_API ATOMX_Trackball
{
public:
	ATOMX_Trackball (int w, int h, float radius = 0.9f);
	void reset (void);
	void setTranslationRadius (float radius);
	void setWindow (int width, int height, float radius = 0.9f);
	void setOffset (int x, int y);
	void begin (int x, int y);
	void move (int x, int y);
	void end (void);
	ATOM_Matrix4x4f getRotationMatrix (void) const;
	const ATOM_Matrix4x4f & getTranslationMatrix (void) const;
	const ATOM_Matrix4x4f & getTranslationDeltaMatrix (void) const;
	bool isBeginDragged (void) const;
	const ATOM_Quaternion &getQuaternionNow (void) const;
	void setQuaternionNow (const ATOM_Quaternion &q);
	static ATOM_Quaternion quatFromBallPoints (const ATOM_Vector3f &from, const ATOM_Vector3f &to);

private:
	ATOM_Vector3f screenToVector (float x, float y);

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
};

#endif // __ATOM3DX_TRACKBALL_H
