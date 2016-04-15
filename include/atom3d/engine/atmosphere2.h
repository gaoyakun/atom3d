#ifndef __ATOM3D_ENGINE_ATMOSPHERE2_H
#define __ATOM3D_ENGINE_ATMOSPHERE2_H

#include "node.h"

class ATOM_Light;

class ATOM_ENGINE_API ATOM_Atmosphere2: public ATOM_Node, public ATOM_Drawable
{
	ATOM_CLASS(engine, ATOM_Atmosphere2, Atmosphere2)
	ATOM_DECLARE_SCRIPT_INTERFACE(ATOM_Atmosphere2)

public:
	ATOM_Atmosphere2 (void);
	virtual ~ATOM_Atmosphere2 (void);

public:
	void init (void);
	bool updateSkyBuffer (ATOM_RenderDevice *device);
	bool update (ATOM_RenderDevice *device, const ATOM_Matrix4x4f &modelView, const ATOM_Matrix4x4f &projection);
	bool updateGround (ATOM_RenderDevice *device, const ATOM_Matrix4x4f &projection);
	ATOM_Material *getMaterial (void) const;

public:
	virtual void accept (ATOM_Visitor &visitor);
	virtual bool draw (ATOM_RenderDevice *device, ATOM_Camera *camera, ATOM_Material *material);

protected:
	virtual bool onLoad (ATOM_RenderDevice *device);

private:
	bool createSkyDome (ATOM_RenderDevice *device, unsigned cols, unsigned rows);
	//bool createInScatteringLUT (ATOM_RenderDevice *device, unsigned w, unsigned h);

public:
	// inline methods
	float scale (float fCos) const
	{
		float x = 1.f - fCos;
		return _rayleighScaleDepth * expf(-0.00287f + x * (0.459f + x * (3.83f + x * (-6.80f + x * 5.25f))));
	}

	ATOM_Vector3f expv (const ATOM_Vector3f &v) const
	{
		return ATOM_Vector3f (expf(v.x), expf(v.y), expf(v.z));
	}

	void preRTT (ATOM_RenderDevice *device, unsigned index, ATOM_Texture *texture)
	{
		_savedRT = device->getRenderTarget (index);
		device->setRenderTarget (index, texture);
	}

	void postRTT (ATOM_RenderDevice *device, unsigned index)
	{
		device->setRenderTarget (index, _savedRT.get());
	}

	ATOM_Texture *getWrite (void) const
	{
		return _skyBuffer ? _skyBufferTex.get() : _skyBackBufferTex.get();
	}

	ATOM_Texture *getRead (void) const
	{
		return _skyBuffer ? _skyBackBufferTex.get() : _skyBufferTex.get();
	}

public:
	void setRayleighMultiplier (float Kr);
	float getRayleighMultiplier (void) const;
	void setMieMultiplier (float Km);
	float getMieMultiplier (void) const;
	void setSunIntensity (float intensity);
	float getSunIntensity (void) const;
	void setG (float g);
	float getG (void) const;
	void setRadius (float radius);
	float getRadius (void) const;
	void setRayleighScaleDepth (float rayleighScaleDepth);
	float getRayleighScaleDepth (void) const;
	void setWaveLength (const ATOM_Vector3f &waveLength);
	const ATOM_Vector3f &getWaveLength (void) const;
	void setSunPos (const ATOM_Vector3f &sunPos);
	const ATOM_Vector3f &getSunPos (void) const;
	void setSunBrightness (float intensity);
	float getSunBrightness (void) const;
	void setFogDensity (float density);
	float getFogDensity (void) const;
	void setExposure (float exposure);
	float getExposure (void) const;
	void enableToneMap (int enable);
	int isToneMapEnabled (void) const;
	ATOM_Light *getSunLight (void) const;

private:
	float _sunTheta;
	float _sunPhi;
	float _Kr;
	float _Kr4PI;
	float _Km;
	float _Km4PI;
	float _ESun;
	float _sunIntensity;
	float _KrESun;
	float _KmESun;
	float _g;
	float _g2;
	float _exposure;
	float _innerRadius;
	float _innerRadius2;
	float _outerRadius;
	float _outerRadius2;
	float _scaleval;
	float _scaleOverScaleDepth;
	float _rayleighScaleDepth;
	float _fogDensity;

	ATOM_Light *_sunLight;
	ATOM_Vector3f _invWaveLength4;
	ATOM_Vector3f _waveLength;
	ATOM_Vector3f _HG;
	ATOM_Vector3f _eye;
	ATOM_Vector3f _groundc0;
	ATOM_Vector3f _groundc1;

	bool _tonemap;
	bool _bufferUpdated;
	bool _domeUpdated;

	unsigned _size;
	unsigned _samples;
	bool _skyBuffer;
	ATOM_AUTOPTR(ATOM_Material) _skyMaterial;
	ATOM_AUTOREF(ATOM_VertexArray) _skyDomeVertices;
	ATOM_AUTOREF(ATOM_IndexArray) _skyDomeIndices;
	ATOM_VertexDecl _skyDomeVertexDecl;
	ATOM_AUTOREF(ATOM_Texture) _skyBufferTex;
	ATOM_AUTOREF(ATOM_Texture) _skyBackBufferTex;
	ATOM_AUTOREF(ATOM_Texture) _savedRT;
	//ATOM_AUTOREF(ATOM_Texture) _inScatteringLUT;
};

#endif // __ATOM3D_ENGINE_ATMOSPHERE2_H
