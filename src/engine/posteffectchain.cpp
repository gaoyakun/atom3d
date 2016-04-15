#include "StdAfx.h"
#include "posteffectchain.h"
#include "postEffectGammaCorrect.h"

ATOM_SCRIPT_INTERFACE_BEGIN(ATOM_PostEffect)
	ATOM_ATTRIBUTES_BEGIN(ATOM_PostEffect)
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_PostEffect, "Enabled", isEnabled, enable, 1, "group=ATOM_PostEffect")
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_PostEffect, "SaveAllRTs", isSaveAllRTs, saveAllRTs, 0, "group=ATOM_PostEffect;desc='保存RT'")
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_PostEffect, "DebugDraw", isDebugDraw, debugDraw, 0, "group=ATOM_PostEffect;desc='调试绘制'")
	ATOM_ATTRIBUTES_END()
ATOM_SCRIPT_INTERFACE_ROOT_END(ATOM_PostEffect)

ATOM_PostEffect::ATOM_PostEffect (void)
{
	_prev = 0;
	_next = 0;
	_chain = 0;

	//--- wangjian added ---//
	_enabled = true;
	_saveAllRTs = false;
	_debugDraw = false;

	_destroyed = false;
	_vp_ratioWidth = 1.0f;
	_vp_ratioHeight = 1.0f;

	_gammaFlag = AFTER_GAMMA_CORRECT;
	//----------------------//
}

ATOM_PostEffect::~ATOM_PostEffect (void)
{
	if (_chain)
	{
		_chain->removePostEffect (this);
	}
}

void ATOM_PostEffect::setRenderTarget (ATOM_Texture *texture)
{
	_target = texture;
}

ATOM_PostEffectChain *ATOM_PostEffect::getChain (void) const
{
	return _chain;
}

ATOM_PostEffect *ATOM_PostEffect::getPreviousEffect (void) const
{
	return _prev.get();
}

ATOM_PostEffect *ATOM_PostEffect::getNextEffect (void) const
{
	return _next.get();
}

ATOM_Texture *ATOM_PostEffect::getRenderTarget (void) const
{
	return _target.get();
}

void ATOM_PostEffect::enable (int b)
{
	_enabled = (b != 0);
}
int ATOM_PostEffect::isEnabled (void) const
{
	return _enabled ? 1 : 0;
}
void ATOM_PostEffect::saveAllRTs(int b)
{
	_saveAllRTs = ( b != 0 );
}
int ATOM_PostEffect::isSaveAllRTs(void) const
{
	return _saveAllRTs;
}
void ATOM_PostEffect::debugDraw(int b)
{
	_debugDraw = ( b != 0 );
}
int ATOM_PostEffect::isDebugDraw(void) const
{
	return _debugDraw;
}

void ATOM_PostEffect::drawTexturedFullscreenQuad (	ATOM_RenderDevice *device, ATOM_Material *material, unsigned w, unsigned h,
													float u0/* = 0.0f*/, float v0/* = 0.0f*/, float u1/* = 1.0f*/, float v1/* = 1.0f*/	)
{
	device->pushMatrix (ATOM_MATRIXMODE_WORLD);
	device->pushMatrix (ATOM_MATRIXMODE_VIEW);
	device->pushMatrix (ATOM_MATRIXMODE_PROJECTION);
	device->loadIdentity (ATOM_MATRIXMODE_WORLD);
	device->loadIdentity (ATOM_MATRIXMODE_VIEW);
	device->loadIdentity (ATOM_MATRIXMODE_PROJECTION);

	float deltax = (float)1.f/(float)w;
	float deltay = (float)1.f/(float)h;

	float vertices[4 * 5] = {
		-1.f - deltax, -1.f + deltay, 0.f, u0, v1,
		1.f - deltax, -1.f + deltay, 0.f, u1, v1,
		1.f - deltax,  1.f + deltay, 0.f, u1, v0,
		-1.f - deltax,  1.f + deltay, 0.f, u0, v0
	};

	unsigned short indices[4] = {
		0, 1, 2, 3
	};

	unsigned num = material ? material->begin (device) : 0;
	for (unsigned pass = 0; pass < num; ++pass)
	{
		if (material->beginPass(device, pass))
		{
			device->renderInterleavedMemoryStreamsIndexed (ATOM_PRIMITIVE_TRIANGLE_FAN, 4, 4 * 5, ATOM_VERTEX_ATTRIB_COORD|ATOM_VERTEX_ATTRIB_TEX1_2, 5 * sizeof(float), vertices, indices);
			material->endPass (device, pass);
		}
	}
	if (material)
	{
		material->end (device);
	}

	device->popMatrix (ATOM_MATRIXMODE_WORLD);
	device->popMatrix (ATOM_MATRIXMODE_VIEW);
	device->popMatrix (ATOM_MATRIXMODE_PROJECTION);
}

//--- wangjian added ---//
// 获得前一个可用的PE
ATOM_PostEffect * ATOM_PostEffect::getPrevEnabledEffect(void) const
{
	ATOM_PostEffect * prev_enabled = _prev.get();

	while( prev_enabled && !prev_enabled->isEnabled() )
	{
		prev_enabled = prev_enabled->getPreviousEffect();
	}
	
	return prev_enabled;
}
// 获得输入的纹理
ATOM_AUTOREF(ATOM_Texture) ATOM_PostEffect::getSourceInputTex() const
{
	ATOM_PostEffect *prevEffect = getPrevEnabledEffect ();
	ATOM_Texture *inputTexture = prevEffect ? prevEffect->getRenderTarget() : _chain->getInputTexture();
	return inputTexture;
}

bool ATOM_PostEffect::isGamma()const
{
	return _gammaFlag == GAMMA_CORRECT ;
}
bool ATOM_PostEffect::isBeforeGamma(void) const
{
	return _gammaFlag == BEFORE_GAMMA_CORRECT ;
}
bool ATOM_PostEffect::isAfterGamma(void) const
{
	return _gammaFlag == AFTER_GAMMA_CORRECT ;
}

bool ATOM_PostEffect::init(ATOM_RenderDevice *device)
{
	// 获得当前VP的大小
	float viewportWidth		= (float)ATOM_RenderScheme::getCurrentRenderScheme()->getWidth();
	float viewportHeight	= (float)ATOM_RenderScheme::getCurrentRenderScheme()->getHeight();
	// 获得输入纹理的大小
	float texWidth			= (float)getSourceInputTex()->getWidth();
	float texHeight			= (float)getSourceInputTex()->getHeight();
	// 计算VP大小和纹理大小的比率作为纹理坐标的缩放值
	_vp_ratioWidth			= viewportWidth / texWidth;
	_vp_ratioHeight			= viewportHeight / texHeight;

	_destroyed = false;

	return true;
}
bool ATOM_PostEffect::destroy()
{
	_destroyed = true;

	return true;
}
bool ATOM_PostEffect::isDestroyed() const
{
	return _destroyed;
}

int ATOM_PostEffect::getEffectOrderID() const								// 获得排序ID
{
	return ATOM_PostEffectChain::getEffectID(getClassName());
}


//----------------------//

////////////////////////////////////////////////////////////////////////////////////////
//=============================//
// wangjian added
// rendertarget管理器

ATOM_AUTOREF(ATOM_Texture) ATOM_PostEffectRTMgr::_rtPingPong[2] = {0};				// 用于单个POSTEFFECT的输入RT和输出RT

ATOM_PostEffectRTMgr::RTMAP ATOM_PostEffectRTMgr::_rtSet;			// RT集合

// 初始化所有一开始就需要的RT
void ATOM_PostEffectRTMgr::init()
{

}

// 重置所有RT（清除RT集合）
void ATOM_PostEffectRTMgr::reset()
{
	if( !_rtSet.empty() )
		_rtSet.clear();
}

// 创建需要的RT，保存到集合
ATOM_AUTOREF(ATOM_Texture) ATOM_PostEffectRTMgr::getRT(unsigned width, unsigned height, ATOM_PixelFormat format, int id)
{
	DWORD64 rt_id = getRTID(width,height,format);
	std::pair<RTITER, RTITER> range = _rtSet.equal_range (rt_id);

	RTITER iter = range.first;
	while(iter!=range.second)
	{
		sRT & rt = iter->second;
		if( rt._id == id )
			return rt._rt;

		iter++;
	}

	sRT rt;
	rt._id = id;
	rt._rt = ATOM_GetRenderDevice()->allocTexture (	0, 0, 
													width, height, 
													format, 
													ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET	);
	_rtSet.insert(std::make_pair(rt_id,rt));

	return rt._rt;
}

DWORD64 ATOM_PostEffectRTMgr::getRTID(unsigned width, unsigned height, ATOM_PixelFormat format)
{
	return ((DWORD64)width << 32) | (DWORD64)height << 16 | (DWORD64)format;
}
//=============================//

// wangjian added
ATOM_MAP<ATOM_STRING,int> ATOM_PostEffectChain::_postEffectOrderMap;
#define ORDER_PE(pename,id) \
	_postEffectOrderMap.insert( std::make_pair(pename,id++) );

////////////////////////////////////////////////////////////////////////////////////////
ATOM_PostEffectChain::ATOM_PostEffectChain (void)
{
	_firstEffect = 0;
	_numEffects = 0;

	

	//-------wangjian : 初始化顺序 ----//
	int id = 0;
	//ORDER_PE("ATOM_GammaCorrectEffect",id);
	
	ORDER_PE("ATOM_DepthMaskEffect",id);
	ORDER_PE("ATOM_FXAAEffect",id);
	ORDER_PE("ATOM_SSAOEffect",id);
	ORDER_PE("ATOM_RainEffect",id)
	ORDER_PE("ATOM_RainLayerEffect",id)
	ORDER_PE("ATOM_SunShaftEffect",id)
	ORDER_PE("ATOM_DOFEffect",id)
	ORDER_PE("ATOM_BLOOMEffect",id)
	//ORDER_PE("ATOM_GammaCorrectEffect",id)
	ORDER_PE("ATOM_HeatHazeEffect",id)
	ORDER_PE("ATOM_ColorFilterEffect",id);

	// 首先将GammaCorrectEffect添加到链表中
	//_gammaCorrectEffect = ATOM_CreateObject ("ATOM_GammaCorrectEffect", 0);
	//if( _gammaCorrectEffect )
	//	appendPostEffect(_gammaCorrectEffect.get());
	//----------------------//
}

ATOM_PostEffectChain::~ATOM_PostEffectChain (void)
{
	clear ();
}

bool ATOM_PostEffectChain::appendPostEffect (ATOM_PostEffect *postEffect)
{
	if (!postEffect || postEffect->_next || postEffect->_prev || postEffect->_chain)
	{
		return false;
	}

	//-------------- wangjian modified ------------------//
#if 0

	if (_firstEffect)
	{
		bool bInsertBefore = false;
		ATOM_AUTOREF(ATOM_PostEffect) p = _firstEffect.get();
		for (;;)
		{
			if (postEffect == p.get())
			{
				return false;
			}

			//--- wangjian added ---//
			// 如果p是在Gamma之后，而postEffect在Gamma之前 应该将其插在p之前
			if( ( !p->isBeforeGamma() ) && ( !postEffect->isAfterGamma() ) )
			{
				bInsertBefore = true;
				break;
			}
			//----------------------//

			if (!p->_next)
			{
				break;
			}

			p = p->_next.get();
		}

		//--- wangjian modified ---//
		// 插在前头
		if( bInsertBefore )
		{
			if( p->_prev )
			{
				p->_prev->_next = postEffect;
				postEffect->_prev = p->_prev;
			}
			else
			{
				ATOM_AUTOREF(ATOM_PostEffect) first = _firstEffect;
				_firstEffect = postEffect;
			}

			postEffect->_next = p;
			p->_prev = postEffect;
		}
		// 插在后头
		else
		{
			p->_next = postEffect;
			postEffect->_prev = p;
		}
		//-------------------------//
	}
	else
	{
		_firstEffect = postEffect;
	}

#else

	if (_firstEffect)
	{
		bool bInsertBefore = false;
		ATOM_AUTOREF(ATOM_PostEffect) p = _firstEffect.get();
		for (;;)
		{
			if (postEffect == p.get())
			{
				return false;
			}

			//--- wangjian added ---//
			// 如果p的ID是在postEffect的ID之后,应该将postEffect插在p之前。
			int id_exist = p->getEffectOrderID();
			int id_new = postEffect->getEffectOrderID();
			ATOM_ASSERT( id_exist!= -1 && id_new != -1);
			if( id_exist > id_new )
			{
				bInsertBefore = true;
				break;
			}
			//----------------------//

			if (!p->_next)
			{
				break;
			}

			p = p->_next.get();
		}

		//--- wangjian modified ---//
		// 插在前头
		if( bInsertBefore )
		{
			if( p->_prev )
			{
				p->_prev->_next = postEffect;
				postEffect->_prev = p->_prev;
			}
			else
			{
				ATOM_AUTOREF(ATOM_PostEffect) first = _firstEffect;
				_firstEffect = postEffect;
			}

			postEffect->_next = p;
			p->_prev = postEffect;
		}
		// 插在后头
		else
		{
			p->_next = postEffect;
			postEffect->_prev = p;
		}
		//-------------------------//
	}
	else
	{
		_firstEffect = postEffect;
	}

#endif

	//==========================================//

	postEffect->_chain = this;
	++_numEffects;
	return true;
}

ATOM_PostEffect *ATOM_PostEffectChain::getEffect (unsigned index) const
{
	ATOM_PostEffect *effect = _firstEffect.get();
	while (effect && index > 0)
	{
		effect = effect->getNextEffect ();
		index--;
	}
	return effect;
}

void ATOM_PostEffectChain::removePostEffect (ATOM_PostEffect *postEffect)
{
	ATOM_AUTOREF(ATOM_PostEffect) refHolder = postEffect;

	if (postEffect && postEffect->_chain == this)
	{
		postEffect->_chain = 0;

		if (postEffect == _firstEffect)
		{
			_firstEffect = postEffect->_next;

			if (_firstEffect)
			{
				_firstEffect->_prev = 0;
			}
		}
		else
		{
			postEffect->_prev->_next = postEffect->_next;
			if (postEffect->_next)
			{
				postEffect->_next->_prev = postEffect->_prev;
			}
		}

		postEffect->_prev = 0;
		postEffect->_next = 0;
		--_numEffects;
	}
}

ATOM_PostEffect *ATOM_PostEffectChain::getFirstEffect (void) const
{
	return _firstEffect.get();
}

unsigned ATOM_PostEffectChain::getNumPostEffects (void) const
{
	return _numEffects;
}

void ATOM_PostEffectChain::clear (void)
{
	while (_firstEffect)
	{
		removePostEffect (_firstEffect.get());
	}
}

void ATOM_PostEffectChain::setInputTexture (ATOM_Texture *texture)
{
	_texturePingPong[0] = texture;
}

void ATOM_PostEffectChain::setOutputTexture (ATOM_Texture *texture)
{
	_texturePingPong[1] = texture;
}

// wangjian modified
ATOM_Texture *ATOM_PostEffectChain::render (ATOM_RenderDevice *device)
{
	ATOM_PostEffect *effect = _firstEffect.get();
	_inputTextureIndex = 0;
	int outputTextureIndex = _inputTextureIndex;

	//--- wangjian modified ---//
	while (effect)
	{
		// 如果该PE开启了，渲染
		if( effect->isEnabled() )
		{
			outputTextureIndex = 1 - _inputTextureIndex;

			effect->setRenderTarget (_texturePingPong[outputTextureIndex].get());

			if (effect->render (device))
			{
				_inputTextureIndex = 1 - _inputTextureIndex;
				effect = effect->_next.get();
			}
			else
			{
				break;
			}
		}
		// 如果禁用了
		else
		{
			// 如果还未销毁 销毁资源
			//if( !effect->isDestroyed() )
			//	effect->destroy();

			effect = effect->_next.get();
		}
	}
	//-------------------------//

	return _texturePingPong[outputTextureIndex].get();
}

ATOM_Texture *ATOM_PostEffectChain::getInputTexture (void) const
{
	return _texturePingPong[_inputTextureIndex].get();
}

//--- wangjian added ---//
void  ATOM_PostEffectChain::setHDREnabled(bool b)
{
	// 如果开启HDR
	if( b && _gammaCorrectEffect.get() )
	{
		removePostEffect( _gammaCorrectEffect.get() );
		_gammaCorrectEffect = 0;
	}
	if( !b && !_gammaCorrectEffect.get() )
	{
		//_gammaCorrectEffect = ATOM_CreateObject ("ATOM_GammaCorrectEffect", 0);
		//if( _gammaCorrectEffect )
		//	appendPostEffect(_gammaCorrectEffect.get());
	}
}

ATOM_PostEffect * ATOM_PostEffectChain::getEffect( const char* name ) const
{
	ATOM_PostEffect *effect = _firstEffect.get();
	while (effect)
	{
		if( stricmp( effect->getClassName(), name ) == 0 )
			return effect;

		effect = effect->getNextEffect ();
	}
	return 0;
}
ATOM_PostEffect * ATOM_PostEffectChain::enablePostEffect( const char* postEffectName, int enableOrDisable )
{
	ATOM_PostEffect * effect = getEffect(postEffectName);
	if( enableOrDisable != 0 && !effect )
	{
		ATOM_AUTOREF(ATOM_PostEffect) postEffect = ATOM_CreateObject (postEffectName, 0);
		if (postEffect)
		{
			appendPostEffect (postEffect.get());
			effect = postEffect.get();
		}
	}
	if( effect )
		effect->enable(enableOrDisable);

	return effect;
}
void ATOM_PostEffectChain::destroyPostEffect( const char* postEffectName )
{
	ATOM_PostEffect * effect = getEffect(postEffectName);
	if( effect )
		removePostEffect(effect);
}
void ATOM_PostEffectChain::setParam( const char* postEffectName, const char* paramName, float val )
{
	ATOM_PostEffect * effect = enablePostEffect(postEffectName,true);
	if( effect )
	{
		ATOM_ScriptVar v( val );
		ATOM_SetAttribValue(effect,paramName,v);
	}
}
int ATOM_PostEffectChain::getEffectID(const char* name) 
{
	if( !name || name[0] == '\0')
		return -1;

	ATOM_MAP<ATOM_STRING,int>::const_iterator iter = _postEffectOrderMap.find(name);
	if( iter != _postEffectOrderMap.end() )
		return iter->second;

	return -1;
}

void ATOM_PostEffectChain::beginScissorTest(ATOM_RenderDevice *device, const ATOM_Rect2Di & rect)
{
	_savedScissorTest = device->isScissorTestEnabled (0);
	device->getScissorRect (0, &_savedScissorRect[0], &_savedScissorRect[1], &_savedScissorRect[2], &_savedScissorRect[3]);
	device->enableScissorTest (0, true);
	device->setScissorRect (0, rect);
}
void ATOM_PostEffectChain::beginScissorTest (ATOM_RenderDevice *device, int scissorX, int scissorY, int scissorW, int scissorH)
{
	_savedScissorTest = device->isScissorTestEnabled (0);
	device->getScissorRect (0, &_savedScissorRect[0], &_savedScissorRect[1], &_savedScissorRect[2], &_savedScissorRect[3]);
	device->enableScissorTest (0, true);
	device->setScissorRect (0, scissorX, scissorY, scissorW, scissorH);
}

void ATOM_PostEffectChain::endScissorTest (ATOM_RenderDevice *device)
{
	device->enableScissorTest (0, _savedScissorTest);
	device->setScissorRect (0, _savedScissorRect[0], _savedScissorRect[1], _savedScissorRect[2], _savedScissorRect[3]);
}
//----------------------//

