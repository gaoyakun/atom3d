#ifndef __ATOM3D_ENGINE_TERRAINRENDERDATAS_H
#define __ATOM3D_ENGINE_TERRAINRENDERDATAS_H

#if _MSC_VER > 100
# pragma once
#endif

#include <ATOM_dbghlp.h>

class ATOM_TerrainPatch;

class ATOM_TerrainRenderDatas: public ATOM_Drawable
{
public:
	void addPatch (ATOM_TerrainPatch *patch, bool detail);
	void clearPatches (void);
	void setTerrain (ATOM_Terrain *terrain);
	void setFogEnabled (bool fogEnabled);

public:
    virtual bool draw (ATOM_RenderDevice *device, ATOM_Camera *camera, ATOM_Material *material);

private:
	ATOM_VECTOR<ATOM_TerrainPatch*> _detailPatches;
	ATOM_VECTOR<ATOM_TerrainPatch*> _nondetailPatches;
	bool _fogEnabled;
	ATOM_Terrain *_terrain;
};

class ATOM_TerrainRenderDatasFF: public ATOM_Drawable
{
public:
	bool create (ATOM_RenderDevice *device, ATOM_Terrain *terrain, ATOM_HeightField *hf, const ATOM_Vector3f *normals);

public:
    virtual bool draw (ATOM_RenderDevice *device, ATOM_Camera *camera, ATOM_Material *material);

private:
	ATOM_AUTOREF(ATOM_VertexArray) _vertices;
	ATOM_AUTOREF(ATOM_IndexArray) _indices;
	ATOM_VertexDecl _vertexDecl;
	ATOM_Terrain *_terrain;
	ATOM_Matrix4x4f _scaleMatrix;
};

#endif // __ATOM3D_ENGINE_TERRAINRENDERDATAS_H
