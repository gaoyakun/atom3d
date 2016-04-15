#include "StdAfx.h"
#include "camera_modal.h"

AS_CameraModal::AS_CameraModal (void)
{
	_position.set (0.f, 0.f, -1.f);
	_direction.set (0.f, 0.f, 0.f);
	_up.set (0.f, 1.f, 0.f);
	_matrixDirty = true;

	_nearPlane = 1.f;
	_farPlane = 10000.f;
	_fovY = ATOM_Pi / 3.f;
	_aspect = 1.f;
	_projmatrixDirty = true;
}

void AS_CameraModal::setPosition (const ATOM_Vector3f &pos)
{
	_position = pos;
	_matrixDirty = true;
}

const ATOM_Vector3f &AS_CameraModal::getPosition (void) const
{
	return _position;
}

void AS_CameraModal::setDirection (const ATOM_Vector3f &dir)
{
	_direction = dir;
	_matrixDirty = true;
}

const ATOM_Vector3f &AS_CameraModal::getDirection (void) const
{
	return _direction;
}

void AS_CameraModal::setUpVector (const ATOM_Vector3f &up)
{
	_up = up;
	_matrixDirty = true;
}

const ATOM_Vector3f &AS_CameraModal::getUpVector (void) const
{
	return _up;
}

const ATOM_Matrix4x4f &AS_CameraModal::getViewMatrix (void) const
{
	if (_matrixDirty)
	{
		_matrixDirty = false;
		_matrix.makeLookatLH (_position, _position + _direction, _up);
	}
	return _matrix;
}

void AS_CameraModal::setAspect (float aspect)
{
	_aspect = aspect;
	_projmatrixDirty = true;
}

float AS_CameraModal::getAspect (void) const
{
	return _aspect;
}

void AS_CameraModal::setFovY (float rad)
{
	_fovY = rad;
	_projmatrixDirty = true;
}

float AS_CameraModal::getFovY (void) const
{
	return _fovY;
}

void AS_CameraModal::setNearPlane (float np)
{
	_nearPlane = np;
	_projmatrixDirty = true;
}

float AS_CameraModal::getNearPlane (void) const
{
	return _nearPlane;
}

void AS_CameraModal::setFarPlane (float fp)
{
	_farPlane = fp;
	_projmatrixDirty = true;
}

float AS_CameraModal::getFarPlane (void) const
{
	return _farPlane;
}

const ATOM_Matrix4x4f &AS_CameraModal::getProjectionMatrix (void) const
{
	if (_projmatrixDirty)
	{
		_projmatrixDirty = false;
		_projmatrix.makePerspectiveFovLH (_fovY, _aspect, _nearPlane, _farPlane);
	}
	return _projmatrix;
}


void AS_CameraModal::setupOptions (ATOMX_TweakBar *bar)
{
	setupPerspectiveOptions (bar);
	setupTransformOptions (bar);
}

void AS_CameraModal::setupPerspectiveOptions (ATOMX_TweakBar *bar)
{
	bar->addFloatVar ("FOV", CM_FOV, getFovY() * 180.f / 3.1415926f, false, "Perspective");
	bar->setVarMinMax ("FOV", 1.f, 180.f);
	bar->setVarStep ("FOV", 1.f);
	bar->addFloatVar ("Near", CM_NEAR, getNearPlane(), false, "Perspective");
	bar->setVarMinMax ("Near", 0.f, 10000000.f);
	bar->addFloatVar ("Far", CM_FAR, getFarPlane(), false, "Perspective");
	bar->setVarMinMax ("Far", 0.f, 10000000.f);
}

void AS_CameraModal::setupTransformOptions (ATOMX_TweakBar *bar)
{
	bar->addVector3fVar ("Position", CM_POSITION, getPosition(), false, "Transform");
	bar->addDirVar ("Direction", CM_DIRECTION, getDirection().x, getDirection().y, getDirection().z, false, "Transform");
}

void AS_CameraModal::onParameterChanged (ATOMX_TWValueChangedEvent *event)
{
	switch (event->id)
	{
	case CM_FOV:
		{
			setFovY (event->newValue.getF() * 3.1415926f / 180.f);
			break;
		}
	case CM_NEAR:
		{
			setNearPlane (event->newValue.getF());
			break;
		}
	case CM_FAR:
		{
			setFarPlane (event->newValue.getF());
			break;
		}
	case CM_POSITION:
		{
			setPosition (ATOM_Vector3f(event->newValue.get3F ()[0], event->newValue.get3F ()[1], event->newValue.get3F ()[2]));
			break;
		}
	case CM_DIRECTION:
		{
			setDirection (ATOM_Vector3f(event->newValue.get3F ()[0], event->newValue.get3F ()[1], event->newValue.get3F ()[2]));
			break;
		}
	}
}

void AS_CameraModal::updateOptions (ATOMX_TweakBar *bar)
{
	updatePerspectiveOptions (bar);
	updateTransformOptions (bar);
}

void AS_CameraModal::updatePerspectiveOptions (ATOMX_TweakBar *bar)
{
	bar->setF ("FOV", getFovY() * 180.f / 3.1415926f);
	bar->setF ("Near", getNearPlane());
	bar->setF ("Far", getFarPlane());
}

void AS_CameraModal::updateTransformOptions (ATOMX_TweakBar *bar)
{
	bar->set3F ("Position", getPosition ());
	bar->set3F ("Direction", getDirection ());
}

