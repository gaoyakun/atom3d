#include "StdAfx.h"
#include "gpuprogram_d3d.h"
#include "d3d9device.h"

ATOM_GPUProgramD3D9::ATOM_GPUProgramD3D9 (void)
{
	_device = 0;
}

ATOM_GPUProgramD3D9::~ATOM_GPUProgramD3D9 (void)
{
}

void ATOM_GPUProgramD3D9::setDevice (ATOM_D3D9Device *device)
{
	_device = device;
}

ATOM_D3D9Device *ATOM_GPUProgramD3D9::getDevice (void) const
{
	return _device;
}

bool ATOM_GPUProgramD3D9::addShader (ATOM_Shader *shader)
{
	if (shader)
	{
		switch (shader->getShaderType())
		{
		case ATOM_SHADER_TYPE_VERTEXSHADER:
			_vertexShader = (ATOM_VertexShaderD3D9*)shader;
			return true;
		case ATOM_SHADER_TYPE_PIXELSHADER:
			_pixelShader = (ATOM_PixelShaderD3D9*)shader;
			return true;
		}
	}
	return false;
}

void ATOM_GPUProgramD3D9::removeShader (ATOM_ShaderType type)
{
	switch (type)
	{
		case ATOM_SHADER_TYPE_VERTEXSHADER:
			_vertexShader = 0;
			break;
		case ATOM_SHADER_TYPE_PIXELSHADER:
			_pixelShader = 0;
			break;
	}
}

ATOM_Shader *ATOM_GPUProgramD3D9::getShader (ATOM_ShaderType type) const
{
	switch (type)
	{
		case ATOM_SHADER_TYPE_VERTEXSHADER:
			return _vertexShader.get();
		case ATOM_SHADER_TYPE_PIXELSHADER:
			return _pixelShader.get();
		default:
			return 0;
	}
}


void ATOM_GPUProgramD3D9::clear (void)
{
	_vertexShader = 0;
	_pixelShader = 0;
}

bool ATOM_GPUProgramD3D9::bind (void)
{
	if (_vertexShader)
	{
		if (!_vertexShader->bind ())
		{
			return false;
		}
	}
	else
	{
		_device->getD3DDevice()->SetVertexShader (NULL);
	}

	if (_pixelShader)
	{
		if (!_pixelShader->bind ())
		{
			return false;
		}
	}
	else
	{
		_device->getD3DDevice()->SetPixelShader (NULL);
	}

	return true;
}

