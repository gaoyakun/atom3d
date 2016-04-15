#ifndef __ATOM_IMAGE_IMAGECODEC_H
#define __ATOM_IMAGE_IMAGECODEC_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_kernel.h"
#include "basedefs.h"

enum 
{
	GIF_DISPOSAL_NONE,
	GIF_DISPOSAL_LEAVEALONE,
	GIF_DISPOSAL_BKGROUND,
	GIF_DISPOSAL_PREVIOUS
};

struct ATOM_GifFrameDesc
{
	int width;
	int height;
	int left;
	int top;
	int disposal;
	bool transparent;
	unsigned char colorkey[3];
	unsigned char bkcolor[3];
	long delay;
};

struct ATOM_GifImageDesc
{
	unsigned framecount;
	unsigned screenwidth;
	unsigned screenheight;
	unsigned frameindex;
	bool expanded;
	bool alphachannel;
	ATOM_GifFrameDesc* descriptions;
	void* reserved;
};

class ATOM_IMAGE_API ATOM_ImageCodec : public ATOM_Object
{
	ATOM_DECLARE_SCRIPT_INTERFACE(ATOM_ImageCodec)

public:
	enum 
	{
		JPEG = 0,
		PNG = 1,
		BMP = 2,
		GIF = 3,
		TIFF = 4,
		DDS = 5,
		TGA = 6,
		ANY = 7,
	};

	struct CodecInfo
	{
		int codec;
		int width;
		int height;
		ATOM_PixelFormat format;
		int mipcount;
		int buffersize;
		void* reserved;
		CodecInfo (void) { memset(this, 0, sizeof(CodecInfo)); } 
	};

public:
	virtual ~ATOM_ImageCodec();

public:
	virtual int getCodecType (void) const = 0;
	virtual bool beginDecode (ATOM_RWops* input, CodecInfo* info) = 0;
	virtual bool decode (ATOM_RWops* output, CodecInfo* info) = 0;
	virtual void endDecode (CodecInfo* info) = 0;
	virtual bool beginEncode (ATOM_RWops* input, CodecInfo* info) = 0;
	virtual bool encode (ATOM_RWops* output, CodecInfo* info) = 0;
	virtual void endEncode (CodecInfo* info) = 0;
	virtual bool checkType (ATOM_RWops* input) const = 0;

public:
	static int guessTypeByExtension(const char* ext);
	static int guessTypeByFileName(const char *fn);
};

#endif // __SHINE3D_IIMAGECODEC_H
