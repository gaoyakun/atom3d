#if 0
#ifndef __HEIGHTFIELD_AOGEN_H
#define __HEIGHTFIELD_AOGEN_H

#include <ATOM_dbghlp.h>
#include <ATOM_sgengine.h>

class Terrain_AOGenerator
{
public:
	Terrain_AOGenerator (void);
	Terrain_AOGenerator (const char *sceneFileName, const char *bentnormalMap, void (__cdecl *callback)(void*,int) = 0, void *userdata = 0);

public:
	void setSceneFileName (const char *sceneFileName);
	void setBentNormalMapFileName (const char *bentnormalMapFileName);
	bool generate (unsigned size);
	void setProcessCallback (void (__cdecl *callback)(void *userdata, int percent), void *userdata);
	const char *getErrorMessage (void) const;

private:
	bool createRenderTarget (void);
	bool createAOMaterial (void);
	void createBackground (void);
	void setupCamera (const ATOM_Vector3f &origin, int dir, ATOM_Camera *camera);
	void drawBackground (ATOM_Camera *camera);
	void drawScene (ATOM_Camera *camera);
	void drawTerrain (ATOM_Camera *camera);
	void prepareTerrainVertices (ATOM_HeightField *hf);
	void setErrorMessage (const char *format,...);

private:
	ATOM_AUTOREF(ATOM_Texture) _rendertarget[6];
	ATOM_AUTOREF(ATOM_Material) _occmaterial;
	ATOM_AUTOREF(ATOM_Material) _unoccmaterial;
	ATOM_AUTOREF(ATOM_VertexArray) _background;
	ATOM_AUTOREF(ATOM_VertexArray) _backgroundc;
	ATOM_AUTOREF(ATOM_VertexArray) _terrainVerts;
	ATOM_AUTOREF(ATOM_IndexArray) _terrainIndices;
	ATOM_STRING _bentnormalMap;
	ATOM_STRING _sceneFileName;
	ATOM_STRING _errorMessage;
	void (__cdecl *_callback)(void*, int);
	void *_callbackData;
	ATOM_SDLScene _scene;
};

#endif // __HEIGHTFIELD_AOGEN_H
#endif
