#ifndef __ATOM3D_RENDER_PARAMETER_H
#define __ATOM3D_RENDER_PARAMETER_H

#include <ATOM_kernel.h>
#include <ATOM_math.h>
#include <ATOM_render.h>
#include "basedefs.h"

#define ATOM_PARAMETER_TYPE_IS_ARRAY(type) ((((unsigned)(type))&0x80000000)!=0)
#define ATOM_PARAMETER_TYPE_GET_SIZE(type) ((((unsigned)(type))&0x7FFFFFFF)>>4)

class ATOM_CoreMaterial;
class ATOM_MaterialEffect;
class ATOM_MaterialPass;
class ATOM_EffectParameterValue;

class ATOM_ENGINE_API ATOM_MaterialParam: public ATOM_ReferenceObj
{
public:
	typedef void (*ParamCallback)(ATOM_EffectParameterValue *param, void *context, void *context2);
	typedef void (*ValueCallback)(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);
	struct CallbackInfo
	{
		ParamCallback callback;
		void *context;
		void *context2;
		ATOM_MaterialEffect *effect;
#if 1
		ATOM_MaterialPass * pass;			// wangjian added : EFFECT PASS
#endif
	};

	enum
	{
		Flag_ShaderConstantVS = (1<<0),
		Flag_ShaderConstantPS = (1<<1),
		Flag_StateValue = (1<<2),
		Flag_ConditionValue = (1<<3)
	};

	enum EditorType
	{
		ParamEditorType_Disable = 0,
		ParamEditorType_None,
		ParamEditorType_Bool,
		ParamEditorType_Color,
		ParamEditorType_Direction,
		ParamEditorType_Constant_PrimitiveType,
		ParamEditorType_Constant_ColorMask,
		ParamEditorType_Constant_ShadeMode,
		ParamEditorType_Constant_BlendFunc,
		ParamEditorType_Constant_BlendOp,
		ParamEditorType_Constant_CompareFunc,
		ParamEditorType_Constant_StencilOp,
		ParamEditorType_Constant_FillMode,
		ParamEditorType_Constant_CullMode,
		ParamEditorType_Constant_FrontFace,
		ParamEditorType_Constant_SamplerFilter,
		ParamEditorType_Constant_SamplerAddress,
		ParamEditorType_Constant_TextureOp,
		ParamEditorType_Constant_TextureArg,
		ParamEditorType_Constant_TexCoordGen,
		ParamEditorType_Constant_FogMode
	};

	enum Type
	{
		ParamType_Unknown = 0,
		ParamType_Float = 1|(sizeof(float)<<4),
		ParamType_FloatArray = 0x80000000|ParamType_Float,
		ParamType_Int = 2|(sizeof(int)<<4),
		ParamType_IntArray = 0x80000000|ParamType_Int,
		ParamType_Matrix44 = 3|(sizeof(ATOM_Matrix4x4f)<<4),
		ParamType_Matrix44Array = 0x80000000|ParamType_Matrix44,
		ParamType_Matrix43 = 4|(sizeof(ATOM_Matrix3x4f)<<4),
		ParamType_Matrix43Array = 0x80000000|ParamType_Matrix43,
		ParamType_Vector = 5|(sizeof(ATOM_Vector4f)<<4),
		ParamType_VectorArray = 0x80000000|ParamType_Vector,
		ParamType_Texture = 6,
		ParamType_Sampler = 7
	};

public:
	ATOM_MaterialParam (void);
	virtual ~ATOM_MaterialParam (void);
	virtual Type getParamType (void) const;
	virtual unsigned getDimension (void) const;
	virtual unsigned getValueSize (void) const = 0;
	virtual void *getValuePtr (void) = 0;
	virtual unsigned getRegisterCount (void);
	virtual void writeRegisterValue (ATOM_Vector4f *buffer);
	virtual void writeRegisterValue (ATOM_Vector4f *buffer, const void *valuePtr, const ATOM_Shader::ParamDesc *desc);

public:
	void setValueCallback (ValueCallback callback);

	//--------------------------------------------------//
	// wangjian modified : 增加一个参数：EFFECT PASS
#if 0
	void addApplyCallback (ParamCallback callback, void *context, void *context2, ATOM_MaterialEffect *effect);
#else
	void addApplyCallback (ParamCallback callback, void *context, void *context2, ATOM_MaterialEffect *effect, ATOM_MaterialPass *pass = 0);
#endif
	//--------------------------------------------------//

	ValueCallback getValueCallback (void) const;
	void setEditorType (EditorType editorType);
	EditorType getEditorType (void) const;
	void setRegisterIndex (int index);
	int getRegisterIndex (void) const;
	void setParamIndex (int index);
	int getParamIndex (void) const;
	void setFlags (unsigned flags);
	unsigned getFlags (void) const;
	//void apply (ATOM_MaterialEffect *effect/*, unsigned int pass*/);
	int getNumCallbacks (void) const;
	const CallbackInfo *getCallback (int i) const;
	const CallbackInfo *getCallbackForEffect (ATOM_MaterialEffect *effect) const;
	//--- wangjian added ---//
	/*void setPassIndex( unsigned int pass )
	{
		_pass |= 1 << pass;
	}
	unsigned int validatePass( unsigned int pass )
	{
		return _pass == 0 ? ( pass == 0 ? 1 : 1 ) : _pass & (1<<pass);
	}*/
	//----------------------//

private:
	ATOM_VECTOR<CallbackInfo> _applyCallbacks;
	ValueCallback _valueCallback;
	EditorType _editorType;
	int _registerIndex;
	int _parameterIndex;
	unsigned _flags;
	//--- wangjian added for pass test ---//
	//unsigned int _pass;
	//------------------------------------//
};

class ATOM_MaterialFloatParam: public ATOM_MaterialParam
{
public:
	ATOM_MaterialFloatParam (void);
	ATOM_MaterialFloatParam (float value);
	virtual Type getParamType (void) const;
	virtual unsigned getValueSize (void) const;
	virtual void *getValuePtr (void);
	virtual unsigned getRegisterCount (void);
	virtual void writeRegisterValue (ATOM_Vector4f *buffer);
	virtual void writeRegisterValue (ATOM_Vector4f *buffer, const void *valuePtr, const ATOM_Shader::ParamDesc *desc);

public:
	void setValue (float value);
	float getValue (void) const;

private:
	float _value;
	float _defaultValue;
};

class ATOM_MaterialFloatArrayParam: public ATOM_MaterialParam
{
public:
	ATOM_MaterialFloatArrayParam (unsigned dimension);
	ATOM_MaterialFloatArrayParam (unsigned dimension, const float *value);
	virtual ~ATOM_MaterialFloatArrayParam (void);
	virtual Type getParamType (void) const;
	virtual unsigned getValueSize (void) const;
	virtual void *getValuePtr (void);
	virtual unsigned getDimension (void) const;
	virtual void setDimension (unsigned dimension);
	virtual unsigned getRegisterCount (void);
	virtual void writeRegisterValue (ATOM_Vector4f *buffer);
	virtual void writeRegisterValue (ATOM_Vector4f *buffer, const void *valuePtr, const ATOM_Shader::ParamDesc *desc);

public:
	void setValue (const float *value, unsigned count);
	float *getValue (void) const;
	void setElement (unsigned index, float value);

private:
	float *_value;
	unsigned _dimension;
};

class ATOM_MaterialIntParam: public ATOM_MaterialParam
{
public:
	ATOM_MaterialIntParam (void);
	ATOM_MaterialIntParam (int value);
	virtual Type getParamType (void) const;
	virtual unsigned getValueSize (void) const;
	virtual void *getValuePtr (void);
	virtual unsigned getRegisterCount (void);
	virtual void writeRegisterValue (ATOM_Vector4f *buffer);
	virtual void writeRegisterValue (ATOM_Vector4f *buffer, const void *valuePtr, const ATOM_Shader::ParamDesc *desc);

public:
	void setValue (int value);
	int getValue (void) const;

private:
	int _value;
};

class ATOM_MaterialIntArrayParam: public ATOM_MaterialParam
{
public:
	ATOM_MaterialIntArrayParam (unsigned dimension);
	ATOM_MaterialIntArrayParam (unsigned dimension, const int *value);
	virtual ~ATOM_MaterialIntArrayParam (void);
	virtual Type getParamType (void) const;
	virtual unsigned getValueSize (void) const;
	virtual void *getValuePtr (void);
	virtual unsigned getDimension (void) const;
	virtual void setDimension (unsigned dimension);
	virtual unsigned getRegisterCount (void);
	virtual void writeRegisterValue (ATOM_Vector4f *buffer);
	virtual void writeRegisterValue (ATOM_Vector4f *buffer, const void *valuePtr, const ATOM_Shader::ParamDesc *desc);

public:
	void setValue (const int *value, unsigned count);
	int *getValue (void) const;
	void setElement (unsigned index, int value);

private:
	int *_value;
	unsigned _dimension;
};

class ATOM_MaterialMatrix44Param: public ATOM_MaterialParam
{
public:
	ATOM_MaterialMatrix44Param (void);
	ATOM_MaterialMatrix44Param (const ATOM_Matrix4x4f &value);
	virtual Type getParamType (void) const;
	virtual unsigned getValueSize (void) const;
	virtual void *getValuePtr (void);
	virtual unsigned getRegisterCount (void);
	virtual void writeRegisterValue (ATOM_Vector4f *buffer);
	virtual void writeRegisterValue (ATOM_Vector4f *buffer, const void *valuePtr, const ATOM_Shader::ParamDesc *desc);

public:
	void setValue (const ATOM_Matrix4x4f &matrix);
	const ATOM_Matrix4x4f &getValue (void) const;

private:
	ATOM_Matrix4x4f _value;
};

class ATOM_MaterialMatrix44ArrayParam: public ATOM_MaterialParam
{
public:
	ATOM_MaterialMatrix44ArrayParam (unsigned dimension);
	ATOM_MaterialMatrix44ArrayParam (unsigned dimension, const ATOM_Matrix4x4f *value);
	virtual ~ATOM_MaterialMatrix44ArrayParam (void);
	virtual Type getParamType (void) const;
	virtual unsigned getValueSize (void) const;
	virtual void *getValuePtr (void);
	virtual unsigned getDimension (void) const;
	virtual void setDimension (unsigned dimension);
	virtual unsigned getRegisterCount (void);
	virtual void writeRegisterValue (ATOM_Vector4f *buffer);
	virtual void writeRegisterValue (ATOM_Vector4f *buffer, const void *valuePtr, const ATOM_Shader::ParamDesc *desc);

public:
	void setValue (const ATOM_Matrix4x4f *matrix, unsigned count);
	ATOM_Matrix4x4f *getValue (void) const;
	void setElement (unsigned index, const ATOM_Matrix4x4f &value);

private:
	ATOM_Matrix4x4f *_value;
	unsigned _dimension;
};

class ATOM_MaterialMatrix43Param: public ATOM_MaterialParam
{
public:
	ATOM_MaterialMatrix43Param (void);
	ATOM_MaterialMatrix43Param (const ATOM_Matrix3x4f &value);
	virtual Type getParamType (void) const;
	virtual unsigned getValueSize (void) const;
	virtual void *getValuePtr (void);
	virtual unsigned getRegisterCount (void);
	virtual void writeRegisterValue (ATOM_Vector4f *buffer);
	virtual void writeRegisterValue (ATOM_Vector4f *buffer, const void *valuePtr, const ATOM_Shader::ParamDesc *desc);

public:
	void setValue (const ATOM_Matrix3x4f &matrix);
	const ATOM_Matrix3x4f &getValue (void) const;

private:
	ATOM_Matrix3x4f _value;
};

class ATOM_MaterialMatrix43ArrayParam: public ATOM_MaterialParam
{
public:
	ATOM_MaterialMatrix43ArrayParam (unsigned dimension);
	ATOM_MaterialMatrix43ArrayParam (unsigned dimension, const ATOM_Matrix3x4f *value);
	virtual ~ATOM_MaterialMatrix43ArrayParam (void);
	virtual Type getParamType (void) const;
	virtual unsigned getValueSize (void) const;
	virtual void *getValuePtr (void);
	virtual unsigned getDimension (void) const;
	virtual void setDimension (unsigned dimension);
	virtual unsigned getRegisterCount (void);
	virtual void writeRegisterValue (ATOM_Vector4f *buffer);
	virtual void writeRegisterValue (ATOM_Vector4f *buffer, const void *valuePtr, const ATOM_Shader::ParamDesc *desc);

public:
	void setValue (const ATOM_Matrix3x4f *matrix, unsigned count);
	ATOM_Matrix3x4f *getValue (void) const;
	void setElement (unsigned index, const ATOM_Matrix3x4f &value);

private:
	ATOM_Matrix3x4f *_value;
	unsigned _dimension;
};

class ATOM_MaterialVectorParam: public ATOM_MaterialParam
{
public:
	ATOM_MaterialVectorParam (void);
	ATOM_MaterialVectorParam (const ATOM_Vector4f &value);
	virtual Type getParamType (void) const;
	virtual unsigned getValueSize (void) const;
	virtual void *getValuePtr (void);
	virtual unsigned getRegisterCount (void);
	virtual void writeRegisterValue (ATOM_Vector4f *buffer);
	virtual void writeRegisterValue (ATOM_Vector4f *buffer, const void *valuePtr, const ATOM_Shader::ParamDesc *desc);

public:
	void setValue (const ATOM_Vector4f &value);
	const ATOM_Vector4f &getValue (void) const;

private:
	ATOM_Vector4f _value;
};

class ATOM_MaterialVectorArrayParam: public ATOM_MaterialParam
{
public:
	ATOM_MaterialVectorArrayParam (unsigned dimension);
	ATOM_MaterialVectorArrayParam (unsigned dimension, const ATOM_Vector4f *value);
	virtual ~ATOM_MaterialVectorArrayParam (void);
	virtual Type getParamType (void) const;
	virtual unsigned getValueSize (void) const;
	virtual void *getValuePtr (void);
	virtual unsigned getDimension (void) const;
	virtual void setDimension (unsigned dimension);
	virtual unsigned getRegisterCount (void);
	virtual void writeRegisterValue (ATOM_Vector4f *buffer);
	virtual void writeRegisterValue (ATOM_Vector4f *buffer, const void *valuePtr, const ATOM_Shader::ParamDesc *desc);

public:
	void setValue (const ATOM_Vector4f *value, unsigned count);
	ATOM_Vector4f *getValue (void) const;
	void setElement (unsigned index, const ATOM_Vector4f &value);

private:
	ATOM_Vector4f *_value;
	unsigned _dimension;
};

class ATOM_MaterialTextureParam: public ATOM_MaterialParam
{
public:
	ATOM_MaterialTextureParam (void);
	ATOM_MaterialTextureParam (ATOM_Texture *value);
	virtual Type getParamType (void) const;
	virtual unsigned getValueSize (void) const;
	virtual void *getValuePtr (void);

public:
	void setValue (ATOM_Texture *value);
	ATOM_Texture *getValue (void) const;
	void setFileName (const char *filename);
	const char *getFileName (void) const;

private:
	ATOM_STRING _filename;
	ATOM_AUTOREF(ATOM_Texture) _value;
};

class ATOM_MaterialSamplerParam: public ATOM_MaterialParam
{
public:
	ATOM_MaterialSamplerParam (void);
	virtual Type getParamType (void) const;
	virtual unsigned getValueSize (void) const;
	virtual void *getValuePtr (void);

public:
	void setSamplerStates (ATOM_SamplerAttributes *value);
	ATOM_SamplerAttributes *getSamplerStates (void) const;
	void setIndex (unsigned index);
	unsigned getIndex (void) const;
	void setTexParam (ATOM_MaterialTextureParam *param);
	ATOM_MaterialTextureParam *getTexParam (void) const;

private:
	unsigned _index;
	ATOM_AUTOPTR(ATOM_MaterialTextureParam) _texParam;
	ATOM_AUTOPTR(ATOM_SamplerAttributes) _samplerStates;
};

template <class T>
struct ATOM_MaterialParamTypeT;

template <>
struct ATOM_MaterialParamTypeT<ATOM_MaterialFloatParam>
{
	static const int type = ATOM_MaterialParam::ParamType_Float;
};

template <>
struct ATOM_MaterialParamTypeT<ATOM_MaterialFloatArrayParam>
{
	static const int type = ATOM_MaterialParam::ParamType_FloatArray;
};

template <>
struct ATOM_MaterialParamTypeT<ATOM_MaterialIntParam>
{
	static const int type = ATOM_MaterialParam::ParamType_Int;
};

template <>
struct ATOM_MaterialParamTypeT<ATOM_MaterialMatrix44Param>
{
	static const int type = ATOM_MaterialParam::ParamType_Matrix44;
};

template <>
struct ATOM_MaterialParamTypeT<ATOM_MaterialMatrix44ArrayParam>
{
	static const int type = ATOM_MaterialParam::ParamType_Matrix44Array;
};

template <>
struct ATOM_MaterialParamTypeT<ATOM_MaterialMatrix43Param>
{
	static const int type = ATOM_MaterialParam::ParamType_Matrix43;
};

template <>
struct ATOM_MaterialParamTypeT<ATOM_MaterialMatrix43ArrayParam>
{
	static const int type = ATOM_MaterialParam::ParamType_Matrix43Array;
};

template <>
struct ATOM_MaterialParamTypeT<ATOM_MaterialVectorParam>
{
	static const int type = ATOM_MaterialParam::ParamType_Vector;
};

template <>
struct ATOM_MaterialParamTypeT<ATOM_MaterialVectorArrayParam>
{
	static const int type = ATOM_MaterialParam::ParamType_VectorArray;
};

template <>
struct ATOM_MaterialParamTypeT<ATOM_MaterialTextureParam>
{
	static const int type = ATOM_MaterialParam::ParamType_Texture;
};

template <>
struct ATOM_MaterialParamTypeT<ATOM_MaterialSamplerParam>
{
	static const int type = ATOM_MaterialParam::ParamType_Sampler;
};

template <>
struct ATOM_MaterialParamTypeT<ATOM_MaterialIntArrayParam>
{
	static const int type = ATOM_MaterialParam::ParamType_IntArray;
};

template <>
struct ATOM_MaterialParamTypeT<ATOM_MaterialParam>
{
	static const int type = ATOM_MaterialParam::ParamType_Unknown;
};

template <class T>
T *ATOM_MaterialParamCast (ATOM_MaterialParam *param)
{
	if (param)
	{
		return (ATOM_MaterialParamTypeT<T>::type == param->getParamType()) ? (T*)param : 0;
	}
	else
	{
		return 0;
	}
}

class ATOM_EffectParameterValue
{
public:
	ATOM_MaterialParam *handle;
	unsigned dimension;
	ATOM_STRING parameterName;
	ATOM_STRING textureFileName;
	//--- wangjian added ---//
	unsigned isValidationFlag;
	unsigned validationBitIndex;	// 该PARAM在整个COREEFFECT中的validation的ID号
	//----------------------//

	union
	{
		float *f;
		int *i;
		ATOM_Vector4f *v;
		ATOM_Matrix4x4f *m44;
		ATOM_Matrix3x4f *m43;
		ATOM_Texture *t;
		void *p;
	};

	ATOM_EffectParameterValue (const char *name, ATOM_MaterialParam *param);
	ATOM_EffectParameterValue (const ATOM_EffectParameterValue &other);
	~ATOM_EffectParameterValue (void);
	void swap (ATOM_EffectParameterValue &other);
	ATOM_EffectParameterValue &operator = (const ATOM_EffectParameterValue &other);
	bool isSameValueAs (const ATOM_EffectParameterValue &other) const;
	bool isAutoParameter (void) const;
	const char *getParameterName (void) const;
	bool apply (ATOM_CoreMaterial *material);
	void resize (ATOM_MaterialParam::Type paramType, unsigned arrayCount);

	void setFloat (float value);
	void setFloatArray (const float *values, unsigned count);
	void setInt (int value);
	void setIntArray (const int *values, unsigned count);
	void setVector (const ATOM_Vector4f &value);
	void setVector (const ATOM_Vector3f &value);
	void setVector (const ATOM_Vector2f &value);
	void setVectorArray (const ATOM_Vector4f *values, unsigned count);
	void setVectorArray (const ATOM_Vector3f *values, unsigned count);
	void setVectorArray (const ATOM_Vector2f *values, unsigned count);
	void setMatrix44 (const ATOM_Matrix4x4f &value);
	void setMatrix44Array (const ATOM_Matrix4x4f *values, unsigned count);
	void setMatrix43 (const ATOM_Matrix3x4f &value);
	void setMatrix43Array (const ATOM_Matrix3x4f *values, unsigned count);
	void setTexture (ATOM_Texture *value);
	//--- wangjian modified ---//
	// 异步加载：添加了一个加载优先级（默认为异步）
	void setTexture (const char *filename, int texLoadPriority = ATOM_LoadPriority_ASYNCBASE);
	//-------------------------//
	
};


#endif // __ATOM3D_RENDER_PARAMETER_H
