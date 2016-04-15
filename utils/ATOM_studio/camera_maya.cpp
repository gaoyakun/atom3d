#include "StdAfx.h"
#include "camera_modal.h"
#include "camera_maya.h"

CameraMaya::CameraMaya (void)
{
	_zoomSpeed = 4.f;
	_moveSpeed = 4.f;
	_rotateSpeed = 1.f;
	_lat = 0.f;
	_mer = 0.f;
	_isRotating = false;
	_isZooming = false;
	_isMoving = false;
	_width = 0;
	_height = 0;
	_rotateRadius = 100.f;
}

void CameraMaya::reset (ATOM_RealtimeCtrl *realtimeCtrl)
{
	_width = realtimeCtrl->getClientRect().size.w;
	_height = realtimeCtrl->getClientRect().size.h;

	_isRotating = false;
	_isZooming = false;
	_isMoving = false;
}

bool CameraMaya::handleKeyDown (ATOM_Key key, unsigned keymod)
{
	return false;
}

bool CameraMaya::handleKeyUp (ATOM_Key key, unsigned keymod)
{
	return false;
}

bool CameraMaya::handleMouseButtonDown (ATOM_MouseButton button, int x, int y, unsigned keymod)
{
	if (keymod != KEYMOD_LALT && keymod != KEYMOD_RALT)
	{
		return false;
	}

	switch (button)
	{
	case BUTTON_LEFT:
		{
			if (_isRotating)
			{
				return false;
			}
			_isRotating = true;
			_dirC = -getDirection();
			_dirC.normalize ();
			_dirC *= _rotateRadius;
			_posRef = getPosition();
			_centerRef = getPosition() - _dirC;
			_lat = 0.f;
			_mer = 0.f;
			break;
		}
	case BUTTON_MIDDLE:
		if (_isMoving)
		{
			return false;
		}
		_isMoving = true;
		_posRef = getPosition();
		break;
	case BUTTON_RIGHT:
		if (_isZooming)
		{
			return false;
		}
		_isZooming = true;
		_posRef = getPosition();
		break;
	default:
		return false;
	}

	_lastMouseX = x;
	_lastMouseY = y;

	return true;
}

bool CameraMaya::handleMouseButtonUp (ATOM_MouseButton button, int x, int y, unsigned keymod)
{
	switch (button)
	{
	case BUTTON_LEFT:
		if (_isRotating)
		{
			_isRotating = false;
			::ReleaseCapture ();
			return true;
		}
		break;
	case BUTTON_MIDDLE:
		if (_isMoving)
		{
			_isMoving = false;
			::ReleaseCapture ();
			return true;
		}
		break;
	case BUTTON_RIGHT:
		if (_isZooming)
		{
			_isZooming = false;
			::ReleaseCapture ();
			return true;
		}
		break;
	}

	return false;
}

bool CameraMaya::handleMouseMove (int x, int y)
{
	if (_isRotating)
	{
		int rel_x = x - _lastMouseX;
		int rel_y = y - _lastMouseY;
		if (rel_x != 0 || rel_y != 0)
		{
			_lastMouseX = x;
			_lastMouseY = y;

			_lat += _rotateSpeed * rel_x * 0.01f;
			_mer += _rotateSpeed * rel_y * 0.01f;

			ATOM_Vector3f axis(-_dirC.z, 0.f, _dirC.x);
			axis.normalize ();
			ATOM_Matrix4x4f m44Mer;
			m44Mer.makeRotateAngleNormalizedAxis (_mer, axis);
			ATOM_Vector3f dir = m44Mer.transformPoint (_dirC);

			ATOM_Matrix4x4f m44Lat = ATOM_Matrix4x4f::getRotateYMatrix (_lat);
			dir = m44Lat.transformPoint (dir);

			//setPosition (_centerRef + dir);
			setDirection (-dir);
		}
		return true;
	}
	else if (_isMoving)
	{
		ATOM_Vector4f offset((x - _lastMouseX) * _moveSpeed, (_lastMouseY - y) * _moveSpeed, 0.f, 0.f);
		ATOM_Vector4f offsetW = getViewMatrix() >> offset;
		setPosition (_posRef - ATOM_Vector3f(offsetW.x, offsetW.y, offsetW.z));

		return true;
	}
	else if (_isZooming)
	{
		float dist = ((x - _lastMouseX) + (y - _lastMouseY)) * _zoomSpeed;
		ATOM_Vector4f offset(0.f, 0.f, dist, 0.f);
		ATOM_Vector4f offsetW = getViewMatrix() >> offset;
		setPosition (_posRef + ATOM_Vector3f(offsetW.x, offsetW.y, offsetW.z));

		return true;
	}

	return false;
}

bool CameraMaya::handleMouseWheel (int x, int y, int delta, unsigned keymod)
{
	if (_isZooming)
	{
		return false;
	}

	float dist = delta * _zoomSpeed * 10;
	ATOM_Vector4f offset(0.f, 0.f, dist, 0.f);
	ATOM_Vector4f offsetW = getViewMatrix() >> offset;
	setPosition (getPosition() + ATOM_Vector3f(offsetW.x, offsetW.y, offsetW.z));

	return true;
}

void CameraMaya::setZoomSpeed (float val)
{
	_zoomSpeed = val;
}

float CameraMaya::getZoomSpeed (void) const
{
	return _zoomSpeed;
}

void CameraMaya::setMoveSpeed (float val)
{
	_moveSpeed = val;
}

float CameraMaya::getMoveSpeed (void) const
{
	return _moveSpeed;
}

void CameraMaya::setRotateSpeed (float val)
{
	_rotateSpeed = val;
}

float CameraMaya::getRotateSpeed (void)
{
	return _rotateSpeed;
}

void CameraMaya::setRotateRadius (float val)
{
	_rotateRadius = val;
}

float CameraMaya::getRotateRadius (void) const
{
	return _rotateRadius;
}


