#include "stdafx.h"

ATOM_GfxResource::ATOM_GfxResource (void) 
{
  _M_device = 0;
  _M_prev = 0;
  _M_next = 0;
  _M_last_used_tick = 0;
  _M_invalid = false;
  _M_need_reset = false;
  _M_loading_state = LS_LOADED;
}

ATOM_GfxResource::~ATOM_GfxResource (void) {
  setRenderDevice (0);
}

bool ATOM_GfxResource::needRestore (void) const 
{
  return _M_invalid;
}

void ATOM_GfxResource::setNeedReset (bool b) 
{
  _M_need_reset = b;
}

bool ATOM_GfxResource::needReset (void) const 
{
  return _M_need_reset;
}

ATOM_GfxResource *ATOM_GfxResource::getNext (void) const 
{
  return _M_next;
}

ATOM_GfxResource *ATOM_GfxResource::getPrev (void) const 
{
  return _M_prev;
}

void ATOM_GfxResource::setRenderDevice (ATOM_RenderDevice *device) {
  if (device != _M_device)
  {
    if (device)
      device->insertResource (this);
    else if (_M_device)
      _M_device->freeResource (this);

    _M_device = device;
  }
}

ATOM_RenderDevice *ATOM_GfxResource::getRenderDevice (void) const {
  return _M_device;
}

void ATOM_GfxResource::invalidate (bool needRestore) {
  invalidateImpl (needRestore);
  _M_invalid = true;
}

void ATOM_GfxResource::restore (void) {
  if (_M_invalid)
  {
    _M_invalid = false;

    restoreImpl ();
  }
}

void ATOM_GfxResource::invalidateImpl (bool /* needRestore */) {
}

void ATOM_GfxResource::restoreImpl (void) {
}

bool ATOM_GfxResource::realize (void) 
{
  return true;
}

void ATOM_GfxResource::setLoadingState (LOADINGSTATE val)
{
	_M_loading_state = val;
}

ATOM_GfxResource::LOADINGSTATE ATOM_GfxResource::getLoadingState (void) const
{
	return _M_loading_state;
}

