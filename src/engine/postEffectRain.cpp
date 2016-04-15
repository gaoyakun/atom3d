#include "StdAfx.h"
#include "postEffectRain.h"


///////////////////////////////////////////////////////////////////////
ATOM_SCRIPT_INTERFACE_BEGIN(ATOM_RainEffect)
	ATOM_ATTRIBUTES_BEGIN(ATOM_RainEffect)
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_RainEffect, "RainFallTexture", getRainFallTextureFileName, setRainFallTextureFileName, "/textures/rainfall.dds", "group=ATOM_RainEffect;type=vfilename;desc='雨滴贴图'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_RainEffect, "DropSpeed", getDropSpeed, setDropSpeed, ATOM_Vector4f(1.0f), "group=ATOM_RainEffect;desc='雨(雪)速'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_RainEffect, "Highlight", getHighLight, setHighLight, ATOM_Vector4f(1.0f), "group=ATOM_RainEffect;desc='颜色强度'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_RainEffect, "ConeSize", getConeSize, setConeSize, ATOM_Vector4f(5.0f), "group=ATOM_RainEffect;desc='范围大小'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_RainEffect, "TexTiling01", getTexTilingFactor01, setTexTilingFactor01, ATOM_Vector4f(0.15f,0.15f,0.15f,0.15f), "group=ATOM_RainEffect;desc='层平铺[0|1]'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_RainEffect, "TexTiling23", getTexTilingFactor23, setTexTilingFactor23, ATOM_Vector4f(0.15f,0.15f,0.15f,0.15f), "group=ATOM_RainEffect;desc='层平铺[2|3]'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_RainEffect, "WindSpeed", getWindSpeed, setWindSpeed, ATOM_Vector4f(1.0f), "group=ATOM_RainEffect;desc='风速'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_RainEffect, "TopScale", getTopScale, setTopScale, ATOM_Vector4f(0.5f), "group=ATOM_RainEffect;desc='顶缩放因子'")
	ATOM_ATTRIBUTES_END()
ATOM_SCRIPT_INTERFACE_END(ATOM_RainEffect, ATOM_PostEffect)


ATOM_RainEffect::ATOM_RainEffect (void)
{
	_rainRT = 0;
	_qualityLevel = RAIN_QUALITY_HIGH;

	_width_rt = 0;
	_height_rt = 0;
	_recreateRainRT = false;
	_layerCount = 4;
	/////////////////////////////////////////////////////////////////
	_fRainDropsSpeed	= ATOM_Vector4f(1.0f);
	_fHighlight			= ATOM_Vector4f(1.0f,0.75f,0.5f,0.25f);
	_fSizeMult			= ATOM_Vector4f(25.0f,100.0f,225.0f,400.0f);
	_vTexTiling[0]		= ATOM_Vector4f(0.15f,0.15f,0.15f,0.15f);
	_vTexTiling[1]		= ATOM_Vector4f(0.15f,0.15f,0.15f,0.15f);
	_fWindSpeed			= ATOM_Vector4f(1.0f);
	_fTopScale			= ATOM_Vector4f(0.5f);
	/////////////////////////////////////////////////////////////////
	_rainRT = 0;
	_rainFallTex = 0;
	_material = 0;
	_vertexArrayCone = 0;
	_vertexDecl = 0;
	_coneVertexCount = 0;

	_rainFallTexFileName = "/textures/rainfall.dds";

	_gammaFlag = BEFORE_GAMMA_CORRECT;
}

ATOM_RainEffect::~ATOM_RainEffect (void)
{
	destroy();
}

void ATOM_RainEffect::setQualityLevel(int quality)
{
	if( _qualityLevel != quality )
	{
		_qualityLevel = quality;
		_recreateRainRT = true;
	}
}
int ATOM_RainEffect::getQualityLevel()const
{
	return _qualityLevel;
}
//void ATOM_RainEffect::setRainDropParams(/*float fdropspeed,float fhighlight,float fsize,float fMultiplier*/const ATOM_Vector4f & params)
//{
//	_fRainDropsSpeed	= params.x;
//	_fHighlight			= params.y;
//	_fSizeMult			= params.z;
//	_fMultiplier		= params.w;
//}
//const ATOM_Vector4f & ATOM_RainEffect::getRainDropParams(void) const
//{
//	return ATOM_Vector4f( _fRainDropsSpeed,_fHighlight,_fSizeMult,_fMultiplier );
//}

void ATOM_RainEffect::setDropSpeed(const ATOM_Vector4f & dropspeed)
{
	_fRainDropsSpeed	= dropspeed;
}
const ATOM_Vector4f & ATOM_RainEffect::getDropSpeed(void) const
{
	return _fRainDropsSpeed;
}

void ATOM_RainEffect::setHighLight(const ATOM_Vector4f & highlight)
{
	_fHighlight	= highlight;
}
const ATOM_Vector4f & ATOM_RainEffect::getHighLight(void) const
{
	return _fHighlight;
}

void ATOM_RainEffect::setConeSize(const ATOM_Vector4f & conesize)
{
	_fSizeMult	= conesize;
}
const ATOM_Vector4f & ATOM_RainEffect::getConeSize(void) const
{
	return _fSizeMult;
}

void ATOM_RainEffect::setTexTilingFactor01(const ATOM_Vector4f & textiling)
{
	_vTexTiling[0]	= textiling;
}
const ATOM_Vector4f & ATOM_RainEffect::getTexTilingFactor01(void) const
{
	return _vTexTiling[0];
}
void ATOM_RainEffect::setTexTilingFactor23(const ATOM_Vector4f & textiling)
{
	_vTexTiling[1]	= textiling;
}
const ATOM_Vector4f & ATOM_RainEffect::getTexTilingFactor23(void) const
{
	return _vTexTiling[1];
}
void ATOM_RainEffect::setWindSpeed(const ATOM_Vector4f & windSpeed)
{
	_fWindSpeed	= windSpeed;
}
const ATOM_Vector4f & ATOM_RainEffect::getWindSpeed(void) const
{
	return _fWindSpeed;
}
void ATOM_RainEffect::setTopScale(const ATOM_Vector4f & topScale)
{
	_fTopScale	= topScale;
}
const ATOM_Vector4f & ATOM_RainEffect::getTopScale(void) const
{
	return _fTopScale;
}
///////////////////////////////////////////////////////////////////////////////////////

void ATOM_RainEffect::setRainFallTextureFileName (const ATOM_STRING &filename)
{
	ATOM_STACK_TRACE(ATOM_RainEffect::setTextureFileName);

	if ( !filename.empty () && _rainFallTexFileName!=filename )
	{
		_rainFallTexFileName = filename;

		ATOM_RenderDevice *device = ATOM_GetRenderDevice();

		ATOM_AUTOREF(ATOM_Texture) newTexture = ATOM_CreateTextureResource (_rainFallTexFileName.c_str(), 
																			ATOM_PIXEL_FORMAT_UNKNOWN, 
																			1/*ATOM_LoadPriority_IMMEDIATE*/	);

		if (newTexture != _rainFallTex)
		{
			_rainFallTex = newTexture;
			_material = 0;
		}
	}
	else if ( filename.empty () && _rainFallTex)
	{
		_rainFallTex = 0;
		_material = 0;
		_rainFallTexFileName = "";
	}
}

const ATOM_STRING & ATOM_RainEffect::getRainFallTextureFileName (void) const
{
	return _rainFallTexFileName;
}

bool ATOM_RainEffect::createRainRT()
{
	// 如果RT已经存在 且 不需要重建RT
	if( _rainRT && !_recreateRainRT )
		return true;

	if(_recreateRainRT)
		 _rainRT = 0;

	ATOM_GetRenderDevice()->getBackbufferSize(&_width_rt,&_height_rt);
	if(_qualityLevel==RAIN_QUALITY_MEDIUM)
	{
		_width_rt >>=1;
		_height_rt >>=1;
	}
	else if( _qualityLevel==RAIN_QUALITY_LOW)
	{
		_width_rt >>=2;
		_height_rt >>=2;
	}

	if( _width_rt == 0 || _height_rt == 0 )
		return false;

	if (!_rainRT)
	{
		// wangjian modified : 使用ATOM_PostEffectRTMgr创建全局的RenderTarget
#if 0
		_rainRT = ATOM_GetRenderDevice()->allocTexture (0, 0, _width_rt, _height_rt, ATOM_PIXEL_FORMAT_RGBA8888, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET);
#else
		_rainRT = ATOM_PostEffectRTMgr::getRT( _width_rt, _height_rt, ATOM_PIXEL_FORMAT_RGBA8888, 0 );
#endif
	}

	return _rainRT!=0;
}
bool ATOM_RainEffect::createBuffer()
{
	if( _vertexArrayCone && _vertexDecl )
		return true;

#define DEG2RAD( a ) ( (a) * (3.1415926f/180.0f) )

	const int nSlices = 12;
	const float nSliceStep( DEG2RAD( 360.0f / (float) nSlices ) );

	ATOM_VECTOR<SVF_P3F_T2F> pVB;
	SVF_P3F_T2F vVertex;
	
	// Generate cylinder vertices
	for (int h = 0; h < nSlices + 1; ++h)
	{
		vVertex.st.x = h / nSlices;

		vVertex.xyz = ATOM_Vector3f(	cosf( ((float)h) * nSliceStep ),
										-1.0f,
										sinf( ((float)h) * nSliceStep )	);
		vVertex.st.y = 1.0f;
		pVB.push_back(vVertex);

		vVertex.xyz.y = 1.f;
		vVertex.st.y = 0.0f;
		pVB.push_back(vVertex);

		
	}
	// Base of cylinder to cover also lower parts of screen instead of custom RT clearing
	/*vVertex.xyz.y = -1.f;
	pVB.push_back(vVertex);
	pVB.push_back(vVertex);
	int nL = 0;
	int nH = nSlices * 2;
	while (nL < nH)
	{
	nL += 2;
	nH -= 2;
	pVB.push_back(pVB[nL]);
	pVB.push_back(pVB[nH]);
	}
	pVB.push_back(pVB[nL]);*/

	_coneVertexCount = pVB.size();

	if (!_vertexArrayCone)
	{
		_vertexArrayCone = ATOM_GetRenderDevice()->allocVertexArray (ATOM_VERTEX_ATTRIB_COORD|ATOM_VERTEX_ATTRIB_TEX1_2, ATOM_USAGE_STATIC, _coneVertexCount, true);
		SVF_P3F_T2F *v = (SVF_P3F_T2F*)_vertexArrayCone->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
		memcpy( v, &pVB[0], _coneVertexCount * sizeof( SVF_P3F_T2F ) );
		_vertexArrayCone->unlock ();
	}

	if (!_vertexDecl)
	{
		_vertexDecl = ATOM_GetRenderDevice()->createInterleavedVertexDeclaration (ATOM_VERTEX_ATTRIB_COORD|ATOM_VERTEX_ATTRIB_TEX1_2);
	}

	return _vertexArrayCone!=0 && _vertexDecl!=0;
}

bool ATOM_RainEffect::createMaterial()
{
	if (!_material)
	{
		_material = ATOM_MaterialManager::createMaterialFromCore (ATOM_GetRenderDevice(), "/materials/builtin/Rain.mat");
		if( !_material )
			return false;
	}
	return true;
}

bool ATOM_RainEffect::createRainFallTex()
{
	if( !_rainFallTex )
	{
		ATOM_STRING tex = _rainFallTexFileName;
		if( tex.empty() )
			tex = "/textures/rainfall.dds";

		_rainFallTex = ATOM_CreateTextureResource(tex.c_str(),ATOM_PIXEL_FORMAT_UNKNOWN,1/*ATOM_LoadPriority_IMMEDIATE*/);
		if( !_rainFallTex )
			return false;
	}
	return true;
}

bool ATOM_RainEffect::init(ATOM_RenderDevice *device)
{
	if( !ATOM_PostEffect::init(device) )
		return false;

	if( !createMaterial() )
		return false;

	if( !createRainRT() )
		return false;

	if( !createBuffer() )
		return false;

	if( !createRainFallTex() )
		return false;

	return true;
}

bool ATOM_RainEffect::destroy()
{
	_rainRT = 0;
	_rainFallTex = 0;
	_material = 0;
	_vertexArrayCone = 0;

	ATOM_GetRenderDevice()->destroyVertexDecl(_vertexDecl);
	_vertexDecl = 0;

	return ATOM_PostEffect::destroy();
}

void ATOM_RainEffect::enable (int b)
{
	ATOM_PostEffect::enable(b);
	ATOM_RenderSettings::enableRain( b==0 ? false:true );
}
int ATOM_RainEffect::isEnabled (void) const
{
	if(!ATOM_RenderSettings::isRainEnabled())
		return 0;
	return ATOM_PostEffect::isEnabled();
}

bool ATOM_RainEffect::render (ATOM_RenderDevice *device)
{
	// 如果BACKBUFFER的大小改变了 重新生成RT
	int width = _width_rt,height = _height_rt;
	if( ATOM_GetRenderDevice() )
		ATOM_GetRenderDevice()->getBackbufferSize(&_width_rt,&_height_rt);
	if( _width_rt != width || _height_rt != height )
		_recreateRainRT = true;

	// 初始化
	if( !init(device) )
		return false;

	/////////////////////////////////////////////////////////////////////////////////

	_material->setActiveEffect ("rainGen");

	ATOM_AUTOREF(ATOM_Texture) oldRenderTarget = device->getRenderTarget (0);

	device->setRenderTarget (0, _rainRT.get());
	device->setRenderTarget (1, 0);
	device->setViewport (0, ATOM_Rect2Di(0, 0, _rainRT->getWidth(), _rainRT->getHeight()));
	device->setClearColor (NULL, 0);
	device->clear(true,false,false);

	device->setStreamSource (0, _vertexArrayCone.get());
	device->setVertexDecl (_vertexDecl);

	_material->getParameterTable()->setTexture ("rainFallTexture", _rainFallTex.get() );

	ATOM_Vector4f params;
	ATOM_Vector4f params1;
	for( int nCurLayer = 0; nCurLayer < _layerCount; ++nCurLayer )
	{
#if 0
		params.z = float(nCurLayer) / max(_layerCount, 1);					// 
		params.x = _fRainDropsSpeed * (_layerCount - nCurLayer);			// 
		params.y = _fHighlight * (1.f - params.z);
		params.z = nCurLayer+1;
		params.w = (nCurLayer+1) * _fSizeMult;
		params.w = powf(params.w, 2.0f);

		_material->getParameterTable()->setVector("texTilingFactor", _vTexTiling * params.z);
#else
		params.z = float(nCurLayer) / max(_layerCount, 1);					// 
		params.x = _fRainDropsSpeed[nCurLayer];								// 
		params.y = _fHighlight[nCurLayer];
		params.z = nCurLayer+1;
		params.w = _fSizeMult[nCurLayer];
		params.w = powf(params.w, 2.0f);

		ATOM_Vector4f tiling(1);
		unsigned index0 = nCurLayer / 2;
		unsigned index1 = nCurLayer % 2;
		tiling.x = _vTexTiling[index0][index1*2];
		tiling.y = _vTexTiling[index0][index1*2+1];
		_material->getParameterTable()->setVector("texTilingFactor", tiling);

		params1.x = _fWindSpeed[nCurLayer];
		params1.y = _fTopScale[nCurLayer];
#endif

		_material->getParameterTable()->setVector ("sceneRainParams", params);
		_material->getParameterTable()->setVector ("sceneRainParams1", params1);

		/*ATOM_Vector3f worldViewPos(0,0,0);
		ATOM_DeferredScene *currentScene = dynamic_cast<ATOM_DeferredScene*>( ATOM_RenderScheme::getCurrentScene () );
		if( currentScene && currentScene->getCamera() )
			worldViewPos = currentScene->getCamera()->getPosition();
		_material->getParameterTable()->setVector ("eyeWorldPos", worldViewPos);*/

		unsigned numPasses = _material->begin (device);
		for (unsigned pass = 0; pass < numPasses; ++pass)
		{
			if (_material->beginPass (device, pass))
			{
				device->renderStreams ( ATOM_PRIMITIVE_TRIANGLE_STRIP, _coneVertexCount - 2);
				_material->endPass (device, pass);
			}
		}
		_material->end (device);
	}
	device->setRenderTarget (0, oldRenderTarget.get());

	_material->setActiveEffect ("default");

#if 0 
	ATOM_PostEffect *prevEffect = getPreviousEffect ();
	ATOM_Texture *inputTexture = prevEffect ? prevEffect->getRenderTarget() : _chain->getInputTexture();
#else
	ATOM_AUTOREF(ATOM_Texture) inputTexture = getSourceInputTex();
#endif
	if (!inputTexture)
	{
		return false;
	}
	
	device->setRenderTarget (0, getRenderTarget());
	device->setViewport (0, ATOM_Rect2Di(0, 0, getRenderTarget()->getWidth(), getRenderTarget()->getHeight()));

	// 开启SRGB写RT
	device->enableSRGBWrite(true);
	
	_material->getParameterTable()->setTexture ("inputTexture", inputTexture.get());
	_material->getParameterTable()->setTexture ("rainRTTexture", _rainRT.get());

	drawTexturedFullscreenQuad (device, 
								_material.get(), 
								inputTexture->getWidth(), 
								inputTexture->getHeight());

	// 关闭SRGB写RT
	device->enableSRGBWrite(false);

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////