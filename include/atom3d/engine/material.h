#ifndef __ATOM3D_RENDER_MATERIAL_H
#define __ATOM3D_RENDER_MATERIAL_H

#include "../ATOM_kernel.h"
#include "../ATOM_render.h"
#include "parameter.h"
#include "material_manager.h"

struct ParamArrayInfoMap;
class ATOM_ParameterTable;

//------- wangjian modified -------//
struct ParamCallbackInfo
{
	ATOM_MaterialParam *param;
	const ATOM_MaterialParam::CallbackInfo *info;
};
//---------------------------------//

class ATOM_ENGINE_API ATOM_MaterialPass: public ATOM_ReferenceObj
{
	friend class ATOM_MaterialEffect;

public:
	ATOM_MaterialPass (ATOM_RenderDevice *device, unsigned numParams);
	~ATOM_MaterialPass (void);

public:
	void setStateSet (ATOM_RenderStateSet *stateSet);
	ATOM_RenderStateSet *getStateSet (void) const;
	void setGPUProgram (ATOM_GPUProgram *gpuProgram);
	ATOM_GPUProgram *getGPUProgram (void) const;
	void setStreamDefine (unsigned streamDefine);
	unsigned getStreamDefine (void) const;
	bool begin (ATOM_RenderDevice *device, ATOM_ParameterTable *paramTable);
	void end (void);
	bool commit (void);

	//--- wangjian added ---//
	void addParamInfo(ATOM_MaterialParam *param,const ATOM_MaterialParam::CallbackInfo *info);
	//----------------------//

	const ATOM_Shader::ParamDesc *getShaderParamDescVS (void) const;
	const ATOM_Shader::ParamDesc *getShaderParamDescPS (void) const;
	unsigned getMinRegisterIndexVS (void) const;
	unsigned getRegisterCountVS (void) const;
	unsigned getMinRegisterIndexPS (void) const;
	unsigned getRegisterCountPS (void) const;
	bool isInBegin (void) const;

private:
	ATOM_AUTOREF(ATOM_RenderStateSet) _stateset;
	ATOM_AUTOREF(ATOM_GPUProgram) _gpuProgram;
	unsigned _streamDefine;
	unsigned _minRegisterIndexVS;
	unsigned _totalRegisterCountVS;
	unsigned _minRegisterIndexPS;
	unsigned _totalRegisterCountPS;
	bool _inBegin;
	
	ATOM_VECTOR<ParamCallbackInfo> _paramInfo;		// pass的参数集合
	ATOM_VECTOR<ATOM_Shader::ParamDesc> _paramDescsVS;
	ATOM_VECTOR<ATOM_Shader::ParamDesc> _paramDescsPS;
};

class ATOM_ENGINE_API ATOM_ParameterTable;
class ATOM_ENGINE_API ATOM_CoreMaterial;
class ATOM_ENGINE_API ATOM_RenderScheme;
class ATOM_ENGINE_API ATOM_RenderSchemeFactory;

class ATOM_ENGINE_API ATOM_MaterialEffect: public ATOM_ReferenceObj
{
	friend class ATOM_ENGINE_API ATOM_CoreMaterial;
	friend class ATOM_ENGINE_API ATOM_RenderSchemeFactory;
	friend class ATOM_ENGINE_API ATOM_ParameterTable;

public:
	enum Compare
	{
		EQ = 0,
		GE,
		GT,
		LE,
		LT,
		NE
	};

	struct RequireEntry
	{
		ATOM_MaterialParam *param;
		void *context;
		Compare compare;
	};

public:
	ATOM_MaterialEffect (const char *name, const char *scheme, int layer, ATOM_CoreMaterial *material);
	~ATOM_MaterialEffect (void);

public:
	unsigned addPass (ATOM_MaterialPass *pass);
	void removePass (unsigned index);
	void removeAllPasses (void);
	unsigned getNumPasses (void) const;
	ATOM_MaterialPass *getPass (unsigned index) const;
	ATOM_CoreMaterial *getMaterial (void) const;
	const char *getName (void) const;
	const char *getRenderScheme (void) const;
	int getRenderSchemeLayerNumber (void) const;
	bool validate (ATOM_RenderScheme *currentRenderScheme, int currentSchemeLayer) const;
	//--- wangjian added ---//
	void enable(bool bEnable);
	bool isEnable(void) const;
	//----------------------//

public:
	unsigned begin (ATOM_RenderDevice *device, ATOM_ParameterTable *paramTable);
	void end (ATOM_RenderDevice *device);
	bool beginPass (ATOM_RenderDevice *device, ATOM_ParameterTable *paramTable, unsigned pass);
	void endPass (ATOM_RenderDevice *device, unsigned pass);

private:
	ATOM_STRING prepareShaderSource (const char *shaderCode, ParamArrayInfoMap *arrayInfoMap) const;
	//void applySamplerIndices (void);
	void linkShaderParameters (void);
	void gatherUsedParams (void);
	const ATOM_VECTOR<RequireEntry> &getRequireEntries (void) const;

private:
	ATOM_CoreMaterial *_material;
	ATOM_VECTOR<ATOM_AUTOPTR(ATOM_MaterialPass)> _passes;
	ATOM_VECTOR<RequireEntry> _requireEntries;
	ATOM_STRING _name;
	ATOM_STRING _scheme;
	int _renderSchemeLayerNumber;
	ATOM_RenderSchemeFactory *_schemeFactory;
	//--- wangjian added ---//
	bool _enabled;
	//----------------------//

	//--- wangjian modified ---//
	/*struct ParamInfo
	{
		ATOM_MaterialParam *param;
		const ATOM_MaterialParam::CallbackInfo *info;
	};*/
	ATOM_VECTOR<ParamCallbackInfo> _paramInfo;			// EFFECT的参数集合
	//-------------------------//
};

//--- wangjian added ---//
//class ATOM_ShaderD3D9;
//class ATOM_AsyncableLoadMaterial : public ATOM_AsyncableLoadObject
//{
//public:
//	struct shaderCreationParam
//	{
//		ATOM_ShaderD3D9 * shader;
//		ATOM_STRING		* shader_code;
//		ATOM_STRING		entryPoint; 
//		int				shaderCompileTarget; 
//		bool			createFromBin;
//		ATOM_File *		binFile;
//	};
//
//	ATOM_AsyncableLoadMaterial();
//	void SetAttribute( ATOM_CoreMaterial * material );
//	//void GetAttribute( int & width, int & height, ATOM_PixelFormat & fmt );
//	virtual bool Load();
//	virtual bool OnloadEnd();
//
//	void AddShaderCreationItem(	ATOM_ShaderD3D9 * shader,
//								ATOM_STRING		* shader_code,
//								ATOM_STRING		entryPoint,
//								int				shaderCompileTarget,
//								bool			createFromBin,
//								ATOM_File *		binFile);
//
//protected:
//	ATOM_VECTOR<shaderCreationParam> shaderCreationParams;
//	
//};
//-------------------------//
class ATOM_ENGINE_API ATOM_CoreMaterial: public ATOM_Object
{
	friend class ATOM_MaterialEffect;
	friend class ATOM_MaterialManager;
	friend class ATOM_ParameterTable;
	friend class ATOM_Material;

	ATOM_CLASS(engine, ATOM_CoreMaterial, ATOM_CoreMaterial)

public:
	typedef ATOM_MaterialParam *ParamHandle;

public:
	ATOM_CoreMaterial (void);
	~ATOM_CoreMaterial (void);

public:
	bool loadXML (ATOM_RenderDevice *device, const char *filename);
	bool loadString (ATOM_RenderDevice *device, const char *str);
	bool loadXMLElement (ATOM_RenderDevice *device, ATOM_TiXmlDocument *doc);
	unsigned getNumEffects (void) const;
	void addEffect (ATOM_MaterialEffect *effect);
	ATOM_MaterialEffect *getEffect (unsigned index) const;
	ATOM_MaterialEffect *getEffectByName (const char *name) const;
	ATOM_MaterialEffect *autoSelectEffect (ATOM_RenderScheme *currentRenderScheme, int currentSchemeLayer);
	bool supportRenderSchemeLayer (ATOM_RenderSchemeFactory *factory, int layer);
	//--- wangjian added ---//
	// 得到该CoreMaterial支持的scheme对应layer的ID数组
	ATOM_DEQUE<int> * getRenderSchemeLayers(ATOM_RenderScheme* renderScheme);
	// 得到该CoreMaterial支持的scheme和layer中的effect id数组
	ATOM_DEQUE<int> * getRenderSchemeLayerEffects(ATOM_RenderScheme* renderScheme,int layer);
	//----------------------//
	bool isAutoSelectEffect (void) const;
	//--- wangjian added ---//
	void setAutoSelectEffect(bool bAuto);
	/*void appendLayerValidationBit(int layer,unsigned bitIndex);
	void appendSchemeLayerEffect( ATOM_RenderSchemeFactory * scheme, int layer, unsigned effectid, ATOM_MaterialEffect * effect );*/
	//----------------------//

public:
	ATOM_MaterialParam *getParameter (const char *name) const;
	void addParameter (const char *name, ATOM_MaterialParam *param);
	ParamHandle getParameterHandle (const char *name) const;
	unsigned getNumParameters (void) const;

public:
	bool setFloat (ParamHandle param, float value);
	float getFloat (ParamHandle param) const;
	bool setFloatArray (ParamHandle param, const float *values, unsigned count);
	const float *getFloatArray (ParamHandle param) const;
	bool setInt (ParamHandle param, int value);
	int getInt (ParamHandle param) const;
	bool setIntArray (ParamHandle param, const int *values, unsigned count);
	const int *getIntArray (ParamHandle param) const;
	bool setVector (ParamHandle param, const ATOM_Vector4f &value);
	ATOM_Vector4f getVector (ParamHandle param) const;
	bool setVectorArray (ParamHandle param, const ATOM_Vector4f *values, unsigned count);
	const ATOM_Vector4f *getVectorArray (ParamHandle param) const;
	bool setMatrix44 (ParamHandle param, const ATOM_Matrix4x4f &value);
	ATOM_Matrix4x4f getMatrix44 (ParamHandle param) const;
	bool setMatrix44Array (ParamHandle param, const ATOM_Matrix4x4f *values, unsigned count);
	const ATOM_Matrix4x4f *getMatrix44Array (ParamHandle param) const;
	bool setMatrix43 (ParamHandle param, const ATOM_Matrix3x4f &value);
	ATOM_Matrix3x4f getMatrix43 (ParamHandle param) const;
	bool setMatrix43Array (ParamHandle param, const ATOM_Matrix3x4f *values, unsigned count);
	const ATOM_Matrix3x4f *getMatrix43Array (ParamHandle param) const;
	bool setTexture (ParamHandle param, ATOM_Texture *texture);
	ATOM_Texture *getTexture (ParamHandle param) const;
	bool setFloat (const char *name, float value);
	float getFloat (const char *name) const;
	bool setFloatArray (const char *name, const float *values, unsigned count);
	const float *getFloatArray (const char *name) const;
	bool setInt (const char *name, int value);
	int getInt (const char *name) const;
	bool setIntArray (const char *name, const int *values, unsigned count);
	const int *getIntArray (const char *name) const;
	bool setVector (const char *name, const ATOM_Vector4f &value);
	ATOM_Vector4f getVector (const char *name) const;
	bool setVectorArray (const char *name, const ATOM_Vector4f *values, unsigned count);
	const ATOM_Vector4f *getVectorArray (const char *name) const;
	bool setMatrix44 (const char *name, const ATOM_Matrix4x4f &value);
	ATOM_Matrix4x4f getMatrix44 (const char *name) const;
	bool setMatrix44Array (const char *name, const ATOM_Matrix4x4f *values, unsigned count);
	const ATOM_Matrix4x4f *getMatrix44Array (const char *name) const;
	bool setTexture (const char *name, ATOM_Texture *texture);
	ATOM_Texture *getTexture (const char *name) const;

public:
	ATOM_ParameterTable *getDefaultParameterTable (void) const;
	unsigned begin (ATOM_RenderDevice *device);
	void end (ATOM_RenderDevice *device);
	bool beginPass (ATOM_RenderDevice *device, unsigned pass);
	void endPass (ATOM_RenderDevice *device, unsigned pass);

private:
	class Include
	{
	public:
		ATOM_TiXmlDocument *doc;
		ATOM_VECTOR<Include*> children;
		~Include (void);
	};

	bool parseXML (ATOM_RenderDevice *device, ATOM_TiXmlElement *element, ParamArrayInfoMap *arrayInfoMap);
	bool loadIncludes_r (Include *includes, ATOM_TiXmlDocument *doc);
	//--- wangjian modified ---//
	bool preprocess_r (Include *includes);
	//bool preprocess_r (ATOM_RenderDevice *device,Include *includes);
	//-------------------------//
	bool parse_r (ATOM_RenderDevice *device, Include *includes, ParamArrayInfoMap *arrayInfoMap);
	Include *parseIncludes (ATOM_TiXmlDocument *doc);
	//--- wangjian modified ---//
	bool parseDefines (ATOM_TiXmlElement *element);
	//bool parseDefines (ATOM_RenderDevice *device,ATOM_TiXmlElement *element);
	//-------------------------//
	bool parseParamAndShaderCode (ATOM_RenderDevice *device, ATOM_TiXmlElement *element, ATOM_STRING &shaderCode, ParamArrayInfoMap *arrayInfoMap);
	bool parseParamElement (ATOM_RenderDevice *device, ATOM_TiXmlElement *element, ParamArrayInfoMap *arrayInfoMap);
	//--- wangjian modified ---//
	bool parseDefineElement (ATOM_TiXmlElement *element);
	//bool parseDefineElement (ATOM_RenderDevice *device, ATOM_TiXmlElement *element);
	//-------------------------//
	bool parseEffectElement (ATOM_RenderDevice *device, ATOM_TiXmlElement *element, const char *shaderCode, ParamArrayInfoMap *arrayInfoMap);
	bool parseCode (ATOM_RenderDevice *device, ATOM_TiXmlElement *element, ATOM_STRING &code) const;
	bool parsePass (ATOM_RenderDevice *device, ATOM_TiXmlElement *element, ATOM_MaterialEffect *effect, const char *shaderCode, ParamArrayInfoMap *arrayInfoMap);
	bool parseRequire (ATOM_RenderDevice *device, ATOM_TiXmlElement *element, ATOM_MaterialEffect *effect) const;
	unsigned parseStreamDefine (ATOM_RenderDevice *device, ATOM_TiXmlElement *element) const;
	bool parseVertexShader (ATOM_RenderDevice *device, ATOM_TiXmlElement *element, ATOM_MaterialPass *pass, const char *shaderCode, ParamArrayInfoMap *arrayInfoMap) const;
	bool parsePixelShader (ATOM_RenderDevice *device, ATOM_TiXmlElement *element, ATOM_MaterialPass *pass, const char *shaderCode, ParamArrayInfoMap *arrayInfoMap) const;
	bool parseDimension (const char *str, unsigned *value) const;
	void setupArrayParams (ParamArrayInfoMap *arrayInfoMap, ATOM_Shader *shader);
	ATOM_MaterialParam::Type parseTypeString (const char *str, ATOM_STRING &arraySizeStr) const;
	//bool parseFloat (const char *str, float &value) const;
	//bool parseInt (const char *str, int &value) const;
	//bool parseFloatArray (const char *str, int desiredDimension, float *value) const;
	//bool parseIntArray (const char *str, int desiredDimension, int *value) const;
	void initParameterTable (ATOM_ParameterTable *paramTable);

private:
	ATOM_VECTOR<ATOM_AUTOPTR(ATOM_MaterialEffect)> _effects;
	ATOM_HASHMAP<ATOM_STRING, ATOM_AUTOPTR(ATOM_MaterialParam)> _params;
	ATOM_HASHMAP<ATOM_STRING, ATOM_STRING> _definitions;
	ATOM_STRING _shaderCode;
	bool _autoSelectEffect;
	ATOM_AUTOPTR(ATOM_ParameterTable) _defParams;

	//--- wangjian added ---//
	/*static */ATOM_File *						_shaderBin;
	/*static */bool								_readBin;

	struct sRenderSchemeLayerEffects
	{
		ATOM_DEQUE<int> _effects;
	};
	struct sRenderSchemeLayers
	{
		ATOM_DEQUE<int> _layers;
		ATOM_MAP<int,sRenderSchemeLayerEffects> _layerEffects;
	};
	typedef ATOM_MAP<ATOM_RenderSchemeFactory *,sRenderSchemeLayers> RENDERSCHEMELAYERS;
	RENDERSCHEMELAYERS	_renderSchemeLayerSupports;

	typedef ATOM_SET<ATOM_STRING> LAYERSET;
	LAYERSET _renderLayerNames;
	///////////////////////////////////////////////////////////////////////////////////////

	/*unsigned int validationBitIndex;
	ATOM_MAP<int,unsigned>	layerValidationBitMask;

	struct sSchemeLayerEffects
	{
		ATOM_MAP<unsigned,ATOM_MaterialEffect*> effects;
	};
	ATOM_MAP<unsigned,sSchemeLayerEffects> schemeLayerEffects;*/

	//----------------------//

public:
	//--- wangjian added ---//
	void openShaderBinFile(const char* filename);
	void closeShaderBinFile(bool hasError=false);
	//----------------------//

private:
	//--- wangjian added ---//
	//ATOM_AUTOPTR(ATOM_AsyncableLoadMaterial) _asyncLoader;
	void saveEffectRenderSchemeLayer(ATOM_MaterialEffect* effect);
	//----------------------//
};

class ATOM_ENGINE_API ATOM_Material: public ATOM_ReferenceObj
{
public:
	ATOM_Material (void);
	ATOM_Material (const ATOM_Material &other);
	virtual ~ATOM_Material (void);

public:
	ATOM_Material &operator = (const ATOM_Material &other);
	ATOM_AUTOPTR(ATOM_Material) clone (void) const;
	ATOM_CoreMaterial *getCoreMaterial (void);
	void setCoreMaterial (ATOM_CoreMaterial *coreMaterial);
	const char *getCoreMaterialFileName (void) const;
	void setCoreMaterialFileName (const char *filename);
	bool isSameAs (ATOM_Material *other) const;
	ATOM_MaterialEffect *getActiveEffect (void);
	void setActiveEffect (const char *name);
	ATOM_ParameterTable *getParameterTable (void);

	//--- wangjian modified ---//
#if 0
	int getValidEffectIndex (ATOM_RenderScheme *scheme, int schemeLayer) const;
	void autoSetActiveEffect (void);
#else 
	void setActiveEffect(int index);
	int getValidEffectIndex (ATOM_RenderScheme *scheme, int schemeLayer, int & debuginfo);
	void autoSetActiveEffect (int queueid=-1);
#endif

	// 设置材质的全局ID号
	void setMaterialId(ATOM_STRING strId);
	unsigned getMaterialId(void) const;
	//-------------------------//

	unsigned begin (ATOM_RenderDevice *device);
	void end (ATOM_RenderDevice *device);
	bool beginPass (ATOM_RenderDevice *device, unsigned pass/*=0*/);	// wangjian modified : add a default value
	void endPass (ATOM_RenderDevice *device, unsigned pass);

	//------- wangjian modified --------//
	// 加载材质参数表时会顺带设置该设置的纹理，此时会有加载纹理的操作，所以添加一个纹理加载优先级的参数
#if 0
	bool loadXML (ATOM_RenderDevice *device, const char *filename);
	bool loadString (ATOM_RenderDevice *device, const char *str);
#else
	bool loadXML (ATOM_RenderDevice *device, const char *filename, int texLoadPriority = ATOM_LoadPriority_ASYNCBASE);
	bool loadString (ATOM_RenderDevice *device, const char *str, int texLoadPriority = ATOM_LoadPriority_ASYNCBASE);
#endif
	//----------------------------------//

	bool saveXML (const char *filename) const;

	ATOM_STRING _coreMaterialFileName;

private:
	ATOM_AUTOREF(ATOM_CoreMaterial) _coreMaterial;
	ATOM_AUTOPTR(ATOM_ParameterTable) _parameterTable;
	ATOM_AUTOPTR(ATOM_MaterialEffect) _activeEffect;
	//ATOM_STRING _coreMaterialFileName;

	//--- wangjian added ---//
	// 缓存使用到的effect
	struct sEffectUse
	{
		int id;
		int enabled;
		ATOM_SET<int> _mru;		// recently used effects 
	};
	typedef ATOM_MAP<UINT64,sEffectUse> EFFECT_MAP;
	EFFECT_MAP _effectInuse;

	// 缓存使用到的layer
	typedef ATOM_SET<UINT64> LAYER_SET;
	LAYER_SET _layerInuse;
		
	// 材质ID
	unsigned _matId;
	//----------------------//
};

#endif // __ATOM3D_RENDER_MATERIAL_H
