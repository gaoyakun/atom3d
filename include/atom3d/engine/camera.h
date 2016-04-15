#ifndef ATOM_ENGINE_CAMERA_H_
#define ATOM_ENGINE_CAMERA_H_

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_render.h"
#include "../ATOM_geometry.h"
#include "basedefs.h"

class ATOM_Node;
class ATOM_CullVisitor;

class ATOM_ENGINE_API ATOM_Camera
{
public:
	ATOM_Camera (void);
	virtual ~ATOM_Camera (void);

private:
	ATOM_Rect2Di _viewport;
	float _depthMin;
	float _depthMax;
	unsigned _M_flags;

	ATOM_Matrix4x4f _ProjMatrix;
	ATOM_Matrix4x4f _ViewMatrix;
	ATOM_Matrix4x4f _InvViewMatrix;
	ATOM_Matrix4x4f _VPMatrix;

public:
	ATOM_Vector3f getPosition() const { return getViewMatrix().getRow3 (3); }
	void setViewport(int x, int y, int w, int h, float depthMin = 0.f, float depthMax = 1.f);
	const ATOM_Rect2Di &getViewport(void) const;
	void setPerspective (float fovy, float aspect, float znear, float zfar);
	void setFrustum (float left, float right, float bottom, float top, float znear, float zfar);
	void setOrtho (float left, float right, float bottom, float top, float znear, float zfar);
	void setProjectionMatrix (const ATOM_Matrix4x4f &mat);
	void setViewMatrix(const ATOM_Matrix4x4f &mat);
	const ATOM_Matrix4x4f &getProjectionMatrix () const;
	const ATOM_Matrix4x4f &getViewMatrix () const;
	const ATOM_Matrix4x4f &getInvViewMatrix () const;
	const ATOM_Matrix4x4f &getViewProjectionMatrix () const;
	void translateView(const ATOM_Vector3f& t);
	void translateWorld(const ATOM_Vector3f& t);
	void rotateView(const ATOM_Vector3f& axis, float angle);
	void rotateViewWorld(const ATOM_Vector3f& axis, float angle);
	void lookAt(const ATOM_Vector3f& eye, const ATOM_Vector3f& center, const ATOM_Vector3f& up);

public:
	float getLeft (void) const;
	float getTop (void) const;
	float getRight (void) const;
	float getBottom (void) const;
	float getNearPlane (void) const;
	float getFarPlane (void) const;
	bool isPerspective (void) const;
	bool isOrtho (void) const;
	float getAspect (void) const;
	float getInvTanHalfFovy (void) const;
	float getTanHalfFovy (void) const;
	void calculateOrigin (float nearDistance, float farDistance, ATOM_Vector3f *origin, float *radius) const;

public:
	void synchronise(ATOM_RenderDevice *device, ATOM_RenderWindow *view);
	void setFlags (unsigned flags);
	unsigned getFlags () const;

public:
	//FIME:pUp can't support temporary;
	void getCameraPos(ATOM_Vector3f *pEye, ATOM_Vector3f *pAt, ATOM_Vector3f *pUp);
};

#endif//ATOM_ENGINE_CAMERA_H_
