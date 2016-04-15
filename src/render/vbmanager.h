#ifndef __ATOM_GLRENDER_IVBMANAGER_H
#define __ATOM_GLRENDER_IVBMANAGER_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <vector>
#include <ATOM_utils.h>
#include <ATOM_kernel.h>
#include "gfxtypes.h"
#include "vertexarray.h"

class ATOM_VertexArray;
class ATOM_RenderDevice;
class ATOM_VBManager
{
public:
	ATOM_VBManager (ATOM_RenderDevice *device);
	virtual ~ATOM_VBManager() {}
	virtual ATOM_AUTOREF(ATOM_VertexArray) allocVertexArray(unsigned attributes, int usage, unsigned length, unsigned attribFlags=0);
protected:
	virtual ATOM_AUTOREF(ATOM_VertexArray) _doAllocVertexArray (unsigned attributes, int usage, unsigned attribFlags=0) = 0;
protected:
	ATOM_RenderDevice *_M_device;
};

inline ATOM_VBManager::ATOM_VBManager (ATOM_RenderDevice *device)
{
	_M_device = device;
}

#endif // __ATOM_GLRENDER_IVBMANAGER_H
