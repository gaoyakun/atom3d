#include "stdafx.h"
#include "depthbuffer.h"

ATOM_DepthBuffer::ATOM_DepthBuffer (void)
{
	_width = 0;
	_height = 0;
}

ATOM_DepthBuffer::~ATOM_DepthBuffer (void)
{
}

void ATOM_DepthBuffer::setRenderDevice (ATOM_RenderDevice *device)
{
	ATOM_GfxResource::setRenderDevice (device);
}

unsigned ATOM_DepthBuffer::getWidth(void) const
{
	return _width;
}

unsigned ATOM_DepthBuffer::getHeight(void) const
{
	return _height;
}


