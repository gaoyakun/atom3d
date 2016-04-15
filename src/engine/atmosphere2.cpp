#include "StdAfx.h"
#include "atmosphere2.h"
#include "deferredrenderscheme.h"

ATOM_SCRIPT_INTERFACE_BEGIN(ATOM_Atmosphere2)
	ATOM_ATTRIBUTES_BEGIN(ATOM_Atmosphere2)
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_Atmosphere2, "Kr", getRayleighMultiplier, setRayleighMultiplier, 0.0025f, "group=ATOM_Atmosphere2")
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_Atmosphere2, "Km", getMieMultiplier, setMieMultiplier, 0.001f, "group=ATOM_Atmosphere2")
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_Atmosphere2, "SunIntensity", getSunIntensity, setSunIntensity, 20.f, "group=ATOM_Atmosphere2")
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_Atmosphere2, "SunBrightness", getSunBrightness, setSunBrightness, 1.f, "group=ATOM_Atmosphere2")
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_Atmosphere2, "FogDensity", getFogDensity, setFogDensity, 1.f, "group=ATOM_Atmosphere2")
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_Atmosphere2, "G", getG, setG, -0.99f, "group=ATOM_Atmosphere2")
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_Atmosphere2, "Radius", getRadius, setRadius, 100.f, "group=ATOM_Atmosphere2")
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_Atmosphere2, "ScaleDepth", getRayleighScaleDepth, setRayleighScaleDepth, 0.25f, "group=ATOM_Atmosphere2")
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_Atmosphere2, "WaveLength", getWaveLength, setWaveLength, ATOM_Vector3f(0.650f,0.570f,0.475f), "group=ATOM_Atmosphere2")
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_Atmosphere2, "SunPos", getSunPos, setSunPos, ATOM_Vector3f(0.5f), "group=ATOM_Atmosphere2")
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_Atmosphere2, "Exposure", getExposure, setExposure, -2.f, "group=ATOM_Atmosphere2")
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_Atmosphere2, "ToneMap", isToneMapEnabled, enableToneMap, 0, "group=ATOM_Atmosphere2")
	ATOM_ATTRIBUTES_END()
ATOM_SCRIPT_INTERFACE_END(ATOM_Atmosphere2, ATOM_Node)

ATOM_Atmosphere2::ATOM_Atmosphere2 (void)
{
	_skyDomeVertexDecl = 0;
	_sunLight = ATOM_NEW(ATOM_Light);
	_sunLight->setLightType (ATOM_Light::Directional);
	_sunLight->setColor4f (ATOM_Vector4f (1.f));

	init ();
}

ATOM_Atmosphere2::~ATOM_Atmosphere2 (void)
{
	ATOM_DELETE(_sunLight);
}

void ATOM_Atmosphere2::init (void)
{
	_size = 32; // higher, better, more CPU/GPU
	_samples = 5; // higher, better, more CPU/GPU
	_Kr = 0.0025f;
	_Kr4PI = _Kr * 4.f * ATOM_Pi;
	_Km = 0.001f;
	_Km4PI = _Km * 4.f * ATOM_Pi;
	_ESun = 20.f;
	_sunIntensity = 2.f;
	_KrESun = _ESun * _Kr;
	_KmESun = _ESun * _Km;
	_g = -0.99f;
	_g2 = _g * _g;
	_exposure = -2.f;
	_innerRadius = 100.f;
	_innerRadius2 = _innerRadius * _innerRadius;
	_outerRadius = _innerRadius * 1.025f;
	_outerRadius2 = _outerRadius * _outerRadius;
	_scaleval = 1.f / (_outerRadius - _innerRadius);
	_rayleighScaleDepth = 0.25f;
	_scaleOverScaleDepth = _scaleval/_rayleighScaleDepth;
	_fogDensity = 1.f;
	_waveLength.x = 0.650f;
	_waveLength.y = 0.570f;
	_waveLength.z = 0.475f;
	_invWaveLength4.x = 1.f/powf(_waveLength.x, 4.f);
	_invWaveLength4.y = 1.f/powf(_waveLength.y, 4.f);
	_invWaveLength4.z = 1.f/powf(_waveLength.z, 4.f);
	_HG.x = 1.5f * ((1.f - _g2)/(2.f + _g2));
	_HG.y = 1.f + _g2;
	_HG.z = 2.f * _g;
	_eye.x = 0.f;
	_eye.y = _innerRadius + 1.0e-6f;
	_eye.z = 0.f;
	_skyBuffer = false;
	_sunLight->setDirection (ATOM_Vector3f(0.5f));

	_tonemap = false;
	_bufferUpdated = false;
	_domeUpdated = false;
}

void ATOM_Atmosphere2::setRayleighMultiplier (float Kr)
{
	if (Kr != _Kr)
	{
		_Kr = Kr;
		_Kr4PI = _Kr * 4.f * ATOM_Pi;
		_KrESun = _ESun * _Kr;

		_bufferUpdated = false;
	}
}

float ATOM_Atmosphere2::getRayleighMultiplier (void) const
{
	return _Kr;
}

void ATOM_Atmosphere2::setMieMultiplier (float Km)
{
	if (Km != _Km)
	{
		_Km = Km;
		_Km4PI = _Km * 4.f * ATOM_Pi;
		_KmESun = _ESun * _Km;

		_bufferUpdated = false;
	}
}

float ATOM_Atmosphere2::getMieMultiplier (void) const
{
	return _Km;
}

void ATOM_Atmosphere2::setSunIntensity (float intensity)
{
	if (intensity != _ESun)
	{
		_ESun = intensity;
		_KrESun = _ESun * _Kr;
		_KmESun = _ESun * _Km;

		_bufferUpdated = false;
	}
}

float ATOM_Atmosphere2::getSunIntensity (void) const
{
	return _ESun;
}

void ATOM_Atmosphere2::setG (float g)
{
	if (g != _g)
	{
		_g = g;
		_g2 = g * g;
		_HG.x = 1.5f * ((1.f - _g2)/(2.f + _g2));
		_HG.y = 1.f + _g2;
		_HG.z = 2.f * _g;

		_bufferUpdated = false;
	}
}

float ATOM_Atmosphere2::getG (void) const
{
	return _g;
}

void ATOM_Atmosphere2::setRadius (float radius)
{
	if (_innerRadius != radius)
	{
		_innerRadius = radius;
		_innerRadius2 = _innerRadius * _innerRadius;
		_outerRadius = _innerRadius * 1.025f;
		_outerRadius2 = _outerRadius * _outerRadius;
		_scaleval = 1.f / (_outerRadius - _innerRadius);
		_scaleOverScaleDepth = _scaleval/_rayleighScaleDepth;
		_eye.y = _innerRadius + 1.0e-6f;

		_bufferUpdated = false;
		_domeUpdated = false;
	}
}

float ATOM_Atmosphere2::getRadius (void) const
{
	return _innerRadius;
}

void ATOM_Atmosphere2::setRayleighScaleDepth (float rayleighScaleDepth)
{
	if (rayleighScaleDepth != _rayleighScaleDepth)
	{
		_rayleighScaleDepth = rayleighScaleDepth;
		_scaleOverScaleDepth = _scaleval/_rayleighScaleDepth;

		_bufferUpdated = false;
	}
}

float ATOM_Atmosphere2::getRayleighScaleDepth (void) const
{
	return _rayleighScaleDepth;
}

void ATOM_Atmosphere2::setWaveLength (const ATOM_Vector3f &waveLength)
{
	if (waveLength != _waveLength)
	{
		_waveLength = waveLength;

		_invWaveLength4.x = 1.f/powf(_waveLength.x, 4.f);
		_invWaveLength4.y = 1.f/powf(_waveLength.y, 4.f);
		_invWaveLength4.z = 1.f/powf(_waveLength.z, 4.f);

		_bufferUpdated = false;
	}
}

const ATOM_Vector3f &ATOM_Atmosphere2::getWaveLength (void) const
{
	return _waveLength;
}

void ATOM_Atmosphere2::setSunPos (const ATOM_Vector3f &sunPos)
{
	ATOM_Vector3f n;
	n.normalizeFrom (sunPos);

	if (n != _sunLight->getDirection())
	{
		_sunLight->setDirection (sunPos);
		_bufferUpdated = false;
	}
}

const ATOM_Vector3f &ATOM_Atmosphere2::getSunPos (void) const
{
	return _sunLight->getDirection();
}

void ATOM_Atmosphere2::setSunBrightness (float intensity)
{
	_sunIntensity = intensity;

	ATOM_Vector4f v = _sunLight->getColor4f ();
	v.w = intensity;
	_sunLight->setColor4f (v);
}

float ATOM_Atmosphere2::getSunBrightness (void) const
{
	return _sunIntensity;
}

void ATOM_Atmosphere2::setFogDensity (float density)
{
	_fogDensity = density;
}

float ATOM_Atmosphere2::getFogDensity (void) const
{
	return _fogDensity;
}

void ATOM_Atmosphere2::setExposure (float exposure)
{
	_exposure = exposure;
}

float ATOM_Atmosphere2::getExposure (void) const
{
	return _exposure;
}

void ATOM_Atmosphere2::enableToneMap (int enable)
{
	_tonemap = enable != 0;
}

int ATOM_Atmosphere2::isToneMapEnabled (void) const
{
	return _tonemap ? 1 : 0;
}

ATOM_Light *ATOM_Atmosphere2::getSunLight (void) const
{
	return _sunLight;
}

/*
bool ATOM_Atmosphere2::createInScatteringLUT (ATOM_RenderDevice *device, unsigned w, unsigned h)
{
	_inScatteringLUT = device->allocTexture (0, 0, w, h, ATOM_PIXEL_FORMAT_RGBA16F, ATOM_Texture::TEXTURE2D|ATOM_Texture::NOCOMPRESS);

	ATOM_Texture::LockedRect lrc;
	if (_inScatteringLUT->lock (0, &lrc))
	{
		unsigned short *buffer = (unsigned short*)(lrc.bits);
		float height = _innerRadius + 1e-6f;
		float radius = ATOM_sqrt(_outerRadius * _outerRadius - height * height);
		float s =  radius * 2.f / w;
		float b = -radius;
		ATOM_Vector3f vVecPos;
		ATOM_Vector3f vEye(0.f, height, 0.f);

		for (unsigned x = 0; x < w; ++x)
		{
			for (unsigned y = 0; y < h; ++y)
			{
				vVecPos.x = x * s + b;
				vVecPos.y = height;
				vVecPos.z = y * s + b;

				ATOM_Vector3f v3Pos = vVecPos;
				ATOM_Vector3f v3Ray = v3Pos - vEye;
				float fFar = v3Ray.getLength ();
				if (fFar > 0.0001f)
				{
					v3Ray /= fFar;

					ATOM_Vector3f v3Start = vEye;
					float fHeight = v3Start.getLength ();
					float fDepth = ATOM_exp(_scaleOverScaleDepth * (_innerRadius - vEye.y));
					float fStartAngle = dotProduct (v3Ray, v3Start) / fHeight;
					float fStartOffset = fDepth * scale(fStartAngle);

					float fSampleLength = fFar / _samples;
					float fScaledLength = fSampleLength * _scaleval;
					ATOM_Vector3f v3SampleRay = v3Ray * fSampleLength;
					ATOM_Vector3f v3SamplePoint = v3Start + v3SampleRay * 0.5f;

					ATOM_Vector3f v3FrontColor (0.f);
					for (unsigned i = 0; i < _samples; ++i)
					{
						if (x == _size - 1)
						{
							int a = 1;
						}

						float fHeight = v3SamplePoint.getLength();
						float fDepth = ATOM_exp(_scaleOverScaleDepth * (_innerRadius - fHeight));
						float fLightAngle = dotProduct (_sunDir, v3SamplePoint) / fHeight;
						float fCameraAngle = dotProduct (v3Ray, v3SamplePoint) / fHeight;
						float fScatter = (fStartOffset + fDepth * (scale(fLightAngle) - scale(fCameraAngle)));
						ATOM_Vector3f v3Attenuate = expv (-fScatter * (_invWaveLength4 * _Kr4PI + ATOM_Vector3f(_Km4PI)));
						v3FrontColor += v3Attenuate * (fDepth * fScaledLength);
						v3SamplePoint += v3SampleRay;
					}

					float fCos = dotProduct (_sunDir, -v3Ray);
					float fCos2 = fCos * fCos;
					float rayleighPhase = 0.75 + 0.75 * fCos2;
					ATOM_Vector3f rayleighColor = v3FrontColor * (_invWaveLength4 * _KrESun) * rayleighPhase;

					float r = ATOM_min2(rayleighColor.x, 6.5519996e4f);
					float g = ATOM_min2(rayleighColor.y, 6.5519996e4f);
					float b = ATOM_min2(rayleighColor.z, 6.5519996e4f);
					*buffer++ = ATOM_float_to_half (r);
					*buffer++ = ATOM_float_to_half (g);
					*buffer++ = ATOM_float_to_half (b);
					*buffer++ = ATOM_float_to_half (0.f);
				}
				else
				{
					*buffer++ = ATOM_float_to_half (0.f);
					*buffer++ = ATOM_float_to_half (0.f);
					*buffer++ = ATOM_float_to_half (0.f);
					*buffer++ = ATOM_float_to_half (0.f);
				}
			}
		}

		_inScatteringLUT->unlock (0);

		static int tmp = 0;
		if (tmp)
		{
			_inScatteringLUT->saveToFile ("/lut.dds");
		}

		return true;
	}

	return false;
}
*/
bool ATOM_Atmosphere2::updateSkyBuffer (ATOM_RenderDevice *device)
{
	ATOM_Texture *texture = getWrite ();
	ATOM_Texture::LockedRect lrc;

	if (texture->lock (0, &lrc))
	{
		unsigned short *buffer = (unsigned short*)(lrc.bits);

		ATOM_Vector3f HorizonSamples(0.f);
		float height = _innerRadius+1e-6f;

		for (unsigned x = 0; x <= _size; ++x)
		{
			const float fCosxz = (x < _size) ? ATOM_cos(1.f) * x / (float)(_size - 1.f) : ATOM_acos(height/_outerRadius);

			for (unsigned y = 0; y < _size; ++y)
			{
				const float fCosy =  ATOM_TwoPi * y / (float)(_size - 1.f);
				ATOM_Vector3f vVecPos;
				ATOM_Vector3f vEye(0.f, height, 0.f);
				vVecPos.x = ATOM_sin(fCosxz) * ATOM_cos(fCosy) * _outerRadius;
				vVecPos.y = ATOM_cos(fCosxz) * _outerRadius;
				vVecPos.z = ATOM_sin(fCosxz) * ATOM_sin(fCosy) * _outerRadius;

				ATOM_Vector3f v3Pos = vVecPos;
				ATOM_Vector3f v3Ray = v3Pos - vEye;
				float fFar = v3Ray.getLength ();
				v3Ray /= fFar;

				ATOM_Vector3f v3Start = vEye;
				float fHeight = v3Start.getLength ();
				float fDepth = ATOM_exp(_scaleOverScaleDepth * (_innerRadius - vEye.y));
				float fStartAngle = dotProduct (v3Ray, v3Start) / fHeight;
				float fStartOffset = fDepth * scale(fStartAngle);

				float fSampleLength = fFar / _samples;
				float fScaledLength = fSampleLength * _scaleval;
				ATOM_Vector3f v3SampleRay = v3Ray * fSampleLength;
				ATOM_Vector3f v3SamplePoint = v3Start + v3SampleRay * 0.5f;

				ATOM_Vector3f v3FrontColor (0.f);
				for (unsigned i = 0; i < _samples; ++i)
				{
					float fHeight = v3SamplePoint.getLength();
					float fDepth = ATOM_exp(_scaleOverScaleDepth * (_innerRadius - fHeight));
					float fLightAngle = dotProduct (_sunLight->getDirection(), v3SamplePoint) / fHeight;
					float fCameraAngle = dotProduct (v3Ray, v3SamplePoint) / fHeight;
					float fScatter = (fStartOffset + fDepth * (scale(fLightAngle) - scale(fCameraAngle)));
					ATOM_Vector3f v3Attenuate = expv (-fScatter * (_invWaveLength4 * _Kr4PI + ATOM_Vector3f(_Km4PI)));
					v3FrontColor += v3Attenuate * (fDepth * fScaledLength);
					v3SamplePoint += v3SampleRay;
				}

				float r = ATOM_min2(v3FrontColor.x, 6.5519996e4f);
				float g = ATOM_min2(v3FrontColor.y, 6.5519996e4f);
				float b = ATOM_min2(v3FrontColor.z, 6.5519996e4f);

				if (x < _size)
				{
					*buffer++ = ATOM_float_to_half (r);
					*buffer++ = ATOM_float_to_half (g);
					*buffer++ = ATOM_float_to_half (b);
					*buffer++ = ATOM_float_to_half (0.f);
				}
				else
				{
					float fCos = dotProduct (_sunLight->getDirection(), v3Ray);
					float rayleighPhase = 0.75f * (1.f + fCos * fCos);
					HorizonSamples.x += r * rayleighPhase;
					HorizonSamples.y += g * rayleighPhase;
					HorizonSamples.z += b * rayleighPhase;
				}
			}
		}

		HorizonSamples /= (float)_size;
		_groundc0 = HorizonSamples * _invWaveLength4 * _KrESun;
		_groundc1 = HorizonSamples * _KmESun;

		texture->unlock (0);
		_skyBuffer = !_skyBuffer;

		return true;
	}

	return false;
}

bool ATOM_Atmosphere2::update (ATOM_RenderDevice *device, const ATOM_Matrix4x4f &modelView, const ATOM_Matrix4x4f &projection)
{
	ATOM_Matrix4x4f mv = modelView;
	mv.m30 = 0.f;
	mv.m31 = _innerRadius+1.0e-6f;
	mv.m32 = 0.f;
	mv.invertAffine ();
	ATOM_Matrix4x4f mvp = projection >> mv;

	_skyMaterial->getParameterTable()->setMatrix44 ("worldViewProj", mvp);
	_skyMaterial->getParameterTable()->setFloat ("fKrESun", _KrESun);
	_skyMaterial->getParameterTable()->setFloat ("fKmESun", _KmESun);
	ATOM_Vector4f temp(float(_size), 1.f/float(_size), 0.f, 0.f);
	_skyMaterial->getParameterTable()->setVector ("Tex", temp);
	_skyMaterial->getParameterTable()->setVector ("vSunPos", _sunLight->getDirection());
	_skyMaterial->getParameterTable()->setVector ("vHG", _HG);
	_skyMaterial->getParameterTable()->setVector ("vInvWavelength", _invWaveLength4);
	_skyMaterial->getParameterTable()->setVector ("vEye", _eye);
	_skyMaterial->getParameterTable()->setFloat ("fExposure", _exposure);
	_skyMaterial->getParameterTable()->setTexture ("tSkyBuffer", getRead ());
	_skyMaterial->getParameterTable()->setInt ("HDR", _tonemap?1:0);

	return true;
}

bool ATOM_Atmosphere2::updateGround (ATOM_RenderDevice *device, const ATOM_Matrix4x4f &projection)
{
	_skyMaterial->getParameterTable()->setMatrix44 ("projection", projection);
	_skyMaterial->getParameterTable()->setVector ("groundc0", _groundc0);
	_skyMaterial->getParameterTable()->setVector ("gourndc1", _groundc1);

	return true;
}

ATOM_Material *ATOM_Atmosphere2::getMaterial (void) const
{
	return _skyMaterial.get();
}

void ATOM_Atmosphere2::accept (ATOM_Visitor &visitor)
{
	visitor.visit (*this);
}

bool ATOM_Atmosphere2::onLoad (ATOM_RenderDevice *device)
{
	_skyBufferTex = device->allocTexture (0, 0, _size, _size, ATOM_PIXEL_FORMAT_RGBA16F, ATOM_Texture::TEXTURE2D);
	_skyBackBufferTex = device->allocTexture (0, 0, _size, _size, ATOM_PIXEL_FORMAT_RGBA16F, ATOM_Texture::TEXTURE2D);
	_skyMaterial = ATOM_MaterialManager::createMaterialFromCore (device, "/materials/builtin/atmosphere2.mat");
	_domeUpdated = false;
	_bufferUpdated = false;

	return true;
}

bool ATOM_Atmosphere2::draw (ATOM_RenderDevice *device, ATOM_Camera *camera, ATOM_Material *material)
{
	createSkyDome (device, 32, 32);

	if (!_bufferUpdated)
	{
		updateSkyBuffer (device);
		//createInScatteringLUT (device, 32, 32);
		_bufferUpdated = true;
	}

	update (device, camera->getViewMatrix(), camera->getProjectionMatrix());

	_skyMaterial->getParameterTable()->setInt ("Fog", 0);
	device->setVertexDecl (_skyDomeVertexDecl);
	device->setStreamSource (0, _skyDomeVertices.get());

	unsigned numPasses = _skyMaterial->begin (device);
	for (unsigned pass = 0; pass < numPasses; ++pass)
	{
		if (_skyMaterial->beginPass (device, pass))
		{
			device->renderStreamsIndexed (_skyDomeIndices.get(), ATOM_PRIMITIVE_TRIANGLES, _skyDomeIndices->getNumIndices()/3);
			_skyMaterial->endPass (device, pass);
		}
	}
	_skyMaterial->end (device);

	ATOM_DeferredRenderScheme *renderScheme = dynamic_cast<ATOM_DeferredRenderScheme*>(ATOM_RenderScheme::getCurrentRenderScheme());
	if (renderScheme)
	{
		_skyMaterial->getParameterTable()->setInt ("Fog", 1);
		_skyMaterial->getParameterTable()->setVector ("Groundc0", _groundc0);
		_skyMaterial->getParameterTable()->setMatrix44 ("worldViewProj", ATOM_Matrix4x4f::getIdentityMatrix());
		_skyMaterial->getParameterTable()->setFloat ("fogDensity", _fogDensity * 0.0001f);
		renderScheme->drawScreenQuad (device, _skyMaterial.get());
	}


	return true;
}

bool ATOM_Atmosphere2::createSkyDome (ATOM_RenderDevice *device, unsigned cols, unsigned rows)
{
	if (_domeUpdated)
	{
		return true;
	}
	_domeUpdated = true;

	unsigned DVSize = cols * rows;
	unsigned DISize = (cols-1) * (rows-1) * 6;

	if (!_skyDomeVertexDecl)
	{
		_skyDomeVertexDecl = device->createInterleavedVertexDeclaration (ATOM_VERTEX_ATTRIB_COORD|ATOM_VERTEX_ATTRIB_TEX1_2);
	}

	if (!_skyDomeVertices)
	{
		_skyDomeVertices = device->allocVertexArray (ATOM_VERTEX_ATTRIB_COORD|ATOM_VERTEX_ATTRIB_TEX1_2, ATOM_USAGE_STATIC, DVSize, true);
	}

	struct DomeVertex
	{
		float x, y, z;
		float u, v;
	};

	DomeVertex *vertices = (DomeVertex*)_skyDomeVertices->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
	unsigned domeIndex = 0;
	for (int i = 0; i < cols; ++i)
	{
		const float MoveXZ = ATOM_cos(1.f) * i / (cols - 1);
		for (int j = 0; j < rows; ++j)
		{
			const float MoveY = ATOM_TwoPi * j / (rows - 1);

			vertices[domeIndex].x = ATOM_sin(MoveXZ) * ATOM_cos(MoveY) * _outerRadius;;
			vertices[domeIndex].y = ATOM_cos(MoveXZ) * _outerRadius;
			vertices[domeIndex].z = ATOM_sin(MoveXZ) * ATOM_sin(MoveY) * _outerRadius;
			vertices[domeIndex].u = j / (rows - 1.f);
			vertices[domeIndex].v = i / (cols - 1.f);
			domeIndex++;
		}
	}
	_skyDomeVertices->unlock ();

	if (!_skyDomeIndices)
	{
		_skyDomeIndices = device->allocIndexArray (ATOM_USAGE_STATIC, DISize, false, true);
	}

	unsigned short *indices = (unsigned short*)_skyDomeIndices->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
	for (unsigned i = 0; i < rows-1; ++i)
	{
		for (unsigned j = 0; j < cols-1; ++j)
		{
			*indices++ = i * rows + j;
			*indices++ = (i+1) * rows + j;
			*indices++ = (i+1) * rows + j + 1;
			*indices++ = (i+1) * rows + j + 1;
			*indices++ = i * rows + j + 1;
			*indices++ = i * rows + j;
		}
	}
	_skyDomeIndices->unlock ();

	return true;
}
