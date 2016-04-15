#ifndef __ATOM3D_RENDER_GPUPROGRAM_H
#define __ATOM3D_RENDER_GPUPROGRAM_H

#include "shader.h"

class ATOM_GPUProgram: public ATOM_Object
{
public:
	virtual ~ATOM_GPUProgram (void);
	virtual bool addShader (ATOM_Shader *shader) = 0;
	virtual void removeShader (ATOM_ShaderType type) = 0;
	virtual ATOM_Shader *getShader (ATOM_ShaderType type) const = 0;
	virtual void clear (void) = 0;
	virtual bool bind (void) = 0;
};

#endif // __ATOM3D_RENDER_GPUPROGRAM_H
