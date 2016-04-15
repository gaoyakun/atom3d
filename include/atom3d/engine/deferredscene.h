#ifndef __ATOM3D_ENGINE_DEFERREDSCENE_H
#define __ATOM3D_ENGINE_DEFERREDSCENE_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_utils.h"
#include "../ATOM_math.h"
#include "../ATOM_dbghlp.h"
#include "sdlscene.h"
#include "light.h"
#include "atmosphere.h"

class ATOM_DeferredLighting;
class ATOM_Decal;

// Deferred Shading lScene
class ATOM_ENGINE_API ATOM_DeferredScene: public ATOM_SDLScene
{
public:
	ATOM_DeferredScene (void);
	virtual ~ATOM_DeferredScene (void);

public:
	//void addLight (ATOM_Light *light);
	//void addDecal (ATOM_Decal *decal);
	//void clearLights (void);
	//void clearDecals (void);
	//void setAtmosphere (ATOM_Atmosphere *atmosphere);
	//ATOM_Atmosphere *getAtmosphere (void) const;
	//void enableDeferredRendering (bool enable);
	//bool isDeferredRenderingEnabled (void) const;

protected:
	//ATOM_VECTOR<ATOM_Decal*> _decals;
};

#endif // __ATOM3D_ENGINE_DEFERREDSCENE_H
