#ifndef __ATOM_IMAGE_IMAGE_H
#define __ATOM_IMAGE_IMAGE_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_kernel.h"
#include "basedefs.h"
#include "baseimage.h"

class ATOM_IMAGE_API ATOM_Image : public ATOM_Resource
{
	ATOM_CLASS(image, ATOM_Image, ATOM_Image);
	ATOM_DECLARE_SCRIPT_INTERFACE(ATOM_Image)
	
public:
	ATOM_Image (void);
	virtual ~ATOM_Image (void);

public:
	ATOM_BaseImage &getBaseImage (void);
	const ATOM_BaseImage &getBaseImage (void) const;

public:
	void fini (void);
	void init (unsigned width, unsigned height, ATOM_PixelFormat format, const void* data = 0, int mipcount = 1);
	bool init (ATOM_RWops* input, int type = ATOM_ImageCodec::ANY, ATOM_PixelFormat format = ATOM_PIXEL_FORMAT_END);
	void convertFormat (ATOM_PixelFormat format);
	void castFormat (ATOM_PixelFormat format);
	void blitTo (ATOM_BaseImage *dst, int srcx, int srcy, int srcw, int srch, int dstx, int dsty);
	void resize (unsigned width, unsigned height);
	void flip (void);
	void crop (int left, int top, unsigned width, unsigned height);
	bool save (ATOM_File *file, ATOM_PixelFormat format);
	bool save (ATOM_RWops *rwops, int type, ATOM_PixelFormat format);
	bool load (ATOM_File *file, ATOM_PixelFormat format);
	bool load (ATOM_RWops *rwops, int type, ATOM_PixelFormat format);
	unsigned getWidth (void) const;
	unsigned getHeight (void) const;
	ATOM_PixelFormat getFormat (void) const;
	int getNumMipmaps (void) const;
	int getBufferSize (void) const;
	unsigned char* getData (void) const;

public:
	virtual bool readFromFile (ATOM_File* file, void *userdata = 0);
	virtual bool writeToFile (ATOM_File* file, ATOM_PixelFormat format = ATOM_PIXEL_FORMAT_END);

private:
	ATOM_BaseImage _M_baseimage;
};

#endif // __ATOM_IMAGE_IMAGE_H
