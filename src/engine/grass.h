#ifndef __ATOM3D_ENGINE_GRASS_H
#define __ATOM3D_ENGINE_GRASS_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <ATOM_dbghlp.h>
#include <ATOM_math.h>
#include <ATOM_render.h>
#include <ATOM_geometry.h>

class ATOM_HeightField;

struct GrassCell
{
	struct GrassObject
	{
		ATOM_Vector3f position;
		ATOM_Vector3f d[3];
		ATOM_Vector3f n;
		ATOM_Vector3f norm;
		ATOM_Vector2f t[2];
		float distance;
	};

	ATOM_VECTOR<GrassObject> grassObjects;
	float x;
	float z;
	float distance;
	float w;
	float h;
	ATOM_BBox bbox;
};

class ATOM_MultiStreamGeometry;

class GrassCluster: public ATOM_Drawable
{
	struct GrassVertex
	{
		ATOM_Vector3f pos;
		ATOM_Vector3f normal;
		ATOM_Vector2f texcoord;
	};
public:
	GrassCluster (ATOM_Terrain *terrain);
	~GrassCluster (void);

public:
	void initialize (float cellW, float cellH, unsigned cellDimX, unsigned cellDimZ, ATOM_Texture *texture);
	void addLayer (const ATOM_Vector2f &tcmin, const ATOM_Vector2f &tcmax, float width, float height, unsigned num, const ATOM_Vector3f *positions, const ATOM_Vector3f *normals, float vMin, float vMax);
	bool initVertexArrays (void);
	void setVPMatrix (const ATOM_Matrix4x4f &m, bool shadowPass=false);
	void updateScale (float vMin, float vMax);
	void updateSize (float width, float height);
	ATOM_Material *getMaterial (void) const;
	void addGrassObject (const ATOM_Vector3f &pos, float width, float height, float vMin, float vMax, const ATOM_Vector2f &tcMin, const ATOM_Vector2f &tcMax, const ATOM_Vector3f &normal);
	void removeRandomGrassObject (const ATOM_Vector3f &pos, float radiusX, float radiusZ, int count);
	void getGrassPositions (ATOM_VECTOR<ATOM_Vector3f> &v) const;

public:
	virtual bool draw (ATOM_RenderDevice *device, ATOM_Camera *camera, ATOM_Material *material);

private:
	void determineVisibleRegion (const ATOM_Vector3f &pos, float maxDistance);
	void drawCell (ATOM_RenderDevice *device, ATOM_Material *material, const GrassCell *cell, GrassVertex *&p, unsigned &totalSize);
	void flush (ATOM_RenderDevice *device, ATOM_Material *material, unsigned numVerts);

private:
	GrassCell *_cells;
	float _cellWidth;
	float _cellHeight;

	unsigned _cellDimX;
	unsigned _cellDimZ;
	unsigned _visibleCellStartX;
	unsigned _visibleCellStartZ;
	unsigned _visibleCellCountX;
	unsigned _visibleCellCountZ;

	ATOM_Matrix4x4f _vpMatrix;
	//-- wangjian added ---//
	ATOM_Matrix4x4f _vpMatrix_shadow;
	//---------------------//

	ATOM_AUTOREF(ATOM_Texture) _texture;
	ATOM_AUTOREF(ATOM_VertexArray) _vertex_array;
	ATOM_AUTOREF(ATOM_IndexArray) _index_array;
	ATOM_AUTOPTR(ATOM_Material) _material;
	ATOM_Terrain *_terrain;

	ATOM_InterleavedStreamGeometry *_geometry;
};


#endif // __ATOM3D_ENGINE_GRASS_H
