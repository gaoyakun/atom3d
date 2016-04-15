#ifndef __ATOM3D_ENGINE_MODELANIMATIONKEYFRAME_H
#define __ATOM3D_ENGINE_MODELANIMATIONKEYFRAME_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_dbghlp.h"
#include "../ATOM_kernel.h"
#include "../ATOM_math.h"

#include "basedefs.h"

struct ATOM_JointTransformInfo
{
	ATOM_Quaternion rotate;
	ATOM_Vector4f translate;
	ATOM_Vector4f scale;
};

/**
	header layout
	--------------------------
	data

	numJointFrames
	numUVFrames
	numVertexFrames
	numTransforms

	jointCount
	jointoffset
	jointCount
	jointoffset
	...
	jointCount
	jointoffset

	uvCount
	uvoffset
	uvCount
	uvoffset
	...
	uvCount
	uvoffset

	vertexCount
	vertexoffset
	vertexCount
	vertexoffset
	...
	vertexCount
	vertexoffset

	transformoffset
	----------------------------
 */

class ATOM_ModelAnimationDataCache: public ATOM_ReferenceObj
{
private:
	char *animationData;
	unsigned headerOffset;

public:
	ATOM_ModelAnimationDataCache (void);
	~ATOM_ModelAnimationDataCache (void);
	unsigned getNumJointFrames (void) const;
	unsigned getJointCount (unsigned frame) const;
	const ATOM_JointTransformInfo *getJoints (unsigned frame) const;
	ATOM_JointTransformInfo *getJoints (unsigned frame);
	unsigned getNumUVFrames (void) const;
	unsigned getUVCount (unsigned frame) const;
	const ATOM_Vector2f *getUVs (unsigned frame) const;
	unsigned getNumVertexFrames (void) const;
	unsigned getVertexCount (unsigned frame) const;
	const ATOM_Vector3f *getVertices (unsigned frame) const;
	unsigned getNumTransforms (void) const;
	const ATOM_Matrix4x4f *getTransforms (void) const;
	void create (	unsigned numJointFrames, 
					const ATOM_VECTOR<unsigned> &jointCounts,
					const ATOM_VECTOR<const void*> &jointStreams,
					unsigned numUVFrames,
					const ATOM_VECTOR<unsigned> &uvCounts,
					const ATOM_VECTOR<const void*> &uvStreams,
					unsigned numVertexFrames,
					const ATOM_VECTOR<unsigned> &vertexCounts,
					const ATOM_VECTOR<const void*> &vertexStreams,
					unsigned numTransforms,
					const ATOM_Matrix4x4f *transformStream);
};

//struct ModelAnimationMeshKeyFrameMeshInfo
//{
//	// flag for keyframe. 1 means this is keyframe
//	int keyframe;
//
//	// for vertex animation
//	ATOM_VECTOR<ATOM_Vector3f> vertices;
//	ATOM_VECTOR<ATOM_Vector2f> uvs;
//
//	// bounding box for the mesh
//	ATOM_BBox bbox;
//};
//
//struct ModelAnimationKeyFrameJoint
//{
//	unsigned frameTime;
//
//	ATOM_VECTOR<ModelAnimationKeyFrameMeshInfo> keyFramesMeshes;
//
//	// for skeletal animation joint informations is same for all meshes
//	ATOM_VECTOR<ATOM_Matrix4x4f> jointTransforms;
//};

//struct ModelAnimationKeyFrameVertex
//{
//	unsigned frameTime;
//
//	ATOM_VECTOR<ModelAnimationKeyFrameMeshInfo> keyFramesMeshes;
//};

//class ModelAnimationKeyFrameCache: public ATOM_Object
//{
//public:
//	ATOM_VECTOR<ModelAnimationKeyFrameJoint> keyFrameJoints;
//	ATOM_VECTOR<ModelAnimationKeyFrameVertex> keyFrameVertices;
//};

#endif // __ATOM3D_ENGINE_MODELANIMATIONKEYFRAME_H
