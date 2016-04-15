#include "gfxresource.h"
#include "gfxdriver.h"

ATOM_GfxResource::ATOM_GfxResource (void) 
{
  _driver = 0;
  _prev = 0;
  _next = 0;
  _invalid = false;
  _needReset = false;
}

ATOM_GfxResource::~ATOM_GfxResource (void) {
  setGfxDriver (0);
}

bool ATOM_GfxResource::needRestore (void) const 
{
  return _invalid;
}

void ATOM_GfxResource::setNeedReset (bool b) 
{
  _needReset = b;
}

bool ATOM_GfxResource::needReset (void) const 
{
  return _needReset;
}

ATOM_GfxResource *ATOM_GfxResource::getNext (void) const 
{
  return _next;
}

ATOM_GfxResource *ATOM_GfxResource::getPrev (void) const 
{
  return _prev;
}

void ATOM_GfxResource::setGfxDriver (ATOM_GfxDriver *driver) {
  if (driver != _driver)
  {
    if (driver)
      driver->insertResource (this);
    else if (_driver)
      _driver->freeResource (this);

    _driver = driver;
  }
}

ATOM_GfxDriver *ATOM_GfxResource::getGfxDriver (void) const {
  return _driver;
}

void ATOM_GfxResource::invalidate (bool needRestore) {
  invalidateImpl (needRestore);
  _invalid = true;
}

void ATOM_GfxResource::restore (void) {
  if (_invalid)
  {
    _invalid = false;

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

