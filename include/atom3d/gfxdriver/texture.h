#ifndef __ATOM3D_GFXDRIVER_TEXTURE_H
#define __ATOM3D_GFXDRIVER_TEXTURE_H

#include "../ATOM_utils.h"
#include "../ATOM_image.h"
#include "basedefs.h"
#include "gfxresource.h"

class ATOM_DepthBuffer;

class ATOM_Texture: public ATOM_GfxResource
{
public:
    enum 
	{
      // General options
      RenderTarget = (1 << 0),
      NoMipmap = (1 << 1),
      TextureCube = (1 << 2),
      NoCompress = (1 << 4),
      Texture2D = (1 << 7),
      Texture3D = (1 << 8),
	  TextureDepth = (1<<9)
    };

    enum CubeFace {
      CubeFacePX = 0,
      CubeFaceNX = 1,
      CubeFacePY = 2,
      CubeFaceNY = 3,
      CubeFacePZ = 4,
      CubeFaceNZ = 5,
    };

	struct LockedRect {
		unsigned pitch;
		void *bits;
	};

protected:
    ATOM_Texture(void);
	virtual ~ATOM_Texture (void);

public:
    virtual unsigned getFlags (void) const;
    virtual unsigned getWidth (void) const;
    virtual unsigned getHeight (void) const;
	virtual unsigned getDepth (void) const;
	virtual bool loadFromFile (const char *filename, ATOM_PixelFormat format) = 0;
	virtual bool loadFromFileInMemory (const void *contents, ATOM_PixelFormat format) = 0;
	virtual bool reset (unsigned flags, unsigned width, unsigned height, unsigned depth) = 0;
	virtual bool saveToFile (const char *filename) = 0;
	virtual bool texImage2D (unsigned mipLevel, const ATOM_Rect2Di *updateRegion, const void *srcData, ATOM_PixelFormat srcFormat, unsigned stride) = 0;
	virtual bool texImageCubeFace (CubeFace face, unsigned mipLevel, const ATOM_Rect2Di *updateRegion, const void *srcData, ATOM_PixelFormat srcFormat, unsigned stride) = 0;
	virtual bool texImage3D (unsigned mipLevel, const ATOM_Cube3Di *updateRegion, const void *srcData, ATOM_PixelFormat srcFormat, unsigned lineStride, unsigned sliceStride) = 0;
	virtual bool texImage3DSlice (unsigned mipLevel, const ATOM_Rect2Di *updateRegion, const void *srcData, ATOM_PixelFormat srcFormat, unsigned lineStride) = 0;
	virtual bool clear (float r, float g, float b, float a) = 0;
	virtual bool lock2D (unsigned mipLevel, LockedRect *rc) = 0;
	virtual bool unlock (int level) = 0;
	virtual unsigned getNumLevels (void) const = 0;
	virtual bool saveToFileEx(const char *filename, ATOM_PixelFormat format) = 0;
	virtual void *saveToMem(ATOM_PixelFormat fileformat, unsigned *size) = 0;
	virtual void *saveToMemEx(ATOM_PixelFormat format, int fileformat, unsigned *size) = 0;
	virtual bool isContentLost (void) const;
	virtual void setContentLost (bool b);
	virtual ATOM_DepthBuffer *getDepthBuffer (void) = 0;

protected:
    unsigned _flags;
    int _width;
    int _height;
	int _depth;
	bool _contentLost;
};

#endif // __ATOM3D_GFXDRIVER_TEXTURE_H
