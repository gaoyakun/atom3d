#include "StdAfx.h"
#include "instanceskeleton.h"



ATOM_InstanceSkeleton::ATOM_InstanceSkeleton (void)
{
}

ATOM_InstanceSkeleton::ATOM_InstanceSkeleton (ATOM_Skeleton *skeleton)
{
	_skeleton = skeleton;
}

void ATOM_InstanceSkeleton::setSkeleton (ATOM_Skeleton *skeleton)
{
	_skeleton = skeleton;
}

ATOM_Skeleton *ATOM_InstanceSkeleton::getSkeleton (void) const
{
	return _skeleton.get();
}

void ATOM_InstanceSkeleton::setComponents (ATOM_Components components)
{
	_owner = components;
}

ATOM_Components ATOM_InstanceSkeleton::getComponents (void) const
{
	return _owner;
}


