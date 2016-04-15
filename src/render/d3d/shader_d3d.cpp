#include "StdAfx.h"
#include <d3d9.h>
#include <d3dx9.h>
#include "shader_d3d.h"
#include "d3d9device.h"

ATOM_ShaderD3D9::ATOM_ShaderD3D9(void)
{
	_constantTable = 0;
}

ATOM_Shader::ParamHandle ATOM_ShaderD3D9::getParamHandle (const char *name, ParamHandle parent) const
{
	if (_constantTable)
	{
		return (ATOM_Shader::ParamHandle)(_constantTable->GetConstantByName((D3DXHANDLE)parent, name));
	}
	return 0;
}

bool ATOM_ShaderD3D9::setFloat (ParamHandle param, float value)
{
	if (_constantTable)
	{
		HRESULT hr = _constantTable->SetFloat(((ATOM_D3D9Device*)getRenderDevice())->getD3DDevice(), (D3DXHANDLE)param, value);
		return SUCCEEDED(hr) ? true : false;
	}
	return false;
}

bool ATOM_ShaderD3D9::setFloatArray (ParamHandle param, const float *value, unsigned count)
{
	if (_constantTable)
	{
		HRESULT hr = _constantTable->SetFloatArray(((ATOM_D3D9Device*)getRenderDevice())->getD3DDevice(), (D3DXHANDLE)param, value, count);
		return SUCCEEDED(hr) ? true : false;
	}
	return false;
}

bool ATOM_ShaderD3D9::setInt (ParamHandle param, int value)
{
	if (_constantTable)
	{
		HRESULT hr = _constantTable->SetInt(((ATOM_D3D9Device*)getRenderDevice())->getD3DDevice(), (D3DXHANDLE)param, value);
		return SUCCEEDED(hr) ? true : false;
	}
	return false;
}

bool ATOM_ShaderD3D9::setIntArray (ParamHandle param, const int *value, unsigned count)
{
	if (_constantTable)
	{
		HRESULT hr = _constantTable->SetIntArray(((ATOM_D3D9Device*)getRenderDevice())->getD3DDevice(), (D3DXHANDLE)param, value, count);
		return SUCCEEDED(hr) ? true : false;
	}
	return false;
}

bool ATOM_ShaderD3D9::setMatrix44 (ParamHandle param, const ATOM_Matrix4x4f &value)
{
	if (_constantTable)
	{
		HRESULT hr = _constantTable->SetMatrix(((ATOM_D3D9Device*)getRenderDevice())->getD3DDevice(), (D3DXHANDLE)param, (D3DXMATRIX*)&value);
		return SUCCEEDED(hr) ? true : false;
	}
	return false;
}

bool ATOM_ShaderD3D9::setMatrix44Array (ParamHandle param, const ATOM_Matrix4x4f *value, unsigned count)
{
	if (_constantTable)
	{
		HRESULT hr = _constantTable->SetMatrixArray(((ATOM_D3D9Device*)getRenderDevice())->getD3DDevice(), (D3DXHANDLE)param, (D3DXMATRIX*)value, count);
		return SUCCEEDED(hr) ? true : false;
	}
	return false;
}

bool ATOM_ShaderD3D9::setMatrix43 (ParamHandle param, const ATOM_Matrix3x4f &value)
{
	if (_constantTable)
	{
		HRESULT hr = _constantTable->SetVectorArray(((ATOM_D3D9Device*)getRenderDevice())->getD3DDevice(), (D3DXHANDLE)param, (D3DXVECTOR4*)&value, 3);
		return SUCCEEDED(hr) ? true : false;
	}
	return false;
}

bool ATOM_ShaderD3D9::setMatrix43Array (ParamHandle param, const ATOM_Matrix3x4f *value, unsigned count)
{
	if (_constantTable)
	{
		//HRESULT hr = _constantTable->SetValue (((ATOM_D3D9Device*)getRenderDevice())->getD3DDevice(), (D3DXHANDLE)param, value, count*12*sizeof(float));
		HRESULT hr = _constantTable->SetVectorArray(((ATOM_D3D9Device*)getRenderDevice())->getD3DDevice(), (D3DXHANDLE)param, (D3DXVECTOR4*)value, count*3);
		return SUCCEEDED(hr) ? true : false;
	}
	return false;
}

bool ATOM_ShaderD3D9::setVector (ParamHandle param, const ATOM_Vector4f &value)
{
	if (_constantTable)
	{
		HRESULT hr = _constantTable->SetVector(((ATOM_D3D9Device*)getRenderDevice())->getD3DDevice(), (D3DXHANDLE)param, (D3DXVECTOR4*)&value);
		return SUCCEEDED(hr) ? true : false;
	}
	return false;
}

bool ATOM_ShaderD3D9::setVectorArray (ParamHandle param, const ATOM_Vector4f *value, unsigned count)
{
	if (_constantTable)
	{
		HRESULT hr = _constantTable->SetVectorArray(((ATOM_D3D9Device*)getRenderDevice())->getD3DDevice(), (D3DXHANDLE)param, (D3DXVECTOR4*)value, count);
		return SUCCEEDED(hr) ? true : false;
	}
	return false;
}

bool ATOM_ShaderD3D9::setValue (ParamHandle param, const void *value, unsigned bytes)
{
	if (_constantTable)
	{
		HRESULT hr = _constantTable->SetValue(((ATOM_D3D9Device*)getRenderDevice())->getD3DDevice(), (D3DXHANDLE)param, value, bytes);
		return SUCCEEDED(hr) ? true : false;
	}
	return false;
}

int ATOM_ShaderD3D9::getSamplerIndex (ParamHandle sampler)
{
	if (_constantTable)
	{
		return _constantTable->GetSamplerIndex ((D3DXHANDLE)sampler);
	}
	return -1;
}

int ATOM_ShaderD3D9::getArrayLength (ParamHandle param)
{
	if (_constantTable)
	{
		D3DXCONSTANT_DESC desc;
		UINT count = 1;

		HRESULT hr = _constantTable->GetConstantDesc ((D3DXHANDLE)param, &desc, &count);
		if (FAILED(hr))
		{
			return -1;
		}

		return desc.Elements;
	}

	return -1;
}

bool ATOM_ShaderD3D9::getParamDesc (ParamHandle param, ParamDesc *desc)
{
	memset (desc, 0, sizeof(ParamDesc));
	if (_constantTable)
	{
		D3DXCONSTANT_DESC d;
		UINT count = 1;

		HRESULT hr = _constantTable->GetConstantDesc ((D3DXHANDLE)param, &d, &count);
		if (SUCCEEDED(hr))
		{
			if (desc)
			{
				desc->numRows = d.Rows;
				desc->numCols = d.Columns;
				desc->numElements = d.Elements;
				desc->registerIndex = d.RegisterIndex;
				desc->registerCount = d.RegisterCount;
			}
			return true;
		}
	}
	return false;
}

