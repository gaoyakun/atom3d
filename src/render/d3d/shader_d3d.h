#ifndef __ATOM3D_RENDER_SHADER_D3D_H
#define __ATOM3D_RENDER_SHADER_D3D_H

#include <d3d9.h>
#include <d3dx9.h>
#include "shader.h"

class ATOM_ShaderD3D9: public ATOM_Shader
{
public:
    ATOM_ShaderD3D9(void);

public:
	virtual ParamHandle getParamHandle (const char *name, ParamHandle parent) const;
	virtual bool setFloat (ParamHandle param, float value);
	virtual bool setFloatArray (ParamHandle param, const float *value, unsigned count);
	virtual bool setInt (ParamHandle param, int value);
	virtual bool setIntArray (ParamHandle param, const int *value, unsigned count);
	virtual bool setMatrix44 (ParamHandle param, const ATOM_Matrix4x4f &value);
	virtual bool setMatrix44Array (ParamHandle param, const ATOM_Matrix4x4f *value, unsigned count);
	virtual bool setMatrix43 (ParamHandle param, const ATOM_Matrix3x4f &value);
	virtual bool setMatrix43Array (ParamHandle param, const ATOM_Matrix3x4f *value, unsigned count);
	virtual bool setVector (ParamHandle param, const ATOM_Vector4f &value);
	virtual bool setVectorArray (ParamHandle param, const ATOM_Vector4f *value, unsigned count);
	virtual bool setValue (ParamHandle param, const void *value, unsigned bytes);
	virtual int getSamplerIndex (ParamHandle sampler);
	virtual int getArrayLength (ParamHandle param);
	virtual bool getParamDesc (ParamHandle param, ParamDesc *desc);

protected:
	ID3DXConstantTable *_constantTable;
};

#endif // __ATOM3D_RENDER_SHADER_D3D_H
