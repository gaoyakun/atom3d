#ifndef __ATOM3D_RENDER_D3DTEXTURE_H
#define __ATOM3D_RENDER_D3DTEXTURE_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <ddraw.h>
#include "texture.h"

class ATOM_ID3DTextureManager;
class ATOM_D3D9Device;

class ATOM_D3DTexture : public ATOM_Texture
{
    ATOM_CLASS(render, ATOM_D3DTexture, ATOM_D3DTexture)
    friend class ATOM_ID3DTextureManager;

public:
    ATOM_D3DTexture(void);
    virtual ~ATOM_D3DTexture(void);
public:
    virtual bool realize (unsigned numMips);
	//--- wangjian added for DX9 DEPTH STENCIL FORMAT ---//
	// * 新增参数 bool bUseDX9DSFormat
	// * 用以某些特殊情况时 需要生成DX9 DEPTH STENCIL FORMAT类型的DEPTH TEXTURE.
	// * 现在用于shadow mapping的硬件PCF.
	virtual bool createAsDepthTexture (unsigned width, unsigned height,bool bUseDX9DSFormat = false);
	//---------------------------------------------------//
    virtual bool loadTexImageFromFile (const char *filename, unsigned width, unsigned height, ATOM_PixelFormat format);
    virtual bool loadTexImageFromMemory (const void *contents, int w, int h, int d, ATOM_PixelFormat format, unsigned numMips);
	virtual bool loadTexImageFromFileInMemory (const void *fileInMemory, unsigned size, unsigned width, unsigned height, ATOM_PixelFormat format);
    virtual bool loadCubeTexImage (const void *contents[6], int size, ATOM_PixelFormat format, unsigned numMips);
    virtual bool updateTexImage(const void *contents, int offsetx,int offsety, int w, int h, ATOM_PixelFormat format);
    virtual bool copyTexImage(ATOM_PixelFormat format, int x, int y, int w, int h);
    virtual bool copySubTexImage(int offsetx,int offsety, int x, int y, int w, int h);
    virtual void getTexImage(ATOM_PixelFormat format, void *data);
	virtual bool generateMipSubLevels (void);
	virtual bool clear (float r, float g, float b, float a);
	virtual unsigned getNumLevels (void) const;
	virtual bool lock (int level, LockedRect *rc);
	virtual bool unlock (int level);
	//--- wangjian added ---//
	virtual bool lock (int level, int face, LockedRect *rc);
	virtual bool unlock (int level, int face);
	//----------------------//
	virtual bool saveToFile(const char *filename);
	virtual bool saveToFileEx(const char *filename, ATOM_PixelFormat format);
	virtual void *saveToMem(ATOM_PixelFormat fileformat, unsigned *size);
	virtual void *saveToMemEx(ATOM_PixelFormat format, int fileformat, unsigned *size);
    virtual HRESULT getLevelDesc (int nLevel, D3DSURFACE_DESC *pDesc);
	//--- wangjian modified ---//
    virtual HRESULT getSurfaceLevel (int nLevel, IDirect3DSurface9 **ppSurface, unsigned face = -1);
	//-------------------------//
	virtual ATOM_DepthBuffer *getDepthBuffer (void);

public:
    virtual bool mtload (const char *filename, ATOM_TextureLockStruct *tls);
    virtual bool mtlock (const char *filename, ATOM_TextureLockStruct *tls);
    virtual bool mtrealize (const char *filename, ATOM_TextureLockStruct *tls);
    virtual bool mtunlock (const char *filename, ATOM_TextureLockStruct *tls);

public:
    IDirect3DBaseTexture9 *getD3DTexture (void) const;
    D3DFORMAT getD3DFormat (void) const;

private:
    IDirect3DTexture9 *internalCreateDummyTexture (ATOM_D3D9Device *device, int w, int h, D3DFORMAT format, int numMips);
    bool internalCreateTexture (ATOM_D3D9Device *device, int w, int h, ATOM_PixelFormat format, unsigned numMips);
    bool internalCreateCubeTexture (ATOM_D3D9Device *device, int size, ATOM_PixelFormat format, unsigned numMips);
    bool internalCreateVolumeTexture (ATOM_D3D9Device *device, int w, int h, int d, ATOM_PixelFormat format, unsigned numMips);
    D3DFORMAT chooseD3DFormat (ATOM_D3D9Device *device, ATOM_PixelFormat format, bool compress, bool rendertarget, int w, int h) const;
    void releaseTexture (IDirect3DBaseTexture9 *texture);
    void releaseSurface (IDirect3DSurface9 *surface);

protected:
    virtual void invalidateImpl (bool needRestore);
    virtual void restoreImpl (void);

protected:
    ATOM_ID3DTextureManager* _M_manager;
    IDirect3DBaseTexture9 *_M_texture;
	int _M_nummips;
    ATOM_PixelFormat _M_format;
    bool _M_need_restore;
    D3DFORMAT _M_d3dformat;
    DWORD _M_d3dusage;
    D3DPOOL _M_d3dpool;
	ATOM_AUTOREF(ATOM_DepthBuffer) _M_depthBuffer;
};

#endif // __ATOM3D_RENDER_D3DTEXTURE_H
