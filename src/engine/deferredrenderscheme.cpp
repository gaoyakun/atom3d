#include "StdAfx.h"
#include "deferred_cullvisitor.h"
#include "deferredrenderscheme.h"
#include "shadowmaprenderscheme.h"
#include "parameterchannel.h"
#include "hdr_pipe.h"

//--- wangjian added ---//
#include "postEffectSSAO.h"

#define DEBUG_RENDERQUEUE 1

#if DEBUG_RENDERQUEUE

#include "kernel/profiler.h"

#define DEBUG_RENDERQUEUE_LOG_START(render_pass_name) \
	const char* flag = ATOM_RenderSettings::isStateSortingEnabled() ? "_state_sorting_enabled":"_state_sorting_disabled"; \
	ATOM_STRING name = render_pass_name; \
	name += flag; \
	{ ATOM_Profiler profile(name.c_str()); 
	
#define DEBUG_RENDERQUEUE_LOG_END(render_pass_name) \
	}

#else

#define DEBUG_RENDERQUEUE_LOG_START(render_pass_name)
#define DEBUG_RENDERQUEUE_LOG_END(render_pass_name)

#endif
//----------------------//

static const unsigned omniAttrib = ATOM_VERTEX_ATTRIB_COORD|ATOM_VERTEX_ATTRIB_TEX1_3|ATOM_VERTEX_ATTRIB_TEX2_3;
// wangjian added
static const unsigned omniAttrib_V2 = ATOM_VERTEX_ATTRIB_COORD|ATOM_VERTEX_ATTRIB_TEX1_4;
ATOM_DeferredRenderScheme *currentDeferredScheme = 0;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// wangjian added
#if 1
ATOM_AUTOREF(ATOM_Texture)		ATOM_DeferredRenderScheme::_depthTexture = 0;
ATOM_AUTOREF(ATOM_Texture)		ATOM_DeferredRenderScheme::_normalTexture = 0;
ATOM_AUTOREF(ATOM_Texture)		ATOM_DeferredRenderScheme::_albedoTexture = 0;
ATOM_AUTOREF(ATOM_Texture)		ATOM_DeferredRenderScheme::_lightAccTexture = 0;
ATOM_AUTOREF(ATOM_Texture)		ATOM_DeferredRenderScheme::_shadedTexture = 0;
ATOM_AUTOREF(ATOM_Texture)		ATOM_DeferredRenderScheme::_ldrResult[2] = {0};
ATOM_AUTOPTR(ATOM_Material)		ATOM_DeferredRenderScheme::_deferredMaterial = 0;
ATOM_VertexDecl					ATOM_DeferredRenderScheme::_vertexDeclOmni = 0;
ATOM_AUTOREF(ATOM_VertexArray)	ATOM_DeferredRenderScheme::_vertexArrayOmni = 0;
ATOM_VertexDecl					ATOM_DeferredRenderScheme::_vertexDeclOmni_V2 = 0;
ATOM_AUTOREF(ATOM_VertexArray)	ATOM_DeferredRenderScheme::_vertexArrayOmni_V2 = 0;
ATOM_AUTOREF(ATOM_IndexArray)	ATOM_DeferredRenderScheme::_indexArrayOmni = 0;
bool							ATOM_DeferredRenderScheme::_isOk = false;
#endif
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

ATOM_DeferredRenderScheme::ATOM_DeferredRenderScheme (ATOM_RenderSchemeFactory *factory)
: ATOM_RenderScheme (factory)
{
	for (unsigned i = 0; i < RenderScheme_DeferredSchemeCount; ++i)
	{
		_renderQueue[i] = ATOM_NEW(ATOM_RenderQueue);
		//--- wangjian added ---//
		_renderQueue[i]->setQueueId(i);
		//----------------------//
	}
	_currentViewportWidth = 0;
	_currentViewportHeight = 0;

	// wangjian modified
#if 0
	_isOk = true;
#endif
	/////////////////////

	_drawGbufferOnly = false;
	_ambientOnly = false;
	_isTransparent = false;
	
	// wangjian modified
#if 0
	_vertexDeclOmni = 0;
#endif
	/////////////////////

	_sunLight = 0;
	_shadowMapRenderScheme = (ATOM_DeferredShadowMapRenderScheme*)ATOM_RenderScheme::createRenderScheme ("shadowmap");
	_HDRPipeline = ATOM_NEW(HDR_Pipeline);
	_cullVisitor = 0;

	//--- wangjian added ---//
	_mat_gammacorrect = 0;
	_deferredSchemeFlag = DSF_NONE;
	_transparentMaskTextureFile = "";
	_transparentMaskTexture = ATOM_GetColorTexture(0);
	//----------------------//
}

ATOM_DeferredRenderScheme::~ATOM_DeferredRenderScheme (void)
{
	for (unsigned i = 0; i < RenderScheme_DeferredSchemeCount; ++i)
	{
		ATOM_DELETE(_renderQueue[i]);
	}
	ATOM_RenderScheme::destroyRenderScheme (_shadowMapRenderScheme);
}

ATOM_DeferredShadowMapRenderScheme *ATOM_DeferredRenderScheme::getShadowMapRenderScheme (void) const
{
	return _shadowMapRenderScheme;
}

bool ATOM_DeferredRenderScheme::init (ATOM_RenderDevice *device, unsigned width, unsigned height)
{
	resize (device, width, height);

	return true;
}

void ATOM_DeferredRenderScheme::registerTextureCallbacks (void)
{
	ATOM_ParameterChannel::registerChannel ("Gbuffer_DepthTexture", &depthTexture_Callback, 0, 0, ATOM_MaterialParam::ParamType_Texture);
	ATOM_ParameterChannel::registerChannel ("Gbuffer_NormalTexture", &normalTexture_Callback, 0, 0, ATOM_MaterialParam::ParamType_Texture);
	ATOM_ParameterChannel::registerChannel ("Gbuffer_AlbedoTexture", &albedoTexture_Callback, 0, 0, ATOM_MaterialParam::ParamType_Texture);
	ATOM_ParameterChannel::registerChannel ("Deferred_LightBuffer", &lightBuffer_Callback, 0, 0, ATOM_MaterialParam::ParamType_Texture);
	ATOM_ParameterChannel::registerChannel ("Deferred_ShadedTexture", &shadedTexture_Callback, 0, 0, ATOM_MaterialParam::ParamType_Texture);
	ATOM_ParameterChannel::registerChannel ("Deferred_SunShadowTexture", &sunShadowTexture_Callback, 0, 0, ATOM_MaterialParam::ParamType_Texture);
	ATOM_ParameterChannel::registerChannel ("Deferred_CompositionTexture", &composTexture_Callback, 0, 0, ATOM_MaterialParam::ParamType_Texture);
	ATOM_ParameterChannel::registerChannel ("Deferred_LinearizeDepthParam", &linearizeDepthParam_Callback, 0, 0, ATOM_MaterialParam::ParamType_Vector);
	ATOM_ParameterChannel::registerChannel ("Deferred_SizeParam", &sizeParam_Callback, 0, 0, ATOM_MaterialParam::ParamType_Vector);
	ATOM_ParameterChannel::registerChannel ("Deferred_LightColor", &lightColor_Callback, 0, 0, ATOM_MaterialParam::ParamType_Vector);
	ATOM_ParameterChannel::registerChannel ("Deferred_LightDir", &lightDir_Callback, 0, 0, ATOM_MaterialParam::ParamType_Vector);
	ATOM_ParameterChannel::registerChannel ("Camera_ViewAspect", &viewAspect_Callback, 0, 0, ATOM_MaterialParam::ParamType_Float);
	ATOM_ParameterChannel::registerChannel ("Camera_InvTanHalfFov", &invTanHalfFov_Callback, 0, 0, ATOM_MaterialParam::ParamType_Float);
	ATOM_ParameterChannel::registerChannel ("SceneAmbient", &ambient_Callback, 0, 0, ATOM_MaterialParam::ParamType_Vector);
	ATOM_ParameterChannel::registerChannel ("SunLightDir", &sunLightDir_Callback, 0, 0, ATOM_MaterialParam::ParamType_Vector);
	ATOM_ParameterChannel::registerChannel ("SunLightColor", &sunLightColor_Callback, 0, 0, ATOM_MaterialParam::ParamType_Vector);
	//--- wangjian added ---//
	ATOM_ParameterChannel::registerChannel ("InvScreenSize", &invScreenSize_Callback, 0, 0, ATOM_MaterialParam::ParamType_Vector);
	ATOM_ParameterChannel::registerChannel ("Use_HDR", &useHDR_Callback, 0, 0, ATOM_MaterialParam::ParamType_Float);
	ATOM_ParameterChannel::registerChannel ("Deferred_ShadedSceneTexture", &shadedSceneTexture_Callback, 0, 0, ATOM_MaterialParam::ParamType_Texture);
	ATOM_ParameterChannel::registerChannel ("Deferred_WaterCausticsTexture", &waterCausticsTexture_Callback, 0, 0, ATOM_MaterialParam::ParamType_Texture);
	ATOM_ParameterChannel::registerChannel ("Deferred_ScreenCoordScaleBias", &screenCoordScaleBias_Callback, 0, 0, ATOM_MaterialParam::ParamType_Vector);
	//ATOM_ParameterChannel::registerChannel ("Deferred_SSSBRDFTexture", &SSSBRDFTexture_Callback, 0, 0, ATOM_MaterialParam::ParamType_Texture);
	//ATOM_ParameterChannel::registerChannel ("Use_SSS", &useSSS_Callback, 0, 0, ATOM_MaterialParam::ParamType_Float);
	ATOM_ParameterChannel::registerChannel ("SSAOTexture", &ssaoTexture_Callback, 0, 0, ATOM_MaterialParam::ParamType_Texture);
	//----------------------//
}

void ATOM_DeferredRenderScheme::unregisterTextureCallbacks (void)
{
	ATOM_ParameterChannel::unregisterChannel ("Gbuffer_DepthTexture");
	ATOM_ParameterChannel::unregisterChannel ("Gbuffer_NormalTexture");
	ATOM_ParameterChannel::unregisterChannel ("Gbuffer_AlbedoTexture");
	ATOM_ParameterChannel::unregisterChannel ("Deferred_LightBuffer");
	ATOM_ParameterChannel::unregisterChannel ("Deferred_ShadedTexture");
	ATOM_ParameterChannel::unregisterChannel ("Deferred_SunShadowTexture");
	ATOM_ParameterChannel::unregisterChannel ("Deferred_CompositionTexture");
	ATOM_ParameterChannel::unregisterChannel ("Deferred_LinearizeDepthParam");
	ATOM_ParameterChannel::unregisterChannel ("Deferred_SizeParam");
	ATOM_ParameterChannel::unregisterChannel ("Deferred_LightColor");
	ATOM_ParameterChannel::unregisterChannel ("Deferred_LightDir");
}

bool ATOM_DeferredRenderScheme::onRender (ATOM_RenderDevice *device, ATOM_Scene *scene)
{
	if (_isOk)
	{
		currentDeferredScheme = this;

		scene->setRenderScheme (this);

		if (_deferredMaterial)
		{
			if( _transparentMaskTexture )
				_deferredMaterial->getParameterTable()->setTexture("TransMaskTexture",_transparentMaskTexture.get());
			else
				_deferredMaterial->getParameterTable()->setTexture("TransMaskTexture",ATOM_GetColorTexture(0));
		}

		// first update 
		{
			ATOM_UpdateVisitor updateVisitor;
			updateVisitor.setCamera (scene->getCamera ());

			{
				DEBUG_RENDERQUEUE_LOG_START("UpdateVisit");

				updateVisitor.traverse (*scene->getRootNode ());

				DEBUG_RENDERQUEUE_LOG_END("UpdateVisit");
			}
		}

		ATOM_AUTOREF(ATOM_Texture) oldRenderTarget = device->getRenderTarget (0);
		ATOM_AUTOREF(ATOM_DepthBuffer) oldDepthBuffer = device->getDepthBuffer ();
		ATOM_Rect2Di oldViewport = device->getViewport (NULL);

		if (!_drawGbufferOnly)
		{
			// wangjian modified 如果开启阴影渲染
			if( ATOM_RenderSettings::isShadowEnabled() && !(_deferredSchemeFlag & DSF_NOSHADOW )  )
			{
				// 如果还未初始化 初始化
				_shadowMapRenderScheme->init(device,_currentViewportWidth,_currentViewportHeight);

				//draw shadowmap first
				{
					scene->setRenderScheme (_shadowMapRenderScheme);
					ATOM_RenderScheme::setCurrentRenderSchemeLayer(0);
				}

				_shadowMapRenderScheme->setRenderMode (ATOM_DeferredShadowMapRenderScheme::RenderMode_ShadowMap);
				scene->render (device, true);

				{
					ATOM_RenderScheme::setCurrentRenderSchemeLayer(-1);
				}
			}
			else
			{
				// 销毁资源
				_shadowMapRenderScheme->fini(device);
			}
		}

		scene->setRenderScheme (this);

		//----------------------------- CULLING --------------------------------//
		static unsigned frame_cull_id = 0;

		// 视角的改变是否突兀
		static ATOM_Vector3f camera_dir = ATOM_Vector3f(0,1,0);
		const float angle_threshold = ATOM_RenderSettings::getCullUpdateViewDirThreshold(); // 5 degree
		const ATOM_Matrix4x4f & mat_view = scene->getCamera ()->getViewMatrix();
		bool bDirChangeFast = dotProduct( camera_dir, mat_view.getRow3(2) ) < angle_threshold;
		camera_dir = mat_view.getRow3(2);

		// 视点坐标的改变是否突兀
		static ATOM_Vector3f camera_pos = ATOM_Vector3f(0,0,0);
		float coordinate_threshold = ATOM_RenderSettings::getCullUpdateViewPosThreshold();
		coordinate_threshold = coordinate_threshold * coordinate_threshold;
		const ATOM_Vector3f & view_pos = scene->getCamera ()->getPosition();
		bool bViewPointChange = ( camera_pos - view_pos ).getSquaredLength() > coordinate_threshold;
		camera_pos = view_pos;

		// 是否进行提出检测
		ATOM_DeferredCullVisitor v(this);
		ATOM_DeferredCullVisitor *cullVisitor = _cullVisitor ? _cullVisitor : &v;

		bool bCull =	( ATOM_RenderSettings::getSceneDrawItemUpdateFrameCache() < 2 ) || ATOM_RenderScheme::getForceCullUpdate() ||
						( bDirChangeFast ) || 
						( bViewPointChange ) ||
						( frame_cull_id == 0 ) || 
						( frame_cull_id % ATOM_RenderSettings::getSceneDrawItemUpdateFrameCache() == 0 );
		if( 1 )
		{
			_omniLights.resize (0);
			_directionalLights.resize (0);
			_spotLights.resize (0);
			_sunLight = 0;

			for (unsigned i = 0; i < RenderScheme_DeferredSchemeCount; ++i)
			{
				_renderQueue[i]->clear ();
			}

			cullVisitor->setCamera (scene->getCamera ());
			cullVisitor->setUpdateVisibleStamp (true);
			cullVisitor->setNumRenderQueues (RenderScheme_DeferredSchemeCount);
			for (unsigned i = 0; i < RenderScheme_DeferredSchemeCount; ++i)
			{
				cullVisitor->setRenderQueue (i, _renderQueue[i]);
			}

			{
				DEBUG_RENDERQUEUE_LOG_START("CullVisit");
				cullVisitor->traverse (*scene->getRootNode ());
				DEBUG_RENDERQUEUE_LOG_END("CullVisit");
			}

			if( ATOM_RenderScheme::getForceCullUpdate() )
				ATOM_RenderScheme::setForceCullUpdate(false);
		}
		frame_cull_id++;
		//----------------------------------------------------------------//
		
		

		int pass = RenderScheme_GbufferConstruction;
		while (pass >= 0)
		{
			pass = drawPass (device, scene, pass);
		}

		if (_drawGbufferOnly)
		{
			return true;
		}

		// 之前所有操作的最终结果保存在_ldrResult[0]中

		//====================================== wangjian 后处理 ===========================================//
		ATOM_AUTOREF(ATOM_Texture) showTexture = _ldrResult[0];

		if( ATOM_RenderSettings::isPostEffectEnabled() )
		{
			ATOM_PostEffectChain *postEffectChain = getPostEffectChain();
			// 设置是否开启了HDR或者使用浮点线性的shadingRT
			bool bUseFloatingShading =	ATOM_RenderSettings::isHDRRenderingEnabled() || 
				ATOM_RenderSettings::isFloatShadingEnabled();
			postEffectChain->setHDREnabled(bUseFloatingShading);
			if (postEffectChain->getNumPostEffects() > 0)
			{
				postEffectChain->setInputTexture (_ldrResult[0].get());
				postEffectChain->setOutputTexture (_ldrResult[1].get());
				showTexture = postEffectChain->render (device);
			}
		}
		
		if (showTexture != _ldrResult[0])
		{
			std::swap (_ldrResult[0], _ldrResult[1]);
			int d = 0;
			if (d)
			{
				_ldrResult[0]->saveToFile ("/ldr1.png");
			}
		}

		drawHUD (device, scene->getCamera ());


		device->setRenderTarget (0, oldRenderTarget.get());
		device->setDepthBuffer (oldDepthBuffer.get());
		device->setViewport (NULL, oldViewport);

		_deferredMaterial->setActiveEffect (_isTransparent ? "framebuffer_transparent_blit" : "framebuffer_blit");
		ATOM_AUTOREF(ATOM_Texture) save = _ldrResult[0];

		if (ATOM_RenderSettings::isDebugDeferredColorsEnabled ())
		{
			_deferredMaterial->setActiveEffect ("framebuffer_srgb_blit");
			_ldrResult[0] = _albedoTexture;
		}
		else if (ATOM_RenderSettings::isDebugShadowMapEnabled())
		{
			_deferredMaterial->setActiveEffect ("framebuffer_depth_blit");
			_ldrResult[0] = _shadowMapRenderScheme->getShadowMap ();
		}
		else if (ATOM_RenderSettings::isDebugDeferredNormalsEnabled ())
		{
			_deferredMaterial->setActiveEffect ("framebuffer_expand_blit");
			_ldrResult[0] = _normalTexture;
		}
		
		else if (ATOM_RenderSettings::isDebugDeferredLightBufferEnabled ())
		{
			// 这里只有在不使用浮点格式SHADING的时候才会开启
			// _lightAccTexture是整型格式RT
			if( false == ATOM_RenderSettings::isHDRRenderingEnabled() && false == ATOM_RenderSettings::isFloatShadingEnabled() )
			{
				_deferredMaterial->setActiveEffect ("framebuffer_lightbufferLDR_blit");
				_ldrResult[0] = _lightAccTexture;
			}
			else
			{
				_deferredMaterial->setActiveEffect ("framebuffer_srgb_blit");
				_ldrResult[0] = _lightAccTexture;
			}
		}
		
		else if (ATOM_RenderSettings::isDebugDeferredDepthEnabled ())
		{
			_deferredMaterial->setActiveEffect ("framebuffer_depth_blit");
			_ldrResult[0] = _depthTexture;
		}
		else if (ATOM_RenderSettings::isDebugShadowMaskEnabled ())
		{
			_ldrResult[0] = _shadowMapRenderScheme->getShadowMask();
		}
		//--- wangjian added for debug KD/KS ---//
		// 调试HALFLAMBERT
		else if (ATOM_RenderSettings::isDebugHalfLambertBufferEnabled ())
		{
			_deferredMaterial->setActiveEffect ("framebuffer_unpackHalfLambert_blit");
			_ldrResult[0] = _normalTexture;
		}
		else if(ATOM_RenderSettings::isDebugHdrLuminEnabled())
		{
			ATOM_AUTOREF(ATOM_Texture) lumin = _HDRPipeline->getAdaptedLumCurrent();
			if( lumin )
			{
				_deferredMaterial->setActiveEffect ("framebuffer_one_pixel");
				_ldrResult[0] = lumin;
			}
		}
		//--------------------------------------//


		int n = 0;
		if (n)
		{
			_ldrResult[0]->saveToFile ("/ldrResult.jpg");
		}

#if 0
		float u1 = (float)_currentViewportWidth / _albedoTexture->getWidth();
		float v1 = (float)_currentViewportHeight / _albedoTexture->getHeight();
		drawScreenQuad (device, _deferredMaterial.get(),0,0,u1,v1);
#else
		drawScreenQuad (device, _deferredMaterial.get());
#endif

		_ldrResult[0] = save;

		if(ATOM_RenderSettings::isDebugHdrRTsEnabled())
		{
			_HDRPipeline->DebugAllRT(device);
		}


		//////////////////////////////////////////////////////////////////////////////
		//TestCustomRenderScheme(device,scene);
	
	}

	return _isOk;
}

void ATOM_DeferredRenderScheme::TestCustomRenderScheme(ATOM_RenderDevice *device, ATOM_Scene *scene)
{
	//return;

#if 1
	static bool mainPass = true;
	if( !mainPass )
	{
		mainPass = !mainPass;
		return;
	}

	mainPass = !mainPass;

	static ATOM_DeferredScene * _scene = 0;

	static bool initScene = false;
	if( !initScene )
	{
		initScene = true;

		_scene = ATOM_NEW(ATOM_DeferredScene);
		if (!_scene->load ("/map/roleprop.3sg"))
		{
			ATOM_DELETE(_scene);
			_scene = 0;
			return;
		}

		const ATOM_Matrix4x4f *cameraMatrix = _scene->getCameraMatrix();
		ATOM_Matrix4x4f _camera = *cameraMatrix;

		const char *placeholderName = "$roleprop";
		if (!placeholderName)
		{
			ATOM_DELETE(_scene);
			_scene = 0;
			return;
		}

		class PlaceHolderFindVisitor: public ATOM_Visitor
		{
			ATOM_STRING _name;
			ATOM_Node *_node;

		public:
			PlaceHolderFindVisitor (const char *name): _name(name), _node(0)
			{
			}

			ATOM_Node *getNode (void) const
			{
				return _node;
			}

		public:
			virtual void visit (ATOM_Node &node)
			{
				if (_name == node.getDescribe())
				{
					_node = &node;
					cancelTraverse ();
				}
			}
		};
		PlaceHolderFindVisitor v(placeholderName);
		v.traverse (*_scene->getRootNode());
		ATOM_Node *placeHolder = v.getNode();
		if (!placeHolder)
		{
			ATOM_DELETE(_scene);
			_scene = 0;
			return;
		}

		ATOM_Matrix4x4f _origin = placeHolder->getO2T();
		ATOM_AUTOREF(ATOM_Node) _parent = placeHolder->getParent();
		_parent->removeChild (placeHolder);

		//ATOM_CustomRenderScheme * scheme = (ATOM_CustomRenderScheme*)ATOM_RenderScheme::createRenderScheme ("custom");
		ATOM_DeferredRenderScheme *scheme = (ATOM_DeferredRenderScheme*)ATOM_RenderScheme::createRenderScheme ("deferred");
		scheme->setSchemeFlag(DSF_NOMAINSCHEME | DSF_NOSHADOW);
		scheme->setTransparent(true);
		//scheme->setTransparent (true);
		_scene->setRenderScheme (scheme);
		_scene->getCamera()->setViewMatrix (_camera);

		ATOM_AUTOREF(ATOM_Node) test_role = ATOM_Node::loadNodeFromFile("/resource/actor/zj/nvfashi01/mesh_yz01.nm2");
		test_role->setO2T(_origin);
		_parent->appendChild(test_role.get());

		ATOM_Geode* pCharacter = dynamic_cast<ATOM_Geode*>(test_role.get());
		if( pCharacter )
			pCharacter->doAction("stand", ATOM_Geode::ACTIONFLAGS_DOWNSIDE | ATOM_Geode::ACTIONFLAGS_UPSIDE, 0, true, 200);
	}
	
	ATOM_Rect2Di savedViewport = device->getViewport (NULL);
	ATOM_ColorARGB savedClearColor = device->getClearColor(NULL);

	ATOM_Rect2Di rc(200,200, 400,600);
	device->setViewport (NULL, rc);

	_scene->getCamera()->setPerspective (_scene->getFOVY(), 1, _scene->getNearPlane(), _scene->getFarPlane());
	_scene->getCamera()->setViewport (200,200, 400,600);

	_scene->render (device, false);

	device->setClearColor (NULL, savedClearColor);
	device->setViewport (NULL, savedViewport);

#else

	if( (::GetAsyncKeyState(VK_SPACE) & 0x8000) != 0 )
		ATOM_CustomRenderScheme::bRender = 1 - ATOM_CustomRenderScheme::bRender;

	if( ATOM_CustomRenderScheme::bRender )
	{
		if( !_customRenderScheme )
		{
			test_scene = ATOM_NEW(ATOM_DeferredScene);
			if (!test_scene->load ("/map/roleprop_test_forward.3sg"))
			{
				ATOM_DELETE(test_scene);
				test_scene = 0;
				return;
			}

			class SkyNodeDeleteVisitor: public ATOM_Visitor
			{
			public:
				SkyNodeDeleteVisitor ()
				{
				}

			public:
				virtual void visit (ATOM_Sky &node)
				{
					//node.getParent()->removeChild(&node);
					node.setShow(0);
				}
			};
			SkyNodeDeleteVisitor v_temp;
			v_temp.traverse(*test_scene->getRootNode());

			const ATOM_Matrix4x4f *cameraMatrix = test_scene->getCameraMatrix();
			test_camera = *cameraMatrix;

			class PlaceHolderFindVisitor: public ATOM_Visitor
			{
				ATOM_STRING _name;
				ATOM_Node *_node;

			public:
				PlaceHolderFindVisitor (const char *name): _name(name), _node(0)
				{
				}

				ATOM_Node *getNode (void) const
				{
					return _node;
				}

			public:
				virtual void visit (ATOM_Node &node)
				{
					if (_name == node.getDescribe())
					{
						_node = &node;
						cancelTraverse ();
					}
				}
			};
			PlaceHolderFindVisitor v("$roleprop");
			v.traverse (*test_scene->getRootNode());
			ATOM_Node *placeHolder = v.getNode();
			if (!placeHolder)
			{
				ATOM_DELETE(test_scene);
				test_scene = 0;
				return;
			}

			test_origin = placeHolder->getO2T();
			test_parent = placeHolder->getParent();
			test_parent->removeChild (placeHolder);

			_customRenderScheme = (ATOM_CustomRenderScheme*)ATOM_RenderScheme::createRenderScheme ("custom");
			
			test_scene->setRenderScheme (_customRenderScheme);
			test_scene->getCamera()->setViewMatrix (test_camera);

			test_role = ATOM_Node::loadNodeFromFile("/resource/actor/npc/npc021/npc021.csp");
			test_role->setO2T(ATOM_Matrix4x4f::getIdentityMatrix());

			ATOM_Geode* pCharacter = dynamic_cast<ATOM_Geode*>(test_role.get());
			if( pCharacter )
				pCharacter->doAction("stand", ATOM_Geode::ACTIONFLAGS_DOWNSIDE | ATOM_Geode::ACTIONFLAGS_UPSIDE, 0, true, 200);
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		ATOM_Rect2Di savedViewport = device->getViewport (NULL);
		ATOM_ColorARGB savedClearColor = device->getClearColor(NULL);

		ATOM_Rect2Di rc(500,300, 300,300);
		device->setViewport (NULL, rc);

		test_scene->getCamera()->setPerspective (test_scene->getFOVY(), 1, test_scene->getNearPlane(), test_scene->getFarPlane());
		test_scene->getCamera()->setViewport (500,300, 300,300);


		ATOM_AUTOREF(ATOM_Node) oldParent;
		ATOM_Matrix4x4f oldMatrix;

		if (test_role)
		{
			oldParent = test_role->getParent();
			if (oldParent)
				oldParent->removeChild (test_role.get());
			oldMatrix = test_role->getO2T();

			test_parent->appendChild (test_role.get());
			
			ATOM_Matrix4x4f matrix = test_origin >> ATOM_Matrix4x4f::getRotateYMatrix(0);
			test_role->setO2T(matrix);
		}

		test_scene->render (device, false);

		if (test_role)
		{
			test_parent->removeChild (test_role.get());
			if (oldParent)
				oldParent->appendChild (test_role.get());
			test_role->setO2T (oldMatrix);
		}

		device->setClearColor (NULL, savedClearColor);
		device->setViewport (NULL, savedViewport);
	}

#endif
}

// wangjian added 
// 清除掉所有的延迟渲染模式资源（生成的所有RT资源)
void ATOM_DeferredRenderScheme::DestoryAllDSResources()
{
	ATOM_PostEffectRTMgr::reset();

	_depthTexture = 0;
	_normalTexture = 0;
	_albedoTexture = 0;
	_lightAccTexture = 0;
	_shadedTexture = 0;
	_ldrResult[0] = 0;
	_ldrResult[1] = 0;

	_deferredMaterial = 0;

	_vertexArrayOmni = 0;
	_vertexArrayOmni_V2 = 0;
	_indexArrayOmni = 0;
	if( ATOM_GetRenderDevice() )
	{
		ATOM_GetRenderDevice()->destroyVertexDecl(_vertexDeclOmni);
		_vertexDeclOmni = 0;
		ATOM_GetRenderDevice()->destroyVertexDecl(_vertexDeclOmni_V2);
		_vertexDeclOmni_V2 = 0;
	}

	currentDeferredScheme = 0;

	_isOk = false;
}

bool ATOM_DeferredRenderScheme::fini (ATOM_RenderDevice *device)
{
	_depthTexture = 0;
	_normalTexture = 0;
	_albedoTexture = 0;
	_lightAccTexture = 0;
	_shadedTexture = 0;
	_ldrResult[0] = 0;
	_ldrResult[1] = 0;
	ATOM_DELETE(_HDRPipeline);
	_HDRPipeline = 0;

	return true;
}

void ATOM_DeferredRenderScheme::setTransparent (bool transparent)
{
	_isTransparent = transparent;
}

bool ATOM_DeferredRenderScheme::isTransparent (void) const
{
	return _isTransparent;
}

//--- wangjian added ---//
void ATOM_DeferredRenderScheme::setTransMaskTexture(const char* file)
{
	if( !_isTransparent )
		return;

	if( !file || ('\0'==file[0]) )
		return;

	if( _transparentMaskTextureFile == file )
		return;

	_transparentMaskTextureFile == file;

	_transparentMaskTexture = ATOM_CreateTextureResource(	file,
															ATOM_PIXEL_FORMAT_UNKNOWN,
															10000	);

}
void ATOM_DeferredRenderScheme::flushRenderQueue()
{
	if( _renderQueue[0] )
	{
		for (unsigned i = 0; i < RenderScheme_DeferredSchemeCount; ++i)
		{
			_renderQueue[i]->clear ();
		}
	}
}
//----------------------//

void ATOM_DeferredRenderScheme::setHDRBrightPassThreshold (float value)
{
	if (_HDRPipeline)
	{
		_HDRPipeline->setBrightPassThreshold (value);
	}
}

float ATOM_DeferredRenderScheme::getHDRBrightPassThreshold (void) const
{
	return _HDRPipeline ? _HDRPipeline->getBrightPassThreshold () : 0.f;
}

void ATOM_DeferredRenderScheme::setHDRBrightPassOffset (float value)
{
	if (_HDRPipeline)
	{
		_HDRPipeline->setBrightPassOffset (value);
	}
}

float ATOM_DeferredRenderScheme::getHDRBrightPassOffset (void) const
{
	return _HDRPipeline ? _HDRPipeline->getBrightPassOffset () : 0.f;
}

void ATOM_DeferredRenderScheme::setHDRMiddleGray (float value)
{
	if (_HDRPipeline)
	{
		_HDRPipeline->setMiddleGray (value);
	}
}

float ATOM_DeferredRenderScheme::getHDRMiddleGray (void) const
{
	return _HDRPipeline ? _HDRPipeline->getMiddleGray () : 0.f;
}

//--- wangjian added ---//
// HDR相干
void ATOM_DeferredRenderScheme::setHDREnable(bool enable)
{
	if (_HDRPipeline)
	{
		_HDRPipeline->setHDREnable (enable);

		if( enable != ATOM_RenderSettings::isHDRRenderingEnabled() )
		{
			ATOM_RenderSettings::enableHDRRendering(enable);

			// 如果开启HDR
			if( enable )
			{
				// 创建浮点的_shadedTexture
				if( !_shadedTexture && ATOM_GetRenderDevice() )
				{
					_shadedTexture = ATOM_GetRenderDevice()->allocTexture (0, 0, _currentViewportWidth, _currentViewportHeight, ATOM_PIXEL_FORMAT_RGBA16F, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET);
				}

				// 创建浮点lightAccTexture
				if( ATOM_GetRenderDevice() )
				{
					if( _lightAccTexture )
						_lightAccTexture = 0;
					_lightAccTexture = ATOM_GetRenderDevice()->allocTexture (0, 0, _currentViewportWidth, _currentViewportHeight, ATOM_PIXEL_FORMAT_RGBA16F, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET);
				}
			}
			// 如果关闭HDR渲染
			else
			{
				// 如果不使用浮点格式_shadedTexture
				if( false == ATOM_RenderSettings::isFloatShadingEnabled() )
				{
					// 不需要_shadedTexture 直接渲染到LDR的RT
					_shadedTexture = 0;

					// 创建整型的_lightAccTexture
					if( ATOM_GetRenderDevice() )
					{
						if( _lightAccTexture )
							_lightAccTexture = 0;
						_lightAccTexture = ATOM_GetRenderDevice()->allocTexture (0, 0, _currentViewportWidth, _currentViewportHeight, ATOM_PIXEL_FORMAT_RGBA8888, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET);
					}	
				}
				else
				{
					// 创建浮点格式的_shadedTexture
					if( !_shadedTexture && ATOM_GetRenderDevice() )
					{
						_shadedTexture = ATOM_GetRenderDevice()->allocTexture (0, 0, _currentViewportWidth, _currentViewportHeight, ATOM_PIXEL_FORMAT_RGBA16F, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET);
					}
					// 和浮点格式的_lightAccTexture
					if( ATOM_GetRenderDevice() )
					{
						if( _lightAccTexture )
							_lightAccTexture = 0;
						_lightAccTexture = ATOM_GetRenderDevice()->allocTexture (0, 0, _currentViewportWidth, _currentViewportHeight, ATOM_PIXEL_FORMAT_RGBA16F, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET);
					}
				}
			}
		}
	}
}
bool ATOM_DeferredRenderScheme::getHDREnable(void) const
{
	if (_HDRPipeline)
	{
		return _HDRPipeline->getHDREnable ();
	}
	return false;
}
void ATOM_DeferredRenderScheme::setHDRFilmCurveParams(float filmCurveShoulder,float filmCurveMiddleTone,float filmCurveToe, float filmCurveWhitePoint)
{
	if (_HDRPipeline)
	{
		_HDRPipeline->setHDRFilmCurveParams (filmCurveShoulder,filmCurveMiddleTone,filmCurveToe,filmCurveWhitePoint);
	}
}
void ATOM_DeferredRenderScheme::getHDRFilmCurveParams(float & filmCurveShoulder,float & filmCurveMiddleTone,float & filmCurveToe, float & filmCurveWhitePoint) const
{
	if (_HDRPipeline)
	{
		_HDRPipeline->getHDRFilmCurveParams (filmCurveShoulder,filmCurveMiddleTone,filmCurveToe,filmCurveWhitePoint);
	}
}
void ATOM_DeferredRenderScheme::setHDRColorParams(float HDRSaturate,float HDRContrast,ATOM_Vector3f HDRColorBalance)
{
	if (_HDRPipeline)
	{
		_HDRPipeline->setHDRColorParams (HDRSaturate,HDRContrast,HDRColorBalance);
	}
}
void ATOM_DeferredRenderScheme::getHDRColorParams(float & HDRSaturate,float & HDRContrast,ATOM_Vector3f & HDRColorBalance) const
{
	if (_HDRPipeline)
	{
		_HDRPipeline->getHDRColorParams (HDRSaturate,HDRContrast,HDRColorBalance);
	}
}
void ATOM_DeferredRenderScheme::setSchemeFlag(unsigned flag)
{
	_deferredSchemeFlag = flag;
}
unsigned ATOM_DeferredRenderScheme::getSchemeFlag(void) const
{
	return _deferredSchemeFlag;
}
//----------------------//

const char *ATOM_DeferredRenderScheme::getName (void) const
{
	return "deferred";
}

void ATOM_DeferredRenderScheme::setSunLight (ATOM_Light *light)
{
	_sunLight = light;
}

void ATOM_DeferredRenderScheme::setCullVisitor (ATOM_DeferredCullVisitor *cullVisitor)
{
	_cullVisitor = cullVisitor;
}

void ATOM_DeferredRenderScheme::setDrawGbufferOnly (bool b)
{
	_drawGbufferOnly = b;
}

void ATOM_DeferredRenderScheme::addLight (ATOM_Light *light)
{
	switch (light->getLightType ())
	{
	case ATOM_Light::Point:
		_omniLights.push_back (light);
		break;
	case ATOM_Light::Spot:
		_spotLights.push_back (light);
		break;
	case ATOM_Light::Directional:
		_directionalLights.push_back (light);
		break;
	default:
		break;
	}
}

int ATOM_DeferredRenderScheme::onRenderPass (ATOM_RenderDevice *device, ATOM_Scene *scene, int layer)
{
	ATOM_Camera *camera = scene->getCamera();

	switch (layer)
	{
	case RenderScheme_GbufferConstruction:
		return drawToGbuffer (device, camera)&&!_drawGbufferOnly ? RenderScheme_LightAccumulation : -1;

	case RenderScheme_LightAccumulation:
		return drawLightAcc (device, camera) ? RenderScheme_Shading : -1;

	case RenderScheme_Shading:
		return drawShading (device, camera) ? RenderScheme_PostShading : -1;

	case RenderScheme_PostShading:
		return drawPostShading (device, camera) ? RenderScheme_PostShadingCustom : -1;

	//-------- wangjian added -----------//
	case RenderScheme_PostShadingCustom:
		return drawPostShadingCustom (device, camera) ? RenderScheme_SolidForward : -1;
	case RenderScheme_SolidForward:
		return drawSolidForward (device, camera) ? RenderScheme_Forward : -1;
	//-----------------------------------//

	case RenderScheme_Forward:
		return drawForward (device, camera) ? RenderScheme_PostForward : -1;

	case RenderScheme_PostForward:
		return RenderScheme_FinalComposition;

	case RenderScheme_FinalComposition:
		drawComposition (device, camera);
		return RenderScheme_PostComposition;

	case RenderScheme_PostComposition:
		drawPostComposition (device, camera);
		return -1;

	default:
		return -1;
	}
}

void ATOM_DeferredRenderScheme::onResize (ATOM_RenderDevice *device, unsigned width, unsigned height)
{
	if (width == _currentViewportWidth && height == _currentViewportHeight)
	{
		return;
	}

	_currentViewportWidth = width;
	_currentViewportHeight = height;

	///////////////////////////////////////////////
	// wangjian added
	// 如开启阴影
	if( !(_deferredSchemeFlag & DSF_NOSHADOW) )
		_shadowMapRenderScheme->resize (device, _currentViewportWidth, _currentViewportHeight);

	// 如果不是主scheme,退出
	if( _deferredSchemeFlag & DSF_NOMAINSCHEME )
		return;
	///////////////////////////////////////////////

	//-----------------------------//
	// wangjian added 
	ATOM_PostEffectRTMgr::reset();
	//-----------------------------//

	_depthTexture = 0;
	_normalTexture = 0;
	_albedoTexture = 0;
	_lightAccTexture = 0;
	_shadedTexture = 0;
	_ldrResult[0] = 0;
	_ldrResult[1] = 0;

	_isOk = false;

	if ( width > 0 && height > 0 )
	{
		_depthTexture = device->allocDepthTexture (width, height);
		if (!_depthTexture)
		{
			fini (device);
			return;
		}

		//=================== wangjian modified ===========================//
		// 使用2张32bitRGBA 取代 3张16浮点类型
		// _normalTexture : RG: 法线(PACKING) / B：specular power / A:kd & ks
		// _albedoTexture : RGB: albedo / A : glossness

		_normalTexture = device->allocTexture (0, 0, _currentViewportWidth, _currentViewportHeight, ATOM_PIXEL_FORMAT_RGBA8888, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET);
		if (!_normalTexture)
		{
			fini (device);
			return;
		}

		_albedoTexture = device->allocTexture (0, 0, _currentViewportWidth, _currentViewportHeight, ATOM_PIXEL_FORMAT_RGBA8888, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET);
		if (!_albedoTexture)
		{
			fini (device);
			return;
		}
		//===============================================================================//

		//--- wangjian modified ---//
		// 如果开启HDR渲染
		if( ATOM_RenderSettings::isHDRRenderingEnabled() )
		{
			// 创建浮点lightAccTexture
			_lightAccTexture = device->allocTexture (0, 0, _currentViewportWidth, _currentViewportHeight, ATOM_PIXEL_FORMAT_RGBA16F, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET);
			if (!_lightAccTexture)
			{
				fini (device);
				return;
			}

			// 创建浮点的_shadedTexture
			_shadedTexture = device->allocTexture (0, 0, _currentViewportWidth, _currentViewportHeight, ATOM_PIXEL_FORMAT_RGBA16F, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET);
			if (!_shadedTexture)
			{
				fini (device);
				return;
			}
		}
		// 如果不开启HDR渲染
		else
		{
			// 如果使用浮点格式shading RT
			if( ATOM_RenderSettings::isFloatShadingEnabled() )
			{
				// 创建浮点的_shadedTexture
				_shadedTexture = device->allocTexture (0, 0, _currentViewportWidth, _currentViewportHeight, ATOM_PIXEL_FORMAT_RGBA16F, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET);
				if (!_shadedTexture)
				{
					fini (device);
					return;
				}

				// 创建浮点的lightAccTexture
				_lightAccTexture = device->allocTexture (0, 0, _currentViewportWidth, _currentViewportHeight, ATOM_PIXEL_FORMAT_RGBA16F, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET);
				if (!_lightAccTexture)
				{
					fini (device);
					return;
				}
			}
			else
			{
				// 创建32位整型lightAccTexture
				_lightAccTexture = device->allocTexture (0, 0, _currentViewportWidth, _currentViewportHeight, ATOM_PIXEL_FORMAT_RGBA8888, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET);
				if (!_lightAccTexture)
				{
					fini (device);
					return;
				}
			}
		}
		
		// 两个LDR的BUFFER
		_ldrResult[0] = device->allocTexture (0, 0, width, _currentViewportHeight, ATOM_PIXEL_FORMAT_BGRA8888, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET);
		if (!_ldrResult[0])
		{
			fini (device);
			return;
		}

		_ldrResult[1] = device->allocTexture (0, 0, width, _currentViewportHeight, ATOM_PIXEL_FORMAT_BGRA8888, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET);
		if (!_ldrResult[1])
		{
			fini (device);
			return;
		}

		if (!_deferredMaterial)
		{
			_deferredMaterial = ATOM_MaterialManager::createMaterialFromCore (device, "/materials/builtin/deferredrendering.mat");

			if (!_deferredMaterial)
			{
				fini (device);
				return;
			}
		}

		if (!_vertexDeclOmni)
		{
			_vertexDeclOmni = device->createInterleavedVertexDeclaration (omniAttrib, 0);
			if (!_vertexDeclOmni)
			{
				return;
			}
		}

		if (!_vertexArrayOmni)
		{
			_vertexArrayOmni = device->allocVertexArray (omniAttrib, ATOM_USAGE_DYNAMIC, 8, true);
			if (!_vertexArrayOmni)
			{
				return;
			}
		}

		if (!_vertexDeclOmni_V2)
		{
			_vertexDeclOmni_V2 = device->createInterleavedVertexDeclaration (omniAttrib_V2, 0);
			if (!_vertexDeclOmni_V2)
			{
				return;
			}
		}
		if (!_vertexArrayOmni_V2)
		{
			_vertexArrayOmni_V2 = device->allocVertexArray (omniAttrib_V2, ATOM_USAGE_DYNAMIC, 8, true);
			if (!_vertexArrayOmni_V2)
			{
				return;
			}
		}

		if (!_indexArrayOmni)
		{
			_indexArrayOmni = device->allocIndexArray (ATOM_USAGE_STATIC, 12 * 3, false, true);
			if (!_indexArrayOmni)
			{
				_vertexArrayOmni = 0;
				_vertexArrayOmni_V2 = 0;
				return ;
			}

			unsigned short *indices = (unsigned short*)_indexArrayOmni->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
			if (!indices)
			{
				_vertexArrayOmni = 0;
				_vertexArrayOmni_V2 = 0;
				_indexArrayOmni = 0;
				return;
			}

			*indices++ = 0;
			*indices++ = 3;
			*indices++ = 1;
			*indices++ = 3;
			*indices++ = 2;
			*indices++ = 1;
			*indices++ = 0;
			*indices++ = 1;
			*indices++ = 5;
			*indices++ = 0;
			*indices++ = 5;
			*indices++ = 4;
			*indices++ = 4;
			*indices++ = 5;
			*indices++ = 7;
			*indices++ = 7;
			*indices++ = 5;
			*indices++ = 6;
			*indices++ = 7;
			*indices++ = 6;
			*indices++ = 2;
			*indices++ = 7;
			*indices++ = 2;
			*indices++ = 3;
			*indices++ = 5;
			*indices++ = 1;
			*indices++ = 2;
			*indices++ = 5;
			*indices++ = 2;
			*indices++ = 6;
			*indices++ = 4;
			*indices++ = 3;
			*indices++ = 0;
			*indices++ = 4;
			*indices++ = 7;
			*indices++ = 3;

			_indexArrayOmni->unlock ();
		}

		_isOk = true;
	}
}

bool ATOM_DeferredRenderScheme::drawToGbuffer (ATOM_RenderDevice *device, ATOM_Camera *camera)
{
	device->setDepthBuffer (_depthTexture->getDepthBuffer ());
	device->setRenderTarget (0, _normalTexture.get());
	device->setRenderTarget (1, _albedoTexture.get());

#if 0
	camera->setViewport (0, 0, _normalTexture->getWidth(), _normalTexture->getHeight());
#else
	camera->setViewport (0, 0, _currentViewportWidth, _currentViewportHeight);
#endif
	camera->synchronise (device, device->getCurrentView ());
	//--- wangjian modified ---//
	//device->setClearColor (NULL, 0);
	// * 原来是清除为0，现在清除为1
	device->setClearColor (NULL, 1.f, 1.f, 1.f, 1.f );
	//-------------------------//
	device->clear (true, true, true);

	{
		DEBUG_RENDERQUEUE_LOG_START("drawToGbuffer");

#if 0
		ATOM_LOGGER::log("=================== drawToGbuffer is START drawing ===================\n");
#endif

		_renderQueue[RenderScheme_GbufferConstruction]->SetName("drawToGbuffer");
		_renderQueue[RenderScheme_GbufferConstruction]->render (device, camera, ATOM_RenderQueue::SortNone, true);

#if 0
		ATOM_LOGGER::log("=================== drawToGbuffer is END drawing ===================\n");
#endif

		DEBUG_RENDERQUEUE_LOG_END("drawToGbuffer");
	}
	
	device->setRenderTarget (0, 0);
	device->setRenderTarget (1, 0);

	if (!_drawGbufferOnly)
	{
		// wangjian modified 
		// 如果开启阴影渲染
		if( ATOM_RenderSettings::isShadowEnabled() && !(_deferredSchemeFlag & DSF_NOSHADOW) )
		{
			// 设置阴影RenderScheme 和 RenderScheme layer
			{
				ATOM_RenderScheme::getCurrentScene()->setRenderScheme (_shadowMapRenderScheme);
				//ATOM_RenderScheme::setCurrentRenderSchemeLayer(1);
			}

			_shadowMapRenderScheme->setRenderMode (ATOM_DeferredShadowMapRenderScheme::RenderMode_ShadowMask);
			_shadowMapRenderScheme->setDepthBias (((ATOM_SDLScene*)ATOM_RenderScheme::getCurrentScene())->getDepthBias ());
			_shadowMapRenderScheme->setShadowDistance( ((ATOM_SDLScene*)ATOM_RenderScheme::getCurrentScene())->getShadowDistance ());
			_shadowMapRenderScheme->setFadeMin ( ((ATOM_SDLScene*)ATOM_RenderScheme::getCurrentScene())->getShadowFadeMin());
			_shadowMapRenderScheme->setFadeMax ( ((ATOM_SDLScene*)ATOM_RenderScheme::getCurrentScene())->getShadowFadeMax());
			
			//--- wangjian added ---//
			_shadowMapRenderScheme->setShadowBlur( ((ATOM_SDLScene*)ATOM_RenderScheme::getCurrentScene())->getShadowBlur() );
			_shadowMapRenderScheme->setShadowSlopeBias( ((ATOM_SDLScene*)ATOM_RenderScheme::getCurrentScene())->getShadowSlopeBias() );
			_shadowMapRenderScheme->setShadowDensity( ((ATOM_SDLScene*)ATOM_RenderScheme::getCurrentScene())->getShadowDensity() );
			//----------------------//
			ATOM_RenderScheme::getCurrentScene()->render (device, true);

			// 恢复延迟RenderScheme 和 RenderScheme layer
			{
				//ATOM_RenderScheme::setCurrentRenderSchemeLayer(-1);
				ATOM_RenderScheme::getCurrentScene()->setRenderScheme (this);
			}
		}
		else
		{
			_shadowMapRenderScheme->generateWhiteShadowMask();
		}
	}

	int n = 0;
	if (n)
	{
		_albedoTexture->saveToFile ("/albedo.dds");
		_normalTexture->saveToFile ("/normal.dds");
	}

	//========================================================//
#if 1
	ATOM_PostEffectChain * chain = getPostEffectChain();
	if( chain )
	{
		ATOM_SSAOEffect * effect = (ATOM_SSAOEffect*)chain->getEffect("ATOM_SSAOEffect");
		if( effect )
		{
			if( !effect->isDebugDraw() )
				effect->render(device);
		}
	}
#endif
	//========================================================//

	return true;
}

bool ATOM_DeferredRenderScheme::drawLightAcc (ATOM_RenderDevice *device, ATOM_Camera *camera)
{
	device->setRenderTarget (0, _lightAccTexture.get());
	device->setRenderTarget (1, NULL);

	camera->setViewport (0, 0, _currentViewportWidth, _currentViewportHeight);
	camera->synchronise (device, device->getCurrentView ());

	// wangjian added
	// 如果不是用浮点纹理 开启SRGB写RT
	if( false == ATOM_RenderSettings::isHDRRenderingEnabled() && false == ATOM_RenderSettings::isFloatShadingEnabled() )
	{
		// 必须如此设置
		_deferredMaterial->getParameterTable()->setInt("srcBlend", ATOM_RenderAttributes::BlendFunc_DestColor);
		_deferredMaterial->getParameterTable()->setInt ("destBlend", ATOM_RenderAttributes::BlendFunc_Zero);

		// 这里必须是清1
		device->setClearColor (NULL, 0xffffffff);
	}
	else
	{
		// 必须如此设置
		_deferredMaterial->getParameterTable()->setInt("srcBlend", ATOM_RenderAttributes::BlendFunc_One);
		_deferredMaterial->getParameterTable()->setInt ("destBlend", ATOM_RenderAttributes::BlendFunc_One);

		// 这里必须是清0
		device->setClearColor (NULL, 0);
	}
	device->clear (true, false, false);

	// 环境光照
	_deferredMaterial->setActiveEffect ("ambient");
	_deferredMaterial->getParameterTable()->setMatrix44 ("view", camera->getViewMatrix());

	drawScreenQuad (device, _deferredMaterial.get());
	
	if ( ATOM_RenderSettings::isAllLightTypeEnabled() && !_ambientOnly )
	{
		// 点光源光照
		if ( ATOM_RenderSettings::isPointLightTypeEnabled() && !_omniLights.empty ())
		{
#if 0
			drawOmniLights (device, _depthTexture.get(), _normalTexture.get(), &_omniLights[0], _omniLights.size(), camera);
#else
			drawOmniLights (device, camera);
#endif
		}

		// 平行光源光照
		if ( ATOM_RenderSettings::isDirectionalLightTypeEnabled() && !_directionalLights.empty ())
		{
#if 0
			drawDirectionalLights (device, _depthTexture.get(), _normalTexture.get(), &_directionalLights[0], _directionalLights.size(), camera);
#else
			drawDirectionalLights (device, camera);
#endif
		}

		// 补光
		drawComplementLight(device,camera);
	}

	device->setRenderTarget (0, 0);
	int n = 0;
	if (n)
	{
		_lightAccTexture->saveToFile ("/lightacc.dds");
	}
	return true;
}

bool ATOM_DeferredRenderScheme::drawShading (ATOM_RenderDevice *device, ATOM_Camera *camera)
{
	//------------ wangjian added --------------------//
	// 如果使用HDR 使用浮点RT / 否则 使用32bit整型RT
	ATOM_Texture * shadingRT = ( ATOM_RenderSettings::isHDRRenderingEnabled() || ATOM_RenderSettings::isFloatShadingEnabled() ) ? 
								_shadedTexture.get() : 
								_ldrResult[0].get();
	//------------------------------------------------//

	device->setRenderTarget (0, shadingRT);
	device->setRenderTarget (1, NULL);

	camera->setViewport (0, 0, _currentViewportWidth, _currentViewportHeight);
	camera->synchronise (device, device->getCurrentView ());

	device->setClearColor(NULL,0.5,0.5,0.5,1.0);
	device->clear (true, false, false);
	
	// 如果不使用浮点格式，那么需要SRGB读_lightAccTexture
	if( false == ATOM_RenderSettings::isHDRRenderingEnabled() && false == ATOM_RenderSettings::isFloatShadingEnabled() )
	{
		device->enableSRGBWrite(true);
	}
	//// 否则直接读取线性浮点格式
	//else
	//{
	//	_deferredMaterial->setActiveEffect ("shading");
	//}

	_deferredMaterial->setActiveEffect ("shading");

	//--- wangjian added ---//
	/*ATOM_Vector4f v(1.0f/(float)device->getRenderTarget(0)->getWidth(),
					1.0f/(float)device->getRenderTarget(0)->getHeight(),
					0,0);
	_deferredMaterial->getParameterTable()->setVector( "InvScreenSize",v );*/
	//----------------------//

	drawScreenQuad (device, _deferredMaterial.get());

	//device->setRenderTarget (0, shadingRT);

	{
		DEBUG_RENDERQUEUE_LOG_START("drawShading");

		_renderQueue[RenderScheme_Shading]->SetName("drawShading");
		_renderQueue[RenderScheme_Shading]->render (device, camera, ATOM_RenderQueue::SortNone, false);

		DEBUG_RENDERQUEUE_LOG_END("drawShading");
	}

	device->setRenderTarget (0, 0);
	int n = 0;
	if (n)
	{
		shadingRT->saveToFile ("/shaded.dds");
	}

	// wangjian added
#if 0
	copySceneTexture(device,camera);
#endif

	return true;
}

bool ATOM_DeferredRenderScheme::drawPostShading (ATOM_RenderDevice *device, ATOM_Camera *camera)
{
	//------------ wangjian added --------------------//
	// 如果使用HDR 使用浮点RT / 否则 使用32bit整型RT
	ATOM_Texture * shadingRT = ( ATOM_RenderSettings::isHDRRenderingEnabled() || ATOM_RenderSettings::isFloatShadingEnabled() ) ? 
								_shadedTexture.get() : 
								_ldrResult[0].get();
	//------------------------------------------------//

	device->setRenderTarget (0, shadingRT);

	device->setRenderTarget (1, NULL);

	camera->setViewport (0, 0, _currentViewportWidth, _currentViewportHeight);
	camera->synchronise (device, device->getCurrentView ());

	//if( false == getHDREnable() )
	//	device->enableSRGBWrite(true);

	// wangjian added for profiler purposes

	{
		DEBUG_RENDERQUEUE_LOG_START("drawPostShading");

		_renderQueue[RenderScheme_PostShading]->SetName("drawPostShading");
		_renderQueue[RenderScheme_PostShading]->render (device, camera, ATOM_RenderQueue::SortNone, false);

		DEBUG_RENDERQUEUE_LOG_END("drawPostShading");
	}
	


	//if( false == getHDREnable() )
	//	device->enableSRGBWrite(false);

	//--------------------------------------//
	// wangjian added
	// 将_ldrResult[0]拷贝到_ldrResult[1]，作为水体或者其他透明效果的折射图
#if 1
	copySceneTexture(device,camera);
#endif
	//--------------------------------------//

	return true;
}

//------------ wangjian added --------------------//
void ATOM_DeferredRenderScheme::copySceneTexture(ATOM_RenderDevice *device, ATOM_Camera *camera)
{
	/*if( ATOM_RenderSettings::_waterQuality <= QUALITY_MEDIUM )
		return;*/

	ATOM_AUTOREF(ATOM_Texture) oldRT = device->getRenderTarget(0);

	device->setRenderTarget (0, _ldrResult[1].get());

	camera->setViewport (0, 0, _currentViewportWidth, _currentViewportHeight);
	camera->synchronise (device, device->getCurrentView ());

	/*ATOM_ColorARGB savedClearColor = device->getClearColor(NULL);
	device->setClearColor(NULL, 0);
	device->clear(true,false,false);*/

	device->enableSRGBWrite(false);

	_deferredMaterial->setActiveEffect("framebuffer_blit");
	_deferredMaterial->getParameterTable()->setTexture ("FinalResultTexture", _ldrResult[0].get());

	drawScreenQuad(device,_deferredMaterial.get());

	//device->setClearColor(NULL, savedClearColor);

	device->setRenderTarget (0, oldRT.get());

	device->enableSRGBWrite(true);
}
//------------------------------------------------//
// wangjian added

// 渲染水之类
bool ATOM_DeferredRenderScheme::drawPostShadingCustom (ATOM_RenderDevice *device, ATOM_Camera *camera)
{
	//------------ wangjian added --------------------//
	// 如果使用HDR 使用浮点RT / 否则 使用32bit整型RT
	ATOM_Texture * shadingRT = ( ATOM_RenderSettings::isHDRRenderingEnabled() || ATOM_RenderSettings::isFloatShadingEnabled() ) ? 
								_shadedTexture.get() : 
								_ldrResult[0].get();
	//------------------------------------------------//

	device->setRenderTarget (0, shadingRT);
	device->setRenderTarget (1, NULL);

	camera->setViewport (0, 0, _currentViewportWidth, _currentViewportHeight);
	camera->synchronise (device, device->getCurrentView ());

	{
		// wangjian added for profiler purposes
		DEBUG_RENDERQUEUE_LOG_START("drawPostShadingCustom");

		_renderQueue[RenderScheme_PostShadingCustom]->SetName("drawPostShadingCustom");

		// 透明对象使用从后往前进行排序
		_renderQueue[RenderScheme_PostShadingCustom]->render (device, camera, ATOM_RenderQueue::SortNone, false);

		DEBUG_RENDERQUEUE_LOG_END("drawPostShadingCustom");
	}

	return true;
}

// 渲染水之类
bool ATOM_DeferredRenderScheme::drawSolidForward (ATOM_RenderDevice *device, ATOM_Camera *camera)
{
	//------------ wangjian added --------------------//
	// 如果使用HDR 使用浮点RT / 否则 使用32bit整型RT
	ATOM_Texture * shadingRT = ( ATOM_RenderSettings::isHDRRenderingEnabled() || ATOM_RenderSettings::isFloatShadingEnabled() ) ? 
								_shadedTexture.get() : 
								_ldrResult[0].get();
	//------------------------------------------------//

	device->setRenderTarget (0, shadingRT);
	device->setRenderTarget (1, NULL);

	camera->setViewport (0, 0, _currentViewportWidth, _currentViewportHeight);
	camera->synchronise (device, device->getCurrentView ());

	{
		// wangjian added for profiler purposes
		DEBUG_RENDERQUEUE_LOG_START("drawSolidForward");

		_renderQueue[RenderScheme_SolidForward]->SetName("drawSolidForward");

		// 透明对象使用从后往前进行排序
		_renderQueue[RenderScheme_SolidForward]->render (device, camera, ATOM_RenderQueue::SortBackToFront, false);

		DEBUG_RENDERQUEUE_LOG_END("drawSolidForward");
	}

	return true;
}

bool ATOM_DeferredRenderScheme::drawForward (ATOM_RenderDevice *device, ATOM_Camera *camera)
{
	//------------ wangjian added --------------------//
	// 如果使用HDR 使用浮点RT / 否则 使用32bit整型RT
	ATOM_Texture * shadingRT = ( ATOM_RenderSettings::isHDRRenderingEnabled() || ATOM_RenderSettings::isFloatShadingEnabled() ) ? 
								_shadedTexture.get() : 
								_ldrResult[0].get();
	//------------------------------------------------//

	device->setRenderTarget (0, shadingRT);
	device->setRenderTarget (1, NULL);

	camera->setViewport (0, 0, _currentViewportWidth, _currentViewportHeight);
	camera->synchronise (device, device->getCurrentView ());

	{
		// wangjian added for profiler purposes
		DEBUG_RENDERQUEUE_LOG_START("drawForward");

		_renderQueue[RenderScheme_Forward]->SetName("drawForward");

		// 透明对象使用从后往前进行排序
		_renderQueue[RenderScheme_Forward]->render (device, camera, /*ATOM_RenderQueue::SortNone*/ATOM_RenderQueue::SortBackToFront, false);

		DEBUG_RENDERQUEUE_LOG_END("drawForward");
	}


	return true;
}

bool ATOM_DeferredRenderScheme::drawPostComposition (ATOM_RenderDevice *device, ATOM_Camera *camera)
{
	device->setRenderTarget (0, _ldrResult[0].get());
	device->setRenderTarget (1, NULL);

#if 0
	device->setViewport (NULL, ATOM_Rect2Di(0,0,_ldrResult[0]->getWidth(),_ldrResult[0]->getHeight()));
#else
	device->setViewport (NULL, ATOM_Rect2Di(0,0,_currentViewportWidth,_currentViewportHeight));
#endif

	camera->setViewport (0, 0, _currentViewportWidth, _currentViewportHeight);
	camera->synchronise (device, device->getCurrentView ());
	

	//device->enableSRGBWrite(true);

	// wangjian added for profiler purposes

	{
		DEBUG_RENDERQUEUE_LOG_START("drawPostComposition");

		_renderQueue[RenderScheme_PostComposition]->SetName("drawPostComposition");
		_renderQueue[RenderScheme_PostComposition]->render (device, camera, ATOM_RenderQueue::SortNone, false);

		DEBUG_RENDERQUEUE_LOG_END("drawPostComposition");

	}
	

	//--- wangjian added ---//
	// 如果不是用浮点纹理 关闭SRGB写RT
	if( false == ATOM_RenderSettings::isHDRRenderingEnabled() && false == ATOM_RenderSettings::isFloatShadingEnabled() )
		device->enableSRGBWrite(false);
	//----------------------//


	return true;
}

void ATOM_DeferredRenderScheme::drawHUD (ATOM_RenderDevice *device, ATOM_Camera *camera)
{
	device->setRenderTarget (0, _ldrResult[0].get());
	device->setRenderTarget (1, NULL);

#if 0
	device->setViewport (NULL, ATOM_Rect2Di(0,0,_ldrResult[0]->getWidth(),_ldrResult[1]->getHeight()));
#else
	device->setViewport (NULL, ATOM_Rect2Di(0,0,_currentViewportWidth,_currentViewportHeight));
#endif

	camera->setViewport (0, 0, _currentViewportWidth, _currentViewportHeight);
	camera->synchronise (device, device->getCurrentView ());
	

	// wangjian added for profiler purposes

	{
		ATOM_RenderScheme::setCurrentRenderScheme(this);
		ATOM_RenderScheme::setCurrentRenderSchemeLayer(RenderScheme_HUD);

		DEBUG_RENDERQUEUE_LOG_START("drawHUD");

		_renderQueue[RenderScheme_HUD]->SetName("drawHUD");
		_renderQueue[RenderScheme_HUD]->render (device, camera, ATOM_RenderQueue::SortNone, false);

		DEBUG_RENDERQUEUE_LOG_END("drawHUD");

		ATOM_RenderScheme::setCurrentRenderSchemeLayer(-1);
	}
	

}

//--- wangjian added ---//
void ATOM_DeferredRenderScheme::drawGammaCorrect(ATOM_RenderDevice * device, ATOM_Camera *camera)
{
	if( !_mat_gammacorrect )
	{
		_mat_gammacorrect = ATOM_MaterialManager::createMaterialFromCore (device, "/materials/builtin/gamma_correct.mat");
		if (!_mat_gammacorrect)
		{
			ATOM_LOGGER::error(" create gamma_correct material failed! ");
			return;
		}
		_mat_gammacorrect->setActiveEffect ("no_gamma_read");
	}

	if (ATOM_RenderSettings::isDebugDeferredLightBufferEnabled())
		_mat_gammacorrect->getParameterTable()->setTexture ( "inputTexture", _lightAccTexture.get() );
	else
		_mat_gammacorrect->getParameterTable()->setTexture ( "inputTexture", _shadedTexture.get() );

	device->setRenderTarget (0, _ldrResult[0].get());
	device->setViewport (0, ATOM_Rect2Di(0, 0, _ldrResult[0]->getWidth(), _ldrResult[0]->getHeight()));

	camera->setViewport (0, 0, _currentViewportWidth, _currentViewportHeight);
	camera->synchronise (device, device->getCurrentView ());

	// 开启SRGB写RT
	device->enableSRGBWrite(true);

	device->pushMatrix (ATOM_MATRIXMODE_WORLD);
	device->pushMatrix (ATOM_MATRIXMODE_VIEW);
	device->pushMatrix (ATOM_MATRIXMODE_PROJECTION);
	device->loadIdentity (ATOM_MATRIXMODE_WORLD);
	device->loadIdentity (ATOM_MATRIXMODE_VIEW);
	device->loadIdentity (ATOM_MATRIXMODE_PROJECTION);

	float deltax = (float)1.f/(float)_shadedTexture->getWidth();
	float deltay = (float)1.f/(float)_shadedTexture->getHeight();

	float vertices[4 * 5] = {
		-1.f - deltax, -1.f + deltay, 0.f, 0.f, 1.f,
		1.f - deltax, -1.f + deltay, 0.f, 1.f, 1.f,
		1.f - deltax,  1.f + deltay, 0.f, 1.f, 0.f,
		-1.f - deltax,  1.f + deltay, 0.f, 0.f, 0.f
	};

	unsigned short indices[4] = {
		0, 1, 2, 3
	};

	unsigned num = _mat_gammacorrect ? _mat_gammacorrect->begin (device) : 0;
	for (unsigned pass = 0; pass < num; ++pass)
	{
		if (_mat_gammacorrect->beginPass(device, pass))
		{
			device->renderInterleavedMemoryStreamsIndexed (ATOM_PRIMITIVE_TRIANGLE_FAN, 4, 4 * 5, ATOM_VERTEX_ATTRIB_COORD|ATOM_VERTEX_ATTRIB_TEX1_2, 5 * sizeof(float), vertices, indices);
			_mat_gammacorrect->endPass (device, pass);
		}
	}
	if (_mat_gammacorrect)
	{
		_mat_gammacorrect->end (device);
	}

	device->popMatrix (ATOM_MATRIXMODE_WORLD);
	device->popMatrix (ATOM_MATRIXMODE_VIEW);
	device->popMatrix (ATOM_MATRIXMODE_PROJECTION);

	// 关闭SRGB写RT
	device->enableSRGBWrite(false);
}
//----------------------//

bool ATOM_DeferredRenderScheme::drawComposition (ATOM_RenderDevice *device, ATOM_Camera *camera)
{
	//-------- wangjian added ------------//
	// 如果不使用HDR 直接退出 不做HDR处理
	if( false == ATOM_RenderSettings::isHDRRenderingEnabled() )
	{
		// 如果使用浮点格式保存SHADING信息（线性）
		if( ATOM_RenderSettings::isFloatShadingEnabled() )
			drawGammaCorrect(device,camera);		// gamma correction

		return true;
	}
	//------------------------------------//

	int n = 0;
	if (n)
	{
		_shadedTexture->saveToFile ("/shadedTexture.png");
	}

	if (ATOM_RenderSettings::isDebugDeferredLightBufferEnabled())
	{
		_HDRPipeline->setSource (device, _lightAccTexture.get());
	}
	else
	{
		_HDRPipeline->setSource (device, _shadedTexture.get());
	}
	_HDRPipeline->setDestination (device, _ldrResult[0].get());
	_HDRPipeline->processHDRPipeLine (device, true);

	/*
	device->setRenderTarget (0, NULL);
	device->setRenderTarget (1, NULL);
	device->clear (true, false, false);

	_deferredMaterial->setActiveEffect ("composition");
	_deferredMaterial->setTexture ("finalresult", _finalComposTexture.get());
	drawScreenQuad (device, _deferredMaterial.get());
	*/

	return true;
}
//--- wangjian modified ---//
#if 0
void ATOM_DeferredRenderScheme::drawDirectionalLights (ATOM_RenderDevice *device, ATOM_Texture *depthTexture, ATOM_Texture *normalTexture, ATOM_Light **lights, unsigned numLights, ATOM_Camera *camera)
{
	if (!numLights)
	{
		return;
	}

	_deferredMaterial->setActiveEffect ("directional");

	for (unsigned i = 0; i < numLights; ++i)
	{
		const ATOM_Light *light = lights[i];
		_currentLightDir.setVector3 (camera->getInvViewMatrix().transformVectorAffine (light->getDirection()));
		_currentLightDir.w = 0.f;
		_currentLightColor = light->getColor4f ();

		drawScreenQuad (device, _deferredMaterial.get());
	}
}
void ATOM_DeferredRenderScheme::drawOmniLights (ATOM_RenderDevice *device, ATOM_Texture *depthTexture, ATOM_Texture *normalTexture, ATOM_Light **lights, unsigned numLights, ATOM_Camera *camera)
{
	if (!numLights)
	{
		return;
	}

	_deferredMaterial->setActiveEffect ("omni");

	const ATOM_Matrix4x4f &viewMatrix = camera->getViewMatrix ();
	const ATOM_Matrix4x4f &projMatrix = camera->getProjectionMatrix ();

	static const ATOM_Vector3f directions[8] = {
		ATOM_Vector3f (-1.f,  1.f, -1.f),
		ATOM_Vector3f ( 1.f,  1.f, -1.f),
		ATOM_Vector3f ( 1.f,  1.f,  1.f),
		ATOM_Vector3f (-1.f,  1.f,  1.f),
		ATOM_Vector3f (-1.f, -1.f, -1.f),
		ATOM_Vector3f ( 1.f, -1.f, -1.f),
		ATOM_Vector3f ( 1.f, -1.f,  1.f),
		ATOM_Vector3f (-1.f, -1.f,  1.f)
	};

	struct LightVertex
	{
		ATOM_Vector3f xyz;
		ATOM_Vector3f lightParams;
		ATOM_Vector3f lightAttenuation;
	};

	device->setVertexDecl (_vertexDeclOmni);

	for (unsigned i = 0; i < numLights; ++i)
	{
		LightVertex *vertices = (LightVertex*)_vertexArrayOmni->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
		if (!vertices)
		{
			return;
		}

		const ATOM_Light *light = lights[i];
		const ATOM_Vector3f &lightPos = light->getPosition();
		ATOM_Vector3f lightParams (lightPos.x, lightPos.y, lightPos.z);
		const ATOM_Vector3f &lightAtt = light->getAttenuation ();
		_currentLightColor = light->getColor4f ();

		for (unsigned j = 0; j < 8; ++j)
		{
			vertices->xyz = lightPos + directions[j] * light->getRange();
			vertices->lightParams = lightParams;
			vertices->lightAttenuation = lightAtt;
			++vertices;
		}

		_vertexArrayOmni->unlock ();

		device->setStreamSource (0, _vertexArrayOmni.get(), 0);

		unsigned numPasses = _deferredMaterial->begin (device);
		for (unsigned pass = 0; pass < numPasses; ++pass)
		{
			if (_deferredMaterial->beginPass (device, pass))
			{
				device->renderStreamsIndexed (_indexArrayOmni.get(), ATOM_PRIMITIVE_TRIANGLES, 12);
				_deferredMaterial->endPass (device, pass);
			}
		}
		_deferredMaterial->end (device);
	}
}

#else


void ATOM_DeferredRenderScheme::drawComplementLight (ATOM_RenderDevice *device, ATOM_Camera *camera)
{
	/*ATOM_SDLScene* currentSDLScene = (ATOM_SDLScene*)ATOM_RenderScheme::getCurrentScene();
	if (currentSDLScene)
	{
	ATOM_Vector3f	lightDir;
	ATOM_Vector4f	lightColor;
	float			lightIntensity;
	currentSDLScene->getComplementLightParams(lightDir,lightColor,lightIntensity);
	if( lightIntensity > 0 )
	{
	_deferredMaterial->setActiveEffect ("complementDirectional");

	_currentLightDir.setVector3 (camera->getInvViewMatrix().transformVectorAffine (lightDir));
	_currentLightDir.normalize();
	_currentLightDir.w = 0.f;
	_currentLightColor = lightColor;
	_currentLightColor.w = lightIntensity;

	drawScreenQuad (device, _deferredMaterial.get());
	}
	}*/
}
void ATOM_DeferredRenderScheme::drawDirectionalLights (ATOM_RenderDevice *device, ATOM_Camera *camera)
{
	if ( _directionalLights.empty() )
	{
		return;
	}

	_deferredMaterial->setActiveEffect ("directional");

	for (unsigned i = 0; i < _directionalLights.size(); ++i)
	{
		const ATOM_Light *light = _directionalLights[i].get();
		if( !light )
			continue;

		_currentLightDir.setVector3 (camera->getInvViewMatrix().transformVectorAffine (light->getDirection()));
		_currentLightDir.normalize();
		_currentLightDir.w = 0.f;
		_currentLightColor = light->getColor4f ();

		drawScreenQuad (device, _deferredMaterial.get());
	}
}

bool omnilight_sort( ATOM_AUTOPTR(ATOM_Light) l0, ATOM_AUTOPTR(ATOM_Light) l1 )
{
	return !( l0->hasAttenuationParam() );
}
bool omnilight_pred( ATOM_AUTOPTR(ATOM_Light) l )
{
	return ( l->hasAttenuationParam() );
}
void ATOM_DeferredRenderScheme::drawOmniLights (ATOM_RenderDevice *device, ATOM_Camera *camera)
{
	if ( _omniLights.empty() )
	{
		return;
	}

#if 1

	const ATOM_Matrix4x4f &viewMatrix = camera->getViewMatrix ();
	const ATOM_Matrix4x4f &projMatrix = camera->getProjectionMatrix ();

	static const ATOM_Vector3f directions[8] = 
	{
		ATOM_Vector3f (-1.f,  1.f, -1.f),
		ATOM_Vector3f ( 1.f,  1.f, -1.f),
		ATOM_Vector3f ( 1.f,  1.f,  1.f),
		ATOM_Vector3f (-1.f,  1.f,  1.f),
		ATOM_Vector3f (-1.f, -1.f, -1.f),
		ATOM_Vector3f ( 1.f, -1.f, -1.f),
		ATOM_Vector3f ( 1.f, -1.f,  1.f),
		ATOM_Vector3f (-1.f, -1.f,  1.f)
	};

	struct LightVertex_V1
	{
		ATOM_Vector3f xyz;
		ATOM_Vector3f lightParams;
		ATOM_Vector3f lightAttenuation;
	};
	struct LightVertex_V2
	{
		ATOM_Vector3f xyz;
		ATOM_Vector4f lightParams;
	};

	// sort
	std::sort(_omniLights.begin(),_omniLights.end(),omnilight_sort);

	// 查找第一个老版本的点光源
	ATOM_VECTOR<ATOM_AUTOPTR(ATOM_Light)>::iterator iter_v0 = std::find_if(_omniLights.begin(),_omniLights.end(),omnilight_pred);

	// 如果有新版本的光源
	if( iter_v0 != _omniLights.begin() )
	{
		_deferredMaterial->setActiveEffect ("omni_V2");
		device->setVertexDecl (_vertexDeclOmni_V2);

		ATOM_VECTOR<ATOM_AUTOPTR(ATOM_Light)>::iterator iter_v1 = _omniLights.begin();
		for( ; iter_v1 != iter_v0; ++iter_v1 )
		{
			const ATOM_Light * l_v1 = (*iter_v1).get();
			if( !l_v1 )
				continue;

			LightVertex_V2 * vertices = (LightVertex_V2*)_vertexArrayOmni_V2->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
			if (!vertices)
			{
				return;
			}

			const ATOM_Vector3f &lightPos	= l_v1->getPosition();
			float radius					= ATOM_min2( l_v1->getRange().x, ATOM_min2( l_v1->getRange().y, l_v1->getRange().z ) );
			ATOM_Vector4f lightParams (lightPos.x, lightPos.y, lightPos.z, 1.0f/radius );

			_currentLightColor = l_v1->getColor4f ();

			for (unsigned j = 0; j < 8; ++j)
			{
				vertices->xyz			= lightPos + directions[j] * l_v1->getRange();
				vertices->lightParams	= lightParams;
				++vertices;
			}

			_vertexArrayOmni_V2->unlock ();

			device->setStreamSource (0, _vertexArrayOmni_V2.get(), 0);

			unsigned numPasses = _deferredMaterial->begin (device);
			for (unsigned pass = 0; pass < numPasses; ++pass)
			{
				if (_deferredMaterial->beginPass (device, pass))
				{
					device->renderStreamsIndexed (_indexArrayOmni.get(), ATOM_PRIMITIVE_TRIANGLES, 12);
					_deferredMaterial->endPass (device, pass);
				}
			}
			_deferredMaterial->end (device);

		}
	}
	// 如果有老版本的光源
	if( iter_v0 != _omniLights.end() )
	{
		_deferredMaterial->setActiveEffect ("omni");
		device->setVertexDecl (_vertexDeclOmni);

		ATOM_VECTOR<ATOM_AUTOPTR(ATOM_Light)>::iterator iter = iter_v0;
		for( ; iter != _omniLights.end(); ++iter )
		{
			const ATOM_Light * l_v0 = (*iter).get();
			if( !l_v0 )
				continue;

			LightVertex_V1 * vertices = (LightVertex_V1*)_vertexArrayOmni->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
			if (!vertices)
			{
				return;
			}

			const ATOM_Vector3f &lightPos = l_v0->getPosition();
			ATOM_Vector3f lightParams (lightPos.x, lightPos.y, lightPos.z);
			const ATOM_Vector3f &lightAtt = l_v0->getAttenuation ();
			_currentLightColor = l_v0->getColor4f ();

			for (unsigned j = 0; j < 8; ++j)
			{
				vertices->xyz = lightPos + directions[j] * l_v0->getRange();
				vertices->lightParams = lightParams;
				vertices->lightAttenuation = lightAtt;
				++vertices;
			}

			_vertexArrayOmni->unlock ();

			device->setStreamSource (0, _vertexArrayOmni.get(), 0);

			unsigned numPasses = _deferredMaterial->begin (device);
			for (unsigned pass = 0; pass < numPasses; ++pass)
			{
				if (_deferredMaterial->beginPass (device, pass))
				{
					device->renderStreamsIndexed (_indexArrayOmni.get(), ATOM_PRIMITIVE_TRIANGLES, 12);
					_deferredMaterial->endPass (device, pass);
				}
			}
			_deferredMaterial->end (device);
		}
	}

#else

	_deferredMaterial->setActiveEffect ("omni");

	const ATOM_Matrix4x4f &viewMatrix = camera->getViewMatrix ();
	const ATOM_Matrix4x4f &projMatrix = camera->getProjectionMatrix ();

	static const ATOM_Vector3f directions[8] = {
		ATOM_Vector3f (-1.f,  1.f, -1.f),
		ATOM_Vector3f ( 1.f,  1.f, -1.f),
		ATOM_Vector3f ( 1.f,  1.f,  1.f),
		ATOM_Vector3f (-1.f,  1.f,  1.f),
		ATOM_Vector3f (-1.f, -1.f, -1.f),
		ATOM_Vector3f ( 1.f, -1.f, -1.f),
		ATOM_Vector3f ( 1.f, -1.f,  1.f),
		ATOM_Vector3f (-1.f, -1.f,  1.f)
	};

	struct LightVertex
	{
		ATOM_Vector3f xyz;
		ATOM_Vector3f lightParams;
		ATOM_Vector3f lightAttenuation;
	};

	device->setVertexDecl (_vertexDeclOmni);

	for (unsigned i = 0; i < _omniLights.size(); ++i)
	{
		const ATOM_Light *light = _omniLights[i].get();
		if( !light )
			continue;

		LightVertex *vertices = (LightVertex*)_vertexArrayOmni->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
		if (!vertices)
		{
			return;
		}

		const ATOM_Vector3f &lightPos = light->getPosition();
		ATOM_Vector3f lightParams (lightPos.x, lightPos.y, lightPos.z);
		const ATOM_Vector3f &lightAtt = light->getAttenuation ();
		_currentLightColor = light->getColor4f ();

		for (unsigned j = 0; j < 8; ++j)
		{
			vertices->xyz = lightPos + directions[j] * light->getRange();
			vertices->lightParams = lightParams;
			vertices->lightAttenuation = lightAtt;
			++vertices;
		}

		_vertexArrayOmni->unlock ();

		device->setStreamSource (0, _vertexArrayOmni.get(), 0);

		unsigned numPasses = _deferredMaterial->begin (device);
		for (unsigned pass = 0; pass < numPasses; ++pass)
		{
			if (_deferredMaterial->beginPass (device, pass))
			{
				device->renderStreamsIndexed (_indexArrayOmni.get(), ATOM_PRIMITIVE_TRIANGLES, 12);
				_deferredMaterial->endPass (device, pass);
			}
		}
		_deferredMaterial->end (device);
	}

#endif
}

const int ATOM_DeferredRenderScheme::getNumDirectionalLight() const
{
	return _directionalLights.size();
}
const int ATOM_DeferredRenderScheme::getNumPointLight() const
{
	return _omniLights.size();
}

#endif
//-----------------------------------------------//


void ATOM_DeferredRenderScheme::sunShadowTexture_Callback (ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	if (currentDeferredScheme)
	{
		param->setTexture (currentDeferredScheme->_shadowMapRenderScheme->getShadowMask());
		//((ATOM_MaterialTextureParam*)param)->setValue (currentDeferredScheme->_shadowMapRenderScheme->getShadowMask());
	}
}

void ATOM_DeferredRenderScheme::ambient_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	if (ATOM_RenderScheme::getCurrentScene())
	{
		param->setVector (ATOM_RenderScheme::getCurrentScene()->getAmbientLight());

		if (vsConstantPtr)
		{
			*vsConstantPtr = *param->v;
		}

		if (psConstantPtr)
		{
			*psConstantPtr = *param->v;
		}
		//((ATOM_MaterialVectorParam*)param)->setValue (ATOM_RenderScheme::getCurrentScene()->getAmbientLight());
	}
}

void ATOM_DeferredRenderScheme::depthTexture_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	if (currentDeferredScheme)
	{
		param->setTexture (currentDeferredScheme->_depthTexture.get());
		//((ATOM_MaterialTextureParam*)param)->setValue (currentDeferredScheme->_depthTexture.get());
	}
}

void ATOM_DeferredRenderScheme::normalTexture_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	if (currentDeferredScheme)
	{
		param->setTexture (currentDeferredScheme->_normalTexture.get());
		//((ATOM_MaterialTextureParam*)param)->setValue (currentDeferredScheme->_normalTexture.get());
	}
}

void ATOM_DeferredRenderScheme::albedoTexture_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	if (currentDeferredScheme)
	{
		param->setTexture (currentDeferredScheme->_albedoTexture.get());
		//((ATOM_MaterialTextureParam*)param)->setValue (currentDeferredScheme->_albedoTexture.get());
	}
}

//void ATOM_DeferredRenderScheme::auxTexture0_Callback(ATOM_MaterialParam *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
//{
//	if (currentDeferredScheme)
//	{
//		((ATOM_MaterialTextureParam*)param)->setValue (currentDeferredScheme->_auxTexture0.get());
//	}
//}

void ATOM_DeferredRenderScheme::lightBuffer_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	if (currentDeferredScheme)
	{
		param->setTexture (currentDeferredScheme->_lightAccTexture.get());
		//((ATOM_MaterialTextureParam*)param)->setValue (currentDeferredScheme->_lightAccTexture.get());
	}
}

void ATOM_DeferredRenderScheme::shadedTexture_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	if (currentDeferredScheme)
	{
		param->setTexture (currentDeferredScheme->_shadedTexture.get());
		//((ATOM_MaterialTextureParam*)param)->setValue (currentDeferredScheme->_shadedTexture.get());
	}
}

void ATOM_DeferredRenderScheme::composTexture_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	if (currentDeferredScheme)
	{
		param->setTexture (currentDeferredScheme->_ldrResult[0].get());
		//((ATOM_MaterialTextureParam*)param)->setValue (currentDeferredScheme->_ldrResult[0].get());
	}
}

void ATOM_DeferredRenderScheme::linearizeDepthParam_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	ATOM_Camera *camera = ATOM_RenderScheme::getCurrentScene()->getCamera();
	float f = camera->getFarPlane();
	float n = camera->getNearPlane();
	float q = f/(f-n);
	float mq = n*q;
	param->setVector (ATOM_Vector4f(mq, q, n, f));

	if (vsConstantPtr)
	{
		*vsConstantPtr = *param->v;
	}

	if (psConstantPtr)
	{
		*psConstantPtr = *param->v;
	}
	//((ATOM_MaterialVectorParam*)param)->setValue (ATOM_Vector4f(mq, q, n, f));
}

void ATOM_DeferredRenderScheme::sizeParam_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	if (currentDeferredScheme)
	{
		float sw = currentDeferredScheme->_depthTexture->getWidth();
		float sh = currentDeferredScheme->_depthTexture->getHeight();
		float tw = 1.f / (2.f * sw);
		float th = 1.f / (2.f * sh);
		param->setVector (ATOM_Vector4f(sw, sh, tw, th));
		//((ATOM_MaterialVectorParam*)param)->setValue (ATOM_Vector4f(sw, sh, tw, th));

		if (vsConstantPtr)
		{
			*vsConstantPtr = *param->v;
		}

		if (psConstantPtr)
		{
			*psConstantPtr = *param->v;
		}
	}
}

void ATOM_DeferredRenderScheme::viewAspect_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	float val = ATOM_RenderScheme::getCurrentScene()->getCamera()->getAspect();
	param->setFloat (val);

	if (vsConstantPtr)
	{
		vsConstantPtr->set(val, val, val, val);
	}

	if (psConstantPtr)
	{
		psConstantPtr->set(val, val, val, val);
	}
	//((ATOM_MaterialFloatParam*)param)->setValue (ATOM_RenderScheme::getCurrentScene()->getCamera()->getAspect());
}

void ATOM_DeferredRenderScheme::invTanHalfFov_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	float val = ATOM_RenderScheme::getCurrentScene()->getCamera()->getInvTanHalfFovy();
	param->setFloat (val);

	if (vsConstantPtr)
	{
		vsConstantPtr->set(val, val, val, val);
	}

	if (psConstantPtr)
	{
		psConstantPtr->set(val, val, val, val);
	}
	//((ATOM_MaterialFloatParam*)param)->setValue (ATOM_RenderScheme::getCurrentScene()->getCamera()->getInvTanHalfFovy());
}

void ATOM_DeferredRenderScheme::lightColor_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	if (currentDeferredScheme)
	{
		param->setVector(currentDeferredScheme->_currentLightColor);

		if (vsConstantPtr)
		{
			*vsConstantPtr = *param->v;
		}

		if (psConstantPtr)
		{
			*psConstantPtr = *param->v;
		}
		//((ATOM_MaterialVectorParam*)param)->setValue (currentDeferredScheme->_currentLightColor);
	}
}

void ATOM_DeferredRenderScheme::lightDir_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	if (currentDeferredScheme)
	{
		param->setVector (currentDeferredScheme->_currentLightDir);

		if (vsConstantPtr)
		{
			*vsConstantPtr = *param->v;
		}

		if (psConstantPtr)
		{
			*psConstantPtr = *param->v;
		}
		//((ATOM_MaterialVectorParam*)param)->setValue (currentDeferredScheme->_currentLightDir);
	}
}

void ATOM_DeferredRenderScheme::sunLightColor_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	if (currentDeferredScheme)
	{
		if (currentDeferredScheme->_sunLight)
			param->setVector (currentDeferredScheme->_sunLight->getColor4f());
			//((ATOM_MaterialVectorParam*)param)->setValue (currentDeferredScheme->_sunLight->getColor4f());
		else
			param->setVector (ATOM_Vector4f(0.f, 0.f, 0.f, 0.f));
			//((ATOM_MaterialVectorParam*)param)->setValue (ATOM_Vector4f(0.f, 0.f, 0.f, 0.f));

		if (vsConstantPtr)
		{
			*vsConstantPtr = *param->v;
		}

		if (psConstantPtr)
		{
			*psConstantPtr = *param->v;
		}
	}
}

void ATOM_DeferredRenderScheme::sunLightDir_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	if (currentDeferredScheme)
	{
		if (currentDeferredScheme->_sunLight)
		{
			ATOM_Vector4f v;
			v.setVector3 (currentDeferredScheme->_sunLight->getDirection());
			v.w = 0.f;
			param->setVector (v);
			//((ATOM_MaterialVectorParam*)param)->setValue (v);
		}
		else
		{
			param->setVector (ATOM_Vector4f(0.f, 1.f, 0.f, 0.f));
			//((ATOM_MaterialVectorParam*)param)->setValue (ATOM_Vector3f(0.f, 1.f, 0.f));
		}

		if (vsConstantPtr)
		{
			*vsConstantPtr = *param->v;
		}

		if (psConstantPtr)
		{
			*psConstantPtr = *param->v;
		}
	}
}

//--- wangjian added ---//
void ATOM_DeferredRenderScheme::invScreenSize_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	if (currentDeferredScheme)
	{
		param->setVector (ATOM_Vector4f(	1.0f / currentDeferredScheme->getWidth (),1.0f / currentDeferredScheme->getHeight(),0,0));
		if (vsConstantPtr)
		{
			*vsConstantPtr = *param->v;
		}
		if (psConstantPtr)
		{
			*psConstantPtr = *param->v;
		}
	}
		//((ATOM_MaterialVectorParam*)param)->setValue ( ATOM_Vector4f(	1.0f / currentDeferredScheme->getWidth (),1.0f / currentDeferredScheme->getHeight(),0,0 ) );
}

void ATOM_DeferredRenderScheme::useHDR_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	float val = ATOM_RenderSettings::isHDRRenderingEnabled() ? 1 : 0;
	param->setFloat (val);
	if (vsConstantPtr)
	{
		vsConstantPtr->set(val, val, val, val);
	}
	if (psConstantPtr)
	{
		psConstantPtr->set(val, val, val, val);
	}
	//((ATOM_MaterialFloatParam*)param)->setValue ( ATOM_RenderSettings::isHDRRenderingEnabled() ? 1 : 0 );
}

void ATOM_DeferredRenderScheme::shadedSceneTexture_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	if (currentDeferredScheme)
	{
		param->setTexture (currentDeferredScheme->_ldrResult[1].get());
		//((ATOM_MaterialTextureParam*)param)->setValue (currentDeferredScheme->_ldrResult[1].get());
	}
}

void ATOM_DeferredRenderScheme::waterCausticsTexture_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	if (currentDeferredScheme)
	{
		static ATOM_VECTOR<ATOM_AUTOREF(ATOM_Texture)>		_causticsTexs;
		static const int _sequcenCount = 32;
		static bool bTexLoaded = true;
		if( _causticsTexs.empty() && bTexLoaded )
		{
			for( int i = 0; i < _sequcenCount; ++i )
			{
				char causticsname[128]= {0};
				sprintf(causticsname,"/textures/caustics/Caustics_%d.dds",i);
				ATOM_AUTOREF(ATOM_Texture) tex = ATOM_CreateTextureResource(causticsname,ATOM_PIXEL_FORMAT_UNKNOWN,1/*ATOM_LoadPriority_IMMEDIATE*/);
				if( tex )
				{
					_causticsTexs.push_back(tex);
				}
				else
				{
					bTexLoaded = false;
					ATOM_LOGGER::warning("the caustics sequence texture load failed!\n");
					break;
				}
			}
		}
		if( bTexLoaded && !_causticsTexs.empty() )
		{
			static const float _timeInterval = 0.0333f * 3;
			int _frame = (int( ATOM_APP->getFrameStamp().currentTick * 0.001f / _timeInterval )) % _sequcenCount;
			if( _frame >= 0 && _frame < _causticsTexs.size() )
				param->setTexture (_causticsTexs[_frame].get());
				//((ATOM_MaterialTextureParam*)param)->setValue (_causticsTexs[_frame].get());
		}
	}
}

void ATOM_DeferredRenderScheme::screenCoordScaleBias_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	if (currentDeferredScheme)
	{
		float screenWidth		= (float)currentDeferredScheme->_albedoTexture->getWidth();
		float screenHeight		= (float)currentDeferredScheme->_albedoTexture->getHeight();
		float viewportWidth		= (float)currentDeferredScheme->getWidth();
		float viewportHeight	= (float)currentDeferredScheme->getHeight();
		param->setVector (ATOM_Vector4f(	viewportWidth / screenWidth,
										viewportHeight / screenHeight,
										0,
										0 ) );

		if (vsConstantPtr)
		{
			*vsConstantPtr = *param->v;
		}

		if (psConstantPtr)
		{
			*psConstantPtr = *param->v;
		}
	}
}

//{
//	//((ATOM_MaterialFloatParam*)param)->setValue ( ATOM_RenderSettings::isSSSRenderingEnabled() ? 1 : 0 );
//}
void ATOM_DeferredRenderScheme::SSSBRDFTexture_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	if (currentDeferredScheme)
	{
		static ATOM_AUTOREF(ATOM_Texture)		_brdfTex = 0;
		static bool bTexLoaded = true;
		if( !_brdfTex && bTexLoaded )
		{
			_brdfTex = ATOM_CreateTextureResource(	"/textures/sss/brdf_sss.png",
													ATOM_PIXEL_FORMAT_UNKNOWN,
													ATOM_LoadPriority_IMMEDIATE	);
			if( !_brdfTex )
			{
				bTexLoaded = false;
				ATOM_LOGGER::warning("the brdf_sss texture load failed!\n");
			}
		}
		if( bTexLoaded && _brdfTex )
		{
			param->setTexture (_brdfTex.get());
			//((ATOM_MaterialTextureParam*)param)->setValue (_brdfTex.get());
		}
		else
		{
			param->setTexture ((ATOM_Texture*)nullptr);
			//((ATOM_MaterialTextureParam*)param)->setValue (0);
		}
	}
}

void ATOM_DeferredRenderScheme::ssaoTexture_Callback (ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	if (currentDeferredScheme)
	{
		ATOM_PostEffectChain * chain = currentDeferredScheme->getPostEffectChain();
		if( !chain )
		{
			param->setTexture(ATOM_GetColorTexture(0xffffffff));
			return;
		}
		ATOM_SSAOEffect * effect = (ATOM_SSAOEffect*)chain->getEffect("ATOM_SSAOEffect");
		if( !effect )
		{
			param->setTexture(ATOM_GetColorTexture(0xffffffff));
			return;
		}
		if( !effect->getAOBuffer() )
		{
			param->setTexture(ATOM_GetColorTexture(0xffffffff));
			return;
		}
		param->setTexture (effect->getAOBuffer());
		//((ATOM_MaterialTextureParam*)param)->setValue (currentDeferredScheme->_shadowMapRenderScheme->getShadowMask());
	}
}
//----------------------//

unsigned ATOM_DeferredRenderScheme::getWidth () const
{
	return _currentViewportWidth;
}

unsigned ATOM_DeferredRenderScheme::getHeight () const
{
	return _currentViewportHeight;
}

ATOM_DepthBuffer *ATOM_DeferredRenderScheme::getDepthBuffer (void) const
{
	return _depthTexture->getDepthBuffer ();
}

ATOM_Texture *ATOM_DeferredRenderScheme::getDepthTexture (void) const
{
	return _depthTexture.get();
}

ATOM_Texture *ATOM_DeferredRenderScheme::getAlbedoTexture (void) const
{
	return _albedoTexture.get();
}

void ATOM_DeferredRenderScheme::setDrawAmbientOnly (bool b)
{
	_ambientOnly = b;
}

bool ATOM_DeferredRenderScheme::isDrawAmbientOnly (void) const
{
	return _ambientOnly;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ATOM_DeferredRenderSchemeFactory::ATOM_DeferredRenderSchemeFactory (void)
{
}

ATOM_DeferredRenderSchemeFactory::~ATOM_DeferredRenderSchemeFactory (void)
{
}

ATOM_RenderScheme *ATOM_DeferredRenderSchemeFactory::create (void)
{
	return ATOM_NEW(ATOM_DeferredRenderScheme, this);
}

void ATOM_DeferredRenderSchemeFactory::destroy (ATOM_RenderScheme *renderScheme)
{
	ATOM_DELETE(renderScheme);
}

int ATOM_DeferredRenderSchemeFactory::getLayerNumber (const char *layerName) const
{
	if (!layerName)
	{
		return -1;
	}

	if (!stricmp (layerName, "GbufferConstruction"))
	{
		return ATOM_DeferredRenderScheme::RenderScheme_GbufferConstruction;
	}
	else if (!stricmp (layerName, "LightAccumulation"))
	{
		return ATOM_DeferredRenderScheme::RenderScheme_LightAccumulation;
	}
	else if (!stricmp (layerName, "Shading"))
	{
		return ATOM_DeferredRenderScheme::RenderScheme_Shading;
	}
	else if (!stricmp (layerName, "PostShading"))
	{
		return ATOM_DeferredRenderScheme::RenderScheme_PostShading;
	}
	//--- wangjian added ---//
	else if (!stricmp (layerName, "PostShadingCustom"))
	{
		return ATOM_DeferredRenderScheme::RenderScheme_PostShadingCustom;
	}
	else if (!stricmp (layerName, "SolidForward"))
	{
		return ATOM_DeferredRenderScheme::RenderScheme_SolidForward;
	}
	//----------------------//
	else if (!stricmp (layerName, "Forward"))
	{
		return ATOM_DeferredRenderScheme::RenderScheme_Forward;
	}
	else if (!stricmp (layerName, "PostForward"))
	{
		return ATOM_DeferredRenderScheme::RenderScheme_PostForward;
	}
	else if (!stricmp (layerName, "FinalComposition"))
	{
		return ATOM_DeferredRenderScheme::RenderScheme_FinalComposition;
	}
	else if (!stricmp (layerName, "PostComposition"))
	{
		return ATOM_DeferredRenderScheme::RenderScheme_PostComposition;
	}
	else if (!stricmp (layerName, "HUD"))
	{
		return ATOM_DeferredRenderScheme::RenderScheme_HUD;
	}
	else
	{
		return -1;
	}
}

const char *ATOM_DeferredRenderSchemeFactory::getLayerName (int layerNumber) const
{
	switch (layerNumber)
	{
	case ATOM_DeferredRenderScheme::RenderScheme_GbufferConstruction:
		return "GbufferConstruction";
	case ATOM_DeferredRenderScheme::RenderScheme_LightAccumulation:
		return "LightAccumulation";
	case ATOM_DeferredRenderScheme::RenderScheme_Shading:
		return "Shading";
	case ATOM_DeferredRenderScheme::RenderScheme_PostShading:
		return "PostShading";
	//--- wangjian added ---//
	case ATOM_DeferredRenderScheme::RenderScheme_PostShadingCustom:
		return "PostShadingCustom";
	case ATOM_DeferredRenderScheme::RenderScheme_SolidForward:
		return "SolidForward";
	//----------------------//
	case ATOM_DeferredRenderScheme::RenderScheme_Forward:
		return "Forward";
	case ATOM_DeferredRenderScheme::RenderScheme_PostForward:
		return "PostForward";
	case ATOM_DeferredRenderScheme::RenderScheme_FinalComposition:
		return "FinalComposition";
	case ATOM_DeferredRenderScheme::RenderScheme_PostComposition:
		return "PostComposition";
	case ATOM_DeferredRenderScheme::RenderScheme_HUD:
		return "HUD";
	default:
		return 0;
	}
}

//--- wangjian added ---//
unsigned ATOM_DeferredRenderSchemeFactory::getLayerCount() const
{
	return ATOM_DeferredRenderScheme::RenderScheme_DeferredSchemeCount;
}
unsigned ATOM_DeferredRenderSchemeFactory::getLayerMaskId( int layer ) const
{
	return 1 << ( layer + ATOM_DeferredShadowMapRenderScheme::RenderScheme_ShadowMapSchemeCount );
}
//----------------------//

///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////


int ATOM_CustomRenderScheme::bRender = 0;
ATOM_CustomRenderScheme *g_customRenderScheme = 0;

ATOM_AUTOREF(ATOM_Texture)		ATOM_CustomRenderScheme::_ldrResult[2] = {0};

ATOM_CustomRenderScheme::ATOM_CustomRenderScheme (ATOM_RenderSchemeFactory *factory): ATOM_RenderScheme (factory)
{
	_currentViewportWidth = 0;
	_currentViewportHeight = 0;
	
	_visitor = 0;

	for (unsigned i = 0; i < RenderScheme_CustomSchemeCount; ++i)
	{
		_renderQueue[i] = ATOM_NEW(ATOM_RenderQueue);
		_renderQueue[i]->setQueueId(i);
	}

	_isTransparent = false;
}

ATOM_CustomRenderScheme::~ATOM_CustomRenderScheme (void)
{
	ATOM_DELETE(_visitor);

	for (unsigned i = 0; i < RenderScheme_CustomSchemeCount; ++i)
	{
		ATOM_DELETE(_renderQueue[i]);
	}
}

void ATOM_CustomRenderScheme::registerTextureCallbacks (void)
{
	ATOM_ParameterChannel::registerChannel ("Custom_LightColorIntensity",		&custom_lightColor_Callback,		0, 0, ATOM_MaterialParam::ParamType_VectorArray);
	ATOM_ParameterChannel::registerChannel ("Custom_LightPosition",				&custom_lightPosition_Callback,		0, 0, ATOM_MaterialParam::ParamType_VectorArray);
	ATOM_ParameterChannel::registerChannel ("Custom_LightAttenuate",			&custom_lightAtten_Callback,		0, 0, ATOM_MaterialParam::ParamType_VectorArray);

	ATOM_ParameterChannel::registerChannel ("Custom_SceneAmbient",				&custom_ambient_Callback,			0, 0, ATOM_MaterialParam::ParamType_Vector);

	ATOM_ParameterChannel::registerChannel ("Custom_SunLightDir",				&custom_sunLightDir_Callback,		0, 0, ATOM_MaterialParam::ParamType_Vector);
	ATOM_ParameterChannel::registerChannel ("Custom_SunLightColorIntensity",	&custom_sunLightColor_Callback,		0, 0, ATOM_MaterialParam::ParamType_Vector);

	ATOM_ParameterChannel::registerChannel ("Custom_SizeParam", &custom_sizeParam_Callback, 0, 0, ATOM_MaterialParam::ParamType_Vector);
	ATOM_ParameterChannel::registerChannel ("Custom_ShadedSceneTexture", &custom_shadedSceneTexture_Callback, 0, 0, ATOM_MaterialParam::ParamType_Texture);
	ATOM_ParameterChannel::registerChannel ("Custom_FogFactor", &custom_fogFactor_Callback, 0, 0, ATOM_MaterialParam::ParamType_Vector);
	ATOM_ParameterChannel::registerChannel ("Custom_FogColor", &custom_fogColor_Callback, 0, 0, ATOM_MaterialParam::ParamType_Vector);

}

void ATOM_CustomRenderScheme::unregisterTextureCallbacks (void)
{
	ATOM_ParameterChannel::unregisterChannel ("LightColorIntensity");
	ATOM_ParameterChannel::unregisterChannel ("LightPosition");
	ATOM_ParameterChannel::unregisterChannel ("LightAttenuate");
	ATOM_ParameterChannel::unregisterChannel ("SceneAmbient");
	ATOM_ParameterChannel::unregisterChannel ("SunLightDir");
	ATOM_ParameterChannel::unregisterChannel ("SunLightColorIntensity");

	ATOM_ParameterChannel::unregisterChannel ("Custom_SizeParam");
	ATOM_ParameterChannel::unregisterChannel ("Custom_ShadedSceneTexture");
	ATOM_ParameterChannel::unregisterChannel ("Custom_FogFactor");
	ATOM_ParameterChannel::unregisterChannel ("Custom_FogColor");
}

void ATOM_CustomRenderScheme::setCustomVisitor(ATOM_Visitor * visitor)
{
	_visitor = visitor;
}

const char * ATOM_CustomRenderScheme::getName (void) const
{
	return "custom";
}

bool ATOM_CustomRenderScheme::init (ATOM_RenderDevice *device, unsigned width, unsigned height)
{
	resize (device, width, height);
	return true;
}

bool ATOM_CustomRenderScheme::fini (ATOM_RenderDevice *device)
{
	return true;
}

unsigned ATOM_CustomRenderScheme::getWidth () const
{
	return _currentViewportWidth;
}

unsigned ATOM_CustomRenderScheme::getHeight () const
{
	return _currentViewportHeight;
}

void ATOM_CustomRenderScheme::onResize (ATOM_RenderDevice *device, unsigned width, unsigned height)
{
	if (width == _currentViewportWidth && height == _currentViewportHeight)
	{
		return;
	}

	_currentViewportWidth = width;
	_currentViewportHeight = height;
}

bool ATOM_CustomRenderScheme::draw (ATOM_RenderDevice *device, ATOM_Camera *camera)
{
	if (!ATOM_RenderScheme::getCurrentScene())
		return false;

	ATOM_Sky * sky = ATOM_RenderScheme::getCurrentScene()->getSkyNode();
	if( sky )
	{
		ATOM_Vector3f color ( sky->getFogColor() * sky->getFogDensity() * 1000.0f * 2 );
		device->setClearColor (NULL, color.x, color.y, color.z, 1.f );
		device->clear (true, true, true);
	}
	else
	{
		//device->setClearColor (NULL, 0, 0, 0, 0 );
		if( !_isTransparent )
		{
			device->setClearColor (NULL, 0, 0, 0, 1.f );
			device->clear (true, true, true);
		}
	}

	device->enableSRGBWrite(true);

#if 0
	_renderQueue[RenderScheme_Custom]->render (device, camera, ATOM_RenderQueue::SortNone, false);
#else

	ATOM_RenderScheme::setCurrentRenderSchemeLayer(RenderScheme_Solid);
	_renderQueue[RenderScheme_Solid]->render (device, camera, ATOM_RenderQueue::SortNone, false);

	ATOM_RenderScheme::setCurrentRenderSchemeLayer(RenderScheme_PostSolid);
	_renderQueue[RenderScheme_PostSolid]->render (device, camera, ATOM_RenderQueue::SortNone, false);

	ATOM_RenderScheme::setCurrentRenderSchemeLayer(RenderScheme_SolidTransparent);
	_renderQueue[RenderScheme_SolidTransparent]->render (device, camera, ATOM_RenderQueue::SortNone, false);

	ATOM_RenderScheme::setCurrentRenderSchemeLayer(RenderScheme_Transparent);
	_renderQueue[RenderScheme_Transparent]->render (device, camera, ATOM_RenderQueue::SortBackToFront, false);

	ATOM_RenderScheme::setCurrentRenderSchemeLayer(RenderScheme_HUD);
	_renderQueue[RenderScheme_HUD]->render (device, camera, ATOM_RenderQueue::SortNone, false);

#endif

	device->enableSRGBWrite(false);

	return true;
}

class ATOM_ModelChangeMaterialVistor : public ATOM_Visitor
{
public :
	
	virtual void visit (ATOM_Geode &node)
	{
		for( int i = 0; i < node.getNumMeshes(); ++i )
		{
			ATOM_StaticMesh * mesh = node.getStaticMesh(i);
			if( !mesh )
				continue;

			ATOM_AUTOPTR(ATOM_Material) _oldMaterial = mesh->getMaterial();
			if( !stricmp( _oldMaterial->getCoreMaterialFileName(),"/materials/builtin/model_custom_forward.mat" ) )
				continue;

			ATOM_AUTOPTR(ATOM_Material) _newMaterial = 
				ATOM_MaterialManager::createMaterialFromCore (	ATOM_GetRenderDevice(), "/materials/builtin/model_custom_forward.mat" );

			// 从旧材质拷贝参数值
			_newMaterial->getParameterTable()->getParameterFrom(_oldMaterial->getParameterTable());

			// 设置新材质
			node.getStaticMesh(i)->setMaterial( _newMaterial.get() );

			// 不使用实例化
			mesh->resetObjFlag(ATOM_Drawable::OF_GEOINSTANCING);
		}
	}
};

bool ATOM_CustomRenderScheme::onRender (ATOM_RenderDevice *device, ATOM_Scene *scene)
{
	g_customRenderScheme = this;

	scene->setRenderScheme (this);
	ATOM_RenderScheme::setCurrentRenderSchemeLayer(0);

	// first update 
	ATOM_UpdateVisitor updateVisitor;
	updateVisitor.setCamera (scene->getCamera ());
	updateVisitor.traverse (*scene->getRootNode ());

	/*if( !_visitor )
	{
		ATOM_Visitor * visitor = ATOM_NEW(ATOM_ModelChangeMaterialVistor);
		setCustomVisitor(visitor);
	}
	if( _visitor )
		_visitor->traverse(*scene->getRootNode());*/

	{
		class ATOM_CustomForwardCullVisitor: public ATOM_CullVisitor
		{
		public:
			ATOM_CustomForwardCullVisitor (ATOM_CustomRenderScheme * customForwardRenderScheme)
			{
				_customForwardRenderScheme = customForwardRenderScheme;
			}

		public:
			virtual void visit (ATOM_LightNode &node)
			{
				if (node.getShow () != ATOM_Node::SHOW)
					return;
				if( !node.getLight() )
					return;

				const ATOM_Matrix4x4f &projMatrix = getCamera()->getProjectionMatrix();
				const ATOM_Matrix4x4f &viewMatrix = getCamera()->getViewMatrix();
				const ATOM_BBox &bbox = node.getWorldBoundingbox ();

				if (frustumTest (&node, bbox))
				{
					node.updateLightParams (projMatrix, viewMatrix);
					_customForwardRenderScheme->addLight (node.getLight());
				}
			}
			virtual void visit (ATOM_Atmosphere2 &node)
			{

			}
			virtual void visit (ATOM_Sky &node)
			{
				if (node.getShow() == ATOM_Node::SHOW)
				{
					ATOM_CullVisitor::visit (node);
				}
				_customForwardRenderScheme->addLight (node.getSunLight());
			}

		private:
			ATOM_CustomRenderScheme *_customForwardRenderScheme;
		};

		_omniLights.resize (0);
		_directionalLights.resize (0);
		_spotLights.resize (0);

		ATOM_CustomForwardCullVisitor cull_v(this);
		for (unsigned i = 0; i < RenderScheme_CustomSchemeCount; ++i)
			_renderQueue[i]->clear ();

		cull_v.setCamera (scene->getCamera ());
		cull_v.setUpdateVisibleStamp (true);
		cull_v.setNumRenderQueues (RenderScheme_CustomSchemeCount);
		for (unsigned i = 0; i < RenderScheme_CustomSchemeCount; ++i)
		{
			cull_v.setRenderQueue (i, _renderQueue[i]);
		}
		cull_v.traverse (*scene->getRootNode ());
	}
	
	//----------------------------------------------------------------//

	draw(device,scene->getCamera());

	ATOM_RenderScheme::setCurrentRenderSchemeLayer(-1);

	return true;
}

int ATOM_CustomRenderScheme::onRenderPass (ATOM_RenderDevice *device, ATOM_Scene *scene, int layer)
{
	return -1;
}

void ATOM_CustomRenderScheme::addLight (ATOM_Light *light)
{
	switch (light->getLightType ())
	{
	case ATOM_Light::Point:
		if( _omniLights.size() < 4 )
			_omniLights.push_back (light);
		break;
	case ATOM_Light::Spot:
		_spotLights.push_back (light);
		break;
	case ATOM_Light::Directional:
		if( _directionalLights.empty() )
			_directionalLights.push_back (light);
		break;
	default:
		break;
	}
}

void ATOM_CustomRenderScheme::setTransparent (bool transparent)
{
	_isTransparent = transparent;
}

bool ATOM_CustomRenderScheme::isTransparent (void) const
{
	return _isTransparent;
}

void ATOM_CustomRenderScheme::custom_lightColor_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	if (g_customRenderScheme)
	{
		ATOM_VECTOR<ATOM_AUTOPTR(ATOM_Light)> & point_lights = g_customRenderScheme->_omniLights;

#if 0
		if (vsConstantPtr)
		{
			vsConstantPtr[0] = point_lights[0]->getColor4f();
			vsConstantPtr[1] = point_lights[1]->getColor4f();
			vsConstantPtr[2] = point_lights[2]->getColor4f();
			vsConstantPtr[3] = point_lights[3]->getColor4f();
		}
		if (psConstantPtr)
		{
			psConstantPtr[0] = point_lights[0]->getColor4f();
			psConstantPtr[1] = point_lights[1]->getColor4f();
			psConstantPtr[2] = point_lights[2]->getColor4f();
			psConstantPtr[3] = point_lights[3]->getColor4f();
		}
#else
		ATOM_Vector4f light_colors[4];
		memset( light_colors,0,sizeof(ATOM_Vector4f)*4);
		for( int i = 0; i < point_lights.size(); ++i )
			light_colors[i] = point_lights[i]->getColor4f();
		//param->setVectorArray(light_colors, 4);
		if (vsConstantPtr)
		{
			param->handle->writeRegisterValue (vsConstantPtr, light_colors, descVS);
		}
		if (psConstantPtr)
		{
			param->handle->writeRegisterValue (psConstantPtr, light_colors, descPS);
		}
		//((ATOM_MaterialVectorArrayParam*)param)->setValue (light_colors,4);
#endif
	}
}
void ATOM_CustomRenderScheme::custom_lightPosition_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	if (g_customRenderScheme && ATOM_RenderScheme::getCurrentScene())
	{
		ATOM_VECTOR<ATOM_AUTOPTR(ATOM_Light)> & point_lights = g_customRenderScheme->_omniLights;
		ATOM_Vector4f light_positions[4];
		memset( light_positions,0,sizeof(ATOM_Vector4f)*4);

		const ATOM_Matrix4x4f & matView = ATOM_RenderScheme::getCurrentScene()->getCamera ()->getInvViewMatrix();

		for( int i = 0; i < point_lights.size(); ++i )
		{
			light_positions[i] = point_lights[i]->getPosition();
			light_positions[i] <<= matView;
		}

#if 0
		if (vsConstantPtr)
		{
			memcpy (vsConstantPtr, light_positions, 4*sizeof(float)*4);
		}
		if (psConstantPtr)
		{
			memcpy (psConstantPtr, light_positions, 4*sizeof(float)*4);
		}
#else
		//param->setVectorArray (light_positions, 4);
		if (vsConstantPtr)
		{
			param->handle->writeRegisterValue (vsConstantPtr, light_positions, descVS);
		}

		if (psConstantPtr)
		{
			param->handle->writeRegisterValue (psConstantPtr, light_positions, descPS);
		}
#endif
		//((ATOM_MaterialVectorArrayParam*)param)->setValue (light_positions,4);
	}
}
void ATOM_CustomRenderScheme::custom_lightAtten_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	if (g_customRenderScheme)
	{
		ATOM_VECTOR<ATOM_AUTOPTR(ATOM_Light)> & point_lights = g_customRenderScheme->_omniLights;

#if 0
		if (vsConstantPtr)
		{
			vsConstantPtr[0] = point_lights[0]->getAttenuation();
			vsConstantPtr[1] = point_lights[1]->getAttenuation();
			vsConstantPtr[2] = point_lights[2]->getAttenuation();
			vsConstantPtr[3] = point_lights[3]->getAttenuation();
		}
		if (psConstantPtr)
		{
			psConstantPtr[0] = point_lights[0]->getAttenuation();
			psConstantPtr[1] = point_lights[1]->getAttenuation();
			psConstantPtr[2] = point_lights[2]->getAttenuation();
			psConstantPtr[3] = point_lights[3]->getAttenuation();
		}
#else
		ATOM_Vector4f light_attens[4];
		memset( light_attens,0,sizeof(ATOM_Vector4f)*4);
		for( int i = 0; i < point_lights.size(); ++i )
			light_attens[i] = point_lights[i]->getAttenuation();

		//param->setVectorArray (light_attens, 4);
		if (vsConstantPtr)
		{
			param->handle->writeRegisterValue (vsConstantPtr, light_attens, descVS);
		}

		if (psConstantPtr)
		{
			param->handle->writeRegisterValue (psConstantPtr, light_attens, descPS);
		}
#endif
		//((ATOM_MaterialVectorArrayParam*)param)->setValue (light_attens,4);
	}
}
void ATOM_CustomRenderScheme::custom_ambient_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	if (ATOM_RenderScheme::getCurrentScene())
	{
		param->setVector (ATOM_RenderScheme::getCurrentScene()->getAmbientLight());
		//((ATOM_MaterialVectorParam*)param)->setValue (ATOM_RenderScheme::getCurrentScene()->getAmbientLight());

		if (vsConstantPtr)
		{
			*vsConstantPtr = *param->v;
		}
		if (psConstantPtr)
		{
			*psConstantPtr = *param->v;
		}
	}
}
void ATOM_CustomRenderScheme::custom_sunLightColor_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	if (g_customRenderScheme)
	{
		if (!g_customRenderScheme->_directionalLights.empty())
		{
			ATOM_VECTOR<ATOM_AUTOPTR(ATOM_Light)> & directional_lights = g_customRenderScheme->_directionalLights;
			param->setVector (directional_lights[0]->getColor4f());
			//((ATOM_MaterialVectorParam*)param)->setValue (directional_lights[0]->getColor4f());
		}
		else
		{
			param->setVector (ATOM_Vector4f(1.f, 1.f, 1.f, 1.f));
			//((ATOM_MaterialVectorParam*)param)->setValue (ATOM_Vector4f(1.f, 1.f, 1.f, 1.f));
		}

		if (vsConstantPtr)
		{
			*vsConstantPtr = *param->v;
		}
		if (psConstantPtr)
		{
			*psConstantPtr = *param->v;
		}
	}
}
void ATOM_CustomRenderScheme::custom_sunLightDir_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	if ( g_customRenderScheme && ATOM_RenderScheme::getCurrentScene() && ATOM_RenderScheme::getCurrentScene()->getCamera () )
	{
		if (!g_customRenderScheme->_directionalLights.empty())
		{
			const ATOM_Light *light = g_customRenderScheme->_directionalLights[0].get();
			if( !light )
			{
				param->setVector (ATOM_Vector4f(0.f, 0.f, -1.f, 0.f));
			}
			else
			{
				ATOM_Vector4f v;
				v.setVector3 (ATOM_RenderScheme::getCurrentScene()->getCamera ()->getInvViewMatrix().transformVectorAffine (light->getDirection()));
				v.normalize();
				v.w = 0.f;
				param->setVector (v);
			}
		}
		else
		{
			param->setVector (ATOM_Vector4f(0.f, 0.f, -1.f, 0.f));
			//((ATOM_MaterialVectorParam*)param)->setValue (ATOM_Vector3f(0.f, 0.f, -1.f));
		}

		if (vsConstantPtr)
		{
			*vsConstantPtr = *param->v;
		}
		if (psConstantPtr)
		{
			*psConstantPtr = *param->v;
		}
	}
}
void ATOM_CustomRenderScheme::custom_sizeParam_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	if ( g_customRenderScheme && g_customRenderScheme->_currentViewportWidth != 0 && g_customRenderScheme->_currentViewportHeight != 0 )
	{
		float sw = (float)g_customRenderScheme->_currentViewportWidth;
		float sh = (float)g_customRenderScheme->_currentViewportHeight;
		float tw = 1.f / (2.f * sw);
		float th = 1.f / (2.f * sh);
		param->setVector (ATOM_Vector4f(sw, sh, tw, th));
		//((ATOM_MaterialVectorParam*)param)->setValue (ATOM_Vector4f(sw, sh, tw, th));

		if (vsConstantPtr)
		{
			*vsConstantPtr = *param->v;
		}

		if (psConstantPtr)
		{
			*psConstantPtr = *param->v;
		}
	}
}
void ATOM_CustomRenderScheme::custom_shadedSceneTexture_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	if (g_customRenderScheme)
	{
		if(g_customRenderScheme->_ldrResult[1].get())
			param->setTexture (g_customRenderScheme->_ldrResult[1].get());
		else
			param->setTexture (ATOM_GetColorTexture(0));

		//((ATOM_MaterialTextureParam*)param)->setValue (currentDeferredScheme->_ldrResult[1].get());
	}
}
void ATOM_CustomRenderScheme::custom_fogFactor_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	if (ATOM_RenderScheme::getCurrentScene())
	{
		if( ATOM_Sky * sky = ATOM_RenderScheme::getCurrentScene()->getSkyNode() )
		{
			ATOM_Vector4f fog;
			fog.x = sky->getFogNear();
			fog.y = 1.0f/(sky->getFogFar()-fog.x);
			fog.z = sky->getFogFar();
			fog.w = sky->getFogDensity() * 1000.0f;
			param->setVector (fog);
		}
		else
		{
			param->setVector (ATOM_Vector4f(10000,10000,0,1));
		}

		//((ATOM_MaterialVectorParam*)param)->setValue (ATOM_RenderScheme::getCurrentScene()->getAmbientLight());

		if (vsConstantPtr)
		{
			*vsConstantPtr = *param->v;
		}
		if (psConstantPtr)
		{
			*psConstantPtr = *param->v;
		}
	}
}
void ATOM_CustomRenderScheme::custom_fogColor_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	if (ATOM_RenderScheme::getCurrentScene())
	{
		if( ATOM_Sky * sky = ATOM_RenderScheme::getCurrentScene()->getSkyNode() )
		{
			ATOM_Vector4f fogColor(sky->getFogColor());
			param->setVector (fogColor);
		}
		else
		{
			param->setVector (ATOM_Vector4f(1,1,1,1));
		}

		//((ATOM_MaterialVectorParam*)param)->setValue (ATOM_RenderScheme::getCurrentScene()->getAmbientLight());

		if (vsConstantPtr)
		{
			*vsConstantPtr = *param->v;
		}
		if (psConstantPtr)
		{
			*psConstantPtr = *param->v;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////

ATOM_CustomRenderSchemeFactory::ATOM_CustomRenderSchemeFactory (void)
{
}
ATOM_CustomRenderSchemeFactory::~ATOM_CustomRenderSchemeFactory (void)
{
}
ATOM_RenderScheme *ATOM_CustomRenderSchemeFactory::create (void)
{
	return ATOM_NEW(ATOM_CustomRenderScheme, this);
}
void ATOM_CustomRenderSchemeFactory::destroy (ATOM_RenderScheme *renderScheme)
{
	ATOM_DELETE(renderScheme);
}
int ATOM_CustomRenderSchemeFactory::getLayerNumber (const char *layerName) const
{
	if (!layerName)
	{
		return -1;
	}

#if 0

	if (!stricmp (layerName, "Forward"))
	{
		return ATOM_CustomRenderScheme::RenderScheme_Custom;
	}

#else

	if (!stricmp (layerName, "Solid"))
	{
		return ATOM_CustomRenderScheme::RenderScheme_Solid;
	}
	else if (!stricmp (layerName, "PostSolid"))
	{
		return ATOM_CustomRenderScheme::RenderScheme_PostSolid;
	}
	else if (!stricmp (layerName, "SolidTransparent"))
	{
		return ATOM_CustomRenderScheme::RenderScheme_SolidTransparent;
	}
	else if (!stricmp (layerName, "Transparent"))
	{
		return ATOM_CustomRenderScheme::RenderScheme_Transparent;
	}
	else if (!stricmp (layerName, "HUD"))
	{
		return ATOM_CustomRenderScheme::RenderScheme_HUD;
	}
#endif
	else
	{
		return -1;
	}
}
const char *ATOM_CustomRenderSchemeFactory::getLayerName (int layerNumber) const
{
	switch (layerNumber)
	{
#if 0

	case ATOM_CustomRenderScheme::RenderScheme_Custom:
		return "Forward";

#else

	case ATOM_CustomRenderScheme::RenderScheme_Solid:
		return "Solid";
	case ATOM_CustomRenderScheme::RenderScheme_PostSolid:
		return "PostSolid";
	case ATOM_CustomRenderScheme::RenderScheme_SolidTransparent:
		return "SolidTransparent";
	case ATOM_CustomRenderScheme::RenderScheme_Transparent:
		return "Transparent";
	case ATOM_CustomRenderScheme::RenderScheme_HUD:
		return "HUD";
#endif

	default:
		return 0;
	}
}
unsigned ATOM_CustomRenderSchemeFactory::getLayerCount() const
{
	return ATOM_CustomRenderScheme::RenderScheme_CustomSchemeCount;
}
unsigned ATOM_CustomRenderSchemeFactory::getLayerMaskId( int layer ) const
{
	return 1 << (	layer + 
					ATOM_DeferredShadowMapRenderScheme::RenderScheme_ShadowMapSchemeCount + 
					ATOM_DeferredRenderScheme::RenderScheme_DeferredSchemeCount );
}