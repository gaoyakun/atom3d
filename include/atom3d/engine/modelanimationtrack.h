#ifndef __ATOM3D_ENGINE_MODELANIMATIONTRACK_H
#define __ATOM3D_ENGINE_MODELANIMATIONTRACK_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_kernel.h"
#include "modelanimationkeyframe.h"
#include "skinanim_sse.h"
#include "model.h"
#include "property.h"
#include "instancemesh.h"

class ATOM_InstanceMesh;

class ATOM_ENGINE_API ATOM_ModelAnimationTrack: public ATOM_ReferenceObj
{
	friend class ModelChunkReader;

public:
	struct MeshKeyFrame
	{
		int vertexIndex;
		int uvIndex;
		int bboxIndex;
		int jointIndex;
		int transformIndex;
		float transparency;
		ATOM_Vector3f billboardNormal;
		ATOM_Vector3f billboardXAxis;
		ATOM_Vector3f billboardCenter;
	};

	struct KeyFrame
	{
		ATOM_VECTOR<MeshKeyFrame> meshKeyFrames;
		int jointIndex;
		unsigned frameTime;
	};

	enum
	{
		AP_VERTEX = (1<<0),
		AP_UV = (1<<1),
		AP_NORMAL = (1<<2)
	};

public:
	ATOM_ModelAnimationTrack (void);
	virtual ~ATOM_ModelAnimationTrack (void);

public:
	//virtual AnimationType getType (void) const = 0;
	virtual unsigned getTotalAnimationTime (void) const;

public:
	void invalidBoundingbox (void);
	void computeBBoxInterpolateArray (ATOM_Skeleton *skeleton, const ATOM_BitArray *jointMask);
	bool getBoundingbox (unsigned cycleTick, ATOM_BBox &bbox) const;
	void setPrecomputedBoundingbox (const ATOM_BBox &bbox);
	const ATOM_BBox &getPrecomputedBoundingbox (void);
	bool hasPrecomputedBoundingbox (void) const;
	float getAnimationFrame (unsigned cycleTick) const;
	int getJointIndex (unsigned frame) const;
	bool getJointTransform (float frame, int joint, ATOM_JointTransformInfo *p) const;
	void setName (const char *name);
	const char *getName (void) const;
	void setNumFrames (unsigned num);
	unsigned getNumFrames (void) const;
	KeyFrame &getFrame (unsigned frame);
	const KeyFrame &getFrame (unsigned frame) const;
	void setAnimationDatas (ATOM_ModelAnimationDataCache *data);
	ATOM_ModelAnimationDataCache *getAnimationDatas (void) const;
	void setHashCode (unsigned code);
	unsigned getHashCode (void) const;
	bool getBoneMatrix (float frame, unsigned index, ATOM_Skeleton *skeleton, ATOM_Matrix4x4f &matrix) const;
	void computeBillboardNormal (unsigned mesh, const unsigned short *indices, const ATOM_Matrix4x4f &worldMatrix);
	unsigned getAnimationParts (ATOM_InstanceMesh *mesh, float frame, bool animationChanged) const;
	bool needProcessVertices (ATOM_InstanceMesh *mesh, float frame, bool animationChanged) const;
	bool needProcessUVs (ATOM_InstanceMesh *mesh, float frame, bool animationChanged) const;
	bool needProcessNormals (ATOM_InstanceMesh *mesh, float frame, bool animationChanged) const;
	bool updateVertexAnimation (ATOM_InstanceMesh *mesh, unsigned cycleTick);


public:
	// for debug purpose
	void dumpToFile (const char *filename) const;

public:
	// return true if the boundingbox of the mesh was changed, other wise the boundingbox of the mesh was not changed.
	bool updateMesh (unsigned cycleTick, ATOM_InstanceMesh *mesh, ATOM_InstanceSkeleton *skeleton) const;

	bool updateMeshMT (ATOM_MeshAnimationContext *context) const;

public:
	// for internal usage
	void setKeyFrameCache (ATOM_ModelAnimationDataCache *cache) { _keyframeDatas = cache; }
	void setHashes (const ATOM_HASHMAP<unsigned, unsigned> &hashes) { _jointHashes = hashes; }
	const ATOM_HASHMAP<unsigned, unsigned> &getHashes (void) const { return _jointHashes; }
	ATOM_ModelAnimationDataCache *getKeyFrameCache (void) const { return _keyframeDatas.get(); }
	const char *getProperty (const char *key) const;
	const ATOM_JointTransformInfo *getFrameJoints (unsigned frame, unsigned *numJoints) const;

private:
	void updateVertexAnimation (
		const ATOM_Vector3f *vertices1,
		const ATOM_Vector2f *uvs1,
		const ATOM_Vector3f *vertices2,
		const ATOM_Vector2f *uvs2,
		float factor,
		ATOM_Vector3f *vertices,
		ATOM_Vector2f *uvs,
		unsigned numVerts,
		bool uvAnimationNoIpol) const;

	void setPropertyIndex (int index);
	void setPropertyCache (ATOM_PropertySetCache *propCache);

private:
	unsigned _hashcode;
	ATOM_STRING _actionName;
	ATOM_VECTOR<KeyFrame> _keyframes;
	ATOM_VECTOR<ATOM_BBox> _bboxInterpolateArray;
	ATOM_AUTOPTR(ATOM_ModelAnimationDataCache) _keyframeDatas;
	float _lastFrame;
	ATOM_HASHMAP<unsigned, unsigned> _jointHashes;
	ATOM_VECTOR<ATOM_VECTOR<unsigned> > _meshJointRemapping;
	int _propIndex;
	bool _bboxComputed;
	bool _bboxPrecomputed;
	ATOM_BBox _precomputedBBox;
	ATOM_AUTOPTR(ATOM_PropertySetCache) _propertySetCache;
	mutable int _jointMatricesIndex;
	mutable ATOM_VECTOR<ATOM_Matrix3x4f> _jointMatrices;
	mutable ATOM_VECTOR<ATOM_JointTransformInfo> _blendedJointTransforms;
	mutable ATOM_Mutex _updateLock;
};

struct ATOM_MeshAnimationContext
{
	ATOM_Vector4f diffuseColorIn;
	ATOM_Vector4f diffuseColorOut;
	int meshIndex;
	volatile long finished;
	bool processed;
	bool animationChanged;
	float lastAnimationFrame;
	unsigned vertexAttrib;
	unsigned cycleTick;
	ATOM_AUTOPTR(ATOM_InstanceMesh) imesh;
	ATOM_AUTOPTR(ATOM_InstanceSkeleton) skeleton;
	ATOM_AUTOPTR(ATOM_ModelAnimationTrack) track;
	ATOM_Vector3f billboardNormal;
	ATOM_Vector3f billboardXAxis;
	ATOM_Vector3f billboardCenter;
	ATOM_Matrix4x4f worldMatrix;
	void *vertices;
	void *uvs;
	void *normals;

	ATOM_MeshAnimationContext (void)
	{
		vertices = 0;
		uvs = 0;
		normals = 0;
		processed = true;
	}

	~ATOM_MeshAnimationContext (void)
	{
	}
};

#endif // __ATOM3D_ENGINE_MODELANIMATIONTRACK_H
