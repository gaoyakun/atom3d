#ifndef __ATOM_GLRENDER_IIBMANAGER_H
#define __ATOM_GLRENDER_IIBMANAGER_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <vector>
#include <ATOM_utils.h>
#include <ATOM_kernel.h>
#include "gfxtypes.h"

class ATOM_IndexArray;
class ATOM_RenderDevice;
class ATOM_IBManager
{
public:
    ATOM_IBManager (ATOM_RenderDevice *device);
    virtual ~ATOM_IBManager(void) {}

public:
    virtual ATOM_AUTOREF(ATOM_IndexArray) allocIndexArray(int usage, unsigned length, bool use32bit);

protected:
    virtual ATOM_AUTOREF(ATOM_IndexArray) _doAllocIndexArray (int usage, unsigned length, bool use32bit) = 0;

protected:
    ATOM_RenderDevice *_M_device;
  };

// inline member functions
inline ATOM_IBManager::ATOM_IBManager (ATOM_RenderDevice *device) 
{
  _M_device = device;
}

#endif // __ATOM_GLRENDER_IIBMANAGER_H
