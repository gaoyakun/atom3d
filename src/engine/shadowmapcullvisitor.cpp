#include "StdAfx.h"
#include "shadowmapcullvisitor.h"
#include "shadowmaprenderscheme.h"

ATOM_ShadowMapCullVisitor::ATOM_ShadowMapCullVisitor (ATOM_DeferredShadowMapRenderScheme *shadowMapRenderScheme)
{
	_shadowMapRenderScheme = shadowMapRenderScheme;
	_terrain = 0;

	_enableLoading = false;
}

ATOM_ShadowMapCullVisitor::~ATOM_ShadowMapCullVisitor (void)
{
}

void ATOM_ShadowMapCullVisitor::visit (ATOM_LightNode &node)
{
}

void ATOM_ShadowMapCullVisitor::visit (ATOM_Atmosphere2 &node)
{
}

void ATOM_ShadowMapCullVisitor::visit (ATOM_Sky &node)
{
}

void ATOM_ShadowMapCullVisitor::visit (ATOM_Water &node)
{
}

void ATOM_ShadowMapCullVisitor::visit (ATOM_ParticleSystem &node)
{
}

void ATOM_ShadowMapCullVisitor::visit (ATOM_Hud &node)
{
}

void ATOM_ShadowMapCullVisitor::visit (ATOM_Terrain &node)
{
	_terrain = &node;
	_terrain->setLODCamera (_terrainLODcamera);

	ATOM_CullVisitor::visit (node);
}

void ATOM_ShadowMapCullVisitor::visit (ATOM_GuiHud &node)
{
}

void ATOM_ShadowMapCullVisitor::visit (ATOM_Decal &node)
{
}

void ATOM_ShadowMapCullVisitor::visit (ATOM_Geode &node)
{
	ATOM_CullVisitor::visit (node);
}

void ATOM_ShadowMapCullVisitor::visit (ATOM_Actor &node)
{
}

