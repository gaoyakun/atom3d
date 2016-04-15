#include "terrain_shadowmap_cullvisitor.h"

TerrainShadowMapCullVisitor::TerrainShadowMapCullVisitor (EditorRenderScheme *editorRenderScheme)
{
	_editorRenderScheme = editorRenderScheme;
}

TerrainShadowMapCullVisitor::~TerrainShadowMapCullVisitor (void)
{
}

void TerrainShadowMapCullVisitor::visit (ATOM_Node &node)
{
	return;
}

void TerrainShadowMapCullVisitor::visit (ATOM_Terrain &node)
{
	ATOM_CullVisitor::visit (node);
}

void TerrainShadowMapCullVisitor::visit (ATOM_VisualNode &node)
{
	return;
}

void TerrainShadowMapCullVisitor::visit (ATOM_Geode &node)
{
	return;
}

void TerrainShadowMapCullVisitor::visit (ATOM_ParticleSystem &node)
{
	return;
}

void TerrainShadowMapCullVisitor::visit (ATOM_Hud &node)
{
	return;
}

void TerrainShadowMapCullVisitor::visit (ATOM_GuiHud &node)
{
	return;
}

void TerrainShadowMapCullVisitor::visit (ATOM_Water &node)
{
	return;
}

void TerrainShadowMapCullVisitor::visit (ATOM_LightNode &node)
{
	return;
}

void TerrainShadowMapCullVisitor::visit (ATOM_Atmosphere &node)
{
	return;
}

void TerrainShadowMapCullVisitor::visit (ATOM_Atmosphere2 &node)
{
	return;
}

void TerrainShadowMapCullVisitor::visit (ATOM_Sky &node)
{
	return;
}

void TerrainShadowMapCullVisitor::visit (ATOM_Actor &node)
{
	return;
}

void TerrainShadowMapCullVisitor::visit (ATOM_Decal &node)
{
	return;
}

void TerrainShadowMapCullVisitor::visit (ClientSimpleCharacter &node)
{
	return;
}
