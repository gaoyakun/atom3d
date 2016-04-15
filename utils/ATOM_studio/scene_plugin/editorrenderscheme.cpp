#include "editorrenderscheme.h"
#include "terrain_shadowmap_cullvisitor.h"

EditorRenderScheme::EditorRenderScheme (ATOM_RenderSchemeFactory *factory)
	: ATOM_RenderScheme(factory)
{
	for (unsigned i = 0; i < EditorRenderScheme::RenderScheme_EditorRenderSchemeCount; ++i)
	{
		_renderQueue[i] = ATOM_NEW(ATOM_RenderQueue);
		//--- wangjian added ---//
		_renderQueue[i]->setQueueId(i);
		//----------------------//
	}
	_currentViewportWidth = 0;
	_currentViewportHeight = 0;
	_isOk = false;
	_outputImage = 0;
	_currentLightDir.set(0.f, -1.f, 0.f);
}

EditorRenderScheme::~EditorRenderScheme (void)
{
	for (unsigned i = 0; i < RenderScheme_EditorRenderSchemeCount; ++i)
	{
		ATOM_DELETE(_renderQueue[i]);
	}
}

const char *EditorRenderScheme::getName (void) const
{
	return "editor";
}

bool EditorRenderScheme::init (ATOM_RenderDevice *device, unsigned width, unsigned height)
{
	onResize (device, width, height);

	return true;
}

bool EditorRenderScheme::fini (ATOM_RenderDevice *device)
{
	_uvNormalTexture = 0;

	return true;
}

unsigned EditorRenderScheme::getWidth (void) const
{
	return _currentViewportWidth;
}

unsigned EditorRenderScheme::getHeight (void) const
{
	return _currentViewportHeight;
}

void EditorRenderScheme::setLightDirection (const ATOM_Vector3f &dir)
{
	_currentLightDir = dir;
}

void EditorRenderScheme::setOutputImage (ATOM_BaseImage *image)
{
	_outputImage = image;
}

ATOM_BaseImage *EditorRenderScheme::getOutputImage (void) const
{
	return _outputImage;
}

ATOM_Texture *EditorRenderScheme::getUVNormalTexture (void) const
{
	return _uvNormalTexture.get();
}

bool EditorRenderScheme::onRender (ATOM_RenderDevice *device, ATOM_Scene *scene)
{
	drawPass (device, scene, RenderScheme_TerrainShadowMap);

	return true;
}

int EditorRenderScheme::onRenderPass (ATOM_RenderDevice *device, ATOM_Scene *scene, int layer)
{
	switch (layer)
	{
	case RenderScheme_TerrainShadowMap:
		{
			drawShadowMap (device, scene);
			break;
		}
	default:
		break;
	}
	return -1;
}

void EditorRenderScheme::drawShadowMap (ATOM_RenderDevice *device, ATOM_Scene *scene)
{
	if (_uvNormalTexture)
	{
		device->setRenderTarget (0, _uvNormalTexture.get());
		device->setViewport (NULL, ATOM_Rect2Di(0, 0, _currentViewportWidth, _currentViewportHeight));
		device->setClearColor (NULL, ATOM_ColorARGB(1.f, 0.f, 1.f, 1.f));
		device->clear (true, false, false);

		TerrainShadowMapCullVisitor cullVisitor (this);
		cullVisitor.setCamera (scene->getCamera ());
		cullVisitor.setUpdateVisibleStamp (false);
		cullVisitor.setNumRenderQueues (EditorRenderScheme::RenderScheme_EditorRenderSchemeCount);
		for (unsigned i = 0; i < EditorRenderScheme::RenderScheme_EditorRenderSchemeCount; ++i)
		{
			cullVisitor.setRenderQueue (i, _renderQueue[i]);
		}
		cullVisitor.traverse (*scene->getRootNode ());

		_renderQueue[RenderScheme_TerrainShadowMap]->render (device, scene->getCamera(), ATOM_RenderQueue::SortNone, false);
	}
}

void EditorRenderScheme::onResize (ATOM_RenderDevice *device, unsigned width, unsigned height)
{
	if (width == _currentViewportWidth && height == _currentViewportHeight)
	{
		return;
	}

	_uvNormalTexture = 0;
	_currentViewportWidth = width;
	_currentViewportHeight = height;
	_isOk = false;

	if (width > 0 && height > 0)
	{
		_uvNormalTexture = ATOM_GetRenderDevice ()->allocTexture (0, 0, width, height, ATOM_PIXEL_FORMAT_RGBA32F, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET|ATOM_Texture::NOMIPMAP);
		if (!_uvNormalTexture)
		{
			fini (device);
			return;
		}
		_isOk = true;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EditorRenderSchemeFactory::EditorRenderSchemeFactory (void)
{
}

EditorRenderSchemeFactory::~EditorRenderSchemeFactory (void)
{
}

ATOM_RenderScheme *EditorRenderSchemeFactory::create (void)
{
	return ATOM_NEW(EditorRenderScheme, this);
}

void EditorRenderSchemeFactory::destroy (ATOM_RenderScheme *renderScheme)
{
	ATOM_DELETE(renderScheme);
}

int EditorRenderSchemeFactory::getLayerNumber (const char *layerName) const
{
	if (!layerName)
	{
		return -1;
	}

	if (!stricmp (layerName, "TerrainShadowMapGen"))
	{
		return EditorRenderScheme::RenderScheme_TerrainShadowMap;
	}

	return -1;
}

const char *EditorRenderSchemeFactory::getLayerName (int layerNumber) const
{
	switch (layerNumber)
	{
	case EditorRenderScheme::RenderScheme_TerrainShadowMap:
		return "TerrainShadowMapGen";
	default:
		return 0;
	}
}

//--- wangjian added ---//
unsigned EditorRenderSchemeFactory::getLayerCount() const
{
	return EditorRenderScheme::RenderScheme_EditorRenderSchemeCount;
}
unsigned EditorRenderSchemeFactory::getLayerMaskId( int layer ) const
{
	return 0;
}
//----------------------//

