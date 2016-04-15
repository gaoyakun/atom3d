#ifndef __ATOM3D_ENGINE_WATER_MATERIAL_H
#define __ATOM3D_ENGINE_WATER_MATERIAL_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_math.h"
#include "../ATOM_render.h"
#include "basedefs.h"
#include "materiallib.h"

class ATOM_WaterMaterialObject: public ATOM_MaterialObject
{
public:
	ATOM_WaterMaterialObject (void);
	virtual ~ATOM_WaterMaterialObject (void);

public:
	virtual const char *getClass (void) const;
	virtual bool load (ATOM_TiXmlElement *xml);
	virtual void enableFog (bool enable);
	virtual bool create (unsigned flags);
	virtual unsigned getVertexAttrib (void) const;
	virtual ATOM_Material *getMaterial (void);
	virtual void copyFrom (const ATOM_MaterialObject *other);
	virtual bool beginRender (ATOM_RenderDevice *device, const ATOM_Matrix4x4f *worldMatrix);
	virtual void endRender (void);

public:
	void setProjectionMVPMatrix (const ATOM_Matrix4x4f &matrix);
	const ATOM_Matrix4x4f &getProjectionMVPMatrix (void);
	void setReflectionTexture (ATOM_Texture *texture);
	void setNormalTexture (ATOM_Texture *texture);
	void setHeightTexture (ATOM_Texture *texture);
	void setWind (const ATOM_Vector2f &wind);
	void setNormalScale (float normalScale);
	void setSurfaceColor (const ATOM_Vector4f &color);
	void setDepthColor (const ATOM_Vector4f &color);
	void setSunScale (float val);
	void setWaveScale (const ATOM_Vector2f &waveScale);
	void setR0 (float val);
	void setRefractionStrength (float val);
	void setShininess (float val);
	void setWaveHeight (float val);

private:
	static bool realize (void);

protected:
	ATOM_Matrix4x4f _projectMVP;
	ATOM_AUTOREF(ATOM_Material) _material;
	ATOM_AUTOREF(ATOM_Texture) _reflectionTexture;
	ATOM_AUTOREF(ATOM_Texture) _normalTexture;
	ATOM_AUTOREF(ATOM_Texture) _heightTexture;
	ATOM_Vector2f _wind;
	float _normalScale;
	ATOM_Vector4f _depthcolor;
	ATOM_Vector4f _bigDepthColor;
	float _sunscale;
	float _R0;
	float _refractionStrength;
	float _shininess;
	float _waveHeight;
	ATOM_Vector2f _waveScale;

	static ATOM_AUTOREF(ATOM_Material) _materialNoFog;
	static ATOM_AUTOREF(ATOM_Material) _materialFog;
};

ATOM_DECLARE_MATERIALFACTORY(ATOM_WaterMaterialFactory)

#endif // __ATOM3D_ENGINE_WATER_MATERIAL_H
