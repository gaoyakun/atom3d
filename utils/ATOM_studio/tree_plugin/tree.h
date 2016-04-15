#ifndef __ATOM3D_STUDIO_TREE_H
#define __ATOM3D_STUDIO_TREE_H

#if _MSC_VER > 1000
# pragma once
#endif

#if defined(SUPPORT_BILLBOARD_TREE)

#include "trunk.h"
#include "leaf.h"

#pragma pack(push, 4)

class TreeNode: public ATOM_VisualNode
{
	ATOM_CLASS(studio, TreeNode, TreeNode)

public:
	enum RenderMode
	{
		DRAW_AUTO,
		DRAW_NORMAL,
		DRAW_BILLBOARD,
		CREATE_BILLBOARD
	};

private:
	struct BillboardInfo
	{
		float width;
		float height;
		float originX;
		float originY;
	};

public:
	TreeNode (void);
	virtual ~TreeNode (void);

public:
	virtual void buildBoundingbox (void) const;
	virtual void setupRenderQueue (ATOM_CullVisitor *visitor);
	virtual bool rayIntersectionTest (ATOM_Camera *camera, const ATOM_Ray &ray, float *len) const;

protected:
	virtual bool onLoad(ATOM_RenderDevice *device);

public:
	void setRenderMode (RenderMode renderMode);
	TreeNode::RenderMode getRenderMode (void);
	void setFileName (const ATOM_STRING &fileName);
	const ATOM_STRING &getFileName (void) const;
	void setLODDistance (float d);
	float getLODDistance (void) const;
	bool generateBillboard (ATOM_RenderDevice *device, unsigned textureWidth, unsigned textureHeight);
	bool exportTreeModel (const char *filename);
	bool save (const char *filename);
	void invalidateBillboard (void);
	TrunkModel &getTrunk (void);
	LeafModel &getLeaves (void);
	void regenerateLeaves (bool reseed);
	void regenerateTrunk (bool reseed);
	unsigned getRandomSeedLeaves (void) const;
	unsigned getRandomSeedTrunk (void) const;
	void checkBillboardValid (void);

private:
	bool createBillboardTextures (ATOM_RenderDevice *device, unsigned w, unsigned h);
	bool updateBillboardGeometry (ATOM_RenderDevice *device);

private:
	ATOM_STRING _fileName;
	bool _billboardDirty;
	RenderMode _renderMode;

	BillboardInfo _billboardInfo[4];
	ATOM_AUTOREF(ATOM_VertexArray) _billboardVertexArray;
	ATOM_AUTOREF(ATOM_IndexArray) _billboardIndexArray;
	ATOM_AUTOREF(ATOM_Texture) _billboardTexture;
	ATOM_AUTOREF(ATOM_Texture) _billboardNormalTexture;
	ATOM_AUTOPTR(ATOM_Material) _billboardMaterial;
	ATOM_VertexDecl _billboardVertexDecl;

	ATOM_AUTOREF(ATOM_VertexArray) _trunkModelVertexArray;
	ATOM_AUTOREF(ATOM_IndexArray) _trunkModelIndexArray;

	TrunkModel _trunkModel;
	LeafModel _leafModel;

	unsigned _randomSeedLeaves;
	unsigned _randomSeedTrunk;
	float _LODdistance;
	float _cameraDistanceSq;
};

#pragma pack(pop)

#endif

#endif // __ATOM3D_STUDIO_TREE_H
