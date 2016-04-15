#ifndef __ATOM3D_ENGINE_JOINT_ANIMATION_TRACK
#define __ATOM3D_ENGINE_JOINT_ANIMATION_TRACK

#include "../ATOM_kernel.h"
#include "basedefs.h"
#include "animation_data_cache.h"

class ATOM_SharedModel;

class ATOM_JointAnimationTrack: public ATOM_ReferenceObj
{
public:
	ATOM_JointAnimationTrack (ATOM_SharedModel *model);
	virtual ~ATOM_JointAnimationTrack (void);

public:
	ATOM_SharedModel *getModel (void) const;
	void setName (const char *name);
	const char *getName (void) const;
	unsigned getNumFrames (void) const;
	void setNumFrames (unsigned num);
	unsigned getFrameJointIndex (unsigned frame) const;
	unsigned *getFrameJointIndices (void);
	const unsigned *getFrameJointIndices (void) const;
	void setFrameJointIndex (unsigned frame, unsigned jointIndex);
	void setFrameJointIndices (const unsigned *indices);
	unsigned getFrameTime (unsigned frame) const;
	const ATOM_JointTransformInfo *getFrameJoints (unsigned frame, unsigned *numJoints) const;
	unsigned *getFrameTimes (void);
	const unsigned *getFrameTimes (void) const;
	void setFrameTime (unsigned frame, unsigned time);
	void setFrameTimes (const unsigned *times);
	ATOM_JointAnimationDataCache *getAnimationDataCache (void) const;
	void setAnimationDataCache (ATOM_JointAnimationDataCache *cache);

public:
	float getAnimationFrame (unsigned tick) const;
	unsigned getTotalAnimationTime (void) const;

private:
	ATOM_SharedModel *_model;
	ATOM_STRING _name;
	ATOM_VECTOR<unsigned> _jointIndices;
	ATOM_VECTOR<unsigned> _frameTimes;
	ATOM_AUTOPTR(ATOM_JointAnimationDataCache) _dataCache;
};

#endif // __ATOM3D_ENGINE_JOINT_ANIMATION_TRACK
