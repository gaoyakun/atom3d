#ifndef __ATOM3D_STDSHADOWMAP_MANAGER_H
#define __ATOM3D_STDSHADOWMAP_MANAGER_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <ATOM_math.h>
#include "shadowmap_manager.h"

class ATOM_StandardShadowMapManager: public ATOM_ShadowMapManager
{
public:
	ATOM_StandardShadowMapManager (void);
	virtual ~ATOM_StandardShadowMapManager (void);

public:
	void setLightDistance (float lightDistance);
	float getLightDistance (void) const;
	void setSceneScale (float value);
	float getSceneScale (void) const;
	void setLightSize (float value);
	float getLightSize (void) const;
	void setDepthBias (float value);
	float getDepthBias (void) const;

public:
	virtual ATOM_Texture *getShadowMapTexture (void) const;
	virtual bool isValid (void) const;

protected:
	virtual bool onRenderShadowMap (ATOM_DeferredScene *scene, ATOM_Camera *camera);
	virtual bool onCreateShadowMaskTexture (ATOM_Texture *shadowMaskTexture, ATOM_Texture *gbufferTexture);
	virtual bool initializeShadowMapTexture (unsigned w, unsigned h);

private:
	void calculateLightMatrices (const ATOM_Vector3f &lightDir, const ATOM_Vector3f &origin, float radius, ATOM_Matrix4x4f &viewMatrix, ATOM_Matrix4x4f &projMatrix) const;

public:
	static ATOM_StandardShadowMapManager *getInstance (void);

private:
	float _lightDistance;
	ATOM_Matrix4x4f _lightViewProj;
	ATOM_Matrix4x4f _lightView;
	float _viewAspect;
	float _invTanHalfFovY;
	float _sceneScale;
	float _lightSize;
	float _depthBias;
	ATOM_AUTOPTR(ATOM_Material) _shadowMaskMaterial;
	ATOM_AUTOREF(ATOM_Texture) _rtOld;
	ATOM_AUTOREF(ATOM_DepthBuffer) _depthOld;
	ATOM_Rect2Di _vpOld;
	ATOM_ColorARGB _clearColorOld;
	mutable ATOM_AUTOREF(ATOM_Texture) _shadowMap;
	mutable ATOM_AUTOREF(ATOM_DepthBuffer) _shadowDepthBuffer;
};

#endif // __ATOM3D_STDSHADOWMANAGER_H
