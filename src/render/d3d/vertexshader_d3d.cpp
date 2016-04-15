#include "StdAfx.h"
#include "d3d9device.h"
#include "vertexshader_d3d.h"
#include "statecache.h"
//--- wangjian added ---//
#include <d3d10.h>
#include <D3Dcompiler.h>
//----------------------//

extern const char *ShaderCompileTargetNames[ATOM_SHADER_COMPILE_TARGET_COUNT];

ATOM_VertexShaderD3D9::ATOM_VertexShaderD3D9(void)
{
	_d3dVertexShader = 0;
	_constantTable = 0;
	_compileTarget = ATOM_SHADER_COMPILE_TARGET_UNKNOWN;
}

ATOM_VertexShaderD3D9::~ATOM_VertexShaderD3D9(void)
{
	setRenderDevice (0);
}

ATOM_ShaderType ATOM_VertexShaderD3D9::getShaderType (void) const
{
	return ATOM_SHADER_TYPE_VERTEXSHADER;
}

//--- wangjian modified ---//
// createFromBin : true:从二进制文件读取生成shader/false:写入二进制文件中
// binFile ： 二进制文件
bool ATOM_VertexShaderD3D9::create (const char *source, const char *entryPoint, int shaderCompileTarget, bool createFromBin, ATOM_File * binFile)
{
	//---- wangjian added ---//
	// 从二进制文件中生成shader
	if( source && createFromBin )
	{
		IDirect3DDevice9 *d3ddevice = ((ATOM_D3D9Device*)getRenderDevice())->getD3DDevice();
		IDirect3DVertexShader9 *shader = 0;
		HRESULT hr = d3ddevice->CreateVertexShader((DWORD*)source, &shader);
		if (FAILED(hr))
		{
			ATOM_LOGGER::error("CreateVertexShader failed.\n");
			ATOM_CHECK_D3D_RESULT (hr);
			return false;
		}

		if (_d3dVertexShader)
		{
			_d3dVertexShader->Release();
		}
		_d3dVertexShader = shader;

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

#define USE_D3DX
#ifndef USE_D3DX
	
	ID3DBlob *buffer = 0;
	ID3DBlob *errors = 0;
	HRESULT hr = D3DCompile(source, strlen(source), NULL, NULL, NULL, entryPoint, ShaderCompileTargetNames[shaderCompileTarget], D3D10_SHADER_OPTIMIZATION_LEVEL3, 0, &buffer, &errors);

#else
	ID3DXBuffer *buffer = 0;
	ID3DXBuffer *errors = 0;
	ID3DXConstantTable *constantTable = 0;

#if defined(DEBUG_D3D_SHADER)
	static int n = 0;
	char buffer[256];
	::CreateDirectoryA ("C:\\ShaderTemp", NULL);
	sprintf (buffer, "C:\\ShaderTemp\Shader%d.fx", n++);
	FILE *fp = fopen (buffer, "wt");
	fwrite (source, 1, strlen(source), fp);
	fclose (fp);
	HRESULT hr = D3DXCompileShaderFromFile (buffer, NULL, NULL, entryPoint, ShaderCompileTargetNames[shaderCompileTarget], D3DXSHADER_DEBUG|D3DXSHADER_SKIPOPTIMIZATION, &buffer, &errors, &constantTable);
#else

	HRESULT hr = D3DXCompileShader(source, strlen(source), 0, 0, entryPoint, ShaderCompileTargetNames[shaderCompileTarget], D3DXSHADER_OPTIMIZATION_LEVEL3, &buffer, &errors, &constantTable);

#endif

#endif

	if (FAILED(hr))
	{
		ATOM_LOGGER::error("HLSL vertex shader compilation failed:");
		if (errors)
		{
			ATOM_LOGGER::error("%s\n", (const char*)errors->GetBufferPointer());
			errors->Release();
			int prompt = ::MessageBoxA (ATOM_APP->getMainWindow(), "编译VertexShader失败，是否查看源文件？", "ATOM3D", MB_YESNO|MB_ICONQUESTION);
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
		IDirect3DVertexShader9 *shader = 0;
		hr = d3ddevice->CreateVertexShader((DWORD*)buffer->GetBufferPointer(), &shader);
		if (FAILED(hr))
		{
			buffer->Release();
			ATOM_LOGGER::error("CreateVertexShader failed.\n");
			ATOM_CHECK_D3D_RESULT (hr);
			return false;
		}

		if (_d3dVertexShader)
		{
			_d3dVertexShader->Release();
		}
		_d3dVertexShader = shader;

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
		ATOM_LOGGER::log("file : %s, vertex shader %s, size : %d \n", binFile->getName(), entryPoint, size );
#endif
		//---------------------//

		buffer->Release();

		return true;
	}

	return false;
}

bool ATOM_VertexShaderD3D9::bind (void)
{
	IDirect3DDevice9 *d3ddevice = ((ATOM_D3D9Device*)getRenderDevice())->getD3DDevice();

	HRESULT hr = d3ddevice->SetVertexShader (_d3dVertexShader);
	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("SetVertexShader failed\n");
		ATOM_CHECK_D3D_RESULT(hr);
		return false;
	}

	return true;
}

void ATOM_VertexShaderD3D9::invalidateImpl (bool needRestore)
{
	ATOM_ShaderD3D9::invalidateImpl (needRestore);

	if (!needRestore)
	{
		if (_d3dVertexShader)
		{
			_d3dVertexShader->Release();
			_d3dVertexShader = 0;
		}

		if (_constantTable)
		{
			_constantTable->Release();
			_constantTable = 0;
		}
	}
}

void ATOM_VertexShaderD3D9::restoreImpl (void)
{
	if (!_d3dVertexShader && !_source.empty ())
	{
		create (_source.c_str(), _entryPoint.c_str(), _compileTarget);
	}
}

