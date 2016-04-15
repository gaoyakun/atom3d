#ifndef __ATOM3D_RENDER_PARAMETERTABLE_H
#define __ATOM3D_RENDER_PARAMETERTABLE_H

#include "../ATOM_dbghlp.h"
#include "../ATOM_math.h"
#include "material.h"

class ATOM_ENGINE_API ATOM_ParameterTable: public ATOM_ReferenceObj
{
	friend class ATOM_CoreMaterial;
	friend class ATOM_MaterialPass;
	friend class ATOM_MaterialEffect;
	typedef bool (*ValidateFunction)(void *context, void *context2);

	struct ValidationCallbackInfo
	{
		ValidateFunction callback;
		void *context;
		void *context2;
	};
	struct EffectValidationInfo
	{
		ATOM_VECTOR<ValidationCallbackInfo> callbacks;
	};

public:
	typedef ATOM_EffectParameterValue *ValueHandle;

	struct ParamFindInfo
	{
		void *handle;
		const char *name;
		ValueHandle value;
	};

public:
	ATOM_ParameterTable (void);
	ATOM_ParameterTable (ATOM_CoreMaterial *material);
	~ATOM_ParameterTable (void);

	ValueHandle getValueHandle (const char *param) const;
	bool hasSameParametersAs (ATOM_ParameterTable *other) const;
	bool setFloat (const char *param, float value);
	bool setFloat (ValueHandle param, float value);
	bool setFloatArray (const char *param, const float *values, unsigned count);
	bool setFloatArray (ValueHandle param, const float *values, unsigned count);
	bool setInt (const char *param, int value);
	bool setInt (ValueHandle param, int value);
	bool setIntArray (const char *param, const int *values, unsigned count);
	bool setIntArray (ValueHandle param, const int *values, unsigned count);
	bool setVector (const char *param, const ATOM_Vector4f &value);
	bool setVector (ValueHandle param, const ATOM_Vector4f &value);
	bool setVector (const char *param, const ATOM_Vector3f &value);
	bool setVector (ValueHandle param, const ATOM_Vector3f &value);
	bool setVector (const char *param, const ATOM_Vector2f &value);
	bool setVector (ValueHandle param, const ATOM_Vector2f &value);
	bool setVectorArray (const char *param, const ATOM_Vector4f *values, unsigned count);
	bool setVectorArray (ValueHandle param, const ATOM_Vector4f *values, unsigned count);
	bool setVectorArray (const char *param, const ATOM_Vector3f *values, unsigned count);
	bool setVectorArray (ValueHandle param, const ATOM_Vector3f *values, unsigned count);
	bool setVectorArray (const char *param, const ATOM_Vector2f *values, unsigned count);
	bool setVectorArray (ValueHandle param, const ATOM_Vector2f *values, unsigned count);
	bool setMatrix44 (const char *param, const ATOM_Matrix4x4f &value);
	bool setMatrix44 (ValueHandle param, const ATOM_Matrix4x4f &value);
	bool setMatrix44Array (const char *param, const ATOM_Matrix4x4f *values, unsigned count);
	bool setMatrix44Array (ValueHandle param, const ATOM_Matrix4x4f *values, unsigned count);
	bool setMatrix43 (const char *param, const ATOM_Matrix3x4f &value);
	bool setMatrix43 (ValueHandle param, const ATOM_Matrix3x4f &value);
	bool setMatrix43Array (const char *param, const ATOM_Matrix3x4f *values, unsigned count);
	bool setMatrix43Array (ValueHandle param, const ATOM_Matrix3x4f *values, unsigned count);
	bool setTexture (const char *param, ATOM_Texture *value);
	bool setTexture (ValueHandle param, ATOM_Texture *value);
	bool setTexture (const char *param, const char *filename);
	bool setTexture (ValueHandle param, const char *filename);
	bool isEffectValid (unsigned index) const;
	unsigned getNumParameters (void) const;
	ATOM_EffectParameterValue *getParameter (int index) const;
	//void writeShaderConstant (ValueHandle param, const void *data, unsigned regCount);
	void writeShaderConstant (ValueHandle param);

	void saveToXML (ATOM_TiXmlElement *xml) const;
	//--- wangjian modified ---//
	// 异步加载：在该方法中，会有加载纹理的操作，所以加了一个纹理加载优先级（默认为异步）
	void loadFromXML (const ATOM_TiXmlElement *xml, int texLoadPriority = ATOM_LoadPriority_ASYNCBASE);
	//-------------------------//
	ATOM_AUTOPTR(ATOM_ParameterTable) clone (void) const;

	//--- wangjian added ---//
	bool isDirty( ATOM_RenderScheme* scheme = 0, int layer = -1 );
	void setDirty( int dirty, ATOM_RenderScheme* scheme = 0, int layer = -1 );
	void resetDirtyFlag();
	bool getParameterFrom( ATOM_ParameterTable* other ) const;
	//----------------------//

private:
	bool addParameter (const char *name, ATOM_CoreMaterial::ParamHandle param);
	void clear (void);
	ValueHandle findValueHandle (ATOM_MaterialParam *param) const;
	void createValidationInfo (EffectValidationInfo *info, ATOM_MaterialEffect *effect) const;
	void updateShaderConstantMirrors (void);
	void commitShaderConstants (ATOM_RenderDevice *device);
	void evalValue (ValueHandle param);

private:
	static bool requireFunc_FloatEQ (void *context, void *context2);
	static bool requireFunc_IntEQ (void *context, void *context2);
	static bool requireFunc_FloatNE (void *context, void *context2);
	static bool requireFunc_IntNE (void *context, void *context2);
	static bool requireFunc_FloatGE (void *context, void *context2);
	static bool requireFunc_IntGE (void *context, void *context2);
	static bool requireFunc_FloatGT (void *context, void *context2);
	static bool requireFunc_IntGT (void *context, void *context2);
	static bool requireFunc_FloatLE (void *context, void *context2);
	static bool requireFunc_IntLE (void *context, void *context2);
	static bool requireFunc_FloatLT (void *context, void *context2);
	static bool requireFunc_IntLT (void *context, void *context2);

private:
	ATOM_CoreMaterial *_coreMaterial;
	ATOM_HASHMAP<ATOM_STRING, int> _paramTable;
	ATOM_VECTOR<EffectValidationInfo> _effectValidationInfos;
	int _modifyStamp;

	ATOM_VECTOR<ATOM_Vector4f> _shaderConstantMirrorVS;
	ATOM_VECTOR<ATOM_Vector4f> _shaderConstantMirrorPS;

	ATOM_MaterialPass *_currentPass;

	//--- wangjian added ---//
	unsigned _dirtyFlag;
	ATOM_MAP<ATOM_RenderSchemeFactory*,ATOM_RenderScheme*> _default_schemes;
	//----------------------//

	bool _constantDirty;
	ATOM_VECTOR<ATOM_EffectParameterValue*> _paramsArray;
};

#endif // __ATOM3D_RENDER_PARAMETERTABLE_H
