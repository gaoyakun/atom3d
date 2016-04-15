#ifndef __ATOM3D_RENDER_STATESTATUS_H
#define __ATOM3D_RENDER_STATESTATUS_H

#include "stateset.h"

class ATOM_StateCache
{
public:
	ATOM_StateCache (ATOM_RenderDevice *device);
	
public:
	virtual ATOM_RenderStateSet *getDesiredStates (void) const;
	virtual ATOM_RenderStateSet *getCurrentStates (void) const;
	virtual ATOM_RenderStateSet *getDefaultStates (void) const;
	virtual bool loadDefaults (void);
	virtual bool sync (void);
	virtual void setCurrentPass (int pass);
	virtual ATOM_RenderDevice *getDevice (void) const;
	
protected:
	ATOM_RenderDevice *_device;
	int _currentPass;
	ATOM_AUTOPTR(ATOM_RenderStateSet) _desiredStates;
	ATOM_AUTOPTR(ATOM_RenderStateSet) _currentStates;
	ATOM_AUTOPTR(ATOM_RenderStateSet) _defaultStates;
};

#endif // __ATOM3D_RENDER_STATESTATUS_H
