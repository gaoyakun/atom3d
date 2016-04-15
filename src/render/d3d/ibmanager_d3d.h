#ifndef __ATOM_GLRENDER_IIBMANAGERD3D_H
#define __ATOM_GLRENDER_IIBMANAGERD3D_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "ibmanager.h"

class ATOM_D3DIBManager : public ATOM_IBManager
  {
  public:
    ATOM_D3DIBManager (ATOM_RenderDevice *device);
    virtual ~ATOM_D3DIBManager();

  protected:
    virtual ATOM_AUTOREF(ATOM_IndexArray) _doAllocIndexArray (int usage, unsigned length, bool use32bit);
  };

inline ATOM_D3DIBManager::ATOM_D3DIBManager (ATOM_RenderDevice *device): ATOM_IBManager (device)
{
}

#endif // __ATOM_GLRENDER_IIBMANAGERD3D_H
