#include "StdAfx.h"
#include "shader.h"

ATOM_Shader::ATOM_Shader (void)
{
}

ATOM_Shader::~ATOM_Shader (void)
{
}

void ATOM_Shader::setRenderDevice (ATOM_RenderDevice *device)
{
	ATOM_GfxResource::setRenderDevice (device);
}

