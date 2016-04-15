#ifndef __ATOM3D_ENGINE_SHADOWMAP_H
#define __ATOM3D_ENGINE_SHADOWMAP_H

#include "../ATOM_render.h"
#include "basedefs.h"

class ATOM_ENGINE_API ATOM_ShadowMap
{
public:
	void setLightParams (const ATOM_Vector3f &lightDir, float lightRadius);
	void mergeShadowCaster (const ATOM_BBox &bbox);
	void mergeShadowReciever (const ATOM_BBox &bbox);
	void mergeFrustum (const ATOM_Frustum3d &frustum);
	void beginShadowMapGen (ATOM_RenderDevice *device, bool renderToTexture = true, bool renderShadowMap = true, bool unitCubeClipping = true);
	void endShadowMapGen (ATOM_RenderDevice *device, bool renderToTexture = true);
	void prepareShadowMap (ATOM_RenderDevice *device);
	const ATOM_Matrix4x4f &getLightViewProjMatrix (void) const;

protected:
	virtual void generateShadowMapMatrix (bool unitCubeClipping, ATOM_Matrix4x4f *matrix) = 0;
	virtual const ATOM_Matrix4x4f &getLightViewProjMatrixImpl (void) const = 0;
	virtual void setLightParamsImpl (const ATOM_Vector3f &lightDir, float lightRadius) = 0;

protected:
	ATOM_BBox _casterBBox;
	ATOM_Vector3f _casterVertex[8];

	ATOM_BBox _receiverBBox;
	ATOM_Vector3f _receiverVertex[8];

	ATOM_BBox _frustumBBox;
	ATOM_Vector3f _frustumVertex[8];

	ATOM_Matrix4x4 _view;
	ATOM_Matrix4x4 _lightSpaceBasis;

private:
	ATOM_AUTOREF(ATOM_Texture) _shadowMapTex;
	ATOM_AUTOREF(ATOM_DepthBuffer) _shadowMapDepth;

	ATOM_AUTOREF(ATOM_Texture) _oldRT;
	ATOM_AUTOREF(ATOM_DepthBuffer) _oldDepth;
};

#endif // __ATOM3D_ENGINE_SHADOWMAP_H
