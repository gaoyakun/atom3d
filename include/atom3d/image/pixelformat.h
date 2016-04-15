#ifndef __ATOM_IMAGE_PIXELFORMAT_H
#define __ATOM_IMAGE_PIXELFORMAT_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "basedefs.h"
#define ATOM_PIXEL_FORMAT(format) ::_DefPixelFormats[format]
#define ATOM_IS_VALID_FORMAT(format) ((format) >= ATOM_PIXEL_FORMAT_START && (format) < ATOM_PIXEL_FORMAT_END)

// These is all supported pixel format in shine3d
enum  ATOM_PixelFormat
{
	ATOM_PIXEL_FORMAT_UNKNOWN = -1,
	ATOM_PIXEL_FORMAT_START = 0,
	ATOM_PIXEL_FORMAT_RGBA8888 = 0,
	ATOM_PIXEL_FORMAT_RGBX8888,
	ATOM_PIXEL_FORMAT_ARGB8888,
	ATOM_PIXEL_FORMAT_XRGB8888,
	ATOM_PIXEL_FORMAT_BGRA8888,
	ATOM_PIXEL_FORMAT_BGRX8888,
	ATOM_PIXEL_FORMAT_ABGR8888,
	ATOM_PIXEL_FORMAT_XBGR8888,
	ATOM_PIXEL_FORMAT_RGB888,
	ATOM_PIXEL_FORMAT_BGR888,
	ATOM_PIXEL_FORMAT_RGBA4444,
	ATOM_PIXEL_FORMAT_RGBX4444,
	ATOM_PIXEL_FORMAT_ARGB4444,
	ATOM_PIXEL_FORMAT_XRGB4444,
	ATOM_PIXEL_FORMAT_BGRA4444,
	ATOM_PIXEL_FORMAT_BGRX4444,
	ATOM_PIXEL_FORMAT_ABGR4444,
	ATOM_PIXEL_FORMAT_XBGR4444,
	ATOM_PIXEL_FORMAT_ARGB1555,
	ATOM_PIXEL_FORMAT_XRGB1555,
	ATOM_PIXEL_FORMAT_RGBA5551,
	ATOM_PIXEL_FORMAT_RGBX5551,
	ATOM_PIXEL_FORMAT_BGR565,
	ATOM_PIXEL_FORMAT_A8,
	ATOM_PIXEL_FORMAT_GREY8,
	ATOM_PIXEL_FORMAT_GREY8A8,
	ATOM_PIXEL_FORMAT_DEPTHSTENCIL,
	ATOM_PIXEL_FORMAT_RGBA16,
	ATOM_PIXEL_FORMAT_RGB16,
	ATOM_PIXEL_FORMAT_A16,
	ATOM_PIXEL_FORMAT_GREY16,
	ATOM_PIXEL_FORMAT_GREY16A16,
	ATOM_PIXEL_FORMAT_RGBA32F,
	ATOM_PIXEL_FORMAT_R32F,
	ATOM_PIXEL_FORMAT_RG32F,
	ATOM_PIXEL_FORMAT_RGBA16F,
	ATOM_PIXEL_FORMAT_R16F,
	ATOM_PIXEL_FORMAT_RG16F,
	ATOM_PIXEL_FORMAT_DXT1,
	ATOM_PIXEL_FORMAT_DXT3,
	ATOM_PIXEL_FORMAT_DXT5,

	// new formats comes here
	ATOM_PIXEL_FORMAT_END,
};

struct TPixelFormat
{
	unsigned bpp;
	unsigned red_mask;
	unsigned green_mask;
	unsigned blue_mask;
	unsigned alpha_mask;
	unsigned char red_shift;
	unsigned char green_shift;
	unsigned char blue_shift;
	unsigned char alpha_shift;
	unsigned char red_loss;
	unsigned char green_loss;
	unsigned char blue_loss;
	unsigned char alpha_loss;

	// for internal usage
	int _has_alpha_channel;
	int _is_compressed_format;
};

// FIXME!!! Static array will avoid exports, but will increment module size. Try a better algorithm.
static const TPixelFormat _DefPixelFormats[ATOM_PIXEL_FORMAT_END] = 
{
	{ 32,0xFF000000,0x00FF0000,0x0000FF00,0x000000FF,24,16,8,0,0,0,0,0,1,0 }, 
	{ 32,0xFF000000,0x00FF0000,0x0000FF00,0x00000000,24,16,8,0,0,0,0,0,1,0 }, 
	{ 32,0x00FF0000,0x0000FF00,0x000000FF,0xFF000000,16,8,0,24,0,0,0,0,1,0 }, 
	{ 32,0x00FF0000,0x0000FF00,0x000000FF,0x00000000,16,8,0,24,0,0,0,0,1,0 }, 
	{ 32,0x0000FF00,0x00FF0000,0xFF000000,0x000000FF,8,16,24,0,0,0,0,0,1,0 }, 
	{ 32,0x0000FF00,0x00FF0000,0xFF000000,0x00000000,8,16,24,0,0,0,0,0,1,0 }, 
	{ 32,0x000000FF,0x0000FF00,0x00FF0000,0xFF000000,0,8,16,24,0,0,0,0,1,0 }, 
	{ 32,0x000000FF,0x0000FF00,0x00FF0000,0x00000000,0,8,16,24,0,0,0,0,1,0 }, 
	{ 24,0x00FF0000,0x0000FF00,0x000000FF,0x00000000,16,8,0,0,0,0,0,0,0,0 }, 
	{ 24,0x000000FF,0x0000FF00,0x00FF0000,0x00000000,0,8,16,0,0,0,0,0,0,0 }, 
	{ 16,0x0000F000,0x00000F00,0x000000F0,0x0000000F,12,8,4,0,4,4,4,4,1,0 }, 
	{ 16,0x0000F000,0x00000F00,0x000000F0,0x00000000,12,8,4,0,4,4,4,4,1,0 }, 
	{ 16,0x00000F00,0x000000F0,0x0000000F,0x0000F000,8,4,0,12,4,4,4,4,1,0 }, 
	{ 16,0x00000F00,0x000000F0,0x0000000F,0x00000000,8,4,0,12,4,4,4,4,1,0 }, 
	{ 16,0x000000F0,0x00000F00,0x0000F000,0x0000000F,4,8,12,0,4,4,4,4,1,0 }, 
	{ 16,0x000000F0,0x00000F00,0x0000F000,0x00000000,4,8,12,0,4,4,4,4,1,0 }, 
	{ 16,0x0000000F,0x000000F0,0x00000F00,0x0000F000,0,4,8,12,4,4,4,4,1,0 }, 
	{ 16,0x0000000F,0x000000F0,0x00000F00,0x00000000,0,4,8,12,4,4,4,4,1,0 }, 
	{ 16,0x00007C00,0x000003E0,0x0000001F,0x00008000,10,5,0,15,3,3,3,7,1,0 }, 
	{ 16,0x00007C00,0x000003E0,0x0000001F,0x00000000,10,5,0,15,3,3,3,7,1,0 }, 
	{ 16,0x0000F800,0x000007C0,0x0000003E,0x00000001,11,6,1,0,3,3,3,7,1,0 }, 
	{ 16,0x0000F800,0x000007C0,0x0000003E,0x00000000,11,6,1,0,3,3,3,7,1,0 }, 
	{ 16,0x0000001F,0x000007E0,0x0000F800,0x00000000,0,5,11,0,3,2,3,0,0,0 },
	{  8,0x00000000,0x00000000,0x00000000,0x000000FF,0,0,0,0,0,0,0,0,1,0},
	{  8,0x000000FF,0x000000FF,0x000000FF,0x00000000,0,0,0,0,0,0,0,0,0,0},
	{ 16,0x0000FF00,0x0000FF00,0x0000FF00,0x000000FF,0,0,0,0,0,0,0,0,1,0},
	{ 32,0x00000000,0x00000000,0x00000000,0x00000000,0,0,0,0,0,0,0,0,0,0},
	{ 64,0x00000000,0x00000000,0x00000000,0x00000000,0,0,0,0,0,0,0,0,1,0},
	{ 48,0x00000000,0x00000000,0x00000000,0x00000000,0,0,0,0,0,0,0,0,0,0},
	{ 16,0x00000000,0x00000000,0x00000000,0x00000000,0,0,0,0,0,0,0,0,1,0},
	{ 16,0x00000000,0x00000000,0x00000000,0x00000000,0,0,0,0,0,0,0,0,0,0},
	{ 32,0x00000000,0x00000000,0x00000000,0x00000000,0,0,0,0,0,0,0,0,1,0},
	{128,0x00000000,0x00000000,0x00000000,0x00000000,0,0,0,0,0,0,0,0,1,0},
	{ 32,0x00000000,0x00000000,0x00000000,0x00000000,0,0,0,0,0,0,0,0,1,0},
	{ 64,0x00000000,0x00000000,0x00000000,0x00000000,0,0,0,0,0,0,0,0,1,0},
	{ 64,0x00000000,0x00000000,0x00000000,0x00000000,0,0,0,0,0,0,0,0,1,0},
	{ 16,0x00000000,0x00000000,0x00000000,0x00000000,0,0,0,0,0,0,0,0,1,0},
	{ 32,0x00000000,0x00000000,0x00000000,0x00000000,0,0,0,0,0,0,0,0,1,0},
	{  0,0x00000000,0x00000000,0x00000000,0x00000000,0,0,0,0,0,0,0,0,0,1},
	{  0,0x00000000,0x00000000,0x00000000,0x00000000,0,0,0,0,0,0,0,0,1,1},
	{  0,0x00000000,0x00000000,0x00000000,0x00000000,0,0,0,0,0,0,0,0,1,1}
};

inline void ATOM_EncodePixel(int format, unsigned char r, unsigned char g, unsigned char b, unsigned char a, unsigned * dst) 
{
	const TPixelFormat& pfdst = ATOM_PIXEL_FORMAT(format);
	unsigned pixel = (((r << pfdst.red_shift) >> pfdst.red_loss) & pfdst.red_mask)
				 | (((g << pfdst.green_shift) >> pfdst.green_loss) & pfdst.green_mask)
				 | (((b << pfdst.blue_shift) >> pfdst.blue_loss) & pfdst.blue_mask);

	if ( pfdst._has_alpha_channel)
	{
		pixel |= (((a << pfdst.alpha_shift) >> pfdst.alpha_loss) & pfdst.alpha_mask);
	}

	unsigned char * p = (unsigned char *) dst;

	switch ( pfdst.bpp)
	{
	case 32:
		{
			p[0] = ((unsigned char *) &pixel)[3];
			p[1] = ((unsigned char *) &pixel)[2];
			p[2] = ((unsigned char *) &pixel)[1];
			p[3] = ((unsigned char *) &pixel)[0];
			break;
		}
	case 24:
		{
			p[0] = ((unsigned char *) &pixel)[2];
			p[1] = ((unsigned char *) &pixel)[1];
			p[2] = ((unsigned char *) &pixel)[0];
			break;
		}
	case 16:
		{
			p[0] = ((unsigned char *) &pixel)[1];
			p[1] = ((unsigned char *) &pixel)[0];
			break;
		}
	case 8:
		{
			p[0] = ((unsigned char *)&pixel)[0];
			break;
		}
	default:
		{
			break;
		}
	}
}

inline void ATOM_DecodePixel(int format,
                 unsigned char* r,
                 unsigned char* g,
                 unsigned char* b,
                 unsigned char* a,
                 const unsigned * src) 
{
	const TPixelFormat& pfsrc = ATOM_PIXEL_FORMAT(format);
	unsigned pixel;
	unsigned char * p = (unsigned char *) &pixel;

	switch ( pfsrc.bpp)
	{
	case 32:
		{
			p[0] = ((unsigned char *) src)[3];
			p[1] = ((unsigned char *) src)[2];
			p[2] = ((unsigned char *) src)[1];
			p[3] = ((unsigned char *) src)[0];
			break;
		}
	case 24:
		{
			p[0] = ((unsigned char *) src)[2];
			p[1] = ((unsigned char *) src)[1];
			p[2] = ((unsigned char *) src)[0];
			p[3] = 0;
			break;
		}
	case 16:
		{
			p[0] = ((unsigned char *) src)[1];
			p[1] = ((unsigned char *) src)[0];
			p[2] = 0;
			p[3] = 0;
			break;
		}
	case 8:
		{
			p[0] = ((unsigned char *) src)[0];
			p[1] = 0;
			p[2] = 0;
			p[3] = 0;
			break;
		}
	default:
		{
			return;
		}
	}

	unsigned rv = (pixel & pfsrc.red_mask) >> pfsrc.red_shift;
	*r = (rv << pfsrc.red_loss) + (rv >> (8 - pfsrc.red_loss));
	unsigned gv = (pixel & pfsrc.green_mask) >> pfsrc.green_shift;
	*g = (gv << pfsrc.green_loss) + (gv >> (8 - pfsrc.green_loss));
	unsigned bv = (pixel & pfsrc.blue_mask) >> pfsrc.blue_shift;
	*b = (bv << pfsrc.blue_loss) + (bv >> (8 - pfsrc.blue_loss));

	if ( !pfsrc.alpha_mask)
	{
		*a = 0xFF;
	}
	else
	{
		unsigned av = (pixel & pfsrc.alpha_mask) >> pfsrc.alpha_shift;
		*a = (av << pfsrc.alpha_loss) + (av >> (8 - pfsrc.alpha_loss));
	}
}

inline void ATOM_ConvertPixel(int srcfmt, int dstfmt, const unsigned * from, unsigned * to) 
{
	unsigned char r, g, b, a;
	ATOM_DecodePixel(srcfmt, &r, &g, &b, &a, from);
	ATOM_EncodePixel(dstfmt, r, g, b, a, to);
}

#endif // __ATOM_IMAGE_PIXELFORMAT_H
