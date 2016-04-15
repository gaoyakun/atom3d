#ifndef __ATOM3D_ENGINE_SHADOWMAPCULLVISITOR_H
#define __ATOM3D_ENGINE_SHADOWMAPCULLVISITOR_H

#include "cullvisitor.h"

class ATOM_DeferredShadowMapRenderScheme;

class ATOM_ENGINE_API ATOM_ShadowMapCullVisitor: public ATOM_CullVisitor
{
public:
	ATOM_ShadowMapCullVisitor (ATOM_DeferredShadowMapRenderScheme *shadowMapRenderScheme);
	virtual ~ATOM_ShadowMapCullVisitor (void);

public:
	virtual void visit (ATOM_LightNode &node);
	virtual void visit (ATOM_Atmosphere2 &node);
	virtual void visit (ATOM_Sky &node);
	virtual void visit (ATOM_Water &node);
	virtual void visit (ATOM_ParticleSystem &node);
	virtual void visit (ATOM_Hud &node);
	virtual void visit (ATOM_GuiHud &node);
	virtual void visit (ATOM_Terrain &node);
	virtual void visit (ATOM_Decal &node);
	virtual void visit (ATOM_Geode &node);
	virtual void visit (ATOM_Actor &node);

public:
	ATOM_Camera *_terrainLODcamera;
	ATOM_Terrain *_terrain;

private:
	ATOM_DeferredShadowMapRenderScheme *_shadowMapRenderScheme;
};

#endif // __ATOM3D_ENGINE_SHADOWMAPCULLVISITOR_H
