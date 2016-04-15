#ifndef __ATOM_IMAGE_IBMPCODEC_H
#define __ATOM_IMAGE_IBMPCODEC_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_kernel.h"
#include "imagecodec.h"
#include "image.h"

#ifndef BI_RGB
# define BI_RGB       0
# define BI_RLE8      1
# define BI_RLE4      2
# define BI_BITFIELDS 3
#endif

class ATOM_BmpCodec : public ATOM_ImageCodec
{
	ATOM_CLASS(image, ATOM_BmpCodec);
	ATOM_DECLARE_SCRIPT_INTERFACE(ATOM_BmpCodec)

	struct _Palette
	{
		unsigned char r;
		unsigned char g;
		unsigned char b;
		unsigned char a;
	};

public:
	ATOM_BmpCodec (void);
	virtual ~ATOM_BmpCodec (void);

public:
	virtual int getCodecType (void) const;
	virtual bool beginDecode (ATOM_RWops* input, CodecInfo* info);
	virtual bool decode (ATOM_RWops* output, CodecInfo* info);
	virtual void endDecode (CodecInfo* info);
	virtual bool beginEncode (ATOM_RWops* input, CodecInfo* info);
	virtual bool encode (ATOM_RWops* output, CodecInfo* info);
	virtual void endEncode (CodecInfo* info);
	virtual bool checkType (ATOM_RWops* input) const;

private:
	bool _M_in_decode_begin;
	bool _M_in_encode_begin;
	ATOM_RWops* _M_input;

	// Bmp header
	char bh_magic[2];
	unsigned long bh_size;
	unsigned long bh_reserved;
	unsigned long bh_offbits;

	// Bmp info
	unsigned long bi_size;
	long bi_width;
	long bi_height;
	unsigned short bi_planes;
	unsigned short bi_bitcount;
	unsigned long bi_compression;
	unsigned long bi_size_image;
	long bi_x_pels_per_meter;
	long bi_y_pels_per_meter;
	unsigned long bi_color_used;
	unsigned long bi_color_important;

	// expand flag
	bool _M_inversed;
	int _M_pitch;
	int _M_bytes_per_row;
	int _M_pixel_offset;

	// stream position
	int _M_stream_pos;

	// image instance (for writing bmp)
	ATOM_AUTOREF(ATOM_Image)	_M_image;
};

#endif // __ATOM_IMAGE_IBMPCODEC_H
