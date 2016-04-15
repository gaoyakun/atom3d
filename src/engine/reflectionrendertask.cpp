#include "StdAfx.h"
#include "reflectionrendertask.h"



ATOM_ReflectionRenderTask::ATOM_ReflectionRenderTask (void)
{
	ATOM_STACK_TRACE(ATOM_ReflectionRenderTask::ATOM_ReflectionRenderTask);
}

ATOM_ReflectionRenderTask::~ATOM_ReflectionRenderTask (void)
{
	ATOM_STACK_TRACE(ATOM_ReflectionRenderTask::~ATOM_ReflectionRenderTask);
}

static inline float DistanceToPlane (const ATOM_Vector3f &p, const ATOM_Vector4f &plane) 
{
	return p.x * plane.x + p.y * plane.y + p.z * plane.z + plane.w;
}

static int ClipQuadToPlane (const ATOM_Vector3f &p0, const ATOM_Vector3f &p1, const ATOM_Vector3f &p2,
                            const ATOM_Vector3f &p3, const ATOM_Vector4f &plane, ATOM_Vector3f *result) 
{
	float d = DistanceToPlane(p0, plane);
	ATOM_Vector3f *p = result;
	float dlast = d;
	float dfirst = d;
	bool isFront = d >= 0.001f;
	bool lastIsFront = isFront;
	bool firstIsFront = isFront;

	if (isFront)
	{
		*result++ = p0;
	}

	d = DistanceToPlane(p1, plane);
	isFront = d >= 0.001f;
	if (isFront && lastIsFront)
	{
		*result++ = p1;
	}
	else if (isFront || lastIsFront)
	{
		*result++ = (p0 + (p1 - p0) * (dlast / (dlast - d)));
		if (isFront)
		{
			*result++ = p1;
		}
	}
	dlast = d;
	lastIsFront = isFront;

	d = DistanceToPlane(p2, plane);
	isFront = d >= 0.001f;
	if (isFront && lastIsFront)
	{
		*result++ = p2;
	}
	else if (isFront || lastIsFront)
	{
		*result++ = (p1 + (p2 - p1) * (dlast / (dlast - d)));
		if (isFront)
		{
			*result++ = p2;
		}
	}
	dlast = d;
	lastIsFront = isFront;

	d = DistanceToPlane(p3, plane);
	isFront = d >= 0.001f;
	if (isFront && lastIsFront)
	{
		*result++ = p3;
	}
	else if (isFront || lastIsFront)
	{
		*result++ = (p2 + (p3 - p2) * (dlast / (dlast - d)));
		if (isFront)
		{
			*result++ = p3;
		}
	}
	dlast = d;
	lastIsFront = isFront;

	if (firstIsFront != lastIsFront)
	{
		*result++ = (p3 + (p0 - p3) * (dlast / (dlast - dfirst)));
	}

	return result - p;
}

static inline float sgn(float a)
{
	if (a > 0.f) return 1.f;
	if (a < 0.f) return -1.f;
	return (0.f);
}

static void modifyProjectionMatrix(const ATOM_Vector4f &clipPlane, float *matrix)
{
	ATOM_Vector4f q;

	q.x = sgn(clipPlane.x) / matrix[0];
	q.y = sgn(clipPlane.y) / matrix[5];
	q.z = 1.f;
	q.w = (1.f - matrix[10]) / matrix[14];

	// Calculate the scaled plane vector
	ATOM_Vector4f c = clipPlane  / dotProduct(clipPlane, q);

	// Replace the third row of the projection matrix
	matrix[2] = c.x;
	matrix[6] = c.y;
	matrix[10] = c.z;
	matrix[14] = c.w;
}

void ATOM_ReflectionRenderTask::render (ATOM_Scene *scene, bool clear)
{/*
	ATOM_STACK_TRACE(ATOM_ReflectionRenderTask::render);

	ATOM_RenderDevice *device = ATOM_GetRenderDevice();

	ATOM_Camera *camera = scene->getCamera ();
	const ATOM_Matrix4x4f oldViewMatrix = camera->getViewMatrix ();
	const ATOM_Matrix4x4f oldProjectionMatrix = camera->getProjectionMatrix ();

	ATOM_Matrix4x4f reflectionMatrix = ATOM_Matrix4x4f::getReflectionMatrix (_reflectionPlane);
	ATOM_Matrix4x4f obliqueMat = oldProjectionMatrix;
	ATOM_Matrix4x4f viewMat = camera->getInvViewMatrix ();
	viewMat >>= reflectionMatrix;
	viewMat.invert ();
	viewMat.transpose ();
	ATOM_Vector4f cp = _reflectionPlane;
	cp <<= viewMat;
    modifyProjectionMatrix (cp, obliqueMat.m);
	viewMat.transpose ();

	ATOM_Matrix4x4f viewCull = camera->getInvViewMatrix ();
	viewCull >>= reflectionMatrix;
	viewCull.invert ();

	camera->setViewMatrix (viewCull);

	int oldViewport[4];
	memcpy (oldViewport, camera->getViewport(), 4 * sizeof(int));
	camera->setViewport (0, 0, _reflectionTexture->getWidth(), _reflectionTexture->getHeight());

	ATOM_AUTOREF(ATOM_Texture) oldRT = device->getRenderTarget (0);
	ATOM_AUTOREF(ATOM_DepthBuffer) oldDepth = device->getDepthBuffer();

	bool shadowEnabled = ATOM_RenderSettings::isShadowEnabled ();
	ATOM_RenderSettings::enableShadow (false);

	unsigned cullFlags = scene->getCullVisitor()->getFlags();
	scene->getCullVisitor()->setFlags (cullFlags | ATOM_CullVisitor::CULL_REFLECTION_PASS);
	bool updateVisibleStamp = scene->getCullVisitor()->getUpdateVisibleStamp ();
	scene->getCullVisitor()->setUpdateVisibleStamp (false);

	scene->beginRender (true);
	scene->cull ();

	device->setRenderTarget (0, _reflectionTexture.get());
	device->setDepthBuffer (_reflectionDepth.get());
	device->setViewport (0, 0, _reflectionTexture->getWidth(), _reflectionTexture->getHeight(), 0.f, 1.f);
	device->clear (true, true, true);
	device->revertWindingOrder (true);


	scene->draw (forceMaterial, ATOM_RenderQueue::SKY_LAYERS);

	camera->setProjectionMatrix (obliqueMat);
	scene->draw (forceMaterial, (ATOM_RenderQueue::ALL_LAYER & ~(ATOM_RenderQueue::SKY_LAYERS | ATOM_RenderQueue::HUD_LAYERS)));

	scene->endRender ();

	device->revertWindingOrder (false);
	device->setRenderTarget (0, oldRT.get());
	device->setDepthBuffer (oldDepth.get());
	device->setViewport (0, oldViewport[0], oldViewport[1], oldViewport[2], oldViewport[3]);

	scene->getCullVisitor()->setFlags (cullFlags);
	scene->getCullVisitor()->setUpdateVisibleStamp (updateVisibleStamp);

	ATOM_RenderSettings::enableShadow (shadowEnabled);

	camera->setViewMatrix (oldViewMatrix);
	camera->setProjectionMatrix (oldProjectionMatrix);
	camera->setViewport (oldViewport[0], oldViewport[1], oldViewport[2], oldViewport[3]);

	int b = 0;
	if (b)
	{
		_reflectionTexture->saveToFile ("/reflection.png");
	}*/
}

void ATOM_ReflectionRenderTask::setReflectionPlane (const ATOM_Vector4f &planeInWorldSpace)
{
	_reflectionPlane = planeInWorldSpace;
}

const ATOM_Vector4f & ATOM_ReflectionRenderTask::getReflectionPlane (void) const
{
	return _reflectionPlane;
}

void ATOM_ReflectionRenderTask::setReflectionTexture (ATOM_Texture *texture)
{
	_reflectionTexture = texture;
}

void ATOM_ReflectionRenderTask::setReflectionDepth (ATOM_DepthBuffer *depth)
{
	_reflectionDepth = depth;
}

void ATOM_ReflectionRenderTask::setShape (float minx, float maxx, float y, float minz, float maxz)
{
	_minx = minx;
	_maxx = maxx;
	_y = y;
	_minz = minz;
	_maxz = maxz;
}


