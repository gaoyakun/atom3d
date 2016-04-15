#ifndef __ATOM3D_RENDER_LOADINTERFACE_H
#define __ATOM3D_RENDER_LOADINTERFACE_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_kernel.h"

class ATOM_Texture;
class ATOM_Texture2DLoader;

class ATOM_TextureLoadInterface: public ATOM_LoadInterface
{
public:
	ATOM_TextureLoadInterface (ATOM_Texture *owner);
	virtual ~ATOM_TextureLoadInterface (void);

public:
	virtual ATOM_Texture *getTexture (void) const;
	virtual void setTexture (ATOM_Texture *texture);
	virtual void setSkipMipMap (bool skip);
	virtual bool isSkipMipMap (void) const;
	virtual void setWidth (unsigned width);
	virtual unsigned getWidth (void) const;
	virtual void setHeight (unsigned height);
	virtual unsigned getHeight (void) const;
	virtual void setFormat (ATOM_PixelFormat format);
	virtual ATOM_PixelFormat getFormat (void) const;

public:
	virtual ATOM_LoadInterface::LoadingState load (void);
	virtual ATOM_LoadInterface::LoadingState mtload (unsigned flags, long group, ATOM_LoadingRequestCallback callback, void *userData);
	virtual void unload (void);
	virtual void finishLoad (void);
	virtual ATOM_BaseResourceLoader *getLoader (void);

private:
	ATOM_Texture *_texture;
	bool _skipMipMap;
	unsigned _width;
	unsigned _height;
	ATOM_PixelFormat _format;
	ATOM_Texture2DLoader *_loader;
};

#endif // __ATOM3D_RENDER_LOADINTERFACE_H
