#include "StdAfx.h"
#include "shadowmaprenderscheme.h"
#include "shadowmapcullvisitor.h"

#include "Lisp.h"

//--- wangjian added for profile shadow ---//
#define PROFILE_SHADOW_RENDERING 0

#if PROFILE_SHADOW_RENDERING

#include "kernel/profiler.h"
#define PROFILING_SHADOW_START(render_pass_name) ATOM_Profiler profile(render_pass_name);
	
#else

#define PROFILING_SHADOW_START(render_pass_name)

#endif
//----------------------------------------//


ATOM_DeferredShadowMapRenderScheme::ATOM_DeferredShadowMapRenderScheme (ATOM_RenderSchemeFactory *factory)
: ATOM_RenderScheme (factory)
{
	for (unsigned i = 0; i < RenderScheme_ShadowMapSchemeCount; ++i)
	{
		_renderQueue[i] = ATOM_NEW(ATOM_RenderQueue);
		//--- wangjian added ---//
		_renderQueue[i]->setQueueId(i);
		//----------------------//
	}
	_currentViewportWidth = 0;
	_currentViewportHeight = 0;
	_shadowDistance = 500.f;
	_fadeMin = 300;
	_fadeMax = 800;
	//--- wangjian added ---//
	// 阴影相关
	_shadowBlur = 1.0f;
	_shadowSlopeBias = 1.0f;
	_shadowDensity = 1.0f;
	//----------------------//
	_lightDistance = 500.f;
	_sceneScale = 0.2f;
	_lightSize = 0.01f;
	_depthBias = 0.001f;
	_renderMode = RenderMode_ShadowMap;
	_isOk = false;

	//=== wangjian added for LISPSM ===//
	_liSP = ATOM_NEW(LiSP, 1);
	_useLiSP = true;
	_prevLightDir = ATOM_Vector3f(0.0f);
	_prevViewPosition = ATOM_Vector3f(0.0f);
	_lightDirInit = false;
	//=================================//
}

ATOM_DeferredShadowMapRenderScheme::~ATOM_DeferredShadowMapRenderScheme (void)
{
	for (unsigned i = 0; i < RenderScheme_ShadowMapSchemeCount; ++i)
	{
		ATOM_DELETE(_renderQueue[i]);
	}

	//=== wangjian added ===//
	ATOM_DELETE(_liSP);
	fini(ATOM_GetRenderDevice());
	//=================================//
}

void ATOM_DeferredShadowMapRenderScheme::setRenderMode (int renderMode)
{
	_renderMode = renderMode;
}

const char *ATOM_DeferredShadowMapRenderScheme::getName (void) const
{
	return "shadowmap";
}

bool ATOM_DeferredShadowMapRenderScheme::init (ATOM_RenderDevice *device, unsigned width, unsigned height)
{
	onResize (device, width, height);

	return true;
}

bool ATOM_DeferredShadowMapRenderScheme::fini (ATOM_RenderDevice *device)
{
	_shadowMap = 0;
	_shadowDepthBuffer = 0;
	_shadowMask = 0;

	//--- wangjian added ---//
	// * 如果设备支持NULL RENDER TARGET的能力，那么可以使用NRT将shadowmap的color rt以节省资源 
	//   参见 “Advanced DX9 Capabilities for ATI Radeon Cards”其介绍以及
	//	 “http://aras-p.info/texts/D3D9GPUHacks.html”参考支持该能力的设备*/
	_shadowMap_dummy = 0;

	_rotationTexture = 0;

	_currentViewportWidth = 0;
	_currentViewportHeight = 0;
	//----------------------//

	return true;
}

unsigned ATOM_DeferredShadowMapRenderScheme::getWidth (void) const
{
	return _currentViewportWidth;
}

unsigned ATOM_DeferredShadowMapRenderScheme::getHeight (void) const
{
	return _currentViewportHeight;
}

void ATOM_DeferredShadowMapRenderScheme::setLightDirection (const ATOM_Vector3f &dir)
{
	_currentLightDir = dir;
	_currentLightDir.normalize ();
	if( !_lightDirInit )
		_lightDirInit = true;
}

bool ATOM_DeferredShadowMapRenderScheme::onRender (ATOM_RenderDevice *device, ATOM_Scene *scene)
{
	//return true;

	// wangjian modified : 移到CULL VISIT中去了
	/*for (unsigned i = 0; i < RenderScheme_ShadowMapSchemeCount; ++i)
	{
		_renderQueue[i]->clear ();
	}*/

	if( !_shadowMap_dummy && !_shadowMap )
		return false;

	//=== wangjian modified ===//
	// 增加对LISPSM的支持
	if (_renderMode == RenderMode_ShadowMap)
	{
		// 使用LISPSM
		if (_useLiSP)
		{
			//-------- calc cull matrix --------//
			/*ATOM_Vector3f origin;
			float radius;
			scene->getCamera ()->calculateOrigin (1.f, ATOM_min2(scene->getCamera()->getFarPlane(), getShadowDistance()), &origin, &radius);
			calculateLightMatrices (-_currentLightDir, origin, radius, _lightViewMatrix_Cull, _lightProjMatrix_Cull);*/
			//----------------------------------//

			ATOM_DeferredScene *currentScene = dynamic_cast<ATOM_DeferredScene*>( ATOM_RenderScheme::getCurrentScene () );
			if( !currentScene )
				return false;

			if( _lightDirInit || _prevLightDir != _currentLightDir || _prevViewPosition != currentScene->getCamera()->getPosition() )
			{
				if( _lightDirInit || _prevLightDir != _currentLightDir )
				{
					ATOM_Vector3f lightPos = currentScene->getPrecalculatedBoundingBox().getCenter() + _currentLightDir * _lightDistance;
					UpdateLightViewProj(	lightPos, 
											currentScene->getPrecalculatedBoundingBox() );
					_prevLightDir = _currentLightDir;
				}
				if( _lightDirInit || _prevViewPosition != currentScene->getCamera()->getPosition() )
				{
					CalcPolygonBodyBDirectional( currentScene->getPrecalculatedBoundingBox() );
					if ( _intersectionBody.GetVertexCount() == 0 )
						return false;
					UpdateDirLightViewProj( scene	);
					_prevViewPosition = currentScene->getCamera()->getPosition();
				}
				if( _lightDirInit )
					_lightDirInit = false;

				/*if (split == 0)*/
				{
					ATOM_Vector3f eye,at,up;
					scene->getCamera()->getCameraPos(&eye,&at,&up);
					_liSP->UpdateLDirVDirAngle(	eye, at, -_currentLightDir );

					_liSP->SetLightView(_lightViewMatrix);
					_liSP->SetLightProjMat(_lightProjMatrix);

					_liSP->SetCamView( scene->getCamera()->getViewMatrix() );
					_liSP->SetCamProjMat( scene->getCamera()->getProjectionMatrix() );

					float fov = scene->getCamera()->getTanHalfFovy();
					fov = atanf( fov );
					_liSP->SetFieldOfView( fov * 2.0f );

					_liSP->SetSplitParam(0.5f);

					_liSP->SetPseudoNear(0.0f);

					_liSP->SetPseudoFar(0.0f);
				}

				ATOM_Vector3f eye,at,up;
				currentScene->getCamera ()->getCameraPos(&eye,&at,&up);
				float tanHalfFovY = currentScene->getCamera ()->getTanHalfFovy();
				float aspect = currentScene->getCamera ()->getAspect();
				float cameraNear = currentScene->getCamera()->getNearPlane();
				float cameraFar = currentScene->getCamera()->getFarPlane();
				float shadowDist = getShadowDistance();
				sSimpleFrustm camFrustum(	eye,at,up,
					tanHalfFovY,aspect,
					cameraNear,
					/*cameraFar*/ATOM_min2(cameraFar,shadowDist ) );

				_depthRange.x = cameraNear;
				_depthRange.y = /*cameraFar;*/ATOM_min2(cameraFar,shadowDist );

				ATOM_Matrix4x4f liSPMtx = _liSP->GetLiSPMtx(	0, 
																_intersectionBody,
																camFrustum,
																_currentLightDir,
																currentScene->getPrecalculatedBoundingBox(), 
																_depthRange	);

				_lightProjMatrix = liSPMtx >> _lightProjMatrix;

				CalcLightViewProjMat();
			}
		}
		else
		{
			ATOM_LOGGER::log ("Use UNIFORM.\n");

			ATOM_Vector3f origin;
			float radius;
			scene->getCamera ()->calculateOrigin (1.f, ATOM_min2(scene->getCamera()->getFarPlane(), getShadowDistance()), &origin, &radius);
			calculateLightMatrices (-_currentLightDir, origin, radius, _lightViewMatrix, _lightProjMatrix);
		}
	}
	//=========================//

	if (_renderMode == RenderMode_ShadowMap)
	{
		drawShadowMap (device, scene, _lightViewMatrix, _lightProjMatrix);
	}
	else
	{
		ATOM_Matrix4x4f lightViewProj;
		lightViewProj.invertAffineFrom ( _lightViewMatrix );
		lightViewProj <<= _lightProjMatrix;

		//--- wangjian modified ---//
		// * 添加对NULL RENDER TARGET的支持 *//
		int width_sm	= device->getCapabilities().null_rtf ? _shadowMap_dummy->getWidth() : _shadowMap->getWidth();
		int height_sm	= device->getCapabilities().null_rtf ? _shadowMap_dummy->getHeight() : _shadowMap->getHeight();
		float offsX = 0.5f + 0.5f / width_sm;
		float offsY = 0.5f + 0.5f / height_sm;
		//-------------------------//
		ATOM_Matrix4x4f adj;
		adj.setRow (0, 0.5f,   0.0f,  0.0f, 0.0f);
		adj.setRow (1, 0.0f,  -0.5f,  0.0f, 0.0f);
		adj.setRow (2, 0.0f,   0.0f,  1.0f, 0.0f);
		adj.setRow (3, offsX,  offsY, 0.0f, 1.0f);
		lightViewProj <<= adj;
		lightViewProj >>= scene->getCamera()->getViewMatrix();

		drawShadowMask (device, lightViewProj);
	}

	return true;
}

int ATOM_DeferredShadowMapRenderScheme::onRenderPass (ATOM_RenderDevice *device, ATOM_Scene *scene, int layer)
{
	return -1;
}

void ATOM_DeferredShadowMapRenderScheme::drawShadowMap (ATOM_RenderDevice *device, ATOM_Scene *scene, const ATOM_Matrix4x4f &lightView, const ATOM_Matrix4x4f &lightProj)
{
	//_renderQueue[RenderScheme_ShadowMap]->render (device, camera, ATOM_RenderQueue::SortNone);
	ATOM_Vector3f savedCameraPos = scene->getCamera()->getPosition();
	ATOM_Matrix4x4f savedViewMatrix = scene->getCamera()->getViewMatrix();
	ATOM_Matrix4x4f savedProjMatrix = scene->getCamera()->getProjectionMatrix();
	ATOM_Rect2Di savedViewport = scene->getCamera()->getViewport ();

	ATOM_Camera LODcamera;
	LODcamera.setViewMatrix (savedViewMatrix);
	LODcamera.setProjectionMatrix (savedProjMatrix);
	LODcamera.setViewport (savedViewport.point.x, savedViewport.point.y, savedViewport.size.w, savedViewport.size.h);

	//--------- CULL -----------//

	scene->getCamera()->setViewMatrix (_lightViewMatrix);
	scene->getCamera()->setProjectionMatrix (_lightProjMatrix);
	
	static unsigned frame_shadow_cull_id = 0;
	

	ATOM_ShadowMapCullVisitor cullVisitor (this);

	// 检查是否需要进行剔除检测
	static ATOM_Vector3f camera_dir = ATOM_Vector3f(0,1,0);
	const float angle_threshold = ATOM_RenderSettings::getCullUpdateViewDirThreshold();	// 5 degree
	bool bDirChangeFast = dotProduct( camera_dir, savedViewMatrix.getRow3(2) ) < angle_threshold;
	camera_dir = savedViewMatrix.getRow3(2);

	// 视点坐标的改变是否突兀
	static ATOM_Vector3f camera_pos = ATOM_Vector3f(0,0,0);
	float coordinate_threshold = ATOM_RenderSettings::getCullUpdateViewPosThreshold();
	coordinate_threshold = coordinate_threshold * coordinate_threshold;
	bool bViewPointChange = ( camera_pos - savedCameraPos ).getSquaredLength() > coordinate_threshold;
	camera_pos = savedCameraPos;

	bool bCull = true;/*( ATOM_RenderSettings::getShadowDrawItemUpdateFrameCache() < 2 ) || ATOM_RenderScheme::getForceCullUpdate() ||
				( bDirChangeFast ) ||
				( bViewPointChange ) ||
				( frame_shadow_cull_id == 0 ) || 
				( frame_shadow_cull_id % ATOM_RenderSettings::getShadowDrawItemUpdateFrameCache() == ATOM_RenderSettings::getShadowDrawItemUpdateFrameCache() / 2 );*/
	if( 1 )
	{
		for (unsigned i = 0; i < RenderScheme_ShadowMapSchemeCount; ++i)
		{
			_renderQueue[i]->clear ();
		}

		cullVisitor.setCamera (scene->getCamera());
		cullVisitor._terrainLODcamera = &LODcamera;
		cullVisitor.setUpdateVisibleStamp (false);
		cullVisitor.setNumRenderQueues (RenderScheme_ShadowMapSchemeCount);
		for (unsigned i = 0; i < RenderScheme_ShadowMapSchemeCount; ++i)
		{
			cullVisitor.setRenderQueue (i, _renderQueue[i]);
		}

		{
			PROFILING_SHADOW_START("shadow_cull_vistor_profiler");
			cullVisitor.traverse (*scene->getRootNode ());
		}

		if (cullVisitor._terrain)
		{
			cullVisitor._terrain->setLODCamera (0);
		}
		cullVisitor._terrainLODcamera = 0;
	}
	frame_shadow_cull_id++;
	//--------------------------//

	//--------- RENDER -----------//
	//--- wangjian modified ---//
	// * 添加对NULL RENDER TARGET的支持 *//
	int width = device->getCapabilities().null_rtf ? _shadowMap_dummy->getWidth() : _shadowMap->getWidth();
	int height = device->getCapabilities().null_rtf ? _shadowMap_dummy->getHeight() : _shadowMap->getHeight();

	scene->getCamera()->setViewMatrix (lightView);
	scene->getCamera()->setProjectionMatrix (lightProj);
	scene->getCamera()->setViewport (0, 0, width, height);

	// 设置RenderTargetSurface
	if( device->getCapabilities().null_rtf )
	{
		device->setRenderTargetSurface(0, _shadowMap_dummy.get());
	}
	else
	{
		device->setRenderTarget (0, _shadowMap.get());
	}
	//------------------------//
	device->setDepthBuffer ( _shadowDepthBuffer->getDepthBuffer());
	scene->getCamera()->synchronise (device, device->getCurrentView());
	device->setClearColor (0, 1.f, 1.f, 1.f, 1.f);
	device->clear (true, true, true);

	//--- wangjian added ---//
	// 阴影相关
	float fDepthbias = 0.0f;
	float fBiasSlope = 0.0f;
	if(_shadowSlopeBias>0.0f)
		fBiasSlope = _shadowSlopeBias;
	device->applySlopeScaleBias(fDepthbias,fBiasSlope);
	//----------------------//

	{
		PROFILING_SHADOW_START("Gen shadow map");
		_renderQueue[RenderScheme_ShadowMap]->SetName("ShadowGen");

#if 0
		ATOM_LOGGER::log("=================== ShadowGen is START drawing ===================\n");
#endif

		_renderQueue[RenderScheme_ShadowMap]->render (device, scene->getCamera(), ATOM_RenderQueue::SortNone, false);

#if 0
		ATOM_LOGGER::log("=================== ShadowGen is END drawing ===================\n");
#endif

		_shadowDPCount		= _renderQueue[RenderScheme_ShadowMap]->getDPCountApproximate();
		_shadowBatchCount	= _renderQueue[RenderScheme_ShadowMap]->getBatchCountApproximate();
	}
	
	//--- wangjian added ---//
	device->applySlopeScaleBias();
	//----------------------//

	//--- wangjian added ---//
	// * 添加对NULL RENDER TARGET的支持 *//
	if( device->getCapabilities().null_rtf )
	{
		device->setRenderTargetSurface(0, 0);
	}
	else
	{
		device->setRenderTarget(0, 0);
	}
	//----------------------//

	//----------------------//
	/*if( bCull )
	{
		if (cullVisitor._terrain)
		{
			cullVisitor._terrain->setLODCamera (0);
		}
	}*/
	/*frame_shadow_cull_id++;*/
	//----------------------//

	scene->getCamera()->setViewMatrix (savedViewMatrix);
	scene->getCamera()->setProjectionMatrix (savedProjMatrix);
	scene->getCamera()->setViewport (savedViewport.point.x, savedViewport.point.y, savedViewport.size.w, savedViewport.size.h);
	scene->getCamera()->synchronise (device, device->getCurrentView());
}

void ATOM_DeferredShadowMapRenderScheme::drawShadowMask (ATOM_RenderDevice *device, const ATOM_Matrix4x4f &lightViewProj)
{
	device->setRenderTarget (0, _shadowMask.get());
	device->setRenderTarget (1, 0);
	device->setViewport (NULL, 0, 0, _shadowMask->getWidth(), _shadowMask->getHeight());
	device->setClearColor (0, 1.f, 1.f, 1.f, 1.f);
	device->clear (true, false, false);

	ATOM_Vector4f fadeParam (_fadeMax, 1.f/(_fadeMax - _fadeMin), 1.f, 1.f);

	//--- wangjian modified ---//
	// * 添加对NULL RENDER TARGET的支持 *//
	int width_sm	= device->getCapabilities().null_rtf ? _shadowMap_dummy->getWidth() : _shadowMap->getWidth();
	ATOM_Vector4f mapParam (1.f / width_sm, width_sm/_rotationTexture->getWidth(), 0.f, 0.f);
	//-------------------------//

	_shadowMaskMaterial->getParameterTable()->setTexture (	"shadowMap",	
															_shadowDepthBuffer.get()	);
	_shadowMaskMaterial->getParameterTable()->setTexture ("rotationTexture", _rotationTexture.get());
	_shadowMaskMaterial->getParameterTable()->setMatrix44 ("lightViewProj", lightViewProj);
	_shadowMaskMaterial->getParameterTable()->setFloat ("shadowDistance", 1.f/getShadowDistance());
	_shadowMaskMaterial->getParameterTable()->setFloat ("sceneScale", getSceneScale());
	_shadowMaskMaterial->getParameterTable()->setFloat ("lightSize", getLightSize());
	_shadowMaskMaterial->getParameterTable()->setFloat ("bias", getDepthBias());
	_shadowMaskMaterial->getParameterTable()->setVector ("fadeParam", fadeParam);
	_shadowMaskMaterial->getParameterTable()->setVector ("mapParam", mapParam);
	//--- wangjian added : set the light dir in view space ---//
	// 添加一个太阳光在视空间中的方向矢量参数，用于计算阴影的BIAS值
	ATOM_Scene *currentScene = ATOM_RenderScheme::getCurrentScene ();
	ATOM_Matrix4x4f matViewTransInv;
	matViewTransInv.invertTransposeAffineFrom(currentScene->getCamera()->getInvViewMatrix());
	ATOM_Vector3f lightdirView = matViewTransInv >> _currentLightDir;
	lightdirView.normalize();
	_shadowMaskMaterial->getParameterTable()->setVector( "lightDirView", lightdirView );
	_shadowMaskMaterial->getParameterTable()->setFloat ("shadowBlur", _shadowBlur);
	_shadowMaskMaterial->getParameterTable()->setFloat ("shadowDensity", _shadowDensity);
	//--------------------------------------------------------//

	struct Vertex
	{
		float xyz[3];
		float uv[2];
	};

	float deltax = (float)0.5f/(float)_currentViewportWidth;
	float deltay = (float)0.5f/(float)_currentViewportHeight;
	const Vertex vertices[4] = {
		{ -1.f - deltax, -1.f + deltay, 0.f, 0.f, 1.f },
		{  1.f - deltax, -1.f + deltay, 0.f, 1.f, 1.f },
		{  1.f - deltax,  1.f + deltay, 0.f, 1.f, 0.f },
		{ -1.f - deltax,  1.f + deltay, 0.f, 0.f, 0.f }
	};

	const unsigned short indices[4] = {
		0, 1, 2, 3
	};

	unsigned numPasses = _shadowMaskMaterial->begin(device);
	if (numPasses)
	{
		for (unsigned pass = 0; pass < numPasses; ++pass)
		{
			if (_shadowMaskMaterial->beginPass (device, pass))
			{
				device->renderInterleavedMemoryStreamsIndexed (ATOM_PRIMITIVE_TRIANGLE_FAN, 4, 4, ATOM_VERTEX_ATTRIB_COORD|ATOM_VERTEX_ATTRIB_TEX1_2, sizeof(Vertex), vertices, indices);
				_shadowMaskMaterial->endPass (device, pass);
			}
		}
	}
	_shadowMaskMaterial->end (device);

	device->setRenderTarget (0, 0);
}

void ATOM_DeferredShadowMapRenderScheme::onResize (ATOM_RenderDevice *device, unsigned width, unsigned height)
{
	if (width == _currentViewportWidth && height == _currentViewportHeight)
	{
		return;
	}

	_shadowMask = 0;
	_shadowMap_dummy = 0;
	_shadowMap = 0;
	_shadowDepthBuffer = 0;

	_currentViewportWidth = width;
	_currentViewportHeight = height;

	_isOk = false;

	if (!_rotationTexture)
	{
		generateRotationTexture (32);
	}

	if (width > 0 && height > 0)
	{
		if (!_shadowMaskMaterial)
		{
			_shadowMaskMaterial = ATOM_MaterialManager::createMaterialFromCore (device, "/materials/builtin/stdshadowmap.mat");
			if (!_shadowMaskMaterial)
			{
				fini (device);
				ATOM_RenderSettings::enableShadow(false);
				return;
			}
			_shadowMaskMaterial->setActiveEffect ( device->getCapabilities().hardware_pcf ? "hardware_pcf" : "default" );
		}

		if (!_shadowMap)
		{
			//--- WANGJIAN MODIFIED ---//
			// * 添加对NULL RENDER TARGET的支持 *//
			if( device->getCapabilities().null_rtf )
			{
				_shadowMap_dummy = ATOM_GetRenderDevice()->allocRenderTargetSurface(ATOM_RenderSettings::getShadowMapSize(),
																					ATOM_RenderSettings::getShadowMapSize(),
																					ATOM_RenderTargetSurface::ATOM_RTSURFACE_FMT_NULLRT);
				if (!_shadowMap_dummy)
				{
					fini (device);
					ATOM_RenderSettings::enableShadow(false);
					return;
				}
			}
			else
			{
				int filtering;
				ATOM_PixelFormat format = ATOM_GetRenderDevice()->getBestHDRFormatR (&filtering);
				if (format == ATOM_PIXEL_FORMAT_UNKNOWN)
				{
					fini (device);
					ATOM_RenderSettings::enableShadow(false);
					return;
				}

				// 如果使用HARDWARE PCF，不需要浮点纹理的支持，因为深度测试只使用到_shadowDepthBuffer
				// _shadowMap实际上是个DUMMY RT
				_shadowMap = ATOM_GetRenderDevice()->allocTexture (	0, 0, 
																	ATOM_RenderSettings::getShadowMapSize(), 
																	ATOM_RenderSettings::getShadowMapSize(), 
																	device->getCapabilities().hardware_pcf ? ATOM_PIXEL_FORMAT_RGBA8888 : format, 
																	ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET );
				if (!_shadowMap)
				{
					fini (device);
					ATOM_RenderSettings::enableShadow(false);
					return;
				}
			}
			
		}

		if (!_shadowDepthBuffer)
		{
			//--- WANGJIAN MODIFIED FOR TEST ---//
			// 如果使用HARDWARE PCF，_shadowDepthBuffer需要使用DirectX 9 depth buffer formats
			// allocDepthTexture增加了一个用于设置该格式的参数
			bool bHardware_PCF = device->getCapabilities().hardware_pcf;
			_shadowDepthBuffer = ATOM_GetRenderDevice()->allocDepthTexture (	ATOM_RenderSettings::getShadowMapSize(), 
																				ATOM_RenderSettings::getShadowMapSize(), 
																				bHardware_PCF	);
			if (!_shadowDepthBuffer)
			{
				fini (device);
				ATOM_RenderSettings::enableShadow(false);
				return;
			}
		}

		// 
		int _shadowMaskWidth = width / ATOM_RenderSettings::getShadowMaskSizeScale();
		int _shadowMaskHeight = height / ATOM_RenderSettings::getShadowMaskSizeScale();
		_shadowMask = ATOM_GetRenderDevice ()->allocTexture (0, 0, _shadowMaskWidth, _shadowMaskHeight, ATOM_PIXEL_FORMAT_GREY8, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET|ATOM_Texture::NOMIPMAP);
		if (!_shadowMask)
		{
			fini (device);
			ATOM_RenderSettings::enableShadow(false);
			return;
		}
		// 清为白色 这样如果不渲染阴影 也不会导致场景变暗
		_shadowMask->clear(1,1,1,1);

		_isOk = true;
	}
}

void ATOM_DeferredShadowMapRenderScheme::calculateLightMatrices (const ATOM_Vector3f &lightDir, const ATOM_Vector3f &origin, float radius, ATOM_Matrix4x4f &viewMatrix, ATOM_Matrix4x4f &projMatrix) const
{
	ATOM_Vector3f lightPos = origin - lightDir * _lightDistance;

	int i = 0;
	if (ATOM_abs(lightDir.y) < ATOM_abs(lightDir.x)) i = 1;
	if (ATOM_abs(lightDir.z) < ATOM_abs(lightDir.xyz[i])) i = 2;
	ATOM_Vector3f lightUp(0.f, 0.f, 0.f);
	lightUp.xyz[i] = 1.f;
	viewMatrix.makeLookatLH (lightPos, origin, lightUp);

	projMatrix.makeOrthoLH (radius * 2.f, radius * 2.f, 0.f, _lightDistance + radius);
}

ATOM_Texture *ATOM_DeferredShadowMapRenderScheme::getShadowMap (void) const
{
	//return _shadowDepthBuffer.get();
	return _shadowMap.get();
}

ATOM_Texture *ATOM_DeferredShadowMapRenderScheme::getShadowMask (void) const
{
	if( _shadowMask )
		return _shadowMask.get();
	else
		return ATOM_GetColorTexture (0xFFFFFFFF);
}

void ATOM_DeferredShadowMapRenderScheme::setShadowDistance (float distance)
{
	_shadowDistance = distance;
}

float ATOM_DeferredShadowMapRenderScheme::getShadowDistance (void) const
{
	return _shadowDistance;
}

void ATOM_DeferredShadowMapRenderScheme::setLightDistance (float lightDistance)
{
	_lightDistance = lightDistance;
}

float ATOM_DeferredShadowMapRenderScheme::getLightDistance (void) const
{
	return _lightDistance;
}

void ATOM_DeferredShadowMapRenderScheme::setSceneScale (float value)
{
	_sceneScale = value;
}

float ATOM_DeferredShadowMapRenderScheme::getSceneScale (void) const
{
	return _sceneScale;
}

void ATOM_DeferredShadowMapRenderScheme::setLightSize (float value)
{
	_lightSize = value;
}

float ATOM_DeferredShadowMapRenderScheme::getLightSize (void) const
{
	return _lightSize;
}

void ATOM_DeferredShadowMapRenderScheme::setDepthBias (float value)
{
	_depthBias = value;
}

float ATOM_DeferredShadowMapRenderScheme::getDepthBias (void) const
{
	return _depthBias;
}

void ATOM_DeferredShadowMapRenderScheme::setFadeMin (float fadeMin)
{
	_fadeMin = fadeMin;
}

float ATOM_DeferredShadowMapRenderScheme::getFadeMin (void) const
{
	return _fadeMin;
}

void ATOM_DeferredShadowMapRenderScheme::setFadeMax (float fadeMax)
{
	_fadeMax = fadeMax;
}

float ATOM_DeferredShadowMapRenderScheme::getFadeMax (void) const
{
	return _fadeMax;
}
//--- wangjian added ---//
// 阴影相关
void ATOM_DeferredShadowMapRenderScheme::setShadowBlur (float blur)
{
	_shadowBlur = blur;
}
float ATOM_DeferredShadowMapRenderScheme::getShadowBlur (void) const
{
	return _shadowBlur;
}
void ATOM_DeferredShadowMapRenderScheme::setShadowSlopeBias(float bias)
{
	_shadowSlopeBias = bias;
}
float ATOM_DeferredShadowMapRenderScheme::getShadowSlopeBias (void) const
{
	return _shadowSlopeBias;
}
void ATOM_DeferredShadowMapRenderScheme::setShadowDensity (float density)
{
	_shadowDensity = density;
}
float ATOM_DeferredShadowMapRenderScheme::getShadowDensity (void) const
{
	return _shadowDensity;
}
void ATOM_DeferredShadowMapRenderScheme::generateWhiteShadowMask()
{
	if( ATOM_RenderSettings::isShadowEnabled() )
		return;

	/*if( !_shadowMask )
	{*/
		_shadowMask = ATOM_GetColorTexture (0xFFFFFFFF);
	//}
}
//---------------------//
void ATOM_DeferredShadowMapRenderScheme::generateRotationTexture (int size)
{
	ATOM_ColorARGB *data = ATOM_NEW_ARRAY(ATOM_ColorARGB, size * size);
	for (unsigned i = 0; i < size * size; ++i)
	{
		float rotateAngle = ATOM_TwoPi * (float(rand())/float(RAND_MAX));
		float s = ATOM_saturate (ATOM_sin (rotateAngle) * 0.5f + 0.5f);
		float c = ATOM_saturate (ATOM_cos (rotateAngle) * 0.5f + 0.5f);
		data[i].setFloats (s, c, 0.f, 0.f);
	}
	_rotationTexture = ATOM_GetRenderDevice()->allocTexture (0, data, size, size, ATOM_PIXEL_FORMAT_BGRA8888, ATOM_Texture::TEXTURE2D|ATOM_Texture::NOMIPMAP|ATOM_Texture::NOCOMPRESS);
}

//=== wangjian added ===//
void ATOM_DeferredShadowMapRenderScheme::UpdateLightViewProj(	const ATOM_Vector3f & lightPos,
																const ATOM_BBox & sceneBBox	)
{
	int i = 0;
	if (ATOM_abs(_currentLightDir.y) < ATOM_abs(_currentLightDir.x)) i = 1;
	if (ATOM_abs(_currentLightDir.z) < ATOM_abs(_currentLightDir.xyz[i])) i = 2;
	ATOM_Vector3f lightUp(0.f, 0.f, 0.f);
	lightUp.xyz[i] = 1.f;
	/*_lightViewMatrix*/_lightViewMatOrig.makeLookatLH (lightPos, sceneBBox.getCenter(), lightUp);
	
	float maxZ = .0f, minZ = FLT_MAX, maxDim = .0f;

	ATOM_Vector3f viewDir(sceneBBox.getCenter() - lightPos);
	viewDir.normalize();
	
	const ATOM_Vector3f downVec(-lightUp);
	const ATOM_Vector3f leftVec( crossProduct(viewDir, lightUp) );
	const ATOM_Vector3f rightVec(-leftVec);

	// for each point in AABB
	const ATOM_Vector3f & SceneBoxMin = sceneBBox.getMin();
	const ATOM_Vector3f & SceneBoxMax = sceneBBox.getMax();
	const ATOM_Vector3f Corners[8] = {  SceneBoxMin,
										ATOM_Vector3f(SceneBoxMin.x,SceneBoxMin.y,SceneBoxMax.z), 
										ATOM_Vector3f(SceneBoxMin.x,SceneBoxMax.y,SceneBoxMin.z),
										ATOM_Vector3f(SceneBoxMin.x,SceneBoxMax.y,SceneBoxMax.z),
										ATOM_Vector3f(SceneBoxMax.x,SceneBoxMin.y,SceneBoxMin.z),
										ATOM_Vector3f(SceneBoxMax.x,SceneBoxMin.y,SceneBoxMax.z),
										ATOM_Vector3f(SceneBoxMax.x,SceneBoxMax.y,SceneBoxMin.z),
										SceneBoxMax	
										};
	for (unsigned int i = 0; i < 8; ++i)
	{
		const ATOM_Vector3f corner = Corners[i];
		const ATOM_Vector3f vPointToCam = corner - lightPos;

		const float dist = dotProduct(vPointToCam, viewDir);
		maxZ = max(dist, maxZ);
		minZ = min(dist, minZ);

		maxDim = max(abs(dotProduct(vPointToCam, leftVec)), maxDim);
		maxDim = max(abs(dotProduct(vPointToCam, rightVec)), maxDim);
		maxDim = max(abs(dotProduct(vPointToCam, lightUp)), maxDim);
		maxDim = max(abs(dotProduct(vPointToCam, downVec)), maxDim);
	}

	const float zNear = max(minZ, 1.0f);
	const float zFar  = max(maxZ, zNear + 1.0f);

	/*_lightProjMatrix*/_lightProjMatOrig.makeOrthoFrustumLH ( -maxDim, maxDim, -maxDim, maxDim, zNear, zFar );
}
void ATOM_DeferredShadowMapRenderScheme::CalcPolygonBodyBDirectional(	const ATOM_BBox& sBB	)
{
	_intersectionBody.Clear();

	ATOM_Scene *currentScene = ATOM_RenderScheme::getCurrentScene ();
	ATOM_Vector3f eye,at,up;
	currentScene->getCamera ()->getCameraPos(&eye,&at,&up);
	float tanHalfFovY = currentScene->getCamera ()->getTanHalfFovy();
	float aspect = currentScene->getCamera ()->getAspect();
	float cameraNear = currentScene->getCamera()->getNearPlane();
	float cameraFar = currentScene->getCamera()->getFarPlane();
	float shadowDist = getShadowDistance();
	_intersectionBody.AddFrustum(	eye,at,up, 
									tanHalfFovY, aspect,
									cameraNear, 
									/*cameraFar*/ATOM_min2(cameraFar,shadowDist ) );

	// directional light
	_intersectionBody.Clip(sBB);
	_intersectionBody.Extrude(_currentLightDir, sBB);
}

void ATOM_DeferredShadowMapRenderScheme::UpdateDirLightViewProj(	const ATOM_Scene * scene	)
{
	ATOM_Vector3f eye,at,up;
	scene->getCamera()->getCameraPos(&eye,&at,&up);
	ATOM_Vector3f eyeVector = at - eye;
	ATOM_Vector3f leftVec(crossProduct(up, eyeVector));
	leftVec.normalize();
	ATOM_Vector3f upVec(crossProduct(- _currentLightDir,leftVec));
	upVec.normalize();

	_lightViewMatrix.makeLookatLH( eye, eye - _currentLightDir, upVec );

	_lightProjMatrix = _lightProjMatOrig;
}

void ATOM_DeferredShadowMapRenderScheme::CalcLightViewProjMat()
{
	
	ATOM_Matrix4x4f lViewProjMat = _lightViewMatrix;
	lViewProjMat.invertAffine();
	lViewProjMat <<= _lightProjMatrix;

	unsigned int size = _intersectionBody.GetVertexCount();
	const Polygon3d::tVertexArray& vertices = _intersectionBody.GetVertexArray();

	// map to unit cube
	ATOM_BBox bodyAABB(FLT_MAX,-FLT_MAX);
	for (unsigned int i = 0; i < size; ++i)
	{
		/*ATOM_Vector3f transformed = lViewProjMat >> vertices[i];
		bodyAABB.extend( transformed );*/
		ATOM_Vector4f transformed = lViewProjMat >> ATOM_Vector4f(vertices[i].x,vertices[i].y,vertices[i].z,1);
		bodyAABB.extend( ATOM_Vector3f( transformed.x/transformed.w,transformed.y/transformed.w,transformed.z/transformed.w) );
	}

	ATOM_Vector3f maximum = bodyAABB.getMax();
	ATOM_Vector3f minimum = bodyAABB.getMin();

	// maximize z for more robust shadows to avoid unwanted far plane clipping

	maximum.z = 1.0f;

	const float diffX =  maximum.x - minimum.x;
	const float diffY =  maximum.y - minimum.y;
	const float diffZ =  maximum.z - minimum.z;

	const float sumX = maximum.x + minimum.x;
	const float sumY = maximum.y + minimum.y;
	//const float sumZ = maximum.z + minimum.z;

	ATOM_Vector3f scale(2.0f / diffX, 2.0f / diffY, 1.0f / diffZ);
	ATOM_Vector3f offset(-sumX / diffX, -sumY / diffY, -minimum.z / diffZ);

	//if (_reduceSwimming && !(_useLiSP || _updateLightUp))
	{
	/*float scaleQuantizer = 64.0f;
	scale.x = 1.0f / ceil(1.0f / scale.x * scaleQuantizer) * scaleQuantizer;
	scale.y = 1.0f / ceil(1.0f / scale.y * scaleQuantizer) * scaleQuantizer;

	offset.x = -0.5f * (sumX) * scale.x;
	offset.y = -0.5f * (sumY) * scale.y;

	float halfTexSz = 0.5f * _shadowMap_dummy->getWidth();
	offset.x = ceil(offset.x * halfTexSz) / halfTexSz;
	offset.y = ceil(offset.y * halfTexSz) / halfTexSz;*/
	}

	ATOM_Matrix4x4f scaleTranslate
	(
		scale.x,	.0,			.0,			.0,
		.0,			scale.y,	.0,			.0,
		.0,			.0,			scale.z,	.0,
		offset.x,	offset.y,	offset.z,	1.0
	);

	_lightProjMatrix = scaleTranslate >> _lightProjMatrix;
}
//================================//

/////////////////////////////////////////////////////////////////////////////////////////////////

ATOM_ShadowMapRenderSchemeFactory::ATOM_ShadowMapRenderSchemeFactory (void)
{
}

ATOM_ShadowMapRenderSchemeFactory::~ATOM_ShadowMapRenderSchemeFactory (void)
{
}

ATOM_RenderScheme *ATOM_ShadowMapRenderSchemeFactory::create (void)
{
	return ATOM_NEW(ATOM_DeferredShadowMapRenderScheme, this);
}

void ATOM_ShadowMapRenderSchemeFactory::destroy (ATOM_RenderScheme *renderScheme)
{
	ATOM_DELETE(renderScheme);
}

int ATOM_ShadowMapRenderSchemeFactory::getLayerNumber (const char *layerName) const
{
	if (!layerName)
	{
		return -1;
	}

	if (!stricmp (layerName, "ShadowMapGen"))
	{
		return ATOM_DeferredShadowMapRenderScheme::RenderScheme_ShadowMap;
	}
	else if (!stricmp (layerName, "ShadowMask"))
	{
		return ATOM_DeferredShadowMapRenderScheme::RenderScheme_ShadowMask;
	}

	return -1;
}

const char *ATOM_ShadowMapRenderSchemeFactory::getLayerName (int layerNumber) const
{
	switch (layerNumber)
	{
	case ATOM_DeferredShadowMapRenderScheme::RenderScheme_ShadowMap:
		return "ShadowMapGen";
	case ATOM_DeferredShadowMapRenderScheme::RenderScheme_ShadowMask:
		return "ShadowMask";
	default:
		return 0;
	}
}

//--- wangjian added ---//
unsigned ATOM_ShadowMapRenderSchemeFactory::getLayerCount() const
{
	return ATOM_DeferredShadowMapRenderScheme::RenderScheme_ShadowMapSchemeCount;
}
unsigned ATOM_ShadowMapRenderSchemeFactory::getLayerMaskId( int layer ) const
{
	return 1 << layer;
}
//----------------------//

