#ifndef __ATOM3D_ENGINE_SHADOWMAPRENDERSCHEME_H
#define __ATOM3D_ENGINE_SHADOWMAPRENDERSCHEME_H

#include "renderscheme.h"

#include "../geometry/PolygonBody.h"

class LiSP;

class ATOM_ENGINE_API ATOM_DeferredShadowMapRenderScheme: public ATOM_RenderScheme
{
public:
	static const int RenderScheme_ShadowMap = 0;
	static const int RenderScheme_ShadowMask = 1;
	static const int RenderScheme_ShadowMapSchemeCount = 2;

	static const int RenderMode_ShadowMap = 1;
	static const int RenderMode_ShadowMask = 2;


public:
	ATOM_DeferredShadowMapRenderScheme (ATOM_RenderSchemeFactory *factory);
	virtual ~ATOM_DeferredShadowMapRenderScheme (void);

public:
	virtual const char *getName (void) const;
	virtual bool init (ATOM_RenderDevice *device, unsigned width, unsigned height);
	virtual bool fini (ATOM_RenderDevice *device);
	virtual unsigned getWidth (void) const;
	virtual unsigned getHeight (void) const;
	virtual void setLightDirection (const ATOM_Vector3f &dir);
	virtual ATOM_Texture *getShadowMap (void) const;
	virtual ATOM_Texture *getShadowMask (void) const;
	virtual void setFadeMin (float fadeMin);
	virtual float getFadeMin (void) const;
	virtual void setFadeMax (float fadeMax);
	virtual float getFadeMax (void) const;
	//--- wangjian added ---//
	virtual void	setShadowBlur (float blur);
	virtual float	getShadowBlur (void) const;
	virtual void	setShadowSlopeBias(float bias);
	virtual float	getShadowSlopeBias(void) const;
	virtual void	setShadowDensity (float density);
	virtual float	getShadowDensity (void) const;
	void			generateWhiteShadowMask();
	//----------------------//

protected:
	virtual bool onRender (ATOM_RenderDevice *device, ATOM_Scene *scene);
	virtual int onRenderPass (ATOM_RenderDevice *device, ATOM_Scene *scene, int layer);
	virtual void onResize (ATOM_RenderDevice *device, unsigned width, unsigned height);

private:
	void calculateLightMatrices (const ATOM_Vector3f &lightDir, const ATOM_Vector3f &origin, float radius, ATOM_Matrix4x4f &viewMatrix, ATOM_Matrix4x4f &projMatrix) const;
	//void drawShadowMap (ATOM_RenderDevice *device, ATOM_Camera *camera);
	void drawShadowMap (ATOM_RenderDevice *device, ATOM_Scene *scene, const ATOM_Matrix4x4f &lightView, const ATOM_Matrix4x4f &lightProj);
	void drawShadowMask (ATOM_RenderDevice *device, const ATOM_Matrix4x4f &lightViewProj);
	void generateRotationTexture (int size);

public:
	void setShadowDistance (float distance);
	float getShadowDistance (void) const;
	void setLightDistance (float lightDistance);
	float getLightDistance (void) const;
	void setSceneScale (float value);
	float getSceneScale (void) const;
	void setLightSize (float value);
	float getLightSize (void) const;
	void setDepthBias (float value);
	float getDepthBias (void) const;
	void setRenderMode (int renderMode);



private:
	ATOM_RenderQueue *_renderQueue[RenderScheme_ShadowMapSchemeCount];
	unsigned _currentViewportWidth;
	unsigned _currentViewportHeight;
	bool _isOk;
	ATOM_AUTOREF(ATOM_Texture) _shadowMap;

	//--- wangjian added ---//
	// * 如果设备支持NULL RENDER TARGET的能力，那么可以使用NRT将shadowmap的color rt以节省资源 
	//   参见 “Advanced DX9 Capabilities for ATI Radeon Cards”其介绍以及
	//	 “http://aras-p.info/texts/D3D9GPUHacks.html”参考支持该能力的设备*/
	ATOM_AUTOREF(ATOM_RenderTargetSurface) _shadowMap_dummy;
	//----------------------//

	ATOM_AUTOREF(ATOM_Texture) _shadowDepthBuffer;
	ATOM_AUTOREF(ATOM_Texture) _shadowMask;

	//--- wangjian added ---//
	// 用于temporal coherence，减少阴影边缘的swim现象，暂时未使用。
	//ATOM_AUTOREF(ATOM_Texture) _shadowMaskAccumulate;
	//----------------------//

	ATOM_AUTOREF(ATOM_Texture) _rotationTexture;
	ATOM_AUTOPTR(ATOM_Material) _shadowMaskMaterial;
	float _shadowDistance;
	float _lightDistance;
	float _sceneScale;
	float _lightSize;
	float _depthBias;
	float _fadeMin;
	float _fadeMax;
	//--- wangjian added ---//
	float _shadowBlur;
	float _shadowSlopeBias;
	float _shadowDensity;
	//----------------------//
	int _renderMode;
	ATOM_Matrix4x4f _lightViewProj;
	ATOM_Vector3f _currentLightDir;

	//=== wangjian added ===//
	//* 增加LISPSM的支持 *//
	int				_shadowMapWidth;
	int				_shadowMapHeight;
	ATOM_Matrix4x4f _lightViewMatOrig;
	ATOM_Matrix4x4f _lightProjMatOrig;
	ATOM_Matrix4x4f _lightViewMatrix;
	ATOM_Matrix4x4f _lightProjMatrix;

	//ATOM_Matrix4x4f _lightViewMatrix_Cull;			//for grass cull
	//ATOM_Matrix4x4f _lightProjMatrix_Cull;			//for grass cull

	ATOM_Vector3f	_prevLightDir;
	ATOM_Vector3f	_prevViewPosition;
	ATOM_Vector2f	_depthRange;
	LiSP*			_liSP;
	bool			_useLiSP;
	bool			_lightDirInit;
	PolygonBody		_intersectionBody;
	

	void UpdateLightViewProj(	const ATOM_Vector3f & lightPos,
								const ATOM_BBox & sceneBBox	);
	void CalcPolygonBodyBDirectional(	const ATOM_BBox& sBB	);
	void UpdateDirLightViewProj(	const ATOM_Scene *scene	);
	void CalcLightViewProjMat();
	//======================//
};

class ATOM_ShadowMapRenderSchemeFactory: public ATOM_RenderSchemeFactory
{
public:
	ATOM_ShadowMapRenderSchemeFactory (void);
	virtual ~ATOM_ShadowMapRenderSchemeFactory (void);
	virtual ATOM_RenderScheme *create (void);
	virtual void destroy (ATOM_RenderScheme *renderScheme);
	virtual int getLayerNumber (const char *layerName) const;
	virtual const char *getLayerName (int layerNumber) const;
	//--- wangjian added ---//
	virtual unsigned getLayerCount() const;
	virtual unsigned getLayerMaskId( int layer ) const;
	//----------------------//
};

#endif // __ATOM3D_ENGINE_SHADOWMAPRENDERSCHEME_H
