#ifndef __ATOM3D_ENGINE_MODEL_H
#define __ATOM3D_ENGINE_MODEL_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_dbghlp.h"
#include "../ATOM_kernel.h"
#include "../ATOM_geometry.h"
#include "../ATOM_render.h"
#include "basedefs.h"
#include "skeleton.h"
#include "property.h"
#include "loadinterface.h"

class ATOM_ModelAnimationTrack;

struct ATOM_MeshLockStruct
{
	void *vertices;
	void *colors;
	void *normals;
	void *tangents;
	void *binormals;
	void *texcoords;
	void *blendweights;
	void *blendindices;
	void *indices;
	void *interleaved_vertices;
};

struct ATOM_ModelLockStruct
{
	ATOM_VECTOR<ATOM_MeshLockStruct> meshLockStructs;
	int group;
	bool wait;
	ATOM_VECTOR<ATOM_LoadInterface*> depends;
};


class ATOM_ENGINE_API ATOM_Model;
class ATOM_ENGINE_API ATOM_Geometry;
class ATOM_ENGINE_API ATOM_Mesh: public ATOM_ReferenceObj
{
public:
	enum TransparencyMode
	{
		TM_NORMAL,
		TM_ADDITIVE,
		TM_PARTICLE,
		TM_MODULATE,	// modulate (0, srcColor)
		TM_MODULATE_B,	// modulate brighter (srcAlpha, srcColor)
		TM_IMODULATE,	// inverse modulate (0, 1-srcColor)
		TM_IMODULATE_B,	// inverse modulate brigher (srcAlpha, 1-srcColor)
		TM_INORMAL		// inverse normal blending (1-srcAlpha, srcAlpha)
	};

	struct VertexWeight
	{
		float weight;
		int jointIndex;
		unsigned offsetToNextWeight;
	};

	struct SkeletonVertex
	{
		ATOM_Vector4f vertex;
		ATOM_Vector4f normal;
		ATOM_Vector4f tangent;
	};

public:
	ATOM_Mesh (ATOM_Model *model);
	virtual ~ATOM_Mesh (void);

	ATOM_Model *owner;
	ATOM_Vector4f diffuseColor;
	ATOM_Vector3f billboardNormal;
	ATOM_Vector3f billboardXAxis;
	ATOM_Vector3f billboardCenter;
	ATOM_Matrix4x4f worldMatrix;

	bool alphaTest;
	bool doubleSide;
	bool wireFrameMode;
	bool alphablend;
	bool billboard;
	bool sky;
	bool water;
	bool uvAnimationNoIpol;
	bool fixBlending;
	float alpharef;
	float shininess;
	float transparency;
	bool castshadow;
	bool recvshadow;
	bool dummy;
	TransparencyMode transparencyMode;

	unsigned hashcode;
	ATOM_BBox boundingBox;

	int propertyIndex;

	ATOM_BitArray jointMask;
	ATOM_VECTOR<VertexWeight> weights;
	ATOM_VECTOR<unsigned> jointHashes;
	//ATOM_SwappableRawMemory *vertices_sa;
	ATOM_VECTOR<SkeletonVertex> vertices_sa;

	ATOM_Geometry *geometry;
	ATOM_AUTOREF(ATOM_VertexArray) vertices;
	ATOM_AUTOREF(ATOM_VertexArray) colors;
	ATOM_AUTOREF(ATOM_VertexArray) normals;
	ATOM_AUTOREF(ATOM_VertexArray) tangents;
	ATOM_AUTOREF(ATOM_VertexArray) binormals;
	ATOM_AUTOREF(ATOM_VertexArray) texcoords;
	ATOM_AUTOREF(ATOM_VertexArray) blendweights;
	ATOM_AUTOREF(ATOM_VertexArray) blendindices;
	ATOM_AUTOREF(ATOM_VertexArray) interleavedarray;
	ATOM_AUTOREF(ATOM_IndexArray) indices;
	ATOM_AUTOREF(ATOM_Texture) diffuseTexture;

	ATOM_STRING diffuseTextureName;
	ATOM_AUTOPTR(ATOM_PropertySetCache) propertySetCache;

	ATOM_VECTOR<ATOM_Vector3f> vertexData;
	ATOM_VECTOR<ATOM_Vector3f> normalData;
	ATOM_VECTOR<ATOM_Vector3f> tangentData;
	ATOM_VECTOR<ATOM_Vector3f> binormalData;
	ATOM_VECTOR<ATOM_Vector2f> texcoordData;
	ATOM_VECTOR<unsigned> diffuseData;
	ATOM_VECTOR<unsigned short> indexData;
	ATOM_VECTOR<int> jointMap;

	void initJointMask (void);

	void copyfrom (const ATOM_Mesh *other);
	const char *getProperty (const char *key) const;
    virtual void render(ATOM_RenderDevice* device);
};

class ATOM_MyAABBTree;

class ATOM_ENGINE_API ATOM_Model: public ATOM_Object
{
	friend class ModelChunkReader;
	ATOM_CLASS(engine, ATOM_Model, Model)

public:
	ATOM_Model (void);
	virtual ~ATOM_Model (void);

public:
	bool load (ATOM_RenderDevice *device, const char *filename = 0);
	//--- wangjian added ---//
	// 异步加载：异步加载相关函数
	bool load (const char *filename = 0);
	//----------------------//
	bool memload (ATOM_RenderDevice *device, const void *mem, unsigned size, bool loadtexture, bool batch);
	void clear (void);

	// for multithread loading
	bool lock (void);
	bool realize (void);
	void unlock (void);
	void wait (void);
	void initLockStruct (long group);
	ATOM_ModelLockStruct *getLockStruct (void);
	bool initMaterials (bool batch, bool mt);

public:
	ATOM_AABBTree *getAABBTree (void) const;
	unsigned getNumTracks (void) const;
	unsigned getNumMeshes (void) const;
	ATOM_ModelAnimationTrack *getTrack (unsigned index) const;
	ATOM_Mesh *getMesh (unsigned index) const;
	ATOM_Skeleton *getSkeleton (void) const;
	const char *getProperty (const char *key) const;
	ATOM_LoadInterface *getLoadInterface (void);

public:
	void addTrack (ATOM_ModelAnimationTrack *track);
	void addMesh (ATOM_Mesh *mesh);
	void setSkeleton (ATOM_Skeleton *skeleton);

private:
	void setPropertyCache (ATOM_PropertySetCache *props);
	void setPropertyIndex (int index);

public:
	static void finishRequest (ATOM_LoadingRequest *request);

protected:
	ATOM_VECTOR<ATOM_AUTOPTR(ATOM_Mesh)> _meshes;
	ATOM_VECTOR<ATOM_AUTOPTR(ATOM_ModelAnimationTrack)> _tracks;
	ATOM_AUTOPTR(ATOM_Skeleton) _skeleton;
	ATOM_VECTOR<ATOM_LoadingRequest*> _textureRequests;

	ATOM_MyAABBTree *_aabbtree;

	ATOM_AUTOREF(ATOM_Texture) _overrideTexture;
	ATOM_Vector4f _overrideColor;
	bool _overrideColorEnable;
	bool _overrideTextureEnable;

	int _properties;
	ATOM_AUTOPTR(ATOM_PropertySetCache) _propertySetCache;

	ATOM_ModelLockStruct *_lockStruct;
	ATOM_AUTOPTR(ATOM_ModelLoadInterface) _loadInterface;
};

#endif // __ATOM3D_ENGINE_MODEL_H
