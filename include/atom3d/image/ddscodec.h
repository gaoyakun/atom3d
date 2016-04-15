#ifndef __ATOM_IMAGE_DDSCODEC_H
#define __ATOM_IMAGE_DDSCODEC_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "imagecodec.h"

class ATOM_DDSCodec: public ATOM_ImageCodec
{
	ATOM_CLASS(image, ATOM_DDSCodec, ATOM_DDSCodec);
	ATOM_DECLARE_SCRIPT_INTERFACE(ATOM_DDSCodec)

public:
	virtual int getCodecType (void) const;
	virtual bool beginDecode (ATOM_RWops* input, CodecInfo* info);
	virtual bool decode (ATOM_RWops* output, CodecInfo* info);
	virtual void endDecode (CodecInfo* info);
	virtual bool beginEncode (ATOM_RWops* input, CodecInfo* info);
	virtual bool encode (ATOM_RWops* output, CodecInfo* info);
	virtual void endEncode (CodecInfo* info);
	virtual bool checkType (ATOM_RWops* input) const;
};

#endif // __ATOM_IMAGE_DDSCODEC_H

