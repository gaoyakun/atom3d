#include "StdAfx.h"
#include "joint_animation_track.h"

ATOM_JointAnimationTrack::ATOM_JointAnimationTrack (ATOM_SharedModel *model)
{
	_model = model;
}

ATOM_JointAnimationTrack::~ATOM_JointAnimationTrack (void)
{
}

ATOM_SharedModel *ATOM_JointAnimationTrack::getModel (void) const
{
	return _model;
}

unsigned ATOM_JointAnimationTrack::getTotalAnimationTime (void) const
{
	return _frameTimes.empty() ? 0 : _frameTimes.back() - _frameTimes.front();
}

void ATOM_JointAnimationTrack::setName (const char *name)
{
	_name = name;
}

const char *ATOM_JointAnimationTrack::getName (void) const
{
	return _name.c_str();
}

unsigned ATOM_JointAnimationTrack::getNumFrames (void) const
{
	return _jointIndices.size();
}

void ATOM_JointAnimationTrack::setNumFrames (unsigned num)
{
	_jointIndices.resize (num);
	_frameTimes.resize (num);
}

unsigned ATOM_JointAnimationTrack::getFrameJointIndex (unsigned frame) const
{
	return _jointIndices[frame];
}

unsigned *ATOM_JointAnimationTrack::getFrameJointIndices (void)
{
	return _jointIndices.empty() ? 0 : &_jointIndices[0];
}

const unsigned *ATOM_JointAnimationTrack::getFrameJointIndices (void) const
{
	return _jointIndices.empty() ? 0 : &_jointIndices[0];
}

void ATOM_JointAnimationTrack::setFrameJointIndex (unsigned frame, unsigned jointIndex)
{
	_jointIndices[frame] = jointIndex;
}

unsigned *ATOM_JointAnimationTrack::getFrameTimes (void)
{
	return _frameTimes.empty() ? 0 : &_frameTimes[0];
}

const unsigned *ATOM_JointAnimationTrack::getFrameTimes (void) const
{
	return _frameTimes.empty() ? 0 : &_frameTimes[0];
}

void ATOM_JointAnimationTrack::setFrameJointIndices (const unsigned *indices)
{
	memcpy (&_jointIndices[0], indices, sizeof(unsigned)*_jointIndices.size());
}

unsigned ATOM_JointAnimationTrack::getFrameTime (unsigned frame) const
{
	return _frameTimes[frame];
}

void ATOM_JointAnimationTrack::setFrameTime (unsigned frame, unsigned time)
{
	_frameTimes[frame] = time;
}

void ATOM_JointAnimationTrack::setFrameTimes (const unsigned *times)
{
	memcpy (&_frameTimes[0], times, sizeof(unsigned)*_frameTimes.size());
}

float ATOM_JointAnimationTrack::getAnimationFrame (unsigned tick) const
{
	int frame1 = -1;

	if (_frameTimes.size () > 0)
	{
		tick += _frameTimes[0];

		ATOM_VECTOR<unsigned>::const_iterator it = std::lower_bound (_frameTimes.begin(), _frameTimes.end(), tick);
		if (it == _frameTimes.end ())
		{
			return _frameTimes.size() - 1;
		}
		else if (it == _frameTimes.begin())
		{
			return 0.f;
		}
		else
		{
			unsigned time = *it;
			if (time > tick)
			{
				unsigned prev = *(it-1);
				float frac = ((float)tick - (float)prev)/((float)time-(float)prev);
				return (it-1-_frameTimes.begin()) + frac;
			}
			else
			{
				return it-_frameTimes.begin();
			}
		}
	}

	return frame1;
}

ATOM_JointAnimationDataCache *ATOM_JointAnimationTrack::getAnimationDataCache (void) const
{
	return _dataCache.get();
}

void ATOM_JointAnimationTrack::setAnimationDataCache (ATOM_JointAnimationDataCache *cache)
{
	_dataCache = cache;
}

const ATOM_JointTransformInfo *ATOM_JointAnimationTrack::getFrameJoints (unsigned frame, unsigned *numJoints) const
{
	unsigned jointIndex = getFrameJointIndex (frame);
	ATOM_ASSERT(jointIndex < _dataCache->getNumFrames ());
	if (numJoints)
	{
		*numJoints = _dataCache->getNumJoints (jointIndex);
	}
	return _dataCache->getJoints (jointIndex);
}

