#include "StdAfx.h"
#include "renderscheme.h"
#include "deferredrenderscheme.h"
#include "shadowmaprenderscheme.h"

struct SchemeStruct
{
	ATOM_HASHMAP<ATOM_STRING, ATOM_RenderSchemeFactory*> _schemeFactories;
	ATOM_HASHMAP<ATOM_RenderScheme*, ATOM_STRING> _schemeInstances;

	SchemeStruct (void)
	{
		_schemeFactories["deferred"] = ATOM_NEW(ATOM_DeferredRenderSchemeFactory);
		ATOM_DeferredRenderScheme::registerTextureCallbacks ();

		_schemeFactories["shadowmap"] = ATOM_NEW(ATOM_ShadowMapRenderSchemeFactory);

		//--- wangjian added ---//
		_schemeFactories["custom"] = ATOM_NEW(ATOM_CustomRenderSchemeFactory);
		ATOM_CustomRenderScheme::registerTextureCallbacks ();
		//----------------------//
	}

	~SchemeStruct (void)
	{
		_schemeFactories.clear ();
		ATOM_DeferredRenderScheme::unregisterTextureCallbacks ();
		//--- wangjian added ---//
		ATOM_CustomRenderScheme::unregisterTextureCallbacks ();
		//----------------------//
	}
};

SchemeStruct schemeStruct;
static ATOM_RenderScheme *_currentRenderScheme = 0;
static ATOM_Scene *_currentScene = 0;
static int _currentRenderSchemeLayer = -1;
//--- wangjian added ---//
static bool _forceCull = false;

unsigned ATOM_RenderScheme::_shadowDPCount = 0;
unsigned ATOM_RenderScheme::_shadowBatchCount = 0;

//----------------------//

ATOM_RenderScheme::ATOM_RenderScheme (ATOM_RenderSchemeFactory *factory)
{
	_factory = factory;
	_postEffectChain = 0;
}

ATOM_RenderScheme::~ATOM_RenderScheme (void)
{
}

ATOM_RenderSchemeFactory *ATOM_RenderScheme::getFactory (void) const
{
	return _factory;
}

void ATOM_RenderScheme::resize (ATOM_RenderDevice *device, unsigned width, unsigned height)
{
	onResize (device, width, height);
}

bool ATOM_RenderScheme::render (ATOM_RenderDevice *device, ATOM_Scene *scene)
{
	const ATOM_Rect2Di &viewport = scene->getCamera()->getViewport();
	if (viewport.size.w != getWidth() || viewport.size.h != getHeight())
	{
		resize (device, viewport.size.w, viewport.size.h);
	}

	ATOM_RenderScheme *currentRenderScheme = ATOM_RenderScheme::getCurrentRenderScheme ();
	ATOM_RenderScheme::setCurrentRenderScheme (this);

	ATOM_Scene *currentScene = ATOM_RenderScheme::getCurrentScene ();
	ATOM_RenderScheme::setCurrentScene (scene);

	scene->getCamera()->synchronise (device, NULL);

	ATOM_AUTOREF(ATOM_Texture) oldRenderTarget = device->getRenderTarget (0);
	ATOM_AUTOREF(ATOM_DepthBuffer) oldDepthBuffer = device->getDepthBuffer ();
	ATOM_Rect2Di oldViewport = viewport;
	ATOM_ColorARGB oldClearColor = device->getClearColor (0);

	bool ret = onRender (device, scene);

	device->setRenderTarget (0, oldRenderTarget.get());
	device->setDepthBuffer (oldDepthBuffer.get());
	device->setViewport (NULL, oldViewport);
	device->setClearColor (NULL, oldClearColor);

	ATOM_RenderScheme::setCurrentScene (currentScene);
	ATOM_RenderScheme::setCurrentRenderScheme (currentRenderScheme);

	return ret;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// wangjian modified
#if 0

void ATOM_RenderScheme::drawScreenQuad (ATOM_RenderDevice *device, ATOM_Material *material)
{
	device->pushMatrix (ATOM_MATRIXMODE_WORLD);
	device->pushMatrix (ATOM_MATRIXMODE_VIEW);
	device->pushMatrix (ATOM_MATRIXMODE_PROJECTION);
	device->loadIdentity (ATOM_MATRIXMODE_WORLD);
	device->loadIdentity (ATOM_MATRIXMODE_VIEW);
	device->loadIdentity (ATOM_MATRIXMODE_PROJECTION);

	float deltax = (float)1.f/(float)(getWidth());
	float deltay = (float)1.f/(float)(getHeight());

	float vertices[4 * 5] = {
		-1.f - deltax, -1.f + deltay, 1.f, 0.f, 1.f,
		1.f - deltax, -1.f + deltay, 1.f, 1.f, 1.f,
		1.f - deltax,  1.f + deltay, 1.f, 1.f, 0.f,
		-1.f - deltax,  1.f + deltay, 1.f, 0.f, 0.f
	};

	unsigned short indices[4] = {
		0, 1, 2, 3
	};

	unsigned numPasses = material->begin (device);
	for (unsigned pass = 0; pass < numPasses; ++pass)
	{
		if (material->beginPass (device, pass))
		{
			device->renderInterleavedMemoryStreamsIndexed (ATOM_PRIMITIVE_TRIANGLE_FAN, 4, 4, ATOM_VERTEX_ATTRIB_COORD|ATOM_VERTEX_ATTRIB_TEX1_2, 5 * sizeof(float), vertices, indices);
			material->endPass (device, pass);
		}
	}
	material->end (device);

	device->popMatrix (ATOM_MATRIXMODE_WORLD);
	device->popMatrix (ATOM_MATRIXMODE_VIEW);
	device->popMatrix (ATOM_MATRIXMODE_PROJECTION);
}

#else

void ATOM_RenderScheme::drawScreenQuad (ATOM_RenderDevice *device, ATOM_Material *material,
										float u0 /*= 0.0f*/, float v0/* = 0.0f*/, float u1/* = 1.0f*/, float v1/* = 1.0f*/)
{
	device->pushMatrix (ATOM_MATRIXMODE_WORLD);
	device->pushMatrix (ATOM_MATRIXMODE_VIEW);
	device->pushMatrix (ATOM_MATRIXMODE_PROJECTION);
	device->loadIdentity (ATOM_MATRIXMODE_WORLD);
	device->loadIdentity (ATOM_MATRIXMODE_VIEW);
	device->loadIdentity (ATOM_MATRIXMODE_PROJECTION);

	float deltax = 1.f/(float)getWidth();
	float deltay = 1.f/(float)getHeight();

	float vertices[4 * 5] = {
		-1.f - deltax, -1.f + deltay, 1.f, u0, v1,
		1.f - deltax, -1.f + deltay, 1.f, u1, v1,
		1.f - deltax,  1.f + deltay, 1.f, u1, v0,
		-1.f - deltax,  1.f + deltay, 1.f, u0, v0
	};

	unsigned short indices[4] = {
		0, 1, 2, 3
	};

	unsigned numPasses = material->begin (device);
	for (unsigned pass = 0; pass < numPasses; ++pass)
	{
		if (material->beginPass (device, pass))
		{
			device->renderInterleavedMemoryStreamsIndexed (ATOM_PRIMITIVE_TRIANGLE_FAN, 4, 4, ATOM_VERTEX_ATTRIB_COORD|ATOM_VERTEX_ATTRIB_TEX1_2, 5 * sizeof(float), vertices, indices);
			material->endPass (device, pass);
		}
	}
	material->end (device);

	device->popMatrix (ATOM_MATRIXMODE_WORLD);
	device->popMatrix (ATOM_MATRIXMODE_VIEW);
	device->popMatrix (ATOM_MATRIXMODE_PROJECTION);
}
#endif
///////////////////////////////////////////////////////////////////////////////////////////////
int ATOM_RenderScheme::drawPass (ATOM_RenderDevice *device, ATOM_Scene *scene, int pass)
{
	_currentRenderSchemeLayer = pass;

	int ret = onRenderPass (device, scene, pass);

	_currentRenderSchemeLayer = -1;

	return ret;
}

ATOM_RenderSchemeFactory *ATOM_RenderScheme::getRegisteredFactory (const char *name)
{
	if (name)
	{
		ATOM_HASHMAP<ATOM_STRING, ATOM_RenderSchemeFactory*>::const_iterator it = schemeStruct._schemeFactories.find (name);
		if (it != schemeStruct._schemeFactories.end ())
		{
			return it->second;
		}
	}
	return 0;
}

bool ATOM_RenderScheme::registerRenderScheme (const char *name, ATOM_RenderSchemeFactory *factory)
{
	if (name && factory)
	{
		ATOM_HASHMAP<ATOM_STRING, ATOM_RenderSchemeFactory*>::const_iterator it = schemeStruct._schemeFactories.find (name);
		if (it == schemeStruct._schemeFactories.end ())
		{
			schemeStruct._schemeFactories[name] = factory;
			return true;
		}
	}
	return false;
}

ATOM_RenderScheme *ATOM_RenderScheme::createRenderScheme (const char *name)
{
	if (name)
	{
		ATOM_HASHMAP<ATOM_STRING, ATOM_RenderSchemeFactory*>::const_iterator it = schemeStruct._schemeFactories.find (name);
		if (it != schemeStruct._schemeFactories.end ())
		{
			ATOM_RenderScheme *scheme = it->second->create();
			if (scheme)
			{
				schemeStruct._schemeInstances[scheme] = name;
				return scheme;
			}
		}
	}
	return 0;
}

void ATOM_RenderScheme::destroyRenderScheme (ATOM_RenderScheme *renderScheme)
{
	if (renderScheme)
	{
		ATOM_HASHMAP<ATOM_RenderScheme*, ATOM_STRING>::iterator it = schemeStruct._schemeInstances.find (renderScheme);
		if (it != schemeStruct._schemeInstances.end ())
		{
			ATOM_HASHMAP<ATOM_STRING, ATOM_RenderSchemeFactory*>::const_iterator itFactory = schemeStruct._schemeFactories.find (it->second);
			if (itFactory != schemeStruct._schemeFactories.end ())
			{
				itFactory->second->destroy (renderScheme);
				schemeStruct._schemeInstances.erase (it);
			}
		}
	}
}

int ATOM_RenderScheme::getRenderSchemeLayerNumber (const char *renderSchemeName, const char *renderSchemeLayerName)
{
	if (renderSchemeName && renderSchemeLayerName)
	{
		ATOM_HASHMAP<ATOM_STRING, ATOM_RenderSchemeFactory*>::const_iterator it = schemeStruct._schemeFactories.find (renderSchemeName);
		if (it != schemeStruct._schemeFactories.end ())
		{
			return it->second->getLayerNumber (renderSchemeLayerName);
		}
	}
	return -1;
}

const char *ATOM_RenderScheme::getRenderSchemeLayerName (const char *renderSchemeName, int renderSchemeLayerNumber)
{
	if (renderSchemeName)
	{
		ATOM_HASHMAP<ATOM_STRING, ATOM_RenderSchemeFactory*>::const_iterator it = schemeStruct._schemeFactories.find (renderSchemeName);
		if (it != schemeStruct._schemeFactories.end ())
		{
			return it->second->getLayerName (renderSchemeLayerNumber);
		}
	}
	return 0;
}

ATOM_RenderScheme *ATOM_RenderScheme::getCurrentRenderScheme (void)
{
	return _currentRenderScheme;
}

void ATOM_RenderScheme::setCurrentRenderScheme (ATOM_RenderScheme *renderScheme)
{
	_currentRenderScheme = renderScheme;
}

void ATOM_RenderScheme::setCurrentScene (ATOM_Scene *scene)
{
	_currentScene = scene;
}

ATOM_Scene *ATOM_RenderScheme::getCurrentScene (void)
{
	return _currentScene;
}

int ATOM_RenderScheme::getCurrentRenderSchemeLayer (void)
{
	return _currentRenderSchemeLayer;
}
//--- wangjian added ---//
void ATOM_RenderScheme::setCurrentRenderSchemeLayer(int layer)
{
	_currentRenderSchemeLayer = layer;
}
void ATOM_RenderScheme::setForceCullUpdate(bool bCull)
{
	_forceCull = bCull;
}
bool ATOM_RenderScheme::getForceCullUpdate(void)
{
	return _forceCull;
}
unsigned  ATOM_RenderScheme::getSchemeMaskID(const char* schemeName)
{
	if( !stricmp(schemeName,"shadowmap") )
		return 1<<0;
	if( !stricmp(schemeName,"deferred") )
		return 1<<1;
	
	return 0;
}
unsigned  ATOM_RenderScheme::getSchemeLayerMaskID(ATOM_RenderSchemeFactory* schemeFactory, int layer )
{
	if( schemeFactory && layer >= 0 )
		return schemeFactory->getLayerMaskId(layer);

	return 0;
}
//----------------------//

ATOM_PostEffectChain *ATOM_RenderScheme::getPostEffectChain (void)
{
	return _postEffectChain;
}

void ATOM_RenderScheme::setPostEffectChain (ATOM_PostEffectChain *chain)
{
	_postEffectChain = chain;
}

//////////////////////////////////////////Factory///////////////////////////////////////////

ATOM_RenderSchemeFactory::ATOM_RenderSchemeFactory (void)
{
	_currentLayer = -1;
}

ATOM_RenderSchemeFactory::~ATOM_RenderSchemeFactory (void)
{
}

