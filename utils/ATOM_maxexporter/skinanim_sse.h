#ifndef __ATOM3D_MAYAEXPORTER_SKINANIM_SSE_H
#define __ATOM3D_MAYAEXPORTER_SKINANIM_SSE_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <ATOM_math.h>

struct JointTransformInfo
{
	ATOM_Quaternion rotate;
	ATOM_Vector4f translate;
	ATOM_Vector4f scale;
};

struct BaseVNT
{
	float px, py, pz, pw;
	float nx, ny, nz, nw;
	float tx, ty, tz, tw;
};

void slerpJointQuatsEx (JointTransformInfo *dst, const JointTransformInfo *q1, const JointTransformInfo *q2, float t, unsigned numJoints, const unsigned *indices);
void slerpJointQuats (JointTransformInfo *dst, const JointTransformInfo *q1, const JointTransformInfo *q2, float t, unsigned numJoints);
void convertJointQuatToJointMat (ATOM_Matrix3x4f *mats, const JointTransformInfo *quats, unsigned numJoints);
void untransformSkeleton (ATOM_Matrix3x4f *jointMats, const int *parents, int firstJoint, int lastJoint);
void transformJoint (ATOM_Matrix3x4f *result, const ATOM_Matrix3x4f *joints1, const ATOM_Matrix3x4f *joints2, int numJoints);

#endif // __ATOM3D_MAYAEXPORTER_SKINANIM_SSE_H
