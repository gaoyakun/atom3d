#include "StdAfx.h"
#include "d3d9device.h"
#include "pixelshader_d3d.h"
//--- wangjian added ---//
#include <d3d10.h>
#include <D3Dcompiler.h>
//----------------------//

extern const char *ShaderCompileTargetNames[ATOM_SHADER_COMPILE_TARGET_COUNT];

ATOM_PixelShaderD3D9::ATOM_PixelShaderD3D9(void)
{
	_d3dPixelShader = 0;
	_constantTable = 0;
	_compileTarget = ATOM_SHADER_COMPILE_TARGET_UNKNOWN;
}

ATOM_PixelShaderD3D9::~ATOM_PixelShaderD3D9(void)
{
	setRenderDevice (0);
}

bool ATOM_PixelShaderD3D9::create (const char *source, const char *entryPoint, int shaderCompileTarget, bool createFromBin, ATOM_File * binFile)
{
	//---- wangjian added ---//
	// 从二进制文件中生成shader
	if( source && createFromBin )
	{
		IDirect3DDevice9 *d3ddevice = ((ATOM_D3D9Device*)getRenderDevice())->getD3DDevice();
		IDirect3DPixelShader9 *shader = 0;
		HRESULT hr = d3ddevice->CreatePixelShader((DWORD*)source, &shader);
		if (FAILED(hr))
		{
			ATOM_LOGGER::error("CreatePixelShader failed.\n");
			ATOM_CHECK_D3D_RESULT (hr);
			return false;
		}

		if (_d3dPixelShader)
		{
			_d3dPixelShader->Release();
		}
		_d3dPixelShader = shader;

		if (_constantTable)
		{
			_constantTable->Release();
		}

		D3DXGetShaderConstantTable( (DWORD*)source,&_constantTable);

		return true;
	}
	//----------------------//

	if (!ATOM_IS_VALID_SHADER_COMPILE_TARGET(shaderCompileTarget))
	{
		return false;
	}

	//--- wangjian commented ---//
	// 去除D3DX依赖：怎么去除？除了使用D3DXCompileShader编译shader,还能使用什么别的方式？
	// 怎么去除对 ID3DXConstantTable 的依赖？
#define USE_D3DX
#ifndef USE_D3DX
	ID3DBlob *buffer = 0;
	ID3DBlob *errors = 0;
	HRESULT hr = D3DCompile(source, strlen(source), NULL, NULL, NULL, entryPoint, ShaderCompileTargetNames[shaderCompileTarget], D3D10_SHADER_OPTIMIZATION_LEVEL3, 0, &buffer, &errors);
#else
	ID3DXBuffer *buffer = 0;
	ID3DXBuffer *errors = 0;
	ID3DXConstantTable *constantTable = 0;
	HRESULT hr = D3DXCompileShader(source, strlen(source), 0, 0, entryPoint, ShaderCompileTargetNames[shaderCompileTarget], 0, &buffer, &errors, &constantTable);
#endif

	if (FAILED(hr))
	{
		ATOM_LOGGER::error("HLSL pixel shader compilation failed:");
		if (errors)
		{
			ATOM_LOGGER::error("%s\n", (const char*)errors->GetBufferPointer());
			errors->Release();
			int prompt = ::MessageBoxA (ATOM_APP->getMainWindow(), "编译PixelShader失败，是否查看源文件？", "ATOM3D", MB_YESNO|MB_ICONQUESTION);
			if (prompt == IDYES)
			{
				char *fn = tempnam (0, "$atm");
				if (fn)
				{
					char buffer[MAX_PATH];
					strcpy (buffer, fn);
					strcat (buffer, ".txt");
					free (fn);

					FILE *fp = fopen (buffer, "wt");
					if (fp)
					{
						fwrite (source, 1, strlen(source), fp);
						fclose (fp);
						fp = 0;
					}

					::ShellExecute (NULL, "open", buffer, NULL, NULL, SW_SHOWNORMAL);
				}
			}
		}
		return false;
	}

	if (buffer)
	{
		IDirect3DDevice9 *d3ddevice = ((ATOM_D3D9Device*)getRenderDevice())->getD3DDevice();
		IDirect3DPixelShader9 *shader = 0;
		hr = d3ddevice->CreatePixelShader((DWORD*)buffer->GetBufferPointer(), &shader);
		if (FAILED(hr))
		{
			buffer->Release();
			ATOM_LOGGER::error("CreatePixelShader failed.\n");
			ATOM_CHECK_D3D_RESULT (hr);
			return false;
		}

		if (_d3dPixelShader)
		{
			_d3dPixelShader->Release();
		}
		_d3dPixelShader = shader;

		if (_constantTable)
		{
			_constantTable->Release();
		}
		_constantTable = constantTable;

		//--- wangjian added ---//
		// 写二进制数据
		if ( !binFile )
		{
			buffer->Release();
			return true;
		}
		
		// 写大小
		unsigned int size = (unsigned int)buffer->GetBufferSize();
		binFile->write (&size, sizeof(unsigned int));
		// 写数据
		binFile->write ((char*)buffer->GetBufferPointer(), size);

#if 0
		ATOM_LOGGER::log("file : %s, pixel shader %s, size : %d \n", binFile->getName(), entryPoint, size );
#endif

		buffer->Release();

		return true;
	}

	return false;
}

bool ATOM_PixelShaderD3D9::bind (void)
{
	IDirect3DDevice9 *d3ddevice = ((ATOM_D3D9Device*)getRenderDevice())->getD3DDevice();

	HRESULT hr = d3ddevice->SetPixelShader (_d3dPixelShader);
	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("SetPixelShader failed\n");
		ATOM_CHECK_D3D_RESULT(hr);
		return false;
	}

	return true;
}

ATOM_ShaderType ATOM_PixelShaderD3D9::getShaderType (void) const
{
	return ATOM_SHADER_TYPE_PIXELSHADER;
}

void ATOM_PixelShaderD3D9::invalidateImpl (bool needRestore)
{
	ATOM_ShaderD3D9::invalidateImpl (needRestore);

	if (!needRestore)
	{
		if (_d3dPixelShader)
		{
			_d3dPixelShader->Release();
			_d3dPixelShader = 0;
		}

		if (_constantTable)
		{
			_constantTable->Release();
			_constantTable = 0;
		}
	}
}

void ATOM_PixelShaderD3D9::restoreImpl (void)
{
	if (!_d3dPixelShader && !_source.empty ())
	{
		create (_source.c_str(), _entryPoint.c_str(), _compileTarget);
	}
}

