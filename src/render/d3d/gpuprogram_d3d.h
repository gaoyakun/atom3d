#ifndef __ATOM3D_RENDER_GPUPROGRAM_D3D_H
#define __ATOM3D_RENDER_GPUPROGRAM_D3D_H

#include "vertexshader_d3d.h"
#include "pixelshader_d3d.h"

class ATOM_D3D9Device;
class ATOM_GPUProgramD3D9: public ATOM_GPUProgram
{
	ATOM_CLASS(render, ATOM_GPUProgramD3D9, ATOM_GPUProgram)

public:
	ATOM_GPUProgramD3D9 (void);
	virtual ~ATOM_GPUProgramD3D9 (void);
	virtual bool addShader (ATOM_Shader *shader);
	virtual void removeShader (ATOM_ShaderType type);
	virtual ATOM_Shader *getShader (ATOM_ShaderType type) const;
	virtual void clear (void);
	virtual bool bind (void);

public:
	void setDevice (ATOM_D3D9Device *device);
	ATOM_D3D9Device *getDevice (void) const;

private:
	ATOM_D3D9Device *_device;
	ATOM_AUTOREF(ATOM_VertexShaderD3D9) _vertexShader;
	ATOM_AUTOREF(ATOM_PixelShaderD3D9) _pixelShader;
};

#endif // __ATOM3D_RENDER_GPUPROGRAM_D3D_H
