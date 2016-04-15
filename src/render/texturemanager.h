#ifndef __ATOM_GLRENDER_ITEXTUREMANAGER_H
#define __ATOM_GLRENDER_ITEXTUREMANAGER_H

#if _MSC_VER > 1000
# pragma once
#endif

class ATOM_Texture;
class Image;
class ATOM_RenderDevice;
class ATOM_TextureManager;

struct ATOM_TextureNode
{
	int allocId;
	int numFrames;
	void *callstack[16];
	ATOM_Texture *texture;
	char filename[256];
	ATOM_TextureNode *prev;
	ATOM_TextureNode *next;
	ATOM_TextureManager *manager;
};

class ATOM_TextureManager
  {
  public:
    ATOM_TextureManager (ATOM_RenderDevice *device);
    virtual ~ATOM_TextureManager();
  public:
    virtual ATOM_AUTOREF(ATOM_Texture) AllocateTexture (unsigned flags, const char* name);
    virtual ATOM_AUTOREF(ATOM_Texture) AllocateTexture (const void* image, int w, int h, ATOM_PixelFormat format, unsigned flags, const char* name);
    virtual ATOM_AUTOREF(ATOM_Texture) AllocateVolumeTexture (const void* image, int w, int h, int d, ATOM_PixelFormat format, unsigned flags, const char* name);
    virtual ATOM_AUTOREF(ATOM_Texture) AllocateCubeTexture (const void* image[6], int size, ATOM_PixelFormat format, unsigned flags, const char* name);
	//--- wangjian added for DX9 DEPTH STENCIL FORMAT ---//
	// * 新增参数 bool bUseDX9DSFormat
	// * 用以某些特殊情况时 需要生成DX9 DEPTH STENCIL FORMAT类型的DEPTH TEXTURE.
	// * 现在用于shadow mapping的硬件PCF.
	virtual ATOM_AUTOREF(ATOM_Texture) AllocateDepthTexture (int w, int h, bool bUseDX9DSFormat = false );
	//---------------------------------------------------//
	//--- wangjian MODIFIED ---//
	// 异步加载：添加异步加载优先级（默认为异步加载）
	virtual ATOM_AUTOREF(ATOM_Texture) AllocateTexture (const char *filename, unsigned width, unsigned height, bool asResource, ATOM_PixelFormat format, 
														int loadPriority = ATOM_LoadPriority_ASYNCBASE);
	//-------------------------//
    virtual ATOM_AUTOREF(ATOM_Texture) LookupTexture(const char* name) = 0;
	virtual unsigned GetNumTextures (void) const;
	virtual bool DumpTextureStats (const char *filename);

  protected:
    virtual ATOM_AUTOREF(ATOM_Texture) doAllocateTexture (unsigned flags, const char* name) = 0;
    virtual ATOM_AUTOREF(ATOM_Texture) doAllocateTexture (const void* image, int w, int h, ATOM_PixelFormat format, unsigned flags, const char* name) = 0;
    virtual ATOM_AUTOREF(ATOM_Texture) doAllocateVolumeTexture (const void* image, int w, int h, int d, ATOM_PixelFormat format, unsigned flags, const char* name) = 0;
    virtual ATOM_AUTOREF(ATOM_Texture) doAllocateCubeTexture (const void* image[6], int size, ATOM_PixelFormat format, unsigned flags, const char* name) = 0;
    //--- wangjian added for DX9 DEPTH STENCIL FORMAT ---//
	// * 新增参数 bool bUseDX9DSFormat
	// * 用以某些特殊情况时 需要生成DX9 DEPTH STENCIL FORMAT类型的DEPTH TEXTURE.
	// * 现在用于shadow mapping的硬件PCF.
	virtual ATOM_AUTOREF(ATOM_Texture) doAllocateDepthTexture (int w, int h, bool bUseDX9DSFormat = false ) = 0;
	//---------------------------------------------------//
	//--- wangjian MODIFIED for Multithreaded ---//
	// 异步加载：添加异步加载优先级（默认为异步加载）
	virtual ATOM_AUTOREF(ATOM_Texture) doAllocateTexture (	const char *filename, unsigned width, unsigned height, bool asResource, ATOM_PixelFormat format, 
															int loadPriority = ATOM_LoadPriority_ASYNCBASE	) = 0;
	//-------------------------------------------//
  public:
    ATOM_TextureNode *newTextureNode (ATOM_Texture *texture, const char *filename);
	void freeTextureNode (ATOM_TextureNode *node);
	void attachNode (ATOM_TextureNode *node);
	void detachNode (ATOM_TextureNode *node);
	void stepFront (ATOM_TextureNode *node);
	void stepBack (ATOM_TextureNode *node);
	void setFront (ATOM_TextureNode *node);
	void validateNodeList (void);

  protected:
    ATOM_RenderDevice *_M_device;
	ATOM_TextureNode *_M_nodes;
	unsigned _M_num_textures;
  };

#endif // __ATOM_GLRENDER_ITEXTUREMANAGER_H
