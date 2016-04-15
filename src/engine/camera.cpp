#include "stdafx.h"
#include "cullvisitor.h"

ATOM_Camera::ATOM_Camera() 
{
	ATOM_STACK_TRACE(ATOM_Camera::ATOM_Camera);

 _ProjMatrix.makeIdentity(); 
  _ViewMatrix.makeIdentity();
  _InvViewMatrix.makeIdentity();
  _VPMatrix.makeIdentity();

  _M_flags = 0;
  _viewport.point.x = 0;
  _viewport.point.y = 0;
  _viewport.size.w = 0;
  _viewport.size.h = 0;
  _depthMin = 0.f;
  _depthMax = 1.f;
}

ATOM_Camera::~ATOM_Camera () 
{
	ATOM_STACK_TRACE(ATOM_Camera::~ATOM_Camera);
}

void ATOM_Camera::setViewport(int x, int y, int w, int h, float depthMin, float depthMax) 
{
	ATOM_STACK_TRACE(ATOM_Camera::setViewport);

	_viewport.point.x = x;
	_viewport.point.y = y;
	_viewport.size.w = w;
	_viewport.size.h = h;
	_depthMin = depthMin;
	_depthMax = depthMax;
}

const ATOM_Rect2Di &ATOM_Camera::getViewport(void) const
{
	return _viewport;
}

void ATOM_Camera::setViewMatrix(const ATOM_Matrix4x4f &mat) 
{
	ATOM_STACK_TRACE(ATOM_Camera::setViewMatrix);

	_ViewMatrix = mat;
	_InvViewMatrix.invertAffineFrom (_ViewMatrix);
	_VPMatrix = _ProjMatrix >> _InvViewMatrix;
}

void ATOM_Camera::translateView(const ATOM_Vector3f& t) 
{
	ATOM_STACK_TRACE(ATOM_Camera::translateView);

	_ViewMatrix >>= ATOM_Matrix4x4f::getTranslateMatrix (t);
	_InvViewMatrix.invertAffineFrom (_ViewMatrix);
	_VPMatrix = _ProjMatrix >> _InvViewMatrix;
}

void ATOM_Camera::translateWorld(const ATOM_Vector3f& t)
{
	ATOM_STACK_TRACE(ATOM_Camera::translateWorld);

	ATOM_Matrix4x4f m = _InvViewMatrix;
	m.m30 = 0;
	m.m31 = 0;
	m.m32 = 0;
	translateView (m >> t);
}

void ATOM_Camera::rotateView(const ATOM_Vector3f& axis, float angle) 
{
	ATOM_STACK_TRACE(ATOM_Camera::rotateView);

	float ca, sa;
	ATOM_sincos (angle, &sa, &ca);
	ATOM_Vector3f u;
	u.normalizeFrom (axis);
	float omcaux, omcauy, omcauz, uxsa, uysa, uzsa;
	omcaux		= (1-ca)*u.x;
	omcauy		= (1-ca)*u.y;
	omcauz		= (1-ca)*u.z;
	uxsa		= u.x*sa;
	uysa		= u.y*sa;
	uzsa		= u.z*sa;
	ATOM_Matrix4x4f m(	u.x*omcaux + ca,	u.y*omcaux - uzsa,	u.z*omcaux + uysa,	  0,
						u.x*omcauy + uzsa,	u.y*omcauy + ca,	u.z*omcauy - uxsa,	  0,
						u.x*omcauz - uysa,	u.y*omcauz + uxsa,	u.z*omcauz + ca,	  0,
						0,					0,					0,			          1);
	_InvViewMatrix <<= m;
	_ViewMatrix.invertAffineFrom (_InvViewMatrix);
	_VPMatrix = _ProjMatrix >> _InvViewMatrix;
}

void ATOM_Camera::rotateViewWorld(const ATOM_Vector3f& axis, float angle) 
{
	ATOM_STACK_TRACE(ATOM_Camera::rotateViewWorld);

	float ca, sa;
	ATOM_sincos (angle, &sa, &ca);
	ATOM_Vector3f u;
	u.normalizeFrom (axis);
	float omcaux, omcauy, omcauz, uxsa, uysa, uzsa;
	omcaux	= (1-ca)*u.x;
	omcauy	= (1-ca)*u.y;
	omcauz	= (1-ca)*u.z;
	uxsa		= -u.x*sa;
	uysa		= -u.y*sa;
	uzsa		= -u.z*sa;
	ATOM_Matrix3x3f m = ATOM_Matrix3x3f (
		u.x*omcaux + ca,	u.y*omcaux - uzsa,	u.z*omcaux + uysa,
		u.x*omcauy + uzsa,	u.y*omcauy + ca,	u.z*omcauy - uxsa,
		u.x*omcauz - uysa,	u.y*omcauz + uxsa,	u.z*omcauz + ca
		) >> _ViewMatrix.getUpper3();
	_ViewMatrix.setUpper3 (m);
	_InvViewMatrix.invertAffineFrom (_ViewMatrix);
	_VPMatrix = _ProjMatrix >> _InvViewMatrix;
}

void ATOM_Camera::lookAt(const ATOM_Vector3f& eye, const ATOM_Vector3f& center, const ATOM_Vector3f& up) 
{
	ATOM_STACK_TRACE(ATOM_Camera::lookAt);

	_ViewMatrix.makeLookatLH (eye, center, up);
	_InvViewMatrix.invertAffineFrom (_ViewMatrix);
	_VPMatrix = _ProjMatrix >> _InvViewMatrix;
}

void ATOM_Camera::synchronise (ATOM_RenderDevice *device, ATOM_RenderWindow *view)
{
	ATOM_STACK_TRACE(ATOM_Camera::synchronise);

	device->setTransform(ATOM_MATRIXMODE_VIEW, _ViewMatrix);
	device->setTransform(ATOM_MATRIXMODE_PROJECTION, _ProjMatrix);
	device->setViewport(view, _viewport, _depthMin, _depthMax);
}

const ATOM_Matrix4x4f &ATOM_Camera::getProjectionMatrix () const
{
	return _ProjMatrix;
}

const ATOM_Matrix4x4f & ATOM_Camera::getViewMatrix () const {
	return _ViewMatrix;
}

const ATOM_Matrix4x4f & ATOM_Camera::getInvViewMatrix () const {
	return _InvViewMatrix;
}

void ATOM_Camera::setPerspective (float fovy, float aspect, float znear, float zfar)
{
	_ProjMatrix.makePerspectiveFovLH (fovy, aspect, znear, zfar);
	_VPMatrix = _ProjMatrix >> _InvViewMatrix;
}

void ATOM_Camera::setProjectionMatrix (const ATOM_Matrix4x4f &mat)
{
	_ProjMatrix = mat;
	_VPMatrix = _ProjMatrix >> _InvViewMatrix;
}

void ATOM_Camera::setOrtho (float left, float right, float bottom, float top, float znear, float zfar)
{
	_ProjMatrix.makeOrthoFrustumLH (left, right, bottom, top, znear, zfar);
	_VPMatrix = _ProjMatrix >> _InvViewMatrix;
}

void ATOM_Camera::setFlags (unsigned flags) {
	_M_flags = flags;
}

unsigned ATOM_Camera::getFlags () const {
	return _M_flags;
}

void ATOM_Camera::getCameraPos(ATOM_Vector3f *pEye, ATOM_Vector3f *pAt, ATOM_Vector3f *pUp)
{
	_ViewMatrix.decomposeLookatLH (*pEye, *pAt, *pUp);
}

const ATOM_Matrix4x4f &ATOM_Camera::getViewProjectionMatrix (void) const
{
	return _VPMatrix;
}

float ATOM_Camera::getLeft (void) const
{
	const ATOM_Matrix4x4f &projMatrix = getProjectionMatrix ();

	if (isPerspective ())
	{
		return projMatrix.m32 / (projMatrix.m22 * projMatrix.m00);
	}
	else
	{
		return -1.f / projMatrix.m00;
	}
}

float ATOM_Camera::getTop (void) const
{
	const ATOM_Matrix4x4f &projMatrix = getProjectionMatrix ();

	if (isPerspective ())
	{
		return -projMatrix.m32 / (projMatrix.m11 * projMatrix.m00);
	}
	else
	{
		return 1.f / projMatrix.m11;
	}
}

float ATOM_Camera::getRight (void) const
{
	const ATOM_Matrix4x4f &projMatrix = getProjectionMatrix ();

	if (isPerspective ())
	{
		return -projMatrix.m32 / (projMatrix.m22 * projMatrix.m00);
	}
	else
	{
		return 1.f / projMatrix.m00;
	}
}

float ATOM_Camera::getBottom (void) const
{
	const ATOM_Matrix4x4f &projMatrix = getProjectionMatrix ();

	if (isPerspective ())
	{
		return projMatrix.m32 / (projMatrix.m11 * projMatrix.m00);
	}
	else
	{
		return -1.f / projMatrix.m11;
	}
}

float ATOM_Camera::getNearPlane (void) const
{
	const ATOM_Matrix4x4f &projMatrix = getProjectionMatrix ();
	return -projMatrix.m32 / projMatrix.m22;
}

float ATOM_Camera::getFarPlane (void) const
{
	const ATOM_Matrix4x4f &projMatrix = getProjectionMatrix ();

	if (isPerspective ())
	{
		return projMatrix.m32 / (1.f - projMatrix.m22);
	}
	else
	{
		return (1.f - projMatrix.m32) / projMatrix.m22;
	}
}

bool ATOM_Camera::isPerspective (void) const
{
	return ATOM_equal (getProjectionMatrix().m33, 0.f);
}

bool ATOM_Camera::isOrtho (void) const
{
	return ATOM_equal (getProjectionMatrix().m33, 1.f);
}

float ATOM_Camera::getAspect (void) const
{
	const ATOM_Matrix4x4f &projMatrix = getProjectionMatrix ();
	return projMatrix.m11 / projMatrix.m00;
}

float ATOM_Camera::getInvTanHalfFovy (void) const
{
	const ATOM_Matrix4x4f &projMatrix = getProjectionMatrix ();
	return projMatrix.m11;
}

float ATOM_Camera::getTanHalfFovy (void) const
{
	const ATOM_Matrix4x4f &projMatrix = getProjectionMatrix ();
	return 1.f / projMatrix.m11;
}

void ATOM_Camera::calculateOrigin (float nearDistance, float farDistance, ATOM_Vector3f *origin, float *radius) const
{
	float nearPlane = nearDistance;
	float farPlane = farDistance;

	const ATOM_Matrix4x4f &viewMatrix = getViewMatrix ();
	ATOM_Vector3f eye, center, up;
	viewMatrix.decomposeLookatLH (eye, center, up);
	ATOM_Vector3f z = center - eye;
	ATOM_Vector3f x = crossProduct (up, z);
	ATOM_Vector3f y = crossProduct (z, x);

	float tanHalfFovY = getTanHalfFovy ();
	float aspect = getAspect();
	float nearPlaneHeight = tanHalfFovY * nearPlane;
	float nearPlaneWidth = nearPlaneHeight * aspect;
	float farPlaneHeight = tanHalfFovY * farPlane;
	float farPlaneWidth = farPlaneHeight * aspect;

	float tanHalfFovX = farPlaneWidth/ATOM_sqrt(ATOM_sqr(farPlaneHeight) + ATOM_sqr(farPlane));
	float cosHalfFovX = 1.f/ATOM_sqrt(1.f + ATOM_sqr(tanHalfFovX));
	float t = (z * farPlane + y * farPlaneHeight - x * farPlaneWidth).getLength() * 0.5f / cosHalfFovX;
	float cosHalfFovY = 1.f/ATOM_sqrt(1.f + ATOM_sqr(tanHalfFovY));
	t = t / cosHalfFovY;

	*origin = eye + z * t;

	float radius0 = (eye - *origin).getLength();
	float radius1 = (eye + z * farPlane - x * farPlaneWidth + y * farPlaneHeight - *origin).getLength();
	float radius2 = (eye + z * farPlane + x * farPlaneWidth + y * farPlaneHeight - *origin).getLength();
	float radius3 = (eye + z * farPlane + x * farPlaneWidth - y * farPlaneHeight - *origin).getLength();
	float radius4 = (eye + z * farPlane - x * farPlaneWidth - y * farPlaneHeight - *origin).getLength();

	*radius = ATOM_max2(ATOM_max2(ATOM_max2(ATOM_max2(radius0, radius1), radius2), radius3), radius4);
}

