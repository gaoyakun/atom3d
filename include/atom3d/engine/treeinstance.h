#ifndef __ATOM3D_ENGINE_TREEINSTANCE_H
#define __ATOM3D_ENGINE_TREEINSTANCE_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_kernel.h"

#pragma pack(push, 4)

class ATOM_TreeBatch;

class ATOM_TreeInstance: public ATOM_Object
{
	ATOM_CLASS(engine, ATOM_TreeInstance, ATOM_TreeInstance)

public:
	struct BillboardInfo
	{
		float width;
		float height;
		float originX;
		float originY;
	};

public:
	ATOM_TreeInstance (void);
	virtual ~ATOM_TreeInstance (void);

public:
	bool load(ATOM_RenderDevice *device, const char *filename);
	bool rayIntersectionTest (const ATOM_Ray &ray, float *len) const;

public:
	const BillboardInfo &getBillboardInfo (int index) const;
	ATOM_Texture *getTrunkTexture (void) const;
	ATOM_Texture *getLeafTexture (void) const;
	ATOM_Texture *getBillboardTexture (void) const;
	ATOM_Texture *getBillboardNormalMap (void) const;
	ATOM_VertexArray *getTrunkVertexArray (void) const;
	ATOM_IndexArray *getTrunkIndexArray (void) const;
	ATOM_VertexArray *getLeafCardVertexArray (void) const;
	ATOM_IndexArray *getLeafCardIndexArray (void) const;
	ATOM_VertexArray *getBillboardVertexArray (void) const;
	ATOM_IndexArray *getBillboardIndexArray (void) const;
	ATOM_TreeBatch *getBatch (void) const;
	float getLODDistance (void) const;
	const ATOM_BBox &getBoundingbox (void) const;
	const ATOM_BBox &getTrunkBoundingbox (void) const;
	const ATOM_BBox &getLeafBoundingbox (void) const;
	bool hasTrunk (void) const;
	bool hasLeaves (void) const;

private:
	BillboardInfo _billboardInfo[4];

	ATOM_AUTOREF(ATOM_VertexArray) _trunkVertexArray;
	ATOM_AUTOREF(ATOM_IndexArray) _trunkIndexArray;
	ATOM_AUTOREF(ATOM_Texture) _trunkTexture;

	ATOM_AUTOREF(ATOM_VertexArray) _leafCardVertexArray;
	ATOM_AUTOREF(ATOM_IndexArray) _leafCardIndexArray;
	ATOM_AUTOREF(ATOM_Texture) _leafCardTexture;

	ATOM_AUTOREF(ATOM_VertexArray) _billboardVertexArray;
	ATOM_AUTOREF(ATOM_IndexArray) _billboardIndexArray;
	ATOM_AUTOREF(ATOM_Texture) _billboardTexture;
	ATOM_AUTOREF(ATOM_Texture) _billboardNormalTexture;

	ATOM_BBox _trunkBBox;
	ATOM_BBox _leafBBox;
	ATOM_BBox _boundingBox;

	float _LODdistance;

	ATOM_TreeBatch *_batch;
};

#pragma pack(pop)

#endif // __ATOM3D_ENGINE_TREEINSTANCE_H
