#ifndef __ATOM3D_ENGINE_WATERRENDERTASK_H
#define __ATOM3D_ENGINE_WATERRENDERTASK_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_math.h"
#include "../ATOM_render.h"
#include "rendertask.h"
#include "water.h"

class ATOM_WaterRenderTask: public ATOM_RenderTask
{
public:
	ATOM_WaterRenderTask (ATOM_Water *waterNode);
	virtual ~ATOM_WaterRenderTask (void);

public:
	virtual void render (ATOM_Scene *scene, bool clear);

private:
	ATOM_AUTOREF(ATOM_Water) _waterNode;
	ATOM_AUTOREF(ATOM_Texture) _renderTarget;
	ATOM_AUTOREF(ATOM_Texture) _renderDepth;
};

#endif // __ATOM3D_ENGINE_REFLECTIONRENDERTASK_H
