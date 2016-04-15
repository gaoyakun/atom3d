#ifndef __ATOM3D_RENDER_TEXTURE_H
#define __ATOM3D_RENDER_TEXTURE_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "basedefs.h"
#include "gfxtypes.h"
#include "gfxresource.h"
#include "loadinterface.h"
//--- wangjian added ---//
// 异步加载
#include "../engine/ayncLoader.h"
//----------------------//

struct ATOM_TextureNode;
struct ATOM_TextureLockStruct;
class ATOM_DepthBuffer;

//--- wangjian added ---//
class ATOM_AsyncableLoadTexture;
//----------------------//

class ATOM_Texture: public ATOM_GfxResource
{
    friend class ID3DTextureManager;

  public:
    enum 
	{
		// General options
		NOMIPMAP = (1 << 1),
		NOCOMPRESS = (1 << 2),
		NORENDER = (1 << 3),
		RENDERTARGET = (1 << 4),
		DYNAMIC = (1 << 5),
		CUBEMAP = (1 << 6),
		TEXTURE2D = (1 << 7),
		TEXTURE3D = (1 << 8),
		DEPTHSTENCIL = (1 << 9),
		//--- wangjian added ---//
		AUTOGENMIPMAP = (1<< 10),		// 自动生成MIPMAP
		//----------------------//
    };

    enum {
      CUBEFACE_PX = 0,
      CUBEFACE_NX = 1,
      CUBEFACE_PY = 2,
      CUBEFACE_NY = 3,
      CUBEFACE_PZ = 4,
      CUBEFACE_NZ = 5,
    };

	struct LockedRect {
		int pitch;
		void *bits;
	};

  protected:
    ATOM_Texture(void);
	virtual ~ATOM_Texture (void);

  public:
    virtual void setRenderDevice (ATOM_RenderDevice *device);
    virtual unsigned getFlags() const;
    virtual int getWidth() const;
    virtual int getHeight() const;
	//--- wangjian added for DX9 DEPTH STENCIL FORMAT ---//
	// * 新增参数 bool bUseDX9DSFormat
	// * 用以某些特殊情况时 需要生成DX9 DEPTH STENCIL FORMAT类型的DEPTH TEXTURE.
	// * 现在用于shadow mapping的硬件PCF.
	virtual bool createAsDepthTexture (unsigned width, unsigned height,bool bUseDX9DSFormat = false ) = 0;
	//---------------------------------------------------//
    virtual bool loadTexImageFromFile (const char *filename, unsigned width, unsigned height, ATOM_PixelFormat format) = 0;
    virtual bool loadTexImageFromMemory (const void *contents, int w, int h, int d, ATOM_PixelFormat format, unsigned numMips) = 0;
	virtual bool loadTexImageFromFileInMemory (const void *fileInMemory, unsigned size, unsigned width, unsigned height, ATOM_PixelFormat format) = 0;
    virtual bool loadCubeTexImage (const void *contents[6], int size, ATOM_PixelFormat format, unsigned numMips) = 0;
    virtual bool updateTexImage(const void *contents, int offsetx,int offsety, int w, int h, ATOM_PixelFormat format) = 0;
    virtual bool copyTexImage(ATOM_PixelFormat format, int x, int y, int w, int h) = 0;
    virtual bool copySubTexImage(int offsetx,int offsety, int x, int y, int w, int h) = 0;
    virtual void getTexImage(ATOM_PixelFormat format, void *data) = 0;
	virtual bool generateMipSubLevels (void) = 0;
	virtual bool clear (float r, float g, float b, float a) = 0;
	virtual bool lock (int level, LockedRect *rc) = 0;
	virtual bool unlock (int level) = 0;
	//--- wangjian added ---//
	virtual bool lock (int level, int face, LockedRect *rc) = 0;
	virtual bool unlock (int level, int face) = 0;
	//----------------------//
	virtual unsigned getNumLevels (void) const = 0;
    virtual void setFileName(int index, const char *filename);
	virtual bool saveToFile(const char *filename) = 0;
	virtual bool saveToFileEx(const char *filename, ATOM_PixelFormat format) = 0;
	virtual void *saveToMem(ATOM_PixelFormat fileformat, unsigned *size) = 0;
	virtual void *saveToMemEx(ATOM_PixelFormat format, int fileformat, unsigned *size) = 0;
    virtual const char *getFileName(int index) const;
	virtual ATOM_TextureLoadInterface *getLoadInterface (void);
	virtual bool isContentLost (void) const;
	virtual void setContentLost (bool b);
	virtual bool isLocked (void) const;
	virtual ATOM_DepthBuffer *getDepthBuffer (void) = 0;

	//--- wangjian added ---//
	// 异步加载
	virtual ATOM_AsyncableLoadTexture * getAsyncLoader();
	void	setRenderTargetFace(int face=-1);
	int		getRenderTargetFace();
	//----------------------//

  public:
    void setFlags(unsigned flags);
	void setNode (ATOM_TextureNode *node);
	ATOM_TextureNode *getNode (void) const;

  public:
    virtual bool mtload (const char *filename, ATOM_TextureLockStruct *tls) = 0;
    virtual bool mtlock (const char *filename, ATOM_TextureLockStruct *tls) = 0;
    virtual bool mtrealize (const char *filename, ATOM_TextureLockStruct *tls) = 0;
    virtual bool mtunlock (const char *filename, ATOM_TextureLockStruct *tls) = 0;

  protected:
    ATOM_TextureNode *_M_node;
    unsigned _M_flags;
    int _M_pixel_width;
    int _M_pixel_height;
	int _M_pixel_depth;
	bool _contentLost;
	int _locked;
    ATOM_STRING _M_filenames[6];
	ATOM_AUTOPTR(ATOM_TextureLoadInterface) _loadInterface;
	//--- wangjian added ---//
	// 异步加载
	ATOM_AUTOPTR(ATOM_AsyncableLoadTexture) _asyncLoader;
	int		_rt_face;
	//----------------------//
  };

struct ATOM_TextureLockStruct
{
	ATOM_VECTOR<unsigned char> fileContent;
	unsigned fileSize;
	unsigned width;
	unsigned height;
	unsigned numMips;
	ATOM_PixelFormat format;
	int imagetype;
	bool skipMipMap;
	ATOM_VECTOR<ATOM_Texture::LockedRect> lockedRect;
};

//--- wangjian added ---//
// 异步加载 ：纹理相关类
class ATOM_AsyncableLoadTexture : public ATOM_AsyncableLoadObject
{
public:
	ATOM_AsyncableLoadTexture();
	virtual ~ATOM_AsyncableLoadTexture();
	void SetAttribute( ATOM_Texture * texture, int width,int height, ATOM_PixelFormat format, unsigned int flags = 0, bool skipMipMap = false );
	void GetAttribute( int & width, int & height, ATOM_PixelFormat & fmt, unsigned int & flags, bool & skipMipMap );
	//==========================================================================================//
	virtual bool Load();
	virtual bool OnloadEnd();
	//=================================///
	virtual bool Lock(bool & needRecurse);
	virtual bool Fill();
	virtual bool Unlock();
	//=================================///
	void setExistInDisk(bool bExist);
	bool isExistInDisk() const;
	//=================================//

protected:
	int m_width;
	int m_height;
	unsigned m_depth;
	unsigned m_numMips;
	unsigned m_mipStart;
	unsigned m_dataPtrStart;

	ATOM_PixelFormat m_format;
	bool m_needResize;
	bool m_needRePow2;

	unsigned int m_flags;
	int m_imagetype;
	int m_texturetype;
	bool m_skipMipMap;
	ATOM_VECTOR<ATOM_Texture::LockedRect> m_lockedRect;
	bool m_bExistInDisk;
	//ATOM_Texture * m_texture;
	//ATOM_AUTOREF(ATOM_Texture) m_texture;
};
//---------------------------------------------------------//

#endif // __ATOM3D_RENDER_TEXTURE_H
