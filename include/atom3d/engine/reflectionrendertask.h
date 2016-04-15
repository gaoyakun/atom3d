#ifndef __ATOM3D_ENGINE_REFLECTIONRENDERTASK_H
#define __ATOM3D_ENGINE_REFLECTIONRENDERTASK_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_math.h"
#include "../ATOM_render.h"
#include "rendertask.h"

class ATOM_ReflectionRenderTask: public ATOM_RenderTask
{
public:
	ATOM_ReflectionRenderTask (void);
	virtual ~ATOM_ReflectionRenderTask (void);

public:
	virtual void render (ATOM_Scene *scene, bool clear);

public:
	void setReflectionPlane (const ATOM_Vector4f &planeInWorldSpace);
	const ATOM_Vector4f & getReflectionPlane (void) const;
	void setReflectionTexture (ATOM_Texture *texture);
	void setReflectionDepth (ATOM_DepthBuffer *depth);
	void setShape (float minx, float maxx, float y, float minz, float maxz);

private:
	ATOM_Vector4f _reflectionPlane;
	ATOM_AUTOREF(ATOM_Texture) _reflectionTexture;
	ATOM_AUTOREF(ATOM_DepthBuffer) _reflectionDepth;
	float _minx;
	float _maxx;
	float _y;
	float _minz;
	float _maxz;
	ATOM_AUTOREF(ATOM_Node) _invisibleNode;
};

#endif // __ATOM3D_ENGINE_REFLECTIONRENDERTASK_H
