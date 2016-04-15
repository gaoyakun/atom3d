#ifndef __ATOM3D_STUDIO_CAMREA_MAYA_H
#define __ATOM3D_STUDIO_CAMREA_MAYA_H

#if _MSC_VER > 1000
# pragma once
#endif

class CameraMaya: public AS_CameraModal
{
public:
	CameraMaya (void);

public:
	virtual void reset (ATOM_RealtimeCtrl *realtimeCtrl);
	virtual bool handleKeyDown (ATOM_Key key, unsigned keymod);
	virtual bool handleKeyUp (ATOM_Key key, unsigned keymod);
	virtual bool handleMouseButtonDown (ATOM_MouseButton button, int x, int y, unsigned keymod);
	virtual bool handleMouseButtonUp (ATOM_MouseButton button, int x, int y, unsigned keymod);
	virtual bool handleMouseMove (int x, int y);
	virtual bool handleMouseWheel (int x, int y, int delta, unsigned keymod);
	virtual void setZoomSpeed (float val);
	virtual float getZoomSpeed (void) const;
	virtual void setMoveSpeed (float val);
	virtual float getMoveSpeed (void) const;
	virtual void setRotateSpeed (float val);
	virtual float getRotateSpeed (void);
	virtual void setRotateRadius (float val);
	virtual float getRotateRadius (void) const;

private:
	float _zoomSpeed;
	float _moveSpeed;
	float _rotateSpeed;
	float _lat;
	float _mer;
	bool _isRotating;
	bool _isZooming;
	bool _isMoving;
	unsigned _width;
	unsigned _height;
	int _lastMouseX;
	int _lastMouseY;
	ATOM_Vector3f _posRef;
	ATOM_Vector3f _centerRef;
	ATOM_Vector3f _dirC;
	float _rotateRadius;
};

#endif // __ATOM3D_STUDIO_CAMREA_MAYA_H