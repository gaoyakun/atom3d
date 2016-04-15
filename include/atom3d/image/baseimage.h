#ifndef __ATOM_IMAGE_BASEIMAGE_H
#define __ATOM_IMAGE_BASEIMAGE_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <algorithm>
#include "../ATOM_kernel.h"
#include "basedefs.h"
#include "pixelformat.h"
#include "imagecodec.h"

class ATOM_IMAGE_API ATOM_BaseImage
{
public:
	ATOM_BaseImage (void);
	ATOM_BaseImage (unsigned width, unsigned height, ATOM_PixelFormat format, const void *data, int mipcount);
	ATOM_BaseImage (ATOM_RWops *input, int type = ATOM_ImageCodec::ANY);
	void swap (ATOM_BaseImage &rhs);

	ATOM_BaseImage (const ATOM_BaseImage &rhs);
	ATOM_BaseImage & operator = (const ATOM_BaseImage &rhs);
	virtual ~ATOM_BaseImage (void);

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

public:
	unsigned getWidth (void) const;
	unsigned getHeight (void) const;
	ATOM_PixelFormat getFormat (void) const;
	int getNumMipmaps (void) const;
	int getBufferSize (void) const;
	unsigned char* getData (void) const;

protected:
	void convertToGray (void);
	void convertToDXTn (ATOM_PixelFormat format);
	void convertFromDXTn (ATOM_PixelFormat format);

protected:
	ATOM_PixelFormat _M_format;
	int _M_nummips;
	unsigned _M_width;
	unsigned _M_height;
	unsigned _M_buffersize;
	unsigned _M_actual_size;
	unsigned char* _M_contents;
};

#endif // __ATOM_IMAGE_IMAGE_H
