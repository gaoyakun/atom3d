#ifndef __ATOM3D_RENDER_PIXELSHADER_D3D_H
#define __ATOM3D_RENDER_PIXELSHADER_D3D_H

#include <d3d9.h>
#include "shader_d3d.h"

class ATOM_PixelShaderD3D9: public ATOM_ShaderD3D9
{
    ATOM_CLASS(render, ATOM_PixelShaderD3D9, ATOM_PixelShaderD3D9)

public:
    ATOM_PixelShaderD3D9(void);
    virtual ~ATOM_PixelShaderD3D9(void);

public:
	virtual bool create (const char *source, const char *entryPoint, int shaderCompileTarget, bool createFromBin = false, ATOM_File * binFile = 0);
	virtual bool bind (void);
	virtual ATOM_ShaderType getShaderType (void) const;

protected:
    virtual void invalidateImpl (bool needRestore);
    virtual void restoreImpl (void);

private:
	ATOM_STRING _source;
	ATOM_STRING _entryPoint;
	int _compileTarget;
	IDirect3DPixelShader9 *_d3dPixelShader;
};

#endif // __ATOM3D_RENDER_PIXELSHADER_D3D_H
