#include "StdAfx.h"
#include "cullvisitor.h"
#include "nodeoctree.h"
#include "camera.h"
#include "terrain.h"
#include "bkimage.h"
#include "sky.h"
#include "particlesystem.h"
#include "atmosphere2.h"
#include "water.h"
#include "decal.h"
#include "renderscheme.h"

static const char StrBBoxMaterial[] = 
  "material { \n"
  "  effect { \n"
  "   source  \" \n"
  "		float4 color = float4(1.0, 1.0, 1.0, 1.0);	\n"
  "     technique t0 {\n"
  "       pass P0 {\n"
  "         CullMode = None;\n"
  "         ColorOp[0] = SelectArg1;\n"
  "         ColorArg1[0] = TFactor;\n"
  "			TextureFactor = color;\n"
  "			ColorOp[1] = Disable;\n"
  "         VertexShader = null;\n"
  "         PixelShader = null;\n"
  "       }\n"
  "     }\";\n"
  "  }\n"
  "}";

static ATOM_AUTOPTR(ATOM_Material) _bboxMaterial;

class ATOM_BBoxRenderDatas: public ATOM_Drawable
{
	enum
	{
		PXPYPZ = 0,
		PXPYNZ = 1,
		PXNYPZ = 2,
		PXNYNZ = 3,
		NXPYPZ = 4,
		NXPYNZ = 5,
		NXNYPZ = 6,
		NXNYNZ = 7,
	};

public:
	ATOM_BBoxRenderDatas (void)
	{
		ATOM_STACK_TRACE(ATOM_BBoxRenderDatas::ATOM_BBoxRenderDatas);
		setupVertices (ATOM_BBox (ATOM_Vector3f(0,0,0), ATOM_Vector3f(1,1,1)));
	}

	ATOM_BBoxRenderDatas (const ATOM_BBox &bbox)
	{
		ATOM_STACK_TRACE(ATOM_BBoxRenderDatas::ATOM_BBoxRenderDatas);
		setupVertices (bbox);
	}

    virtual bool draw(ATOM_RenderDevice* device, ATOM_Camera *camera, ATOM_Material *material)
	{
		const unsigned short indices [24] = {
			PXPYPZ, NXPYPZ,
			NXPYPZ, NXPYNZ,
			NXPYNZ, PXPYNZ,
			PXPYNZ, PXPYPZ,
			PXNYPZ, NXNYPZ,
			NXNYPZ, NXNYNZ,
			NXNYNZ, PXNYNZ,
			PXNYNZ, PXNYPZ,
			PXPYPZ, PXNYPZ,
			NXPYPZ, NXNYPZ,
			NXPYNZ, NXNYNZ,
			PXPYNZ, PXNYNZ
		};

		unsigned numPasses = material->begin(device);
		for (unsigned pass = 0; pass < numPasses; ++pass)
		{
			if (material->beginPass (device, pass))
			{
				device->renderInterleavedMemoryStreamsIndexed (ATOM_PRIMITIVE_LINES, 24, 8, ATOM_VERTEX_ATTRIB_COORD, sizeof(ATOM_Vector3f), _vertices, indices);
				material->endPass (device, pass);
			}
		}
		material->end (device);

		return true;
	}

	void setupVertices (const ATOM_BBox &bbox)
	{
		ATOM_Vector3f center = bbox.getCenter();
		ATOM_Vector3f extents = bbox.getExtents();
		_vertices[PXPYPZ] = center + extents * ATOM_Vector3f(1, 1, 1);
		_vertices[PXPYNZ] = center + extents * ATOM_Vector3f(1, 1, -1);
		_vertices[PXNYPZ] = center + extents * ATOM_Vector3f(1, -1, 1);
		_vertices[PXNYNZ] = center + extents * ATOM_Vector3f(1, -1, -1);
		_vertices[NXPYPZ] = center + extents * ATOM_Vector3f(-1, 1, 1);
		_vertices[NXPYNZ] = center + extents * ATOM_Vector3f(-1, 1, -1);
		_vertices[NXNYPZ] = center + extents * ATOM_Vector3f(-1, -1, 1);
		_vertices[NXNYNZ] = center + extents * ATOM_Vector3f(-1, -1, -1);
	}

private:
	ATOM_Vector3f _vertices[8];
};

//-------------- wangjian added -------------------//
//int getNodeLoadPriority( const ATOM_Vector3f & nodePosition, ATOM_Camera * camera )
//{
//	if( !camera )
//		return 0;
//
//	int priority = 0;
//
//	const int priority_lowest = 0;
//	const int priority_highest = ATOM_AsyncLoader::getSceneMTLoadPriorityHigh();
//
//	ATOM_Vector3f eye, target, up;
//	camera->getCameraPos(&eye,&target,&up);
//
//	const float distance_near = 90000.0f;
//	const float distance_far = 250000.0f;
//
//	ATOM_Vector3f vec = nodePosition - eye;
//	float dist = vec.getSquaredLength();
//
//	float dist_ratio = 1.0 - ( dist - distance_near ) / ( distance_far - distance_near );
//	if( dist_ratio < 0.0f )
//		dist_ratio = 0.0f;
//	if( dist_ratio > 1.0f )
//		dist_ratio = 1.0f;
//
//	priority = dist_ratio * ( priority_highest - priority_lowest );
//
//	ATOM_Vector3f view(target-eye);
//	view.normalize();
//	vec.normalize();
//	float facing = dotProduct(view,vec);
//	if( facing < 0 )
//		facing = 0.0f;
//	
//	priority = ( priority + facing * ( priority_highest - priority_lowest ) ) * 0.5;
//
//	return priority;
//}

ATOM_CullVisitor::ATOM_CullVisitor (void)
{
	ATOM_STACK_TRACE(ATOM_CullVisitor::ATOM_CullVisitor);

	_camera = 0;
	_device = 0;
	_flags = 0;
	_fogEnabled = false;
	_updateVisibleStamp = true;
	_terrainOccluder = 0;
	_vpMatrix.makeIdentity ();
	_frustum.fromMatrix (_vpMatrix);
	_distanceOverrideValue = 0.f;

	//--- wangjian added ---//
	_enableLoading = true;
	//----------------------//
}

ATOM_CullVisitor::~ATOM_CullVisitor (void)
{
	ATOM_STACK_TRACE(ATOM_CullVisitor::~ATOM_CullVisitor);

	for (unsigned i = 0; i < _bboxRDs.size(); ++i)
	{
		ATOM_DELETE(_bboxRDs[i]);
	}
}

void ATOM_CullVisitor::setFlags (unsigned flags)
{
  _flags = flags;
}

unsigned ATOM_CullVisitor::getFlags (void) const
{
  return _flags;
}

ATOM_RenderQueue *ATOM_CullVisitor::getRenderQueue (unsigned i) const
{
	return _renderQueues[i];
}

void ATOM_CullVisitor::setRenderQueue (unsigned i, ATOM_RenderQueue *renderQueue)
{
	_renderQueues[i] = renderQueue;
}

void ATOM_CullVisitor::setNumRenderQueues (unsigned count)
{
	_renderQueues.resize (count);
}

unsigned ATOM_CullVisitor::getNumRenderQueues (void)
{
	return _renderQueues.size();
}

void ATOM_CullVisitor::addDrawable (ATOM_Drawable *drawable, const ATOM_Matrix4x4f &worldmatrix, ATOM_Material *material, int layer, bool projectable)
{
	if (material)
	{
		
		ATOM_RenderScheme *currentRenderScheme = ATOM_RenderScheme::getCurrentRenderScheme ();

		//--- wangjian modified ---//
#if 0
		for (unsigned i = 0; i < _renderQueues.size(); ++i)
		{
			if (layer >= 0 && i != layer)
			{
				continue;
			}

			if (material->getValidEffectIndex (currentRenderScheme, i) >= 0)
			{
				_renderQueues[i]->addDrawable (drawable, _camera, worldmatrix, material, projectable, _distanceOverrideValue);
			}
		}
#else
		
		// 取代遍历所有的renderscheme layers,而只遍历该材质其core_material支持的对应该currentRenderScheme的所有layers
		ATOM_DEQUE<int> * layers_support = 0;
		if( material->getCoreMaterial() )
			layers_support = material->getCoreMaterial()->getRenderSchemeLayers(currentRenderScheme);
		if( !layers_support )
			return;

		int layer_id;

		// 清空该绘制对象的EFFECT集合 以及 设置必要的材质参数得到正确的effect
		drawable->prepareForSort(currentRenderScheme);

		// 循环该currentRenderScheme的所有的渲染层
		for (unsigned i = 0; i < layers_support->size(); ++i)
		{
			layer_id = (*layers_support)[i];
			if (layer >= 0 && layer != layer_id )
			{
				continue;
			}

			int debuginfo = 0;

			int effect_index = material->getValidEffectIndex (currentRenderScheme, layer_id, debuginfo);
			if (effect_index >= 0)
			{
				// 将EFFECT的ID添加进来
				drawable->appendEffectId(currentRenderScheme,effect_index);

				// 设置排序标记并且添加到渲染队列
				_renderQueues[layer_id]->addDrawable (drawable, _camera, worldmatrix, material, projectable, _distanceOverrideValue);

#if 0
				if( debuginfo )
				{
					ATOM_StaticMesh * MESH = dynamic_cast<ATOM_StaticMesh*>(drawable);
					if( MESH )
						ATOM_LOGGER::debug(" %s : effect is changed ! \n", MESH->getModelFileName().c_str());
				}
#endif

			}

			// 重置该RENDERSCHEME的脏标记
			material->getParameterTable()->setDirty( 0, currentRenderScheme, layer_id );
		}
		
#endif
		//------------------------------------------//
	}
	else if (layer >= 0)
	{
		if (layer < _renderQueues.size())
		{
			_renderQueues[layer]->addDrawable (drawable, _camera, worldmatrix, material, projectable, _distanceOverrideValue);
		}
	}
}

void ATOM_CullVisitor::setCamera (ATOM_Camera *camera)
{
  _camera = camera;
}

ATOM_Camera * ATOM_CullVisitor::getCamera (void) const
{
  return _camera;
}

const ATOM_Matrix4x4f & ATOM_CullVisitor::getViewProjectionMatrix (void) const
{
  return _vpMatrix;
}

void ATOM_CullVisitor::onBeginVisitNodeTree (ATOM_Node &node)
{
}

void ATOM_CullVisitor::onEndVisitNodeTree (ATOM_Node &node)
{
}

void ATOM_CullVisitor::onResetVisitor (void)
{
	ATOM_STACK_TRACE(ATOM_CullVisitor::onResetVisitor);

	for (unsigned i = 0; i < _renderQueues.size(); ++i)
	{
		_renderQueues[i]->clear ();
	}
	_vpMatrix = _camera ? _camera->getProjectionMatrix() >> _camera->getInvViewMatrix() : ATOM_Matrix4x4f::getIdentityMatrix ();
	_frustum.fromMatrix (_vpMatrix);
	if (_terrainOccluder)
	{
	  const ATOM_Matrix4x4f &terrainWorldMatrix = _terrainOccluder->getWorldMatrix ();
	  if (terrainWorldMatrix == ATOM_Matrix4x4f::getIdentityMatrix ())
	  {
		  _invTerrainWorldMatrix.makeIdentity ();
		  _isTerrainOccluderWorldMatrixIdentity = true;
	  }
	  else
	  {
		  _invTerrainWorldMatrix.invertAffineFrom (terrainWorldMatrix);
		  _isTerrainOccluderWorldMatrixIdentity = false;
	  }
	}
	_contribute = ATOM_RenderSettings::getMinScreenContribution () * 4.f;
	_frameStamp = ATOM_APP->getFrameStamp().frameStamp;
}

void ATOM_CullVisitor::traverse (ATOM_Node &node)
{
	ATOM_STACK_TRACE(ATOM_CullVisitor::traverse);

	if (!_bboxRDs.empty())
	{
		unsigned size = _bboxRDsFree.size();
		_bboxRDsFree.resize (size + _bboxRDs.size());
		memcpy (&_bboxRDsFree[size], &_bboxRDs[0], _bboxRDs.size() * sizeof(ATOM_BBoxRenderDatas*));
		_bboxRDs.resize (0);
	}

	ATOM_Visitor::traverse (node);
}

void ATOM_CullVisitor::queueBoundingboxRD (ATOM_Node &node, const ATOM_Matrix4x4f &worldMatrix)
{
	ATOM_STACK_TRACE(ATOM_CullVisitor::queueBoundingboxRD);

	static bool tryLoad = true;
	if (!_bboxMaterial && tryLoad)
	{
		tryLoad = false;
		_bboxMaterial = ATOM_MaterialManager::createMaterialFromCore (ATOM_GetRenderDevice(), "/materials/builtin/bbox.mat");
	}

	if (_bboxMaterial)
	{
		_bboxMaterial->setActiveEffect ("default");

		const ATOM_BBox &bbox = node.getWorldBoundingbox ();
		ATOM_BBoxRenderDatas* brd;
		if (!_bboxRDsFree.empty ())
		{
			brd = _bboxRDsFree.back();
			_bboxRDsFree.pop_back();
		}
		else
		{
			brd = ATOM_NEW(ATOM_BBoxRenderDatas);
		}
		_bboxRDs.push_back (brd);
		brd->setupVertices (bbox);

		addDrawable (brd, ATOM_Matrix4x4f::getIdentityMatrix(), _bboxMaterial.get());
	}
}

bool ATOM_CullVisitor::frustumTest (ATOM_Node *node, const ATOM_BBox &bboxW)
{
	return (_flags & CULL_NO_CLIP_TEST) != 0 || bboxW.getClipState (_frustum) != ATOM_BBox::ClipOutside;
}

bool ATOM_CullVisitor::frustumTestMask (ATOM_Node *node, const ATOM_BBox &bboxW, unsigned mask)
{
	return (_flags & CULL_NO_CLIP_TEST) != 0 || bboxW.getClipStateMask (_frustum, mask) != ATOM_BBox::ClipOutside;
}

void ATOM_CullVisitor::visit (ATOM_Node &node)
{
	ATOM_STACK_TRACE(ATOM_CullVisitor::visit);

	const ATOM_Matrix4x4f &projMatrix = _camera->getProjectionMatrix();
	const ATOM_Matrix4x4f &viewMatrix = _camera->getViewMatrix();
	const ATOM_Matrix4x4f &worldmatrix = node.getWorldMatrix ();

	if ((node.getDrawBoundingbox () || (_flags & CULL_DRAW_BOUNDINGBOX) != 0))
	{
		queueBoundingboxRD (node, worldmatrix);
	}
}

static bool contributeCulling (const ATOM_BBox &bboxW, ATOM_Camera *camera)
{
	ATOM_STACK_TRACE(contributeCulling);

	float minContrib = ATOM_RenderSettings::getMinScreenContribution ();
	if (minContrib > 0.f)
	{
		ATOM_Vector3f vPos = camera->getInvViewMatrix() >> bboxW.getCenter();
		ATOM_Vector3f ext = bboxW.getExtents ();
		float l = ATOM_max2 (ATOM_max2 (ext.x, ext.y), ext.z);
		float cullDistance = l * camera->getInvTanHalfFovy() / ATOM_RenderSettings::getMinScreenContribution ();
		return vPos.getSquaredLength() >= cullDistance * cullDistance;
	}
	return false;
}

void ATOM_CullVisitor::visit (ATOM_NodeOctree &node)
{
	ATOM_STACK_TRACE(ATOM_CullVisitor::visit);

	if (!ATOM_RenderSettings::isOctreeEnabled ())
	{
		visit ((ATOM_Node&)node);
		return;
	}

	if (node.getShow () != ATOM_Node::SHOW || !_camera || !node.getOctree())
	{
		return;
	}

	const ATOM_Matrix4x4f &projectionMatrix = _camera->getProjectionMatrix ();
	const ATOM_Matrix4x4f &viewMatrix = _camera->getViewMatrix ();
	ATOM_Matrix4x4f mvpMatrix = _vpMatrix >> node.getWorldMatrix ();
	ATOM_Frustum3d frustum;
	frustum.fromMatrix (mvpMatrix);
	CullOctree_r (node.getOctree()->getRootNode(), true, true, viewMatrix, projectionMatrix, frustum);

	skipChildren ();
}

void ATOM_CullVisitor::CullOctree_r (ATOM_OctreeNode *node, bool clipped, bool nonclip, const ATOM_Matrix4x4f &viewMatrix, const ATOM_Matrix4x4f &projectionMatrix, const ATOM_Frustum3d &frustum)
{
  if (!node)
  {
    return;
  }

  if (clipped)
  {
    ATOM_BBox nodeBBox(node->getMinPointLoosed(), node->getMaxPointLoosed ());
    ATOM_BBox::ClipState clipState = nodeBBox.getClipState (frustum);
    if (clipState == ATOM_BBox::ClipOutside)
    {
      return;
    }
    else if (clipState == ATOM_BBox::ClipInside)
    {
      clipped = false;
    }
  }

  if (nonclip)
  {
	  clipped = true;
  }

  ATOM_SpatialCell::NodeInfo *nodeInfo = node->getHead();
  while (nodeInfo)
  {
    ATOM_SpatialCell::NodeInfo *next = nodeInfo->next;

    unsigned flags = _flags;
	if (!clipped)
    {
      _flags |= CULL_NO_CLIP_TEST;
	}

    nodeInfo->node->accept (*this);

    _flags = flags;

    nodeInfo = next;
  }

  for (int i = ATOM_OctreeNode::PLACEMENTBEGIN; i != ATOM_OctreeNode::PLACEMENTEND; ++i)
  {
    ATOM_OctreeNode *child = node->getChild ((ATOM_OctreeNode::Placement)i);

    if (child)
    {
      CullOctree_r (child, clipped, false, viewMatrix, projectionMatrix, frustum);
    }
  }
}

void ATOM_CullVisitor::visit (ATOM_Terrain &node)
{
	ATOM_STACK_TRACE(ATOM_CullVisitor::visit);

	if (node.getShow () != ATOM_Node::SHOW || !_camera)
	{
		return;
	}

	//--- wangjian added ---//
	if( ATOM_AsyncLoader::isEnableSceneMTLoad() && !_enableLoading && !node.checkNodeLoadStarted() )
		return;

	// 异步加载 ： 如果节点没有加载完成，则返回
	if( !node.checkNodeAllFinshed() )
	{
		return;
	}
	//----------------------//

	bool skipClipTest = node.isSkipClipTest();
	if (skipClipTest || frustumTest (&node, node.getWorldBoundingbox()))
	{
		//--- wangjian added ---//
		// 异步加载 ：如果节点没有开始加载 则加载开始
		if( _enableLoading && !node.checkNodeLoadStarted() )
		{
			node.setLoadPriority( 400 );
			node.load(ATOM_GetRenderDevice());
			return;
		}
		//----------------------//

		if (_updateVisibleStamp)
		{
			node.setVisibleStamp (_frameStamp);
		}
	
		node.setupRenderQueue (this);
	}
}

void ATOM_CullVisitor::visit (ATOM_Decal &node)
{
#if 0
	return;
#else
	ATOM_STACK_TRACE(ATOM_CullVisitor::visit);

	if (node.getShow () != ATOM_Node::SHOW || !_camera)
	{
		return;
	}

	//--- wangjian added ---//
	if( ATOM_AsyncLoader::isEnableSceneMTLoad() && !_enableLoading && !node.checkNodeLoadStarted() )
		return;

	// 异步加载 ： 如果节点没有加载完成，则返回
	if( !node.checkNodeAllFinshed() )
	{
		return;
	}
	//----------------------//

	const ATOM_Matrix4x4f &projMatrix = _camera->getProjectionMatrix();
	const ATOM_Matrix4x4f &viewMatrix = _camera->getViewMatrix();
	const ATOM_BBox &bbox = node.getWorldBoundingbox ();

	bool skipClipTest = node.isSkipClipTest();
	if (skipClipTest || frustumTestMask (&node, bbox, ATOM_BBox::ClipLeft|ATOM_BBox::ClipRight|ATOM_BBox::ClipNear|ATOM_BBox::ClipFar))
	{
		//--- wangjian added ---//
		// 异步加载 ：如果节点没有开始加载 则加载开始
		if( _enableLoading && !node.checkNodeLoadStarted() )
		{
			//node.setLoadPriority( getNodeLoadPriority( node.getWorldTranslation(),_camera ) );
			node.load(ATOM_GetRenderDevice());
			return;
		}
		//----------------------//

		if (_updateVisibleStamp)
		{
			node.setVisibleStamp (_frameStamp);
		}

		ATOM_Material *material = node.getMaterial();
		if (material)
		{
			addDrawable (&node, node.getWorldMatrix(), material, -1);
		}
	}
#endif
}

void ATOM_CullVisitor::visit (ATOM_LightNode &node)
{
	ATOM_STACK_TRACE(ATOM_CullVisitor::visit);

	return;
}

void ATOM_CullVisitor::visit (ATOM_Actor &node)
{
	visit ((ATOM_VisualNode&)node);
}

void ATOM_CullVisitor::visit (ATOM_VisualNode &node)
{
	ATOM_STACK_TRACE(ATOM_CullVisitor::visit);

	if (node.getShow () != ATOM_Node::SHOW || !_camera)
	{
		return;
	}

	if (node.getLoadInterface()->getLoadingState() == ATOM_LoadInterface::LS_LOADING)
	{
		return;
	}

	//--- wangjian added ---//

	if( ATOM_AsyncLoader::isEnableSceneMTLoad() && !_enableLoading && !node.checkNodeLoadStarted() )
		return;

	// 异步加载 ： 如果节点没有加载完成，则返回
	if( !node.checkNodeAllFinshed() )
	{
		return;
	}
	//----------------------//

	const ATOM_Matrix4x4f &projMatrix = _camera->getProjectionMatrix();
	const ATOM_Matrix4x4f &viewMatrix = _camera->getViewMatrix();
	const ATOM_BBox &bbox = node.getWorldBoundingbox ();

	bool skipClipTest = node.isSkipClipTest();
	if (skipClipTest || frustumTest (&node, bbox))
	{
		if (node.isContributeCullingEnabled() && contributeCulling (bbox, _camera))
		{
			return;
		}

#ifdef USE_TERRAIN_OCCLUSION_TEST
		if (!skipClipTest && testOcclusion (&node, 300.f))
		{
			return;
		}
#endif

		//--- wangjian added ---//
		// 异步加载 ：如果节点没有开始加载 则加载开始
		if( _enableLoading && !node.checkNodeLoadStarted() )
		{
			//node.setLoadPriority( getNodeLoadPriority( node.getWorldTranslation(),_camera ) );
			node.load(ATOM_GetRenderDevice());
			return;
		}
		//----------------------//

		if (_updateVisibleStamp)
		{
			node.setVisibleStamp (_frameStamp);
		}

		node.setupRenderQueue (this);

		if ((node.getDrawBoundingbox () || (_flags & CULL_DRAW_BOUNDINGBOX) != 0))
		{
			queueBoundingboxRD (node, node.getWorldMatrix());
		}
	}
}

void ATOM_CullVisitor::visit (ATOM_Geode &node)
{
	ATOM_STACK_TRACE(ATOM_CullVisitor::visit);

	visit ((ATOM_VisualNode&)node);
}

void ATOM_CullVisitor::visit (ATOM_CompositionNode &node)
{
#if 1
	visit ((ATOM_VisualNode&)node);
#else
	if (node.getShow () != ATOM_Node::SHOW || !_camera)
	{
		return;
	}

	if (node.getLoadInterface()->getLoadingState() == ATOM_LoadInterface::LS_LOADING)
	{
		return;
	}

	//--- wangjian added ---//

	if( ATOM_AsyncLoader::isEnableSceneMTLoad() && !_enableLoading && !node.checkNodeLoadStarted() )
		return;

	// 异步加载 ： 如果节点没有加载完成，则返回
	if( !node.checkNodeAllFinshed() )
	{
		return;
	}
	//----------------------//

	const ATOM_Matrix4x4f &projMatrix = _camera->getProjectionMatrix();
	const ATOM_Matrix4x4f &viewMatrix = _camera->getViewMatrix();
	const ATOM_BBox &bbox = node.getWorldBoundingbox ();

	bool skipClipTest = node.isSkipClipTest();
	if (skipClipTest || frustumTest (&node, bbox))
	{
		if (node.isContributeCullingEnabled() && contributeCulling (bbox, _camera))
		{
			return;
		}

#ifdef USE_TERRAIN_OCCLUSION_TEST
		if (!skipClipTest && testOcclusion (&node, 300.f))
		{
			return;
		}
#endif

		//--- wangjian added ---//
		// 异步加载 ：如果节点没有开始加载 则加载开始
		if( _enableLoading && !node.checkNodeLoadStarted() )
		{
			//node.setLoadPriority( getNodeLoadPriority( node.getWorldTranslation(),_camera ) );
			node.load(ATOM_GetRenderDevice());
			return;
		}
		//----------------------//

		if (_updateVisibleStamp)
		{
			node.setVisibleStamp (_frameStamp);
		}

		node.setupRenderQueue (this);

		if ((node.getDrawBoundingbox () || (_flags & CULL_DRAW_BOUNDINGBOX) != 0))
		{
			queueBoundingboxRD (node, node.getWorldMatrix());
		}
	}
#endif
}

void ATOM_CullVisitor::visit (ATOM_Hud &node)
{
	ATOM_STACK_TRACE(ATOM_CullVisitor::visit);

	if (node.getShow () != ATOM_Node::SHOW || !_camera)
	{
		return;
	}

	//--- wangjian added ---//
	// 异步加载 ： 如果节点没有加载完成，则返回
	if( !node.checkNodeAllFinshed() )
	{
		return;
	}
	//----------------------//

	int x, y, w, h;

	if (node.isScreenSpace ())
	{
		node.getPosition (x, y);
		w = node.getWidth ();
		h = node.getHeight ();
		x -= w / 2;
		y -= h / 2;
	}
	else
	{
		const ATOM_Vector3f &c = node.getCenterPosition ();
		if (c.z < 0.f || c.z > 1.f)
		{
			// Behind the camera
			return;
		}

		w = node.getWidth ();
		h = node.getHeight ();
		x = ATOM_ftol (c.x) - w / 2;
		y = ATOM_ftol (c.y) - h / 2;
	}

	const ATOM_Rect2Di &viewport = _camera->getViewport ();
	if (x >= viewport.size.w || y >= viewport.size.h || x + w <= 0 || y + h <= 0)
	{
		return;
	}

	//--- wangjian added ---//
	// 异步加载 ：如果节点没有开始加载 则加载开始
	/*if( !node.checkNodeLoadStarted() )
	{
	node.load(ATOM_GetRenderDevice());
	return;
	}*/
	//----------------------//

	if (_updateVisibleStamp)
	{
		node.setVisibleStamp (_frameStamp);
	}

	ATOM_Material *material = node.getMaterial();
	if (material)
	{
		addDrawable (&node, node.getWorldMatrix (), node.getMaterial());
	}
}

void ATOM_CullVisitor::visit (ATOM_GuiHud &node)
{
	ATOM_STACK_TRACE(ATOM_CullVisitor::visit);

	if (node.getShow () != ATOM_Node::SHOW || !_camera)
	{
		return;
	}

	//--- wangjian added ---//
	// 异步加载 ： 如果节点没有加载完成，则返回
	if( !node.checkNodeAllFinshed() )
	{
		return;
	}
	//----------------------//

	int x, y, w, h;

	if (node.isScreenSpace ())
	{
		node.getPosition (x, y);
		w = node.getWidth ();
		h = node.getHeight ();
		x -= w / 2;
		y -= h / 2;
	}
	else
	{
		const ATOM_Vector3f &c = node.getCenterPosition ();
		if (c.z < 0.f || c.z > 1.f)
		{
			// Behind the camera
			return;
		}

		w = node.getWidth ();
		h = node.getHeight ();
		x = ATOM_ftol (c.x) - w / 2;
		y = ATOM_ftol (c.y) - h / 2;
	}

	const ATOM_Rect2Di &viewport = _camera->getViewport ();
	if (x >= viewport.size.w || y >= viewport.size.h || x + w <= 0 || y + h <= 0)
	{
		return;
	}

	//--- wangjian added ---//
	// 异步加载 ：如果节点没有开始加载 则加载开始
	/*if( !node.checkNodeLoadStarted() )
	{
		node.load(ATOM_GetRenderDevice());
		return;
	}*/
	//----------------------//

	if (_updateVisibleStamp)
	{
		node.setVisibleStamp (_frameStamp);
	}

	//------------------------------//
	// wangjian modified
#if 0
	addDrawable (&node, node.getWorldMatrix (), 0, ATOM_DeferredRenderScheme::RenderScheme_HUD);
#else
	int layer = ATOM_RenderSettings::isNonDeferredShading() ?	ATOM_CustomRenderScheme::RenderScheme_HUD :
																ATOM_DeferredRenderScheme::RenderScheme_HUD;
	addDrawable (	&node, node.getWorldMatrix (), 0, layer	);
#endif
	//------------------------------//
}

void ATOM_CullVisitor::visit (ATOM_Water &node)
{
	/*
	ATOM_STACK_TRACE(ATOM_CullVisitor::visit);

//	return;

	if (node.getShow () != ATOM_Node::SHOW || !_camera)
	{
		return;
	}

	const ATOM_BBox &bbox = node.getWorldBoundingbox ();
	if (frustumTest (&node, bbox))
	{
		if (node.isContributeCullingEnabled() && contributeCulling (bbox, _camera))
		{
			return;
		}

		if (_updateVisibleStamp)
		{
			node.setVisibleStamp (_frameStamp);
		}

		node.update (_camera);
		bool postEffect = (_flags & CULL_GBUFFER) != 0 && ATOM_RenderSettings::getRenderCaps().shaderModel >= 3;
		if (node.setupWaterParameters (postEffect))
		{
			if (!postEffect)
			{
				getRenderQueue()->addDrawable (node.getInstanceMesh(), _camera, node.getWorldMatrix(), 0, 0);
			}
		}
	}
	*/
}

void ATOM_CullVisitor::visit (ATOM_Atmosphere &node)
{
	ATOM_STACK_TRACE(ATOM_CullVisitor::visit);

	if (node.getShow () != ATOM_Node::SHOW || !_camera)
	{
		return;
	}

	addDrawable (&node, ATOM_Matrix4x4f::getIdentityMatrix(), node.getMaterial());
}

void ATOM_CullVisitor::visit (ATOM_Sky &node)
{
	if (!node.isDrawSkyEnabled() || node.getShow () != ATOM_Node::SHOW || !_camera)
	{
		return;
	}

	//--- wangjian added ---//
	if( ATOM_AsyncLoader::isEnableSceneMTLoad() && !node.checkNodeLoadStarted() )
	{
		if( _enableLoading )
			node.load(ATOM_GetRenderDevice());	// 异步加载 ：如果节点没有开始加载 则加载开始
		return;
	}

	// 异步加载 ： 如果节点没有加载完成，则返回
	if( !node.checkNodeAllFinshed() )
	{
		return;
	}
	//----------------------//

	addDrawable (&node, ATOM_Matrix4x4f::getIdentityMatrix(), node.getMaterial());
}

void ATOM_CullVisitor::visit (ATOM_BkImage &node)
{
	if (node.getShow() != ATOM_Node::SHOW)
	{
		return;
	}

	//--- wangjian added ---//
	if( ATOM_AsyncLoader::isEnableSceneMTLoad() && !_enableLoading && !node.checkNodeLoadStarted() )
		return;

	// 异步加载 ： 如果节点没有加载完成，则返回
	if( !node.checkNodeAllFinshed() )
	{
		return;
	}
	//----------------------//

	//--- wangjian added ---//
	// 异步加载 ：如果节点没有开始加载 则加载开始
	if( _enableLoading && !node.checkNodeLoadStarted() )
	{
		node.load(ATOM_GetRenderDevice());
		return;
	}
	//----------------------//

	addDrawable (&node, ATOM_Matrix4x4f::getIdentityMatrix(), node.getMaterial(), -1);
}

void ATOM_CullVisitor::visit (ATOM_Atmosphere2 &node)
{
	ATOM_STACK_TRACE(ATOM_CullVisitor::visit);

	if (node.getShow () != ATOM_Node::SHOW || !_camera)
	{
		return;
	}

	addDrawable (&node, ATOM_Matrix4x4f::getIdentityMatrix(), node.getMaterial());
}

void ATOM_CullVisitor::visit (ATOM_ParticleSystem &node)
{
	ATOM_STACK_TRACE(ATOM_CullVisitor::visit);

	if (node.getShow () != ATOM_Node::SHOW || !_camera)
	{
		return;
	}

	//--- wangjian added ---//
	// 异步加载 ： 如果节点没有加载完成，则返回
	if( !node.checkNodeAllFinshed() )
	{
		return;
	}
	//----------------------//

	const ATOM_Matrix4x4f &projectionMatrix = _camera->getProjectionMatrix();
	const ATOM_Matrix4x4f &viewMatrix = _camera->getViewMatrix ();
	const ATOM_BBox &bbox = node.getWorldBoundingbox ();

	bool skipClipTest = node.isSkipClipTest();
	if (skipClipTest || frustumTest (&node, bbox))
	{
		if (node.isContributeCullingEnabled() && contributeCulling (bbox, _camera))
		{
			return;
		}
#ifdef USE_TERRAIN_OCCLUSION_TEST
		if (node.getActiveParticleCount() > 0 && testOcclusion (&node, 300.f))
		{
			return;
		}
#endif

		//--- wangjian added ---//
		// 异步加载 ：如果节点没有开始加载 则加载开始
		if( _enableLoading && !node.checkNodeLoadStarted() )
		{
			//node.setLoadPriority( getNodeLoadPriority( node.getWorldTranslation(),_camera ) );
			node.load(ATOM_GetRenderDevice());
			return;
		}
		//----------------------//

		if (_updateVisibleStamp)
		{
			node.setVisibleStamp (_frameStamp);
		}

		//node.setUpdateInterval (50);

		if (node.getActiveParticleCount () > 0)
		{
			const ATOM_Matrix4x4f &worldMatrix = node.getWorldMatrix ();

			node.updateVertexArrays (_camera);
			float scalar = node.getScalar ();
			//ATOM_Matrix4x4f matScale = ATOM_Matrix4x4f::getScaleMatrix (ATOM_Vector3f(scalar, scalar, scalar));

			addDrawable (&node, ((node.getFlags() & ATOM_ParticleSystem::PS_WORLDSPACE) == 0) ? worldMatrix : ATOM_Matrix4x4f::getIdentityMatrix(), node.getMaterial());

			if ((node.getDrawBoundingbox () || (_flags & CULL_DRAW_BOUNDINGBOX) != 0))
			{
				queueBoundingboxRD (node, worldMatrix);
			}
		}
	}
	else
	{
		//node.setUpdateInterval (200);
	}
}

void ATOM_CullVisitor::visit (ClientSimpleCharacter &node)
{
	visit ((ATOM_Geode&)node);
}

bool ATOM_CullVisitor::occludedByTerrain (ATOM_Terrain *terrain, ATOM_Node *node, float minSize) const
{
	ATOM_STACK_TRACE(ATOM_CullVisitor::occludedByTerrain);

	const ATOM_Matrix4x4f &viewMatrix = _camera->getViewMatrix ();
	const ATOM_BBox &bboxWorld = node->getWorldBoundingbox ();
	float diagnal = (bboxWorld.getMax() - bboxWorld.getMin()).getSquaredLength();
	if (diagnal > minSize * minSize)
	{
		// object is too big for occlusion test
		return false;
	}

	ATOM_Vector3f cameraPos(viewMatrix.m30, viewMatrix.m31, viewMatrix.m32);
	ATOM_Vector3f rayOrigin = _isTerrainOccluderWorldMatrixIdentity ? cameraPos : _invTerrainWorldMatrix >> cameraPos;

	const ATOM_Vector3f center = bboxWorld.getCenter ();

	float distanceSq = (center - cameraPos).getSquaredLength();
	if (distanceSq < diagnal * 0.25f)
	{
		return false;
	}

	ATOM_Vector3f corners[3] = 
	{
		ATOM_Vector3f(bboxWorld.getMin().x, bboxWorld.getMax().y, bboxWorld.getMax().z),
		ATOM_Vector3f(bboxWorld.getMax().x, bboxWorld.getMax().y, bboxWorld.getMax().z),
		ATOM_Vector3f(bboxWorld.getMax().x, bboxWorld.getMax().y, bboxWorld.getMin().z)
	};

	for (unsigned i = 0; i < 3; ++i)
	{
		float t;

		if (!_isTerrainOccluderWorldMatrixIdentity)
		{
			corners[i] <<= _invTerrainWorldMatrix;
		}

		ATOM_Vector3f direction = corners[i] - rayOrigin;
		if (!terrain->rayIntersect (rayOrigin, direction, t) || t * t > direction.getSquaredLength())
		{
			return false;
		}
	}

	return true;
}

bool ATOM_CullVisitor::testOcclusion (ATOM_Node *node, float minSize) const
{
	ATOM_STACK_TRACE(ATOM_CullVisitor::testOcclusion);

	if (_terrainOccluder)
	{
		unsigned noOccludedFrameStamp = node->getNoOccludedFrameStamp ();
		if (!noOccludedFrameStamp)
		{
			// is occluded last frame, must check
			if (!occludedByTerrain (_terrainOccluder, node, minSize))
			{
				// is visible, set the frame count to next occlusion test
				node->setNoOccludedFrameStamp (60);
			}
			else
			{
				// is occluded
				return true;
			}
		}
		else
		{
			// is visible last frame, decrease the frame count for next occlusion test
			node->setNoOccludedFrameStamp (noOccludedFrameStamp-1);
		}
	}
	return false;
}

void ATOM_CullVisitor::setTerrainOccluder (ATOM_Terrain *occluder)
{
	_terrainOccluder = occluder;
}

ATOM_Terrain *ATOM_CullVisitor::getTerrainOccluder (void) const
{
	return _terrainOccluder;
}

void ATOM_CullVisitor::setUpdateVisibleStamp (bool b)
{
	_updateVisibleStamp = b;
}

bool ATOM_CullVisitor::getUpdateVisibleStamp (void) const
{
	return _updateVisibleStamp;
}

void ATOM_CullVisitor::setDistanceOverrideValue (float value)
{
	_distanceOverrideValue = value;
}

float ATOM_CullVisitor::getDistanceOverrideValue (void) const
{
	return _distanceOverrideValue;
}
