#include "StdAfx.h"
#ifdef __CODEGEARC__
# pragma hdrstop
#endif
//#include "squish.h"
#include "ddsheader.h"

ATOM_SCRIPT_INTERFACE_BEGIN(ATOM_DDSCodec)
	ATOM_ATTRIBUTES_BEGIN(ATOM_DDSCodec)
	ATOM_ATTRIBUTES_END()
ATOM_SCRIPT_INTERFACE_END(ATOM_DDSCodec, ATOM_ImageCodec)

int ATOM_DDSCodec::getCodecType (void) const
{
	return ATOM_ImageCodec::DDS;
}

bool ATOM_DDSCodec::beginDecode (ATOM_RWops* input, CodecInfo* info)
{
	ATOM_STACK_TRACE(ATOM_DDSCodec::beginDecode);
	if ( input == 0)
	{
		return false;
	}

	int length = input->size();
	if ( length <= 0)
	{
		return false;
	}

	ATOM_DDS_header hdr;

	if (input->read (&hdr, 1, sizeof(hdr)) != sizeof(hdr))
	{
		return false;
	}

	if (hdr.dwMagic != DDS_MAGIC)
	{
		return false;
	}

	if (hdr.dwSize != 124)
	{
		return false;
	}

	if (!(hdr.dwFlags & DDSD_PIXELFORMAT))
	{
		return false;
	}

	if (!(hdr.dwFlags & DDSD_CAPS))
	{
		return false;
	}

	int blocksize;
	info->codec = ATOM_ImageCodec::DDS;
	info->width = hdr.dwWidth;
	info->height = hdr.dwHeight;

	if(PF_IS_DXT1(hdr.sPixelFormat))
	{
		info->format = ATOM_PIXEL_FORMAT_DXT1;
		blocksize = 8;
	}
	else if (PF_IS_DXT3(hdr.sPixelFormat))
	{
		info->format = ATOM_PIXEL_FORMAT_DXT3;
		blocksize = 16;
	}
	else if (PF_IS_DXT5(hdr.sPixelFormat))
	{
		info->format = ATOM_PIXEL_FORMAT_DXT5;
		blocksize = 16;
	}
	else
	{
		return false;
	}

	info->mipcount = (hdr.dwFlags & DDSD_MIPMAPCOUNT) ? hdr.dwMipMapCount : 1;
	size_t w = hdr.dwWidth;
	size_t h = hdr.dwHeight;
	const size_t blockdiv = 4;
	info->buffersize = 0;
	for (int i = 0; i < info->mipcount; ++i)
	{
		info->buffersize += ATOM_max2(blockdiv, w)/blockdiv * ATOM_max2(blockdiv, h)/blockdiv * blocksize;
		w = (w+1)>>1;
		h = (h+1)>>1;
	}

	info->reserved = ATOM_MALLOC(info->buffersize);
	ATOM_ASSERT (info->reserved);

	if (input->read (info->reserved, 1, info->buffersize) != unsigned(info->buffersize))
	{
		ATOM_FREE(info->reserved);
		return false;
	}

	return true;
}

bool ATOM_DDSCodec::decode (ATOM_RWops* output, CodecInfo* info)
{
	ATOM_STACK_TRACE(ATOM_DDSCodec::decode);
	if ( output == 0)
	{
		ATOM_LOGGER::error ("[ATOM_BmpCodec::Decode] Invalid arguments: 0x%08X.\n", output);
		return false;
	}

	if (output->write (info->reserved, 1, info->buffersize) != unsigned(info->buffersize))
	{
		return false;
	}

	return true;
}

void ATOM_DDSCodec::endDecode (CodecInfo* info)
{
	ATOM_STACK_TRACE(ATOM_DDSCodec::endDecode);
	if (info->reserved)
	{
		ATOM_FREE(info->reserved);
		info->reserved = 0;
	}
}

bool ATOM_DDSCodec::beginEncode (ATOM_RWops* input, CodecInfo* info)
{
	ATOM_STACK_TRACE(ATOM_DDSCodec::beginEncode);
	if ( input == 0 || info == 0)
	{
		ATOM_LOGGER::error ("[ATOM_PNGCodec::BeginEncode] Invalid arguments: 0x%08X, 0x%08X.\n", input, info);
		return false;
	}

	if ( info->width == 0 || info->height == 0 || !ATOM_IS_VALID_FORMAT(info->format))
	{
		ATOM_LOGGER::error ("[ATOM_DDSCodec::BeginEncode] Invalid input format: %d, %d, %d.\n", info->width, info->height, info->format);
		return false;
	}

	if (!ATOM_PIXEL_FORMAT(info->format)._is_compressed_format && (info->width % 4 || info->height % 4))
	{
		ATOM_LOGGER::error ("[ATOM_DDSCodec::BeginEncode] Invalid source image dimension: %d, %d.\n", info->width, info->height);
		return false;
	}

	ATOM_BaseImage *cookie = ATOM_NEW(ATOM_BaseImage);
	ATOM_ASSERT (cookie);
	cookie->init (info->width, info->height, info->format, 0, info->mipcount);

	if (input->read (cookie->getData(), 1, cookie->getBufferSize()) != unsigned(cookie->getBufferSize()))
	{
		ATOM_DELETE(cookie);
		return false;
	}

	info->codec = ATOM_ImageCodec::DDS;
	info->reserved = cookie;

	return true;
}

bool ATOM_DDSCodec::encode (ATOM_RWops* output, CodecInfo* info)
{
	ATOM_STACK_TRACE(ATOM_DDSCodec::encode);
	if (!output || !info)
	{
		return false;
	}

	ATOM_BaseImage *image = (ATOM_BaseImage*)info->reserved;

	ATOM_PixelFormat outFormat = image->getFormat();

	if (!ATOM_PIXEL_FORMAT(outFormat)._is_compressed_format)
	{
		if (!ATOM_PIXEL_FORMAT(info->format)._is_compressed_format)
		{
			ATOM_DELETE (image);
			ATOM_LOGGER::error ("ATOM_DDSCodec::Encode: Only support DXT1/3/5 format.\n");
			return false;
		}
		outFormat = info->format;
		image->convertFormat (outFormat);
	}

	ATOM_DDS_header hdr;

	memset (&hdr, 0, sizeof(hdr));
	hdr.dwMagic = DDS_MAGIC;
	hdr.dwSize = 124;
	hdr.dwFlags = DDSD_LINEARSIZE|DDSD_PIXELFORMAT|DDSD_CAPS|DDSD_MIPMAPCOUNT|DDSD_WIDTH|DDSD_HEIGHT;
	hdr.dwWidth = image->getWidth();
	hdr.dwHeight = image->getHeight();
	hdr.dwPitchOrLinearSize = image->getBufferSize();
	hdr.dwMipMapCount = image->getNumMipmaps();

	hdr.sPixelFormat.dwSize = 32;
	hdr.sPixelFormat.dwFlags = DDPF_FOURCC;
	switch (image->getFormat())
	{
	case ATOM_PIXEL_FORMAT_DXT1:
		{
			hdr.sPixelFormat.dwFourCC = D3DFMT_DXT1;
			break;
		}
	case ATOM_PIXEL_FORMAT_DXT3:
		{
			hdr.sPixelFormat.dwFourCC = D3DFMT_DXT3;
			break;
		}
	case ATOM_PIXEL_FORMAT_DXT5:
		{
			hdr.sPixelFormat.dwFourCC = D3DFMT_DXT5;
			break;
		}
	}

	hdr.sCaps.dwCaps1 = DDSCAPS_TEXTURE|DDSCAPS_MIPMAP|DDSCAPS_COMPLEX;

	if (output->write (&hdr, 1, sizeof(hdr)) != sizeof(hdr))
	{
		ATOM_DELETE(image);
		return false;
	}

	if (output->write (image->getData(), 1, image->getBufferSize()) != unsigned(image->getBufferSize()))
	{
		ATOM_DELETE(image);
		return false;
	}

	ATOM_DELETE(image);
	return true;
}

void ATOM_DDSCodec::endEncode (CodecInfo* info)
{
}

bool ATOM_DDSCodec::checkType (ATOM_RWops* input) const
{
	ATOM_STACK_TRACE(ATOM_DDSCodec::checkType);
	if ( input == 0)
	{
		return false;
	}

	int length = input->size();
	if ( length <= 0)
	{
		return false;
	}

	ATOM_DDS_header hdr;

	size_t pos = input->tell ();

	if (input->read (&hdr, 1, sizeof(hdr)) != sizeof(hdr))
	{
		input->seek (pos, SEEK_SET);
		return false;
	}

	if (hdr.dwMagic != DDS_MAGIC)
	{
		input->seek (pos, SEEK_SET);
		return false;
	}

	if (hdr.dwSize != 124)
	{
		input->seek (pos, SEEK_SET);
		return false;
	}

	if (!(hdr.dwFlags & DDSD_PIXELFORMAT))
	{
		input->seek (pos, SEEK_SET);
		return false;
	}

	if (!(hdr.dwFlags & DDSD_CAPS))
	{
		input->seek (pos, SEEK_SET);
		return false;
	}

	input->seek (pos, SEEK_SET);
	return true;
}


