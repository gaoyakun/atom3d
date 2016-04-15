#include "StdAfx.h"
#include "water.h"
#include "posteffect_water.h"
#include "instancemesh.h"

static const char waterMaterialSrcFF[] = 
  "material {																	\n"
  "  effect {																	\n"
  "   source  \"																\n"
  "		float4 colorAndTrans;						\n"
  "     technique t0 {\n"
  "       pass P0 {\n"
  "			TextureFactor = colorAndTrans;	\n"
  "         ColorOp[0] = Modulate;\n"
  "			ColorArg1[0] = Diffuse;\n"
  "			ColorArg2[0] = TFactor;\n"
  "			AlphaOp[0] = Modulate;\n"
  "			AlphaArg1[0] = TFactor;\n"
  "			AlphaBlendEnable = True;\n"
  "			SrcBlend = SrcAlpha;\n"
  "			DestBlend = InvSrcAlpha;\n"
  "			ZWriteEnable = False;\n"
  "         VertexShader = null;\n"
  "         PixelShader = null;\n"
  "       }\n"
  "     }\";\n"
  "  }\n"
  "}";

static const char shaderSrcStencilPass[] = 
  "material {																	\n"
  "  effect {																	\n"
  "   source  \"																\n"
  "		float4x4 MVPmatrix;														\n"
  "		float waveHeight;														\n"
  "		struct VS_INPUT															\n"
  "		{																		\n"
  "			float3 position: POSITION0;											\n"
  "		};																		\n"
  "		struct VS_OUTPUT														\n"
  "		{																		\n"
  "			float4 position: POSITION0;											\n"
  "		};																		\n"
  "		struct PS_OUTPUT														\n"
  "		{																		\n"
  "			float4 color : COLOR0;												\n"
  "		};																		\n"
  "		void vs (in VS_INPUT IN, out VS_OUTPUT OUT)								\n"
  "		{																		\n"
  "			float3 p = IN.position + float3(0.0, waveHeight, 0.0);				\n"
  "			OUT.position = mul(float4(p, 1.0), MVPmatrix);						\n"
  "		}																		\n"
  "		void ps(in VS_OUTPUT IN, out PS_OUTPUT OUT)								\n"
  "		{																		\n"
  "			OUT.color = float4(1,1,1,1);										\n"
  "		}																		\n"
  "     technique t0 {\n"
  "       pass P0 {\n"
  "         ColorOp[0] = Disable;\n"
  "			StencilEnable = True;\n"
  "			StencilWriteMask = 0x7F;\n"
  "			StencilPass = Replace;\n"
  "			StencilFunc = Always;\n"
  "			StencilRef = 2;\n"
  "			ZWriteEnable = False;\n"
  "         VertexShader = compile vs_1_1 vs();\n"
  "         PixelShader = compile ps_2_0 ps();\n"
  "       }\n"
  "     }\";\n"
  "		param MVPmatrix matrix44f MVPMATRIX;\n"
  "  }\n"
  "}";

ATOM_SCRIPT_INTERFACE_BEGIN(ATOM_Water)
  ATOM_ATTRIBUTES_BEGIN(ATOM_Water)
	ATOM_ATTRIBUTE_PERSISTENT_NODEFAULT(ATOM_Water, "ModelFileName", getModelFileName, setModelFileName, "group=ATOM_Water;type=vfilename;desc='VFS water file name")
	ATOM_ATTRIBUTE_PERSISTENT_NODEFAULT(ATOM_Water, "FoamTexture", getFoamTextureFileName, setFoamTextureFileName, "group=ATOM_Water;type=vfilename;desc='VFS foam texture file name")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_Water, "FoamExistence", getFoamExistence, setFoamExistence, ATOM_Vector3f(0.65f, 1.35f, 0.5f), "group=ATOM_Water")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_Water, "Shininess", getShininess, setShininess, 64.f, "group=ATOM_Water;min=1;max=512;step=1")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_Water, "WindDir", getWindDirection, setWindDirection, ATOM_Vector3f(0.f, 1.f, 0.f), "group=ATOM_Water;type=dir")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_Water, "WindStrength", getWindStrength, setWindStrength, 1.f, "group=ATOM_Water;step=0.1")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_Water, "WaveScaleX", getWaveScaleX, setWaveScaleX, 0.004f, "group=ATOM_Water;min=0.0001;max=100;step=0.0001")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_Water, "WaveScaleZ", getWaveScaleZ, setWaveScaleZ, 0.004f, "group=ATOM_Water;min=0.0001;max=100;step=0.0001")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_Water, "WaveHeight", getWaveHeight, setWaveHeight, 1.00f, "group=ATOM_Water;min=0.0001;max=50;step=0.0001")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_Water, "FadeSpeed", getFadeSpeed, setFadeSpeed, 0.15f, "group=ATOM_Water;min=0;max=10;step=0.001")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_Water, "Displace", getDisplace, setDisplace, 1.7f, "group=ATOM_Water;min=0;max=100;step=0.01")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_Water, "NormalScale", getNormalScale, setNormalScale, 32.f, "group=ATOM_Water;min=0.1;max=100;step=0.1")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_Water, "SurfaceColor", getSurfaceColor, setSurfaceColor, ATOM_Vector4f(0.0078f, 0.5176f, 0.7f, 1.f), "group=ATOM_Water;type=rgba")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_Water, "DepthColor", getDepthColor, setDepthColor, ATOM_Vector4f(0.0039f, 0.00196f, 0.145f, 1.f), "group=ATOM_Water;type=rgba")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_Water, "Visibility", getVisibility, setVisibility, 4.f, "group=ATOM_Water;min=0;max=1000;step=0.1")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_Water, "SunScale", getSunScale, setSunScale, 3.f, "group=ATOM_Water;min=0;max=100;step=0.1")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_Water, "RefractionStrength", getRefractionStrength, setRefractionStrength, 0.3f, "group=ATOM_Water;min=-1;max=1;step=0.01")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_Water, "ShoreHardness", getShoreHardness, setShoreHardness, 1.f, "group=ATOM_Water;min=0.01;max=100;step=0.01")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_Water, "Extinction", getExtinction, setExtinction, ATOM_Vector4f(7.0f, 30.0f, 40.0f, 1.f), "group=ATOM_Water")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_Water, "HeightModifier", getHeightModifier, setHeightModifier, ATOM_Vector4f(16.f, 8.f, 0.1f, 0.2f), "group=ATOM_Water")
  ATOM_ATTRIBUTES_END()
ATOM_SCRIPT_INTERFACE_END(ATOM_Water, ATOM_Node)

ATOM_IMPLEMENT_NODE_FILE_TYPE(ATOM_Water)

class AutoCreateCache
{
public:
	static const int DROP_COUNT = 160;

	AutoCreateCache (void)
	{
		ATOM_STACK_TRACE(AutoCreateCache::AutoCreateCache);

		needCreate = true;
	}

	void generate (float scale)
	{
		ATOM_STACK_TRACE(AutoCreateCache::generate);

		if (needCreate)
		{
			needCreate = false;
			create (ATOM_GetTick());
		}
	}

	void create (unsigned seed)
	{
		ATOM_STACK_TRACE(AutoCreateCache::create);

		srand (seed);

		int x0[DROP_COUNT];
		int y0[DROP_COUNT];
		float a[DROP_COUNT];
		float omega[DROP_COUNT];
		int lambda[DROP_COUNT];
		int reduce[DROP_COUNT];
		for (unsigned n = 0; n < DROP_COUNT; ++n)
		{
			x0[n] = ATOM_ftol((ATOM_Water::tilesize - 1) * (float(rand()) / float(RAND_MAX)));
			y0[n] = ATOM_ftol((ATOM_Water::tilesize - 1) * (float(rand()) / float(RAND_MAX)));
			a[n] = 1 + (float(rand()) / float(RAND_MAX));
			omega[n] = 2 * ATOM_Pi * (float(rand()) / float(RAND_MAX));
			lambda[n] = ATOM_ftol(3 + 8 * (float(rand()) / float(RAND_MAX)));
			reduce[n] = ATOM_ftol(40 + 30 * (float(rand()) / float(RAND_MAX)));
		}

		memset (&noiseCache[0][0][0], 0, sizeof(float) * ATOM_Water::samplecount * ATOM_Water::tilesize * ATOM_Water::tilesize);
		amplitude = 0.f;

		for (unsigned frame = 0; frame < ATOM_Water::samplecount; ++frame)
		{
			for (unsigned n = 0; n < DROP_COUNT; ++n)
			{
				float offset = ATOM_TwoPi * float(ATOM_Water::samplecount - 1 - frame) / float(ATOM_Water::samplecount);
				float *p1 = &noiseCache[frame][0][0];
				for (unsigned i = 0; i < ATOM_Water::tilesize; ++i)
				{
					for (unsigned j = 0; j < ATOM_Water::tilesize; ++j)
					{
						float f = *p1;
						int nx = (reduce[n] - j + ATOM_Water::tilesize - 1) / ATOM_Water::tilesize;
						int px = (reduce[n] + j - 1  ) / ATOM_Water::tilesize;
						int ny = (reduce[n] - i + ATOM_Water::tilesize - 1) / ATOM_Water::tilesize;
						int py = (reduce[n] + i - 1  ) / ATOM_Water::tilesize;
						for (int s = -nx; s <= px; ++s)
						{
							for (int t = -ny; t <= py; ++t)
							{
								float x1 = x0[n] + s * ATOM_Water::tilesize;
								float y1 = y0[n] + t * ATOM_Water::tilesize; 
								float d = ATOM_sqrt ((j - x1) * (j - x1) + (i - y1) * (i - y1));
								if (d < reduce[n])
									f += a[n] * (1.f - d / reduce[n]) * ATOM_sin (omega[n] + offset + ATOM_TwoPi * d / float(lambda[n]));
							}
						}

						if (f > amplitude)
						{
							amplitude = f;
						}

						*p1++ = f;
					}
				}
			}
		}
	}

	float noiseCache[ATOM_Water::samplecount][ATOM_Water::tilesize][ATOM_Water::tilesize];

private:
	bool needCreate;

public:
	float amplitude;
};

static void calcCachedNormals (const float *heights, int tilesize, unsigned char *normalmap) 
{
	ATOM_STACK_TRACE(calcCachedNormals);

# define WATER_VERTEX(x,z) (heights + (z) * tilesize + (x))

	// Using raw pointer instead of operator [] of vector to increase speed.
	const float *coords = heights;

	// Temporal variables
	const float *vcenter;

	// Compute inner normals
	for (unsigned i = 0; i < tilesize; ++i)
	{
		for (unsigned j = 0; j < tilesize; ++j)
		{
			vcenter = WATER_VERTEX(j, i);
			ATOM_Vector3f n;

			float h1 = (j == tilesize-1) ? *WATER_VERTEX(0, i) : *(vcenter + 1);
			float h2 = (i == 0) ? *WATER_VERTEX(j, tilesize-1) : *(vcenter - tilesize);
			float h3 = (j == 0) ? *WATER_VERTEX(tilesize-1, i) : *(vcenter - 1);
			float h4 = (i == tilesize-1) ? *WATER_VERTEX(j, 0) : *(vcenter + tilesize);

			n.set(h3 - h1, 2, h2 - h4);
			n.normalize();

			*normalmap++ = ATOM_ftol(255 * (n.z * 0.5f + 0.5f));
			*normalmap++ = ATOM_ftol(255 * (n.y * 0.5f + 0.5f));
			*normalmap++ = ATOM_ftol(255 * (n.x * 0.5f + 0.5f));
		}
    }
# undef WATER_VERTEX
}

ATOM_Water::ATOM_Water (void)
{
	ATOM_STACK_TRACE(ATOM_Water::ATOM_Water);

	_mesh = 0;
	_windDir.set (0.f, 1.f, 0.f);
	_windStrength = 1.f;
	_normalScale = 32.f;
	_surfaceColor.set(0.0078f, 0.5176f, 0.7f, 1.f);
	_depthColor.set(0.0039f, 0.00196f, 0.145f, 1.f);
	_extinction.set(1.0f, 30.0f/7.f, 40.0f/7.f, 1.f);
	_visibility = 4.f;
	_sunScale = 3.f;
	_waveHeight = 1.f;
	_R0 = 0.5f;
	_shininess = 64.f;
	_refractionStrength = 0.f;
	_shoreHardness = 1.f;
	_waveScaleX = 0.006f;
	_waveScaleZ = 0.006f;
	_fadeSpeed = 0.15f;
	_displace = 1.7f;
	_foamExistence.set (0.65f, 1.35f, 0.5f);
	_heightModifier.set (16.f, 8.f, 0.1f, 0.2f);
}

ATOM_Water::~ATOM_Water (void)
{
	ATOM_STACK_TRACE(ATOM_Water::~ATOM_Water);

	ATOM_DELETE(_mesh);
}

void ATOM_Water::accept (ATOM_Visitor &visitor)
{
	ATOM_STACK_TRACE(ATOM_Water::accept);

	visitor.visit (*this);
}

void ATOM_Water::buildBoundingbox (void) const
{
	if (_mesh)
	{
		_boundingBox = _mesh->getMesh()->boundingBox;
	}
	else
	{
		_boundingBox.setMin (ATOM_Vector3f(0, 0, 0));
		_boundingBox.setMax (ATOM_Vector3f(0, 0, 0));
	}
}

bool ATOM_Water::onLoad (ATOM_RenderDevice *device)
{
	return false;

	ATOM_STACK_TRACE(ATOM_Water::onLoad);

	ATOM_DELETE (_mesh);
	_mesh = 0;

	if (!getNodeFileName().empty ())
	{
		_modelFileName = getNodeFileName();
	}

	if (!_modelFileName.empty ())
	{
		//--- wangjian modified ---//
		// Òì²½¼ÓÔØ
#if 0
		ATOM_Components comp (_modelFileName.c_str(),false);
#else
		ATOM_Components comp (_modelFileName.c_str(),ATOM_LoadPriority_IMMEDIATE);
#endif
		//-------------------------//

		if (comp.isValid ())
		{
			_model = comp.getModel ();
			if (_model->getNumMeshes () > 0)
			{
				_mesh = ATOM_NEW (ATOM_InstanceMesh, (ATOM_Node*)0, _model->getMesh (0));
				_material = ATOM_MaterialManager::createMaterialFromCore (device, "/materials/builtin/water.mat");
				if (!_material)
				{
					return false;
				}

				createNormalMaps ();

				_reflectionTexture = device->allocTexture (0, 0, 512, 512, ATOM_PIXEL_FORMAT_BGRA8888, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET); 
				_reflectionDepth = device->allocDepthBuffer (512, 512);
				
				_stencilVertexDecl = device->createMultiStreamVertexDeclaration (ATOM_VERTEX_ATTRIB_COORD);

				return true;
			}
		}
	}

	return false;
}

void ATOM_Water::setModelFileName (const ATOM_STRING &fileName)
{
	ATOM_STACK_TRACE(ATOM_Water::setModelFileName);

	_modelFileName = fileName;
}

const ATOM_STRING &ATOM_Water::getModelFileName (void) const
{
	return _modelFileName;
}

ATOM_InstanceMesh *ATOM_Water::getInstanceMesh (void) const
{
	return _mesh;
}

ATOM_Material *ATOM_Water::getMaterial (void) const
{
	return _material.get();
}

void ATOM_Water::createNormalMaps (void)
{
	ATOM_STACK_TRACE(ATOM_Water::createNormalMaps);

	static AutoCreateCache cache;

	cache.generate (_waveHeight);

	if (_normalmaps.empty ())
	{
		_normalmaps.resize (samplecount);

		unsigned char normals[tilesize * tilesize * 3];
		for (unsigned i = 0; i < samplecount; ++i)
		{
			calcCachedNormals (&cache.noiseCache[i][0][0], tilesize, normals);
			_normalmaps[i] = ATOM_GetRenderDevice()->allocTexture (0, normals, tilesize, tilesize, ATOM_PIXEL_FORMAT_BGR888, ATOM_Texture::TEXTURE2D|ATOM_Texture::NOCOMPRESS);
		}
	}

	if (_heightmaps.empty ())
	{
		_heightmaps.resize (samplecount);

		float *heightCache = ATOM_NEW_ARRAY(float, tilesize * tilesize);
		float amplitude = cache.amplitude;
		for (unsigned i = 0; i < samplecount; ++i)
		{
			float *dst = heightCache;
			float *src = &cache.noiseCache[i][0][0];
			for (unsigned j = 0; j < tilesize * tilesize; ++j)
			{
				*dst++ = (*src++) / amplitude;
			}
			_heightmaps[i] = ATOM_GetRenderDevice()->allocTexture (0, heightCache, tilesize, tilesize, ATOM_PIXEL_FORMAT_R32F, ATOM_Texture::TEXTURE2D|ATOM_Texture::NOCOMPRESS);
		}
		ATOM_DELETE_ARRAY(heightCache);
	}
}

void ATOM_Water::update (ATOM_Camera *camera)
{
	ATOM_STACK_TRACE(ATOM_Water::update);

	ATOM_Vector4f reflectionPlane = ATOM_Vector4f (0.f, 1.f, 0.f, -getWorldBoundingbox ().getCenter().y);
	ATOM_Matrix4x4f reflectionMatrix = ATOM_Matrix4x4f::getReflectionMatrix (reflectionPlane);
	_projMatrix = reflectionMatrix;
	_projMatrix <<= camera->getInvViewMatrix ();
	_projMatrix <<= camera->getProjectionMatrix();
	
	ATOM_Matrix4x4f adj;
	float offs = 0.5f;
	adj.setRow (0, 0.5f,  0.0f, 0.0f, 0.0f);
	adj.setRow (1, 0.0f,  -0.5f, 0.0f, 0.0f);
	adj.setRow (2, 0.0f,  0.0f,	1.0f, 0.0f);
	adj.setRow (3, offs,  offs, 0.0f, 1.0f);
	_projMatrix <<= adj;

	_currentNormalMap = (ATOM_APP->getFrameStamp().currentTick % looptime) / interval;
}

bool ATOM_Water::rayIntersectionTest (ATOM_Camera *camera, const ATOM_Ray &ray, float *len) const
{
	if (_model && _model->getAABBTree ())
	{
		float d;
		if (len)
		{
			*len = FLT_MAX;

			if (_model->getAABBTree()->rayIntersect (ray, d) >= 0)
			{
				if (d < *len)
				{
					*len = d;
				}
				return true;
			}
		}
		else
		{
			if (_model->getAABBTree()->rayIntersectionTest (ray))
			{
				return true;
			}
		}
	}
	else
	{
		if (!len)
		{
			return ray.intersectionTest (getBoundingbox ());
		}
		else
		{
			return ray.intersectionTestEx (getBoundingbox (), *len);
		}
	}

	return false;
}

void ATOM_Water::setWaveScaleX (float f)
{
	_waveScaleX = f;
}

float ATOM_Water::getWaveScaleX (void) const
{
	return _waveScaleX;
}

void ATOM_Water::setWaveScaleZ (float f)
{
	_waveScaleZ = f;
}

float ATOM_Water::getWaveScaleZ (void) const
{
	return _waveScaleZ;
}

void ATOM_Water::setDisplace (float val)
{
	_displace = val;
}

float ATOM_Water::getDisplace (void) const
{
	return _displace;
}

void ATOM_Water::setNormalScale (float normalScale)
{
	_normalScale = normalScale;
}

float ATOM_Water::getNormalScale (void) const
{
	return _normalScale;
}

void ATOM_Water::setSurfaceColor (const ATOM_Vector4f &color)
{
	_surfaceColor = color;
}

const ATOM_Vector4f &ATOM_Water::getSurfaceColor (void) const
{
	return _surfaceColor;
}

void ATOM_Water::setDepthColor (const ATOM_Vector4f &color)
{
	_depthColor = color;
}

const ATOM_Vector4f &ATOM_Water::getDepthColor (void) const
{
	return _depthColor;
}

ATOM_Texture *ATOM_Water::getReflectionTexture (void) const
{
	return _reflectionTexture.get();
}

ATOM_DepthBuffer *ATOM_Water::getReflectionDepth (void) const
{
	return _reflectionDepth.get();
}

void ATOM_Water::setExtinction (const ATOM_Vector4f &val)
{
	_extinction = val;
}

const ATOM_Vector4f &ATOM_Water::getExtinction (void) const
{
	return _extinction;
}

void ATOM_Water::setVisibility (float val)
{
	_visibility = val;
}

float ATOM_Water::getVisibility (void) const
{
	return _visibility;
}

void ATOM_Water::setSunScale (float val)
{
	_sunScale = val;
}

float ATOM_Water::getSunScale (void) const
{
	return _sunScale;
}

void ATOM_Water::setR0 (float val)
{
	_R0 = val;
}

float ATOM_Water::getR0 (void) const
{
	return _R0;
}

void ATOM_Water::setRefractionStrength (float val)
{
	_refractionStrength = val;
}

float ATOM_Water::getRefractionStrength (void) const
{
	return _refractionStrength;
}

void ATOM_Water::setShoreHardness (float val)
{
	_shoreHardness = val;
}

float ATOM_Water::getShoreHardness (void) const
{
	return _shoreHardness;
}

void ATOM_Water::setFoamExistence (const ATOM_Vector3f &val)
{
	_foamExistence = val;

	if (_foamExistence.x >= _foamExistence.y)
	{
		_foamExistence.y = _foamExistence.x + 0.1f;
	}
}

const ATOM_Vector3f &ATOM_Water::getFoamExistence (void) const
{
	return _foamExistence;
}

void ATOM_Water::setFoamTextureFileName (const ATOM_STRING &filename)
{
	ATOM_STACK_TRACE(ATOM_Water::setFoamTextureFileName);

	ATOM_AUTOREF(ATOM_Texture) tex = ATOM_CreateTextureResource (filename.c_str());

	if (tex)
	{
		_foamTexture = tex;
		_foamMapFileName = filename;
	}
}

const ATOM_STRING &ATOM_Water::getFoamTextureFileName (void) const
{
	return _foamMapFileName;
}

void ATOM_Water::setFoamTexture (ATOM_Texture *texture)
{
	if (texture != _foamTexture)
	{
		_foamTexture = texture;
		_foamMapFileName = "";
	}
}

ATOM_Texture *ATOM_Water::getFoamTexture (void) const
{
	return _foamTexture.get();
}

void ATOM_Water::setHeightModifier (const ATOM_Vector4f &val)
{
	_heightModifier = val;
}

const ATOM_Vector4f &ATOM_Water::getHeightModifier (void) const
{
	return _heightModifier;
}

void ATOM_Water::setShininess (float val)
{
	_shininess = val;
}

float ATOM_Water::getShininess (void) const
{
	return _shininess;
}

ATOM_Texture *ATOM_Water::getHeightTexture (void) const
{
	return _heightmaps[_currentNormalMap].get();
}

ATOM_Texture *ATOM_Water::getNormalTexture (void) const
{
	return _normalmaps[_currentNormalMap].get();
}

bool ATOM_Water::setupWaterParameters (bool postEffect)
{
#if 1
	return false;
#else
	ATOM_STACK_TRACE(ATOM_Water::setupWaterParameters);

	if (postEffect)
	{
		if (_material)
		{
			_material->getParameterTable()->setFloat ("waveHeight", _waveHeight);

			PostEffectWater::addWater (this);
			PostEffectWater::setReflectionTexture (_reflectionTexture.get());
			PostEffectWater::setFoamTexture (_foamTexture ? _foamTexture.get() : ATOM_GetColorTexture(0xFFFFFFFF));
			PostEffectWater::setNormalTexture (_normalmaps[_currentNormalMap].get());
			PostEffectWater::setHeightTexture (_heightmaps[_currentNormalMap].get());
			PostEffectWater::setWaterLevel (getWorldMatrix ().m31);
			PostEffectWater::setWaveHeight (_waveHeight);
			PostEffectWater::setTextureProjMatrix (_projMatrix);
			PostEffectWater::setLightDir (dynamic_cast<ATOM_SDLScene*>(ATOM_Scene::getCurrentScene())->getLight()->getDirection());
			PostEffectWater::setFresnel (_refractionStrength);
			PostEffectWater::setWaveScale (ATOM_Vector2f(_waveScaleX, _waveScaleZ));
			PostEffectWater::setShoreHardness (_shoreHardness);
			PostEffectWater::setNormalScale (_normalScale);
			PostEffectWater::setFadeSpeed (_fadeSpeed);
			PostEffectWater::setDisplace (_displace);
			PostEffectWater::setVisibility (_visibility);
			PostEffectWater::setWind (ATOM_Vector2f(_windDir.x, _windDir.z) * _windStrength);
			PostEffectWater::setSurfaceColor (_surfaceColor);
			PostEffectWater::setDepthColor (_depthColor);
			PostEffectWater::setSunScale (_sunScale);
			PostEffectWater::setExtinction (_extinction);
			PostEffectWater::setHeightModifier (_heightModifier);
			PostEffectWater::setFoamExistence (_foamExistence);
			PostEffectWater::setShininess(_shininess);
			PostEffectWater::requireRender ();

			return true;
		}
	}
	else
	{
		/*
		ATOM_Material *m = _material.get();
		if (m)
		{
			ATOM_MaterialEffect *effect = m->getEffect ("non_posteffect");

			m->setProjectionMVPMatrix (_projMatrix);
			m->setReflectionTexture (_reflectionTexture.get());
			m->setNormalTexture (_normalmaps[_currentNormalMap].get());
			m->setHeightTexture (_heightmaps[_currentNormalMap].get());
			m->setWind (ATOM_Vector2f(_windDir.x, _windDir.z) * _windStrength);
			m->setNormalScale (_normalScale);
			m->setSurfaceColor (_surfaceColor);
			m->setDepthColor (_depthColor);
			m->setSunScale (_sunScale);
			m->setR0 (_R0);
			m->setShininess (_shininess);
			m->setRefractionStrength (_refractionStrength);
			m->setWaveScale (ATOM_Vector2f(_waveScaleX, _waveScaleZ));
			return true;
		}
		*/
	}

	return false;
#endif
}

void ATOM_Water::setFadeSpeed (float val)
{
	_fadeSpeed = val;
}

float ATOM_Water::getFadeSpeed (void) const
{
	return _fadeSpeed;
}

void ATOM_Water::setWaveHeight (float waveHeight)
{
	_waveHeight = waveHeight;
}

float ATOM_Water::getWaveHeight (void) const
{
	return _waveHeight;
}

void ATOM_Water::setWindDirection (const ATOM_Vector3f &dir)
{
	_windDir = dir;
}

const ATOM_Vector3f &ATOM_Water::getWindDirection (void) const
{
	return _windDir;
}

void ATOM_Water::setWindStrength (float val)
{
	_windStrength = val;
}

float ATOM_Water::getWindStrength (void) const
{
	return _windStrength;
}

void ATOM_Water::setO2T(const ATOM_Matrix4x4f &Mo2t)
{
	ATOM_Matrix4x4f rotation;
	ATOM_Vector3f scale;
	ATOM_Vector3f translation;
	Mo2t.decompose (translation, rotation, scale);
	scale.y = 1.f;
	ATOM_Matrix4x4f m = ATOM_Matrix4x4f::getTranslateMatrix(translation) >> ATOM_Matrix4x4f::getScaleMatrix(scale);
	ATOM_Node::setO2T (m);
}

void ATOM_Water::drawMesh (ATOM_RenderDevice *device, ATOM_Material *material)
{
	ATOM_STACK_TRACE(ATOM_Water::drawMesh);

	if (_mesh && _mesh->getMesh())
	{
		ATOM_VertexArray *va = _mesh->getMesh()->vertices.get();
		if (!va)
		{
			va = _mesh->getMesh()->interleavedarray.get();
		}
			
		ATOM_IndexArray *ia = _mesh->getMesh()->indices.get();
		if (va && ia)
		{
			device->setVertexDecl (_stencilVertexDecl);
			device->setStreamSource (0, va);
			device->setTransform (ATOM_MATRIXMODE_WORLD, getWorldMatrix ());

			unsigned numPasses = material->begin (device);
			for (unsigned pass = 0; pass < numPasses; ++pass)
			{
				if (material->beginPass (device, pass))
				{
					device->renderStreamsIndexed (ia, ATOM_PRIMITIVE_TRIANGLES, ia->getNumIndices() / 3, 0);
					material->endPass (device, pass);
				}
			}
			material->end (device);
		}
	}
}

void ATOM_Water::drawStencil (ATOM_RenderDevice *device)
{
	ATOM_STACK_TRACE(ATOM_Water::drawStencil);

	drawMesh (device, _material.get());
}

bool ATOM_Water::supportFixedFunction (void) const
{
	return true;
}

