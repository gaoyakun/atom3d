#ifndef __ATOM3D_ENGINE_SKELETON_H
#define __ATOM3D_ENGINE_SKELETON_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_dbghlp.h"
#include "../ATOM_kernel.h"
#include "basedefs.h"

class ATOM_ENGINE_API ATOM_Skeleton: public ATOM_ReferenceObj
{
public:
	ATOM_Skeleton (void);

	unsigned getHashCode (void) const;
	void setHashCode (unsigned code);
	bool isUpside (const char *name) const;
	bool isUpside (unsigned joint) const;
	void addAttachPoint (const char *name, unsigned jointIndex);
	void removeAttachPoint (const char *name);
	void clearAttachPoints (void);
	unsigned getNumAttachPoints (void) const;
	const ATOM_HASHMAP<ATOM_STRING, unsigned> &getAttachPoints (void) const;
	unsigned getAttachPointIndex (const char *name) const;
	const char *getAttachPointName (unsigned index) const;

	unsigned getNumJoints (void) const;
	void setNumJoints (unsigned num);
	void transformJoints (const ATOM_Matrix4x4f &matrix);
	void setJointBindMatrix (unsigned joint, const ATOM_Matrix3x4f &matrix);
	const ATOM_Matrix3x4f &getJointBindMatrix (unsigned joint) const;
	ATOM_VECTOR<ATOM_Matrix3x4f> &getJointMatrices (void);
	const ATOM_VECTOR<ATOM_Matrix3x4f> &getJointMatrices (void) const;
	const ATOM_VECTOR<ATOM_Matrix3x4f> &getInverseJointMatricesWorld (void) const;
	const ATOM_VECTOR<ATOM_Matrix3x4f> &getJointMatricesWorld (void) const;
	ATOM_VECTOR<int> &getJointParents (void);
	const ATOM_VECTOR<int> &getJointParents (void) const;

	const ATOM_VECTOR<unsigned> &getUpsideMask (void) const;
	const ATOM_VECTOR<unsigned> &getDownsideMask (void) const;

	void dump (void) const;

private:
	unsigned _hashCode;
	ATOM_VECTOR<unsigned> _upsideMask;
	ATOM_VECTOR<unsigned> _downsideMask;
	ATOM_HASHMAP<ATOM_STRING, unsigned> _attachPoints;
	ATOM_VECTOR<ATOM_Matrix3x4f> _jointBindMatrices;
	mutable ATOM_VECTOR<ATOM_Matrix3x4f> _jointInverseBindMatricesWorld;
	mutable ATOM_VECTOR<ATOM_Matrix3x4f> _jointBindMatricesWorld;
	ATOM_VECTOR<int> _jointParents;
};

#endif // __ATOM3D_ENGINE_SKELETON_H
