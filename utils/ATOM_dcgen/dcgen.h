#ifndef __ATOM_TOOLS_DCGEN_H
#define __ATOM_TOOLS_DCGEN_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <ATOM.h>

#if 0

class DiffuseCubemapGen
{
public:
	DiffuseCubemapGen (void);
	~DiffuseCubemapGen (void);

public:
	void setScene (ATOM_SDLScene *scene);
	void setMesh (ATOM_Geode *model);
	void setTerrain (ATOM_Terrain *terrain);
	ATOM_AUTOREF(ATOM_Texture) generate (unsigned size);

private:
	void setupCamera (ATOM_Camera *camera, const ATOM_Vector3f &origin, int dir);
	void drawScene (ATOM_RenderDevice *device, ATOM_Camera *camera, ATOM_CullVisitor *visitor = 0, ATOM_Material *material = 0);
	void drawSkyNormal (ATOM_RenderDevice *device, float size, ATOM_Camera *camera);
	void drawSkyLookat (ATOM_RenderDevice *device, float size, ATOM_Camera *camera);

private:
	ATOM_SDLScene *_scene;
	ATOM_AUTOREF(ATOM_Geode) _geode;
	ATOM_AUTOREF(ATOM_Terrain) _terrain;
};

#endif

#endif // __ATOM_TOOLS_DCGEN_H
