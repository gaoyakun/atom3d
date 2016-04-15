#include "stdafx.h"
#include "ibmanager_d3d.h"
#include "indexarray_d3d.h"

ATOM_D3DIBManager::~ATOM_D3DIBManager() 
{
}

ATOM_AUTOREF(ATOM_IndexArray) ATOM_D3DIBManager::_doAllocIndexArray(int usage, unsigned length, bool use32bit) {
  ATOM_HARDREF(ATOM_D3D9IndexArray) p;
  if (p)
  {
    p->setRenderDevice (_M_device);
	if (!p->realize (usage, length, use32bit))
	{
		return 0;
	}
  }
  return p.get();
}

