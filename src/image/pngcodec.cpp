#include "StdAfx.h"
#ifdef __CODEGEARC__
# pragma hdrstop
#endif

extern"C" {
#include <png.h>
}

#ifndef PNG_READ_EXPAND_SUPPORTED
# error !!!
#endif

ATOM_SCRIPT_INTERFACE_BEGIN(ATOM_PNGCodec)
	ATOM_ATTRIBUTES_BEGIN(ATOM_PNGCodec)
	ATOM_ATTRIBUTES_END()
ATOM_SCRIPT_INTERFACE_END(ATOM_PNGCodec, ATOM_ImageCodec)

ATOM_PNGCodec::ATOM_PNGCodec (void) 
{
	ATOM_STACK_TRACE(ATOM_PNGCodec::ATOM_PNGCodec);
	_M_in_decode_begin = false;
	_M_in_encode_begin = false;

	png_ptr = 0;
	info_ptr = 0;
	row_pointers = 0;
	write_cache = 0;
	write_cache_size = 0;
	write_cache_pos = 0;
	image_width = 0;
	image_height = 0;
	num_channels = 0;
}

ATOM_PNGCodec::~ATOM_PNGCodec (void) 
{
	ATOM_STACK_TRACE(ATOM_PNGCodec::~ATOM_PNGCodec);
}

bool ATOM_PNGCodec::checkType(ATOM_RWops* input) const 
{
	ATOM_STACK_TRACE(ATOM_PNGCodec::checkType);
	if ( input == 0)
	{
		ATOM_LOGGER::error("[ATOM_PNGCodec::CheckType] Invalid arguments: 0x%08X.\n", input);
		return false;
	}

	int pos = input->tell ();

	char acBuf[8];
	int iRead = input->read(acBuf, 1, 8);
	input->seek (pos, SEEK_SET);

	if ( iRead != 8)
	{
		ATOM_LOGGER::error("[ATOM_PNGCodec::CheckType] Couldn't read image header.\n");
		return false;
	}

	if ( png_sig_cmp((png_byte *) acBuf, (png_size_t) 0, 8))
	{
		return false;
	}

	return true;
}

bool ATOM_PNGCodec::beginEncode (ATOM_RWops* input, CodecInfo* info) 
{
	ATOM_STACK_TRACE(ATOM_PNGCodec::beginEncode);
	if ( _M_in_decode_begin || _M_in_encode_begin)
	{
		ATOM_LOGGER::error("[ATOM_PNGCodec::BeginEncode] Last decoding/encoding process not terminated.\n");
		return false;
	}

	if ( input == 0 || info == 0)
	{
		ATOM_LOGGER::error("[ATOM_PNGCodec::BeginEncode] Invalid arguments: 0x%08X, 0x%08X.\n", input, info);
		return false;
	}

	if ( info->width == 0 || info->height == 0)
	{
		ATOM_LOGGER::error("[ATOM_PNGCodec::BeginEncode] Invalid input format: %d, %d, %d.\n", info->width, info->height, info->format);
		return false;
	}

	unsigned size = info->buffersize;
	unsigned char * data = ATOM_NEW_ARRAY(unsigned char, size);
	unsigned char * dataused = data;
	int stride = info->width * (ATOM_PIXEL_FORMAT(info->format).bpp / 8);
	ATOM_PixelFormat format = info->format;

	if ( input->read(data, 1, size) != size)
	{
		ATOM_DELETE_ARRAY(data);
		ATOM_LOGGER::error("[ATOM_PNGCodec::BeginEncode] Error reading source.\n");
		return false;
	}

	ATOM_HARDREF(ATOM_Image) image;
	ATOM_ASSERT(image);

	image->init (info->width, info->height, info->format, data);
	if ( ATOM_PIXEL_FORMAT(format).bpp != 32 && ATOM_PIXEL_FORMAT(format).bpp != 24 && format != ATOM_PIXEL_FORMAT_GREY8 && format != ATOM_PIXEL_FORMAT_GREY8A8)
	{
		image->init(info->width, info->height, format, data);
		image->convertFormat(ATOM_PIXEL_FORMAT_RGBA8888);
		format = ATOM_PIXEL_FORMAT_RGBA8888;
		size = image->getBufferSize();
		dataused = image->getData();
		stride = image->getWidth() * 4;

		ATOM_DELETE_ARRAY(data);
		data = 0;
	}

	if ( !(png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0)))
	{
		ATOM_LOGGER::error("[ATOM_PNGCodec::BeginEncode] Couldn't create png struct.\n");
		ATOM_DELETE_ARRAY(data);
		return false;
	}

	if ( !(info_ptr = png_create_info_struct(png_ptr)))
	{
		png_destroy_write_struct(&png_ptr, 0);
		ATOM_LOGGER::error("[ATOM_PNGCodec::BeginEncode] Couldn't create info struct.\n");
		ATOM_DELETE_ARRAY(data);
		return false;
	}

	if ( setjmp(png_jmpbuf(png_ptr)))
	{
		ATOM_LOGGER::error("[ATOM_PNGCodec::BeginEncode] Begin encode failed.\n");
		png_destroy_write_struct(&png_ptr, &info_ptr);
		ATOM_DELETE_ARRAY(data);
		return false;
	}

	png_set_write_fn(png_ptr, (png_voidp)this, write_file_callback, 0);

	int color_type;
	bool swap_alpha = false;
	bool swap_rgb = false;
	bool strip_alpha = false;

	switch ( format)
	{
	case ATOM_PIXEL_FORMAT_GREY8:
		{
			color_type = PNG_COLOR_TYPE_GRAY;
			break;
		}
	case ATOM_PIXEL_FORMAT_RGB888:
		{
			color_type = PNG_COLOR_TYPE_RGB;
			break;
		}
	case ATOM_PIXEL_FORMAT_BGR888:
		{
			color_type = PNG_COLOR_TYPE_RGB;
			swap_rgb = true;
			break;
		}
	case ATOM_PIXEL_FORMAT_ABGR8888:
		{
			color_type = PNG_COLOR_TYPE_RGB_ALPHA;
			swap_rgb = true;
			swap_alpha = true;
			break;
		}
	case ATOM_PIXEL_FORMAT_XBGR8888:
		{
			color_type = PNG_COLOR_TYPE_RGB_ALPHA;
			swap_rgb = true;
			swap_alpha = true;
			strip_alpha = true;
			break;
		}
	case ATOM_PIXEL_FORMAT_RGBA8888:
		{
			color_type = PNG_COLOR_TYPE_RGB_ALPHA;
			break;
		}
	case ATOM_PIXEL_FORMAT_RGBX8888:
		{
			color_type = PNG_COLOR_TYPE_RGB_ALPHA;
			strip_alpha = true;
			break;
		}
	case ATOM_PIXEL_FORMAT_ARGB8888:
		{
			color_type = PNG_COLOR_TYPE_RGB_ALPHA;
			swap_alpha = true;
			break;
		}
	case ATOM_PIXEL_FORMAT_XRGB8888:
		{
			color_type = PNG_COLOR_TYPE_RGB_ALPHA;
			swap_alpha = true;
			strip_alpha = true;
			break;
		}
	case ATOM_PIXEL_FORMAT_BGRA8888:
		{
			color_type = PNG_COLOR_TYPE_RGB_ALPHA;
			swap_rgb = true;
			break;
		}
	case ATOM_PIXEL_FORMAT_BGRX8888:
		{
			color_type = PNG_COLOR_TYPE_RGB_ALPHA;
			swap_rgb = true;
			strip_alpha = true;
			break;
		}
	case ATOM_PIXEL_FORMAT_GREY8A8:
		{
			color_type = PNG_COLOR_TYPE_GRAY_ALPHA;
			break;
		}
	default:
		{
			ATOM_DELETE_ARRAY(data);
			return false;
		}
	}

	png_set_IHDR(png_ptr,
			   info_ptr,
			   info->width,
			   info->height,
			   8,
			   color_type,
			   PNG_INTERLACE_NONE,
			   PNG_COMPRESSION_TYPE_BASE,
			   PNG_FILTER_TYPE_BASE);

	png_write_info(png_ptr, info_ptr);
	png_set_packing(png_ptr);

	if ( swap_alpha)
	{
		png_set_swap_alpha(png_ptr);
	}

	if ( swap_rgb)
	{
		png_set_bgr(png_ptr);
	}

	if ( strip_alpha)
	{
		png_set_strip_alpha(png_ptr);
	}

	row_pointers = ATOM_NEW_ARRAY(png_bytep, info->height);
	for ( int i = 0; i < info->height; ++i)
	{
		row_pointers[i] = dataused + stride * i;
	}

	png_write_image(png_ptr, row_pointers);
	png_write_end(png_ptr, info_ptr);
	png_destroy_write_struct(&png_ptr, &info_ptr);

	ATOM_DELETE_ARRAY(row_pointers);
	row_pointers = 0;

	if ( data)
	{
		ATOM_DELETE_ARRAY(data);
		data = 0;
	}

	info->buffersize = write_cache_pos;
	_M_in_encode_begin = true;

	return true;
}

bool ATOM_PNGCodec::beginDecode (ATOM_RWops* input, CodecInfo* info) 
{
	ATOM_STACK_TRACE(ATOM_PNGCodec::beginDecode);
	if ( _M_in_decode_begin || _M_in_encode_begin)
	{
		ATOM_LOGGER::error("[ATOM_PNGCodec::BeginDecode] Last decoding/encoding process not terminated.\n");
		return false;
	}

	if ( input == 0)
	{
		ATOM_LOGGER::error("[ATOM_PNGCodec::BeginDecode] Invalid arguments: 0x%08X, 0x%08X.\n", input, info);
		return false;
	}

	if ( !(png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0)))
	{
		ATOM_LOGGER::error("[ATOM_PNGCodec::BeginDecode] Couldn't create png struct.\n");
		return false;
	}

	if ( !(info_ptr = png_create_info_struct(png_ptr)))
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, 0);
		ATOM_LOGGER::error("[ATOM_PNGCodec::BeginDecode] Couldn't create info struct.\n");
		return false;
	}

	if ( setjmp(png_jmpbuf(png_ptr)))
	{
		ATOM_LOGGER::error("[ATOM_PNGCodec::BeginDecode] Begin decode failed.\n");
		png_destroy_read_struct(&png_ptr, &info_ptr, 0);
		return false;
	}

	png_set_read_fn(png_ptr, (void *) input, &read_file_callback);
	png_read_info(png_ptr, info_ptr);

	int color_type, bits_depth;

	image_width = png_get_image_width(png_ptr, info_ptr);
	image_height = png_get_image_height(png_ptr, info_ptr);
	color_type = png_get_color_type(png_ptr, info_ptr);
	num_channels = (color_type == PNG_COLOR_TYPE_RGB_ALPHA) ? 4 : 3;
	bits_depth = png_get_bit_depth(png_ptr, info_ptr);

	if ( color_type == PNG_COLOR_TYPE_PALETTE)
	{
		num_channels = 3;
		color_type = PNG_COLOR_TYPE_RGB;
		png_set_palette_to_rgb(png_ptr);
	}

	if ( (color_type == PNG_COLOR_TYPE_GRAY) && (bits_depth < 8))
	{
		// FIXME!!: Not test.
		num_channels = 1;
		png_set_expand_gray_1_2_4_to_8(png_ptr);
	}

	if ( color_type == PNG_COLOR_TYPE_GRAY)
	{
		num_channels = 1;
	}

	if ( color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
	{
		num_channels = 2;
	}

	if ( png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
	{
		num_channels = 4;
		color_type = PNG_COLOR_TYPE_RGB_ALPHA;
		png_set_tRNS_to_alpha(png_ptr);
	}

	if ( bits_depth == 16)
	{
		png_set_strip_16(png_ptr);
	}

	if ( (color_type != PNG_COLOR_TYPE_RGB_ALPHA && color_type != PNG_COLOR_TYPE_RGB && color_type != PNG_COLOR_TYPE_GRAY && color_type != PNG_COLOR_TYPE_GRAY_ALPHA) || bits_depth != 8)
	{
		ATOM_LOGGER::error("[ATOM_PNGCodec::BeginDecode] Color format not supported.\n");
		png_destroy_read_struct(&png_ptr, &info_ptr, 0);
		return false;
	}

	if ( info)
	{
		info->width = image_width;
		info->height = image_height;
		switch (num_channels)
		{
		case 1: 
			{
				info->format = ATOM_PIXEL_FORMAT_GREY8; 
				break;
			}
		case 2: 
			{
				info->format = ATOM_PIXEL_FORMAT_GREY8A8; 
				break;
			}
		case 3: 
			{
				info->format = ATOM_PIXEL_FORMAT_BGR888; 
				break;
			}
		case 4: 
			{
				info->format = ATOM_PIXEL_FORMAT_BGRA8888; 
				break;
			}
		}
		info->buffersize = image_width * image_height * num_channels;
	}

	png_read_update_info(png_ptr, info_ptr);

	_M_in_decode_begin = true;
	info->codec = ATOM_ImageCodec::PNG;

	return true;
}

bool ATOM_PNGCodec::encode (ATOM_RWops* output, CodecInfo* info) 
{
	ATOM_STACK_TRACE(ATOM_PNGCodec::encode);
	if ( !_M_in_encode_begin)
	{
		ATOM_LOGGER::error("[ATOM_PNGCodec::Encode] BeginEncode not successfully called.\n");
		return false;
	}

	if ( output == 0)
	{
		ATOM_LOGGER::error("[ATOM_PNGCodec::Encode] Invalid arguments: 0x%08X.\n", output);
		return false;
	}

	if ( output->write(write_cache, 1, write_cache_pos) != write_cache_pos)
	{
		ATOM_LOGGER::error("[ATOM_PNGCodec::Encode] IO error.\n");
		return false;
	}

	return true;
}

bool ATOM_PNGCodec::decode (ATOM_RWops* output, CodecInfo* info) 
{
	ATOM_STACK_TRACE(ATOM_PNGCodec::decode);
	if ( !_M_in_decode_begin)
	{
		ATOM_LOGGER::error("[ATOM_PNGCodec::Decode] BeginDecode not successfully called.\n");
		return false;
	}

	if ( output == 0)
	{
		ATOM_LOGGER::error("[ATOM_PNGCodec::Decode] Invalid arguments: 0x%08X.\n", output);
		return false;
	}

	if ( setjmp(png_jmpbuf(png_ptr)))
	{
		ATOM_LOGGER::error("[ATOM_PNGCodec::Decode] Decode failed.\n");
		png_destroy_read_struct(&png_ptr, &info_ptr, 0);
		ATOM_DELETE_ARRAY(row_pointers);
		row_pointers = 0;       
		return false;
	}

	png_set_bgr(png_ptr);

	row_pointers = ATOM_NEW_ARRAY(png_bytep, image_height);
	for ( int row = 0; row < image_height; ++row)
	{
		row_pointers[row] = (png_bytep) png_malloc(png_ptr, png_get_rowbytes(png_ptr, info_ptr));
	}

	png_read_image(png_ptr, row_pointers);
	png_read_end(png_ptr, info_ptr);

	for ( int i = 0; i < image_height; ++i)
	{
		int bytes_to_write = image_width* num_channels;
		if ( bytes_to_write != output->write(row_pointers[i], 1, bytes_to_write))
		{
			ATOM_LOGGER::error("[ATOM_PNGCodec::Decode] IO error.\n");
			return false;
		}
	}

	return true;
}

void ATOM_PNGCodec::endEncode (CodecInfo* info) 
{
	ATOM_STACK_TRACE(ATOM_PNGCodec::endEncode);
	if ( !_M_in_encode_begin)
	{
		ATOM_LOGGER::error("[ATOM_PNGCodec::EndEncode] BeginEncode not successfully called.\n");
		return;
	}

	_M_in_encode_begin = false;

	ATOM_FREE(write_cache);
	write_cache = 0;
	write_cache_size = 0;
	write_cache_pos = 0;
}

void ATOM_PNGCodec::endDecode (CodecInfo* info) 
{
	ATOM_STACK_TRACE(ATOM_PNGCodec::endDecode);
	if ( !_M_in_decode_begin)
	{
		ATOM_LOGGER::error("[ATOM_PNGCodec::EndDecode] BeginDecode not successfully called.\n");
		return;
	}

	_M_in_decode_begin = false;

	if ( row_pointers)
	{
		for ( int row = 0; row < image_height; ++row)
		{
			png_free(png_ptr, row_pointers[row]);
		}
		ATOM_DELETE_ARRAY(row_pointers);
		row_pointers = 0;
	}

	png_destroy_read_struct(&png_ptr, &info_ptr, 0);
}

void ATOM_PNGCodec::read_file_callback (png_struct* pPng, png_bytep pucData, png_size_t iNum) 
{
	ATOM_STACK_TRACE(ATOM_PNGCodec::read_file_callback);
	ATOM_RWops* input = (ATOM_RWops*) png_get_io_ptr(pPng);
	input->read(pucData, 1, iNum);
}

void ATOM_PNGCodec::write_file_callback (png_struct* pPng, png_bytep pucData, png_size_t iNum) 
{
	ATOM_STACK_TRACE(ATOM_PNGCodec::write_file_callback);
#define CHUNK_SIZE 4096
	ATOM_PNGCodec* codec = (ATOM_PNGCodec*) png_get_io_ptr(pPng);
	if ( codec->write_cache_pos + int(iNum) > codec->write_cache_size)
	{
		int n = (iNum - (codec->write_cache_size - codec->write_cache_pos) + (CHUNK_SIZE - 1)) / CHUNK_SIZE;
		codec->write_cache_size += CHUNK_SIZE * n;
		ATOM_ASSERT(codec->write_cache_pos + int(iNum) <= codec->write_cache_size);
		codec->write_cache = (unsigned char *)ATOM_REALLOC(codec->write_cache, codec->write_cache_size);
	}
#undef CHUNK_SIZE
	memcpy(codec->write_cache + codec->write_cache_pos, pucData, iNum);
	codec->write_cache_pos += iNum;
}

int ATOM_PNGCodec::getCodecType (void) const 
{
  return ATOM_ImageCodec::PNG;
}
