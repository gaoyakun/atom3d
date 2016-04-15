#include "gfxwindow_impl.h"
#include "gfxdriver_impl.h"

ATOM_BEGIN_EVENT_MAP_NOPARENT(ATOM_GfxWindowImpl)
ATOM_END_EVENT_MAP

ATOM_GfxWindowImpl::ATOM_GfxWindowImpl (ATOM_GfxDriverImpl *driver)
: _driver (driver)
{
}

ATOM_GfxWindowImpl::~ATOM_GfxWindowImpl (void)
{
}

ATOM_GfxDriver *ATOM_GfxWindowImpl::getGfxDriver (void) const
{
	return _driver;
}

