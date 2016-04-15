#include "StdAfx.h"
#include "deferred_cullvisitor.h"
#include "deferredrenderscheme.h"
#include "shadowmaprenderscheme.h"

ATOM_DeferredCullVisitor::ATOM_DeferredCullVisitor (ATOM_DeferredRenderScheme *deferredRenderScheme)
{
	_deferredRenderScheme = deferredRenderScheme;
}

ATOM_DeferredCullVisitor::~ATOM_DeferredCullVisitor (void)
{
}

void ATOM_DeferredCullVisitor::visit (ATOM_Atmosphere2 &node)
{
	ATOM_STACK_TRACE(ATOM_CullVisitor::visit);

	if (node.getShow () != ATOM_Node::SHOW || !_camera)
	{
		return;
	}

	_deferredRenderScheme->addLight (node.getSunLight());
	addDrawable (&node, ATOM_Matrix4x4f::getIdentityMatrix(), node.getMaterial());
}

void ATOM_DeferredCullVisitor::visit (ATOM_Sky &node)
{
	if (node.getShow() == ATOM_Node::SHOW)
	{
		ATOM_CullVisitor::visit (node);
		_deferredRenderScheme->addLight (node.getSunLight());
		_deferredRenderScheme->setSunLight (node.getSunLight());

		ATOM_DeferredShadowMapRenderScheme *shadowMapRenderScheme = _deferredRenderScheme->getShadowMapRenderScheme();
		if (shadowMapRenderScheme)
		{
			shadowMapRenderScheme->setLightDirection (node.getSunLight()->getDirection());
		}
	}
}

void ATOM_DeferredCullVisitor::visit (ATOM_LightNode &node)
{
	if (node.getShow () != ATOM_Node::SHOW)
	{
		return;
	}

	const ATOM_Matrix4x4f &projMatrix = getCamera()->getProjectionMatrix();
	const ATOM_Matrix4x4f &viewMatrix = getCamera()->getViewMatrix();
	const ATOM_BBox &bbox = node.getWorldBoundingbox ();

	if (frustumTest (&node, bbox))
	{
		node.updateLightParams (projMatrix, viewMatrix);
		_deferredRenderScheme->addLight (node.getLight());
	}
}

