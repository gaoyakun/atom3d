#ifndef __ATOM3D_ENGINE_INSTANCESKELETON_H
#define __ATOM3D_ENGINE_INSTANCESKELETON_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_kernel.h"
#include "skeleton.h"
#include "components.h"

class ATOM_ENGINE_API ATOM_InstanceSkeleton: public ATOM_ReferenceObj
{
public:
	ATOM_InstanceSkeleton (void);
	ATOM_InstanceSkeleton (ATOM_Skeleton *skeleton);

public:
	void setSkeleton (ATOM_Skeleton *skeleton);
	ATOM_Skeleton *getSkeleton (void) const;
	void setComponents (ATOM_Components components);
	ATOM_Components getComponents (void) const;

protected:
	ATOM_Components _owner;
	ATOM_AUTOPTR(ATOM_Skeleton) _skeleton;
};

#endif // __ATOM3D_ENGINE_INSTANCESKELETON_H
