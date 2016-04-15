#ifndef __ATOM3D_ENGINE_SHADOWMAP_MANAGER_H
#define __ATOM3D_ENGINE_SHADOWMAP_MANAGER_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <ATOM_render.h>
#include "basedefs.h"

class ATOM_DeferredScene;
class ATOM_Camera;

class ATOM_ShadowMapManager
{
public:
	ATOM_ShadowMapManager (void);
	virtual ~ATOM_ShadowMapManager (void);

public:
	void setShadowMapWidth (unsigned width);
	unsigned getShadowMapWidth (void) const;
	void setShadowMapHeight (unsigned height);
	unsigned getShadowMapHeight (void) const;
	void setShadowDistance (float distance);
	float getShadowDistance (void) const;
	bool renderShadowMap (ATOM_DeferredScene *scene, ATOM_Camera *camera);
	bool createShadowMaskTexture (ATOM_Texture *shadowMaskTexture, ATOM_Texture *gbufferTexture);

public:
	virtual ATOM_Texture *getShadowMapTexture (void) const = 0;
	virtual bool isValid (void) const = 0;

protected:
	virtual bool onRenderShadowMap (ATOM_DeferredScene *scene, ATOM_Camera *camera) = 0;
	virtual bool onCreateShadowMaskTexture (ATOM_Texture *shadowMaskTexture, ATOM_Texture *gbufferTexture) = 0;
	virtual bool onShadowMapSizeChanged (void);
	virtual bool onShadowDistanceChanged (void);
	virtual bool initializeShadowMapTexture (unsigned w, unsigned h) = 0;

private:
	float _shadowDistance;
	unsigned _shadowMapWidth;
	unsigned _shadowMapHeight;
	bool _shadowMapSizeChanged;
	bool _shadowDistanceChanged;
};

#endif // __ATOM3D_ENGINE_SHADOWMAP_MANAGER_H
