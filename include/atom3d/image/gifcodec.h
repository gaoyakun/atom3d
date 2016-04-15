#ifndef __ATOM_IMAGE_IGIFCODEC_H
#define __ATOM_IMAGE_IGIFCODEC_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_kernel.h"
#include "imagecodec.h"
#include "image.h"

class ATOM_GIFDecoder;
class ATOM_GIFCodec : public ATOM_ImageCodec
{
	ATOM_CLASS(image, ATOM_GIFCodec, ATOM_GIFCodec);
	ATOM_DECLARE_SCRIPT_INTERFACE(ATOM_GIFCodec)

public:
	ATOM_GIFCodec();
	virtual ~ATOM_GIFCodec();

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
	ATOM_GIFDecoder* _M_decoder;
};

#endif // __ATOM_IMAGE_IGIFCODEC_H
