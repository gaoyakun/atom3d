#ifndef __ATOM3D_ENGINE_SKY_H
#define __ATOM3D_ENGINE_SKY_H

#include "../ATOM_render.h"
#include "node.h"
#include "drawable.h"

class ATOM_Sky: public ATOM_Node, public ATOM_Drawable
{
public:
	ATOM_Sky (void);
	virtual ~ATOM_Sky (void);

public:
	virtual void accept (ATOM_Visitor &visitor);

public:
    virtual bool draw (ATOM_RenderDevice *device, ATOM_Camera *camera, ATOM_Material *material);

protected:
	virtual bool onLoad(ATOM_RenderDevice *device);
	virtual void buildBoundingbox (void) const;
	virtual void buildWorldBoundingbox (void) const;

public:
	bool create (ATOM_RenderDevice *device, int gridDimX, int gridDimZ);
	void destroy (ATOM_RenderDevice *device);
	void update (ATOM_RenderDevice *device, ATOM_Camera *camera);

private:
	bool createCloud (ATOM_RenderDevice *device, int dimX, int dimZ);
	bool createGrid (ATOM_RenderDevice *device, int dimX, int dimZ);
	bool createBlur (ATOM_RenderDevice *device);
	bool createFinalCloud (ATOM_RenderDevice *device);
	bool createSkyPlane (ATOM_RenderDevice *device);
	bool prepareCloudTextures (ATOM_RenderDevice *device);
	void setMaterialParameters (ATOM_RenderDevice *device, ATOM_Camera *camera);

public:
	void setCloudTextureFileName (const ATOM_STRING &fileName);
	const ATOM_STRING &getCloudTextureFileName (void) const;
	void setCloudMaterialFileName (const ATOM_STRING &fileName);
	const ATOM_STRING &getCloudMaterialFileName (void) const;
	void setCloudCover (float cover);
	float getCloudCover (void) const;
	void setVelocity (const ATOM_Vector2f &velocity);
	const ATOM_Vector2f &getVelocity (void) const;
	void setFarClip (float farClip);
	float getFarClip (void) const;
	ATOM_Material *getMaterial (void) const;
	void setLightDir (const ATOM_Vector3f &dir);
	const ATOM_Vector3f &getLightDir (void) const;
	void setExposure (float exposure);
	float getExposure (void) const;
	void setCameraHeight (float height);
	float getCameraHeight (void) const;
	void setLightColor (const ATOM_Vector4f &color);
	const ATOM_Vector4f &getLightColor (void) const;
	void setAmbientLight (const ATOM_Vector4f &color);
	const ATOM_Vector4f &getAmbientLight (void) const;
	void setLightScale (float lightScale);
	float getLightScale (void) const;
	void setFogDensity (float density);
	float getFogDensity (void) const;
	void setRayleigh (const ATOM_Vector3f &rayleigh);
	const ATOM_Vector3f &getRayleigh (void) const;
	void setLightIntensity (float intensity);
	float getLightIntensity (void) const;
	ATOM_Light *getSunLight (void) const;
	void setFogNear (float fogNear);
	float getFogNear (void) const;
	void setFogFar (float fogFar);
	float getFogFar (void) const;
	void enableDrawSky (int enable);
	int isDrawSkyEnabled (void) const;
	//--- wangjian added ---//
	void setFogColor (const ATOM_Vector3f & color);
	const ATOM_Vector3f & getFogColor (void) const;
	void getScatterParams(ATOM_Vector4f& vRayleigh,ATOM_Vector4f& vMie,ATOM_Vector4f& vESun,ATOM_Vector4f& vSum,ATOM_Vector4f& vAmbient) const;
	//----------------------//

private:
	ATOM_AUTOREF(ATOM_Texture) _densityMap;
	ATOM_AUTOREF(ATOM_Texture) _blurredMap;
	ATOM_AUTOREF(ATOM_Texture) _cloudMap;
	ATOM_AUTOPTR(ATOM_Material) _skyMaterial;
	ATOM_AUTOREF(ATOM_VertexArray) _gridVertexArray;
	ATOM_AUTOREF(ATOM_IndexArray) _gridIndexArray;
	ATOM_VertexDecl _gridVertexDecl;
	ATOM_AUTOREF(ATOM_VertexArray) _cloudPlaneVertexArray;
	ATOM_AUTOREF(ATOM_IndexArray) _cloudPlaneIndexArray;
	ATOM_VertexDecl _cloudPlaneVertexDecl;
	ATOM_AUTOREF(ATOM_VertexArray) _skyPlaneVertexArray;
	ATOM_AUTOREF(ATOM_IndexArray) _skyPlaneIndexArray;
	ATOM_VertexDecl _skyPlaneVertexDecl;
	float _cloudCoverage;
	float _cloudHeight;
	float _falloffHeight;
	float _cameraHeight;
	float _fogDensity;
	ATOM_Vector2f _velocity;
	ATOM_Vector2f _uvOffset;
	ATOM_Vector2f _startXZ;
	ATOM_Vector2f _cellSizeXZ;
	ATOM_BBox _boundingBox;
	ATOM_STRING _cloudTextureFileName;
	ATOM_STRING _materialFileName;

	ATOM_Vector3f _lightDir;
	ATOM_Vector4f _lightColor;
	ATOM_Vector4f _ambientLight;
	ATOM_Vector3f _rayleigh;
	ATOM_Vector3f _mie;
	ATOM_Vector3f _fineRayleigh;
	float _lightScale;
	float _G;
	float _earthRadius;
	float _atmosphereHeight;
	float _height;
	float _ambientScale;
	float _farClip;
	float _exposure;
	float _fogNear;
	float _fogFar;
	bool _drawSky;

	//--- wangjian modified ---//
	ATOM_AUTOPTR(ATOM_Light) _sunLight;
	//-------------------------//



	//--- wangjian added ---//
	struct ScatteringParams
	{
		ATOM_Vector4f vRayleigh;   // rgb : 3/(16*PI) * Br           w : -2*g
		ATOM_Vector4f vMie;        // rgb : 1/(4*PI) * Bm * (1-g)^2  w : (1+g^2)
		ATOM_Vector4f vESun;       // rgb : Esun/(Br+Bm)             w : R
		ATOM_Vector4f vSum;        // rgb : (Br+Bm)                  w : h(2R+h)
		ATOM_Vector4f vAmbient;    // rgb : ambient
		ScatteringParams():vRayleigh(ATOM_Vector4f(0)),vMie(ATOM_Vector4f(0)),vESun(ATOM_Vector4f(0)),vSum(ATOM_Vector4f(0)),vAmbient(ATOM_Vector4f(0))
		{}
	};
	ScatteringParams _scatterParams;
	//---------------------//

	ATOM_CLASS(engine, ATOM_Sky, ATOM_Sky)
    ATOM_DECLARE_SCRIPT_INTERFACE(ATOM_Sky)


	//=============================== SKYBOX / SKYDOME ===========================================//
public:
	enum renderMode
	{
		SKY_SCATTERING = 0,
		SKY_BOX,
		SKY_DOME,
		SKY_TOTAL
	};

	void setRenderMode(int mode);
	int getRenderMode(void) const;

	void setSkyTextureFile(const ATOM_STRING & filename);
	const ATOM_STRING & getSkyTextureFile(void) const;

	void setSkyDomeModelFile(const ATOM_STRING & filename);
	const ATOM_STRING & getSkyDomeModelFile(void) const;

	void setSkyDomeTranslateOffset(const ATOM_Vector3f & offset);
	const ATOM_Vector3f & getSkyDomeTranslateOffset(void) const;

	void setSkyDomeScale(const ATOM_Vector3f & scale);
	const ATOM_Vector3f & getSkyDomeScale(void) const;

private:
	bool createSkyBox(ATOM_RenderDevice *device);
	bool createSkyDome(ATOM_RenderDevice *device);

	void drawSkyBox(ATOM_RenderDevice *device, ATOM_Camera *camera, ATOM_Material *material);
	void drawSkyDome(ATOM_RenderDevice *device, ATOM_Camera *camera, ATOM_Material *material);
	
	ATOM_AUTOREF(ATOM_Texture)		_skyCubeTexture;
				
	ATOM_VertexDecl					_skyBoxVertexDecl;
	ATOM_AUTOREF(ATOM_VertexArray)	_skyBoxVertexArray;

	// Ìì¿ÕÇò
	ATOM_STRING						_strSkyTextureFile;
	ATOM_STRING						_strSkyDomeModelFile;
	ATOM_AUTOREF(ATOM_Texture)		_skyDomeTexture;
	ATOM_AUTOREF(ATOM_SharedModel)	_skyDomeModel;
	ATOM_Vector3f					_skydome_translate_offset;
	ATOM_Vector3f					_skydome_scale;

	renderMode						_renderMode;
	ATOM_Vector3f					_fogColor;

	bool							_skyTexDirty;
	bool							_skyDomeModelDirty;
	//===============================================================================//
};

#endif // __ATOM3D_ENGINE_SKY_H
