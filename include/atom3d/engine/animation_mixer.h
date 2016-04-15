#ifndef __ATOM3D_ENGINE_ANIMATION_MIXER_H
#define __ATOM3D_ENGINE_ANIMATION_MIXER_H

#if _MSC_VER > 1000
# pragma once
#endif

class ATOM_AnimationMixer
{
	struct AnimationInfo
	{
		ATOM_AUTOPTR(ATOM_InstanceTrack) track;
		unsigned startTick;
		unsigned cycleTime;
		unsigned cycleTick;
		unsigned loopCount;
public:

};

#endif // __ATOM3D_ENGINE_ANIMATION_MIXER_H
