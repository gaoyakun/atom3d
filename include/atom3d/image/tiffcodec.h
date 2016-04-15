#ifndef __ATOM_IMAGE_ITIFFCODEC_H
#define __ATOM_IMAGE_ITIFFCODEC_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_kernel.h"
#include "imagecodec.h"
#include "image.h"

class ATOM_TIFFCodec : public ATOM_ImageCodec
{
	ATOM_CLASS(image, ATOM_TIFFCodec, ATOM_TIFFCodec);
	ATOM_DECLARE_SCRIPT_INTERFACE(ATOM_TIFFCodec)

public:
	ATOM_TIFFCodec (void);
	virtual ~ATOM_TIFFCodec (void);

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
	unsigned _M_width;
	unsigned _M_height;
	int _M_format;
	void* _M_tiff;
};
#endif // __ATOM_IMAGE_ITIFFCODEC_H
