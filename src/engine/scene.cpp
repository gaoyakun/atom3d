#include "StdAfx.h"
#include "scene.h"
#include "renderscheme.h"
#include "posteffectchain.h"

static ATOM_Scene *currentScene = 0;
static ATOM_Camera *currentCamera = 0;
unsigned sceneCount = 0;

ATOM_Scene::ATOM_Scene (void)
{
	ATOM_STACK_TRACE(ATOM_Scene::ATOM_Scene);
	sceneCount++;

	_camera = ATOM_NEW(ATOM_Camera);
	_camera->setPerspective (ATOM_Pi/3.f, 1.f, 1.f, 500.f);
	_octreeNode = ATOM_HARDREF(ATOM_NodeOctree)();
	_renderScheme = 0;
	_ambientLight.set(0.3f, 0.3f, 0.3f, 1.f);
	_width = 0;
	_height = 0;
	_postEffectChain = ATOM_NEW(ATOM_PostEffectChain);
	_loadNodeCallback = nullptr;
	_loadNodeUserData = nullptr;
}

ATOM_Scene::~ATOM_Scene (void)
{
	ATOM_STACK_TRACE(ATOM_Scene::~ATOM_Scene);
	sceneCount--;

	ATOM_DELETE(_camera);
	ATOM_DELETE(_postEffectChain);

	_pathes.clear ();
}

ATOM_Node *ATOM_Scene::getRootNode (void) const
{
	ATOM_STACK_TRACE(ATOM_Scene::getRootNode);
	if (!_rootNode)
	{
		_rootNode = ATOM_HARDREF(ATOM_NodeOctree)();
		ATOM_ASSERT(_rootNode);
		_rootNode->setO2T (ATOM_Matrix4x4f::getIdentityMatrix ());
	}

	return _rootNode.get();
}

ATOM_Camera *ATOM_Scene::getCamera (void) const
{
	ATOM_STACK_TRACE(ATOM_Scene::getCamera);
	return _camera;
}

void ATOM_Scene::render (ATOM_RenderDevice *device, bool clear)
{
	if (_renderScheme)
	{
		_renderScheme->setPostEffectChain (getPostEffectChain());
		_renderScheme->render (device, this);
	}
	//ATOM_RenderScheme::
	/*
	ATOM_STACK_TRACE(ATOM_Scene::render);
	unsigned nDrawCalls = device->getNumDrawCalls();
	unsigned nPrimitivesDrawn = device->getNumPrimitivesDrawn();

	ATOM_Scene *oldScene = currentScene;
	currentScene = this;

	if (_rootNode)
	{
		updateScene (device);

		for (unsigned i = 0; i < _renderTasks.size(); ++i)
		{
			_renderTasks[i]->render (this, clear, forceMaterial, layerMask);
		}
		_renderTasks.resize (0);

		bool renderToTexture = false;
		int vx, vy, vw, vh;
		int cx, cy, cw, ch;
		ATOM_AUTOREF(ATOM_Texture) oldRenderTarget;
		ATOM_AUTOREF(ATOM_DepthBuffer) oldRenderDepth;

		if (!ATOM_RenderSettings::isFixedFunction() && _postEffect && device->getCapabilities().texture_depth)
		{
			cx = _camera->getViewport ()[0];
			cy = _camera->getViewport ()[1];
			cw = _camera->getViewport ()[2];
			ch = _camera->getViewport ()[3];
			if (_renderTarget && (_renderTarget->getWidth() != cw || _renderTarget->getHeight() != ch))
			{
				_renderTarget = 0;
				_renderDepth = 0;
			}

			if (!_renderTarget)
			{
				ATOM_PixelFormat colorFormat = ATOM_PIXEL_FORMAT_BGRA8888;
				if (_enableHDR)
				{
					ATOM_PixelFormat hdrFormat = device->getBestHDRFormatRGBA (0);
					if (hdrFormat != ATOM_PIXEL_FORMAT_UNKNOWN)
					{
						colorFormat = hdrFormat;
					}
				}
				
				_renderTarget = device->allocTexture (0, 0, cw, ch, colorFormat, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET|ATOM_Texture::NOCOMPRESS|ATOM_Texture::NOMIPMAP);
			}

			if (!_renderDepth)
			{
				_renderDepth = device->allocDepthBuffer (cw, ch);
			}

			if (_renderTarget && _renderDepth)
			{
				renderToTexture = true;
				oldRenderTarget = device->getRenderTarget(0);
				oldRenderDepth = device->getDepthBuffer();
				device->getViewport (device->getCurrentView(), &vx, &vy, &vw, &vh);
				device->setViewport (device->getCurrentView(), 0, 0, _renderTarget->getWidth(), _renderTarget->getHeight());
				_camera->setViewport (0, 0, _renderTarget->getWidth(), _renderTarget->getHeight());

				device->setRenderTarget (0, _renderTarget.get());
				device->setDepthBuffer (_renderDepth.get());
				device->clear (clear, true, true);
			}
		}
		else
		{
			_renderTarget = 0;
			_renderDepth = 0;
		}

		beginRender (clear);

		renderScene (device, clear, forceMaterial, layerMask);

		endRender ();

		if (renderToTexture)
		{
			_renderTarget = device->getRenderTarget (0);

			_camera->setViewport (cx, cy, cw, ch);
			device->setRenderTarget (0, oldRenderTarget.get());
			device->setDepthBuffer (oldRenderDepth.get());

			if (_postEffect)
			{
				device->setViewport (device->getCurrentView(), cx, cy, cw, ch);
				_postEffect->render (device, _renderTarget.get());
			}

			device->setViewport (device->getCurrentView(), vx, vy, vw, vh);
		}
	}

	currentScene = oldScene;
	_numDrawCalls = device->getNumDrawCalls() - nDrawCalls;
	_numPrimitivesDrawn = device->getNumPrimitivesDrawn() - nPrimitivesDrawn;
	*/
}

bool ATOM_Scene::save (const char *filename, ATOM_Node *)
{
	return false;
}

bool ATOM_Scene::load (const char *filename, ATOM_Node *, ATOM_Scene::LoadCallback, void*)
{
	return false;
}

/*
ATOM_Scene *ATOM_Scene::getCurrentScene (void)
{
	return currentScene;
}

void ATOM_Scene::setCurrentScene (ATOM_Scene *scene)
{
	currentScene = scene;
}
*/

bool ATOM_Scene::resizeView (int w, int h)
{
	/*
	if (w == _width && h == _height)
	{
		return true;
	}

	if (_renderScheme)
	{
		_renderScheme->fini (ATOM_GetRenderDevice());
		ATOM_RenderScheme::destroyRenderScheme (_renderScheme);
		_renderScheme = 0;
	}

	_renderScheme = ATOM_RenderScheme::createRenderScheme (_renderSchemeName.c_str());
	if (!_renderScheme->init (ATOM_GetRenderDevice(), w, h))
	{
		ATOM_RenderScheme::destroyRenderScheme (_renderScheme);
		_renderScheme = 0;

		return false;
	}
	*/
	return true;
}

class SceneBBoxCalcVisitor: public ATOM_Visitor
{
	ATOM_BBox _bbox;
	bool _succ;
	ATOM_Matrix4x4f _viewMatrix;
	ATOM_Matrix4x4f _projMatrix;

public:
	SceneBBoxCalcVisitor (const ATOM_Scene *scene)
	{
		_viewMatrix = scene->getCamera()->getViewMatrix ();
		_projMatrix = scene->getCamera()->getProjectionMatrix ();
	}

	const ATOM_BBox *getBBox (void) const 
	{ 
		return _succ ? &_bbox : 0; 
	}

public:
	virtual void traverse (ATOM_Node &node)
	{
		_bbox.beginExtend ();
		_succ = false;

		ATOM_Visitor::traverse (node);
	}

	virtual void visit (ATOM_Node &node)
	{
	}

	virtual void visit (ATOM_Terrain &node)
	{
		ATOM_BBox bboxW = node.getWorldBoundingbox ();
		_bbox.extend (bboxW.getMin ());
		_bbox.extend (bboxW.getMax ());
		_succ = true;
	}

	virtual void visit (ATOM_Geode &node)
	{
		ATOM_BBox bboxW = node.getWorldBoundingbox ();
		_bbox.extend (bboxW.getMin ());
		_bbox.extend (bboxW.getMax ());
		_succ = true;
	}
};

ATOM_BBox ATOM_Scene::calculateSceneBoundingBox (void) const
{
	ATOM_STACK_TRACE(ATOM_Scene::calculateSceneBoundingBox);
	SceneBBoxCalcVisitor v(this);
	v.traverse (*getRootNode ());
	const ATOM_BBox *bbox = v.getBBox ();

	if (bbox)
	{
		return *bbox;
	}
	else
	{
		ATOM_BBox ret;
		ret.setMin (ATOM_Vector3f(0.f, 0.f, 0.f));
		ret.setMax (ATOM_Vector3f(0.f, 0.f, 0.f));
		return ret;
	}
}

void ATOM_Scene::setRenderScheme (ATOM_RenderScheme *renderScheme)
{
	_renderScheme = renderScheme;
}

ATOM_RenderScheme *ATOM_Scene::getRenderScheme (void) const
{
	return _renderScheme;
}

void ATOM_Scene::setAmbientLight (const ATOM_Vector4f &color)
{
	_ambientLight = color;
}

const ATOM_Vector4f &ATOM_Scene::getAmbientLight (void) const
{
	return _ambientLight;
}

ATOM_PostEffectChain *ATOM_Scene::getPostEffectChain (void) const
{
	return _postEffectChain;
}

void ATOM_Scene::setSkyNode (ATOM_Sky *skyNode)
{
	_skyNode = skyNode;
}

ATOM_Sky *ATOM_Scene::getSkyNode (void) const
{
	return (ATOM_Sky*)_skyNode.get();
}

void ATOM_Scene::setLoadNodeCallback (LoadNodeCallback callback, void *userData)
{
	_loadNodeCallback = callback;
	_loadNodeUserData = userData;
}

const ATOM_Path3D *ATOM_Scene::getPath (const char *name) const
{
	ATOM_HASHMAP<ATOM_STRING, ATOM_Path3D>::const_iterator it = _pathes.find (name);
	return it == _pathes.end() ? nullptr : &it->second;
}

void ATOM_Scene::clearPathes (void)
{
	_pathes.clear ();
}

void ATOM_Scene::addPath (const char *name, const ATOM_Path3D &path)
{
	_pathes[name] = path;
}

const ATOM_HASHMAP<ATOM_STRING, ATOM_Path3D> &ATOM_Scene::getPathes (void) const
{
	return _pathes;
}

