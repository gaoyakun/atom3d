#ifndef __ATOM3D_ENGINE_SKINANIM_SSE_H
#define __ATOM3D_ENGINE_SKINANIM_SSE_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "basedefs.h"
#include "model.h"
#include "modelanimationkeyframe.h"

struct JointScale
{
	float sx, sy, sz, sw;
};

struct BaseVNT
{
	float px, py, pz, pw;
	float nx, ny, nz, nw;
	float tx, ty, tz, tw;
};

//void ATOM_ENGINE_API slerpJointFast (ATOM_JointTransformInfo *joints, const ATOM_JointTransformInfo *blendJoints, const float lerp, int numJoints, const unsigned *index);
//void ATOM_ENGINE_API slerpJointFastEx (ATOM_JointTransformInfo *dst, const ATOM_JointTransformInfo *joints, const ATOM_JointTransformInfo *blendJoints, const float t, int numJoints, const unsigned *indices);
void ATOM_ENGINE_API slerpJointQuatsEx (ATOM_JointTransformInfo *dst, const ATOM_JointTransformInfo *q1, const ATOM_JointTransformInfo *q2, float t, unsigned numJoints, const unsigned *indices);
void ATOM_ENGINE_API slerpJointQuats (ATOM_JointTransformInfo *dst, const ATOM_JointTransformInfo *q1, const ATOM_JointTransformInfo *q2, float t, unsigned numJoints);
void ATOM_ENGINE_API convertJointQuatToJointMat (ATOM_Matrix3x4f *mats, const ATOM_JointTransformInfo *quats, unsigned numJoints);
void ATOM_ENGINE_API transformSkeleton (ATOM_Matrix3x4f *jointMats, const int *parents, int firstJoint, int lastJoint);
void ATOM_ENGINE_API untransformSkeleton (ATOM_Matrix3x4f *jointMats, const int *parents, int firstJoint, int lastJoint);
void ATOM_ENGINE_API transformJoint (ATOM_Matrix3x4f *result, const ATOM_Matrix3x4f *joints1, const ATOM_Matrix3x4f *joints2, int numJoints);
void ATOM_ENGINE_API transformSkeleton (ATOM_Matrix3x4f *mats, const int *parents, unsigned size);

void transformVerts (ATOM_Vector3f *verts,
					 const int numVerts,
					 const ATOM_Matrix3x4f *jointMats,
					 const ATOM_Mesh::SkeletonVertex *base,
					 const ATOM_Mesh::VertexWeight *weights,
					 const unsigned *remaps);
void transformVerts (ATOM_Vector3f *verts,
					 const int numVerts,
					 const ATOM_Matrix3x4f *jointMats,
					 const ATOM_Mesh::SkeletonVertex *base,
					 const ATOM_Mesh::VertexWeight *weights);
void transformVertsAndNormals (ATOM_Vector3f *verts,
							   ATOM_Vector3f *normals,
							   const int numVerts,
							   const ATOM_Matrix3x4f *jointMats,
							   const ATOM_Mesh::SkeletonVertex *base,
							   const ATOM_Mesh::VertexWeight *weights,
							   const unsigned *remaps);
void transformVertsAndNormals (ATOM_Vector3f *verts,
							   ATOM_Vector3f *normals,
							   const int numVerts,
							   const ATOM_Matrix3x4f *jointMats,
							   const ATOM_Mesh::SkeletonVertex *base,
							   const ATOM_Mesh::VertexWeight *weights);
void transformVertsAndNormalsAndTangents (ATOM_Vector3f *verts,
										  ATOM_Vector3f *normals,
										  ATOM_Vector3f *tangents,
										  const int numVerts,
										  const ATOM_Matrix3x4f *jointMats,
										  const ATOM_Mesh::SkeletonVertex *base,
										  const ATOM_Mesh::VertexWeight *weights,
										  const unsigned *remaps);
void transformVertsAndNormalsAndTangents (ATOM_Vector3f *verts,
										  ATOM_Vector3f *normals,
										  ATOM_Vector3f *tangents,
										  const int numVerts,
										  const ATOM_Matrix3x4f *jointMats,
										  const ATOM_Mesh::SkeletonVertex *base,
										  const ATOM_Mesh::VertexWeight *weights);

#endif // __ATOM3D_ENGINE_SKINANIM_SSE_H
