#include "StdAfx.h"
#include "animation_data_cache.h"

ATOM_JointAnimationDataCache::ATOM_JointAnimationDataCache (void)
{
}

ATOM_JointAnimationDataCache::~ATOM_JointAnimationDataCache (void)
{
}

unsigned ATOM_JointAnimationDataCache::getNumFrames (void) const
{
	return _jointTransforms.size ();
}

unsigned ATOM_JointAnimationDataCache::getNumJoints (unsigned frame) const
{
	return _jointTransforms[frame].size();
}

void ATOM_JointAnimationDataCache::setNumJoints (unsigned frame, unsigned numJoints)
{
	_jointTransforms[frame].resize (numJoints);
}

ATOM_JointTransformInfo *ATOM_JointAnimationDataCache::getJoints (unsigned frame)
{
	return &_jointTransforms[frame][0];
}

const ATOM_JointTransformInfo *ATOM_JointAnimationDataCache::getJoints (unsigned frame) const
{
	return &_jointTransforms[frame][0];
}

void ATOM_JointAnimationDataCache::setNumFrames (unsigned num)
{
	_jointTransforms.resize (num);
}

void ATOM_JointAnimationDataCache::setJoints (unsigned frame, const ATOM_JointTransformInfo *joints, unsigned numJoints)
{
	_jointTransforms[frame].resize (numJoints);

	if (numJoints && joints)
	{
		memcpy (getJoints(frame), joints, numJoints*sizeof(ATOM_JointTransformInfo));
	}
}

