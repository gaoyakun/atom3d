#ifndef __ATOM3D_ENGINE_ANIMATION_DATA_CACHE_H
#define __ATOM3D_ENGINE_ANIMATION_DATA_CACHE_H

#include "../ATOM_kernel.h"
#include "basedefs.h"
#include "modelanimationkeyframe.h"

class ATOM_JointAnimationDataCache: public ATOM_ReferenceObj
{
public:
	ATOM_JointAnimationDataCache (void);
	virtual ~ATOM_JointAnimationDataCache (void);

public:
	unsigned getNumFrames (void) const;
	unsigned getNumJoints (unsigned frame) const;
	void setNumJoints (unsigned frame, unsigned numJoints);
	ATOM_JointTransformInfo *getJoints (unsigned frame);
	const ATOM_JointTransformInfo *getJoints (unsigned frame) const;
	void setNumFrames (unsigned num);
	void setJoints (unsigned frame, const ATOM_JointTransformInfo *joints, unsigned numJoints);

private:
	ATOM_VECTOR<ATOM_VECTOR<ATOM_JointTransformInfo> > _jointTransforms;
};

#endif // __ATOM3D_ENGINE_ANIMATION_DATA_CACHE_H
