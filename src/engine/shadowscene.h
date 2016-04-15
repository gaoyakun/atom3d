#ifndef __ATOM3D_SHADOWSCENE_H
#define __ATOM3D_SHADOWSCENE_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "scene.h"

class ATOM_ENGINE_API ATOM_ShadowScene: public ATOM_Scene
{
public:
	ATOM_ShadowScene (const ATOM_SDLScene &from);
	virtual ~ATOM_ShadowScene (void);

public:
	ATOM_Camera *getOriginCamera (void) const;

public:
	virtual void render (ATOM_RenderDevice *device, bool clear);

private:
	ATOM_Camera *_originCamera;
};

#endif // __ATOM3D_SHADOWSCENE_H
