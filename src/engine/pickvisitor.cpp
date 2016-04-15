#include "StdAfx.h"
#include "pickvisitor.h"
#include "nodeoctree.h"
#include "camera.h"
#include "terrain.h"
#include "geode2.h"

ATOM_PickVisitor::ATOM_PickVisitor (void)
{
	ATOM_STACK_TRACE(ATOM_PickVisitor::ATOM_PickVisitor);

	_camera = 0;
	_flags = 0;
	_pickAll = false;
	_intersected = false;
	_screenPosX = 0;
	_screenPosY = 0;
}

ATOM_PickVisitor::~ATOM_PickVisitor (void)
{
	ATOM_STACK_TRACE(ATOM_PickVisitor::~ATOM_PickVisitor);

	_camera = 0;
}

void ATOM_PickVisitor::setPickAll (bool pickAll)
{
	_pickAll = pickAll;
}

bool ATOM_PickVisitor::getPickAll (void) const
{
	return _pickAll;
}

void ATOM_PickVisitor::onBeginVisitNodeTree (ATOM_Node &node)
{
}

void ATOM_PickVisitor::onEndVisitNodeTree (ATOM_Node &node)
{
}

void ATOM_PickVisitor::onResetVisitor (void)
{
	_results.resize (0);
	_intersected = false;
}

void ATOM_PickVisitor::onNodePicked (ATOM_Node &node, float distance, float u, float v)
{
	ATOM_STACK_TRACE(ATOM_PickVisitor::onNodePicked);

	_results.resize (_results.size() + 1);
	_results.back().node = &node;
	_results.back().distance = distance;
	_results.back().u = u;
	_results.back().v = v;
}

void ATOM_PickVisitor::traverse (ATOM_Node &node)
{
	ATOM_STACK_TRACE(ATOM_PickVisitor::traverse);

	if (_camera)
	{
		ATOM_Visitor::traverse (node);
	}
}

void ATOM_PickVisitor::visit (ATOM_Node &node)
{
	ATOM_STACK_TRACE(ATOM_PickVisitor::visit);

	if (!_pickAll && !node.getPickable ())
	{
		return;
	}

	if (node.getShow() != ATOM_Node::SHOW && node.getShow() != ATOM_Node::HIDE_PICKABLE)
	{
		return;
	}

	const ATOM_Matrix4x4f &projectionMatrix = _camera->getProjectionMatrix();
	const ATOM_Matrix4x4f &viewMatrix = _camera->getViewMatrix ();
	const ATOM_BBox &bbox = node.getWorldBoundingbox ();

	if ((_flags & INTERSECTION_PROBE) != 0)
	{
		_intersected = _ray.intersectionTest (bbox);

		if (_intersected)
		{
			cancelTraverse ();
		}
	}
	else
	{
		float d;

		if (_ray.intersectionTestEx (bbox, d))
		{
			onNodePicked (node, d, 0.f, 0.f);
		}
	}
}

void ATOM_PickVisitor::visit (ATOM_NodeOctree &node)
{
	ATOM_STACK_TRACE(ATOM_PickVisitor::visit);

	if (!ATOM_RenderSettings::isOctreeEnabled ())
	{
		visit ((ATOM_Node&)node);
		return;
	}

	if (!_pickAll && !node.getOctree ())
	{
		return;
	}

	PickOctree_r (node.getOctree()->getRootNode());

	skipChildren ();
}

void ATOM_PickVisitor::PickOctree_r (ATOM_OctreeNode *node)
{
	if (!node)
	{
		return;
	}

    ATOM_BBox nodeBBox(node->getMinPointLoosed(), node->getMaxPointLoosed ());

	if (!_ray.intersectionTest (nodeBBox))
	{
		return;
	}

	ATOM_SpatialCell::NodeInfo *nodeInfo = node->getHead();
	while (nodeInfo)
	{
		ATOM_SpatialCell::NodeInfo *next = nodeInfo->next;
		nodeInfo->node->accept (*this);
		if (traverseCanceled ()) return;
		nodeInfo = next;
	}

	for (int i = ATOM_OctreeNode::PLACEMENTBEGIN; i != ATOM_OctreeNode::PLACEMENTEND; ++i)
	{
		ATOM_OctreeNode *child = node->getChild ((ATOM_OctreeNode::Placement)i);

		if (child)
		{
			PickOctree_r (child);
		}
	}
}

void ATOM_PickVisitor::visit (ATOM_Hud &node)
{
	ATOM_STACK_TRACE(ATOM_PickVisitor::visit);

	if (!_pickAll && !node.getPickable ())
	{
		return;
	}

	if ((_flags & HAVE_SCREEN_POSITION) == 0 || (node.getShow() != ATOM_Node::SHOW && node.getShow() != ATOM_Node::HIDE_PICKABLE) || node.isScreenSpace())
	{
		return;
	}

	if (_screenPosX < 0 || _screenPosY < 0)
	{
		return;
	}

	const ATOM_Vector3f &center = node.getCenterPosition ();
	int centerX = ATOM_ftol (center.x);
	int centerY = ATOM_ftol (center.y);
	unsigned w = node.getWidth();
	unsigned h = node.getHeight();

	int l, t, r, b;
	if (node.getAlignment() & ATOM_Hud::ALIGN_LEFT)
	{
		l = centerX;
	}
	else if (node.getAlignment() & ATOM_Hud::ALIGN_RIGHT)
	{
		l = centerX - w;
	}
	else
	{
		l = centerX - w / 2;
	}

	if (node.getAlignment() & ATOM_Hud::ALIGN_TOP)
	{
		t = centerY;
	}
	else if (node.getAlignment() & ATOM_Hud::ALIGN_BOTTOM)
	{
		t = centerY - h;
	}
	else
	{
		t = centerY - h / 2;
	}

	r = l + w;
	b = t + h;

	if (_screenPosX < l || _screenPosX >= r || _screenPosY < t || _screenPosY >= b)
	{
		return;
	}

	float u = float(_screenPosX - l) / float(r - l - 1);
	float v = float(_screenPosY - t) / float(b - t - 1);

	const ATOM_Matrix4x4f &worldMatrix = node.getWorldMatrix();
	ATOM_Vector3f vPos(worldMatrix.m30, worldMatrix.m31, worldMatrix.m32);
	ATOM_Vector3f oPos = _ray.getOrigin();
	float distance = (oPos - vPos).getLength ();

	_intersected = true;

	onNodePicked (node, distance, u, v);

	return;
}

void ATOM_PickVisitor::visit (ATOM_CompositionNode &node)
{
	visit ((ATOM_VisualNode&)node);
}

void ATOM_PickVisitor::visit (ATOM_ParticleSystem &node)
{
	visit ((ATOM_Node&)node);
}

void ATOM_PickVisitor::visit (ATOM_ShapeNode &node)
{
	visit ((ATOM_VisualNode&)node);
}

void ATOM_PickVisitor::visit (ATOM_WeaponTrail &node)
{
	visit ((ATOM_VisualNode&)node);
}

void ATOM_PickVisitor::visit (ATOM_Terrain &node)
{
	ATOM_STACK_TRACE(ATOM_PickVisitor::visit);

	if (!_pickAll && !node.getPickable ())
	{
		return;
	}

	if (node.getShow() != ATOM_Node::SHOW && node.getShow() != ATOM_Node::HIDE_PICKABLE)
	{
		return;
	}

	ATOM_Vector3f start = _ray.getOrigin ();
	ATOM_Vector3f end = start + _ray.getDirection ();
	const ATOM_Matrix4x4f &projectionMatrix = _camera->getProjectionMatrix();
	const ATOM_Matrix4x4f &viewMatrix = _camera->getViewMatrix ();
	const ATOM_Matrix4x4f invWorldMatrix = node.getInvWorldMatrix ();
	start <<= invWorldMatrix;
	end <<= invWorldMatrix;
	ATOM_Vector3f dir = end - start;
	dir.normalize ();

	float d;
	if (node.rayIntersect (start, dir, d))
	{
		if ((_flags & INTERSECTION_PROBE) != 0)
		{
			_intersected = true;
			cancelTraverse ();
		}
		else
		{
			dir = node.getWorldMatrix().transformVectorAffine (d * dir);
			onNodePicked (node, dir.getLength(), 0.f, 0.f);
		}
	}
}

void ATOM_PickVisitor::visit (ATOM_Water &node)
{
	ATOM_STACK_TRACE(ATOM_PickVisitor::visit);

	if (!_pickAll && !node.getPickable ())
	{
		return;
	}

	if (node.getShow() != ATOM_Node::SHOW && node.getShow() != ATOM_Node::HIDE_PICKABLE)
	{
		return;
	}

	const ATOM_Matrix4x4f &projectionMatrix = _camera->getProjectionMatrix();
	const ATOM_Matrix4x4f &viewMatrix = _camera->getViewMatrix ();
	const ATOM_Matrix4x4f &invWorld = node.getInvWorldMatrix ();
	ATOM_Vector3f vStart = invWorld >> _ray.getOrigin();
	ATOM_Vector3f vEnd = invWorld >> (_ray.getOrigin() + _ray.getDirection());
	ATOM_Vector3f vDir = vEnd - vStart;
	vDir.normalize ();
	ATOM_Ray rayObjectSpace(vStart, vDir);

	if ((_flags & INTERSECTION_PROBE) != 0)
	{
		if (node.rayIntersectionTest (_camera, rayObjectSpace, 0))
		{
			_intersected = true;
			cancelTraverse ();
		}
	}
	else
	{
		float d;
		if (node.rayIntersectionTest (_camera, rayObjectSpace, &d))
		{
			ATOM_Vector3f dir = node.getWorldMatrix().transformVectorAffine (d * rayObjectSpace.getDirection());
			onNodePicked (node, dir.getLength(), 0.f, 0.f);
		}
	}
}

void ATOM_PickVisitor::visit (ATOM_LightNode &node)
{
	visit ((ATOM_Node &)node);
}

void ATOM_PickVisitor::visit (ATOM_VisualNode &node)
{
	ATOM_STACK_TRACE(ATOM_PickVisitor::visit);

	if (!_pickAll && !node.getPickable ())
	{
		return;
	}

	if (node.getShow() != ATOM_Node::SHOW && node.getShow() != ATOM_Node::HIDE_PICKABLE)
	{
		return;
	}

	if (node.getLoadInterface()->getLoadingState() == ATOM_LoadInterface::LS_LOADING)
	{
		return;
	}

	const ATOM_Matrix4x4f &projectionMatrix = _camera->getProjectionMatrix();
	const ATOM_Matrix4x4f &viewMatrix = _camera->getViewMatrix ();
	const ATOM_Matrix4x4f &invWorld = node.getInvWorldMatrix ();
	ATOM_Vector3f vStart = invWorld >> _ray.getOrigin();
	ATOM_Vector3f vEnd = invWorld >> (_ray.getOrigin() + _ray.getDirection());
	ATOM_Vector3f vDir = vEnd - vStart;
	vDir.normalize ();
	ATOM_Ray rayObjectSpace(vStart, vDir);

	if ((_flags & INTERSECTION_PROBE) != 0)
	{
		if (node.rayIntersectionTest (_camera, rayObjectSpace, 0))
		{
			_intersected = true;
			cancelTraverse ();
		}
	}
	else
	{
		float d;
		if (node.rayIntersectionTest (_camera, rayObjectSpace, &d))
		{
			ATOM_Vector3f dir = node.getWorldMatrix().transformVectorAffine (d * rayObjectSpace.getDirection());
			onNodePicked (node, dir.getLength(), 0.f, 0.f);
		}
	}
}

void ATOM_PickVisitor::visit (ATOM_Geode &node)
{
	visit ((ATOM_VisualNode&)node);
}

void ATOM_PickVisitor::visit (ATOM_Atmosphere &node)
{
}

void ATOM_PickVisitor::visit (ATOM_Atmosphere2 &node)
{
}

void ATOM_PickVisitor::visit (ATOM_Sky &node)
{
	//onNodePicked (node, 999999.f, 0.f, 0.f);
}

void ATOM_PickVisitor::sortResults (void)
{
	ATOM_STACK_TRACE(ATOM_PickVisitor::sortResults);

	std::sort (_results.begin(), _results.end());
}

