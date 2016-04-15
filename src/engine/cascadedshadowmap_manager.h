#ifndef __ATOM3D_CASCADEDSHADOWMAP_MANAGER_H
#define __ATOM3D_CASCADEDSHADOWMAP_MANAGER_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <ATOM_math.h>
#include "shadowmap_manager.h"

class ATOM_CascadedShadowMapManager: public ATOM_ShadowMapManager
{
public:
	ATOM_CascadedShadowMapManager (void);
	virtual ~ATOM_CascadedShadowMapManager (void);

public:
	void setLightDistance (float lightDistance);
	float getLightDistance (void) const;
	void setSplitLambda (float lambda);
	float getSplitLambda (void) const;
	void setMinShadowVariance (float value);
	float getMinShadowVariance (void) const;
	void setAntiLightBleedingValue (float value);
	float getAntiLightBleedingValue (void) const;
	const float *getSplitDistances (void) const;
	const ATOM_Matrix4x4f *getShadowMatrices (void) const;
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
	void calculateLightMatrices (int split, ATOM_Camera *camera, const ATOM_BBox &sceneBBox, const ATOM_Vector3f &lightDir, ATOM_Matrix4x4f &viewMatrix, ATOM_Matrix4x4f &projMatrix);
	void calculateSplitDistances (ATOM_Camera *camera);
	void calculateFrustumCorners (int split, ATOM_Camera *camera, ATOM_Vector3f corners[9]) const;

public:
	static ATOM_CascadedShadowMapManager *getInstance (void);

private:
	float _lightDistance;
	ATOM_Matrix4x4f _lightViewProj[4];
	float _splitDistances[5];
	float _splitLambda;
	float _viewAspect;
	float _invTanHalfFovY;
	float _minShadowVariance;
	float _antiLightBleedingValue;
	float _sceneScale;
	float _lightSize;
	float _depthBias;
	ATOM_AUTOPTR(ATOM_Material) _shadowMaskMaterial;
	mutable ATOM_AUTOREF(ATOM_Texture) _shadowMap;
	mutable ATOM_AUTOREF(ATOM_Texture) _shadowMapTmp;
	mutable ATOM_AUTOREF(ATOM_DepthBuffer) _shadowDepthBuffer;
};

#endif // __ATOM3D_STDSHADOWMANAGER_H
