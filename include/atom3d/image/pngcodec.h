#ifndef __ATOM_IMAGE_IPNGCODEC_H
#define __ATOM_IMAGE_IPNGCODEC_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <png.h>
#include "imagecodec.h"

class ATOM_Image;

class ATOM_PNGCodec : public ATOM_ImageCodec
{
	ATOM_CLASS(image, ATOM_PNGCodec, ATOM_PNGCodec);
	ATOM_DECLARE_SCRIPT_INTERFACE(ATOM_PNGCodec)

public:
	ATOM_PNGCodec (void);
	virtual ~ATOM_PNGCodec (void);

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
	static void read_file_callback(png_struct* pPng, png_bytep pucData, png_size_t iNum);
	static void write_file_callback(png_struct* pPng, png_bytep pucData, png_size_t iNum);

	png_structp png_ptr;
	png_infop info_ptr;
	png_bytep* row_pointers;
	png_bytep write_cache;
	int write_cache_size;
	int write_cache_pos;
	int image_width;
	int image_height;
	int num_channels;
	bool _M_in_decode_begin;
	bool _M_in_encode_begin;
};

#endif // __ATOM_IMAGE_IPNGCODEC_H
