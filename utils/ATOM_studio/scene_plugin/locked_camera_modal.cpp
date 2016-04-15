#include "StdAfx.h"
#include "camera_modal.h"
#include "locked_camera_modal.h"

LockedCamera::LockedCamera (void)
{
	_realtimeCtrl = 0;
	_zoomSpeed = 4.f;
	_moveSpeed = 4.f;
	_rotateSpeed = 1.f;
	_lat = 0.f;
	_mer = 0.f;
	_isRotating = false;
	_isZooming = false;
	_isMoving = false;

	setPosition (ATOM_Vector3f(-100.f, 100.f, -100.f));
	setDirection (ATOM_Vector3f(100.f, -100.f, 100.f));
	setUpVector (ATOM_Vector3f(0.f, 1.f, 0.f));
}

void LockedCamera::reset (ATOM_RealtimeCtrl *realtimeCtrl)
{
	_realtimeCtrl = realtimeCtrl;

	ATOM_Point2Di mousePos (_realtimeCtrl->getClientRect().size.w / 2, _realtimeCtrl->getClientRect().size.h / 2);
	ATOM_PickVisitor v;
	v.setCamera (_realtimeCtrl->getScene()->getCamera());
	v.setPickAll (true);
	_realtimeCtrl->pick (mousePos.x, mousePos.y, v);
	int hitTerrain = -1;

	if (v.getNumPicked())
	{
		v.sortResults ();
		ATOM_Terrain *terrain = 0;
		for (int i = 0; i < v.getNumPicked(); ++i)
		{
			terrain = dynamic_cast<ATOM_Terrain*>(v.getPickResult(i).node.get());
			if (terrain)
			{
				hitTerrain = i;
				break;
			}
		}
	}

	if (hitTerrain >= 0)
	{
		ATOM_Vector3f eye = _realtimeCtrl->getScene()->getCamera()->getPosition();
		ATOM_Vector3f target = eye + v.getRay().getDirection() * v.getPickResult(hitTerrain).distance;
		setPosition (target - getDirection());
	}
	else
	{
		float s = -getPosition().y / getDirection().y;
		ATOM_Vector3f target = getPosition() + getDirection() * s;
		setPosition (target - getDirection ());
	}
}

ATOM_Vector3f LockedCamera::calcMoveVector (int x_rel, int y_rel) const
{
	ATOM_Vector3f right = getViewMatrix().transformVector (ATOM_Vector3f(1.f, 0.f, 0.f));
	right.y = 0;
	right.normalize ();

	ATOM_Vector3f forward = getViewMatrix().transformVector (ATOM_Vector3f(0.f, 0.f, 1.f));
	forward.y = 0;
	forward.normalize ();

	ATOM_Vector3f up = crossProduct (forward, right);
	up.normalize ();

	return -right * x_rel * _moveSpeed + forward * y_rel * _moveSpeed;
}


bool LockedCamera::handleKeyDown (ATOM_Key key, unsigned keymod)
{
	return false;
}

bool LockedCamera::handleKeyUp (ATOM_Key key, unsigned keymod)
{
	return false;
}

bool LockedCamera::handleMouseButtonDown (ATOM_MouseButton button, int x, int y, unsigned keymod)
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
			_posRef = getPosition();
			_centerRef = getPosition() - _dirC;
			_lat = 0.f;
			_mer = 0.f;
			break;
		}
	case BUTTON_MIDDLE:
		{
			_isMoving = true;
			_posRef = getPosition() + getDirection();
			break;
		}
	case BUTTON_RIGHT:
		{
			if (_isZooming)
			{
				return false;
			}
			_isZooming = true;
			_posRef = getPosition();
			_centerRef = getPosition() + getDirection();
			break;
		}
	default:
		return false;
	}

	_lastMouseX = x;
	_lastMouseY = y;

	return true;
}

bool LockedCamera::handleMouseButtonUp (ATOM_MouseButton button, int x, int y, unsigned keymod)
{
	switch (button)
	{
	case BUTTON_LEFT:
		{
			if (_isRotating)
			{
				_isRotating = false;
				::ReleaseCapture ();
				return true;
			}
			break;
		}
	case BUTTON_MIDDLE:
		{
			if (_isMoving)
			{
				_isMoving = false;
				::ReleaseCapture ();
				return true;
			}
			break;
		}
	case BUTTON_RIGHT:
		{
			if (_isZooming)
			{
				_isZooming = false;
				::ReleaseCapture ();
				return true;
			}
			break;
		}
	}

	return false;
}

bool LockedCamera::handleMouseMove (int x, int y)
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

			setPosition (_centerRef + dir);
			setDirection (-dir);
		}
		return true;
	}
	else if (_isMoving)
	{
		ATOM_Vector3f newTarget = _posRef + calcMoveVector (x - _lastMouseX, y - _lastMouseY);

		ATOM_PickVisitor v;
		ATOM_Vector3f o = newTarget + ATOM_Vector3f(0.f, 5000.f, 0.f);
		ATOM_Ray ray(o, ATOM_Vector3f(0.f, -1.f, 0.f));
		v.setRay (ray);
		v.setCamera (_realtimeCtrl->getScene()->getCamera());
		v.setPickAll (true);
		ATOM_Node *root = _realtimeCtrl->getScene()->getRootNode();
		if (root)
		{
			v.traverse (*root);
			int hitTerrain = -1;

			if (v.getNumPicked())
			{
				v.sortResults ();
				ATOM_Terrain *terrain = 0;
				for (int i = 0; i < v.getNumPicked(); ++i)
				{
					terrain = dynamic_cast<ATOM_Terrain*>(v.getPickResult(i).node.get());
					if (terrain)
					{
						hitTerrain = i;
						break;
					}
				}
			}

			if (hitTerrain >= 0)
			{
				ATOM_Vector3f target = o + ATOM_Vector3f (0.f, -v.getPickResult(hitTerrain).distance, 0.f);
				setPosition (target - getDirection());
			}
			else
			{
				ATOM_Vector3f target = o;
				target.y = 0;
				setPosition (target - getDirection ());
			}
			return true;
		}
	}
	else if (_isZooming)
	{
		float dist = ((x - _lastMouseX) + (y - _lastMouseY)) * _zoomSpeed;
		ATOM_Vector3f offset(0.f, 0.f, dist);
		ATOM_Vector3f offsetW = getViewMatrix().transformVector(offset);
		ATOM_Vector3f newPosition = _posRef + offsetW;
		ATOM_Vector3f newDirection = _centerRef - newPosition;
		setPosition (newPosition);
		setDirection (newDirection);
		return true;
	}

	return false;
}

bool LockedCamera::handleMouseWheel (int x, int y, int delta, unsigned keymod)
{
	if (_isZooming || _isRotating || _isMoving)
	{
		return false;
	}

	float s = 1.f - delta * 0.05f;
	if (s < 0.f)
	{
		s = 0.f;
	}
	float oldLen = getDirection().getLength();
	float newLen = getDirection().getLength() * s;
	if (newLen < 1.f)
	{
		newLen = 1.f;
	}
	ATOM_Vector3f center = getPosition() + getDirection();
	ATOM_Vector3f newDirection = getDirection() * newLen / oldLen;
	setDirection (newDirection);
	setPosition (center - newDirection);
	return true;
}

void LockedCamera::setupOptions (ATOMX_TweakBar *bar)
{
	setupPerspectiveOptions (bar);
	setupFixedDirectionOptions (bar);
}

void LockedCamera::setupFixedDirectionOptions (ATOMX_TweakBar *bar)
{
	bar->addFloatVar ("Distance", LCO_FIXEDLENGTH, getDirection().getLength(), false, "Transform");
	bar->setVarMinMax ("Distance", -10000.f, 10000.f);
	bar->addDirVar ("Direction", LCO_FIXEDDIRECTION, getDirection().x, getDirection().y, getDirection().z, false, "Transform");
}

void LockedCamera::onParameterChanged (ATOMX_TWValueChangedEvent *event)
{
	switch (event->id)
	{
	case LCO_FIXEDLENGTH:
		{
			float distance = event->newValue.getF ();
			ATOM_Vector3f newDir = event->bar->get3F ("Direction");
			newDir.normalize ();
			ATOM_Vector3f target = getPosition() + getDirection();
			ATOM_Vector3f newDirection = newDir * distance;
			ATOM_Vector3f newPosition = target - newDirection;
			setPosition (newPosition);
			setDirection (newDirection);
			break;
		}
	case LCO_FIXEDDIRECTION:
		{
			const float *newDir = event->newValue.get3F();
			float distance = event->bar->getF ("Distance");
			ATOM_Vector3f target = getPosition() + getDirection();
			ATOM_Vector3f newDirection = ATOM_Vector3f(newDir[0], newDir[1], newDir[2]);
			newDirection.normalize ();
			newDirection *= distance;
			ATOM_Vector3f newPosition = target - newDirection;
			setPosition (newPosition);
			setDirection (newDirection);
			break;
		}
	default:
		{
			AS_CameraModal::onParameterChanged (event);
			break;
		}
	}
}

void LockedCamera::updateOptions (ATOMX_TweakBar *bar)
{
	updatePerspectiveOptions (bar);
	updateFixedDirectionOptions (bar);
}

void LockedCamera::updateFixedDirectionOptions (ATOMX_TweakBar *bar)
{
	bar->setF("Distance", getDirection().getLength());
	ATOM_Vector3f v = getDirection();
	v.normalize ();
	bar->set3F ("Direction", v);
}

void LockedCamera::setZoomSpeed (float val)
{
	_zoomSpeed = val;
}

float LockedCamera::getZoomSpeed (void) const
{
	return _zoomSpeed;
}

void LockedCamera::setMoveSpeed (float val)
{
	_moveSpeed = val;
}

float LockedCamera::getMoveSpeed (void) const
{
	return _moveSpeed;
}

void LockedCamera::setRotateSpeed (float val)
{
	_rotateSpeed = val;
}

float LockedCamera::getRotateSpeed (void)
{
	return _rotateSpeed;
}

void LockedCamera::setRotateRadius (float val)
{
}

float LockedCamera::getRotateRadius (void) const
{
	return 0.f;
}

