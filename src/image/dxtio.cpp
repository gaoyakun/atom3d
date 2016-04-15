#include "stdafx.h"
#ifdef __CODEGEARC__
# pragma hdrstop
#endif

#include "imageapi.h"
#include <squish.h>

extern int dxt_quality;

int getDXTformat (int format)
{
  switch (format)
  {
  case ATOM_PIXEL_FORMAT_DXT1:
    return squish::kDxt1;
  case ATOM_PIXEL_FORMAT_DXT3:
    return squish::kDxt3;
  case ATOM_PIXEL_FORMAT_DXT5:
    return squish::kDxt5;
  default:
    return squish::kDxt1;
  }
}

bool CreateFromDXTImage (ATOM_PixelFormat format, ATOM_BaseImage *src)
{
	ATOM_STACK_TRACE(CreateFromDXTImage);
	int dxtformat = getDXTformat (src->getFormat());
	ATOM_BaseImage imageRGBA(src->getWidth(), src->getHeight(), ATOM_PIXEL_FORMAT_RGBA8888, 0, 1);
	squish::DecompressImage ((squish::u8*)imageRGBA.getData(), src->getWidth(), src->getHeight(), src->getData(), dxtformat);
	src->init (src->getWidth(), src->getHeight(), imageRGBA.getFormat(), imageRGBA.getData(), 1);
	src->convertFormat (format);
	return true;
};

bool CreateDXTImage (ATOM_PixelFormat format, ATOM_BaseImage *src)
{
	ATOM_STACK_TRACE(CreateDXTImage);
	int dxtformat = getDXTformat (format);
	if (ATOM_PIXEL_FORMAT(src->getFormat())._is_compressed_format)
	{
		return false;
	}
	src->convertFormat (ATOM_PIXEL_FORMAT_RGBA8888);
	int memsize = squish::GetStorageRequirements (src->getWidth(), src->getHeight(), dxtformat);
	void *p = ATOM_MALLOC(memsize);
	squish::CompressImage ((const squish::u8*)src->getData(), src->getWidth(), src->getHeight(), p, dxtformat);
	src->init (src->getWidth(), src->getHeight(), format, p, 1);
	ATOM_FREE(p);

	return true;
}


