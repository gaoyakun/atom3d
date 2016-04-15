#include "stdafx.h"
#include "texture_d3d.h"
#include "texturemanager_d3d.h"
#include "d3d9device.h"
#include "depthbuffer_d3d.h"

extern long TextureCount;
extern long SurfaceCount;
extern D3DFORMAT toD3DFormat (int format); // in d3d9device.cpp

void IncrementTextureCount (void)
{
	::InterlockedIncrement (&TextureCount);
}

void IncrementSurfaceCount (void)
{
	::InterlockedIncrement (&SurfaceCount);
}

IDirect3DBaseTexture9 *ATOM_D3DTexture::getD3DTexture (void) const
{
  return _M_texture;
}

D3DFORMAT ATOM_D3DTexture::getD3DFormat (void) const
{
  return _M_d3dformat;
}

void ATOM_D3DTexture::releaseTexture (IDirect3DBaseTexture9 *texture)
{
  if (texture && !texture->Release ())
  {
    ::InterlockedDecrement (&TextureCount);
  }
}

void ATOM_D3DTexture::releaseSurface(IDirect3DSurface9 *surface)
{
  if (surface && !surface->Release ())
  {
    ::InterlockedDecrement (&SurfaceCount);
  }
}

ATOM_D3DTexture::ATOM_D3DTexture(void)
{
  ATOM_STACK_TRACE(ATOM_D3DTexture::ATOM_D3DTexture);
  _M_manager = 0;
  _M_d3dformat = D3DFMT_UNKNOWN;
  _M_d3dusage = 0;
  _M_nummips = 0;
  _M_texture = 0;
  _M_need_restore = false;
}

ATOM_D3DTexture::~ATOM_D3DTexture(void)
{
  ATOM_STACK_TRACE(ATOM_D3DTexture::~ATOM_D3DTexture);
  setRenderDevice (0);
}

void ATOM_D3DTexture::invalidateImpl (bool need_restore)
{
  ATOM_STACK_TRACE(ATOM_D3DTexture::invalidateImpl);
  ATOM_Texture::invalidateImpl (need_restore);

  if (!need_restore || _M_need_restore)
  {
    if (_M_texture)
    {
      releaseTexture (_M_texture);
      _M_texture = 0;
    }
  }
  _M_depthBuffer = 0;
}

void ATOM_D3DTexture::restoreImpl (void)
{
  ATOM_STACK_TRACE(ATOM_D3DTexture::restoreImpl);
  if (_M_device && !_M_texture && !needRestore())
  {
    realize (_M_nummips);
	setContentLost (true);
  }
}

D3DFORMAT ATOM_D3DTexture::chooseD3DFormat (ATOM_D3D9Device *device, ATOM_PixelFormat format, bool compress, bool rendertarget, int w, int h) const
{
  ATOM_STACK_TRACE(ATOM_D3DTexture::chooseD3DFormat);
  D3DFORMAT d3dformat = D3DFMT_UNKNOWN;
  bool supportS3TC = rendertarget ? false : device->getCapabilities().texture_s3tc_compress;
  bool supportFloatTexture32 = rendertarget ? device->getCapabilities().rendertarget_float32 : device->getCapabilities().texture_float32;
  bool supportFloatTexture16 = rendertarget ? device->getCapabilities().rendertarget_float16 : device->getCapabilities().texture_float16;

  switch (format)
  {
  case ATOM_PIXEL_FORMAT_RGBA8888:
  case ATOM_PIXEL_FORMAT_BGRA8888:
    d3dformat = D3DFMT_A8R8G8B8;
    break;
  case ATOM_PIXEL_FORMAT_RGBX8888:
  case ATOM_PIXEL_FORMAT_RGB888:
  case ATOM_PIXEL_FORMAT_BGRX8888:
  case ATOM_PIXEL_FORMAT_BGR888:
    d3dformat = D3DFMT_X8R8G8B8;
    break;
  case ATOM_PIXEL_FORMAT_A8:
    d3dformat = device->isTextureFormatOk(D3DFMT_A8) ? D3DFMT_A8 : D3DFMT_A8R8G8B8;
    break;
  case ATOM_PIXEL_FORMAT_GREY8:
    d3dformat = device->isTextureFormatOk(D3DFMT_L8) ? D3DFMT_L8 : D3DFMT_X8R8G8B8;
    break;
  case ATOM_PIXEL_FORMAT_DXT1:
    d3dformat = supportS3TC ? D3DFMT_DXT1 : D3DFMT_X8R8G8B8;
    break;
  case ATOM_PIXEL_FORMAT_DXT3:
    d3dformat = supportS3TC ? D3DFMT_DXT3 : D3DFMT_A8R8G8B8;
    break;
  case ATOM_PIXEL_FORMAT_DXT5:
    d3dformat = supportS3TC ? D3DFMT_DXT5 : D3DFMT_A8R8G8B8;
    break;
  case ATOM_PIXEL_FORMAT_R32F:
    if (rendertarget)
	{
		d3dformat = device->isRenderTargetFormatOk(D3DFMT_R32F) ? D3DFMT_R32F : D3DFMT_UNKNOWN;
	}
	else
	{
		d3dformat = device->isTextureFormatOk(D3DFMT_R32F) ? D3DFMT_R32F : D3DFMT_UNKNOWN;
	}
    break;
  case ATOM_PIXEL_FORMAT_RG32F:
    if (rendertarget)
	{
		d3dformat = device->isRenderTargetFormatOk(D3DFMT_G32R32F) ? D3DFMT_G32R32F : D3DFMT_UNKNOWN;
	}
	else
	{
		d3dformat = device->isTextureFormatOk(D3DFMT_G32R32F) ? D3DFMT_G32R32F : D3DFMT_UNKNOWN;
	}
	break;
  case ATOM_PIXEL_FORMAT_RGBA32F:
    if (rendertarget)
	{
		d3dformat = device->isRenderTargetFormatOk(D3DFMT_A32B32G32R32F) ? D3DFMT_A32B32G32R32F : D3DFMT_UNKNOWN;
	}
	else
	{
		d3dformat = device->isTextureFormatOk(D3DFMT_A32B32G32R32F) ? D3DFMT_A32B32G32R32F : D3DFMT_UNKNOWN;
	}
	break;
  case ATOM_PIXEL_FORMAT_R16F:
    if (rendertarget)
	{
		d3dformat = device->isRenderTargetFormatOk(D3DFMT_R16F) ? D3DFMT_R16F : D3DFMT_UNKNOWN;
	}
	else
	{
		d3dformat = device->isTextureFormatOk(D3DFMT_R16F) ? D3DFMT_R16F : D3DFMT_UNKNOWN;
	}
    break;
  case ATOM_PIXEL_FORMAT_RG16F:
    if (rendertarget)
	{
		d3dformat = device->isRenderTargetFormatOk(D3DFMT_G16R16F) ? D3DFMT_G16R16F : D3DFMT_UNKNOWN;
	}
	else
	{
		d3dformat = device->isTextureFormatOk(D3DFMT_G16R16F) ? D3DFMT_G16R16F : D3DFMT_UNKNOWN;
	}
	break;
  case ATOM_PIXEL_FORMAT_RGBA16F:
    if (rendertarget)
	{
		d3dformat = device->isRenderTargetFormatOk(D3DFMT_A16B16G16R16F) ? D3DFMT_A16B16G16R16F : D3DFMT_UNKNOWN;
	}
	else
	{
		d3dformat = device->isTextureFormatOk(D3DFMT_A16B16G16R16F) ? D3DFMT_A16B16G16R16F : D3DFMT_UNKNOWN;
	}
	break;
  default:
    break;
  }

  if (0 && !rendertarget && compress && w >= 4 && h >= 4)
  {
    switch (d3dformat)
    {
    case D3DFMT_A8R8G8B8:
      if (supportS3TC)
        d3dformat = D3DFMT_DXT5;
      break;
    case D3DFMT_X8R8G8B8:
      if (supportS3TC)
        d3dformat = D3DFMT_DXT1;
      break;
    }
  }

  return d3dformat;
}

IDirect3DTexture9 *ATOM_D3DTexture::internalCreateDummyTexture (ATOM_D3D9Device *device, int w, int h, D3DFORMAT format, int numMips)
{
  ATOM_STACK_TRACE(ATOM_D3DTexture::internalCreateDummyTexture);
  D3DFORMAT d3dformat = format;
  if (d3dformat == D3DFMT_UNKNOWN)
  {
	ATOM_LOGGER::error ("%s(0x%08X, %d, %d, %d, %d) Invalid D3D format.\n", __FUNCTION__, device, w, h, format, numMips);
    return 0;
  }

  UINT width = w;
  UINT height = h;
  UINT mipcount = numMips;
  D3DFORMAT actualformat = d3dformat;

  //--- wangjian commented ---//
  // 去除对D3DX依赖：
  HRESULT hr = D3DXCheckTextureRequirements (
    device->getD3DDevice(),
    &width,
    &height,
    &mipcount,
    0,
    &actualformat,
    D3DPOOL_SYSTEMMEM
  );

  if (FAILED(hr))
  {
	ATOM_LOGGER::error ("%s(0x%08X, %d, %d, %d, %d) D3DXCheckTextureRequirements() failed.\n", __FUNCTION__, device, w, h, format, numMips);
	ATOM_CHECK_D3D_RESULT(hr);
    return 0;
  }

  IDirect3DTexture9 *dummy_texture = 0;

  hr = device->getD3DDevice()->CreateTexture(
    width,
    height,
    mipcount,
    0,
    actualformat,
    D3DPOOL_SYSTEMMEM,
    &dummy_texture,
    0
  );

  if (FAILED(hr))
  {
	ATOM_LOGGER::error ("%s(0x%08X, %d, %d, %d, %d) CreateTexture failed.\n", __FUNCTION__, device, w, h, format, numMips);
	ATOM_CHECK_D3D_RESULT(hr);
    return 0;
  }

  ATOM_ASSERT (dummy_texture);

  IncrementTextureCount ();
  return dummy_texture;
}

bool ATOM_D3DTexture::internalCreateTexture (ATOM_D3D9Device *device, int w, int h, ATOM_PixelFormat format, unsigned numMips)
{
  ATOM_STACK_TRACE(ATOM_D3DTexture::internalCreateTexture);
  bool isRenderTarget = _M_flags & ATOM_Texture::RENDERTARGET;
  bool sysmem = _M_flags & ATOM_Texture::NORENDER;
  bool dynamic = !isRenderTarget && (_M_flags & ATOM_Texture::DYNAMIC) && device->getCapabilities().texture_stream_write;
  if (isRenderTarget && sysmem)
  {
	return false;
  }
  

  D3DFORMAT d3dformat = chooseD3DFormat (device, format, (getFlags() & ATOM_Texture::NOCOMPRESS) == 0, isRenderTarget, w, h);
  if (d3dformat == D3DFMT_UNKNOWN)
  {
	ATOM_LOGGER::error ("%s(0x%08X, %d, %d, %d) Invalid format.\n", __FUNCTION__, device, w, h, format);
    return false;
  }

  //--- wangjian added ---//
  // 异步加载 ：是否支持自动生成MIPMAP
  bool isAutoGenMipmap = (_M_flags & ATOM_Texture::AUTOGENMIPMAP) && !(_M_flags & ATOM_Texture::NOMIPMAP) ;
  if( isAutoGenMipmap )
  {
	  isAutoGenMipmap = device->canAutoGenMipmap(d3dformat);
  }

  if (isRenderTarget)
	_M_d3dusage = D3DUSAGE_RENDERTARGET;
  else if (dynamic)
	_M_d3dusage = D3DUSAGE_DYNAMIC;
  //--- wangjian added ---//
  else if(isAutoGenMipmap)
	  _M_d3dusage = D3DUSAGE_AUTOGENMIPMAP;
  //----------------------//
  else
	_M_d3dusage = 0;

  _M_d3dpool = (isRenderTarget||dynamic) ? D3DPOOL_DEFAULT : (sysmem?D3DPOOL_SYSTEMMEM:D3DPOOL_MANAGED);
  UINT width = w;
  UINT height = h;
  UINT mipcount = 1;
  if ((getFlags() & ATOM_Texture::NOMIPMAP) == 0)
  {
	  unsigned n = (w > h) ? w : h;
	  while (n > 4)
	  {
		  mipcount++;
		  n >>= 1;
	  }

	  if (numMips != 0)
	  {
		  if (numMips > mipcount)
		  {
			  numMips = mipcount;
		  }
		  mipcount = numMips;
	  }
  }

    HRESULT hr = D3DXCheckTextureRequirements(
      device->getD3DDevice(),
      &width,
      &height,
      &mipcount,
      _M_d3dusage,
      &d3dformat,
      _M_d3dpool
    );

    if (FAILED(hr) && (d3dformat == D3DFMT_D16 || d3dformat == D3DFMT_D24X8))
    {
	  ATOM_LOGGER::error ("%s(0x%08X, %d, %d, %d) D3DXCheckTextureRequirements() failed.\n", __FUNCTION__, device, w, h, format);
	  ATOM_CHECK_D3D_RESULT(hr);
      d3dformat = (d3dformat == D3DFMT_D16) ? D3DFMT_D24X8 : D3DFMT_D16;

      hr = D3DXCheckTextureRequirements(
        device->getD3DDevice(),
        &width,
        &height,
        &mipcount,
        _M_d3dusage,
        &d3dformat,
        _M_d3dpool
      );

	  if (FAILED(hr) || w != width || h != height)
	  {
	    ATOM_LOGGER::error ("%s(0x%08X, %d, %d, %d) D3DXCheckTextureRequirements() failed.\n", __FUNCTION__, device, w, h, format);
		ATOM_CHECK_D3D_RESULT(hr);
		return false;
	  }
    }

  IDirect3DTexture9 *pTex = NULL;
  hr = device->getD3DDevice()->CreateTexture(
    width,
    height,
    mipcount,
    _M_d3dusage,
    d3dformat,
    _M_d3dpool,
    &pTex,
    0
  );
  _M_texture = pTex;

  if (FAILED(hr))
  {
	if (hr == D3DERR_OUTOFVIDEOMEMORY && _M_d3dpool == D3DPOOL_DEFAULT)
	{
	  ATOM_LOGGER::warning ("%s(0x%08X, %d, %d, %d) CreateTexture() failed due to insufficient video memory, all managed resource will be evicted.\n", __FUNCTION__, device, w, h, format);
	  ATOM_CHECK_D3D_RESULT(device->getD3DDevice()->EvictManagedResources ());

      hr = device->getD3DDevice()->CreateTexture(
        width,
        height,
        mipcount,
        _M_d3dusage,
        d3dformat,
        _M_d3dpool,
        &pTex,
        0
      );
      _M_texture = pTex;

	  if (FAILED(hr))
	  {
		ATOM_LOGGER::error ("%s(0x%08X, %d, %d, %d) CreateTexture() failed.\n", __FUNCTION__, device, w, h, format);
		ATOM_CHECK_D3D_RESULT(hr);
		return false;
	  }
	}
	else
	{
	  ATOM_LOGGER::error ("%s(0x%08X, %d, %d, %d) CreateTexture() failed.\n", __FUNCTION__, device, w, h, format);
	  ATOM_CHECK_D3D_RESULT(hr);
	}
  }

  /// TODO: for test only
  //if (d3dformat != D3DFMT_DXT1 && d3dformat != D3DFMT_DXT3 && d3dformat != D3DFMT_DXT5)
  //{
  //  ATOM_LOGGER::error ("**INFO: Non-compressed texture created with width = %d, height = %d, name = %s\n", width, height, ObjectName());
  //}

  IncrementTextureCount ();

  _M_d3dformat = d3dformat;
  _M_need_restore = _M_d3dpool == D3DPOOL_DEFAULT;

  return true;
}

bool ATOM_D3DTexture::internalCreateCubeTexture (ATOM_D3D9Device *device, int size, ATOM_PixelFormat format, unsigned numMips)
{
  ATOM_STACK_TRACE(ATOM_D3DTexture::internalCreateCubeTexture);
  bool isRenderTarget = _M_flags & ATOM_Texture::RENDERTARGET;
  bool managed = true;

  D3DFORMAT d3dformat = chooseD3DFormat (device, format, (getFlags() & ATOM_Texture::NOCOMPRESS) == 0, isRenderTarget, size, size);
  if (d3dformat == D3DFMT_UNKNOWN)
    return false;

  if (isRenderTarget)
    _M_d3dusage = D3DUSAGE_RENDERTARGET;
  else
	_M_d3dusage = 0;

  _M_d3dpool = (isRenderTarget || !managed) ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED;
  UINT texsize = size;
  UINT mipcount = 1;
  if ((getFlags() & ATOM_Texture::NOMIPMAP) == 0)
  {
	  int n = size;
	  while (n > 4)
	  {
		  mipcount++;
		  n >>= 1;
	  }

	  if (numMips != 0)
	  {
		  if (numMips > mipcount)
		  {
			  numMips = mipcount;
		  }
		  mipcount = numMips;
	  }
  }

  HRESULT hr = D3DXCheckCubeTextureRequirements (device->getD3DDevice(), &texsize, &mipcount, _M_d3dusage, &d3dformat, _M_d3dpool);
  if (FAILED (hr))
  {
	ATOM_LOGGER::error ("%s(0x%08X, %d, %d) D3DXCheckCubeTextureRequirements() failed.\n", __FUNCTION__, device, size, format);
    ATOM_CHECK_D3D_RESULT(hr);
    return false;
  }

  hr = device->getD3DDevice()->CreateCubeTexture (texsize, mipcount, _M_d3dusage, d3dformat, _M_d3dpool, (IDirect3DCubeTexture9**)&_M_texture, NULL);
  if (FAILED(hr))
  {
	if (hr == D3DERR_OUTOFVIDEOMEMORY && _M_d3dpool == D3DPOOL_DEFAULT)
	{
		ATOM_LOGGER::warning ("%s(0x%08X, %d, %d) CreateCubeTexture() failed due to insufficient video memory, all managed resource will be evicted.\n", __FUNCTION__, device, size, format);
		ATOM_CHECK_D3D_RESULT(device->getD3DDevice()->EvictManagedResources());
		hr = device->getD3DDevice()->CreateCubeTexture (texsize, mipcount, _M_d3dusage, d3dformat, _M_d3dpool, (IDirect3DCubeTexture9**)&_M_texture, NULL);
		if (FAILED(hr))
		{
			ATOM_LOGGER::error ("%s(0x%08X, %d, %d) CreateCubeTexture() failed .\n", __FUNCTION__, device, size, format);
			ATOM_CHECK_D3D_RESULT(hr);
			return false;
		}
	}
	else
	{
		ATOM_LOGGER::error ("%s(0x%08X, %d, %d) CreateCubeTexture() failed .\n", __FUNCTION__, device, size, format);
		ATOM_CHECK_D3D_RESULT(hr);
		return false;
	}
  }

  return true;
}

bool ATOM_D3DTexture::internalCreateVolumeTexture (ATOM_D3D9Device *device, int w, int h, int d, ATOM_PixelFormat format, unsigned numMips)
{
  ATOM_STACK_TRACE(ATOM_D3DTexture::internalCreateVolumeTexture);

  D3DFORMAT d3dformat = chooseD3DFormat (device, format, (getFlags() & ATOM_Texture::NOCOMPRESS) == 0, false, w, h);
  if (d3dformat == D3DFMT_UNKNOWN)
    return false;

  _M_d3dusage = 0;

  _M_d3dpool = D3DPOOL_MANAGED;
  UINT mipcount = 1;
  if ((getFlags() & ATOM_Texture::NOMIPMAP) == 0)
  {
	  int n = ATOM_max2(w, ATOM_max2(h, d));
	  while (n > 4)
	  {
		  mipcount++;
		  n >>= 1;
	  }

	  if (numMips != 0)
	  {
		  if (numMips > mipcount)
		  {
			  numMips = mipcount;
		  }
		  mipcount = numMips;
	  }
  }

  unsigned texW = w;
  unsigned texH = h;
  unsigned texD = d;
  HRESULT hr = D3DXCheckVolumeTextureRequirements (device->getD3DDevice(), &texW, &texH, &texD, &mipcount, _M_d3dusage, &d3dformat, _M_d3dpool);
  if (FAILED (hr))
  {
	ATOM_LOGGER::error ("%s(0x%08X, %d, %d, %d, %d) D3DXCheckVolumeTextureRequirements() failed.\n", __FUNCTION__, device, texW, texH, texD, format);
    ATOM_CHECK_D3D_RESULT(hr);
    return false;
  }

  hr = device->getD3DDevice()->CreateVolumeTexture (texW, texH, texD, mipcount, _M_d3dusage, d3dformat, _M_d3dpool, (IDirect3DVolumeTexture9**)&_M_texture, NULL);
  if (FAILED(hr))
  {
	if (hr == D3DERR_OUTOFVIDEOMEMORY && _M_d3dpool == D3DPOOL_DEFAULT)
	{
		ATOM_LOGGER::warning ("%s(0x%08X, %d, %d, %d, %d) CreateVolumeTexture() failed due to insufficient video memory, all managed resource will be evicted.\n", __FUNCTION__, device, texW, texH, texD, format);
		ATOM_CHECK_D3D_RESULT(device->getD3DDevice()->EvictManagedResources());
		hr = device->getD3DDevice()->CreateVolumeTexture (texW, texH, texD, mipcount, _M_d3dusage, d3dformat, _M_d3dpool, (IDirect3DVolumeTexture9**)&_M_texture, NULL);
		if (FAILED(hr))
		{
			ATOM_LOGGER::error ("%s(0x%08X, %d, %d, %d, %d) CreateVolumeTexture() failed .\n", __FUNCTION__, device, texW, texH, texD, format);
			ATOM_CHECK_D3D_RESULT(hr);
			return false;
		}
	}
	else
	{
		ATOM_LOGGER::error ("%s(0x%08X, %d, %d, %d, %d) CreateVolumeTexture() failed .\n", __FUNCTION__, device, texW, texH, texD, format);
		ATOM_CHECK_D3D_RESULT(hr);
		return false;
	}
  }

  return true;
}

static inline void *convert_RGBA_to_BGRA (const unsigned char *p, int w, int h)
{
  unsigned sz = 4 * w * h;
  unsigned char *dst = (unsigned char*)ATOM_MALLOC(sz);
  const unsigned char *src = p;
  for (unsigned i = 0; i < w * h; ++i)
  {
    *dst++ = src[2];
    *dst++ = src[1];
    *dst++ = src[0];
    *dst++ = src[3];
    src += 4;
  }
  return dst - sz;
}

static inline void *convert_RGBX_to_BGR (const unsigned char *p, int w, int h)
{
  unsigned sz = 3 * w * h;
  unsigned char *dst = (unsigned char*)ATOM_MALLOC(3 * w * h);
  const unsigned char *src = p;
  for (unsigned i = 0; i < w * h; ++i)
  {
    *dst++ = src[2];
    *dst++ = src[1];
    *dst++ = src[0];
    src += 4;
  }
  return dst - sz;
}

static inline void *convert_RGB_to_BGR (const unsigned char *p, int w, int h)
{
  unsigned sz = 3 * w * h;
  unsigned char *dst = (unsigned char*)ATOM_MALLOC(3 * w * h);
  const unsigned char *src = p;
  for (unsigned i = 0; i < w * h; ++i)
  {
    *dst++ = src[2];
    *dst++ = src[1];
    *dst++ = src[0];
    src += 3;
  }
  return dst - sz;
}

bool ATOM_D3DTexture::loadCubeTexImage (const void *contents[6], int size, ATOM_PixelFormat format, unsigned numMips)
{
  ATOM_STACK_TRACE(ATOM_D3DTexture::loadCubeTexImage);

  if (!_M_device || !_M_device->isDeviceReady ())
  {
	  return false;
  }

  HRESULT hr;
  IDirect3DDevice9 *device = ((ATOM_D3D9Device*)_M_device)->getD3DDevice();

  //hr = D3DXCreateCubeTextureFromFile(device, "D:/development/childhood/tongnian/tmp/hero/Texture3.dds", (IDirect3DCubeTexture9**)&_M_texture);
  //ATOM_ASSERT (SUCCEEDED(hr));
  //return true;

  _M_pixel_width = _M_pixel_height = size;
  _M_format = format;
  _M_d3dformat = D3DFMT_UNKNOWN;

  if (!realize (numMips))
  {
    return false;
  }

  if (_M_texture->GetType() != D3DRTYPE_CUBETEXTURE)
  {
    return false;
  }

  if (!contents)
  {
	_M_nummips = _M_texture->GetLevelCount ();
    return true;
  }

  D3DFORMAT srcformat;

  switch ( format)
  {
  case ATOM_PIXEL_FORMAT_BGRA8888:
  case ATOM_PIXEL_FORMAT_RGBA8888:
    srcformat = D3DFMT_A8R8G8B8;
    break;
  case ATOM_PIXEL_FORMAT_BGRX8888:
  case ATOM_PIXEL_FORMAT_BGR888:
  case ATOM_PIXEL_FORMAT_RGBX8888:
  case ATOM_PIXEL_FORMAT_RGB888:
    srcformat = D3DFMT_R8G8B8;
    break;
  case ATOM_PIXEL_FORMAT_R32F:
    srcformat = D3DFMT_R32F;
	break;
  case ATOM_PIXEL_FORMAT_RG32F:
	srcformat = D3DFMT_G32R32F;
	break;
  case ATOM_PIXEL_FORMAT_RGBA32F:
    srcformat = D3DFMT_A32B32G32R32F;
	break;
  case ATOM_PIXEL_FORMAT_R16F:
	srcformat = D3DFMT_R16F;
	break;
  case ATOM_PIXEL_FORMAT_RG16F:
    srcformat = D3DFMT_G16R16F;
	break;
  case ATOM_PIXEL_FORMAT_RGBA16F:
    srcformat = D3DFMT_A16B16G16R16F;
	break;
  default:
    ATOM_LOGGER::error("%s(0x%08X, %d, %d) Invalid source format.\n", __FUNCTION__, contents, size, format);
    return false;
  }

  IDirect3DCubeTexture9 *pCubeTex = (IDirect3DCubeTexture9*)_M_texture;
  IDirect3DSurface9 *pSurface = 0;

  RECT rc;
  rc.left = 0;
  rc.top = 0;
  rc.right = _M_pixel_width;
  rc.bottom = _M_pixel_height;

  for (int i=0; i<6; ++i)
  {
    hr = pCubeTex->GetCubeMapSurface ((D3DCUBEMAP_FACES)i, 0, &pSurface);
	if (FAILED(hr))
	{
		ATOM_LOGGER::error("%s(0x%08X, %d, %d) GetCubeMapSurface() failed.\n", __FUNCTION__, contents, size, format);
		ATOM_CHECK_D3D_RESULT(hr);
		return false;
	}

	//--- wangjian commented ---//
	// 使用LOCK/UNLOCK填充
    hr = D3DXLoadSurfaceFromMemory (pSurface, 0, 0, contents[i], srcformat, _M_pixel_width * ATOM_PIXEL_FORMAT(format).bpp/8, 0, &rc, D3DX_FILTER_NONE, 0);
    pSurface->Release();

	if (FAILED(hr))
	{
		ATOM_LOGGER::error("%s(0x%08X, %d, %d) D3DXLoadSurfaceFromMemory() failed.\n", __FUNCTION__, contents, size, format);
		ATOM_CHECK_D3D_RESULT(hr);
		return false;
	}
  }

  _M_nummips = _M_texture->GetLevelCount ();
  return true;
}

bool ATOM_D3DTexture::loadTexImageFromFileInMemory (const void *fileInMemory, unsigned size, unsigned width, unsigned height, ATOM_PixelFormat format)
{
  ATOM_STACK_TRACE(ATOM_D3DTexture::loadTexImageFromFileInMemory);
  D3DXIMAGE_INFO imageInfo;
  HRESULT hr = D3DXGetImageInfoFromFileInMemory (fileInMemory, size, &imageInfo);
  if (FAILED(hr))
  {
	ATOM_LOGGER::error ("%s(%p, %u, %u, %u) D3DXGetImageInfoFromFileInMemory failed.\n", __FUNCTION__, fileInMemory, size, width, height);
	ATOM_CHECK_D3D_RESULT(hr);
	return false;
  }

  int type;
  switch (imageInfo.ResourceType)
  {
  case D3DRTYPE_TEXTURE:
    type = ATOM_Texture::TEXTURE2D;
    break;
  case D3DRTYPE_VOLUMETEXTURE:
    type = ATOM_Texture::TEXTURE3D;
    break;
  case D3DRTYPE_CUBETEXTURE:
    type = ATOM_Texture::CUBEMAP;
    break;
  default:
    return false;
  }

  IDirect3DBaseTexture9 *texture = 0;
  UINT w = imageInfo.Width;
  UINT h = imageInfo.Height;
  UINT d = imageInfo.Depth;
  if (width && w > width) w = width;
  if (height && h > height) h = height;

  UINT nummips = ((getFlags() & ATOM_Texture::NOMIPMAP) == 0) ? (imageInfo.MipLevels > 1 ? imageInfo.MipLevels : 0) : 1;
  _M_d3dformat = (format == ATOM_PIXEL_FORMAT_UNKNOWN) ? imageInfo.Format : toD3DFormat (format);
  if (_M_d3dformat == D3DFMT_UNKNOWN)
  {
	  _M_d3dformat = imageInfo.Format;
  }

  if (!ATOM_ispo2(w) || !ATOM_ispo2(h))
  {
	  if (_M_device->getCapabilities().texture_npo2_c)
	  {
		  nummips = 1;
		  if (_M_d3dformat == D3DFMT_DXT1)
		  {
			  _M_d3dformat = D3DFMT_X8R8G8B8;
		  }
		  else if (_M_d3dformat == D3DFMT_DXT3 || _M_d3dformat == D3DFMT_DXT5)
		  {
			  _M_d3dformat = D3DFMT_A8R8G8B8;
		  }
	  }
	  else if (!_M_device->getCapabilities().texture_npo2)
	  {
		  if (!ATOM_ispo2 (w))
		  {
			  w = ATOM_nextpo2 (w);
		  }

		  if (!ATOM_ispo2 (h))
		  {
			  h = ATOM_nextpo2 (h);
		  }
	  }
  }

  //if ((_M_flags & ATOM_Texture::NOCOMPRESS) == 0)
  //{
	 // switch (_M_d3dformat)
	 // {
	 // case D3DFMT_A8R8G8B8:
		//  _M_d3dformat = D3DFMT_DXT5;
		//  break;
	 // case D3DFMT_A4R4G4B4:
		//  _M_d3dformat = D3DFMT_DXT3;
		//  break;
	 // case D3DFMT_X8R8G8B8:
	 // case D3DFMT_R8G8B8:
	 // case D3DFMT_R5G6B5:
		//  _M_d3dformat = D3DFMT_DXT1;
		//  break;
	 // default:
		//  break;
	 // }
  //}

  DWORD mipFilter = (nummips != 1 && nummips != imageInfo.MipLevels) ? D3DX_FILTER_SRGB|D3DX_FILTER_TRIANGLE : D3DX_FILTER_NONE;

  switch (type)
  {
  case ATOM_Texture::TEXTURE2D:
    {
      HRESULT hr = D3DXCheckTextureRequirements (((ATOM_D3D9Device*)_M_device)->getD3DDevice(), &w, &h, &nummips, _M_d3dusage, &_M_d3dformat, D3DPOOL_MANAGED);
      if (FAILED(hr))
      {
		ATOM_LOGGER::error ("%s(%p, %u, %u, %u) D3DXCheckTextureRequirements failed.\n", __FUNCTION__, fileInMemory, size, width, height);
		ATOM_CHECK_D3D_RESULT(hr);
        return false;
      }

      IDirect3DTexture9 *texture2D = 0;
      hr = D3DXCreateTextureFromFileInMemoryEx (((ATOM_D3D9Device*)_M_device)->getD3DDevice(), fileInMemory, size, w, h, nummips, _M_d3dusage, _M_d3dformat, D3DPOOL_MANAGED, D3DX_FILTER_NONE, mipFilter, 0, &imageInfo, 0, &texture2D);
      if (FAILED(hr))
      {
		ATOM_LOGGER::error ("%s(%p, %u, %u, %u) D3DXCreateTextureFromFileInMemoryEx failed.\n", __FUNCTION__, fileInMemory, size, width, height);
		ATOM_CHECK_D3D_RESULT(hr);
        return false;
      }
	  IncrementTextureCount ();
      texture = texture2D;
      break;
    }
  case ATOM_Texture::TEXTURE3D:
    {
      HRESULT hr = D3DXCheckVolumeTextureRequirements (((ATOM_D3D9Device*)_M_device)->getD3DDevice(), &w, &h, &d, &nummips, _M_d3dusage, &_M_d3dformat, D3DPOOL_MANAGED);
      if (FAILED(hr))
      {
		ATOM_LOGGER::error ("%s(%p, %u, %u, %u) D3DXCheckVolumeTextureRequirements failed.\n", __FUNCTION__, fileInMemory, size, width, height);
		ATOM_CHECK_D3D_RESULT(hr);
        return false;
      }
      IDirect3DVolumeTexture9 *volumeTexture = 0;
      hr = D3DXCreateVolumeTextureFromFileInMemoryEx (((ATOM_D3D9Device*)_M_device)->getD3DDevice(), fileInMemory, size, w, h, d, nummips, _M_d3dusage, _M_d3dformat, D3DPOOL_MANAGED, D3DX_FILTER_NONE, mipFilter, 0, &imageInfo, 0, &volumeTexture);
      if (FAILED(hr))
      {
		ATOM_LOGGER::error ("%s(%p, %u, %u, %u) D3DXCreateVolumeTextureFromFileInMemoryEx failed.\n", __FUNCTION__, fileInMemory, size, width, height);
		ATOM_CHECK_D3D_RESULT(hr);
        return false;
      }
	  IncrementTextureCount ();
      texture = volumeTexture;
      break;
    }
  case ATOM_Texture::CUBEMAP:
    {
      HRESULT hr = D3DXCheckCubeTextureRequirements (((ATOM_D3D9Device*)_M_device)->getD3DDevice(), &w, &nummips, _M_d3dusage, &_M_d3dformat, D3DPOOL_MANAGED);
      if (FAILED(hr))
      {
		ATOM_LOGGER::error ("%s(%p, %u, %u, %u) D3DXCheckCubeTextureRequirements failed.\n", __FUNCTION__, fileInMemory, size, width, height);
		ATOM_CHECK_D3D_RESULT(hr);
        return false;
      }
      IDirect3DCubeTexture9 *cubeTexture = 0;
      hr = D3DXCreateCubeTextureFromFileInMemoryEx (((ATOM_D3D9Device*)_M_device)->getD3DDevice(), fileInMemory, size, w, nummips, _M_d3dusage, _M_d3dformat, D3DPOOL_MANAGED, D3DX_FILTER_NONE, mipFilter, 0, &imageInfo, 0, &cubeTexture);
      if (FAILED(hr))
      {
		ATOM_LOGGER::error ("%s(%p, %u, %u, %u) D3DXCreateCubeTextureFromFileInMemoryEx failed.\n", __FUNCTION__, fileInMemory, size, width, height);
		ATOM_CHECK_D3D_RESULT(hr);
        return false;
      }
	  IncrementTextureCount ();
      texture = cubeTexture;
      break;
    }
  default:
    return false;
  }

  if (texture)
  {
    if (_M_texture)
    {
      releaseTexture (_M_texture);
      _M_texture = 0;
    }

	_M_flags &= ~(ATOM_Texture::TEXTURE2D|ATOM_Texture::CUBEMAP|ATOM_Texture::TEXTURE3D);
    _M_flags |= type;

    _M_texture = texture;
    _M_pixel_width = w;
    _M_pixel_height = h;
	_M_nummips = _M_texture->GetLevelCount ();
    _M_need_restore = false;

    return true;
  }

  return false;
}

bool ATOM_D3DTexture::loadTexImageFromFile (const char *filename, unsigned width, unsigned height, ATOM_PixelFormat format)
{
  ATOM_STACK_TRACE(ATOM_D3DTexture::loadTexImageFromFile);
  if (!_M_device || !_M_device->isDeviceReady ())
  {
    return false;
  }

  //--- wangjian modified ---//
  // 异步加载
  if( ATOM_AsyncLoader::IsRun() && 
	  !filename )
  {
	  int w,h;
	  ATOM_PixelFormat fmt;
	  unsigned int flags;
	  bool bskipmip;
	  getAsyncLoader()->GetAttribute(w,h,fmt,flags,bskipmip);
	  unsigned int data_size = 0;
	  void* data = getAsyncLoader()->GetBuffer(data_size);
	  if( !data && data_size == 0 )
	  {
		  ATOM_LOGGER::error ("%s(%s, %u, %u)  Create texture failed, the data is not exist.\n", __FUNCTION__, filename, width, height);
		  return false;
	  }
	  if (!loadTexImageFromFileInMemory (data, data_size, w, h, fmt))
	  {
		  ATOM_LOGGER::error ("%s(%s, %u, %u)  Create texture failed.\n", __FUNCTION__, filename, width, height);
		  return false;
	  }
  }
  else
  {

	  ATOM_AutoFile fp(filename, ATOM_VFS::read);
	  if (!fp)
	  {
		  ATOM_LOGGER::error ("%s(%s, %u, %u)  Open file failed.\n", __FUNCTION__, filename, width, height);
		  return false;
	  }

	  unsigned size = fp->size ();
	  if (!size)
	  {
		  return false;
	  }

	  ATOM_VECTOR<unsigned char> contents(size);
	  fp->read (&contents[0], size);

	  if (!loadTexImageFromFileInMemory (&contents[0], size, width, height, format))
	  {
		  ATOM_LOGGER::error ("%s(%s, %u, %u)  Create texture failed.\n", __FUNCTION__, filename, width, height);
		  return false;
	  }
  
  }
  //-------------------------------------//
  

  return true;
}

bool ATOM_D3DTexture::loadTexImageFromMemory (const void *contents, int w, int h, int d, ATOM_PixelFormat format, unsigned numMips)
{
  ATOM_STACK_TRACE(ATOM_D3DTexture::loadTexImageFromMemory);
  if (!_M_device || !_M_device->isDeviceReady ())
  {
	return false;
  }

  if (_M_texture)
  {
    releaseTexture (_M_texture);
    _M_texture = 0;
  }

  const ATOM_RenderDeviceCaps & caps = _M_device->getCapabilities();
  unsigned maxSize = caps.max_texture_size;

  _M_pixel_width = w;
  _M_pixel_height = h;
  _M_pixel_depth = d;
  _M_format = format;
  _M_d3dformat = D3DFMT_UNKNOWN;

  if (_M_pixel_width > maxSize)
  {
	  _M_pixel_width = maxSize;
  }

  if (_M_pixel_height > maxSize)
  {
	  _M_pixel_height = maxSize;
  }

  if (!realize (numMips))
  {
    ATOM_LOGGER::error ("%s(0x%08X, %d, %d, %d, %d) Create texture failed.\n", __FUNCTION__, contents, w, h, d, format);
    return false;
  }

  if (!_M_texture)
  {
    ATOM_LOGGER::error ("%s(0x%08X, %d, %d, %d, %d) Create texture failed.\n", __FUNCTION__, contents, w, h, d, format);
	return false;
  }

  if (!contents)
  {
	_M_nummips = _M_texture->GetLevelCount ();

	return true;
  }

  D3DFORMAT srcformat;
  DWORD block_bytes = 0;

  switch ( format)
  {
  case ATOM_PIXEL_FORMAT_BGRA8888:
  case ATOM_PIXEL_FORMAT_RGBA8888:
    srcformat = D3DFMT_A8R8G8B8;
    break;
  case ATOM_PIXEL_FORMAT_BGRX8888:
  case ATOM_PIXEL_FORMAT_BGR888:
  case ATOM_PIXEL_FORMAT_RGBX8888:
  case ATOM_PIXEL_FORMAT_RGB888:
    srcformat = D3DFMT_R8G8B8;
    break;
  case ATOM_PIXEL_FORMAT_A8:
    srcformat = D3DFMT_A8;
    break;
  case ATOM_PIXEL_FORMAT_GREY8:
    srcformat = D3DFMT_L8;
    break;
  case ATOM_PIXEL_FORMAT_DXT1:
    srcformat = D3DFMT_DXT1;
    block_bytes = 8;
    break;
  case ATOM_PIXEL_FORMAT_DXT3:
    srcformat = D3DFMT_DXT3;
    block_bytes = 16;
    break;
  case ATOM_PIXEL_FORMAT_DXT5:
    srcformat = D3DFMT_DXT5;
    block_bytes = 16;
    break;
  case ATOM_PIXEL_FORMAT_R32F:
    srcformat = D3DFMT_R32F;
	break;
  case ATOM_PIXEL_FORMAT_RG32F:
	srcformat = D3DFMT_G32R32F;
	break;
  case ATOM_PIXEL_FORMAT_RGBA32F:
    srcformat = D3DFMT_A32B32G32R32F;
	break;
  case ATOM_PIXEL_FORMAT_R16F:
	srcformat = D3DFMT_R16F;
	break;
  case ATOM_PIXEL_FORMAT_RG16F:
    srcformat = D3DFMT_G16R16F;
	break;
  case ATOM_PIXEL_FORMAT_RGBA16F:
    srcformat = D3DFMT_A16B16G16R16F;
	break;
  default:
	ATOM_LOGGER::error ("%s(0x%08X, %d, %d, %d, %d) Unsupported pixel format.\n", __FUNCTION__, contents, w, h, d, format);
    return false;
  }

  if (_M_flags & ATOM_Texture::TEXTURE2D)
  {
    IDirect3DTexture9 *load_texture = 0;
    IDirect3DSurface9 *surface = 0;

    if (_M_d3dpool == D3DPOOL_DEFAULT)
    {
      HRESULT hr = ((IDirect3DTexture9*)_M_texture)->GetSurfaceLevel (0, &surface);
      if (FAILED(hr))
      {
		ATOM_LOGGER::error ("%s(0x%08X, %d, %d, %d, %d) GetSurfaceLevel() failed.\n", __FUNCTION__, contents, w, h, d, format);
		ATOM_CHECK_D3D_RESULT(hr);
        return false;
      }
      ATOM_ASSERT (surface);

      D3DSURFACE_DESC desc;
      hr = surface->GetDesc (&desc);
      surface->Release ();

      if (FAILED(hr))
      {
		ATOM_LOGGER::error ("%s(0x%08X, %d, %d, %d, %d) GetDesc() failed.\n", __FUNCTION__, contents, w, h, d, format);
		ATOM_CHECK_D3D_RESULT(hr);
        return false;
      }

      load_texture = internalCreateDummyTexture ((ATOM_D3D9Device*)_M_device, desc.Width, desc.Height, desc.Format, _M_texture->GetLevelCount());
      if (!load_texture)
      {
		ATOM_LOGGER::error ("%s(0x%08X, %d, %d, %d, %d) InternalCreateDummyTexture() failed.\n", __FUNCTION__, contents, w, h, d, format);
        return false;
      }
    }
    else
    {
      ATOM_VERIFY (_M_texture, NULL)();

      load_texture = (IDirect3DTexture9*)_M_texture;
      load_texture->AddRef();
    }

    ATOM_ASSERT (load_texture);

    if (ATOM_PIXEL_FORMAT(format)._is_compressed_format)
    {
      if (!_M_device->getCapabilities().texture_s3tc_compress)
      {
        releaseTexture (load_texture);
        return false;
      }

      size_t size = ATOM_max2(4, w)/4 * ATOM_max2(4, h)/4 * block_bytes;

      const unsigned char *bits = (const unsigned char*)contents;
      unsigned tmpW = w;
      unsigned tmpH = h;
      bool textureAssigned = false;

      RECT rc;
      rc.left = 0;
      rc.top = 0;
      rc.right = w;
      rc.bottom = h;

      HRESULT hr = load_texture->GetSurfaceLevel (0, &surface);
      if (FAILED(hr))
      {
		ATOM_LOGGER::error ("%s(0x%08X, %d, %d, %d, %d) GetSurfaceLevel() failed.\n", __FUNCTION__, contents, w, h, d, format);
		ATOM_CHECK_D3D_RESULT(hr);
        releaseTexture(load_texture);
        return false;
      }
      ATOM_ASSERT (surface);

	  //--- wangjian commented ---//
	  // 去除对D3DX依赖：使用LOCK/UNLOCK拷贝
      hr = D3DXLoadSurfaceFromMemory(
        surface,
        0,
        0,
        bits,
        srcformat,
        (srcformat == D3DFMT_DXT1) ? w*2 : w*4,
        0,
        &rc,
        D3DX_FILTER_NONE,
        0
      );

      surface->Release();
      surface = 0;

      if (FAILED(hr))
      {
		ATOM_LOGGER::error ("%s(0x%08X, %d, %d, %d, %d) D3DXLoadSurfaceFromMemory() failed.\n", __FUNCTION__, contents, w, h, d, format);
        ATOM_CHECK_D3D_RESULT(hr);
        releaseTexture(load_texture);
        return false;
      }
    }
    else
    {
      ATOM_BaseImage img;

      img.init (w, h, format, contents, 1);
      contents = img.getData();

      void *tmpContent = 0;
      switch ( format)
      {
      case ATOM_PIXEL_FORMAT_RGBA8888:
        contents = tmpContent = convert_RGBA_to_BGRA ((const unsigned char*)contents, w, h);
        format = ATOM_PIXEL_FORMAT_BGRA8888;
        break;
      case ATOM_PIXEL_FORMAT_RGBX8888:
        contents = tmpContent = convert_RGBX_to_BGR ((const unsigned char*)contents, w, h);
        format = ATOM_PIXEL_FORMAT_BGR888;
        break;
      case ATOM_PIXEL_FORMAT_RGB888:
        contents = tmpContent = convert_RGB_to_BGR ((const unsigned char*)contents, w, h);
        format = ATOM_PIXEL_FORMAT_BGR888;
        break;
      }

	  for (unsigned mip = 0; mip < load_texture->GetLevelCount (); ++mip)
	  {
		  int mipW = _M_pixel_width >> mip;
		  if (mipW < 1) mipW = 1;
		  int mipH = _M_pixel_height >> mip;
		  if (mipH < 1) mipH = 1;

		  if (mip != 0 || mipW != _M_pixel_width || mipH != _M_pixel_height)
		  {
			img.resize(mipW, mipH);
          }

		  HRESULT hr = load_texture->GetSurfaceLevel (mip, &surface);
		  if( FAILED(hr))
		  {
			ATOM_LOGGER::error ("%s(0x%08X, %d, %d, %d, %d) GetSurfaceLevel() failed.\n", __FUNCTION__, contents, w, h, d, format);
			ATOM_CHECK_D3D_RESULT(hr);
			if (tmpContent)
			{
				ATOM_FREE(tmpContent);
			}
			releaseTexture (load_texture);
			return false;
		  }

		  RECT rc;
		  rc.left = 0;
		  rc.top = 0;
		  rc.right = mipW;
		  rc.bottom = mipH;

		  //--- wangjian commented ---//
		  // 去除对D3DX依赖：使用LOCK/UNLOCK拷贝
		  hr = D3DXLoadSurfaceFromMemory(
			surface,
			0,
			0,
			contents,
			srcformat,
			mipW * ATOM_PIXEL_FORMAT(format).bpp/8,
			0,
			&rc,
			D3DX_FILTER_NONE,
			0
		  );

		  surface->Release ();
		  surface = 0;

		  if (FAILED(hr))
		  {
			ATOM_LOGGER::error ("%s(0x%08X, %d, %d, %d, %d) D3DXLoadSurfaceFromMemory() failed.\n", __FUNCTION__, contents, w, h, d, format);
			ATOM_CHECK_D3D_RESULT(hr);
			if (tmpContent)
			{	
				ATOM_FREE(tmpContent);
			}
			releaseTexture (load_texture);
			return false;
		  }
	  }
	  if (tmpContent)
	  {
		  ATOM_FREE(tmpContent);
	  }
    }

    if (load_texture != _M_texture)
    {
      HRESULT hr = ((ATOM_D3D9Device*)_M_device)->getD3DDevice()->UpdateTexture (load_texture, _M_texture);
      if (FAILED(hr))
      {
		ATOM_LOGGER::error ("%s(0x%08X, %d, %d, %d, %d) UpdateTexture() failed.\n", __FUNCTION__, contents, w, h, d, format);
		ATOM_CHECK_D3D_RESULT(hr);
        releaseTexture (load_texture);
        return false;
      }
    }

    releaseTexture (load_texture);
  }
  else if (_M_flags & ATOM_Texture::TEXTURE3D)
  {
    IDirect3DVolumeTexture9 *load_texture = 0;
	IDirect3DVolume9 *volume = 0;
    IDirect3DSurface9 *surface = 0;

    if (_M_d3dpool == D3DPOOL_DEFAULT)
    {
		ATOM_LOGGER::error ("%s(0x%08X, %d, %d, %d, %d) Create volume texture in default pool not supported.\n", __FUNCTION__, contents, w, h, d, format);
	}
    else
    {
	  IDirect3DVolume9 *volume;
	  HRESULT hr = ((IDirect3DVolumeTexture9*)_M_texture)->GetVolumeLevel (0, &volume);
	  if( FAILED(hr))
	  {
		ATOM_LOGGER::error ("%s(0x%08X, %d, %d, %d, %d) GetVolumeLevel() failed.\n", __FUNCTION__, contents, w, h, d, format);
		ATOM_CHECK_D3D_RESULT(hr);
		return false;
	  }

      ATOM_BaseImage img;

      img.init (w, h * d, format, contents, 1);
      contents = img.getData();

      void *tmpContent = 0;
      switch ( format)
      {
      case ATOM_PIXEL_FORMAT_RGBA8888:
        tmpContent = convert_RGBA_to_BGRA ((const unsigned char*)contents, w, h);
        format = ATOM_PIXEL_FORMAT_BGRA8888;
        break;
      case ATOM_PIXEL_FORMAT_RGBX8888:
        tmpContent = convert_RGBX_to_BGR ((const unsigned char*)contents, w, h);
        format = ATOM_PIXEL_FORMAT_BGR888;
        break;
      case ATOM_PIXEL_FORMAT_RGB888:
        tmpContent = convert_RGB_to_BGR ((const unsigned char*)contents, w, h);
        format = ATOM_PIXEL_FORMAT_BGR888;
        break;
      }

	  if (tmpContent)
	  {
		  img.init (w, h*d, format, tmpContent, 1);
		  ATOM_FREE(tmpContent);
		  tmpContent = 0;
	  }

      if (w != _M_pixel_width || h != _M_pixel_height || d != _M_pixel_depth)
      {
        img.resize(_M_pixel_width, _M_pixel_height * _M_pixel_depth);
      }

	  D3DBOX lockBox;
	  lockBox.Left = 0;
	  lockBox.Right = _M_pixel_width;
	  lockBox.Top = 0;
	  lockBox.Bottom = _M_pixel_height;
	  lockBox.Front = 0;
	  lockBox.Back = _M_pixel_depth;

	  unsigned rowPitch;
	  switch (srcformat)
	  {
	  case D3DFMT_DXT1:
		  rowPitch = _M_pixel_width * 2;
		  break;
	  case D3DFMT_DXT3:
	  case D3DFMT_DXT5:
		  rowPitch = _M_pixel_width * 4;
		  break;
	  default:
		  rowPitch = _M_pixel_width * ATOM_PIXEL_FORMAT(format).bpp/8;
		  break;
	  };

	  unsigned slicePitch = rowPitch * _M_pixel_height;

	  hr = D3DXLoadVolumeFromMemory (volume, NULL, NULL, img.getData(), srcformat, rowPitch, slicePitch, NULL, &lockBox, D3DX_FILTER_NONE, 0);
	  if (FAILED(hr))
	  {
		ATOM_LOGGER::error ("%s(0x%08X, %d, %d, %d, %d) D3DXLoadVolumeFromMemory() failed.\n", __FUNCTION__, contents, w, h, d, format);
		ATOM_CHECK_D3D_RESULT(hr);
		return false;
	  }

      volume->Release ();
      volume = 0;
    }
  }
  else
  {
	ATOM_LOGGER::error ("%s(0x%08X, %d, %d, %d) Not implemented.\n", __FUNCTION__, contents, w, h, format);
    return false;
  }

  _M_nummips = _M_texture->GetLevelCount ();

  return true;
}

bool ATOM_D3DTexture::updateTexImage(const void *contents, int offsetx,int offsety, int w, int h, ATOM_PixelFormat format)
{
  ATOM_STACK_TRACE(ATOM_D3DTexture::updateTexImage);
  if (!_M_texture)
  {
    return false;
  }

  IDirect3DSurface9 *surface = 0;

  D3DFORMAT srcformat;
  DWORD pitch;

  switch ( format)
  {
  case ATOM_PIXEL_FORMAT_RGBA8888:
    srcformat = D3DFMT_A8B8G8R8;
    pitch = 4 * w;
    break;
  case ATOM_PIXEL_FORMAT_RGBX8888:
    srcformat = D3DFMT_X8B8G8R8;
    pitch = 4 * w;
    break;
  case ATOM_PIXEL_FORMAT_BGRA8888:
    srcformat = D3DFMT_A8R8G8B8;
    pitch = 4 * w;
    break;
  case ATOM_PIXEL_FORMAT_BGRX8888:
    srcformat = D3DFMT_X8R8G8B8;
    pitch = 4 * w;
    break;
  case ATOM_PIXEL_FORMAT_BGR888:
    srcformat = D3DFMT_R8G8B8;
    pitch = 3 * w;
    break;
  case ATOM_PIXEL_FORMAT_A8:
    srcformat = D3DFMT_A8;
    pitch = w;
    break;
  case ATOM_PIXEL_FORMAT_GREY8:
    srcformat = D3DFMT_L8;
    pitch = w;
    break;
  case ATOM_PIXEL_FORMAT_GREY8A8:
    srcformat = D3DFMT_A8L8;
    pitch = 2 * w;
    break;
  case ATOM_PIXEL_FORMAT_R32F:
    srcformat = D3DFMT_R32F;
	pitch = 4 * w;
	break;
  case ATOM_PIXEL_FORMAT_RG32F:
	srcformat = D3DFMT_G32R32F;
	pitch = 8 * w;
	break;
  case ATOM_PIXEL_FORMAT_RGBA32F:
    srcformat = D3DFMT_A32B32G32R32F;
	pitch = 16 * w;
	break;
  case ATOM_PIXEL_FORMAT_R16F:
	srcformat = D3DFMT_R16F;
	pitch = 2 * w;
	break;
  case ATOM_PIXEL_FORMAT_RG16F:
    srcformat = D3DFMT_G16R16F;
	pitch = 4 * w;
	break;
  case ATOM_PIXEL_FORMAT_RGBA16F:
    srcformat = D3DFMT_A16B16G16R16F;
	pitch = 8 * w;
	break;
  default:
    ATOM_LOGGER::error("%s(0x%08X, %d, %d, %d, %d, %d) Invalid source format.\n", __FUNCTION__, contents, offsetx, offsety, w, h, format);
    return false;
  }

  HRESULT hr = getSurfaceLevel (0, &surface);
  if (FAILED(hr))
  {
    ATOM_LOGGER::error("%s(0x%08X, %d, %d, %d, %d, %d) GetSurfaceLevel failed.\n", __FUNCTION__, contents, offsetx, offsety, w, h, format);
	ATOM_CHECK_D3D_RESULT(hr);
    return false;
  }

  RECT rcSrc, rcDst;
  rcSrc.left = 0;
  rcSrc.top = 0;
  rcSrc.right = w;
  rcSrc.bottom = h;

  rcDst.left = offsetx;
  rcDst.top = offsety;
  rcDst.right = offsetx + w;
  rcDst.bottom = offsety + h;

  hr = D3DXLoadSurfaceFromMemory(
    surface,
    0,
    &rcDst,
    contents,
    srcformat,
    pitch,
    0,
    &rcSrc,
    D3DX_FILTER_NONE,
    0
  );

  surface->Release ();
  surface = 0;

  if (FAILED(hr))
  {
    ATOM_LOGGER::error("%s(0x%08X, %d, %d, %d, %d, %d) D3DXLoadSurfaceFromMemory failed.\n", __FUNCTION__, contents, offsetx, offsety, w, h, format);
    ATOM_CHECK_D3D_RESULT(hr);
    return false;
  }

  return true;
}

bool ATOM_D3DTexture::copyTexImage(ATOM_PixelFormat format, int x, int y, int w, int h)
{
  ATOM_STACK_TRACE(ATOM_D3DTexture::copyTexImage);
  //ATOM_LOGGER::error ("ATOM_D3DTexture::CopyTexImage: Not implemented.\n");

  return copySubTexImage (0, 0, x, y, w, h);
}

bool ATOM_D3DTexture::copySubTexImage(int offsetx, int offsety, int x, int y, int w, int h)
{
  ATOM_STACK_TRACE(ATOM_D3DTexture::copySubTexImage);
  HRESULT hr;
  //ATOM_LOGGER::error ("ATOM_D3DTexture::CopySubTexImage: Not implemented.\n");

  IDirect3DDevice9 *device = ((ATOM_D3D9Device*)_M_device)->getD3DDevice();
  IDirect3DSurface9 *pBackSuf = NULL;
  hr = device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackSuf);
  if (FAILED (hr))
  {
    ATOM_LOGGER::error("%s(%d, %d, %d, %d, %d, %d) GetBackBuffer() failed.\n", __FUNCTION__, offsetx, offsety, x, y, w, h);
    ATOM_CHECK_D3D_RESULT(hr);
    return false;
  }

  D3DSURFACE_DESC desc;
  hr = getLevelDesc(0, &desc);
  if (FAILED (hr))
  {
    ATOM_LOGGER::error("%s(%d, %d, %d, %d, %d, %d) GetLevelDesc() failed.\n", __FUNCTION__, offsetx, offsety, x, y, w, h);
	ATOM_CHECK_D3D_RESULT(hr);
    return false;
  }

  IDirect3DSurface9 *pThisSuf = NULL;
  hr = getSurfaceLevel(0, &pThisSuf);
  if (FAILED (hr))
  {
    ATOM_LOGGER::error("%s(%d, %d, %d, %d, %d, %d) GetSurfaceLevel() failed.\n", __FUNCTION__, offsetx, offsety, x, y, w, h);
	ATOM_CHECK_D3D_RESULT(hr);
    return false;
  }

  RECT BackRect;
  BackRect.left   = x;
  BackRect.right  = x + w;
  BackRect.top    = y;
  BackRect.bottom = y + h;

  RECT ThisRect;
  ThisRect.left   = offsetx;
  ThisRect.right  = desc.Width;
  ThisRect.top    = offsety;
  ThisRect.bottom = desc.Height;

  hr = D3DXLoadSurfaceFromSurface (pThisSuf, NULL, &ThisRect, pBackSuf, NULL, &BackRect, D3DX_FILTER_NONE, 0);

  if (FAILED (hr))
  {
    ATOM_LOGGER::error("%s(%d, %d, %d, %d, %d, %d) D3DXLoadSurfaceFromSurface() failed.\n", __FUNCTION__, offsetx, offsety, x, y, w, h);
    ATOM_CHECK_D3D_RESULT(hr);
    return false;
  }

  pThisSuf->Release();
  pBackSuf->Release();

  return true;
}

void ATOM_D3DTexture::getTexImage(ATOM_PixelFormat format, void *data)
{
  ATOM_STACK_TRACE(ATOM_D3DTexture::getTexImage);
  HRESULT hr;
  //ATOM_LOGGER::error ("ATOM_D3DTexture::GetTexImage: Not implemented.\n");
  ATOM_PixelFormat readformat;
  unsigned compsize;

  //fixme:hero need more format
  D3DFORMAT d3dfmt = D3DFMT_UNKNOWN;
  switch (format)
  {
  case ATOM_PIXEL_FORMAT_RGBA8888:
  case ATOM_PIXEL_FORMAT_BGRA8888:
  case ATOM_PIXEL_FORMAT_A8:
    readformat = ATOM_PIXEL_FORMAT_BGRA8888;
    compsize = 4;
    d3dfmt = D3DFMT_A8R8G8B8;
    break;
  case ATOM_PIXEL_FORMAT_RGB888:
  case ATOM_PIXEL_FORMAT_BGR888:
  case ATOM_PIXEL_FORMAT_RGBX8888:
  case ATOM_PIXEL_FORMAT_BGRX8888:
  case ATOM_PIXEL_FORMAT_GREY8:
    readformat = ATOM_PIXEL_FORMAT_BGRX8888;
    compsize = 4;
    d3dfmt = D3DFMT_X8R8G8B8;
    break;
  case ATOM_PIXEL_FORMAT_RGBA32F:
	readformat = format;
	compsize = 16;
	d3dfmt = D3DFMT_A32B32G32R32F;
	break;
  case ATOM_PIXEL_FORMAT_R32F:
	readformat = format;
	compsize = 4;
	d3dfmt = D3DFMT_R32F;
	break;
  case ATOM_PIXEL_FORMAT_RG32F:
	readformat = format;
	compsize = 8;
	d3dfmt = D3DFMT_G32R32F;
	break;
  case ATOM_PIXEL_FORMAT_RGBA16F:
	readformat = format;
	compsize = 8;
	d3dfmt = D3DFMT_A16B16G16R16F;
	break;
  case ATOM_PIXEL_FORMAT_R16F:
	readformat = format;
	compsize = 2;
	d3dfmt = D3DFMT_R16F;
	break;
  case ATOM_PIXEL_FORMAT_RG16F:
	readformat = format;
	compsize = 4;
	d3dfmt = D3DFMT_G16R16F;
	break;
  default:
    ATOM_LOGGER::error("%s(%d, 0x%08X) Invalid format.\n", __FUNCTION__, format, data);
    return;
  }

  if (!_M_texture)
  {
    restore ();
  }

  if (!_M_texture)
  {
    ATOM_LOGGER::error("%s(%d, 0x%08X) NULL texture.\n", __FUNCTION__, format, data);
    return;
  }

  IDirect3DDevice9 *device = ((ATOM_D3D9Device*)_M_device)->getD3DDevice();

  D3DSURFACE_DESC desc;
  hr = getLevelDesc(0, &desc);
  if (FAILED(hr))
  {
    ATOM_LOGGER::error("%s(%d, 0x%08X) GetLevelDesc() failed.\n", __FUNCTION__, format, data);
	ATOM_CHECK_D3D_RESULT(hr);
    return;
  }

  unsigned imagestride = desc.Width * compsize;
  ATOM_BaseImage tmpimage(desc.Width, desc.Height, readformat, 0, 1);

  IDirect3DSurface9 *pDstSurf = NULL;
  hr = device->CreateOffscreenPlainSurface (desc.Width, desc.Height, d3dfmt, D3DPOOL_SYSTEMMEM, &pDstSurf, NULL);
  if (FAILED(hr))
  {
    ATOM_LOGGER::error("%s(%d, 0x%08X) CreateOffscreenPlainSurface() failed.\n", __FUNCTION__, format, data);
	ATOM_CHECK_D3D_RESULT(hr);
    return;
  }

  IDirect3DSurface9 *pSrcSurf = NULL;
  hr = getSurfaceLevel (0, &pSrcSurf);
  if (FAILED(hr))
  {
    ATOM_LOGGER::error("%s(%d, 0x%08X) GetSurfaceLevel() failed.\n", __FUNCTION__, format, data);
    pDstSurf->Release();
	ATOM_CHECK_D3D_RESULT(hr);
    return;
  }

  hr = D3DXLoadSurfaceFromSurface (pDstSurf, NULL, NULL, pSrcSurf, NULL, NULL, D3DX_FILTER_NONE, 0);
  if (FAILED(hr))
  {
    ATOM_LOGGER::error("%s(%d, 0x%08X) D3DXLoadSurfaceFromSurface() failed.\n", __FUNCTION__, format, data);
    pDstSurf->Release();
    pSrcSurf->Release();
	ATOM_CHECK_D3D_RESULT(hr);
    return;
  }

  D3DLOCKED_RECT lockRect;
  hr = pDstSurf->LockRect(&lockRect, NULL, D3DLOCK_READONLY);
  if (FAILED(hr))
  {
    ATOM_LOGGER::error("%s(%d, 0x%08X) lockRect() failed.\n", __FUNCTION__, format, data);
    pDstSurf->Release();
    pSrcSurf->Release();
	ATOM_CHECK_D3D_RESULT(hr);
    return;
  }

  unsigned char *dstbuf = (unsigned char*)tmpimage.getData();
  unsigned char *srcbuf = (unsigned char*)lockRect.pBits;
  for (unsigned i = 0; i < desc.Height; ++i)
  {
    memcpy (dstbuf, srcbuf, imagestride);
    dstbuf += imagestride;
    srcbuf += lockRect.Pitch;
  }

  pDstSurf->UnlockRect();
  pDstSurf->Release();
  pSrcSurf->Release();

  tmpimage.convertFormat (format);
  memcpy (data, tmpimage.getData(), tmpimage.getBufferSize());
}

bool ATOM_D3DTexture::realize (unsigned numMips)
{
	ATOM_STACK_TRACE(ATOM_D3DTexture::realize);
	if (!_M_texture)
	{
	  ATOM_D3D9Device *device = (ATOM_D3D9Device*)_M_device;
	  ATOM_ASSERT (device);

	  if (_M_flags & ATOM_Texture::TEXTURE2D)
	  {
		  if (_M_format == ATOM_PIXEL_FORMAT_DEPTHSTENCIL)
		  {
			  //--- wangjian modified ---//
			  // 当不改变backbuffer大小（非resize）情况下，如果device丢失的话，
			  // depth texture在恢复的时候会到这里重新创建，而bUseDX9DSFormat标记默认为非，在使用硬件阴影时使用了此标记，这里如果不设置的话
			  // 会导致阴影丢失的情况，所以在此重新检查一下格式。
			  // TODO:不使用D3D格式比较的方式而是用一个标记记录本来的格式用来判断。
			  bool bUseDX9DSFormat = false;
			  if( _M_d3dformat == device->getD3DPresentParams()->AutoDepthStencilFormat )
				  bUseDX9DSFormat = true;
			  if (!createAsDepthTexture (_M_pixel_width, _M_pixel_height, bUseDX9DSFormat))
			  {
				  return false;
			  }
			  //-------------------------//
		  }
		  else if (!internalCreateTexture(device, _M_pixel_width, _M_pixel_height, _M_format, numMips))
		  {
			return false;
		  }
	  }
	  else if (_M_flags & ATOM_Texture::CUBEMAP)
	  {
		if (!internalCreateCubeTexture(device, _M_pixel_width, _M_format, numMips))
		  return false;
	  }
	  else if (_M_flags & ATOM_Texture::TEXTURE3D)
	  {
		if (!internalCreateVolumeTexture(device, _M_pixel_width, _M_pixel_height, _M_pixel_depth, _M_format, numMips))
		  return false;
	  }
	  else
	  {
		ATOM_LOGGER::error ("%s() Uunknown texture flags.\n", __FUNCTION__);
		return false;
	  }
	}

	return _M_texture != 0;
}

HRESULT ATOM_D3DTexture::getLevelDesc (int nLevel, D3DSURFACE_DESC *pDesc)
{
  ATOM_STACK_TRACE(ATOM_D3DTexture::getLevelDesc);
  HRESULT hr;

  if (!_M_texture)
  {
	  return E_FAIL;
  }

  D3DRESOURCETYPE type = _M_texture->GetType();
  if (type == D3DRTYPE_TEXTURE)
  {
    hr = ((IDirect3DTexture9*)_M_texture)->GetLevelDesc(nLevel, pDesc);
  }
  else if (type == D3DRTYPE_VOLUMETEXTURE)
  {
	D3DVOLUME_DESC desc;
	IDirect3DVolume9 *volume = 0;
	hr = ((IDirect3DVolumeTexture9*)_M_texture)->GetVolumeLevel(0, &volume);
	if (FAILED(hr))
	{
		ATOM_CHECK_D3D_RESULT(hr);
		return hr;
	}
	hr = volume->GetDesc (&desc);
	volume->Release();
	if (FAILED(hr))
	{
		ATOM_CHECK_D3D_RESULT(hr);
		return hr;
	}
	pDesc->Format = desc.Format;
	pDesc->Width = desc.Width;
	pDesc->Height = desc.Height;
	pDesc->Usage = desc.Usage;
	pDesc->Pool = desc.Pool;
	pDesc->Type = desc.Type;
	pDesc->MultiSampleQuality = 0;
	pDesc->MultiSampleType = D3DMULTISAMPLE_NONE;
  }
  else if (type == D3DRTYPE_CUBETEXTURE)
  {
    hr = ((IDirect3DCubeTexture9*)_M_texture)->GetLevelDesc(nLevel, pDesc);
  }

  return ATOM_CHECK_D3D_RESULT(hr);
}

HRESULT ATOM_D3DTexture::getSurfaceLevel (int nLevel, IDirect3DSurface9 **ppSurface, unsigned face/* = -1*/)
{
  ATOM_STACK_TRACE(ATOM_D3DTexture::getSurfaceLevel);
  HRESULT hr;

  if (!_M_texture)
  {
    return E_FAIL;
  }

  D3DRESOURCETYPE type = _M_texture->GetType();
  if (type == D3DRTYPE_TEXTURE)
  {
	hr = ((IDirect3DTexture9*)_M_texture)->GetSurfaceLevel(nLevel, ppSurface);
  }
  else if (type == D3DRTYPE_CUBETEXTURE)
  {
	  //--- wangjian modified ---//
	  if( face != -1 )
		hr = ((IDirect3DCubeTexture9*)_M_texture)->GetCubeMapSurface (/*D3DCUBEMAP_FACE_POSITIVE_X*/(D3DCUBEMAP_FACES)face, nLevel, ppSurface);
	  //-------------------------//
  }

  return ATOM_CHECK_D3D_RESULT(hr);
}

bool ATOM_D3DTexture::saveToFile (const char *filename)
{
	ATOM_STACK_TRACE(ATOM_D3DTexture::saveToFile);
	if (filename)
	{
		if (!_M_texture)
		{
			return false;
		}

		int imageType = ATOM_ImageCodec::guessTypeByFileName (filename);
		D3DXIMAGE_FILEFORMAT format;
		switch (imageType)
		{
		case ATOM_ImageCodec::JPEG:
			format = D3DXIFF_JPG;
			break;
		case ATOM_ImageCodec::PNG:
			format = D3DXIFF_PNG;
			break;
		case ATOM_ImageCodec::BMP:
			format = D3DXIFF_BMP;
			break;
		case ATOM_ImageCodec::DDS:
			format = D3DXIFF_DDS;
			break;
		default:
			return false;
		}

		D3DSURFACE_DESC surfaceDesc;
		getLevelDesc (0, &surfaceDesc);
		D3DFORMAT sourcFormat = surfaceDesc.Format;
		D3DFORMAT destFormat;
		IDirect3DBaseTexture9 *texture = _M_texture;

		bool canDoDXTn = false;
		bool isDXTn = false;
		switch (sourcFormat)
		{
		case D3DFMT_DXT1:
		case D3DFMT_DXT2:
		case D3DFMT_DXT3:
		case D3DFMT_DXT4:
		case D3DFMT_DXT5:
			canDoDXTn = true;
			isDXTn = true;
			destFormat = sourcFormat;
			break;
		case D3DFMT_R8G8B8:
		case D3DFMT_X8R8G8B8:
		case D3DFMT_R5G6B5:
		case D3DFMT_X1R5G5B5:
		case D3DFMT_R3G3B2:
		case D3DFMT_X4R4G4B4:
		case D3DFMT_X8B8G8R8:
		case D3DFMT_L8:
			canDoDXTn = true;
			destFormat = D3DFMT_DXT1;
			break;
		case D3DFMT_A8R8G8B8:
		case D3DFMT_A1R5G5B5:
		case D3DFMT_A4R4G4B4:
		case D3DFMT_A8:
		case D3DFMT_A8R3G3B2:
		case D3DFMT_A8B8G8R8:
		case D3DFMT_A8L8:
		case D3DFMT_A4L4:
			canDoDXTn = true;
			destFormat = D3DFMT_DXT5;
			break;
		}

		D3DRESOURCETYPE type = _M_texture->GetType ();
		if (D3DRTYPE_TEXTURE == type && !isDXTn && canDoDXTn && format == D3DXIFF_DDS)
		{
			IDirect3DTexture9 *newTexture = 0;
			HRESULT hr = D3DXCreateTexture(((ATOM_D3D9Device*)_M_device)->getD3DDevice(), surfaceDesc.Width, surfaceDesc.Height, 1, 0, destFormat, D3DPOOL_MANAGED, &newTexture);
			if (SUCCEEDED(hr))
			{
				IDirect3DSurface9 *sourceSurface = 0;
				if (SUCCEEDED(getSurfaceLevel (0, &sourceSurface)))
				{
					IDirect3DSurface9 *mainSurface = 0;
					hr = newTexture->GetSurfaceLevel (0, &mainSurface);
					if (SUCCEEDED(hr))
					{
						HRESULT hr = D3DXLoadSurfaceFromSurface (mainSurface, 0, 0, sourceSurface, 0, 0, D3DX_FILTER_NONE, 0);
						if (SUCCEEDED(hr))
						{
							texture = newTexture;
						}
						mainSurface->Release ();
					}
					sourceSurface->Release ();
				}
			}
			ATOM_CHECK_D3D_RESULT(hr);
		}

		LPD3DXBUFFER buffer = 0;
		bool ret = false;
		HRESULT hr = D3DXSaveTextureToFileInMemory (&buffer, format, texture, 0);
		if (SUCCEEDED(hr))
		{
			ATOM_AutoFile f(filename, ATOM_VFS::write);
			if (f)
			{
				f->write (buffer->GetBufferPointer(), buffer->GetBufferSize());
				ret = true;
			}
		}
		ATOM_CHECK_D3D_RESULT(hr);

		if (buffer)
		{
			buffer->Release ();
		}

		if (texture != _M_texture)
		{
			texture->Release ();
		}

		return ret;
	}

	return false;
}

bool ATOM_D3DTexture::saveToFileEx (const char *filename, ATOM_PixelFormat format)
{
	ATOM_STACK_TRACE(ATOM_D3DTexture::saveToFileEx);
	if (filename)
	{
		if (!_M_texture)
		{
			return false;
		}

		int imageType = ATOM_ImageCodec::guessTypeByFileName (filename);
		D3DXIMAGE_FILEFORMAT format_image;
		switch (imageType)
		{
		case ATOM_ImageCodec::JPEG:
			format_image = D3DXIFF_JPG;
			break;
		case ATOM_ImageCodec::PNG:
			format_image = D3DXIFF_PNG;
			break;
		case ATOM_ImageCodec::BMP:
			format_image = D3DXIFF_BMP;
			break;
		case ATOM_ImageCodec::DDS:
			format_image = D3DXIFF_DDS;
			break;
		default:
			return false;
		}

		D3DSURFACE_DESC surfaceDesc;
		getLevelDesc (0, &surfaceDesc);
		D3DFORMAT sourcFormat = surfaceDesc.Format;
		D3DFORMAT destFormat;
		IDirect3DBaseTexture9 *texture = _M_texture;

		// wangjian modified
		bool isDXTn = false;
		switch (format)
		{
		case ATOM_PIXEL_FORMAT_BGRA8888:
			destFormat = D3DFMT_A8R8G8B8;
			break;
		case ATOM_PIXEL_FORMAT_BGRX8888:
		case ATOM_PIXEL_FORMAT_BGR888:
			destFormat = D3DFMT_R8G8B8;
			break;
		case ATOM_PIXEL_FORMAT_DXT1:
			//isDXTn = true;
			destFormat = D3DFMT_DXT1;
			break;
		case ATOM_PIXEL_FORMAT_DXT3:
			//isDXTn = true;
			destFormat = D3DFMT_DXT3;
			break;
		case ATOM_PIXEL_FORMAT_DXT5:
			//isDXTn = true;
			destFormat = D3DFMT_DXT5;
			break;
		default:
			destFormat = D3DFMT_A8R8G8B8;
			break;
		};

		bool canDoDXTn = false;
		switch (sourcFormat)
		{
		case D3DFMT_DXT1:
		case D3DFMT_DXT2:
		case D3DFMT_DXT3:
		case D3DFMT_DXT4:
		case D3DFMT_DXT5:
			canDoDXTn = true;
			isDXTn = true;
			break;
		case D3DFMT_R8G8B8:
		case D3DFMT_X8R8G8B8:
		case D3DFMT_R5G6B5:
		case D3DFMT_X1R5G5B5:
		case D3DFMT_R3G3B2:
		case D3DFMT_X4R4G4B4:
		case D3DFMT_X8B8G8R8:
		case D3DFMT_L8:
			canDoDXTn = true;
			break;
		case D3DFMT_A8R8G8B8:
		case D3DFMT_A1R5G5B5:
		case D3DFMT_A4R4G4B4:
		case D3DFMT_A8:
		case D3DFMT_A8R3G3B2:
		case D3DFMT_A8B8G8R8:
		case D3DFMT_A8L8:
		case D3DFMT_A4L4:
			canDoDXTn = true;
			break;
		}

		D3DRESOURCETYPE type = _M_texture->GetType ();
		if (D3DRTYPE_TEXTURE == type && !isDXTn && canDoDXTn)
		{
			IDirect3DTexture9 *newTexture = 0;
			HRESULT hr = D3DXCreateTexture(((ATOM_D3D9Device*)_M_device)->getD3DDevice(), surfaceDesc.Width, surfaceDesc.Height, 1, 0, destFormat, D3DPOOL_MANAGED, &newTexture);
			if (SUCCEEDED(hr))
			{
				IDirect3DSurface9 *sourceSurface = 0;
				if (SUCCEEDED(getSurfaceLevel (0, &sourceSurface)))
				{
					IDirect3DSurface9 *mainSurface = 0;
					hr = newTexture->GetSurfaceLevel (0, &mainSurface);
					if (SUCCEEDED(hr))
					{
						HRESULT hr = D3DXLoadSurfaceFromSurface (mainSurface, 0, 0, sourceSurface, 0, 0, D3DX_FILTER_NONE, 0);
						if (SUCCEEDED(hr))
						{
							texture = newTexture;
						}
						mainSurface->Release ();
					}
					sourceSurface->Release ();
				}
			}
			ATOM_CHECK_D3D_RESULT(hr);
		}

		LPD3DXBUFFER buffer = 0;
		bool ret = false;
		HRESULT hr = D3DXSaveTextureToFileInMemory (&buffer, format_image, texture, 0);
		if (SUCCEEDED(hr))
		{
			ATOM_AutoFile f(filename, ATOM_VFS::write);
			if (f)
			{
				f->write (buffer->GetBufferPointer(), buffer->GetBufferSize());
				ret = true;
			}
		}
		ATOM_CHECK_D3D_RESULT(hr);

		if (buffer)
		{
			buffer->Release ();
		}

		if (texture != _M_texture)
		{
			texture->Release ();
		}

		return ret;
	}

	return false;
}

void *ATOM_D3DTexture::saveToMem (ATOM_PixelFormat fileformat, unsigned *size)
{
	ATOM_STACK_TRACE(ATOM_D3DTexture::saveToMem);
	if (!_M_texture)
	{
		return 0;
	}

	D3DSURFACE_DESC surfaceDesc;
	getLevelDesc (0, &surfaceDesc);
	D3DFORMAT sourcFormat = surfaceDesc.Format;
	D3DFORMAT destFormat;
	IDirect3DBaseTexture9 *texture = _M_texture;

	D3DXIMAGE_FILEFORMAT d3dfileFormat;
	switch (fileformat)
	{
	case ATOM_ImageCodec::JPEG:
		d3dfileFormat = D3DXIFF_JPG;
		break;
	case ATOM_ImageCodec::PNG:
		d3dfileFormat = D3DXIFF_PNG;
		break;
	case ATOM_ImageCodec::BMP:
		d3dfileFormat = D3DXIFF_BMP;
		break;
	case ATOM_ImageCodec::DDS:
		d3dfileFormat = D3DXIFF_DDS;
		break;
	default:
		return false;
	}

	bool canDoDXTn = false;
	bool isDXTn = false;
	switch (sourcFormat)
	{
	case D3DFMT_DXT1:
	case D3DFMT_DXT2:
	case D3DFMT_DXT3:
	case D3DFMT_DXT4:
	case D3DFMT_DXT5:
		canDoDXTn = true;
		isDXTn = true;
		destFormat = sourcFormat;
		break;
	case D3DFMT_R8G8B8:
	case D3DFMT_X8R8G8B8:
	case D3DFMT_R5G6B5:
	case D3DFMT_X1R5G5B5:
	case D3DFMT_R3G3B2:
	case D3DFMT_X4R4G4B4:
	case D3DFMT_X8B8G8R8:
	case D3DFMT_L8:
		canDoDXTn = true;
		destFormat = D3DFMT_DXT1;
		break;
	case D3DFMT_A8R8G8B8:
	case D3DFMT_A1R5G5B5:
	case D3DFMT_A4R4G4B4:
	case D3DFMT_A8:
	case D3DFMT_A8R3G3B2:
	case D3DFMT_A8B8G8R8:
	case D3DFMT_A8L8:
	case D3DFMT_A4L4:
		canDoDXTn = true;
		destFormat = D3DFMT_DXT5;
		break;
	default:
		return 0;
	}

	D3DRESOURCETYPE type = _M_texture->GetType ();
	if (D3DRTYPE_TEXTURE == type && !isDXTn && canDoDXTn)
	{
		IDirect3DTexture9 *newTexture = 0;
		HRESULT hr = D3DXCreateTexture(((ATOM_D3D9Device*)_M_device)->getD3DDevice(), surfaceDesc.Width, surfaceDesc.Height, 1, 0, destFormat, D3DPOOL_MANAGED, &newTexture);
		if (SUCCEEDED(hr))
		{
			IDirect3DSurface9 *sourceSurface = 0;
			if (SUCCEEDED(getSurfaceLevel (0, &sourceSurface)))
			{
				IDirect3DSurface9 *mainSurface = 0;
				hr = newTexture->GetSurfaceLevel (0, &mainSurface);
				if (SUCCEEDED(hr))
				{
					HRESULT hr = D3DXLoadSurfaceFromSurface (mainSurface, 0, 0, sourceSurface, 0, 0, D3DX_FILTER_NONE, 0);
					if (SUCCEEDED(hr))
					{
						texture = newTexture;
					}
					mainSurface->Release ();
				}
				sourceSurface->Release ();
			}
		}
		ATOM_CHECK_D3D_RESULT(hr);
	}

	LPD3DXBUFFER buffer = 0;
	void *ret = 0;
	HRESULT hr = D3DXSaveTextureToFileInMemory (&buffer, d3dfileFormat, texture, 0);
	if (SUCCEEDED(hr))
	{
		ret = ATOM_MALLOC(buffer->GetBufferSize());
		if (!ret)
		{
			return 0;
		}
		if (size)
		{
			*size = buffer->GetBufferSize();
		}
		memcpy (ret, buffer->GetBufferPointer(), buffer->GetBufferSize());
	}
	ATOM_CHECK_D3D_RESULT(hr);

	if (buffer)
	{
		buffer->Release ();
	}

	if (texture != _M_texture)
	{
		texture->Release ();
	}

	return ret;
}

void *ATOM_D3DTexture::saveToMemEx (ATOM_PixelFormat format, int fileformat, unsigned *size)
{
	ATOM_STACK_TRACE(ATOM_D3DTexture::saveToMemEx);
	if (!_M_texture)
	{
		return false;
	}

	D3DSURFACE_DESC surfaceDesc;
	getLevelDesc (0, &surfaceDesc);
	D3DFORMAT sourcFormat = surfaceDesc.Format;
	D3DFORMAT destFormat;
	IDirect3DBaseTexture9 *texture = _M_texture;

	D3DXIMAGE_FILEFORMAT d3dfileFormat;
	switch (fileformat)
	{
	case ATOM_ImageCodec::JPEG:
		d3dfileFormat = D3DXIFF_JPG;
		break;
	case ATOM_ImageCodec::PNG:
		d3dfileFormat = D3DXIFF_PNG;
		break;
	case ATOM_ImageCodec::BMP:
		d3dfileFormat = D3DXIFF_BMP;
		break;
	case ATOM_ImageCodec::DDS:
		d3dfileFormat = D3DXIFF_DDS;
		break;
	default:
		return false;
	}

	bool isDXTn = false;
	switch (format)
	{
	case ATOM_PIXEL_FORMAT_BGRA8888:
		destFormat = D3DFMT_A8R8G8B8;
		break;
	case ATOM_PIXEL_FORMAT_BGRX8888:
	case ATOM_PIXEL_FORMAT_BGR888:
		destFormat = D3DFMT_R8G8B8;
		break;
	case ATOM_PIXEL_FORMAT_DXT1:
		isDXTn = true;
		destFormat = D3DFMT_DXT1;
		break;
	case ATOM_PIXEL_FORMAT_DXT3:
		isDXTn = true;
		destFormat = D3DFMT_DXT3;
		break;
	case ATOM_PIXEL_FORMAT_DXT5:
		isDXTn = true;
		destFormat = D3DFMT_DXT5;
		break;
	default:
		destFormat = D3DFMT_A8R8G8B8;
		break;
	};

	bool canDoDXTn = false;
	switch (sourcFormat)
	{
	case D3DFMT_DXT1:
	case D3DFMT_DXT2:
	case D3DFMT_DXT3:
	case D3DFMT_DXT4:
	case D3DFMT_DXT5:
		canDoDXTn = true;
		isDXTn = true;
		break;
	case D3DFMT_R8G8B8:
	case D3DFMT_X8R8G8B8:
	case D3DFMT_R5G6B5:
	case D3DFMT_X1R5G5B5:
	case D3DFMT_R3G3B2:
	case D3DFMT_X4R4G4B4:
	case D3DFMT_X8B8G8R8:
	case D3DFMT_L8:
		canDoDXTn = true;
		break;
	case D3DFMT_A8R8G8B8:
	case D3DFMT_A1R5G5B5:
	case D3DFMT_A4R4G4B4:
	case D3DFMT_A8:
	case D3DFMT_A8R3G3B2:
	case D3DFMT_A8B8G8R8:
	case D3DFMT_A8L8:
	case D3DFMT_A4L4:
		canDoDXTn = true;
		break;
	}

	D3DRESOURCETYPE type = _M_texture->GetType ();
	if (D3DRTYPE_TEXTURE == type && !isDXTn && canDoDXTn)
	{
		IDirect3DTexture9 *newTexture = 0;
		HRESULT hr = D3DXCreateTexture(((ATOM_D3D9Device*)_M_device)->getD3DDevice(), surfaceDesc.Width, surfaceDesc.Height, 1, 0, destFormat, D3DPOOL_MANAGED, &newTexture);
		if (SUCCEEDED(hr))
		{
			IDirect3DSurface9 *sourceSurface = 0;
			if (SUCCEEDED(getSurfaceLevel (0, &sourceSurface)))
			{
				IDirect3DSurface9 *mainSurface = 0;
				hr = newTexture->GetSurfaceLevel (0, &mainSurface);
				if (SUCCEEDED(hr))
				{
					HRESULT hr = D3DXLoadSurfaceFromSurface (mainSurface, 0, 0, sourceSurface, 0, 0, D3DX_FILTER_NONE, 0);
					if (SUCCEEDED(hr))
					{
						texture = newTexture;
					}
					mainSurface->Release ();
				}
				sourceSurface->Release ();
			}
		}
		ATOM_CHECK_D3D_RESULT(hr);
	}

	LPD3DXBUFFER buffer = 0;
	void *ret = 0;
	HRESULT hr = D3DXSaveTextureToFileInMemory (&buffer, d3dfileFormat, texture, 0);
	if (SUCCEEDED(hr))
	{
		ret = ATOM_MALLOC(buffer->GetBufferSize());
		if (!ret)
		{
			return 0;
		}
		if (size)
		{
			*size = buffer->GetBufferSize();
		}
		memcpy (ret, buffer->GetBufferPointer(), buffer->GetBufferSize());
	}
	ATOM_CHECK_D3D_RESULT(hr);

	if (buffer)
	{
		buffer->Release ();
	}

	if (texture != _M_texture)
	{
		texture->Release ();
	}

	return ret;
}

bool ATOM_D3DTexture::generateMipSubLevels (void)
{
	//TODO: No auto mipmap gen
	return false;

	if (_M_texture)
	{
		_M_texture->GenerateMipSubLevels ();
		return true;
	}
	return false;
}

bool ATOM_D3DTexture::clear (float r, float g, float b, float a)
{
	ATOM_STACK_TRACE(ATOM_D3DTexture::clear);

	//--- wangjian modified ---//

	if (_M_texture && _M_device && _M_device->isDeviceReady())
	{
		HRESULT hr;
		PDIRECT3DSURFACE9 pSurface = NULL;

		// 2D TEXTURE
		if( (_M_flags & ATOM_Texture::TEXTURE2D) != 0 )
		{
			hr = ((IDirect3DTexture9*)_M_texture)->GetSurfaceLevel( 0, &pSurface );
			if( SUCCEEDED( hr ) )
			{
				((ATOM_D3D9Device*)_M_device)->getD3DDevice()->ColorFill (pSurface, NULL, D3DCOLOR_COLORVALUE(r, g, b, a));
				if (pSurface)
				{
					pSurface->Release ();
				}
				return true;
			}
		}
		// CUBE TEXTURE
		else if( (_M_flags & ATOM_Texture::CUBEMAP) != 0 )
		{
			bool bSuccess = true;
			for( int face = 0; face < 6; ++face )
			{
				hr = ((IDirect3DCubeTexture9*)_M_texture)->GetCubeMapSurface ((D3DCUBEMAP_FACES)face, 0, &pSurface);
				if( SUCCEEDED( hr ) )
				{
					((ATOM_D3D9Device*)_M_device)->getD3DDevice()->ColorFill (pSurface, NULL, D3DCOLOR_COLORVALUE(r, g, b, a));
					if (pSurface)
						pSurface->Release ();
				}
				else
				{
					bSuccess = false;
				}
			}
			if( bSuccess )
				return true;
		}
		ATOM_CHECK_D3D_RESULT(hr);
	}
	return false;
}

unsigned ATOM_D3DTexture::getNumLevels (void) const
{
	ATOM_STACK_TRACE(ATOM_D3DTexture::getNumLevels);
	if (!_M_texture)
	{
		return 0;
	}
	return _M_texture->GetLevelCount ();
}

bool ATOM_D3DTexture::lock (int level, LockedRect *rc)
{
	ATOM_STACK_TRACE(ATOM_D3DTexture::lock);
	if (!_M_texture || !rc)
	{
		return false;
	}

	if (_M_flags & ATOM_Texture::TEXTURE3D)
	{
		return false;
	}

	int numLevels = _M_texture->GetLevelCount ();
	if (level >= numLevels)
	{
		return false;
	}

	D3DLOCKED_RECT lockedRect;

	if (_M_flags & ATOM_Texture::TEXTURE2D)
	{
		HRESULT hr = ((IDirect3DTexture9*)_M_texture)->LockRect (level, &lockedRect, NULL, 0);
		if (FAILED(hr))
		{
			ATOM_LOGGER::error ("%s(%d, 0x%08X) lockRect() failed.\n", __FUNCTION__, level, rc);
			ATOM_CHECK_D3D_RESULT(hr);
			return false;
		}

		rc->pitch = lockedRect.Pitch;
		rc->bits = lockedRect.pBits;
		_locked++;
		return true;
	}

	return false;
}
bool ATOM_D3DTexture::unlock (int level)
{
	ATOM_STACK_TRACE(ATOM_D3DTexture::unlock);
	if (!_M_texture)
	{
		return false;
	}

	if (_M_flags & ATOM_Texture::TEXTURE3D)
	{
		return false;
	}

	if (_locked == 0)
	{
		return false;
	}

	if (_M_flags & ATOM_Texture::TEXTURE2D)
	{
		HRESULT hr = ((IDirect3DTexture9*)_M_texture)->UnlockRect (level);
		if (FAILED(hr))
		{
			ATOM_LOGGER::error ("%s(%d) UnlockRect() failed.\n", __FUNCTION__, level);
			ATOM_CHECK_D3D_RESULT(hr);
			return false;
		}

		--_locked;
		return true;
	}

	return false;
}
//--- wangjian added ---//
bool ATOM_D3DTexture::lock (int level, int face, LockedRect *rc)
{
	ATOM_STACK_TRACE(ATOM_D3DTexture::lock);
	if (!_M_texture || !rc)
	{
		return false;
	}

	if (_M_flags & ATOM_Texture::TEXTURE3D || 
		_M_flags & ATOM_Texture::TEXTURE2D)
	{
		return false;
	}

	int numLevels = _M_texture->GetLevelCount ();
	if (level >= numLevels)
	{
		return false;
	}

	D3DLOCKED_RECT lockedRect;

	if (_M_flags & ATOM_Texture::CUBEMAP)
	{
		HRESULT hr = ((IDirect3DCubeTexture9*)_M_texture)->LockRect ( (D3DCUBEMAP_FACES)face, level, &lockedRect, NULL, 0 );
		if (FAILED(hr))
		{
			ATOM_LOGGER::error ("%s(mip:%d, face:%d, 0x%08X) lockRect() failed.\n", __FUNCTION__, level, face, rc);
			ATOM_CHECK_D3D_RESULT(hr);
			return false;
		}

		rc->pitch	= lockedRect.Pitch;
		rc->bits	= lockedRect.pBits;
		_locked++;

		return true;
	}

	return false;
}
bool ATOM_D3DTexture::unlock (int level, int face)
{
	ATOM_STACK_TRACE(ATOM_D3DTexture::unlock);
	if (!_M_texture)
	{
		return false;
	}

	if (_M_flags & ATOM_Texture::TEXTURE3D || 
		_M_flags & ATOM_Texture::TEXTURE2D)
	{
		return false;
	}

	if (_locked == 0)
	{
		return false;
	}

	if (_M_flags & ATOM_Texture::CUBEMAP)
	{
		HRESULT hr = ((IDirect3DCubeTexture9*)_M_texture)->UnlockRect ( (D3DCUBEMAP_FACES)face, level );
		if (FAILED(hr))
		{
			ATOM_LOGGER::error ("%s(mip:%d, face:%d,) UnlockRect() failed.\n", __FUNCTION__, level, face);
			ATOM_CHECK_D3D_RESULT(hr);
			return false;
		}

		--_locked;
		return true;
	}

	return false;
}
//----------------------//
bool ATOM_D3DTexture::mtload (const char *filename, ATOM_TextureLockStruct *tls)
{
	ATOM_STACK_TRACE(ATOM_D3DTexture::mtload);
	if (!filename)
	{
		return false;
	}

	ATOM_AutoFile f(filename, ATOM_VFS::read);
	if (!f)
	{
		return false;
	}

	tls->fileSize = f->size ();
	tls->fileContent.resize(tls->fileSize);

	if (tls->fileSize != f->read (&tls->fileContent[0], tls->fileSize))
	{
		return false;
	}

	ATOM_ImageFileInfo info;
	if (!_M_device->getImageFileInfoInMemory (&tls->fileContent[0], tls->fileSize, &info))
	{
		return false;
	}

	tls->width = info.width;
	tls->height = info.height;
	tls->numMips = info.numMips;
	tls->format = info.format;
	tls->imagetype = info.type;

	switch (tls->format)
	{
	case ATOM_PIXEL_FORMAT_RGBA8888:
		tls->format = ATOM_PIXEL_FORMAT_BGRA8888;
		break;
	case ATOM_PIXEL_FORMAT_RGBX8888:
		tls->format = ATOM_PIXEL_FORMAT_BGRX8888;
		break;
	case ATOM_PIXEL_FORMAT_RGB888:
		tls->format = ATOM_PIXEL_FORMAT_BGR888;
		break;
	}

	return true;
}

bool ATOM_D3DTexture::mtlock (const char *filename, ATOM_TextureLockStruct *tls)
{
	ATOM_STACK_TRACE(ATOM_D3DTexture::mtlock);
	if (!loadTexImageFromMemory (0, tls->width, tls->height, 1, tls->format, tls->numMips))
	{
		return false;
	}

	unsigned numMips = getNumLevels();
	if (tls->numMips > numMips)
	{
		tls->numMips = numMips;
	}
	tls->lockedRect.resize (tls->skipMipMap ? 1 : tls->numMips);

	for (unsigned i = 0; i < tls->lockedRect.size(); ++i)
	{
		if (!lock (i, &tls->lockedRect[i]))
		{
			return false;
		}
	}

	return true;
}

void GetSurfaceInfo( UINT width, UINT height, int fmt, UINT* pNumBytes, UINT* pRowBytes, UINT* pNumRows )
{
	ATOM_STACK_TRACE(GetSurfaceInfo);
	unsigned numBytes = 0;
	unsigned rowBytes = 0;
	unsigned numRows = 0;

	// From the DXSDK docs:
	//
	//     When computing DXTn compressed sizes for non-square textures, the
	//     following formula should be used at each mipmap level:
	//
	//         max(1, width ?4) x max(1, height ?4) x 8(DXT1) or 16(DXT2-5)
	//
	//     The pitch for DXTn formats is different from what was returned in
	//     Microsoft DirectX 7.0. It now refers the pitch of a row of blocks.
	//     For example, if you have a width of 16, then you will have a pitch
	//     of four blocks (4*8 for DXT1, 4*16 for DXT2-5.)"

	if( fmt == ATOM_PIXEL_FORMAT_DXT1 || fmt == ATOM_PIXEL_FORMAT_DXT3 || fmt == ATOM_PIXEL_FORMAT_DXT5 )
	{
		// Note: we support width and/or height being 0 in order to compute
		// offsets in functions like CBufferLockEntry::CopyBLEToPerfectSizedBuffer().
		int numBlocksWide = 0;
		if( width > 0 )
			numBlocksWide = ATOM_max2( 1, width / 4 );
		int numBlocksHigh = 0;
		if( height > 0 )
			numBlocksHigh = ATOM_max2( 1, height / 4 );
		//int numBlocks = numBlocksWide * numBlocksHigh;
		int numBytesPerBlock = ( (fmt == ATOM_PIXEL_FORMAT_DXT1) ? 8 : 16 );
		rowBytes = numBlocksWide * numBytesPerBlock;
		numRows = numBlocksHigh;
	}
	else
	{
		unsigned bpp = ATOM_PIXEL_FORMAT(fmt).bpp;
		rowBytes = ( width * bpp + 7 ) / 8; // round up to nearest byte
		numRows = height;
	}
	numBytes = rowBytes * numRows;
	if( pNumBytes != NULL )
		*pNumBytes = numBytes;
	if( pRowBytes != NULL )
		*pRowBytes = rowBytes;
	if( pNumRows != NULL )
		*pNumRows = numRows;
}

bool ATOM_D3DTexture::mtrealize (const char *filename, ATOM_TextureLockStruct *tls)
{
	ATOM_STACK_TRACE(ATOM_D3DTexture::mtrealize);
	unsigned numBytes, rowBytes, numRows;

	switch (tls->imagetype)
	{
	case ATOM_ImageCodec::DDS:
		{
			unsigned char *texData = &tls->fileContent[0] + sizeof(unsigned) + sizeof(DDSURFACEDESC2);

			for (unsigned i = 0; i < tls->lockedRect.size(); ++i)
			{
				GetSurfaceInfo (tls->width, tls->height, tls->format, &numBytes, &rowBytes, &numRows);

				unsigned char *destBits = (unsigned char*)tls->lockedRect[i].bits;

				for (unsigned h = 0; h < numRows; ++h)
				{
					memcpy (destBits, texData, rowBytes);
					destBits += tls->lockedRect[i].pitch;
					texData += rowBytes;
				}

				tls->width >>= 1;
				tls->height >>= 1;

				if (tls->width == 0)
				{
					tls->width = 1;
				}

				if (tls->height == 0)
				{
					tls->height = 1;
				}
			}
			break;
		}
	case ATOM_ImageCodec::PNG:
	case ATOM_ImageCodec::JPEG:
		{
			ATOM_RWops *input = ATOM_RWops::createMemRWops (&tls->fileContent[0], tls->fileSize);
			ATOM_BaseImage image;
			bool succ = image.init (input, tls->imagetype, tls->format);
			ATOM_RWops::destroyRWops (input);
			if (!succ)
			{
				return false;
			}

			GetSurfaceInfo (tls->width, tls->height, tls->format, &numBytes, &rowBytes, &numRows);
			unsigned char *src = image.getData();
			unsigned char *dest = (unsigned char*)tls->lockedRect[0].bits;

			for (unsigned h = 0; h < numRows; ++h)
			{
				memcpy (dest, src, rowBytes);
				dest += tls->lockedRect[0].pitch;
				src += rowBytes;
			}
			break;
		}
	default:
		return false;
	}

	return true;
}

bool ATOM_D3DTexture::mtunlock (const char *filename, ATOM_TextureLockStruct *tls)
{
	ATOM_STACK_TRACE(ATOM_D3DTexture::mtunlock);
	for (unsigned i = 0; i < tls->lockedRect.size(); ++i)
	{
		unlock (i);
	}
	return true;
}
//--- wangjian added for DX9 DEPTH STENCIL FORMAT ---//
// * 新增参数 bool bUseDX9DSFormat
// * 用以某些特殊情况时 需要生成DX9 DEPTH STENCIL FORMAT类型的DEPTH TEXTURE.
// * 现在用于shadow mapping的硬件PCF.
bool ATOM_D3DTexture::createAsDepthTexture (unsigned width, unsigned height, bool bUseDX9DSFormat)
{
	ATOM_D3D9Device *device = (ATOM_D3D9Device*)getRenderDevice();
	if (!device || !device->getD3DDevice())
	{
		return false;
	}
	//------- wangjian modified ---------//
	if ( !bUseDX9DSFormat && !device->getCapabilities().texture_depth)
	{
		return false;
	}
	//-----------------------------------//

	IDirect3DTexture9 *texture = 0;

	HRESULT hr = device->getD3DDevice()->CreateTexture (width, height, 
														1, 
														D3DUSAGE_DEPTHSTENCIL, 
														bUseDX9DSFormat ? device->getD3DPresentParams()->AutoDepthStencilFormat : device->getDepthTextureFormat(), // wangjian modified
														D3DPOOL_DEFAULT, 
														&texture, 
														NULL);
	if (FAILED (hr))
	{
		return false;
	}

	if (_M_texture)
	{
		_M_texture->Release();
		_M_texture = 0;
	}

	_M_texture = texture;
	_M_flags = ATOM_Texture::TEXTURE2D|ATOM_Texture::DEPTHSTENCIL;
	_M_format = ATOM_PIXEL_FORMAT_DEPTHSTENCIL;
	_M_d3dformat = bUseDX9DSFormat ? device->getD3DPresentParams()->AutoDepthStencilFormat : device->getDepthTextureFormat();	// wangjian modified
	_M_d3dusage = D3DUSAGE_DEPTHSTENCIL;
	_M_d3dpool = D3DPOOL_DEFAULT;
	_M_nummips = 1;
	_M_need_restore = true;
	_M_pixel_width = width;
	_M_pixel_height = height;
	_M_pixel_depth = 1;
	_contentLost = false;

	return true;
}
//---------------------------------------------------//
ATOM_DepthBuffer *ATOM_D3DTexture::getDepthBuffer (void)
{
	if (!_M_texture || _M_format != ATOM_PIXEL_FORMAT_DEPTHSTENCIL)
	{
		return 0;
	}

	if (!_M_depthBuffer)
	{
		IDirect3DSurface9 *surface = 0;
		HRESULT hr = ((IDirect3DTexture9*)_M_texture)->GetSurfaceLevel (0, &surface);
		if (FAILED(hr))
		{
			return 0;
		}

		ATOM_HARDREF(ATOM_DepthBufferD3D) depthbuffer;
		depthbuffer->setRenderDevice (getRenderDevice());
		if (!depthbuffer->create (surface))
		{
			depthbuffer = 0;
		}

		surface->Release();
		_M_depthBuffer = depthbuffer;
	}

	return _M_depthBuffer.get();
}

