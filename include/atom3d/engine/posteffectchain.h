#ifndef __ATOM3D_ENGINE_POSTEFFECTCHAIN_H
#define __ATOM3D_ENGINE_POSTEFFECTCHAIN_H

#include "../ATOM_kernel.h"
#include "../ATOM_render.h"
#include "basedefs.h"

class ATOM_ENGINE_API ATOM_PostEffectChain;

class ATOM_ENGINE_API ATOM_PostEffect: public ATOM_Object
{
	ATOM_DECLARE_SCRIPT_INTERFACE(ATOM_PostEffect)
	friend class ATOM_PostEffectChain;

public:
	ATOM_PostEffect (void);
	virtual ~ATOM_PostEffect (void);

public:
	virtual void setRenderTarget (ATOM_Texture *texture);
	virtual ATOM_Texture *getRenderTarget (void) const;
	virtual ATOM_PostEffect *getPreviousEffect (void) const;
	virtual ATOM_PostEffect *getNextEffect (void) const;
	virtual ATOM_PostEffectChain *getChain (void) const;
	virtual void enable (int b);
	virtual int isEnabled (void) const;
	virtual void saveAllRTs(int b);
	virtual int isSaveAllRTs(void) const;
	virtual void debugDraw(int b);
	virtual int isDebugDraw(void) const;


	virtual bool render (ATOM_RenderDevice *device) = 0;

	//--- wangjian added ---//
	bool isGamma()const;
	bool isBeforeGamma(void) const;
	bool isAfterGamma(void) const;
	virtual bool init(ATOM_RenderDevice *device);
	virtual bool destroy();
	bool isDestroyed() const;
	ATOM_PostEffect * getPrevEnabledEffect(void) const;
	int getEffectOrderID() const;								// 获得排序ID
	//----------------------//

protected:
	void drawTexturedFullscreenQuad (	ATOM_RenderDevice *device, ATOM_Material *material, unsigned w, unsigned h,
										float u0 = 0.0f, float v0 = 0.0f, float u1 = 1.0f, float v1 = 1.0f	);

	//--- wangjian added ---//
	ATOM_AUTOREF(ATOM_Texture) getSourceInputTex() const;
	//----------------------//

protected:
	ATOM_AUTOREF(ATOM_PostEffect) _prev;
	ATOM_AUTOREF(ATOM_PostEffect) _next;
	ATOM_PostEffectChain *_chain;
	ATOM_AUTOREF(ATOM_Texture) _target;
	bool	_enabled;
	bool    _saveAllRTs;
	bool	_debugDraw;

	//--- wangjian added ---//
	bool	_destroyed;
	float	_vp_ratioWidth;
	float	_vp_ratioHeight;

	enum
	{
		BEFORE_GAMMA_CORRECT = -1,
		GAMMA_CORRECT = 0,
		AFTER_GAMMA_CORRECT = 1,
	};
	int  _gammaFlag;
	//----------------------//
};

class ATOM_GammaCorrectEffect;

//=============================//
// wangjian added
// rendertarget管理器
class ATOM_PostEffectRTMgr
{
public:
	struct sRT
	{
		ATOM_AUTOREF(ATOM_Texture)	_rt;
		unsigned					_id;
	};
	typedef ATOM_MULTIMAP<DWORD64,sRT>	RTMAP;
	typedef RTMAP::iterator									RTITER;
	typedef RTMAP::const_iterator							RTITER_CONST;

	static ATOM_AUTOREF(ATOM_Texture)	_rtPingPong[2];				// 用于单个POSTEFFECT的输入RT和输出RT

	static RTMAP						_rtSet;						// RT集合

	// 初始化所有一开始就需要的RT
	static void init();

	// 重置所有RT（清除RT集合）
	static void reset();

	// 获得某个RT, 如果没有创建需要的RT，保存到集合
	static ATOM_AUTOREF(ATOM_Texture) getRT( unsigned width, unsigned height, ATOM_PixelFormat format, int id );

private:
	static DWORD64 getRTID(unsigned width, unsigned height, ATOM_PixelFormat format);

};
//=============================//

class ATOM_ENGINE_API ATOM_PostEffectChain
{
public:
	ATOM_PostEffectChain (void);
	virtual ~ATOM_PostEffectChain (void);

public:
	virtual bool appendPostEffect (ATOM_PostEffect *postEffect);
	virtual void removePostEffect (ATOM_PostEffect *postEffect);
	virtual ATOM_PostEffect *getFirstEffect (void) const;
	virtual unsigned getNumPostEffects (void) const;
	virtual ATOM_PostEffect *getEffect (unsigned index) const;
	virtual void clear (void);
	virtual void setInputTexture (ATOM_Texture *texture);
	virtual void setOutputTexture (ATOM_Texture *texture);
	virtual ATOM_Texture *getInputTexture (void) const;
	virtual ATOM_Texture * render (ATOM_RenderDevice *device);

	//--- wangjian added ---//
	void  setHDREnabled(bool b);
	ATOM_PostEffect * getEffect( const char* name ) const;
	ATOM_PostEffect * enablePostEffect( const char* postEffectName, int enableOrDisable );
	void destroyPostEffect( const char* postEffectName );
	void setParam( const char* postEffectName, const char* paramName, float val );

	static int getEffectID(const char* name);

	void beginScissorTest(ATOM_RenderDevice *device, const ATOM_Rect2Di & rect);
	void beginScissorTest (ATOM_RenderDevice *device, int scissorX, int scissorY, int scissorW, int scissorH);
	void endScissorTest (ATOM_RenderDevice *device);
	//----------------------//

private:
	unsigned _numEffects;
	ATOM_AUTOREF(ATOM_PostEffect) _firstEffect;
	ATOM_AUTOREF(ATOM_Texture) _texturePingPong[2];
	int _inputTextureIndex;

	//--- wangjian added ---//
	ATOM_AUTOREF(ATOM_GammaCorrectEffect)	_gammaCorrectEffect;
	static ATOM_MAP<ATOM_STRING,int>		_postEffectOrderMap;		// 用于PE排序

	bool _savedScissorTest;
	int _savedScissorRect[4];
	//----------------------//

};

#endif // __ATOM3D_ENGINE_POSTEFFECTCHAIN_H
