#include "stdafx.h"
#include "texturemanager_d3d.h"
#include "texture_d3d.h"
#include "d3d9device.h"

ATOM_ID3DTextureManager::ATOM_ID3DTextureManager(ATOM_RenderDevice *device)
  :ATOM_TextureManager (device) {
}

ATOM_ID3DTextureManager::~ATOM_ID3DTextureManager() {
  Invalidate ();
}

ATOM_AUTOREF(ATOM_Texture) ATOM_ID3DTextureManager::doAllocateTexture (unsigned flags, const char* name) 
{
  ATOM_ASSERT (!name || !LookupTexture(name));

  if ( (flags & ATOM_Texture::TEXTURE2D) == 0)
  {
    ATOM_LOGGER::error ("[ATOM_ID3DTextureManager::doAllocateTexture] Invalid arguments: 0x%08X, %s.\n", flags, name);
    return 0;
  }

  if ((flags & ATOM_Texture::RENDERTARGET) || !_M_device->getCapabilities().texture_s3tc_compress)
    flags |= ATOM_Texture::NOCOMPRESS;

  if (flags & ATOM_Texture::RENDERTARGET)
  {
	  flags |= ATOM_Texture::NOMIPMAP;
  }

  ATOM_AUTOREF(ATOM_D3DTexture) tex = InternalAllocTexture(flags, name);
  if (!tex)
  {
	  return 0;
  }

  return tex;
}

//--- wangjian MODIFIED for Multithreaded loading ---//
// 异步加载 ： 添加异步加载优先级
ATOM_AUTOREF(ATOM_Texture) ATOM_ID3DTextureManager::doAllocateTexture (	const char *filename, 
																		unsigned width, 
																		unsigned height, 
																		bool asResource, 
																		ATOM_PixelFormat format,
																		int loadPriority	)
{
	ATOM_HARDREF(ATOM_D3DTexture) texture(ATOM_D3DTexture::_classname(), filename);
	texture->setRenderDevice (_M_device);

	//--- wangjian modified ---//
	// 异步加载
	// 检查该纹理资源是否是需要立即加载
	// 如果没有开启异步加载 | 如果显式不用异步加载 | 如果该资源在立即加载的资源表中
	bool bNotMT =	( !ATOM_AsyncLoader::IsRun() ) || 
					( ATOM_LoadPriority_IMMEDIATE == loadPriority ) || 
					ATOM_AsyncLoader::CheckImmediateResource( filename );
	if( bNotMT )
	{
		//--- wangjian added ---//
		// 标记该纹理异步加载完成
		//（虽然不是异步加载的，但是可能出现在直接加载之前该纹理资源进行了异步加载操作）
		texture->getAsyncLoader()->priority = ATOM_LoadPriority_IMMEDIATE;
		texture->getAsyncLoader()->SetLoadStage(ATOM_AsyncLoader::ATOM_ASYNCLOAD_ALLFINISHED);

		if( ATOM_AsyncLoader::isEnableLog() )
			ATOM_LOGGER::log("ATOM_ID3DTextureManager::doAllocateTexture --- sync load texture : %s \n", filename );

		texture->getLoadInterface()->setFileName (filename);
		texture->getLoadInterface()->setWidth (width);
		texture->getLoadInterface()->setHeight (height);
		texture->getLoadInterface()->setFormat (format);
		texture->getLoadInterface()->load ();
		if (texture->getLoadInterface()->getLoadingState() != ATOM_LoadInterface::LS_LOADED)
		{
			return 0;
		}
		//----------------------//
	}
	// 否则 异步加载
	else
	{
		texture->getAsyncLoader()->SetAttribute( texture.get(),width,height,format );
		texture->getAsyncLoader()->Start( filename, loadPriority );

		if( ATOM_AsyncLoader::isEnableLog() )
			ATOM_LOGGER::log("ATOM_ID3DTextureManager::doAllocateTexture --- async load texture : %s [ priority : %d ]\n", filename, loadPriority );
	}
	//------------------------//

	return texture;
}

ATOM_AUTOREF(ATOM_Texture) ATOM_ID3DTextureManager::doAllocateTexture (const void* image, int w, int h, ATOM_PixelFormat format, unsigned flags, const char* name) 
{
  ATOM_ASSERT (!name || !LookupTexture(name));

  if ( (flags & ATOM_Texture::TEXTURE2D) == 0)
  {
    /*ATOM_LOGGER::error ("[ATOM_ID3DTextureManager::doAllocateTexture] Invalid arguments: 0x%08X, %d, %d, %d, %d.\n",
                 image, w, h, format,flags);
    return ATOM_AUTOREF(ATOM_Texture)();*/
  }

  if ((flags & ATOM_Texture::RENDERTARGET) || !_M_device->getCapabilities().texture_s3tc_compress)
    flags |= ATOM_Texture::NOCOMPRESS;

  if (flags & ATOM_Texture::RENDERTARGET)
  {
	  flags |= ATOM_Texture::NOMIPMAP;
  }

  ATOM_AUTOREF(ATOM_D3DTexture) tex = InternalAllocTexture(w, h, format, image, flags, name);
  if (!tex || !tex->getD3DTexture())
  {
	  return 0;
  }

  return tex;
}

ATOM_AUTOREF(ATOM_Texture) ATOM_ID3DTextureManager::doAllocateVolumeTexture (const void* image, int w, int h, int d, ATOM_PixelFormat format, unsigned flags, const char* name) 
{
  ATOM_ASSERT (!name || !LookupTexture(name));

  if ( (flags & ATOM_Texture::TEXTURE3D) == 0)
  {
    ATOM_LOGGER::error ("[ATOM_ID3DTextureManager::doAllocateVolumeTexture] Invalid arguments: 0x%08X, %d, %d, %d, %d, %d.\n",
                 image, w, h, d, format,flags);
    return ATOM_AUTOREF(ATOM_Texture)();
  }

  if ((flags & ATOM_Texture::RENDERTARGET) || !_M_device->getCapabilities().texture_s3tc_compress)
    flags |= ATOM_Texture::NOCOMPRESS;

  if (flags & ATOM_Texture::RENDERTARGET)
  {
	  flags |= ATOM_Texture::NOMIPMAP;
  }

  ATOM_AUTOREF(ATOM_D3DTexture) tex = InternalAllocVolumeTexture(w, h, d, format, image, flags, name);
  if (!tex || !tex->getD3DTexture())
  {
	  return 0;
  }

  return tex;
}

ATOM_AUTOREF(ATOM_Texture) ATOM_ID3DTextureManager::doAllocateCubeTexture (const void* image[6], int size, ATOM_PixelFormat format, unsigned flags, const char* name)
{
  ATOM_ASSERT (!name || !LookupTexture(name));

  if ( (flags & ATOM_Texture::CUBEMAP) == 0)
  {
    ATOM_LOGGER::error ("[ATOM_ID3DTextureManager::doAllocateCubeTexture] Invalid arguments: 0x%08X, %d, %d, %d.\n",
                 image[0], size, format, flags);
    return ATOM_AUTOREF(ATOM_Texture)();
  }


  ATOM_AUTOREF(ATOM_D3DTexture) tex = InternalAllocCubeMap(size, format, image, flags, name);

  return tex;
}
//--- wangjian added for DX9 DEPTH STENCIL FORMAT ---//
// * 新增参数 bool bUseDX9DSFormat
// * 用以某些特殊情况时 需要生成DX9 DEPTH STENCIL FORMAT类型的DEPTH TEXTURE.
// * 现在用于shadow mapping的硬件PCF.
ATOM_AUTOREF(ATOM_Texture) ATOM_ID3DTextureManager::doAllocateDepthTexture (int w, int h, bool bUseDX9DSFormat)
{
  ATOM_AUTOREF(ATOM_D3DTexture) tex = InternalAllocDepthTexture(w, h,bUseDX9DSFormat);

  return tex;
}
//---------------------------------------------------//
ATOM_AUTOREF(ATOM_D3DTexture) ATOM_ID3DTextureManager::InternalAllocTexture(unsigned flags, const char* name) {
  ATOM_HARDREF(ATOM_D3DTexture) tex(0, name);
  tex->_M_manager = this;
  tex->setFlags(flags);
  tex->setRenderDevice (_M_device);
  return tex;
}

// Allocate texture with or without data contents
ATOM_AUTOREF(ATOM_D3DTexture) ATOM_ID3DTextureManager::InternalAllocTexture(int w,
                                                    int h,
                                                    ATOM_PixelFormat sformat,
                                                    const void* contents,
                                                    unsigned flags,
                                                    const char* name) {
  // Check parameters.
  bool paramcheck = true;
  unsigned tex2d = flags & ATOM_Texture::TEXTURE2D;
  int sizelimit = _M_device->getCapabilities().max_texture_size;
  bool npo2_c = _M_device->getCapabilities().texture_npo2_c;
  bool npo2 = _M_device->getCapabilities().texture_npo2;
  if (npo2_c)
  {
	  flags |= ATOM_Texture::NOMIPMAP;
  }

  bool wvalid = (w > 0 && (npo2 || npo2_c || w == ATOM_nextpo2(w)) && w <= sizelimit);
  bool hvalid = (h > 0 && (npo2 || npo2_c || h == ATOM_nextpo2(h)) && h <= sizelimit);

  if ( contents)
  {
    if ( !wvalid || !hvalid)
      paramcheck = false;
    else if ( tex2d && (!wvalid || !hvalid))
      paramcheck = false;
  }

  if ( !paramcheck)
  {
    ATOM_LOGGER::error("[ATOM_ID3DTextureManager::InternalAllocTexture] Invalid arguments: "
                "%d, %d, %d, 0x%08X, %d.\n",
                w,
                h,
                sformat,
                contents,
                flags);
    return 0;
  }

  ATOM_HARDREF(ATOM_D3DTexture) tex(0, name);

  tex->_M_manager = this;
  tex->setFlags(flags);
  tex->setRenderDevice (_M_device);

  int numMips = (flags & ATOM_Texture::NOMIPMAP) ? 1 : 0;
  if (w > 0 && h > 0 && !tex->loadTexImageFromMemory (contents, w, h, 1, sformat, numMips))
  {
	return 0;
  }

  return tex;
}

// Allocate texture with or without data contents
ATOM_AUTOREF(ATOM_D3DTexture) ATOM_ID3DTextureManager::InternalAllocVolumeTexture(int w,
                                                    int h,
													int d,
                                                    ATOM_PixelFormat sformat,
                                                    const void* contents,
                                                    unsigned flags,
                                                    const char* name) {
  // Check parameters.
  bool paramcheck = true;
  int sizelimit = _M_device->getCapabilities().max_texture_size;
  bool wvalid = (w > 0 && w == ATOM_nextpo2(w) && w <= sizelimit);
  bool hvalid = (h > 0 && h == ATOM_nextpo2(h) && h <= sizelimit);

  if ( contents)
  {
    if ( !wvalid || !hvalid)
      paramcheck = false;
  }

  if ( !paramcheck)
  {
    ATOM_LOGGER::error("[ATOM_ID3DTextureManager::InternalAllocVolumeTexture] Invalid arguments: "
                "%d, %d, %d, %d, 0x%08X, %d.\n",
                w,
                h,
				d,
                sformat,
                contents,
                flags);
    return 0;
  }

  ATOM_HARDREF(ATOM_D3DTexture) tex(0, name);

  tex->_M_manager = this;
  tex->setFlags(flags);
  tex->setRenderDevice (_M_device);

  if (w > 0 && h > 0 && d > 0 && !tex->loadTexImageFromMemory (contents, w, h, d, sformat, 1))
  {
	return 0;
  }

  return tex;
}

// Allocate a cubemap with or without data contents
ATOM_AUTOREF(ATOM_D3DTexture) ATOM_ID3DTextureManager::InternalAllocCubeMap (int size, ATOM_PixelFormat sformat, const void* contents[6], unsigned flags, const char* name) 
{
  //ATOM_LOGGER::error("[ATOM_ID3DTextureManager::InternalAllocCubeMap] Not implemented.\n");
  
  // Check parameters.
  unsigned texCube = flags & ATOM_Texture::CUBEMAP;
  int sizelimit = _M_device->getCapabilities().max_texture_size;
  bool valid = size > 0 && size == ATOM_nextpo2(size) && size <= sizelimit;

  bool paramcheck = valid && texCube;
  if ( !paramcheck)
  {
    ATOM_LOGGER::error ("[ATOM_ID3DTextureManager::InternalAllocCubeMap] Invalid arguments: %d, %d, 0x%08X, %d.\n",
                 size, sformat, contents[0], flags);
    return ATOM_AUTOREF(ATOM_D3DTexture)();
  }

  ATOM_HARDREF(ATOM_D3DTexture) tex(0, name);
  if (!tex)
    return ATOM_AUTOREF(ATOM_D3DTexture)();

  tex->_M_manager = this;
  tex->setFlags(flags);
  tex->setRenderDevice (_M_device);
  tex->loadCubeTexImage (contents, size, sformat, 0);

  return tex;
}
//--- wangjian added for DX9 DEPTH STENCIL FORMAT ---//
// * 新增参数 bool bUseDX9DSFormat
// * 用以某些特殊情况时 需要生成DX9 DEPTH STENCIL FORMAT类型的DEPTH TEXTURE.
// * 现在用于shadow mapping的硬件PCF.
ATOM_AUTOREF(ATOM_D3DTexture) ATOM_ID3DTextureManager::InternalAllocDepthTexture (int w, int h, bool bUseDX9DSFormat)
{
  ATOM_HARDREF(ATOM_D3DTexture) tex(0, 0);

  tex->_M_manager = this;
  tex->setFlags(ATOM_Texture::TEXTURE2D|ATOM_Texture::DEPTHSTENCIL);
  tex->setRenderDevice (_M_device);
  tex->createAsDepthTexture (w, h, bUseDX9DSFormat);

  return tex;
}
//---------------------------------------------------//
ATOM_AUTOREF(ATOM_Texture) ATOM_ID3DTextureManager::LookupTexture(const char* name) {
  return ATOM_SOFTREF(ATOM_D3DTexture) (0, name);
}

void ATOM_ID3DTextureManager::Invalidate (void)
{
}

