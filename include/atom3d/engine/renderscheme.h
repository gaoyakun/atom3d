#ifndef __ATOM3D_ENGINE_RENDERSCHEME_H
#define __ATOM3D_ENGINE_RENDERSCHEME_H

#include "../ATOM_render.h"
#include "basedefs.h"

class ATOM_RenderSchemeFactory;

class ATOM_ENGINE_API ATOM_RenderScheme
{
public:
	ATOM_RenderScheme (ATOM_RenderSchemeFactory *factory);
	virtual ~ATOM_RenderScheme (void);
	virtual const char *getName (void) const = 0;
	virtual bool init (ATOM_RenderDevice *device, unsigned width, unsigned height) = 0;
	virtual unsigned getWidth () const = 0;
	virtual unsigned getHeight () const = 0;
	virtual bool render (ATOM_RenderDevice *device, ATOM_Scene *scene);
	virtual bool fini (ATOM_RenderDevice *device) = 0;
	virtual int drawPass (ATOM_RenderDevice *device, ATOM_Scene *scene, int pass);
	virtual ATOM_PostEffectChain *getPostEffectChain (void);
	virtual void setPostEffectChain (ATOM_PostEffectChain *chain);

public:
	ATOM_RenderSchemeFactory *getFactory (void) const;

	///////////////////////////////////////////////////////////////////////////////////////////////
	// wangjian added
#if 0
	void drawScreenQuad (ATOM_RenderDevice *device, ATOM_Material *material);
#else
	void drawScreenQuad (	ATOM_RenderDevice *device, ATOM_Material *material,
							float u0 = 0.0f, float v0 = 0.0f, float u1 = 1.0f, float v1 = 1.0f );
#endif
	///////////////////////////////////////////////////////////////////////////////////////////////

	void resize (ATOM_RenderDevice *device, unsigned width, unsigned height);

protected:
	virtual bool onRender (ATOM_RenderDevice *device, ATOM_Scene *scene) = 0;
	virtual int onRenderPass (ATOM_RenderDevice *device, ATOM_Scene *scene, int layer) = 0;
	virtual void onResize (ATOM_RenderDevice *device, unsigned width, unsigned height) = 0;

public:
	static bool registerRenderScheme (const char *name, ATOM_RenderSchemeFactory *factory);
	static ATOM_RenderSchemeFactory *getRegisteredFactory (const char *name);
	static ATOM_RenderScheme *createRenderScheme (const char *name);
	static void destroyRenderScheme (ATOM_RenderScheme *renderScheme);
	static int getRenderSchemeLayerNumber (const char *renderSchemeName, const char *renderSchemeLayerName);
	static const char *getRenderSchemeLayerName (const char *renderSchemeName, int renderSchemeLayerNumber);
	static ATOM_RenderScheme *getCurrentRenderScheme (void);
	static void setCurrentRenderScheme (ATOM_RenderScheme *renderScheme);
	static int getCurrentRenderSchemeLayer (void);
	//--- wangjian added ---//
	static void setCurrentRenderSchemeLayer(int layer);
	static void setForceCullUpdate(bool bCull);
	static bool getForceCullUpdate(void);
	static unsigned  getSchemeMaskID(const char* schemeName);
	static unsigned  getSchemeLayerMaskID(ATOM_RenderSchemeFactory* schemeFactory, int layer );
	//----------------------//
	static void setCurrentScene (ATOM_Scene *scene);
	static ATOM_Scene *getCurrentScene (void);

private:
	ATOM_RenderSchemeFactory *_factory;
	ATOM_PostEffectChain *_postEffectChain;

	//--- wangjian added ---//
public:
	static unsigned _shadowDPCount;
	static unsigned _shadowBatchCount;
	//----------------------//
};

class ATOM_RenderSchemeFactory
{
public:
	ATOM_RenderSchemeFactory (void);
	virtual ~ATOM_RenderSchemeFactory (void);
	virtual ATOM_RenderScheme *create (void) = 0;
	virtual void destroy (ATOM_RenderScheme *renderScheme) = 0;
	virtual int getLayerNumber (const char *layerName) const = 0;
	virtual const char *getLayerName (int layerNumber) const = 0;
	//--- wangjian added ---//
	virtual unsigned getLayerCount() const = 0;
	virtual unsigned getLayerMaskId( int layer ) const = 0;
	//----------------------//

private:
	int _currentLayer;
};

#endif // __ATOM3D_ENGINE_RENDERSCHEME_H
