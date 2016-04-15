#ifndef __ATOM3D_ENGINE_INSTANCETRACK_H
#define __ATOM3D_ENGINE_INSTANCETRACK_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_kernel.h"
#include "modelanimationtrack.h"
#include "components.h"

class ATOM_ENGINE_API ATOM_InstanceTrack: public ATOM_ReferenceObj
{
public:
	ATOM_InstanceTrack (void);
	ATOM_InstanceTrack (ATOM_ModelAnimationTrack *track);

public:
	void setTrack (ATOM_ModelAnimationTrack *track);
	ATOM_ModelAnimationTrack *getTrack (void) const;
	void setComponents (ATOM_Components components);
	ATOM_Components getComponents (void) const;
	void setId (int id);
	int getId (void) const;

protected:
	ATOM_Components _owner;
	ATOM_AUTOPTR(ATOM_ModelAnimationTrack) _track;
	int _id;
};

#endif // __ATOM3D_ENGINE_INSTANCETRACK_H
