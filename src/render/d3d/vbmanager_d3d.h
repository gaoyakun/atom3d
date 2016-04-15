#ifndef __ATOM_GLRENDER_IVBMANAGERD3D_H
#define __ATOM_GLRENDER_IVBMANAGERD3D_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "vbmanager.h"

class ATOM_ID3DVBManager : public ATOM_VBManager
  {
  public:
    ATOM_ID3DVBManager (ATOM_RenderDevice *device);
    virtual ~ATOM_ID3DVBManager();
  protected:
	virtual ATOM_AUTOREF(ATOM_VertexArray) _doAllocVertexArray (unsigned attributes, int usage, unsigned attribFlags=0);
  };

inline ATOM_ID3DVBManager::ATOM_ID3DVBManager (ATOM_RenderDevice *device): ATOM_VBManager(device) {
}

#endif // __ATOM_GLRENDER_IVBMANAGERD3D_H
