#include "StdAfx.h"
#include "sky.h"

ATOM_SCRIPT_INTERFACE_BEGIN(ATOM_Sky)
  ATOM_ATTRIBUTES_BEGIN(ATOM_Sky)
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_Sky, "DrawSky", isDrawSkyEnabled, enableDrawSky, 1, 0)
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_Sky, "CloudTextureFileName", getCloudTextureFileName, setCloudTextureFileName, "/materials/builtin/cloud.png", 0)
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_Sky, "CloudMaterialFileName", getCloudMaterialFileName, setCloudMaterialFileName, "/materials/builtin/sky.mat", 0)
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_Sky, "CloudCover", getCloudCover, setCloudCover, 0.4f, 0)
    ATOM_ATTRIBUTE_PERSISTENT(ATOM_Sky, "Velociy", getVelocity, setVelocity, ATOM_Vector2f(0.01f, 0.01f), 0)
    ATOM_ATTRIBUTE_PERSISTENT(ATOM_Sky, "FarClip", getFarClip, setFarClip, 40000.f, 0)
    ATOM_ATTRIBUTE_PERSISTENT(ATOM_Sky, "LightDir", getLightDir, setLightDir, ATOM_Vector3f(0.f, -1.f, 0.f), 0)
    ATOM_ATTRIBUTE_PERSISTENT(ATOM_Sky, "Exposure", getExposure, setExposure, 1.f, 0)
    ATOM_ATTRIBUTE_PERSISTENT(ATOM_Sky, "CameraHeight", getCameraHeight, setCameraHeight, 5000.f, 0)
    ATOM_ATTRIBUTE_PERSISTENT(ATOM_Sky, "LightColor", getLightColor, setLightColor, ATOM_Vector4f(1.f, 1.f, 1.f, 1.f), 0)
    ATOM_ATTRIBUTE_PERSISTENT(ATOM_Sky, "LightScale", getLightScale, setLightScale, 1.f, 0)
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_Sky, "LightIntensity", getLightIntensity, setLightIntensity, 1.f, 0)
    ATOM_ATTRIBUTE_PERSISTENT(ATOM_Sky, "AmbientLight", getAmbientLight, setAmbientLight, ATOM_Vector4f(0.3f, 0.35f, 0.4f, 1.f), 0)
    ATOM_ATTRIBUTE_PERSISTENT(ATOM_Sky, "FogDensity", getFogDensity, setFogDensity, 0.0004f, 0)
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_Sky, "FogNear", getFogNear, setFogNear, 500.f, 0)
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_Sky, "FogFar", getFogFar, setFogFar, 3000.f, 0)
    ATOM_ATTRIBUTE_PERSISTENT(ATOM_Sky, "Rayleigh", getRayleigh, setRayleigh, ATOM_Vector3f(0.05f, 0.15f, 1.5f), 0)
	//--- wangjian added ---//
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_Sky, "FogColor", getFogColor, setFogColor, ATOM_Vector3f(1.f, 1.f, 1.f), 0)
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_Sky, "SkyRenderMode", getRenderMode, setRenderMode, ATOM_Sky::SKY_DOME, 0)
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_Sky, "SkyTextureFileName", getSkyTextureFile, setSkyTextureFile, "", 0)
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_Sky, "SkyDomeModelFileName", getSkyDomeModelFile, setSkyDomeModelFile, "", 0)
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_Sky, "SkyDomeOffset", getSkyDomeTranslateOffset, setSkyDomeTranslateOffset, ATOM_Vector3f(0.f, 0.f, 0.f), 0)
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_Sky, "SkyDomeScale", getSkyDomeScale, setSkyDomeScale, ATOM_Vector3f(1.f, 1.f, 1.f), 0)
	//----------------------//
  ATOM_ATTRIBUTES_END()
ATOM_SCRIPT_INTERFACE_END(ATOM_Sky, ATOM_Node)

ATOM_Sky::ATOM_Sky (void)
{
	_enableGeoClipping = false;
	_gridVertexDecl = 0;
	_cloudPlaneVertexDecl = 0;
	_skyPlaneVertexDecl = 0;
	_cloudCoverage = 0.5f;
	_cloudHeight = 0.f;
	_startXZ.set (0.f, 0.f);
	_cellSizeXZ.set (0.f, 0.f);
	_velocity.set (0.01f, 0.01f);
	_uvOffset.set (0.f, 0.f);
	_lightDir.set (0.f, -1.f, 0.f);
	_lightColor.set (1.f, 1.f, 1.f, 1.f);
	_ambientLight.set (0.3f, 0.35f, 0.4f, 1.f);
	_rayleigh.set (0.3f, 0.45f, 0.6f);
	_mie.set (0.3f, 0.3f, 0.3f);
	_G = 0.7f;
	_lightScale = 1.f;
	_earthRadius = 21600000.f;
	_atmosphereHeight = 30000.f;
	_height = 600.f;
	_ambientScale = 0.1f;
	_farClip = 40000.f;
	_exposure = 1.f;
	_cameraHeight = 5000.f;
	_fogDensity = 0.0004f;
	_fineRayleigh.set ( 0.05f, 0.15f, 1.5f );
	_drawSky = true;
	_fogNear = 500.f;
	_fogFar = 3000.f;
	_boundingBox.setMin (ATOM_Vector3f(0.f));
	_boundingBox.setMax (ATOM_Vector3f(0.f));
	_sunLight = ATOM_NEW(ATOM_Light);
	_sunLight->setLightType (ATOM_Light::Directional);
	_sunLight->setColor4f (_lightColor);
	_sunLight->setDirection (-_lightDir);

	//--- wangjian added ---//
	_strSkyTextureFile = "";

	_skyCubeTexture = 0;
	_skyDomeTexture = 0;

	_renderMode = SKY_DOME;
	_skyBoxVertexDecl = 0;

	_strSkyDomeModelFile = "";
	_skyDomeModel = 0;
	_skydome_translate_offset = ATOM_Vector3f(0.0f);
	_skydome_scale = ATOM_Vector3f(1.0f);
	_fogColor = ATOM_Vector3f(1.0f);

	_skyTexDirty = false;
	_skyDomeModelDirty = false;

	//_load_flag = LOAD_ALLFINISHED;
	//----------------------//
}

ATOM_Sky::~ATOM_Sky (void)
{
	destroy (ATOM_GetRenderDevice());

	// wangjian modified 
	//ATOM_DELETE(_sunLight);
}

void ATOM_Sky::accept (ATOM_Visitor &visitor)
{
	visitor.visit (*this);
}

bool ATOM_Sky::onLoad(ATOM_RenderDevice *device)
{
	return create (device, 16, 16);
}

void ATOM_Sky::buildBoundingbox (void) const
{
}

void ATOM_Sky::buildWorldBoundingbox (void) const
{
}

bool ATOM_Sky::create (ATOM_RenderDevice *device, int gridDimX, int gridDimZ)
{
	if (!_skyMaterial)
	{
		_skyMaterial = ATOM_MaterialManager::createMaterialFromCore (device, "/materials/builtin/sky.mat");
		if( !_skyMaterial )
			return false;
	}

	bool success = false;

	//=================== SKYBOX ======================//
	if( _renderMode == SKY_BOX )
	{
		success = createSkyBox(device);
	}
	//=================== SKYDOME ======================//
	else if( _renderMode == SKY_DOME )
	{
		success = createSkyDome(device);
	}
	//=================== SKYSCATTER ======================//
	else
	{
		if (!createCloud (device, gridDimX, gridDimZ))
		{
			destroy (device);
			return false;
		}

		if (!createSkyPlane (device))
		{
			destroy (device);
			return false;
		}

		success = true;
	}

	// 成功 完成
	if( success )
		_load_flag = LOAD_ALLFINISHED;

	return success;
}

bool ATOM_Sky::createCloud (ATOM_RenderDevice *device, int dimX, int dimZ)
{
	_densityMap = device->allocTexture (0, 0, 512, 512, ATOM_PIXEL_FORMAT_BGRA8888, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET);
	_blurredMap = device->allocTexture (0, 0, 512, 512, ATOM_PIXEL_FORMAT_BGRA8888, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET);

	if (!createGrid (device, dimX, dimZ))
	{
		return false;
	}

	if (!createBlur (device))
	{
		return false;
	}

	if (!createFinalCloud (device))
	{
		return false;
	}

	return true;
}

bool ATOM_Sky::createGrid (ATOM_RenderDevice *device, int dimX, int dimZ)
{
	_startXZ.set (-20000.f, -20000.f);
	_cellSizeXZ.set (80000.f/dimX, 80000.f/dimZ);

	unsigned numVertices = (dimX+1)*(dimZ+1);
	_gridVertexArray = device->allocVertexArray (ATOM_VERTEX_ATTRIB_COORD_XYZW, ATOM_USAGE_STATIC, numVertices);
	ATOM_Vector4f *vertices = (ATOM_Vector4f*)_gridVertexArray->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
	if (!vertices)
	{
		return false;
	}
	float fScaleX = 1.f/(dimX+1);
	float fScaleZ = 1.f/(dimZ+1);
	for (unsigned z = 0; z < dimZ+1; ++z)
		for (unsigned x = 0; x < dimX+1; ++x)
		{
			vertices->x = (float)x;
			vertices->y = (float)z;
			vertices->z = x * fScaleX;
			vertices->w = z * fScaleZ;
			++vertices;
		}
	_gridVertexArray->unlock ();

	_boundingBox.setMin (ATOM_Vector3f(_startXZ.x, 0.f, _startXZ.y));
	ATOM_Vector2f vEndXZ (_cellSizeXZ.x * dimX, _cellSizeXZ.y * dimZ);
	vEndXZ += _startXZ;
	_boundingBox.setMax (ATOM_Vector3f(vEndXZ.x, 0.f, vEndXZ.y));

	unsigned numIndices = (dimX+2)*2*dimZ-2;
	_gridIndexArray = device->allocIndexArray (ATOM_USAGE_STATIC, numIndices, false, true);
	unsigned short *indices = (unsigned short*)_gridIndexArray->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
	if (!indices)
	{
		return false;
	}
	unsigned short nVertexNumX = (unsigned short)(dimX + 1);
	for (short x = dimX; 0 <= x; --x)
	{
		*indices++ = x;
		*indices++ = nVertexNumX + x;
	}
	for (unsigned short z = 1; z < dimZ; ++z)
	{
		*indices++ = z * nVertexNumX;
		*indices++ = z * nVertexNumX + dimX;
		for (short x = dimX; 0 <= x; --x)
		{
			*indices++ = z * dimX + x;
			*indices++ = (z+1) * dimX + x;
		}
	}
	_gridIndexArray->unlock ();

	_gridVertexDecl = device->createInterleavedVertexDeclaration (ATOM_VERTEX_ATTRIB_COORD_XYZW);

	return true;
}

bool ATOM_Sky::createBlur (ATOM_RenderDevice *device)
{
	return true;
}

bool ATOM_Sky::createFinalCloud (ATOM_RenderDevice *device)
{
	const int DIV_X = 4;
	const int DIV_Y = 4;
	const int NUM_VERTICES = (DIV_X+1) * (DIV_Y+1);
	const int NUM_INDICES  = 2*DIV_Y * (DIV_X+1) + (DIV_Y-1)*2;
	const int NUM_TRIANGLES = NUM_INDICES-2;

	_cloudPlaneVertexArray = device->allocVertexArray (ATOM_VERTEX_ATTRIB_COORD_XYZW|ATOM_VERTEX_ATTRIB_TEX1_2, ATOM_USAGE_STATIC, NUM_VERTICES);
	struct CloudPlaneVertex
	{
		ATOM_Vector4f pos;
		ATOM_Vector2f uv;
	};
	CloudPlaneVertex *vertices = (CloudPlaneVertex*)_cloudPlaneVertexArray->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
	if (!vertices)
	{
		return false;
	}
	for (unsigned i = 0; i <= DIV_Y; ++i)
		for (unsigned j = 0; j <= DIV_X; ++j)
		{
			float fX = 1.f - j/(float)DIV_X;
			float fY = i/(float)DIV_Y;
			vertices->pos.set (fX*2.f-1.f, -(fY*2.f-1.f), 1.f, 1.f);
			vertices->uv.set (fX, fY);
			++vertices;
		}
	_cloudPlaneVertexArray->unlock ();

	_cloudPlaneIndexArray = device->allocIndexArray (ATOM_USAGE_STATIC, NUM_INDICES, false, true);
	unsigned short *indices = (unsigned short*)_cloudPlaneIndexArray->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
	if (!indices)
	{
		return false;
	}
	for (unsigned short i = 0; i < DIV_Y; ++i)
	{
		for (unsigned short j = 0; j <= DIV_X; ++j)
		{
			*indices++ = i * (DIV_X+1) + j;
			*indices++ = (i+1) * (DIV_X+1) + j;
		}
		if (i+1 < DIV_Y)
		{
			*indices++ = (i+1) * (DIV_X+1) + DIV_X;
			*indices++ = (i+1) * (DIV_X+1);
		}
	}
	_cloudPlaneIndexArray->unlock ();

	_cloudPlaneVertexDecl = device->createInterleavedVertexDeclaration (ATOM_VERTEX_ATTRIB_COORD_XYZW|ATOM_VERTEX_ATTRIB_TEX1_2);

	return true;
}

bool ATOM_Sky::createSkyPlane (ATOM_RenderDevice *device)
{
	const int DIV_X = 4;
	const int DIV_Y = 4;
	const int NUM_VERTICES = (DIV_X+1) * (DIV_Y+1);
	const int NUM_INDICES  = 2*DIV_Y * (DIV_X+1) + (DIV_Y-1)*2;
	const int NUM_TRIANGLES = NUM_INDICES-2;

	_skyPlaneVertexArray = device->allocVertexArray (ATOM_VERTEX_ATTRIB_COORD_XYZW, ATOM_USAGE_STATIC, NUM_VERTICES);
	ATOM_Vector4f *vertices = (ATOM_Vector4f*)_skyPlaneVertexArray->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
	if (!vertices)
	{
		return false;
	}
	for (unsigned i = 0; i <= DIV_Y; ++i)
		for (unsigned j = 0; j <= DIV_X; ++j)
		{
			float fX = 1.f - j/(float)DIV_X;
			float fY = 1.f - i/(float)DIV_Y;
			vertices->set (fX*2.f-1.f, fY*2.f-1.f, 1.f, 1.f);
			++vertices;
		}
	_skyPlaneVertexArray->unlock ();

	_skyPlaneIndexArray = device->allocIndexArray (ATOM_USAGE_STATIC, NUM_INDICES, false, true);
	unsigned short *indices = (unsigned short*)_skyPlaneIndexArray->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
	if (!indices)
	{
		return false;
	}
	for (unsigned short i = 0; i < DIV_Y; ++i)
	{
		for (unsigned short j = 0; j <= DIV_X; ++j)
		{
			*indices++ = i * (DIV_X+1) + j;
			*indices++ = (i+1) * (DIV_X+1) + j;
		}
		if (i+1 < DIV_Y)
		{
			*indices++ = (i+1) * (DIV_X+1) + DIV_X;
			*indices++ = (i+1) * (DIV_X+1);
		}
	}
	_skyPlaneIndexArray->unlock ();

	_skyPlaneVertexDecl = device->createInterleavedVertexDeclaration (ATOM_VERTEX_ATTRIB_COORD_XYZW);

	return true;
}

void ATOM_Sky::destroy (ATOM_RenderDevice *device)
{
	_densityMap = 0;
	_blurredMap = 0;
	_cloudMap = 0;
	_skyMaterial = 0;
	_gridVertexArray = 0;
	_gridIndexArray = 0;
	device->destroyVertexDecl (_gridVertexDecl);
	_gridVertexDecl = 0;
	_cloudPlaneVertexArray = 0;
	_cloudPlaneIndexArray = 0;
	device->destroyVertexDecl (_cloudPlaneVertexDecl);
	_cloudPlaneVertexDecl = 0;
	_skyPlaneVertexArray = 0;
	_skyPlaneIndexArray = 0;
	device->destroyVertexDecl (_skyPlaneVertexDecl);
	_skyPlaneVertexDecl = 0;
	_cloudCoverage = 0.5f;
	_cloudHeight = 0.f;
	_velocity.set (0.01f, 0.01f);
	_uvOffset.set (0.f, 0.f);
	_startXZ.set (0.f, 0.f);
	_cellSizeXZ.set (0.f, 0.f);
	_lightDir.set (0.f, -1.f, 0.f);
	_lightColor.set (1.f, 1.f, 1.f, 1.f);
	_ambientLight.set (0.3f, 0.35f, 0.4f, 1.f);
	_farClip = 40000.f;
	_rayleigh.set (0.3f, 0.45f, 0.6f);
	_mie.set (0.3f, 0.3f, 0.3f);
	_G = 0.7f;
	_lightScale = 1.f;
	_earthRadius = 21600000.f;
	_atmosphereHeight = 30000.f;
	_height = 600.f;
	_exposure = 1.f;
	_cameraHeight = 5000.f;
	_ambientScale = 0.1f;
	_fogDensity = 1.f;
	_fineRayleigh.set ( 0.05f, 0.15f, 1.5f );
	_boundingBox.setMin (ATOM_Vector3f(0.f));
	_boundingBox.setMax (ATOM_Vector3f(0.f));
}

bool ATOM_Sky::prepareCloudTextures (ATOM_RenderDevice *device)
{
	ATOM_AUTOREF(ATOM_Texture) oldRT = device->getRenderTarget (0);
	ATOM_Rect2Di oldVP = device->getViewport (0);
	ATOM_ColorARGB oldClearColor = device->getClearColor (0);
	unsigned numPasses;

	device->setRenderTarget (0, _densityMap.get());
	device->setViewport (0, 0, 0, _densityMap->getWidth(), _densityMap->getHeight());
	device->setClearColor (0, 0.f, 0.f, 0.f, 0.f);
	device->clear (true, false, false);

	_skyMaterial->setActiveEffect ("Grid");
	device->setVertexDecl (_gridVertexDecl);
	device->setStreamSource (0, _gridVertexArray.get());
	numPasses = _skyMaterial->begin (device);
	for (unsigned i = 0; i < numPasses; ++i)
	{
		if (_skyMaterial->beginPass (device, i))
		{
			device->renderStreamsIndexed (_gridIndexArray.get(), ATOM_PRIMITIVE_TRIANGLE_STRIP, _gridIndexArray->getNumIndices()-2);
			_skyMaterial->endPass (device, i);
		}
	}
	_skyMaterial->end (device);

	device->setRenderTarget (0, _blurredMap.get());
	device->setViewport (0, 0, 0, _blurredMap->getWidth(), _blurredMap->getHeight());
	struct BlurVertex
	{
		float pos[4];
		float uv[2];
	};
	const BlurVertex vertices[] = {
		{ {  1.0f,  1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f } },
		{ {  1.0f, -1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f } },
		{ { -1.0f,  1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f } },
		{ { -1.0f, -1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } },
	};
	const unsigned short indices[] = {
		0, 1, 2, 3
	};
	_skyMaterial->setActiveEffect ("Blur");
	numPasses = _skyMaterial->begin (device);
	for (unsigned i = 0; i < numPasses; ++i)
	{
		if (_skyMaterial->beginPass (device, i))
		{
			device->renderInterleavedMemoryStreamsIndexed (ATOM_PRIMITIVE_TRIANGLE_STRIP, 4, 4, ATOM_VERTEX_ATTRIB_COORD_XYZW|ATOM_VERTEX_ATTRIB_TEX1_2, 6*sizeof(float), vertices, indices);
			_skyMaterial->endPass (device, i);
		}
	}
	_skyMaterial->end (device);

	device->setRenderTarget (0, oldRT.get());
	device->setViewport (0, oldVP);
	device->setClearColor (0, oldClearColor);

	int n = 0;
	if (n)
	{
		_densityMap->saveToFile ("/densitymap.png");
		_blurredMap->saveToFile ("/blurredmap.png");
	}

	return true;
}

void ATOM_Sky::update (ATOM_RenderDevice *device, ATOM_Camera *camera)
{
	//ATOM_Matrix4x4f invView (
	//	-0.70710671, 0.40824667, -0.57735145, 0.00000000,
	//	0.00000000, 0.81649816, 0.57734799, 0.00000000,
	//	0.70710677, 0.40824664, -0.57735139, 0.00000000,
	//	-0.00000000, -155.13466, -108.69611, 1.0000000);
	//ATOM_Matrix4x4f projMatrix (
	//	1.8106600, 0.00000000, 0.00000000, 0.00000000,
	//	0.00000000, 2.4142134, 0.00000000, 0.00000000,
	//	0.00000000, 0.00000000, 1.0001667, 1.0000000,
	//	0.00000000, 0.00000000, -5.0008335, 0.00000000);
	//ATOM_Matrix4x4f viewMatrix;
	//viewMatrix.invertAffineFrom (invView);
	//camera->setViewMatrix (viewMatrix);
	//camera->setProjectionMatrix (projMatrix);

	unsigned delta = ATOM_APP->getFrameStamp().elapsedTick;
	ATOM_Vector2f vec = _velocity * delta * 0.001f;
	_uvOffset += vec;

	float fRange = 0.5f * _farClip;
	float fHeight = fRange * 0.12f;
	_cloudHeight = fHeight + _cameraHeight;// +camera->getPosition().y;
	_falloffHeight = -(0.1f/fRange) * (_cameraHeight/fHeight + 1.f);

	ATOM_Vector3f vMax = _boundingBox.getMax ();
	vMax.y = _cloudHeight;
	_boundingBox.setMax (vMax);

	float fCloudCover = (_cloudCoverage - 0.7f) / (1.0f - 0.7f);
	fCloudCover = ATOM_max2( 0.0f, fCloudCover );
	// mie scattering is caused by vapor.
	float fMie = 0.05f * (1.0f - fCloudCover) + 1.5f * fCloudCover;
	_mie = ATOM_Vector3f( fMie, fMie, fMie );
	// rayleigh scattering 
	float fRayleigh =  0.9f*fCloudCover + 1.0f*(1.0f-fCloudCover);
	_rayleigh = _fineRayleigh * fRayleigh;
	// ambient 
	ATOM_Vector3f vFineAmbient( 0.3f, 0.35f, 0.4f );
	ATOM_Vector3f vCloudyAmbient( 0.35f, 0.35f, 0.35f );
	_ambientLight.setVector3 (vFineAmbient + (vCloudyAmbient - vFineAmbient) * fCloudCover);
	_ambientLight.w = 1.0f;
	_ambientScale = 0.5f * (1.0f - fCloudCover) + 1.0f * fCloudCover;
}

void ATOM_Sky::setMaterialParameters (ATOM_RenderDevice *device, ATOM_Camera *camera)
{
	ATOM_Matrix4x4f viewMatrix = camera->getViewMatrix ();
	viewMatrix.m30 = 0.f;
	viewMatrix.m31 = _cameraHeight;
	viewMatrix.m32 = 0.f;
	viewMatrix.invertAffine ();

	ATOM_Matrix4x4f projMatrix = camera->getProjectionMatrix();
	float nearClip = camera->getNearPlane();
	projMatrix.m22 = _farClip / (_farClip - nearClip);
	projMatrix.m32 = -nearClip * projMatrix.m22;

	ATOM_Matrix4x4f viewProj = projMatrix >> viewMatrix;

	ATOM_Matrix4x4f invViewProj;
	invViewProj.invertFrom (viewProj);

	//--- wangjian modified ---//
	//struct ScatteringParams
	//{
	//	ATOM_Vector4f vRayleigh;   // rgb : 3/(16*PI) * Br           w : -2*g
	//	ATOM_Vector4f vMie;        // rgb : 1/(4*PI) * Bm * (1-g)^2  w : (1+g^2)
	//	ATOM_Vector4f vESun;       // rgb : Esun/(Br+Bm)             w : R
	//	ATOM_Vector4f vSum;        // rgb : (Br+Bm)                  w : h(2R+h)
	//	ATOM_Vector4f vAmbient;    // rgb : ambient
	//};
	//ScatteringParams scatterParams;
	_scatterParams.vRayleigh.setVector3 (_rayleigh * 3.f / (16.f * ATOM_Pi));
	_scatterParams.vRayleigh.w = -2.0 * _G;
	float iG = 1.f - _G;
	_scatterParams.vMie.setVector3 (_mie * iG * iG / (4.f * ATOM_Pi));
	_scatterParams.vMie.w = 1.f + _G * _G;
	ATOM_Vector3f vSum = _rayleigh + _mie;
	_scatterParams.vESun.setVector3 (_lightScale * _lightColor.getVector3() / vSum);
	_scatterParams.vESun.w = _earthRadius;
	_scatterParams.vSum.setVector3 (vSum / _farClip);
	_scatterParams.vSum.w = _atmosphereHeight * (2.f * _earthRadius + _atmosphereHeight);
	_scatterParams.vAmbient = _ambientLight * _ambientScale;
	_scatterParams.vAmbient.w = 1.f / ATOM_sqrt (_scatterParams.vSum.w);
	//----------------------------------------------------------------//
	ATOM_Vector4f cloudDistance (_earthRadius, _height * (2.f * _earthRadius + _height), 0.f, 0.f);
	ATOM_Vector4f uvParam(5.f, 5.f, _uvOffset.x, _uvOffset.y);
	ATOM_Vector4f xzParam(_cellSizeXZ.x, _cellSizeXZ.y, _startXZ.x, _startXZ.y);
	ATOM_Vector4f height(_falloffHeight, _cloudHeight, 0.f, 0.f);
	ATOM_Vector4f vPix(0.5f/_densityMap->getWidth(), 0.5f/_densityMap->getHeight(), 0.f, 0.f);
	ATOM_Vector4f dist(_scatterParams.vESun.w, _scatterParams.vSum.w, _atmosphereHeight, 0.f);
	ATOM_Vector4f vLit(_lightDir.x, _lightDir.y, _lightDir.z, 0.f);
	if (vLit.y > 0.f) vLit.y = 0.f;
	vLit.normalize ();
	ATOM_Vector4f vProjPos = viewProj >> vLit;
	ATOM_Vector4f vBlurDir;
	static const float FLOAT_EPSL = 0.000001f;
	if ((ATOM_abs(vProjPos.w) < FLOAT_EPSL) || (ATOM_abs(vProjPos.z) < FLOAT_EPSL))
	{
		vProjPos.w = 0.f;
		vProjPos.z = 0.f;
		vProjPos.normalize ();
		vProjPos.y = -vProjPos.y;
		vBlurDir.set (0.f, 0.f, -vProjPos.x, -vProjPos.y);
	}
	else
	{
		if (0.f < vProjPos.w)
		{
			vProjPos /= vProjPos.w;
			vProjPos.x = 0.5f * vProjPos.x + 0.5f;
			vProjPos.y = -0.5f * vProjPos.y + 0.5f;
			vBlurDir.set (1.f, 1.f, -vProjPos.x, -vProjPos.y);
		}
		else
		{
			vProjPos /= vProjPos.w;
			vProjPos.x = 0.5f * vProjPos.x + 0.5f;
			vProjPos.y = -0.5f * vProjPos.y + 0.5f;
			vBlurDir.set (-1.f, -1.f, vProjPos.x, vProjPos.y);
		}
	}
	float fMaxMove = 0.1f / 16.f;
	ATOM_Vector4f vInvMax (1.f / fMaxMove, 1.f / fMaxMove, 0.f, 0.f);

	_skyMaterial->getParameterTable()->setMatrix44 ("mC2W", invViewProj);
	_skyMaterial->getParameterTable()->setMatrix44 ("mW2C", viewProj);
	_skyMaterial->getParameterTable()->setVector ("vEye", ATOM_Vector3f(0.f, _cameraHeight, 0.f));
	_skyMaterial->getParameterTable()->setVector ("CameraPos", camera->getPosition());
	_skyMaterial->getParameterTable()->setVector ("litDir", _lightDir);
	_skyMaterial->getParameterTable()->setVector ("litDirView", viewMatrix.transformVector(_lightDir));
	_skyMaterial->getParameterTable()->setVectorArray ("scat", &_scatterParams.vRayleigh, 5);
	_skyMaterial->getParameterTable()->setVector ("cLit", _lightColor);
	_skyMaterial->getParameterTable()->setVector ("cAmb", _ambientLight);
	_skyMaterial->getParameterTable()->setVector ("vDistance", cloudDistance);
	_skyMaterial->getParameterTable()->setVector ("vUVParam", uvParam);
	_skyMaterial->getParameterTable()->setVector ("vXZParam", xzParam);
	_skyMaterial->getParameterTable()->setVector ("vHeight", height);
	_skyMaterial->getParameterTable()->setFloat ("fCloudCover", _cloudCoverage);
	_skyMaterial->getParameterTable()->setVector ("vOff", vBlurDir);
	_skyMaterial->getParameterTable()->setVector ("vParam", dist);
	_skyMaterial->getParameterTable()->setVector ("invMax", vInvMax);
	_skyMaterial->getParameterTable()->setVector ("vFalloff", ATOM_Vector4f(-5000.0f, -1.5f, -1.5f, -1000.0f));
	_skyMaterial->getParameterTable()->setFloat ("fExposure", _exposure);
	_skyMaterial->getParameterTable()->setFloat ("fogDensity", _fogDensity * 1000.0f/* * 0.0001f*/);	// wangjian modified : 修改了雾浓度的乘值。
	_skyMaterial->getParameterTable()->setTexture ("densityTexture", _densityMap.get());
	_skyMaterial->getParameterTable()->setTexture ("blurredTexture", _blurredMap.get());
	_skyMaterial->getParameterTable()->setTexture ("cloudTexture", _cloudMap.get());
	//--- wangjian added ---//
	// 将雾的效果改成线性的
	float scene_far = _fogFar;
	ATOM_DeferredScene *currentScene = dynamic_cast<ATOM_DeferredScene*>( ATOM_RenderScheme::getCurrentScene () );
	if( currentScene )
	{
		ATOM_BBox & box = currentScene->getPrecalculatedBoundingBox();
		ATOM_Vector3f eye,at,up;
		camera->getCameraPos(&eye,&at,&up);
		ATOM_Vector3f eye_dir = (at - eye);
		eye_dir.normalize();

		const ATOM_Vector3f & _minPoint = box.getMin();
		const ATOM_Vector3f & _maxPoint = box.getMax();
		ATOM_Vector3f corners[8];
		corners[0] = _minPoint;
		corners[1].x = _minPoint.x; corners[1].y = _maxPoint.y; corners[1].z = _minPoint.z;
		corners[2].x = _maxPoint.x; corners[2].y = _maxPoint.y; corners[2].z = _minPoint.z;
		corners[3].x = _maxPoint.x; corners[3].y = _minPoint.y; corners[3].z = _minPoint.z;
		corners[4] = _maxPoint;
		corners[5].x = _minPoint.x; corners[5].y = _maxPoint.y; corners[5].z = _maxPoint.z;
		corners[6].x = _minPoint.x; corners[6].y = _minPoint.y; corners[6].z = _maxPoint.z;
		corners[7].x = _maxPoint.x; corners[7].y = _minPoint.y; corners[7].z = _maxPoint.z;

		float dist = 0;
		float dist_farest = 0;
		for( int i = 0; i < 8; i++ )
		{
			corners[i] = corners[i] - eye;
			dist = dotProduct(corners[i],eye_dir);
			if( dist < 0 )
				continue;
			if( dist > dist_farest )
				dist_farest = dist;
		}
		scene_far = dist_farest * 0.8f;
	}
	_skyMaterial->getParameterTable()->setVector ( "cameraNearFarFactor", ATOM_Vector4f( _fogNear, 1.0f/(_fogFar-_fogNear) , scene_far, camera->getFarPlane() ) );
	//----------------------//
}

bool ATOM_Sky::draw (ATOM_RenderDevice *device, ATOM_Camera *camera, ATOM_Material *material)
{
	//--- wangjian modified ---//
	if( _renderMode == SKY_BOX )
	{
		ATOM_DeferredRenderScheme *renderScheme = dynamic_cast<ATOM_DeferredRenderScheme*>(ATOM_RenderScheme::getCurrentRenderScheme());
		if (renderScheme)
		{
			// skybox
			drawSkyBox(device,camera,material);

			//setMaterialParameters (device, camera);

			//// fog
			//_skyMaterial->setActiveEffect ("Fog");
			//unsigned numPasses = _skyMaterial->begin (device);
			//for (unsigned i = 0; i < numPasses; ++i)
			//{
			//	if (_skyMaterial->beginPass (device, i))
			//	{
			//		renderScheme->drawScreenQuad (device, _skyMaterial.get());
			//		_skyMaterial->endPass (device, i);
			//	}
			//}
			//_skyMaterial->end (device);
		}
	}
	else if( _renderMode == SKY_DOME )
	{
		ATOM_DeferredRenderScheme *renderScheme = dynamic_cast<ATOM_DeferredRenderScheme*>(ATOM_RenderScheme::getCurrentRenderScheme());
		if (renderScheme)
		{
			// fog
			_skyMaterial->setActiveEffect ("SimpleFog");

			_skyMaterial->getParameterTable()->setVector("fogColor",_fogColor);
			_skyMaterial->getParameterTable()->setFloat("fogDensity",_fogDensity * 1000.0f);
			_skyMaterial->getParameterTable()->setVector ( "cameraNearFarFactor", ATOM_Vector4f( _fogNear, 1.0f/(_fogFar-_fogNear) , _fogFar, camera->getFarPlane() ) );

			renderScheme->drawScreenQuad (device, _skyMaterial.get());
			//unsigned numPasses = _skyMaterial->begin (device);
			//for (unsigned i = 0; i < numPasses; ++i)
			//{
			//	if (_skyMaterial->beginPass (device, i))
			//	{
			//		renderScheme->drawScreenQuad (device, _skyMaterial.get());
			//		_skyMaterial->endPass (device, i);
			//	}
			//}
			//_skyMaterial->end (device);
		}

		drawSkyDome(device,camera,material);
	}
	else
	{
		setMaterialParameters (device, camera);

		//wangjian added
		if( false == ATOM_RenderSettings::isHDRRenderingEnabled() && false == ATOM_RenderSettings::isFloatShadingEnabled() )
			device->enableSRGBWrite(false);

		prepareCloudTextures (device);

		//wangjian added
		if( false == ATOM_RenderSettings::isHDRRenderingEnabled() && false == ATOM_RenderSettings::isFloatShadingEnabled() )
			device->enableSRGBWrite(true);

		unsigned numPasses;

		// Draw sky plane
		_skyMaterial->setActiveEffect ("SkyPlane");
		device->setVertexDecl (_skyPlaneVertexDecl);
		device->setStreamSource (0, _skyPlaneVertexArray.get());
		numPasses = _skyMaterial->begin (device);
		for (unsigned i = 0; i < numPasses; ++i)
		{
			if (_skyMaterial->beginPass (device, i))
			{
				device->renderStreamsIndexed (_skyPlaneIndexArray.get(), ATOM_PRIMITIVE_TRIANGLE_STRIP, _skyPlaneIndexArray->getNumIndices()-2);
				_skyMaterial->endPass (device, i);
			}
		}
		_skyMaterial->end (device);
		// Draw cloud plane
		_skyMaterial->setActiveEffect ("CloudPlane");
		device->setVertexDecl (_cloudPlaneVertexDecl);
		device->setStreamSource (0, _cloudPlaneVertexArray.get());
		numPasses = _skyMaterial->begin (device);
		for (unsigned i = 0; i < numPasses; ++i)
		{
			if (_skyMaterial->beginPass (device, i))
			{
				device->renderStreamsIndexed (_cloudPlaneIndexArray.get(), ATOM_PRIMITIVE_TRIANGLE_STRIP, _cloudPlaneIndexArray->getNumIndices()-2);
				_skyMaterial->endPass (device, i);
			}
		}
		_skyMaterial->end (device);

		ATOM_DeferredRenderScheme *renderScheme = dynamic_cast<ATOM_DeferredRenderScheme*>(ATOM_RenderScheme::getCurrentRenderScheme());
		if (renderScheme)
		{
			_skyMaterial->setActiveEffect ("Fog");
			renderScheme->drawScreenQuad (device, _skyMaterial.get());
		}
	}
	
	return true;
}

void ATOM_Sky::setCloudTextureFileName (const ATOM_STRING &fileName)
{
	_cloudTextureFileName = fileName;
	//--- wangjian modified ---//
	// 异步加载标记
	_cloudMap = ATOM_CreateTextureResource (fileName.c_str(),ATOM_PIXEL_FORMAT_UNKNOWN,_load_priority);
}

const ATOM_STRING &ATOM_Sky::getCloudTextureFileName (void) const
{
	return _cloudTextureFileName;
}

void ATOM_Sky::setCloudMaterialFileName (const ATOM_STRING &fileName)
{
	_materialFileName = fileName;
	_skyMaterial = ATOM_MaterialManager::createMaterialFromCore (ATOM_GetRenderDevice(), fileName.c_str());
}

const ATOM_STRING &ATOM_Sky::getCloudMaterialFileName (void) const
{
	return _materialFileName;
}

void ATOM_Sky::setCloudCover (float cover)
{
	_cloudCoverage = cover;
}

float ATOM_Sky::getCloudCover (void) const
{
	return _cloudCoverage;
}

void ATOM_Sky::setFarClip (float farClip)
{
	_farClip = farClip;
}

float ATOM_Sky::getFarClip (void) const
{
	return _farClip;
}

void ATOM_Sky::setVelocity (const ATOM_Vector2f &velocity)
{
	_velocity = velocity;
}

const ATOM_Vector2f &ATOM_Sky::getVelocity (void) const
{
	return _velocity;
}

ATOM_Material *ATOM_Sky::getMaterial (void) const
{
	return _skyMaterial.get();
}

void ATOM_Sky::setLightDir (const ATOM_Vector3f &dir)
{
	_lightDir = dir;
	_sunLight->setDirection (-dir);
}

const ATOM_Vector3f &ATOM_Sky::getLightDir (void) const
{
	return _lightDir;
}

void ATOM_Sky::setExposure (float exposure)
{
	_exposure = exposure;
}

float ATOM_Sky::getExposure (void) const
{
	return _exposure;
}

void ATOM_Sky::setCameraHeight (float height)
{
	_cameraHeight = height;
}

float ATOM_Sky::getCameraHeight (void) const
{
	return _cameraHeight;
}

void ATOM_Sky::setLightColor (const ATOM_Vector4f &color)
{
	_lightColor = color;
	_sunLight->setColor4f (ATOM_Vector4f(color.x, color.y, color.z, _sunLight->getColor4f().w));
}

const ATOM_Vector4f &ATOM_Sky::getLightColor (void) const
{
	return _lightColor;
}

void ATOM_Sky::setAmbientLight (const ATOM_Vector4f &color)
{
	_ambientLight = color;
}

const ATOM_Vector4f &ATOM_Sky::getAmbientLight (void) const
{
	return _ambientLight;
}

void ATOM_Sky::setLightScale (float lightScale)
{
	_lightScale = lightScale;
}

float ATOM_Sky::getLightScale (void) const
{
	return _lightScale;
}

void ATOM_Sky::setFogDensity (float density)
{
	_fogDensity = density;
}

float ATOM_Sky::getFogDensity (void) const
{
	return _fogDensity;
}

void ATOM_Sky::setRayleigh (const ATOM_Vector3f &rayleigh)
{
	_fineRayleigh = rayleigh;
}

const ATOM_Vector3f &ATOM_Sky::getRayleigh (void) const
{
	return _fineRayleigh;
}

void ATOM_Sky::setLightIntensity (float intensity)
{
	_sunLight->setColor4f (ATOM_Vector4f(_lightColor.x, _lightColor.y, _lightColor.z, intensity));
}

float ATOM_Sky::getLightIntensity (void) const
{
	return _sunLight->getColor4f().w;
}

ATOM_Light *ATOM_Sky::getSunLight (void) const
{
	// wangjian modified
	return _sunLight.get();
}

void ATOM_Sky::setFogNear (float fogNear)
{
	_fogNear = fogNear;
}

float ATOM_Sky::getFogNear (void) const
{
	return _fogNear;
}

void ATOM_Sky::setFogFar (float fogFar)
{
	_fogFar = fogFar;
}

float ATOM_Sky::getFogFar (void) const
{
	return _fogFar;
}

void ATOM_Sky::enableDrawSky (int enable)
{
	_drawSky = enable != 0;
}

int ATOM_Sky::isDrawSkyEnabled (void) const
{
	return _drawSky ? 1 : 0;
}

//--- wangjian added ---//
void ATOM_Sky::setFogColor ( const ATOM_Vector3f & color)
{
	_fogColor = color;
}
const ATOM_Vector3f & ATOM_Sky::getFogColor (void) const
{
	return _fogColor;
}
void ATOM_Sky::getScatterParams(ATOM_Vector4f& vRayleigh,ATOM_Vector4f& vMie,ATOM_Vector4f& vESun,ATOM_Vector4f& vSum,ATOM_Vector4f& vAmbient) const
{
	vRayleigh = _scatterParams.vRayleigh;
	vMie = _scatterParams.vMie;
	vESun = _scatterParams.vESun;
	vSum = _scatterParams.vSum;
	vAmbient = _scatterParams.vAmbient;
}
//----------------------//

void ATOM_Sky::setRenderMode(int mode)
{
	if( _renderMode != mode )
	{
		_renderMode = (renderMode)mode;

		create (ATOM_GetRenderDevice(), 16, 16);
	}
}
int ATOM_Sky::getRenderMode(void) const
{
	return (int)_renderMode;
}
void ATOM_Sky::setSkyTextureFile(const ATOM_STRING & filename)
{
	if( _renderMode != SKY_BOX && _renderMode != SKY_DOME )
		return;

	if( !filename.empty() && _strSkyTextureFile != filename )
	{
		_strSkyTextureFile = filename;

		_skyTexDirty = true;
		/*if( _renderMode == SKY_BOX )
			createSkyBox(ATOM_GetRenderDevice());
		else if( _renderMode == SKY_DOME )
			createSkyDome(ATOM_GetRenderDevice());*/
	}
}

const ATOM_STRING & ATOM_Sky::getSkyTextureFile(void) const
{
	return _strSkyTextureFile;
}

void ATOM_Sky::setSkyDomeModelFile(const ATOM_STRING & filename)
{
	if( _renderMode != SKY_DOME )
		return;

	if( !filename.empty() && _strSkyDomeModelFile != filename )
	{
		_strSkyDomeModelFile = filename;

		_skyDomeModelDirty = true;
		//createSkyDome(ATOM_GetRenderDevice());
	}
}

const ATOM_STRING & ATOM_Sky::getSkyDomeModelFile(void) const
{
	return _strSkyDomeModelFile;
}

void ATOM_Sky::setSkyDomeTranslateOffset(const ATOM_Vector3f & offset)
{
	_skydome_translate_offset = offset;
}
const ATOM_Vector3f & ATOM_Sky::getSkyDomeTranslateOffset(void) const
{
	return _skydome_translate_offset;
}

void ATOM_Sky::setSkyDomeScale(const ATOM_Vector3f & scale)
{
	_skydome_scale = scale;
}
const ATOM_Vector3f & ATOM_Sky::getSkyDomeScale(void) const
{
	return _skydome_scale;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////


bool ATOM_Sky::createSkyBox(ATOM_RenderDevice *device)
{
	if( _renderMode != SKY_BOX )
		return false;

	_skyDomeTexture = 0;
	_skyCubeTexture = 0;

	if( !_strSkyTextureFile.empty() )
	{
		_skyCubeTexture = ATOM_FindOrCreateEmptyTextureResource (_strSkyTextureFile.c_str());
	}
	if( !_skyCubeTexture )
		return false;

	_skyCubeTexture->setFlags (ATOM_Texture::CUBEMAP);
	if( !_skyCubeTexture->loadTexImageFromFile(_strSkyTextureFile.c_str(), 0, 0, ATOM_PIXEL_FORMAT_DXT1) )
		return false;

	if( !_skyBoxVertexArray )
	{
		_skyBoxVertexArray = device->allocVertexArray (ATOM_VERTEX_ATTRIB_COORD_XYZW, ATOM_USAGE_STATIC, /*6*/3);
		ATOM_Vector4f *vertices = (ATOM_Vector4f*)_skyBoxVertexArray->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
		if (!vertices)
			return false;

		vertices->set (-3, -1, 1.f, 1.f);
		++vertices;
		vertices->set (0,  2, 1.f, 1.f);
		++vertices;
		vertices->set (3, -1, 1.f, 1.f);
		++vertices;
		/*vertices->set (-1, -1, 1.f, 1.f);
		++vertices;
		vertices->set (1,  1, 1.f, 1.f);
		++vertices;
		vertices->set (-1, 1, 1.f, 1.f);
		++vertices;

		vertices->set (-1, -1, 1.f, 1.f);
		++vertices;
		vertices->set (1,  -1, 1.f, 1.f);
		++vertices;
		vertices->set (1, 1, 1.f, 1.f);*/

		_skyBoxVertexArray->unlock ();

		_skyBoxVertexDecl = device->createInterleavedVertexDeclaration (ATOM_VERTEX_ATTRIB_COORD_XYZW);
		if( !_skyBoxVertexDecl )
			return false;
	}

	return true;
}

bool ATOM_Sky::createSkyDome(ATOM_RenderDevice *device)
{
	if( _renderMode != SKY_DOME )
		return false;

	_skyCubeTexture = 0;
	
	if(_skyTexDirty && !_strSkyTextureFile.empty())
	{
		_skyDomeTexture = ATOM_CreateTextureResource (	_strSkyTextureFile.c_str(),
														ATOM_PIXEL_FORMAT_UNKNOWN,
														_load_priority	);
		if( !_skyDomeTexture )
		{
			ATOM_LOGGER::warning("Create sky dome texture failed : %s !\n",_strSkyTextureFile.c_str() );
			_skyDomeTexture = ATOM_GetColorTexture (0xFFFFFFFF);
		}
	}
	
	bool createNew = false;
	bool bLoadSuccess = false;

	if(_skyDomeModelDirty && !_strSkyDomeModelFile.empty())
	{
		_skyDomeModel = ATOM_LookupOrCreateObject (	ATOM_SharedModel::_classname(), 
													_strSkyDomeModelFile.c_str(), 
													&createNew	);
		if( _skyDomeModel && createNew )
		{
			if( _load_priority != ATOM_LoadPriority_IMMEDIATE )
			{
				ATOM_LOADRequest_Event * event_load( ATOM_NEW(	ATOM_LOADRequest_Event,
																_strSkyDomeModelFile.c_str(), 
																this,
																_load_priority,
																_skyDomeModel->getAsyncLoader(),
																ATOM_LOADRequest_Event::FLAG_SHOW ) );
			}

			bLoadSuccess = _skyDomeModel->load (device, _strSkyDomeModelFile.c_str(), _load_priority );
			if( !bLoadSuccess )
			{
				ATOM_LOGGER::warning("Create sky dome model failed : %s !\n",_strSkyDomeModelFile.c_str() );
				_skyDomeModel = 0;
			}				
		}
	}

	_skyTexDirty = false;
	_skyDomeModelDirty = false;

	return true;
}

void ATOM_Sky::drawSkyBox(ATOM_RenderDevice *device, ATOM_Camera *camera, ATOM_Material *material)
{
	_skyMaterial->setActiveEffect ("SkyBox");

	ATOM_Matrix4x4f viewMatrix = camera->getViewMatrix ();
	viewMatrix.m30 = 0.f;
	viewMatrix.m31 = _cameraHeight;
	viewMatrix.m32 = 0.f;
	viewMatrix.invertAffine ();

	ATOM_Matrix4x4f projMatrix = camera->getProjectionMatrix();
	float nearClip = camera->getNearPlane();
	projMatrix.m22 = _farClip / (_farClip - nearClip);
	projMatrix.m32 = -nearClip * projMatrix.m22;

	ATOM_Matrix4x4f viewProj = projMatrix >> viewMatrix;

	ATOM_Matrix4x4f invViewProj;
	invViewProj.invertFrom (viewProj);

	_skyMaterial->getParameterTable()->setMatrix44 ("mC2W", invViewProj);
	_skyMaterial->getParameterTable()->setTexture("skyTexture",_skyCubeTexture.get());

	device->setVertexDecl(_skyBoxVertexDecl);
	device->setStreamSource (0, _skyBoxVertexArray.get());

	unsigned numPasses = _skyMaterial->begin (device);
	for (unsigned i = 0; i < numPasses; ++i)
	{
		if (_skyMaterial->beginPass (device, i))
		{
			device->renderStreams (ATOM_PRIMITIVE_TRIANGLES, /*2*/1);
			_skyMaterial->endPass (device, i);
		}
	}
	_skyMaterial->end (device);
}

void ATOM_Sky::drawSkyDome(ATOM_RenderDevice *device, ATOM_Camera *camera, ATOM_Material *material)
{
	// if there is something dirty, recreate the texture or model
	createSkyDome(device);

	if( !_skyDomeModel || 
		!_skyDomeModel->getAsyncLoader()->IsLoadAllFinished() ||
		!_skyDomeModel->getNumMeshes() || 
		!_skyDomeModel->getMesh(0) || 
		!_skyDomeTexture )
	{
		if( ATOM_AsyncLoader::isEnableLog() )
			ATOM_LOGGER::warning("Could not draw Sky\n");
		return;
	}

#if 0
	_skyMaterial->setActiveEffect ("SkyDome");
#else
	// 前向渲染
	if( ATOM_RenderSettings::isNonDeferredShading() )
		_skyMaterial->setActiveEffect ("SkyDome_fs");
	// 延迟渲染
	else
		_skyMaterial->setActiveEffect ("SkyDome");
#endif

	ATOM_Matrix4x4f mTranslation;
	mTranslation.makeTranslate( camera->getPosition()+_skydome_translate_offset );
	ATOM_Matrix4x4f mScale;
	mScale.makeScale( _skydome_scale );
	ATOM_Matrix4x4f mWorld = mScale << mTranslation;
	_skyMaterial->getParameterTable()->setMatrix44 ("mWorld", mWorld);

	ATOM_Matrix4x4f mMVP = mWorld << camera->getInvViewMatrix() << camera->getProjectionMatrix();
	_skyMaterial->getParameterTable()->setMatrix44 ("mMVP", mMVP);

	_skyMaterial->getParameterTable()->setTexture("skyTexture",_skyDomeTexture.get());

	_skyDomeModel->getMesh(0)->draw(device,_skyMaterial.get());
}