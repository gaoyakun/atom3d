#ifndef __ATOM3D_ENGINE_DEFERREDRENDERSCHEME_H
#define __ATOM3D_ENGINE_DEFERREDRENDERSCHEME_H

#include "renderscheme.h"

class HDR_Pipeline;
class ATOM_DeferredCullVisitor;
class ATOM_DeferredShadowMapRenderScheme;

class ATOM_CustomRenderScheme;
class ATOM_CustomRenderSchemeFactory;

class ATOM_EffectParameterValue;

class ATOM_ENGINE_API ATOM_DeferredRenderScheme: public ATOM_RenderScheme
{
public:

	// wangjian modified 
	enum eDeferredRenderLayer
	{
		RenderScheme_GbufferConstruction = 0,
		RenderScheme_LightAccumulation,

		RenderScheme_Shading,
		RenderScheme_PostShading,

		//--- wangjian added ---//
		RenderScheme_PostShadingCustom,		// 用户自定义	
		RenderScheme_SolidForward,			// 水体		
		//----------------------//

		RenderScheme_Forward,
		RenderScheme_PostForward,

		RenderScheme_FinalComposition,
		RenderScheme_PostComposition,
		RenderScheme_HUD,
		RenderScheme_DeferredSchemeCount
	};

	///////////////////////////////////////////////////////
	// wangjian added 
	enum
	{
		DSF_NONE = 0,
		DSF_NOMAINSCHEME = 1 << 0,			// 该scheme非主scheme
		DSF_NOSHADOW = 1 << 1,				// 该scheme无阴影
	};
	//////////////////////////////////////////////////////

public:
	ATOM_DeferredRenderScheme (ATOM_RenderSchemeFactory *factory);
	virtual ~ATOM_DeferredRenderScheme (void);

public:
	virtual const char *getName (void) const;
	virtual bool init (ATOM_RenderDevice *device, unsigned width, unsigned height);
	virtual bool fini (ATOM_RenderDevice *device);
	virtual unsigned getWidth () const;
	virtual unsigned getHeight () const;
	virtual void setHDRBrightPassThreshold (float value);
	virtual float getHDRBrightPassThreshold (void) const;
	virtual void setHDRBrightPassOffset (float value);
	virtual float getHDRBrightPassOffset (void) const;
	virtual void setHDRMiddleGray (float value);
	virtual float getHDRMiddleGray (void) const;
	//--- wangjian added ---//
	virtual void setHDREnable(bool enable);
	virtual bool getHDREnable(void) const;
	void setHDRFilmCurveParams(float filmCurveShoulder,float filmCurveMiddleTone,float filmCurveToe, float filmCurveWhitePoint);
	void getHDRFilmCurveParams(float & filmCurveShoulder,float & filmCurveMiddleTone,float & filmCurveToe, float & filmCurveWhitePoint) const;
	void setHDRColorParams(float HDRSaturate,float HDRContrast,ATOM_Vector3f HDRColorBalance);
	void getHDRColorParams(float & HDRSaturate,float & HDRContrast,ATOM_Vector3f & HDRColorBalance) const;
	void setSchemeFlag(unsigned flag);
	unsigned getSchemeFlag(void) const;
	//----------------------//

public:
	void setDrawGbufferOnly (bool b);
	void addLight (ATOM_Light *light);
	void setSunLight (ATOM_Light *light);
	void setCullVisitor (ATOM_DeferredCullVisitor *cullVisitor);
	ATOM_DepthBuffer *getDepthBuffer (void) const;
	ATOM_Texture *getDepthTexture (void) const;
	ATOM_Texture *getAlbedoTexture (void) const;
	ATOM_DeferredShadowMapRenderScheme *getShadowMapRenderScheme (void) const;
	void setTransparent (bool transparent);
	bool isTransparent (void) const;
	//--- wangjian added ---//
	void setTransMaskTexture(const char* file);
	void flushRenderQueue();
	//----------------------//
	void setDrawAmbientOnly (bool b);
	bool isDrawAmbientOnly (void) const;
	const int	getNumDirectionalLight() const;
	const int	getNumPointLight() const;

protected:
	virtual bool onRender (ATOM_RenderDevice *device, ATOM_Scene *scene);
	virtual int onRenderPass (ATOM_RenderDevice *device, ATOM_Scene *scene, int layer);
	virtual void onResize (ATOM_RenderDevice *device, unsigned width, unsigned height);

private:
	bool drawToGbuffer (ATOM_RenderDevice *device, ATOM_Camera *camera);
	bool drawLightAcc (ATOM_RenderDevice *device, ATOM_Camera *camera);
	bool drawShading (ATOM_RenderDevice *device, ATOM_Camera *camera);
	bool drawPostShading (ATOM_RenderDevice *device, ATOM_Camera *camera);

	//--- wangjian added ---//
	bool drawPostShadingCustom (ATOM_RenderDevice *device, ATOM_Camera *camera);
	bool drawSolidForward (ATOM_RenderDevice *device, ATOM_Camera *camera);
	//----------------------//

	bool drawForward (ATOM_RenderDevice *device, ATOM_Camera *camera);
	bool drawPostComposition (ATOM_RenderDevice *device, ATOM_Camera *camera);
	bool drawComposition (ATOM_RenderDevice *device, ATOM_Camera *camera);
	bool drawShadowMap (ATOM_RenderDevice *device, ATOM_Camera *camera);

	//--- wangjian modified ---//
	// 将光源作为智能指针对象
#if 0
	void drawOmniLights (ATOM_RenderDevice *device, ATOM_Texture *depthTexture, ATOM_Texture *normalTexture, ATOM_Light **lights, unsigned numLights, ATOM_Camera *camera);
	void drawDirectionalLights (ATOM_RenderDevice *device, ATOM_Texture *depthTexture, ATOM_Texture *normalTexture, ATOM_Light **lights, unsigned numLights, ATOM_Camera *camera);
#else
	void drawOmniLights (ATOM_RenderDevice *device, ATOM_Camera *camera);
	void drawDirectionalLights (ATOM_RenderDevice *device, ATOM_Camera *camera);
	void drawComplementLight (ATOM_RenderDevice *device, ATOM_Camera *camera);
	void copySceneTexture(ATOM_RenderDevice *device, ATOM_Camera *camera);
#endif
	//-----------------------//

	void drawHUD (ATOM_RenderDevice *device, ATOM_Camera *camera);

	//--- wangjian added ---//
	void drawGammaCorrect(ATOM_RenderDevice * device, ATOM_Camera *camera);
	//----------------------//

private:
	static void depthTexture_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);
	static void normalTexture_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);
	static void albedoTexture_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);
	//static void auxTexture0_Callback(ATOM_MaterialParam *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);
	static void lightBuffer_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);
	static void sunShadowTexture_Callback (ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);
	static void composTexture_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);
	static void shadedTexture_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);
	static void linearizeDepthParam_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);
	static void sizeParam_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);
	static void viewAspect_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);
	static void invTanHalfFov_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);
	static void lightColor_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);
	static void lightDir_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);
	static void ambient_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);
	static void sunLightColor_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);
	static void sunLightDir_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);
	//--- wangjian added ---//
	static void invScreenSize_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);
	static void useHDR_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);
	static void shadedSceneTexture_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);
	static void waterCausticsTexture_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);
	static void screenCoordScaleBias_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);
	//static void useSSS_Callback(ATOM_MaterialParam *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);
	static void SSSBRDFTexture_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);
	static void ssaoTexture_Callback (ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);
	//----------------------//

public:
	static void registerTextureCallbacks (void);
	static void unregisterTextureCallbacks (void);

protected:
	/////////////////////////////////////////////
	// wangjian test share rt
#if 0
	ATOM_AUTOREF(ATOM_Texture)			_depthTexture;
	ATOM_AUTOREF(ATOM_Texture)			_normalTexture;
	ATOM_AUTOREF(ATOM_Texture)			_albedoTexture;
	ATOM_AUTOREF(ATOM_Texture)			_lightAccTexture;
	ATOM_AUTOREF(ATOM_Texture)			_shadedTexture;
	ATOM_AUTOREF(ATOM_Texture)			_ldrResult[2];
	ATOM_AUTOPTR(ATOM_Material)			_deferredMaterial;
#else
	static ATOM_AUTOREF(ATOM_Texture)	_depthTexture;
	static ATOM_AUTOREF(ATOM_Texture)	_normalTexture;
	static ATOM_AUTOREF(ATOM_Texture)	_albedoTexture;
	static ATOM_AUTOREF(ATOM_Texture)	_lightAccTexture;
	static ATOM_AUTOREF(ATOM_Texture)	_shadedTexture;
	static ATOM_AUTOREF(ATOM_Texture)	_ldrResult[2];
	static ATOM_AUTOPTR(ATOM_Material)	_deferredMaterial;

	unsigned							_deferredSchemeFlag;
	ATOM_AUTOREF(ATOM_Texture)			_transparentMaskTexture;
	ATOM_STRING							_transparentMaskTextureFile;
#endif

private:
	ATOM_RenderQueue *_renderQueue[RenderScheme_DeferredSchemeCount];
	unsigned _currentViewportWidth;
	unsigned _currentViewportHeight;

	// wangjian test share rt
#if 0
	bool _isOk;
#else
	static bool _isOk;
#endif
	/////////////////////////////////////////////

	bool _isTransparent;
	bool _drawGbufferOnly;
	bool _ambientOnly;
	/////////////////////////////////////////////
	
	

	//--- wangjian modified ---//
	// 将光源作为智能指针对象
#if 0
	ATOM_VECTOR<ATOM_Light*> _omniLights;
	ATOM_VECTOR<ATOM_Light*> _directionalLights;
	ATOM_VECTOR<ATOM_Light*> _spotLights;
	ATOM_Light *_sunLight;
#else
	ATOM_VECTOR<ATOM_AUTOPTR(ATOM_Light)>	_omniLights;
	ATOM_VECTOR<ATOM_AUTOPTR(ATOM_Light)>	_directionalLights;
	ATOM_VECTOR<ATOM_AUTOPTR(ATOM_Light)>	_spotLights;
	ATOM_AUTOPTR(ATOM_Light)				_sunLight;
#endif
	//----------------------//

#if 0
	ATOM_VertexDecl							_vertexDeclOmni;
	ATOM_AUTOREF(ATOM_VertexArray)			_vertexArrayOmni;
	ATOM_AUTOREF(ATOM_IndexArray)			_indexArrayOmni;
#else
	static ATOM_VertexDecl					_vertexDeclOmni;
	static ATOM_AUTOREF(ATOM_VertexArray)	_vertexArrayOmni;
	static ATOM_VertexDecl					_vertexDeclOmni_V2;		// wangjian added : new version of omni light[]
	static ATOM_AUTOREF(ATOM_VertexArray)	_vertexArrayOmni_V2;
	static ATOM_AUTOREF(ATOM_IndexArray)	_indexArrayOmni;
#endif

	ATOM_Vector4f							_currentLightColor;
	ATOM_Vector4f							_currentLightDir;

	ATOM_DeferredShadowMapRenderScheme *	_shadowMapRenderScheme;
	HDR_Pipeline *							_HDRPipeline;

	ATOM_DeferredCullVisitor *				_cullVisitor;

	// wangjian added
	ATOM_AUTOPTR(ATOM_Material)				_mat_gammacorrect;

	/*ATOM_CustomRenderSchemeFactory * _customRenderSchemeFactory;
	ATOM_CustomRenderScheme * _customRenderScheme;
	void TestCustomRenderScheme(ATOM_RenderDevice *device, ATOM_Scene *scene);
	ATOM_DeferredScene *		test_scene;
	ATOM_AUTOREF(ATOM_Node)		test_parent;
	ATOM_AUTOREF(ATOM_Node)		test_role;
	ATOM_Matrix4x4f test_origin;
	ATOM_Matrix4x4f test_camera;*/

public:
	void TestCustomRenderScheme(ATOM_RenderDevice *device, ATOM_Scene *scene);

	// wangjian added 
	// 清除掉所有的延迟渲染模式资源（生成的所有RT资源)
	static void DestoryAllDSResources();
	//------------------/
};

class ATOM_DeferredRenderSchemeFactory: public ATOM_RenderSchemeFactory
{
public:
	ATOM_DeferredRenderSchemeFactory (void);
	virtual ~ATOM_DeferredRenderSchemeFactory (void);
	virtual ATOM_RenderScheme *create (void);
	virtual void destroy (ATOM_RenderScheme *renderScheme);
	virtual int getLayerNumber (const char *layerName) const;
	virtual const char *getLayerName (int layerNumber) const;
	//--- wangjian added ---//
	virtual unsigned getLayerCount() const;
	virtual unsigned getLayerMaskId( int layer ) const;
	//----------------------//
};

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

class ATOM_ENGINE_API ATOM_CustomRenderScheme: public ATOM_RenderScheme
{
public:

#if 0
	static const int RenderScheme_Custom = 0;
	static const int RenderScheme_CustomSchemeCount = 1;
#else
	enum eCustomRenderLayer
	{
		RenderScheme_Solid = 0,
		RenderScheme_PostSolid,
		RenderScheme_SolidTransparent,
		RenderScheme_Transparent,
		RenderScheme_HUD,

		RenderScheme_CustomSchemeCount
	};
#endif

	static int bRender;
	static ATOM_AUTOREF(ATOM_Texture)		_ldrResult[2];

public:
	ATOM_CustomRenderScheme (ATOM_RenderSchemeFactory *factory);
	virtual ~ATOM_CustomRenderScheme (void);

	void setCustomVisitor(ATOM_Visitor * visitor);

public:
	static void registerTextureCallbacks (void);
	static void unregisterTextureCallbacks (void);

public:
	virtual const char *getName (void) const;
	virtual bool init (ATOM_RenderDevice *device, unsigned width, unsigned height);
	virtual bool fini (ATOM_RenderDevice *device);
	virtual unsigned getWidth () const;
	virtual unsigned getHeight () const;

	void addLight (ATOM_Light *light);

	void setTransparent (bool transparent);
	bool isTransparent (void) const;
	
protected:
	virtual bool onRender (ATOM_RenderDevice *device, ATOM_Scene *scene);
	virtual int onRenderPass (ATOM_RenderDevice *device, ATOM_Scene *scene, int layer);
	virtual void onResize (ATOM_RenderDevice *device, unsigned width, unsigned height);

protected:
	virtual bool draw (ATOM_RenderDevice *device, ATOM_Camera *camera);

	static void custom_lightColor_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);
	static void custom_lightPosition_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);
	static void custom_lightAtten_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);
	static void custom_ambient_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);
	static void custom_sunLightColor_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);
	static void custom_sunLightDir_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);
	static void custom_sizeParam_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);
	static void custom_shadedSceneTexture_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);
	static void custom_fogFactor_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);
	static void custom_fogColor_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);

protected:
	ATOM_RenderQueue *						_renderQueue[RenderScheme_CustomSchemeCount];

	unsigned								_currentViewportWidth;
	unsigned								_currentViewportHeight;

	ATOM_Visitor *							_visitor;

	ATOM_VECTOR<ATOM_AUTOPTR(ATOM_Light)>	_omniLights;
	ATOM_VECTOR<ATOM_AUTOPTR(ATOM_Light)>	_directionalLights;
	ATOM_VECTOR<ATOM_AUTOPTR(ATOM_Light)>	_spotLights;

	bool									_isTransparent;
};


class ATOM_CustomRenderSchemeFactory: public ATOM_RenderSchemeFactory
{
public:
	ATOM_CustomRenderSchemeFactory (void);
	virtual ~ATOM_CustomRenderSchemeFactory (void);
	virtual ATOM_RenderScheme *create (void);
	virtual void destroy (ATOM_RenderScheme *renderScheme);
	virtual int getLayerNumber (const char *layerName) const;
	virtual const char *getLayerName (int layerNumber) const;
	virtual unsigned getLayerCount() const;
	virtual unsigned getLayerMaskId( int layer ) const;
};

#endif // __ATOM3D_ENGINE_DEFERREDRENDERSCHEME_H
