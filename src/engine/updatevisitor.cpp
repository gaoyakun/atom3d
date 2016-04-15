#include "StdAfx.h"
#include "updatevisitor.h"
#include "sky.h"
#include "octree.h"
#include "nodeoctree.h"
#include "particlesystem.h"
#include "water.h"
#include "atmosphere.h"
#include "reflectionrendertask.h"

ATOM_UpdateVisitor::ATOM_UpdateVisitor (void)
{
	ATOM_STACK_TRACE(ATOM_UpdateVisitor::ATOM_UpdateVisitor);

	_octree = 0;
	_octreeNode = 0;
	_camera = 0;
}

ATOM_UpdateVisitor::~ATOM_UpdateVisitor (void)
{
	ATOM_STACK_TRACE(ATOM_UpdateVisitor::~ATOM_UpdateVisitor);

	_octree = 0;
	_octreeNode = 0;
	_camera = 0;
}

void ATOM_UpdateVisitor::setOctree (ATOM_Octree *octree)
{
  _octree = octree;
}

ATOM_Octree *ATOM_UpdateVisitor::getOctree (void) const
{
	return _octree;
}

void ATOM_UpdateVisitor::setCamera (ATOM_Camera *camera)
{
	_camera = camera;
}

ATOM_Camera * ATOM_UpdateVisitor::getCamera (void) const
{
	return _camera;
}

void ATOM_UpdateVisitor::onBeginVisitNodeTree (ATOM_Node &node)
{
  if (_camera)
  {
    if (_octree)
    {
      ATOM_SpatialCell::NodeInfo *nodeInfo = node.getCellNodeInfo ();
      if (!nodeInfo || nodeInfo->lastUpdateStamp != node.getChangeStamp ())
      {
        updateNodePlacementOfOctree (node);
      }
    }
  }
}

void ATOM_UpdateVisitor::onEndVisitNodeTree (ATOM_Node &node)
{
	if (_octreeNode == &node)
	{
		_octree = 0;
		_octreeNode = 0;
	}
}

void ATOM_UpdateVisitor::onResetVisitor (void)
{
	_octree = 0;
	_octreeNode = 0;
	_frameStamp = ATOM_APP->getFrameStamp().frameStamp;
}

void ATOM_UpdateVisitor::updateNodePlacementOfOctree (ATOM_Node &node) const
{
	const ATOM_BBox &bbox = node.getWorldBoundingbox ();

	if (node.isGeometryClippingEnabled () && !node.isSkipClipTest())
	{
		ATOM_Vector3f origin = bbox.getCenter();
		float size_x = bbox.getExtents().x;
		float size_y = bbox.getExtents().y;
		float size_z = bbox.getExtents().z;
		float size_max = (size_x > size_y) ? size_x : size_y;
		if (size_max < size_z) size_max = size_z;

		ATOM_SpatialCell::NodeInfo *nodeInfo = node.getCellNodeInfo ();
		ATOM_OctreeNode *n = nodeInfo ? (ATOM_OctreeNode*)nodeInfo->cell : 0;
		ATOM_OctreeNode *octreeNode = _octree->locateNodeChain2 (n, origin, size_max);

		if (!octreeNode)
		{
			octreeNode = _octree->getRootNode();
		}
		octreeNode->attachNode (&node, node.getChangeStamp());
	}
	else
	{
		ATOM_SpatialCell::NodeInfo *nodeInfo = node.getCellNodeInfo ();
		if (!nodeInfo || !nodeInfo->cell)
		{
			_octree->getRootNode()->attachNode (&node, node.getChangeStamp());
		}
	}
}

void ATOM_UpdateVisitor::visit (ATOM_Node &node)
{
	ATOM_STACK_TRACE(ATOM_UpdateVisitor::visit);
}

void ATOM_UpdateVisitor::visit (ATOM_NodeOctree &node)
{
	ATOM_STACK_TRACE(ATOM_UpdateVisitor::visit);

	if (!ATOM_RenderSettings::isOctreeEnabled ())
	{
		visit ((ATOM_Node&)node);
		return;
	}

	if (_octree)
	{
		ATOM_LOGGER::error ("octree scene node cannot have child octree scene node.\n");
		return;
	}

	if (!node.getOctree ())
	{
		ATOM_LOGGER::error ("invalid octree scene node.\n");
		return;
	}

	_octree = node.getOctree ();
	_octreeNode = &node;
}

void ATOM_UpdateVisitor::visit (ATOM_ParticleSystem &node)
{
	ATOM_STACK_TRACE(ATOM_UpdateVisitor::visit);

	unsigned vs = node.getVisibleStamp ();

	if (vs + 10 <= _frameStamp)
	{
		vs = _frameStamp - 1;
		node.setVisibleStamp (vs);
	}

	if (vs == 0 || vs == _frameStamp-1 || 10 <= _frameStamp-vs)
	{
		node.update (_camera->getViewMatrix(), _camera->getProjectionMatrix());
	}
}

void ATOM_UpdateVisitor::visit (ATOM_Terrain &node)
{
	ATOM_STACK_TRACE(ATOM_UpdateVisitor::visit);

	node.update ();
}

void ATOM_UpdateVisitor::visit (ATOM_Hud &node)
{
	ATOM_STACK_TRACE(ATOM_UpdateVisitor::visit);

	if (!node.isScreenSpace ())
	{
		const ATOM_Matrix4x4f &vpMatrix = _camera->getViewProjectionMatrix ();
		node.update (_camera->getViewport(), vpMatrix >> node.getWorldMatrix());
	}
}

void ATOM_UpdateVisitor::visit (ATOM_GuiHud &node)
{
	ATOM_STACK_TRACE(ATOM_UpdateVisitor::visit);

	ATOM_AppIdleEvent idleEvent;
	node.getGuiRenderer()->handleEvent (&idleEvent);

	visit ((ATOM_Hud&)node);
}

void ATOM_UpdateVisitor::visit (ATOM_Water &node)
{
	/*
	ATOM_STACK_TRACE(ATOM_UpdateVisitor::visit);

	if (node.getShow () == ATOM_Node::SHOW && ATOM_RenderSettings::getWaterQuality () == ATOM_RenderSettings::QUALITY_HIGH && ATOM_RenderSettings::getRenderCaps().shaderModel >= 3 && ATOM_RenderSettings::getRenderCaps().supportHDRDeferredShading)
	{
		const ATOM_BBox &bbox = node.getWorldBoundingbox ();
		if (bbox.getClipState (_camera->getViewProjectionMatrix()) != ATOM_BBox::ClipOutside)
		{
			ATOM_ReflectionRenderTask *taskRefl = ATOM_NEW(ATOM_ReflectionRenderTask);
			taskRefl->setReflectionPlane (ATOM_Vector4f (0.f, 1.f, 0.f, -bbox.getCenter().y-0.1f));
			taskRefl->setShape (bbox.getMin().x, bbox.getMax().x, bbox.getCenter().y, bbox.getMin().z, bbox.getMax().z);
			taskRefl->setReflectionTexture (node.getReflectionTexture ());
			taskRefl->setReflectionDepth (node.getReflectionDepth ());
			ATOM_Scene::getCurrentScene()->addRenderTask (taskRefl);
		}
	}
	*/
}

void ATOM_UpdateVisitor::visit (ATOM_Atmosphere &node)
{
	ATOM_STACK_TRACE(ATOM_UpdateVisitor::visit);

#if 0
	if (ATOM_RenderSettings::isAtmosphereEffectEnabled ())
	{
		ATOM_SDLScene *scene = (ATOM_SDLScene*)ATOM_Scene::getCurrentScene();
		node.updateTime (this->getCamera());
	}
#endif
}

void ATOM_UpdateVisitor::visit (ATOM_Atmosphere2 &node)
{
	ATOM_STACK_TRACE(ATOM_UpdateVisitor::visit);

}

void ATOM_UpdateVisitor::visit (ATOM_Sky &node)
{
	node.update (ATOM_GetRenderDevice(), getCamera());
	ATOM_Scene *scene = ATOM_RenderScheme::getCurrentScene();
	ATOM_ASSERT(scene);
	scene->setSkyNode (&node);
}

void ATOM_UpdateVisitor::visit (ATOM_VisualNode &node)
{
	ATOM_STACK_TRACE(ATOM_UpdateVisitor::visit);

	if (node.getLoadInterface()->getLoadingState() == ATOM_LoadInterface::LS_LOADING)
	{
		return;
	}

	node.update (getCamera ());
}

void ATOM_UpdateVisitor::visit (ATOM_CompositionNode &node)
{
	// Don't put into Octree
	ATOM_Octree *octree = _octree;
	_octree = nullptr;

	visit ((ATOM_VisualNode&)node);
	traverse_R (*node.getActorParentNode());

	_octree = octree;
}

void ATOM_UpdateVisitor::visit (ATOM_Geode &node)
{
	// wangjian modified
#if 1
	unsigned vs = node.getVisibleStamp ();
	
	bool bNeedUpdate = false;
	if( vs == 0 )
		bNeedUpdate = true;
	
	if( ATOM_RenderSettings::getSceneDrawItemUpdateFrameCache() >= 2 )
	{
		if( vs >= _frameStamp - ATOM_RenderSettings::getSceneDrawItemUpdateFrameCache() )
			bNeedUpdate = true;
	}
	else if( vs == _frameStamp - 1 )
	{
		bNeedUpdate = true;
	}

	if (vs + 10 <= _frameStamp)
	{
		vs = _frameStamp - 1;
		node.setVisibleStamp (vs);
		bNeedUpdate = true;
	}

	if( bNeedUpdate)
		visit ((ATOM_VisualNode&)node);

#else

	unsigned vs = node.getVisibleStamp ();

	if (vs + 10 <= _frameStamp)
	{
		vs = _frameStamp - 1;
		node.setVisibleStamp (vs);
	}

	if (vs == 0 || vs == _frameStamp-1)
	{
		visit ((ATOM_VisualNode&)node);
	}
#endif
}

void ATOM_UpdateVisitor::visit (ClientSimpleCharacter &node)
{
	visit ((ATOM_Geode&)node);
}

