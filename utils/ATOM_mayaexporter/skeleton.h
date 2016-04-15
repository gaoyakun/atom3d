#ifndef __ATOM_MAYAEXPORTER_SKELETON_H
#define __ATOM_MAYAEXPORTER_SKELETON_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <hash_map>
#include <vector>
#include <string>

#include <ATOM_math.h>

#include <maya/MString.h>
#include <maya/MMatrix.h>
#include <maya/MDagPath.h>
#include <maya/MFnSkinCluster.h>

typedef struct jointTag
{
	MString name;
	int id;
	ATOM_Matrix4x4f localMatrix;
	ATOM_Matrix4x4f bindMatrix;
	int parentIndex;
	MDagPath jointDag;
	unsigned hashCode;
} joint;

static inline ATOM_Matrix4x4f MMatrixToMatrix (const MMatrix &matrix)
{
	ATOM_Matrix4x4f m;
	m.m00 = matrix(0, 2);
	m.m01 = matrix(0, 1);
	m.m02 = matrix(0, 0);
	m.m03 = matrix(0, 3);
	m.m10 = matrix(1, 2);
	m.m11 = matrix(1, 1);
	m.m12 = matrix(1, 0);
	m.m13 = matrix(1, 3);
	m.m20 = matrix(2, 2);
	m.m21 = matrix(2, 1);
	m.m22 = matrix(2, 0);
	m.m23 = matrix(2, 3);
	m.m30 = matrix(3, 2);
	m.m31 = matrix(3, 1);
	m.m32 = matrix(3, 0);
	m.m33 = matrix(3, 3);
	return m;
}

class ATOM_Skeleton
{
public:
	ATOM_Skeleton();
	~ATOM_Skeleton();
	void clear();

	MStatus load(void);
	//load skeleton data
	MStatus load(MFnSkinCluster* pSkinCluster);
	//load joint names
	MStatus loadNames (MFnSkinCluster *pSkinCluster);

	//get joints
	std::vector<joint>& getJoints();

	MFnSkinCluster *getSkinCluster (void) const { return m_skinCluster; }

	unsigned getHash (void) const;
	bool isIdentical (const ATOM_Skeleton &other) const;
	bool buildHeirarchy (void);
	void remapJoint (void);

	std::vector<unsigned> &getAttachPoints (void) { return attachPoints; }
	const std::vector<unsigned> &getAttachPoints (void) const { return attachPoints; }

	std::vector<int> &getRemapOldToNew (void) { return m_remapOldToNew; }
	std::vector<int> &getRemapNewToOld (void) { return m_remapNewToOld; }

protected:
	MStatus loadJoint(MDagPath& jointDag, joint* parent, MFnSkinCluster* pSkinCluster);

	std::vector<unsigned> attachPoints;
	std::vector<joint> m_joints;
	std::vector<int> m_remapOldToNew;
	std::vector<int> m_remapNewToOld;
	std::vector<int> m_roots;
	mutable unsigned m_hash;
	MFnSkinCluster *m_skinCluster;
};

#endif // __ATOM_MAYAEXPORTER_SKELETON_H
