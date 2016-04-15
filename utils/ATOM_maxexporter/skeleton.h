#ifndef __ATOM_MAYAEXPORTER_SKELETON_H
#define __ATOM_MAYAEXPORTER_SKELETON_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <hash_map>
#include <vector>
#include <string>

#include <ATOM_math.h>

class ATOM_Skeleton
{
public:
	struct joint
	{
		std::string name;
		int id;
		ATOM_Matrix4x4f localMatrix;
		ATOM_Matrix4x4f bindMatrix;
		int parentIndex;
		int remapIndex;
		unsigned hashCode;
	};

	ATOM_Skeleton();
	~ATOM_Skeleton();
	void clear();

	//get joints
	std::vector<joint>& getJoints();

	unsigned getHash (void) const;
	bool isIdentical (const ATOM_Skeleton &other) const;
	void remapJoint (void);
	static unsigned calcHash (const char *s, unsigned len);

	std::vector<unsigned> &getAttachPoints (void) { return attachPoints; }
	const std::vector<unsigned> &getAttachPoints (void) const { return attachPoints; }

	std::vector<int> &getRemapOldToNew (void) { return m_remapOldToNew; }
	std::vector<int> &getRemapNewToOld (void) { return m_remapNewToOld; }

protected:

	std::vector<unsigned> attachPoints;
	std::vector<joint> m_joints;
	std::vector<int> m_remapOldToNew;
	std::vector<int> m_remapNewToOld;
	std::vector<int> m_roots;
	mutable unsigned m_hash;
};

#endif // __ATOM_MAYAEXPORTER_SKELETON_H
