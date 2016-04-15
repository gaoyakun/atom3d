#ifndef __ATOM3D_STUDIO_LOCKED_CAMERA_MODAL_H
#define __ATOM3D_STUDIO_LOCKED_CAMERA_MODAL_H

#if _MSC_VER > 1000
# pragma once
#endif

class LockedCamera: public AS_CameraModal
{
public:
	enum
	{
		LCO_BEGIN = AS_CameraModal::CM_END,		// wangjian modified : ´ÓAS_CameraModal::CM_END¿ªÊ¼
		LCO_FIXEDDIRECTION = LCO_BEGIN,
		LCO_FIXEDLENGTH,
		LCO_END
	};

public:
	LockedCamera (void);

public:
	virtual void reset (ATOM_RealtimeCtrl *realtimeCtrl);
	virtual bool handleKeyDown (ATOM_Key key, unsigned keymod);
	virtual bool handleKeyUp (ATOM_Key key, unsigned keymod);
	virtual bool handleMouseButtonDown (ATOM_MouseButton button, int x, int y, unsigned keymod);
	virtual bool handleMouseButtonUp (ATOM_MouseButton button, int x, int y, unsigned keymod);
	virtual bool handleMouseMove (int x, int y);
	virtual bool handleMouseWheel (int x, int y, int delta, unsigned keymod);
	virtual void setupOptions (ATOMX_TweakBar *bar);
	virtual void setupFixedDirectionOptions (ATOMX_TweakBar *bar);
	virtual void onParameterChanged (ATOMX_TWValueChangedEvent *event);
	virtual void updateOptions (ATOMX_TweakBar *bar);
	virtual void updateFixedDirectionOptions (ATOMX_TweakBar *bar);
	virtual void setZoomSpeed (float val);
	virtual float getZoomSpeed (void) const;
	virtual void setMoveSpeed (float val);
	virtual float getMoveSpeed (void) const;
	virtual void setRotateSpeed (float val);
	virtual float getRotateSpeed (void);
	virtual void setRotateRadius (float val);
	virtual float getRotateRadius (void) const;

private:
	ATOM_Vector3f calcMoveVector (int x_rel, int y_rel) const;

private:
	ATOM_RealtimeCtrl *_realtimeCtrl;
	float _zoomSpeed;
	float _moveSpeed;
	float _rotateSpeed;
	float _lat;
	float _mer;
	bool _isRotating;
	bool _isZooming;
	bool _isMoving;
	int _lastMouseX;
	int _lastMouseY;
	ATOM_Vector3f _posRef;
	ATOM_Vector3f _centerRef;
	ATOM_Vector3f _dirC;
};

#endif // __ATOM3D_STUDIO_LOCKED_CAMERA_MODAL_H
