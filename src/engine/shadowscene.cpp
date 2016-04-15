#include "StdAfx.h"
#include "shadowscene.h"

ATOM_ShadowScene::ATOM_ShadowScene (const ATOM_SDLScene &from)
{
	ATOM_STACK_TRACE(ATOM_ShadowScene::ATOM_ShadowScene);
	_octreeNode = from.getRootNode()->getParent();
	_rootNode = from.getRootNode ();
	_originCamera = from.getCamera ();
}

ATOM_ShadowScene::~ATOM_ShadowScene (void)
{
	ATOM_STACK_TRACE(ATOM_ShadowScene::~ATOM_ShadowScene);
}

void ATOM_ShadowScene::render (ATOM_RenderDevice *device, bool clear)
{
#if 0 // TODO
	ATOM_STACK_TRACE(ATOM_ShadowScene::render);
	ATOM_Scene *scene = ATOM_Scene::getCurrentScene();
	ATOM_Scene::setCurrentScene (this);

	unsigned flags = _cullVisitor->getFlags ();
	_cullVisitor->setFlags (flags | ATOM_CullVisitor::CULL_SHADOW_PASS);
	bool updateVisibleStamp = _cullVisitor->getUpdateVisibleStamp ();
	_cullVisitor->setUpdateVisibleStamp (false);

	ATOM_Scene::renderScene (device, false, 0, ATOM_RenderQueue::SOLID_LAYERS);

	_cullVisitor->setFlags (flags);
	_cullVisitor->setUpdateVisibleStamp (updateVisibleStamp);

	ATOM_Scene::setCurrentScene (scene);
#endif
}

ATOM_Camera *ATOM_ShadowScene::getOriginCamera (void) const
{
	return _originCamera;
}

