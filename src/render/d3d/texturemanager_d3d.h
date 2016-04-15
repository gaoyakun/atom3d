#ifndef __ATOM_GLRENDER_ID3DTEXTUREMANAGER_H
#define __ATOM_GLRENDER_ID3DTEXTUREMANAGER_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <vector>
#include <algorithm>
#include <string>
#include <ATOM_utils.h>
#include <ATOM_kernel.h>
#include "vertexattrib.h"
#include "texturemanager.h"
#include "texture.h"

class ATOM_D3DTexture;

class ATOM_ID3DTextureManager : public ATOM_TextureManager
  {
  public:
    ATOM_ID3DTextureManager(ATOM_RenderDevice *device);
    virtual ~ATOM_ID3DTextureManager();

  public:
    virtual ATOM_AUTOREF(ATOM_Texture) LookupTexture(const char* name);
    virtual void Invalidate (void);

  protected:
    virtual ATOM_AUTOREF(ATOM_Texture) doAllocateTexture (unsigned flags, const char* name);
    virtual ATOM_AUTOREF(ATOM_Texture) doAllocateTexture (const void* image, int w, int h, ATOM_PixelFormat format, unsigned flags, const char* name);
    virtual ATOM_AUTOREF(ATOM_Texture) doAllocateVolumeTexture (const void* image, int w, int h, int d, ATOM_PixelFormat format, unsigned flags, const char* name);
    virtual ATOM_AUTOREF(ATOM_Texture) doAllocateCubeTexture (const void* image[6], int size, ATOM_PixelFormat format, unsigned flags, const char* name);
    //--- wangjian added for DX9 DEPTH STENCIL FORMAT ---//
	// * �������� bool bUseDX9DSFormat
	// * ����ĳЩ�������ʱ ��Ҫ����DX9 DEPTH STENCIL FORMAT���͵�DEPTH TEXTURE.
	// * ��������shadow mapping��Ӳ��PCF.
	virtual ATOM_AUTOREF(ATOM_Texture) doAllocateDepthTexture (int w, int h, bool bUseDX9DSFormat = false );
	//---------------------------------------------------//
	//---- wangjian MODIFIED for MultiThreaded ---//
	// �첽���� �� ����첽�������ȼ���Ĭ��Ϊ�첽���أ�
	virtual ATOM_AUTOREF(ATOM_Texture) doAllocateTexture (	const char *filename, unsigned width, unsigned height, bool asResource, ATOM_PixelFormat format, 
															int loadPriority = ATOM_LoadPriority_ASYNCBASE );
	//--------------------------------------------//
  private:
    ATOM_AUTOREF(ATOM_D3DTexture) InternalAllocTexture(unsigned flags, const char* name);
    ATOM_AUTOREF(ATOM_D3DTexture) InternalAllocTexture(int w, int h, ATOM_PixelFormat sformat, const void* contents, unsigned flags, const char* name);
    ATOM_AUTOREF(ATOM_D3DTexture) InternalAllocVolumeTexture(int w, int h, int d, ATOM_PixelFormat sformat, const void* contents, unsigned flags, const char* name);
    ATOM_AUTOREF(ATOM_D3DTexture) InternalAllocCubeMap (int size, ATOM_PixelFormat sformat, const void* contents[6], unsigned flags, const char* name);
    //--- wangjian added for DX9 DEPTH STENCIL FORMAT ---//
	// * �������� bool bUseDX9DSFormat
	// * ����ĳЩ�������ʱ ��Ҫ����DX9 DEPTH STENCIL FORMAT���͵�DEPTH TEXTURE.
	// * ��������shadow mapping��Ӳ��PCF.
	ATOM_AUTOREF(ATOM_D3DTexture) InternalAllocDepthTexture (int w, int h, bool bUseDX9DSFormat = false);
	//---------------------------------------------------//
  };

#endif // __ATOM_GLRENDER_ID3DTEXTUREMANAGER_H
