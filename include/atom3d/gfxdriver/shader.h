#ifndef __ATOM3D_GFXDRIVER_SHADER_H
#define __ATOM3D_GFXDRIVER_SHADER_H

#include "basedefs.h"
#include "gfxresource.h"

#define ATOM_SHADER_COMPILE_TARGET_UNKNOWN	-1
#define ATOM_SHADER_COMPILE_TARGET_VS_2_0	0
#define ATOM_SHADER_COMPILE_TARGET_VS_2_X	1
#define ATOM_SHADER_COMPILE_TARGET_VS_3_0	2
#define ATOM_SHADER_COMPILE_TARGET_VS_4_0	3
#define ATOM_SHADER_COMPILE_TARGET_VS_4_1	4
#define ATOM_SHADER_COMPILE_TARGET_VS_5_0	5
#define ATOM_SHADER_COMPILE_TARGET_PS_2_0	6
#define ATOM_SHADER_COMPILE_TARGET_PS_2_X	7
#define ATOM_SHADER_COMPILE_TARGET_PS_3_0	8
#define ATOM_SHADER_COMPILE_TARGET_PS_4_0	9
#define ATOM_SHADER_COMPILE_TARGET_PS_4_1	10
#define ATOM_SHADER_COMPILE_TARGET_PS_5_0	11
#define ATOM_SHADER_COMPILE_TARGET_COUNT	12
#define ATOM_IS_VALID_SHADER_COMPILE_TARGET(target) ((target)>=0&&(target)<ATOM_SHADER_COMPILE_TARGET_COUNT)

enum ATOM_ShaderType
{
	ATOM_SHADER_TYPE_UNKNOWN = 0,
	ATOM_SHADER_TYPE_VERTEXSHADER = 1,
	ATOM_SHADER_TYPE_PIXELSHADER = 2,
	ATOM_SHADER_TYPE_GEOMETRYSHADER = 3
};

class ATOM_Shader: public ATOM_GfxResource
{
public:
	typedef void *ParamHandle;

protected:
	ATOM_Shader (void);
	virtual ~ATOM_Shader (void);

public:
	virtual ATOM_ShaderType getShaderType (void) const = 0;
	virtual bool create (const char *source, const char *entryPoint, int shaderCompileTarget) = 0;
	virtual bool bind (void) = 0;
	virtual ParamHandle getParamHandle (const char *name, ParamHandle parent) const = 0;
	virtual bool setFloat (ParamHandle param, float value) = 0;
	virtual bool setFloatArray (ParamHandle param, const float *value, unsigned count) = 0;
	virtual bool setInt (ParamHandle param, int value) = 0;
	virtual bool setIntArray (ParamHandle param, const int *value, unsigned count) = 0;
	virtual bool setMatrix44 (ParamHandle param, const ATOM_Matrix4x4f &value) = 0;
	virtual bool setMatrix44Array (ParamHandle param, const ATOM_Matrix4x4f *value, unsigned count) = 0;
	virtual bool setMatrix43 (ParamHandle param, const ATOM_Matrix3x4f &value) = 0;
	virtual bool setMatrix43Array (ParamHandle param, const ATOM_Matrix3x4f *value, unsigned count) = 0;
	virtual bool setVector (ParamHandle param, const ATOM_Vector4f &value) = 0;
	virtual bool setVectorArray (ParamHandle param, const ATOM_Vector4f *value, unsigned count) = 0;
	virtual bool setValue (ParamHandle param, const void *value, unsigned bytes) = 0;
	virtual int getSamplerIndex (ParamHandle sampler) = 0;
	virtual int getArrayLength (ParamHandle param) = 0;
};

#endif // __ATOM3D_GFXDRIVER_SHADER_H
