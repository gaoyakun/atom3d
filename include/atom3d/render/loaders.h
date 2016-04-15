#ifndef __ATOM3D_RENDER_ASYNC_LOADER_H
#define __ATOM3D_RENDER_ASYNC_LOADER_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_kernel.h"

#include "basedefs.h"
#include "texture.h"
#include "vertexarray.h"
#include "indexarray.h"

class ATOM_RENDER_API ATOM_Texture2DLoader: public ATOM_BaseResourceLoader
{
public:
	ATOM_Texture2DLoader (const char *filename, ATOM_Texture *texture, bool skipMipMap = false, ATOM_PixelFormat format = ATOM_PIXEL_FORMAT_UNKNOWN);

public:
	ATOM_Texture *getTexture (void) const;

public:
	virtual LOADRESULT loadFromDisk (void);
	virtual bool lock (void);
	virtual int unlock (void);
	virtual bool realize (void);

private:
	unsigned _width;
	unsigned _height;
	unsigned _depth;
	unsigned _numMips;
	ATOM_PixelFormat _format;
	int _imagetype;
	int _texturetype;
	bool _skipMipMap;
	ATOM_AUTOREF(ATOM_Texture) _texture;
	ATOM_VECTOR<ATOM_Texture::LockedRect> _lockedRect;
};

#endif // __ATOM3D_RENDER_ASYNC_LOADER_H
