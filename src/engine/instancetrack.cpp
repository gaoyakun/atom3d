#include "StdAfx.h"
#include "instancetrack.h"
#include "instanceskeleton.h"


ATOM_InstanceTrack::ATOM_InstanceTrack (void)
{
	_id = -1;
}

ATOM_InstanceTrack::ATOM_InstanceTrack (ATOM_ModelAnimationTrack *track)
{
	_track = track;
	_id = -1;
}

void ATOM_InstanceTrack::setTrack (ATOM_ModelAnimationTrack *track)
{
	_track = track;
}

ATOM_ModelAnimationTrack *ATOM_InstanceTrack::getTrack (void) const
{
	return _track.get();
}

void ATOM_InstanceTrack::setComponents (ATOM_Components components)
{
	_owner = components;
}

ATOM_Components ATOM_InstanceTrack::getComponents (void) const
{
	return _owner;
}

void ATOM_InstanceTrack::setId (int id)
{
	_id = id;
}

int ATOM_InstanceTrack::getId (void) const
{
	return _id;
}



