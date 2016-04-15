#ifndef __ATOM_IMAGE_IMAGEAPI_H
#define __ATOM_IMAGE_IMAGEAPI_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "basedefs.h"

#define ATOM_IMAGE_DXT_COMPRESS_QUALITY_FASTEST     1
#define ATOM_IMAGE_DXT_COMPRESS_QUALITY_NORMAL      2
#define ATOM_IMAGE_DXT_COMPRESS_QUALITY_PRODUCTION  3
#define ATOM_IMAGE_DXT_COMPRESS_QUALITY_HIGHEST     4

extern "C" {

void ATOM_IMAGE_API ATOM_SetDXTCompressQuality (int quality);
int ATOM_IMAGE_API ATOM_GetDXTCompressQuality (void);

} // 

#endif // __ATOM_IMAGE_IMAGEAPI_H
