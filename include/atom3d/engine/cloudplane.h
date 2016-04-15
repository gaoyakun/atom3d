#ifndef __ATOM3D_ENGINE_CLOUDPLANE_H
#define __ATOM3D_ENGINE_CLOUDPLANE_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_render.h"

class ATOM_Camera;

class ATOM_CloudPlane
{
	static const int mapRes = 64;
	
public:
	enum DrawMode
	{
		DM_SM3,
		DM_SM2
	};

public:
	ATOM_CloudPlane (void);
	~ATOM_CloudPlane (void);

public:
	void setWidth (float width);
	float getWidth (void) const;
	void setDistance (float distance);
	float getDistance (void) const;
	void setDimension (int dimension);
	int getDimension (void) const;
	void setDeformSpeed (float speed);
	float getDeformSpeed (void) const;
	void setScrollVelocity (const ATOM_Vector2f &velocity);
	const ATOM_Vector2f &getScrollVelocity (void) const;
	void setDrawMode (DrawMode mode);
	ATOM_CloudPlane::DrawMode getDrawMode (void) const;
	void setSunHeight (float val);
	float getSunHeight (void) const;
	void setMoonTexture (ATOM_Texture *texture);
	ATOM_Texture *getMoonTexture(void) const;
	void setStarfieldTexture (ATOM_Texture *texture);
	ATOM_Texture *getStarfieldTexture(void) const;
	void setStarfieldTextureRepeat (float val);
	float getStarfieldTextureRepeat (void) const;

public:
	bool create (ATOM_RenderDevice *device);
	void updateDeformation (ATOM_RenderDevice *device, ATOM_Camera *camera, const ATOM_Vector3f &sunDir, float coverage);
	void draw (ATOM_RenderDevice *device, float coverage, float sharpness, const ATOM_Vector4f &color, float moonSize, const ATOM_Vector4f &moonColor, const ATOM_Vector4f &moonPos);

private:
	bool initMaterial (void);
	bool initGeometry (ATOM_RenderDevice *device);
	bool initNoiseMaps (ATOM_RenderDevice *device, float z);
	bool initColorMap (ATOM_RenderDevice *device, ATOM_Camera *camera, const ATOM_Vector3f &sunDir, float coverage);
	ATOM_Vector2f transformDirToTextureSpace (const ATOM_Vector3f &dir) const;
	ATOM_Vector2f computeMoonPosition (const ATOM_Vector4f &moonPos, const ATOM_Matrix4x4f &mvp);

private:
	DrawMode _drawMode;
	float _width;
	float _dist;
	float _z;
	float _deformSpeed;
	float _sunHeight;
	float _starfieldTextureRepeat;
	ATOM_Vector2f _scrollVelocity;
	int _dimension;
	bool _dirty;
	bool _mdirty;
	ATOM_AUTOREF(ATOM_VertexArray) _vertexArray;
	ATOM_VertexDecl _vertexDecl;
	ATOM_AUTOPTR(ATOM_Material) _materialShade;
	ATOM_AUTOPTR(ATOM_Material) _materialCombine;
	ATOM_AUTOPTR(ATOM_Material) _material;
	ATOM_AUTOPTR(ATOM_Material) _materialLow;
	ATOM_AUTOPTR(ATOM_Material) _materialMoon;
	ATOM_AUTOPTR(ATOM_Material) _materialStarfield;
	ATOM_AUTOREF(ATOM_Texture) _baseNoiseTexture[2];
	ATOM_AUTOREF(ATOM_Texture) _combinedNoiseTexture;
	ATOM_AUTOREF(ATOM_Texture) _colorTexture;
	ATOM_AUTOREF(ATOM_Texture) _moonTexture;
	ATOM_AUTOREF(ATOM_Texture) _starfieldTexture;

	ATOM_Vector2f _offset;
	unsigned _lastDrawStamp;
	unsigned _lastDrawTick;
	unsigned _lastColorUpdateStamp;
	unsigned _lastColorUpdateTick;
	unsigned char *_buffer;
	unsigned _noiseTextureIndex;
};

#endif // __ATOM3D_ENGINE_CLOUDPLANE_H
