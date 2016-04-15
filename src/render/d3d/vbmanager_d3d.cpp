#include "stdafx.h"
#include "vbmanager_d3d.h"
#include "vertexarray_d3d.h"

ATOM_ID3DVBManager::~ATOM_ID3DVBManager() 
{
}

ATOM_AUTOREF(ATOM_VertexArray) ATOM_ID3DVBManager::_doAllocVertexArray(unsigned attributes, int usage, unsigned attribFlags/*=0*/) 
  {
  ATOM_HARDREF(ATOM_D3DVertexArray) va;

  if (va)
  {
		va->setRenderDevice (_M_device);
		va->_M_usage = usage;
		va->_M_attributes = attributes;
		va->_M_vertex_size = ATOM_GetVertexSize (attributes,attribFlags);
		va->_attribFlags = attribFlags;
  }
  return va;
}

