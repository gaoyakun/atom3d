#ifndef __ATOM3D_GFXDRIVER_GFXWINDOW_IMPL_H
#define __ATOM3D_GFXDRIVER_GFXWINDOW_IMPL_H

#include "gfxwindow.h"

class ATOM_GfxDriverImpl;

class ATOM_GfxWindowImpl: public ATOM_GfxWindow
{
public:
	ATOM_GfxWindowImpl (ATOM_GfxDriverImpl *driver);
	virtual ~ATOM_GfxWindowImpl (void);

public:
	virtual ATOM_GfxDriver *getGfxDriver (void) const;

private:
	ATOM_GfxDriver *_driver;

	ATOM_DECLARE_EVENT_MAP_NOPARENT(ATOM_GfxWindowImpl)
};

#endif // __ATOM3D_GFXDRIVER_GFXWINDOW_IMPL_H
