#include "StdAfx.h"
#include "sdlscene.h"
#include "savevisitor.h"
#include "parameterchannel.h"
//--- wangjian added ---//
// 异步加载
#include "kernel/profiler.h"
//----------------------//

#define DEFAULT_SPLIT_LAMBDA 0.7f
#define DEFAULT_SHADOW_DISTANCE 1500.f
#define DEFAULT_LIGHT_DISTANCE 1000.f
#define DEFAULT_SCENE_SCALE 0.5f
#define DEFAULT_LIGHT_SIZE 0.01f
#define DEFAULT_DEPTH_BIAS 0.001f
#define DEFAULT_FADEMIN 1000.f
#define DEFAULT_FADEMAX 1500.f

//--- wangjian modified ---//
// 异步加载
static ATOM_AUTOREF(ATOM_Node) loadNode(	ATOM_RenderDevice *pDevice, 
											const ATOM_TiXmlElement *pXmlElement,
											bool bLoad = true
											//----------------------//
										)
{
	ATOM_STACK_TRACE(loadNode);
	ATOM_AUTOREF(ATOM_Node) p_node;

	if( !pXmlElement || !pDevice  )
	{
		ATOM_LOGGER::error("[ATOM_SceneIO::load]: Invalid params.\r\n");
		return 0;
	}

	const char *s_class	= pXmlElement->Attribute("class");
	if( !s_class )
	{
		ATOM_LOGGER::error("[ATOM_SceneIO::load]: Some node has not class attribute .\r\n");
		return 0;
	}

	if (!strcmp (s_class, ATOM_NodeOctree::_classname()))
	{
		return 0;
	}

	p_node = ATOM_HARDREF(ATOM_Node)(s_class, 0);

	if (p_node)
	{
		//--- wangjian mofidied ---//

		// 加载节点的属性 可能会修改LoadPriority标记
		if( !( p_node->loadAttribute(pXmlElement) ) )
		{
			ATOM_LOGGER::error("[ATOM_SceneIO::load]: node loadAttribute failed\n");
			return 0;
		}

		// 保存本身属性的LoadPriority标记
		int load_priority = p_node->getLoadPriority();	// 节点默认的加载优先级别为ATOM_LoadPriority_ASYNCBASE = 1

		// 如果是立即加载
		if( bLoad )
			p_node->setLoadPriority(ATOM_LoadPriority_IMMEDIATE);
		
		// 如果加载优先级为0 则当前就异步加载 而非在进入视野时再加载
		bool bNotDelayLoad = !bLoad && ( load_priority == 0 );

		bool bPs = strstr( s_class,"ParticleSystem");	// 粒子系统当前直接加载（因为纹理有不显示的问题）
		if( bPs )
			p_node->setLoadPriority(ATOM_LoadPriority_IMMEDIATE);

		bool bTerrain = strstr( s_class,"Terrain");		// 地形当前直接加载（使用本身保存的加载优先级加载）
		bNotDelayLoad |= bTerrain;
		//if( bTerrain )
		//	p_node->setLoadPriority(/*load_priority*/ATOM_LoadPriority_IMMEDIATE);

		bool bBKImage = strstr( s_class,"ATOM_BkImage");		// 地形当前直接加载（使用本身保存的加载优先级加载)
		if( bBKImage )
			p_node->setLoadPriority(ATOM_LoadPriority_IMMEDIATE);
		//----------------------//
			
		// 当前修改为不加载节点，而在CULLVISIT时加载节点的模型等信息
		if( bLoad || bPs || p_node->getLoadPriority() == ATOM_LoadPriority_IMMEDIATE || bNotDelayLoad )
		{
			// 加载
			if( !p_node->load(pDevice))
			{
				return 0;
			}
		}
		else
		{

		}

		// 恢复属性中固有的加载优先级标记
		p_node->setLoadPriority(load_priority);
		
	}

	return p_node;
}

typedef void (*SceneLoadCallback) (ATOM_Node *node, unsigned index, void *userdata);

//--- wangjian modified ---//
// 异步加载
static void IteratorElementNode(	ATOM_RenderDevice *pDevice, 
									const ATOM_TiXmlNode *pXmlNode, 
									ATOM_Node *pNodeParent, 
									SceneLoadCallback callback, 
									void *userdata, 
									ATOM_Scene::LoadNodeCallback nodeCallback,
									void *loadNodeUserData,
									unsigned & index,
									bool bLoad = true
									//----------------------//

								)
{
 	ATOM_ASSERT(pNodeParent&&pXmlNode);
	ATOM_AUTOREF(ATOM_Node) p_node;
	//const char *ts = NULL;
	const ATOM_TiXmlElement *p_xmlelement= NULL;
	//const ATOM_TiXmlText *p_xmltext	= NULL;
	p_xmlelement = pXmlNode->ToElement();
	if (p_xmlelement)
	{
		if( !_stricmp("node", p_xmlelement->Value()) )
		{
			//--- wangjian modified ---//
			// 异步加载
			if (nodeCallback)
				p_node = nodeCallback (pDevice, p_xmlelement, bLoad, loadNodeUserData);
			else
				p_node = loadNode(pDevice, p_xmlelement, bLoad);
			/*if( !bLoad || !( (ATOM_ParticleSystem*)p_node.get() ) )
			{
				ATOM_SDLScene::AddNodeToBlock(p_node.get(), index_block);
			}*/
			//-------------------------//

			++index;
			if (callback)
			{
				callback (p_node.get(), index, userdata);
			}
		}

		if( pNodeParent && p_node )
		{
			pNodeParent->appendChild(p_node.get());
		}
	}

	const ATOM_TiXmlElement *tp = pXmlNode->FirstChildElement();
	while( tp )
	{
		if (!_stricmp("node", tp->Value()))
		{
			//--- wangjian added 2 arguments ---//
			// 异步加载
			IteratorElementNode(pDevice, tp, p_node ? p_node.get() : pNodeParent, callback, userdata, nodeCallback, loadNodeUserData, index, bLoad);
			//----------------------------------//
		}

		tp = tp->NextSiblingElement();
	}
}

class TerrainFindVisitor: public ATOM_Visitor
{
public:
	TerrainFindVisitor (void): terrain(0) {}
	ATOM_Terrain *terrain;

public:
	virtual void visit (ATOM_Terrain &node) 
	{
		terrain = &node;
		cancelTraverse ();
	}
};

ATOM_SDLScene::ATOM_SDLScene (void)
{
	ATOM_STACK_TRACE(ATOM_SDLScene::ATOM_SDLScene);
	_directionalLight = ATOM_NEW(ATOM_Light);
	ATOM_Vector3f v(0.f, -1.f, 2.f);
	v.normalize ();
	_lightDir.setKeyValues (v);
	_lightColor.setKeyValues (ATOM_Vector4f(1.f, 1.f, 1.f, 1.f));
	_lightFactors.setKeyValues (ATOM_Vector4f(1.f, 1.f, 1.f, 1.f));
	_bloomThreshold = 0.7f;
	_bloomOffset = 10.f;
	//--- wangjian added ---//
	_bloomMultiplier = 1.0f;
	// HDR相关
	_hdr_enable = false;
	_filmCurveShoulder = 1.f;
	_filmCurveMiddleTone = 1.f;
	_filmCurveToe = 1.f;
	_filmCurveWhitePoint = 4.f;
	_HDRSaturate = 1.f;
	_HDRContrast = 1.f;
	_HDRColorBalance = ATOM_Vector3f(1,1,1);

	_complement_lightDir		= ATOM_Vector3f(0,-1,2);
	_complement_lightColor		= ATOM_Vector4f(1.f, 1.f, 1.f, 1.f);
	_complement_lightIntensity	= 0.0f;									// default is not enabled
	//----------------------//
	_splitLambda = DEFAULT_SPLIT_LAMBDA;
	_shadowDistance = DEFAULT_SHADOW_DISTANCE;
	_lightDistance = DEFAULT_LIGHT_DISTANCE;
	_sceneScale = DEFAULT_SCENE_SCALE;
	_lightSize = DEFAULT_LIGHT_SIZE;
	_depthBias = DEFAULT_DEPTH_BIAS;
	_sceneBBoxCalculated = false;
	_fixedCameraDirection.set (100.f, -100.f, 100.f);
	_fixedCameraPosition = - _fixedCameraDirection;
	_FOV = ATOM_Pi / 3.f;
	_nearPlane = 1.f;
	_farPlane = 2000.f;
	_fadeMin = DEFAULT_FADEMIN;
	_fadeMax = DEFAULT_FADEMAX;
	_cameraMatrix = 0;
	//--- wangjian added ---//
	// 阴影相关
	_shadowBlur = 1.0f;
	_shadowSlopeBias = 10.0f;
	_shadowDensity = 1.0f;
	//----------------------//
}

ATOM_SDLScene::~ATOM_SDLScene (void)
{
	ATOM_STACK_TRACE(ATOM_SDLScene::~ATOM_SDLScene);
	ATOM_DELETE(_cameraMatrix);
}

ATOM_Light *ATOM_SDLScene::getLight (void) const
{
	return _directionalLight.get();
}

void ATOM_SDLScene::setLightDirKey (int time, const ATOM_Vector3f &dir)
{
	_lightDir.setKeyValue (time, dir);
}

const ATOM_Vector3f &ATOM_SDLScene::getLightDirKey (int time) const
{
	return _lightDir.getKeyValue (time);
}

void ATOM_SDLScene::setLightColorKey (int time, const ATOM_Vector4f &color)
{
	_lightColor.setKeyValue (time, color);
}

const ATOM_Vector4f &ATOM_SDLScene::getLightColorKey (int time) const
{
	return _lightColor.getKeyValue (time);
}

void ATOM_SDLScene::setBloomThreshold (float value)
{
	_bloomThreshold = value;
}

float ATOM_SDLScene::getBloomThreshold (void) const
{
	return _bloomThreshold;
}

void ATOM_SDLScene::setBloomOffset (float value)
{
	_bloomOffset = value;
}

float ATOM_SDLScene::getBloomOffset (void) const
{
	return _bloomOffset;
}
//--- wangjian added ---//
void ATOM_SDLScene::setBloomMultiplier(float value)
{
	_bloomMultiplier = value;
}
float ATOM_SDLScene::getBloomMultiplier(void) const
{
	return _bloomMultiplier;
}
// HDR相关
void ATOM_SDLScene::setHDRToggle(bool toggle)
{
	_hdr_enable = toggle;
}
bool ATOM_SDLScene::getHDRToggle(void) const
{
	return _hdr_enable;
}
void ATOM_SDLScene::setHDRFilmCurveParams(	float filmCurveShoulder,
											float filmCurveMiddleTone,
											float filmCurveToe, 
											float filmCurveWhitePoint	)
{
	_filmCurveShoulder		= filmCurveShoulder;
	_filmCurveMiddleTone	= filmCurveMiddleTone;
	_filmCurveToe			= filmCurveToe;
	_filmCurveWhitePoint	= filmCurveWhitePoint;
}
void ATOM_SDLScene::getHDRFilmCurveParams(	float & filmCurveShoulder,
											float & filmCurveMiddleTone,
											float & filmCurveToe, 
											float & filmCurveWhitePoint	) const
{
	filmCurveShoulder	= _filmCurveShoulder;
	filmCurveMiddleTone = _filmCurveMiddleTone;
	filmCurveToe		= _filmCurveToe;
	filmCurveWhitePoint = _filmCurveWhitePoint;
}
void ATOM_SDLScene::setHDRColorParams(float HDRSaturate,float HDRContrast,ATOM_Vector3f HDRColorBalance)
{
	_HDRSaturate		= HDRSaturate;
	_HDRContrast		= HDRContrast;
	_HDRColorBalance	= HDRColorBalance;
}
void ATOM_SDLScene::getHDRColorParams(float & HDRSaturate,float & HDRContrast,ATOM_Vector3f & HDRColorBalance) const
{
	HDRSaturate		= _HDRSaturate;
	HDRContrast		= _HDRContrast;
	HDRColorBalance	= _HDRColorBalance;
}
//---------------------//

static const char *fillMaterialSrc = 
	"material { effect { source \"					\n"
	"	float4 ps(): COLOR0																							\n"
	"	{																											\n"
	"		return float4(100000.f, 100000.f, 100000.f, 100000.f);													\n"
	"	}																											\n"
	"	technique test																								\n"
	"	{																											\n"
	"		pass p0																									\n"
	"		{																										\n"
	"			CullMode = None;																					\n"
	"			ZEnable = False;																					\n"
	"			vertexshader = null;																				\n"
	"			pixelshader = compile ps_2_0 ps();																	\n"
	"		}																										\n"
	"	}\";																										\n"
	"} }																											\n";

ATOM_AUTOPTR(ATOM_Light) prevActiveLight;
ATOM_Vector4f prevAmbientLight = 0;

#if 0
void ATOM_SDLScene::beginRender (bool clear)
{
	ATOM_STACK_TRACE(ATOM_SDLScene::beginRender);
	ATOM_Scene::beginRender (clear);
	/*
	ATOM_RenderSettings::setLightTimeFactors (_lightFactors);

	_directionalLight->setColor4f (_lightColor.getCurrentValue());
	ATOM_Vector3f dir = _lightDir.getCurrentValue ();
	dir.normalize ();
	_directionalLight->setDirection (dir);

	prevActiveLight = ATOM_DefaultChannels::getActiveLight ();
	prevAmbientLight = ATOM_DefaultChannels::getAmbientLight ();
	ATOM_DefaultChannels::setActiveLight (_directionalLight.get());	
	ATOM_DefaultChannels::setAmbientLight (_ambientLight.getCurrentValue());

	if (ATOM_RenderSettings::isFixedFunction ())
	{
		ATOM_GetRenderDevice()->enableFog (false);

		ATOM_VertexLight light;
		memset (&light, 0, sizeof(ATOM_VertexLight));
		light.type = ATOM_VertexLight::VLT_DIRECTIONAL;
		light.direction = dir;
		light.diffuse = _lightColor.getCurrentValue ();
		light.specular = 0.f;
		light.ambient = 0.f;
		ATOM_GetRenderDevice()->setLight (0, &light);
		ATOM_GetRenderDevice()->enableLight (0, true);

		ATOM_VertexLightMaterial mat;
		mat.diffuse = 1.f;
		mat.ambient = _ambientLight.getCurrentValue();
		mat.specular = 0.f;
		mat.emissive.set(0.f, 0.f, 0.f, 0.f);
		mat.power = 0.f;
		ATOM_GetRenderDevice()->setMaterial (&mat);

		ATOM_GetRenderDevice()->setAmbientColor (0xFFFFFFFF);

		ATOM_GetRenderDevice()->enableVertexLighting (true);
	}
	*/
}

void ATOM_SDLScene::endRender (void)
{
	/*
	ATOM_STACK_TRACE(ATOM_SDLScene::endRender);
	if (ATOM_RenderSettings::isFixedFunction ())
	{
		ATOM_GetRenderDevice()->enableFog (false);
		ATOM_GetRenderDevice()->enableVertexLighting (false);
	}

	ATOM_DefaultChannels::setActiveLight (prevActiveLight.get());
	ATOM_DefaultChannels::setAmbientLight (prevAmbientLight);
	*/
	ATOM_Scene::endRender ();
}
#endif

struct MyUserData
{
	unsigned total;
	unsigned current;
	ATOM_Scene::LoadCallback callback;
	void *userdata;
};

static void sceneLoadCallback (ATOM_Node *node, unsigned index, void *userdata)
{
	MyUserData *p = (MyUserData*)userdata;
	p->current++;
	if (p->callback)
	{
		p->callback (p->current, p->total, p->userdata);
	}
}

//static void sceneAsyncLoadEditorCallback()
//{
//
//}

static void CountElementNode(const ATOM_TiXmlNode *pXmlNode, unsigned &count)
{
	ATOM_ASSERT(pXmlNode);

	const ATOM_TiXmlElement *p_xmlelement= 0;
	p_xmlelement = pXmlNode->ToElement();
	if (p_xmlelement)
	{
		if( !_stricmp("node", p_xmlelement->Value()) )
		{
			++count;
		}
	}

	const ATOM_TiXmlElement *tp = pXmlNode->FirstChildElement();
	while( tp )
	{
		CountElementNode(tp, count);

		tp = tp->NextSiblingElement();
	}
}

static unsigned GetNumNodes (const ATOM_TiXmlNode *pXmlNode)
{
	unsigned num = 0;

	if (pXmlNode)
	{
		CountElementNode (pXmlNode, num);
	}

	return num;
}

bool ATOM_SDLScene::load (const char *filename, ATOM_Node *parent, ATOM_Scene::LoadCallback callback, void *userdata)
{
	ATOM_STACK_TRACE(ATOM_SDLScene::load);

	//--- wangjian added for profile shader compiling ：测试性能用 不影响引擎 ---//
	//ATOM_Profiler profiler(filename);

	//--- wangjian added : 创建在进入关卡之前需要预先加载好的资源 ---//
	// 异步加载
	/*ATOM_STRING preload = filename;
	unsigned int index = preload.find_last_of('/')+1;
	preload.replace(preload.begin()+index,preload.end(),"preloads.xml");
	ATOM_AsyncLoader::ReadLevelPreLoadTable(preload.c_str());*/

	bAsyncLoad = false;
	//---------------------------------------------------------------------------//

	ATOM_AutoFile f(filename, ATOM_VFS::read|ATOM_VFS::text);
	if (!f)
	{
		ATOM_LOGGER::log ("%s(%s, 0x%08X, 0x%08X, 0x%08X) File not found\n", __FUNCTION__, filename, parent, callback, userdata);
		return false;
	}
	unsigned size = f->size ();
	char *p = ATOM_NEW_ARRAY(char, size);
	size = f->read (p, size);
	p[size] = '\0';
	//--- wangjian modified ---//
	ATOM_TiXmlDocument document;
	//-------------------------//
	document.Parse (p);
	ATOM_DELETE_ARRAY(p);
	if (document.Error())
	{
		ATOM_LOGGER::error ("%s(%s, 0x%08X) %s\n", __FUNCTION__, filename, parent, document.ErrorDesc());
		return false;
	}

	ATOM_TiXmlElement *root = document.RootElement ();
	if (!root)
	{
		ATOM_LOGGER::log ("%s(%s, 0x%08X, 0x%08X, 0x%08X) Root XML element not found\n", __FUNCTION__, filename, parent, callback, userdata);
		return false;
	}

	//--- wangjian added ---//
	if( _postEffectChain )
	{
		ATOM_DELETE(_postEffectChain);
		_postEffectChain = 0;
	}
	_postEffectChain = ATOM_NEW(ATOM_PostEffectChain);
	//----------------------//

	ATOM_TiXmlElement *ePostEffects = root->FirstChildElement ("PostEffects");
	if (ePostEffects)
	{
		for (ATOM_TiXmlElement *eEffect = ePostEffects->FirstChildElement ("Effect"); eEffect; eEffect = eEffect->NextSiblingElement ("Effect"))
		{
			const char *classname = eEffect->Attribute ("class");
			ATOM_AUTOREF(ATOM_PostEffect) postEffect = ATOM_CreateObject (classname, 0);
			if (postEffect && postEffect->loadAttribute (eEffect))
			{
				_postEffectChain->appendPostEffect (postEffect.get());
			}
		}
	}

	ATOM_DELETE(_cameraMatrix);
	_cameraMatrix = 0;

	ATOM_TiXmlElement *eCamera = root->FirstChildElement ("Camera");
	if (eCamera)
	{
		double fov = _FOV;
		double nearPlane = _nearPlane;
		double farPlane = _farPlane;
		double fixedX = _fixedCameraDirection.x;
		double fixedY = _fixedCameraDirection.y;
		double fixedZ = _fixedCameraDirection.z;
		double fixedPX = _fixedCameraPosition.x;
		double fixedPY = _fixedCameraPosition.y;
		double fixedPZ = _fixedCameraPosition.z;
		eCamera->QueryDoubleAttribute ("FOV", &fov);
		eCamera->QueryDoubleAttribute ("Near", &nearPlane);
		eCamera->QueryDoubleAttribute ("Far", &farPlane);
		eCamera->QueryDoubleAttribute ("FixedDirectionX", &fixedX);
		eCamera->QueryDoubleAttribute ("FixedDirectionY", &fixedY);
		eCamera->QueryDoubleAttribute ("FixedDirectionZ", &fixedZ);
		eCamera->QueryDoubleAttribute ("FixedPositionX", &fixedPX);
		eCamera->QueryDoubleAttribute ("FixedPositionY", &fixedPY);
		eCamera->QueryDoubleAttribute ("FixedPositionZ", &fixedPZ);
		const char *matrix = eCamera->Attribute ("Matrix");
		if (matrix)
		{
			ATOM_Matrix4x4f cameraMatrix;
			int n = sscanf (matrix, "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f",
				&cameraMatrix.m00, &cameraMatrix.m01, &cameraMatrix.m02, &cameraMatrix.m03,
				&cameraMatrix.m10, &cameraMatrix.m11, &cameraMatrix.m12, &cameraMatrix.m13,
				&cameraMatrix.m20, &cameraMatrix.m21, &cameraMatrix.m22, &cameraMatrix.m23,
				&cameraMatrix.m30, &cameraMatrix.m31, &cameraMatrix.m32, &cameraMatrix.m33);
			if (n == 16)
			{
				_cameraMatrix = ATOM_NEW(ATOM_Matrix4x4f, cameraMatrix);
			}
		}

		_FOV = fov;
		_nearPlane = nearPlane;
		_farPlane = farPlane;
		_fixedCameraDirection.set (fixedX, fixedY, fixedZ);
		_fixedCameraPosition.set (fixedPX, fixedPY, fixedPZ);
	}

	ATOM_TiXmlElement *eShadowParams = root->FirstChildElement("Shadow");
	double splitLambda = DEFAULT_SPLIT_LAMBDA;
	double shadowDistance = DEFAULT_SHADOW_DISTANCE;
	double lightDistance = DEFAULT_LIGHT_DISTANCE;
	double sceneScale = DEFAULT_SCENE_SCALE;
	double lightSize = DEFAULT_LIGHT_SIZE;
	double depthBias = DEFAULT_DEPTH_BIAS;
	double fadeMin = DEFAULT_FADEMIN;
	double fadeMax = DEFAULT_FADEMAX;
	//--- wangjian added ---//
	// 阴影相关
	double shadowblur = 1.0;
	double shadowSlopeBias = 10.0;
	double shadowDensity = 1.0;
	//----------------------//

	if (eShadowParams)
	{
		eShadowParams->QueryDoubleAttribute ("SplitLambda", &splitLambda);
		eShadowParams->QueryDoubleAttribute ("ShadowDistance", &shadowDistance);
		eShadowParams->QueryDoubleAttribute ("LightDistance", &lightDistance);
		eShadowParams->QueryDoubleAttribute ("SceneScale", &sceneScale);
		eShadowParams->QueryDoubleAttribute ("LightSize", &lightSize);
		eShadowParams->QueryDoubleAttribute ("DepthBias", &depthBias);
		eShadowParams->QueryDoubleAttribute ("FadeMin", &fadeMin);
		eShadowParams->QueryDoubleAttribute ("FadeMax", &fadeMax);
		//--- wangjian added ---//
		// 阴影相关
		eShadowParams->QueryDoubleAttribute ("ShadowBlur", &shadowblur);
		eShadowParams->QueryDoubleAttribute ("ShadowSlopeBias", &shadowSlopeBias);
		eShadowParams->QueryDoubleAttribute ("ShadowDensity", &shadowDensity);
		//----------------------//
	}
	_splitLambda = splitLambda;
	_shadowDistance = shadowDistance;
	_lightDistance = lightDistance;
	_sceneScale = sceneScale;
	_lightSize = lightSize;
	_depthBias = depthBias;
	_fadeMin = fadeMin;
	_fadeMax = fadeMax;
	//--- wangjian added ---//
	// 阴影相关
	_shadowBlur = shadowblur;
	_shadowSlopeBias = shadowSlopeBias;
	_shadowDensity = shadowDensity;
	//----------------------//

	ATOM_TiXmlElement *eLights = root->FirstChildElement ("Lights");
	ATOM_TiXmlElement *eLight = eLights ? eLights->FirstChildElement ("ATOM_Light") : 0;
	if (eLight)
	{
		getLight()->setLightType (ATOM_Light::Directional);

		ATOM_TiXmlElement *eLightKeys = eLight->FirstChildElement ("Keys");
		if (eLightKeys)
		{
			ATOM_TiXmlElement *eLightKey = 0;
			for (int i = 0; i < ATOM_TimeManager::numTimeKeys; ++i)
			{
				eLightKey = (i == 0) ? eLightKeys->FirstChildElement ("Key") : eLightKey->NextSiblingElement ("Key");
				if (!eLightKey)
				{
					ATOM_LOGGER::log ("%s(%s, 0x%08X, 0x%08X, 0x%08X) Light key value not found\n", __FUNCTION__, filename, parent, callback, userdata);
					return false;
				}
				const char *lightColor = eLightKey->Attribute ("LightColor");
				float r, g, b;
				if (!lightColor || 3 != sscanf (lightColor, "%f,%f,%f", &r, &g, &b))
				{
					r = 1.f;
					g = 1.f;
					b = 1.f;
				}
				setLightColorKey (i, ATOM_Vector4f(r, g, b, 1.f));

				ATOM_Vector3f normal;
				const char *lightDir = eLightKey->Attribute ("LightDir");
				if (!lightDir || 3 != sscanf (lightDir, "%f,%f,%f", &normal.x, &normal.y, &normal.z))
				{
					normal.set (0.f, -1.f, 2.f);
				}
				normal.normalize ();
				setLightDirKey (i, normal);
			}
		}
		else
		{
			const char *lightColor = eLight->Attribute ("LightColor");
			float r, g, b;
			if (!lightColor || 3 != sscanf (lightColor, "%f,%f,%f", &r, &g, &b))
			{
				r = 1.f;
				g = 1.f;
				b = 1.f;
			}

			ATOM_Vector3f normal;
			const char *lightDir = eLight->Attribute ("LightDir");
			if (!lightDir || 3 != sscanf (lightDir, "%f,%f,%f", &normal.x, &normal.y, &normal.z))
			{
				normal.set (0.f, -1.f, 2.f);
			}
			normal.normalize ();

			_lightColor.setKeyValues (ATOM_Vector4f (r, g, b, 1.f));
			_lightDir.setKeyValues (normal);
		}
	}
	else
	{
		ATOM_Vector3f v(0.f, -1.f, 2.f);
		v.normalize ();
		_lightColor.setKeyValues (ATOM_Vector4f (1.f, 1.f, 1.f, 1.f));
		_lightDir.setKeyValues (v);
	}

	ATOM_TiXmlElement *eLightFactors = root->FirstChildElement ("LightFactors");
	if (eLightFactors)
	{
		ATOM_TiXmlElement *eKey = 0;
		for (int i = 0; i < ATOM_TimeManager::numTimeKeys; ++i)
		{
			eKey = (i == 0) ? eLightFactors->FirstChildElement ("Key") : eKey->NextSiblingElement ("Key");
			if (!eKey)
			{
				ATOM_LOGGER::log ("%s(%s, 0x%08X, 0x%08X, 0x%08X) Light factor key value not found\n", __FUNCTION__, filename, parent, callback, userdata);
				return false;
			}
			float r, g, b;
			const char *factor = eKey->Attribute ("Color");
			if (!factor || 3 != sscanf (factor, "%f,%f,%f", &r, &g, &b))
			{
				r = 1.f;
				g = 1.f;
				b = 1.f;
			}
			setGlobalLightColorKey (i, ATOM_Vector4f(r, g, b, 1.f));
		}
	}
	else
	{
		_lightFactors.setKeyValues (ATOM_Vector4f(1.f, 1.f, 1.f, 1.f));
	}

	setAmbientLight (ATOM_Vector4f(0.3f, 0.3f, 0.3f, 1.f));
	ATOM_TiXmlElement *eAmbient = root->FirstChildElement ("Ambient");
	if (eAmbient)
	{
		ATOM_TiXmlElement *eKeys = eAmbient->FirstChildElement ("Keys");
		if (eKeys)
		{
			float ambientR;
			float ambientG;
			float ambientB;

			ATOM_TiXmlElement *eKey = eKeys->FirstChildElement ("Key");
			if (eKey)
			{
				const char *ambient = eKey->Attribute ("Color");
				if (ambient && 3 == sscanf (ambient, "%f,%f,%f", &ambientR, &ambientG, &ambientB))
				{
					setAmbientLight (ATOM_Vector4f(ambientR, ambientG, ambientB, 1.f));
				}
			}
		}
	}

	// 路径
	ATOM_TiXmlElement *ePathList = root->FirstChildElement ("Pathes");
	if (ePathList)
	{
		for (ATOM_TiXmlElement *ePath = ePathList->FirstChildElement ("Path"); ePath; ePath = ePath->NextSiblingElement("Path"))
		{
			int smoothness = 3;
			float radius = 1.f;
			const char *name = ePath->Attribute ("Name");
			if (!name || _pathes.find(name) != _pathes.end())
			{
				continue;
			}
			ePath->QueryIntAttribute ("Smoothness", &smoothness);
			ePath->QueryFloatAttribute ("Radius", &radius);
			const char *sWorldMatrix = ePath->Attribute ("WorldMatrix");
			ATOM_Path3D path;
			ATOM_Matrix4x4f mWorld;
			path.setRoundRadius (radius);
			path.setSmoothness (smoothness);
			if (!sWorldMatrix || 16 != sscanf(sWorldMatrix, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f"
				,&mWorld.m00,&mWorld.m01,&mWorld.m02,&mWorld.m03
				,&mWorld.m10,&mWorld.m11,&mWorld.m12,&mWorld.m13
				,&mWorld.m20,&mWorld.m21,&mWorld.m22,&mWorld.m23
				,&mWorld.m30,&mWorld.m31,&mWorld.m32,&mWorld.m33))
			{
				mWorld.makeIdentity ();
			}
			path.setWorldMatrix (mWorld);

			for (ATOM_TiXmlElement *eCtlPoint = ePath->FirstChildElement("Point"); eCtlPoint; eCtlPoint=eCtlPoint->NextSiblingElement("Point"))
			{
				ATOM_Matrix4x4f matrix;
				const char *s = eCtlPoint->Attribute ("Matrix");
				if (s && 16 == sscanf(s, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f"
					,&matrix.m00,&matrix.m01,&matrix.m02,&matrix.m03
					,&matrix.m10,&matrix.m11,&matrix.m12,&matrix.m13
					,&matrix.m20,&matrix.m21,&matrix.m22,&matrix.m23
					,&matrix.m30,&matrix.m31,&matrix.m32,&matrix.m33))
				{
					path.appendControlPoint (matrix);
				}
			}
			_pathes[name] = path;
		}
	}

	//--- wangjian modified ---//
	// HDR相关
	ATOM_TiXmlElement *eHDR = root->FirstChildElement ("HDR");
	_bloomThreshold = 0.7f;
	_bloomOffset = 10.f;
	_bloomMultiplier = 1.0f;
	_hdr_enable = false;
	_filmCurveShoulder = 1.f;;
	_filmCurveMiddleTone = 1.f;
	_filmCurveToe = 1.f;
	_filmCurveWhitePoint = 4.f;
	_HDRSaturate = 1.f;
	_HDRContrast = 1.f;
	_HDRColorBalance = ATOM_Vector3f(1,1,1);
	if (eHDR)
	{
		eHDR->QueryFloatAttribute("BloomThreshold", &_bloomThreshold);
		eHDR->QueryFloatAttribute("BloomOffset", &_bloomOffset);
		eHDR->QueryFloatAttribute("BloomMultiplier", &_bloomMultiplier);
		int bHDR;
		eHDR->QueryIntAttribute("HDREnable", &bHDR);
		_hdr_enable = false/*bHDR*/;
		eHDR->QueryFloatAttribute("FilmCurveShoulder", &_filmCurveShoulder);
		eHDR->QueryFloatAttribute("FilmCurveMiddleTone", &_filmCurveMiddleTone);
		eHDR->QueryFloatAttribute("FilmCurveToe", &_filmCurveToe);
		eHDR->QueryFloatAttribute("FilmCurveWhitePoint", &_filmCurveWhitePoint);
		eHDR->QueryFloatAttribute("HDRSaturate", &_HDRSaturate);
		eHDR->QueryFloatAttribute("HDRContrast", &_HDRContrast);
		eHDR->QueryFloatAttribute("HDRColorBalance_Red", &_HDRColorBalance.x);
		eHDR->QueryValueAttribute("HDRColorBalance_Green", &_HDRColorBalance.y);
		eHDR->QueryValueAttribute("HDRColorBalance_Blue", &_HDRColorBalance.z);
	}
	//------------------------//

	// wangjian added
	ATOM_TiXmlElement *eComplementLight = root->FirstChildElement ("ComplementLight");
	if( eComplementLight )
	{
		const char *lightDir = eComplementLight->Attribute ("LightDir");
		if (!lightDir || 3 != sscanf (lightDir, "%f,%f,%f", &_complement_lightDir.x, &_complement_lightDir.y, &_complement_lightDir.z))
		{
			_complement_lightDir.set (0.f, -1.f, 2.f);
		}
		_complement_lightDir.normalize ();
		const char *lightColor = eComplementLight->Attribute ("LightColor");
		if (!lightColor || 3 != sscanf (lightColor, "%f,%f,%f", &_complement_lightColor.x, &_complement_lightColor.y, &_complement_lightColor.z))
		{
			_complement_lightColor.set(1,1,1,1);
		}
		eComplementLight->QueryFloatAttribute("LightIntensity",&_complement_lightIntensity);
	}
	//--------------------------------//

	ATOM_AUTOREF(ATOM_Node) parentNode = parent ? parent : getRootNode();
	//ATOM_ASSERT(!parentNode->getParent());

	//--- wangjian modified for test loading progress ---//
	// 异步加载
	// 原来版本是通过遍历"Nodes"下面的所有子节点得到"Node"节点的数量
	// 改成将场景所有节点的数量保存到"Nodes"的属性"node_total_count"中
	// 在读取时直接读取 不再计算
	ATOM_TiXmlElement *eNodes = root->FirstChildElement ("Nodes");
	unsigned total = 1000;
	/*TiXmlElement *tmp = eNodes;
	unsigned total = 0;
	while (tmp)
	{
	total += GetNumNodes (tmp);
	tmp = tmp->NextSiblingElement ("Nodes");
	}*/
	
	if( eNodes )
	{
		if( 0 == eNodes->QueryIntAttribute("node_total_count",(int*)&total) )
		{
			TiXmlElement *tmp = eNodes;
			total = 0;
			while (tmp)
			{
				total += GetNumNodes (tmp);
				tmp = tmp->NextSiblingElement ("Nodes");
			}
		}
	}
	//---------------------------------------------------//

	MyUserData userData;
	userData.callback = callback;
	userData.total = total;
	userData.current = 0;
	userData.userdata = userdata;

	//--- wangjian added for test ---//
	ATOM_Vector3f eye, target, up, eyeDir;
	if( _cameraMatrix )
	{
		_cameraMatrix->decomposeLookatLH (eye, target, up);
		eyeDir = target - eye;
		eyeDir.normalize();
	}
	//-------------------------------//

	//--- wangjian added ---//
	// 异步加载
	ATOM_AsyncLoader::SetCurrentScene(this);

	while (eNodes)
	{
		const char *layer = eNodes->Attribute ("Layer");

		ATOM_HARDREF(ATOM_Node) node;
		ATOM_ASSERT(node);

		unsigned count = 0;

		//--- wangjian modified ---//
		// 异步加载
		IteratorElementNode(	ATOM_GetRenderDevice(), 
								eNodes, 
								node.get(), 
								&sceneLoadCallback, 
								&userData, 
								_loadNodeCallback,
								_loadNodeUserData,
								count, 
								ATOM_AsyncLoader::isEnableSceneMTLoad() ? false : true ); // 场景对象异步加载：在加载场景时不加载节点模型信息，只读取模型属性
		//-------------------------//

		////--- wangjian added ------//
		//// 异步加载
		//if( eBlocks && block_total_count > 0 )
		//{
		//	count = 0;
		//	IteratorElementBlockNode(	ATOM_GetRenderDevice(),
		//								eNodes,
		//								node.get(),
		//								&sceneLoadCallback, 
		//								&userData, 
		//								count, 
		//								eye, 
		//								eyeDir,
		//								!ATOM_AsyncLoader::IsRun(),		// 如果没有开启异步加载，直接在读取时就加载节点
		//								(_updateAsyncLoadAfterBlock && !_load_block_nodeByNode)	);	// 是否在每加载一个BLOCK后就立即更新异步加载
		//}
		////-------------------------//


		
		if (layer && layer[0])
		{
			if (node->getNumChildren() == 1)
			{
				node = node->getChild (0);
			}
			node->setLayer (layer);
			parentNode->appendChild (node.get());
		}
		else
		{
			//--- wangjian added for test ---//
			int count = node->getNumChildren();
			//-------------------------------//
			while (node->getNumChildren() > 0)
			{
				ATOM_AUTOREF(ATOM_Node) child = node->getChild (0);
				node->removeChild (child.get ());
				parentNode->appendChild (child.get());
			}
			//--- wangjian added for test ---//
			//parentNode->appendChild( block_node_root.get() );
			//-------------------------------//
		}

		eNodes = eNodes->NextSiblingElement ("Nodes");
	}

	if (parentNode != getRootNode())
	{
		getRootNode()->appendChild (parentNode.get());
	}

	calculateBoundingBox ();

	return true;
}

void ATOM_SDLScene::render (ATOM_RenderDevice *device, bool clear)
{
	ATOM_DeferredRenderScheme *scheme = dynamic_cast<ATOM_DeferredRenderScheme*>(getRenderScheme());
	if (scheme)
	{
		//--- wangjian added ---//
		// HDR相关
		scheme->setHDREnable( _hdr_enable );
		//----------------------//
		scheme->setHDRBrightPassThreshold (_bloomThreshold);
		scheme->setHDRBrightPassOffset (_bloomOffset);
		scheme->setHDRFilmCurveParams(_filmCurveShoulder,_filmCurveMiddleTone,_filmCurveToe,_filmCurveWhitePoint);
		scheme->setHDRColorParams(_HDRSaturate,_HDRContrast,_HDRColorBalance);
	}

	ATOM_Scene::render (device, clear);
}

bool ATOM_SDLScene::save (const char *filename, ATOM_Node *parent, ATOM_XmlSaveVisitor::FilterFunction filterFunction, void *filterUserData)
{
	ATOM_STACK_TRACE(ATOM_SDLScene::save);
	char buffer[256];

	char nativeFileName[ATOM_VFS::max_filename_length];
	ATOM_GetNativePathName (filename, nativeFileName);
	ATOM_TiXmlDocument doc (nativeFileName);

	ATOM_TiXmlDeclaration eDecl("1.0", "utf-8", "");
	doc.InsertEndChild (eDecl);

	ATOM_TiXmlElement eRoot("ATOM_Scene");
	
	if (_postEffectChain->getNumPostEffects () > 0)
	{
		ATOM_TiXmlElement ePostEffects("PostEffects");
		for (ATOM_PostEffect *eff = _postEffectChain->getFirstEffect(); eff; eff = eff->getNextEffect())
		{
			//----------- wangjian added -----------//
			// 如果是GAMMA_CORRECTION,跳过不保存
			if( eff->isGamma() )
			{
				continue;
			}
			//-------------------------------------//

			ATOM_TiXmlElement effElement("Effect");
			eff->writeAttribute (&effElement);
			ePostEffects.InsertEndChild (effElement);
		}
		eRoot.InsertEndChild (ePostEffects);
	}

	ATOM_TiXmlElement eShadowParams("Shadow");
	eShadowParams.SetDoubleAttribute ("SplitLambda", _splitLambda);
	eShadowParams.SetDoubleAttribute ("ShadowDistance", _shadowDistance);
	eShadowParams.SetDoubleAttribute ("LightDistance", _lightDistance);
	eShadowParams.SetDoubleAttribute ("SceneScale", _sceneScale);
	eShadowParams.SetDoubleAttribute ("LightSize", _lightSize);
	eShadowParams.SetDoubleAttribute ("DepthBias", _depthBias);
	eShadowParams.SetDoubleAttribute ("FadeMin", _fadeMin);
	eShadowParams.SetDoubleAttribute ("FadeMax", _fadeMax);
	//--- wangjian added ---//
	// 阴影相关
	eShadowParams.SetDoubleAttribute ("ShadowBlur", _shadowBlur);
	eShadowParams.SetDoubleAttribute ("ShadowSlopeBias", _shadowSlopeBias);
	eShadowParams.SetDoubleAttribute ("ShadowDensity", _shadowDensity);
	//----------------------//
	eRoot.InsertEndChild (eShadowParams);

	char matrix[2048];
	if (_cameraMatrix)
	{
		sprintf (matrix, "%.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f",
			_cameraMatrix->m00, _cameraMatrix->m01, _cameraMatrix->m02, _cameraMatrix->m03,
			_cameraMatrix->m10, _cameraMatrix->m11, _cameraMatrix->m12, _cameraMatrix->m13,
			_cameraMatrix->m20, _cameraMatrix->m21, _cameraMatrix->m22, _cameraMatrix->m23,
			_cameraMatrix->m30, _cameraMatrix->m31, _cameraMatrix->m32, _cameraMatrix->m33);
	}

	ATOM_TiXmlElement eCamera("Camera");
	eCamera.SetDoubleAttribute ("FOV", _FOV);
	eCamera.SetDoubleAttribute ("Near", _nearPlane);
	eCamera.SetDoubleAttribute ("Far", _farPlane);
	eCamera.SetDoubleAttribute ("FixedDirectionX", _fixedCameraDirection.x);
	eCamera.SetDoubleAttribute ("FixedDirectionY", _fixedCameraDirection.y);
	eCamera.SetDoubleAttribute ("FixedDirectionZ", _fixedCameraDirection.z);
	eCamera.SetDoubleAttribute ("FixedPositionX", _fixedCameraPosition.x);
	eCamera.SetDoubleAttribute ("FixedPositionY", _fixedCameraPosition.y);
	eCamera.SetDoubleAttribute ("FixedPositionZ", _fixedCameraPosition.z);
	if (_cameraMatrix)
	{
		eCamera.SetAttribute ("Matrix", matrix);
	}
	eRoot.InsertEndChild (eCamera);

	ATOM_TiXmlElement eLights("Lights");
	ATOM_TiXmlElement eLight("ATOM_Light");

	ATOM_TiXmlElement eKeys("Keys");
	for (int i = 0; i < ATOM_TimeManager::numTimeKeys; ++i)
	{
		ATOM_TiXmlElement eKey("Key");
		sprintf (buffer, "%f,%f,%f", getLightColorKey(i).x, getLightColorKey(i).y, getLightColorKey(i).z);
		eKey.SetAttribute ("LightColor", buffer);
		sprintf (buffer, "%f,%f,%f", getLightDirKey(i).x, getLightDirKey(i).y, getLightDirKey(i).z);
		eKey.SetAttribute ("LightDir", buffer);
		eKeys.InsertEndChild (eKey);
	}
	eLight.InsertEndChild (eKeys);
	eLights.InsertEndChild (eLight);
	eRoot.InsertEndChild (eLights);

	ATOM_TiXmlElement eLightFactors ("LightFactors");
	for (int i = 0; i < ATOM_TimeManager::numTimeKeys; ++i)
	{
		ATOM_TiXmlElement eKey("Key");
		sprintf (buffer, "%f,%f,%f", getGlobalLightColorKey(i).x, getGlobalLightColorKey(i).y, getGlobalLightColorKey(i).z);
		eKey.SetAttribute ("Color", buffer);
		eLightFactors.InsertEndChild (eKey);
	}
	eRoot.InsertEndChild (eLightFactors);

	ATOM_TiXmlElement eAmbient("Ambient");
	ATOM_TiXmlElement eAmbientKeys("Keys");
	for (int i = 0; i < 1; ++i)
	{
		ATOM_TiXmlElement eKey("Key");
		sprintf (buffer, "%f,%f,%f", getAmbientLight().x, getAmbientLight().y, getAmbientLight().z);
		eKey.SetAttribute ("Color", buffer);
		eAmbientKeys.InsertEndChild (eKey);
	}
	eAmbient.InsertEndChild (eAmbientKeys);
	eRoot.InsertEndChild (eAmbient);

	// 路径
	if (_pathes.size () > 0)
	{
		ATOM_TiXmlElement ePathes ("Pathes");
		for (ATOM_HASHMAP<ATOM_STRING, ATOM_Path3D>::const_iterator it = _pathes.begin(); it != _pathes.end(); ++it)
		{
			const ATOM_STRING &name = it->first;
			const ATOM_Path3D &path = it->second;
			ATOM_TiXmlElement ePath("Path");
			ePath.SetAttribute ("Name", name.c_str());
			ePath.SetAttribute ("Smoothness", path.getSmoothness());
			ePath.SetDoubleAttribute ("Radius", path.getRoundRadius());

			char buffer[512];
			const ATOM_Matrix4x4f &mWorld = path.getWorldMatrix();
			sprintf (buffer,"%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f"
				,mWorld.m00,mWorld.m01,mWorld.m02,mWorld.m03
				,mWorld.m10,mWorld.m11,mWorld.m12,mWorld.m13
				,mWorld.m20,mWorld.m21,mWorld.m22,mWorld.m23
				,mWorld.m30,mWorld.m31,mWorld.m32,mWorld.m33);
			ePath.SetAttribute ("WorldMatrix", buffer);

			for (int ctlpoint = 0; ctlpoint < path.getNumControlPoints(); ++ctlpoint)
			{
				const ATOM_Matrix4x4f &matrix = path.getControlPoint(ctlpoint);
				sprintf (buffer,"%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f"
					,matrix.m00,matrix.m01,matrix.m02,matrix.m03
					,matrix.m10,matrix.m11,matrix.m12,matrix.m13
					,matrix.m20,matrix.m21,matrix.m22,matrix.m23
					,matrix.m30,matrix.m31,matrix.m32,matrix.m33);
				ATOM_TiXmlElement ePoint("Point");
				ePoint.SetAttribute ("Matrix", buffer);
				ePath.InsertEndChild (ePoint);
			}
			ePathes.InsertEndChild (ePath);
		}
		eRoot.InsertEndChild (ePathes);
	}

	ATOM_TiXmlElement eHDR ("HDR");
	eHDR.SetDoubleAttribute ("BloomThreshold", _bloomThreshold);
	eHDR.SetDoubleAttribute ("BloomOffset", _bloomOffset);
	//--- wangjian added ---//
	eHDR.SetDoubleAttribute ("BloomMultiplier", _bloomMultiplier);
	// HDR相关
	eHDR.SetAttribute("HDREnable",_hdr_enable?1:0);
	eHDR.SetDoubleAttribute("FilmCurveShoulder", _filmCurveShoulder);
	eHDR.SetDoubleAttribute("FilmCurveMiddleTone", _filmCurveMiddleTone);
	eHDR.SetDoubleAttribute("FilmCurveToe", _filmCurveToe);
	eHDR.SetDoubleAttribute("FilmCurveWhitePoint", _filmCurveWhitePoint);
	eHDR.SetDoubleAttribute("HDRSaturate", _HDRSaturate);
	eHDR.SetDoubleAttribute("HDRContrast", _HDRContrast);
	eHDR.SetDoubleAttribute("HDRColorBalance_Red", _HDRColorBalance.x);
	eHDR.SetDoubleAttribute("HDRColorBalance_Green", _HDRColorBalance.y);
	eHDR.SetDoubleAttribute("HDRColorBalance_Blue", _HDRColorBalance.z);
	//----------------------//
	eRoot.InsertEndChild (eHDR);

	// wangjian added
	ATOM_TiXmlElement eComplementLight("ComplementLight");
	sprintf (buffer, "%f,%f,%f", _complement_lightDir.x, _complement_lightDir.y, _complement_lightDir.z);
	eComplementLight.SetAttribute ("LightDir", buffer);
	sprintf (buffer, "%f,%f,%f", _complement_lightColor.x, _complement_lightColor.y, _complement_lightColor.z);
	eComplementLight.SetAttribute ("LightColor", buffer);
	eComplementLight.SetDoubleAttribute("LightIntensity",_complement_lightIntensity);
	eRoot.InsertEndChild(eComplementLight);
	//---------------------------------------------//

	ATOM_AUTOREF(ATOM_Node) toBeSaved = parent ? parent : getRootNode ();

	if (toBeSaved)
	{
		ATOM_TiXmlElement eNodesDefault("Nodes");

		for (unsigned i = 0; i < toBeSaved->getNumChildren(); ++i)
		{
			ATOM_Node *node = toBeSaved->getChild (i);
			if (node)
			{
				const ATOM_STRING &layer = node->getLayer ();
				if (layer == "")
				{
					////--- wangjian modified for test ---//
					//// 异步加载 ： 块状结构保存关卡节点
					//if( ATOM_AsyncLoader::IsRun() )
					//{
					//	ATOM_XmlBlockSaveVisitor saveVisitor (&eNodesDefault,this);
					//	saveVisitor.setFilter (filterFunction, filterUserData);
					//	saveVisitor.traverse (*node);
					//	if (!saveVisitor.savedOK ())
					//	{
					//		return false;
					//	}
					//}
					//else
					{
						ATOM_XmlSaveVisitor saveVisitor (&eNodesDefault);
						saveVisitor.setFilter (filterFunction, filterUserData);
						saveVisitor.traverse (*node);
						if (!saveVisitor.savedOK ())
						{
							return false;
						}
					}
					//----------------------------------//
				}
				else
				{
					ATOM_TiXmlElement nodesLayer("Nodes");

					ATOM_TiXmlElement *nodes = eRoot.FirstChildElement ("Nodes");
					while (nodes)
					{
						const char *layername = nodes->Attribute ("Layer");
						if (layername && !_stricmp(layername, layer.c_str()))
						{
							break;
						}
						nodes = nodes->NextSiblingElement ("Nodes");
					}

					if (!nodes)
					{
						nodes = &nodesLayer;
						nodes->SetAttribute ("Layer", layer.c_str());
					}

					//--- wangjian modified for test ---//
					// 异步加载 ： 块状结构保存关卡节点
					/*if( ATOM_AsyncLoader::IsRun() )
					{
						ATOM_XmlBlockSaveVisitor saveVisitor (nodes,this);
						saveVisitor.setFilter (filterFunction, filterUserData);
						saveVisitor.traverse (*node);
						if (!saveVisitor.savedOK ())
						{
							return false;
						}
					}
					else*/
					{
						ATOM_XmlSaveVisitor saveVisitor (nodes);
						saveVisitor.setFilter (filterFunction, filterUserData);
						saveVisitor.traverse (*node);
						if (!saveVisitor.savedOK ())
						{
							return false;
						}
					}
				

					eRoot.InsertEndChild (nodesLayer);
				}
			}
		}

		if (eNodesDefault.FirstChildElement ())
		{		
			//--- wangjian added : 将总的节点数量写入"Nodes"的属性 ---//
			// 异步加载：块状结构相关
			unsigned total = GetNumNodes (&eNodesDefault);
			eNodesDefault.SetAttribute("node_total_count",(int)total);
			//--------------------------------------------------------//
		
			eRoot.InsertEndChild (eNodesDefault);
		}
	}

	doc.InsertEndChild (eRoot);

	return doc.SaveFile ();
}

void ATOM_SDLScene::setGlobalLightColorKey (int time, const ATOM_Vector4f &color)
{
	_lightFactors.setKeyValue (time, color);
}

const ATOM_Vector4f &ATOM_SDLScene::getGlobalLightColorKey (int time) const
{
	return _lightFactors.getKeyValue (time);
}

void ATOM_SDLScene::setSceneScale (float value)
{
	_sceneScale = value;
}

float ATOM_SDLScene::getSceneScale (void) const
{
	return _sceneScale;
}

void ATOM_SDLScene::setLightSize (float value)
{
	_lightSize = value;
}

float ATOM_SDLScene::getLightSize (void) const
{
	return _lightSize;
}

void ATOM_SDLScene::setDepthBias (float value)
{
	_depthBias = value;
}

float ATOM_SDLScene::getDepthBias (void) const
{
	return _depthBias;
}

void ATOM_SDLScene::setShadowFadeMin (float value)
{
	_fadeMin = value;
}

float ATOM_SDLScene::getShadowFadeMin (void) const
{
	return _fadeMin;
}

void ATOM_SDLScene::setShadowFadeMax (float value)
{
	_fadeMax = value;
}

float ATOM_SDLScene::getShadowFadeMax (void) const
{
	return _fadeMax;
}

//--- wangjian added ---//
// 阴影相关
void ATOM_SDLScene::setShadowBlur (float value)
{
	_shadowBlur = value;
}
float ATOM_SDLScene::getShadowBlur (void) const
{
	return _shadowBlur;
}
void ATOM_SDLScene::setShadowSlopeBias (float value)
{
	_shadowSlopeBias = value;
}
float ATOM_SDLScene::getShadowSlopeBias (void) const
{
	return _shadowSlopeBias;
}
void ATOM_SDLScene::setShadowDensity(float value)
{
	_shadowDensity = value;
}
float ATOM_SDLScene::getShadowDensity(void) const
{
	return _shadowDensity;
}
void ATOM_SDLScene::setComplememtLightParams(	const ATOM_Vector3f	&	lightDir,
												const ATOM_Vector4f	&	lightColor,
												const float				lightIntensity	)
{
	_complement_lightDir		= lightDir;
	_complement_lightColor		= lightColor;
	_complement_lightIntensity	= lightIntensity;
}
void ATOM_SDLScene::getComplementLightParams(	ATOM_Vector3f	&	lightDir,
												ATOM_Vector4f	&	lightColor,
												float			&	lightIntensity	) const
{
	lightDir		= _complement_lightDir;
	lightColor		= _complement_lightColor;
	lightIntensity	= _complement_lightIntensity;
}
//---------------------//

void ATOM_SDLScene::setSplitLambda (float value)
{
	_splitLambda = value;
}

float ATOM_SDLScene::getSplitLambda (void) const
{
	return _splitLambda;
}

void ATOM_SDLScene::setShadowDistance (float value)
{
	_shadowDistance = value;
}

float ATOM_SDLScene::getShadowDistance (void) const
{
	return _shadowDistance;
}

void ATOM_SDLScene::setLightDistance (float value)
{
	_lightDistance = value;
}

float ATOM_SDLScene::getLightDistance (void) const
{
	return _lightDistance;
}

ATOM_BBox ATOM_SDLScene::getPrecalculatedBoundingBox (void) const
{
	return _sceneBBoxCalculated ? _sceneBoundingBox : calculateBoundingBox ();
}

ATOM_BBox ATOM_SDLScene::calculateBoundingBox (void) const
{
	_sceneBBoxCalculated = true;
	_sceneBoundingBox = calculateSceneBoundingBox ();

	return _sceneBoundingBox;
}

void ATOM_SDLScene::setFixedCameraVector (const ATOM_Vector3f &v)
{
	_fixedCameraDirection = v;
}

void ATOM_SDLScene::setFixedCameraPosition (const ATOM_Vector3f &p)
{
	_fixedCameraPosition = p;
}

void ATOM_SDLScene::setPerspectiveParams (float fov, float nearPlane, float farPlane)
{
	_FOV = fov;
	_nearPlane = nearPlane;
	_farPlane = farPlane;
}

const ATOM_Vector3f &ATOM_SDLScene::getFixedCameraVector (void) const
{
	return _fixedCameraDirection;
}

const ATOM_Vector3f &ATOM_SDLScene::getFixedCameraPosition (void) const
{
	return _fixedCameraPosition;
}

float ATOM_SDLScene::getFOVY (void) const
{
	return _FOV;
}

float ATOM_SDLScene::getNearPlane (void) const
{
	return _nearPlane;
}

float ATOM_SDLScene::getFarPlane (void) const
{
	return _farPlane;
}

void ATOM_SDLScene::setCameraMatrix (const ATOM_Matrix4x4f &m)
{
	if (!_cameraMatrix)
	{
		_cameraMatrix = ATOM_NEW(ATOM_Matrix4x4f);
	}

	*_cameraMatrix = m;
}

const ATOM_Matrix4x4f *ATOM_SDLScene::getCameraMatrix (void) const
{
	return _cameraMatrix;
}

//--- wangjian added ---//
// 异步加载
bool ATOM_SDLScene::IsAsyncLoad()
{
	return bAsyncLoad;
}
void ATOM_SDLScene::SetLoadAsyncCallBack( SceneAsyncLoadCallback callback,void* userdata )
{
	_sceneAsyncLoadCallBackInfo.callback = callback;
	_sceneAsyncLoadCallBackInfo.userdata = userdata;
}
//---------------------//
//--- use for test node count ---//
ATOM_MAP<ATOM_STRING,ATOM_SDLScene::sCategory>	ATOM_SDLScene::g_SceneNodesCategory;

void ATOM_SDLScene::appendNode(ATOM_Node* node)
{
	if(node)
	{
		if( node->getClassName() && node->getClassName()[0] != '\0' )
		{
			ATOM_MAP<ATOM_STRING,sCategory>::iterator iter = g_SceneNodesCategory.find(node->getClassName());
			if( iter != g_SceneNodesCategory.end() )
			{
				sCategory & cate = iter->second;
				cate.append(node);
			}
			else
			{
				sCategory cate;
				cate.append(node);
				g_SceneNodesCategory.insert(std::make_pair(node->getClassName(),cate));
			}
		}
	}
}
void ATOM_SDLScene::collectSceneNodeStatistics(ATOM_Scene * scene)
{
	class SceneNodeStatisticsCollectVisitor: public ATOM_Visitor
	{
	public:
		virtual void visit (ATOM_Node &node)
		{
			appendNode(&node);
		}
	};
	SceneNodeStatisticsCollectVisitor v;

	if( scene )
		v.traverse (*scene->getRootNode());
}
void ATOM_SDLScene::dumpSceneNodeStatistics()
{
	ATOM_LOGGER::log("++++++++++++++++++++++ the scene node statistics START +++++++++++++++++++++\n");
	ATOM_MAP<ATOM_STRING,sCategory>::iterator iter = g_SceneNodesCategory.begin();
	unsigned int index = 0;
	for(;iter != g_SceneNodesCategory.end();++iter)
	{
		sCategory & cate = iter->second;

		ATOM_LOGGER::log("---%u --> %s \n",index,iter->first.c_str() );

		cate.dump();

		index++;
	}
	ATOM_LOGGER::log("++++++++++++++++++++++ the scene node statistics END +++++++++++++++++++++\n");

	g_SceneNodesCategory.clear();
}
void ATOM_SDLScene::drawSceneNode(ATOM_Scene * scene, ATOM_STRING nodeFileName/* = ""*/, bool draw /*=true*/)
{
	class SceneNodeDrawVisitor: public ATOM_Visitor
	{
	public:
		SceneNodeDrawVisitor(ATOM_STRING nfn,bool draw):_nfn(nfn),_draw(draw){}
		virtual void visit (ATOM_Node &node)
		{
			if( node.getNodeFileName().empty() )
				return;

			if( node.getNodeFileName() == _nfn )
				node.setShow(_draw);
		}

		ATOM_STRING _nfn;
		bool _draw;
	};
	SceneNodeDrawVisitor v(nodeFileName,draw);
	if( scene )
		v.traverse (*scene->getRootNode());
}
//-------------------------------//