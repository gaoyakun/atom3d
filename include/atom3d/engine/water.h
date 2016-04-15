#ifndef __ATOM3D_ENGINE_WATER_H
#define __ATOM3D_ENGINE_WATER_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_dbghlp.h"
#include "../ATOM_render.h"
#include "node.h"
#include "model.h"

class ATOM_Camera;
class ATOM_InstanceMesh;

class ATOM_ENGINE_API ATOM_Water: public ATOM_Node
{
public:
	static const unsigned tilesize = 128;
	static const unsigned looptime = 2000;
	static const unsigned interval = 100;
	static const unsigned samplecount = looptime / interval;

public:
public:
	ATOM_Water (void);
	virtual ~ATOM_Water (void);

public:
	virtual void accept (ATOM_Visitor &visitor);
	virtual void setO2T(const ATOM_Matrix4x4f &Mo2t);
	bool rayIntersectionTest (ATOM_Camera *camera, const ATOM_Ray &ray, float *len) const;
	virtual bool supportFixedFunction (void) const;

protected:
	virtual void buildBoundingbox (void) const;
	virtual bool onLoad (ATOM_RenderDevice *device);

public:
	void setModelFileName (const ATOM_STRING &fileName);
	const ATOM_STRING &getModelFileName (void) const;
	ATOM_InstanceMesh *getInstanceMesh (void) const;
	ATOM_Material *getMaterial (void) const;
	void update (ATOM_Camera *camera);
	bool setupMaterial (bool gbuffer);
	ATOM_Texture *getReflectionTexture (void) const;
	ATOM_DepthBuffer *getReflectionDepth (void) const;
	void setNormalScale (float normalScale);
	float getNormalScale (void) const;
	void setSurfaceColor (const ATOM_Vector4f &color);
	const ATOM_Vector4f &getSurfaceColor (void) const;
	void setDepthColor (const ATOM_Vector4f &color);
	const ATOM_Vector4f &getDepthColor (void) const;
	void setExtinction (const ATOM_Vector4f &val);
	const ATOM_Vector4f &getExtinction (void) const;
	void setVisibility (float val);
	float getVisibility (void) const;
	void setSunScale (float val);
	float getSunScale (void) const;
	void setFadeSpeed (float val);
	float getFadeSpeed (void) const;
	void setR0 (float val);
	float getR0 (void) const;
	void setShininess (float val);
	float getShininess (void) const;
	void setRefractionStrength (float val);
	float getRefractionStrength (void) const;
	void setShoreHardness (float val);
	float getShoreHardness (void) const;
	void setFoamExistence (const ATOM_Vector3f &val);
	const ATOM_Vector3f &getFoamExistence (void) const;
	void setFoamTexture (ATOM_Texture *texture);
	ATOM_Texture *getFoamTexture (void) const;
	void setFoamTextureFileName (const ATOM_STRING &filename);
	const ATOM_STRING &getFoamTextureFileName (void) const;
	void setHeightModifier (const ATOM_Vector4f &val);
	const ATOM_Vector4f &getHeightModifier (void) const;
	void setWaveScaleX (float val);
	float getWaveScaleX (void) const;
	void setWaveScaleZ (float val);
	float getWaveScaleZ (void) const;
	void setWaveHeight (float waveHeight);
	float getWaveHeight (void) const;
	void setDisplace (float val);
	float getDisplace (void) const;
	ATOM_Texture *getHeightTexture (void) const;
	ATOM_Texture *getNormalTexture (void) const;
	bool setupWaterParameters (bool postEffect);
	void setWindDirection (const ATOM_Vector3f &dir);
	const ATOM_Vector3f &getWindDirection (void) const;
	void setWindStrength (float val);
	float getWindStrength (void) const;
	void drawStencil (ATOM_RenderDevice *device);
	void drawMesh (ATOM_RenderDevice *device, ATOM_Material *material);

private:
	void createNormalMaps (void);

private:
	ATOM_AUTOPTR(ATOM_Material) _material;
	ATOM_InstanceMesh *_mesh;
	ATOM_STRING _modelFileName;
	ATOM_STRING _foamMapFileName;
	ATOM_AUTOREF(ATOM_Model) _model;
	ATOM_AUTOREF(ATOM_Texture) _reflectionTexture;
	ATOM_AUTOREF(ATOM_DepthBuffer) _reflectionDepth;
	ATOM_AUTOREF(ATOM_Texture) _foamTexture;

	float _waveHeight;
	ATOM_Matrix4x4f _projMatrix;
	unsigned _currentNormalMap;
	ATOM_VECTOR<ATOM_AUTOREF(ATOM_Texture)> _normalmaps;
	ATOM_VECTOR<ATOM_AUTOREF(ATOM_Texture)> _heightmaps;

	float _normalScale;
	float _fadeSpeed;
	ATOM_Vector4f _depthColor;
	ATOM_Vector4f _surfaceColor;
	ATOM_Vector4f _extinction;
	ATOM_Vector3f _foamExistence;
	ATOM_Vector4f _heightModifier;
	float _visibility;
	float _sunScale;
	float _R0;
	float _refractionStrength;
	float _shoreHardness;
	float _shininess;
	float _waveScaleX;
	float _waveScaleZ;
	float _displace;
	ATOM_Vector3f _windDir;
	float _windStrength;

	ATOM_VertexDecl _stencilVertexDecl;

	ATOM_CLASS(engine, ATOM_Water, Water)
    ATOM_DECLARE_SCRIPT_INTERFACE(ATOM_Water)
	ATOM_DECLARE_NODE_FILE_TYPE(ATOM_Water, Water, "water", "ATOM3D water")
};

#endif // __ATOM3D_ENGINE_WATER_H
