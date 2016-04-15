#ifndef __ATOM3D_ENGINE_RENDERTASK_H
#define __ATOM3D_ENGINE_RENDERTASK_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_kernel.h"
#include "basedefs.h"

class ATOM_Scene;
class ATOM_Camera;

class ATOM_ENGINE_API ATOM_RenderTask: public ATOM_ReferenceObj
{
public:
	virtual ~ATOM_RenderTask (void) {}
	virtual void render (ATOM_Scene *scene, bool clear) = 0;
};

#endif // __ATOM3D_ENGINE_RENDERTASK_H
