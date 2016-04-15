#ifndef __ATOM_TOOLS_LMGEN_H
#define __ATOM_TOOLS_LMGEN_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <ATOM.h>

class SceneLightmapGen
{
public:
	SceneLightmapGen (void);
	~SceneLightmapGen (void);

public:
	void setScene (ATOM_SDLScene *scene);
	void setTerrainLightmapFileName (const char *filename);
	void setTerrainLightmapSize (unsigned size);
	void setTerrainLightmapBlurFactor (unsigned n);
	void setMeshLightmapPath (const char *path);
	void setMeshLightmapSize (unsigned size);
	bool generate (void);

private:
	ATOM_SDLScene *_scene;
	ATOM_STRING _terrainLightmapFileName;
	ATOM_STRING _objectLightmapPath;
	unsigned _terrainLightmapSize;
	unsigned _terrainLightmapBlurFactor;
	unsigned _objectLightmapSize;
};

class TerrainLightmapGen
{
public:
	TerrainLightmapGen (void);
	~TerrainLightmapGen (void);

public:
	void setTerrain (ATOM_Terrain *terrain);
	void setScene (ATOM_SDLScene *scene);
	ATOM_AUTOREF(ATOM_Texture) generate (unsigned size, unsigned blurFactor);

private:
	ATOM_AUTOREF(ATOM_Texture) generateTerrainNormalmap (unsigned size);

private:
	ATOM_AUTOREF(ATOM_Terrain) _terrain;
	ATOM_SDLScene *_scene;
};

#endif // __ATOM_TOOLS_DCGEN_H
