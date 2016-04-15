#ifndef __ATOM3D_STUDIO_TERRAIN_SHADOWMAP_CULLVISITOR_H
#define __ATOM3D_STUDIO_TERRAIN_SHADOWMAP_CULLVISITOR_H

#include <ATOM_engine.h>

class EditorRenderScheme;

class TerrainShadowMapCullVisitor: public ATOM_CullVisitor
{
public:
	TerrainShadowMapCullVisitor (EditorRenderScheme *editorRenderScheme);
	virtual ~TerrainShadowMapCullVisitor (void);

public:
	virtual void visit (ATOM_Node &node);
	virtual void visit (ATOM_Terrain &node);
	virtual void visit (ATOM_VisualNode &node);
	virtual void visit (ATOM_Geode &node);
	virtual void visit (ATOM_ParticleSystem &node);
	virtual void visit (ATOM_Hud &node);
	virtual void visit (ATOM_GuiHud &node);
	virtual void visit (ATOM_Water &node);
	virtual void visit (ATOM_LightNode &node);
	virtual void visit (ATOM_Atmosphere &node);
	virtual void visit (ATOM_Atmosphere2 &node);
	virtual void visit (ATOM_Sky &node);
	virtual void visit (ATOM_Actor &node);
	virtual void visit (ATOM_Decal &node);
	virtual void visit (ClientSimpleCharacter &node);

public:
	ATOM_BBox _bbox;

private:
	EditorRenderScheme *_editorRenderScheme;
};

#endif // __ATOM3D_STUDIO_TERRAIN_SHADOWMAP_CULLVISITOR_H
