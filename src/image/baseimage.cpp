#include "StdAfx.h"
#ifdef __CODEGEARC__
# pragma hdrstop
#endif

extern bool CreateDXTImage (ATOM_PixelFormat format, ATOM_BaseImage *src);
extern bool CreateFromDXTImage (ATOM_PixelFormat format, ATOM_BaseImage *src);

ATOM_BaseImage::ATOM_BaseImage (void) 
{
	ATOM_STACK_TRACE(ATOM_BaseImage::ATOM_BaseImage);
	_M_width = 0;
	_M_height = 0;
	_M_format = ATOM_PIXEL_FORMAT_END;
	_M_nummips = 0;
	_M_buffersize = 0;
	_M_actual_size = 0;
	_M_contents = 0;
}

ATOM_BaseImage::ATOM_BaseImage (unsigned width, unsigned height, ATOM_PixelFormat format, const void *data, int mipcount) 
{
	ATOM_STACK_TRACE(ATOM_BaseImage::ATOM_BaseImage);
	_M_width = 0;
	_M_height = 0;
	_M_format = ATOM_PIXEL_FORMAT_END;
	_M_nummips = 0;
	_M_buffersize = 0;
	_M_actual_size = 0;
	_M_contents = 0;

	init (width, height, format, data, mipcount);
}

ATOM_BaseImage::ATOM_BaseImage (ATOM_RWops *input, int type) 
{
	ATOM_STACK_TRACE(ATOM_BaseImage::ATOM_BaseImage);
	_M_width = 0;
	_M_height = 0;
	_M_format = ATOM_PIXEL_FORMAT_END;
	_M_nummips = 0;
	_M_buffersize = 0;
	_M_actual_size = 0;
	_M_contents = 0;

	init (input, type);
}

ATOM_BaseImage::ATOM_BaseImage (const ATOM_BaseImage &rhs) 
{
	ATOM_STACK_TRACE(ATOM_BaseImage::ATOM_BaseImage);
	_M_width = 0;
	_M_height = 0;
	_M_format = ATOM_PIXEL_FORMAT_END;
	_M_nummips = 0;
	_M_buffersize = 0;
	_M_actual_size = 0;
	_M_contents = 0;

	init (rhs._M_width, rhs._M_height, rhs._M_format, rhs._M_contents, rhs._M_nummips);
}

ATOM_BaseImage & ATOM_BaseImage::operator = (const ATOM_BaseImage &rhs) 
{
	ATOM_STACK_TRACE(ATOM_BaseImage::operator=);
	if (&rhs != this)
	{
		ATOM_BaseImage tmp(rhs);
		swap (tmp);
	}
	return *this;
}

ATOM_BaseImage::~ATOM_BaseImage (void) 
{
	ATOM_STACK_TRACE(ATOM_BaseImage::~ATOM_BaseImage);
      fini ();
}

void ATOM_BaseImage::init (unsigned width, unsigned height, ATOM_PixelFormat format, const void* data, int mipcount) 
{
	ATOM_STACK_TRACE(ATOM_BaseImage::init);
	if ( width == 0 || height == 0)
	{
		fini ();
		return;
	}

	if ( !ATOM_IS_VALID_FORMAT(format))
	{
		ATOM_LOGGER::error ("[ATOM_BaseImage::init] Bad arguments: %d, %d, %d.\n", width, height, format);
		return;
	}

	unsigned buffersize = 0;
	const int blockdiv = 4;

	switch (format)
	{
	case ATOM_PIXEL_FORMAT_DXT1:
	case ATOM_PIXEL_FORMAT_DXT3:
	case ATOM_PIXEL_FORMAT_DXT5:
		{
			int blocksize = (format == ATOM_PIXEL_FORMAT_DXT1) ? 8 : 16;
			unsigned w = width;
			unsigned h = height;
			for (int i = 0; i < mipcount; ++i)
			{
				buffersize += ATOM_max2(blockdiv, w)/blockdiv * ATOM_max2(blockdiv, h)/blockdiv * blocksize;
				w = (w+1)>>1;
				h = (h+1)>>1;
			}
			break;
		}
	default:
		{
			unsigned bpp = ATOM_PIXEL_FORMAT(format).bpp / 8;
			buffersize = width * height * bpp;
			break;
		}
	}

	if (_M_contents && buffersize > _M_actual_size)
	{
		fini();
	}

	_M_buffersize = buffersize;

	if (!_M_contents)
	{
		_M_contents = ATOM_NEW_ARRAY(unsigned char, _M_buffersize);
		ATOM_ASSERT (_M_contents);
		_M_actual_size = _M_buffersize;
	}

	if ( data)
	{
		memcpy(_M_contents, data, _M_buffersize);
	}

	_M_width = width;
	_M_height = height;
	_M_format = format;
	_M_nummips = mipcount;
}

bool ATOM_BaseImage::init (ATOM_RWops* input, int type, ATOM_PixelFormat format) 
{
	ATOM_STACK_TRACE(ATOM_BaseImage::init);
	fini();

	ATOM_ImageCodec *codec = 0;

	switch ( type)
	{
	case ATOM_ImageCodec::JPEG:
		{
			codec = ATOM_NEW(ATOM_JPEGCodec);
			break;
		}
	case ATOM_ImageCodec::PNG:
		{
			codec = ATOM_NEW(ATOM_PNGCodec);
			break;
		}
	case ATOM_ImageCodec::DDS:
		{
			codec = ATOM_NEW(ATOM_DDSCodec);
			break;
		}
	case ATOM_ImageCodec::ANY:
		{
			codec = ATOM_NEW(ATOM_AnyCodec);
			break;
		}
	}

	if ( codec == 0)
	{
		ATOM_LOGGER::error ("[ATOM_Image::init] Couldn't create decoder.\n");
		return false;
	}

	ATOM_ImageCodec::CodecInfo info;
	if ( !codec->beginDecode(input, &info))
	{
		ATOM_LOGGER::error ("[ATOM_Image::init] Couldn't decode image.\n");
		ATOM_DELETE(codec);
		return false;
	}

	ATOM_RWops *output;

	init(info.width, info.height, info.format, 0, info.mipcount);
	output = ATOM_RWops::createMemRWops(getData(), getBufferSize());
	ATOM_ASSERT(output);

	if ( !codec->decode(output, &info))
	{
		ATOM_LOGGER::error ("[ATOM_Image::init] Couldn't decode image.\n");
		ATOM_RWops::destroyRWops(output);
		ATOM_DELETE(codec);
		fini ();
		return false;
	}

	_M_nummips = info.mipcount;

	codec->endDecode(&info);

	ATOM_RWops::destroyRWops(output);
	ATOM_DELETE(codec);

	if (format != ATOM_PIXEL_FORMAT_END)
	{
		convertFormat(format);
	}

	return true;
}

void ATOM_BaseImage::fini (void) 
{
	ATOM_STACK_TRACE(ATOM_BaseImage::fini);
	ATOM_DELETE_ARRAY(_M_contents);
	_M_contents = 0;
	_M_width = 0;
	_M_height = 0;
	_M_format = ATOM_PIXEL_FORMAT_END;
	_M_buffersize = 0;
	_M_actual_size = 0;
}

void ATOM_BaseImage::castFormat (ATOM_PixelFormat format) 
{
	ATOM_STACK_TRACE(ATOM_BaseImage::castFormat);
	if (!ATOM_IS_VALID_FORMAT(format) || !ATOM_IS_VALID_FORMAT(_M_format))
	{
		return;
	}

	if (ATOM_PIXEL_FORMAT(_M_format).bpp != ATOM_PIXEL_FORMAT(format).bpp)
	{
		ATOM_LOGGER::warning ("Cast format with different pixel size.\n");
	}

	_M_format = format;
}

void ATOM_BaseImage::convertFormat (ATOM_PixelFormat format) 
{
	ATOM_STACK_TRACE(ATOM_BaseImage::convertFormat);
	if ( format == _M_format)
	{
		return;
	}

	if ( !ATOM_IS_VALID_FORMAT(format) || !ATOM_IS_VALID_FORMAT(_M_format) || !_M_contents || _M_width == 0 || _M_height == 0)
	{
		ATOM_LOGGER::error("ATOM_BaseImage::convertFormat: Bad argument format %d\n", format);
		return;
	}

	if (ATOM_PIXEL_FORMAT(_M_format)._is_compressed_format)
	{
		if (ATOM_PIXEL_FORMAT(format)._is_compressed_format)
		{
			convertFromDXTn (ATOM_PIXEL_FORMAT_RGBA8888);
			convertToDXTn (format);
		}
		else
		{
			convertFromDXTn (format);
		}
	}
	else if (ATOM_PIXEL_FORMAT(format)._is_compressed_format)
	{
		convertToDXTn (format);
	}
	else if (format == ATOM_PIXEL_FORMAT_GREY8)
	{
		convertToGray ();
	}
	else
	{
		int srcbpp = ATOM_PIXEL_FORMAT(_M_format).bpp;
		int dstbpp = ATOM_PIXEL_FORMAT(format).bpp;
		unsigned size = _M_width * _M_height * (dstbpp/8);

		unsigned char * newdata = (unsigned char*)ATOM_STACK_ALLOC(size);

		unsigned char * iter = newdata;
		unsigned char * olddata = _M_contents;

		for ( unsigned i = 0; i < _M_width*_M_height; ++i)
		{
			ATOM_ConvertPixel(_M_format, format, (unsigned *) olddata, (unsigned *) iter);
			iter += (dstbpp / 8);
			olddata += (srcbpp / 8);
		}

		init (_M_width, _M_height, format, newdata, _M_nummips);

		ATOM_STACK_FREE(newdata, size);
	}
}

void ATOM_BaseImage::convertToGray (void)
{
	ATOM_STACK_TRACE(ATOM_BaseImage::convertToGray);
	if (_M_format == ATOM_PIXEL_FORMAT_GREY8)
	{
		return;
	}

	if (_M_format == ATOM_PIXEL_FORMAT_A8)
	{
		_M_format = ATOM_PIXEL_FORMAT_GREY8;
		return;
	}

	if (_M_format == ATOM_PIXEL_FORMAT_GREY8A8)
	{
		for (unsigned i = 1; i < _M_width * _M_height; ++i)
		{
			_M_contents[i] = _M_contents[2*i];
		}

		_M_format = ATOM_PIXEL_FORMAT_GREY8;
		return;
	}

	unsigned size = _M_width * _M_height;
	unsigned char * newdata = (unsigned char*)ATOM_STACK_ALLOC(size);

	unsigned char * iter = newdata;
	unsigned char * olddata = _M_contents;
	int srcstep = ATOM_PIXEL_FORMAT(_M_format).bpp / 8;

	for ( unsigned i = 0; i < size; ++i)
	{
		unsigned char r, g, b, a;
		ATOM_DecodePixel (_M_format,	&r, &g, &b, &a, (unsigned*)olddata);
		*iter++ = r * 0.3f + g * 0.59f + b * 0.11f;
		olddata += srcstep;
	}

	init (_M_width, _M_height, ATOM_PIXEL_FORMAT_GREY8, newdata);

	ATOM_STACK_FREE(newdata, size);
}

void ATOM_BaseImage::convertToDXTn (ATOM_PixelFormat format)
{
	ATOM_STACK_TRACE(ATOM_BaseImage::convertToDXTn);
	if (!CreateDXTImage (format, this))
	{
		ATOM_LOGGER::error ("ATOM_BaseImage::convertToDXTn: Conversion failed.\n");
	}
}

void ATOM_BaseImage::convertFromDXTn (ATOM_PixelFormat format)
{
	ATOM_STACK_TRACE(ATOM_BaseImage::convertFromDXTn);
	if (!CreateFromDXTImage (format, this))
	{
		ATOM_LOGGER::error ("ATOM_BaseImage::convertToDXTn: Conversion failed.\n");
	}
}

void ATOM_BaseImage::blitTo (ATOM_BaseImage *dst, int x, int y, int w, int h, int tx, int ty) 
{
	ATOM_STACK_TRACE(ATOM_BaseImage::blitTo);
	if ( !dst || w < 0 || h < 0)
	{
		ATOM_LOGGER::error ("[ATOM_Image::BlitTo] Invalid arguments: 0x%08X, %d, %d, %d, %d, %d, %d.\n", 0, x, y, w, h, tx, ty);
		return;
	}

	if ( !_M_contents || !dst->_M_contents)
	{
		ATOM_LOGGER::error ("[ATOM_Image::BlitTo] Image contains no data.\n");
		return;
	}

	if ( w == 0)
	{
		w = _M_width;
	}

	if ( h == 0)
	{
		h = _M_height;
	}

	int srcx = x;
	int srcy = y;
	int width = w;
	int height = h;

	if ( srcx < 0)
	{
		width += srcx;
		tx -= srcx;
		srcx = 0;
	}

	int maxw = _M_width - srcx;
	if ( maxw < width)
	{
		width = maxw;
	}

	if ( srcy < 0)
	{
		height += srcy;
		ty -= srcy;
		srcy = 0;
	}

	int maxh = _M_height - srcy;
	if ( maxh < height)
	{
		height = maxh;
	}

	int dx = -tx;
	if ( dx > 0)
	{
		width -= dx;
		tx += dx;
		srcx += dx;
	}

	dx = tx + width - dst->_M_width;
	if ( dx > 0)
	{
		width -= dx;
	}

	int dy = -ty;
	if ( dy > 0)
	{
		height -= dy;
		ty += dy;
		srcy += dy;
	}

	dy = ty + height - dst->_M_height;
	if ( dy > 0)
	{
		height -= dy;
	}

	if ( width > 0 && height > 0)
	{
		if ( this == dst)
		{
			ATOM_BaseImage(*this).blitTo(this, srcx, srcy, width, height, tx, ty);
		}
		else
		{
			unsigned char * dstdata = dst->getData();
			unsigned char * srcdata = _M_contents;
			int m = ATOM_PIXEL_FORMAT(_M_format).bpp / 8;
			int n = ATOM_PIXEL_FORMAT(dst->_M_format).bpp / 8;

			if ( _M_format == dst->_M_format)
			{
				unsigned char * pdest = dstdata + ty * dst->_M_width * n + tx * n;
				unsigned char * psrc = srcdata + srcy * _M_width * m + srcx * m;

				for ( int i = 0; i < height; ++i)
				{
					memcpy(pdest, psrc, width * m);
					pdest += dst->_M_width * n;
					psrc += _M_width * m;
				}
			}
			else
			{
				unsigned char tmp[4];

				switch ( n)
				{
				case 1:
					{
						for ( int i = 0; i < height; ++i)
						{
							unsigned char * pdest = dstdata + (ty + i) * dst->_M_width * n + tx * n;
							unsigned char * psrc = srcdata + (srcy + i) * _M_width * m + srcx * m;

							for ( int j = 0; j < width; ++j)
							{
								ATOM_ConvertPixel(_M_format, dst->_M_format, (unsigned *) psrc, (unsigned *) tmp);
								*pdest++ = tmp[0];
								psrc += m;
							}
						}
						break;
					}
				case 2:
					{
						for ( int i = 0; i < height; ++i)
						{
							unsigned char * pdest = dstdata + (ty + i) * dst->_M_width * n + tx * n;
							unsigned char * psrc = srcdata + (srcy + i) * _M_width * m + srcx * m;

							for ( int j = 0; j < width; ++j)
							{
								ATOM_ConvertPixel(_M_format, dst->_M_format, (unsigned *) psrc, (unsigned *) tmp);
								*pdest++ = tmp[0];
								*pdest++ = tmp[1];
								psrc += m;
							}
						}
						break;
					}
				case 3:
					{
						for ( int i = 0; i < height; ++i)
						{
							unsigned char * pdest = dstdata + (ty + i) * dst->_M_width * n + tx * n;
							unsigned char * psrc = srcdata + (srcy + i) * _M_width * m + srcx * m;

							for ( int j = 0; j < width; ++j)
							{
								ATOM_ConvertPixel(_M_format, dst->_M_format, (unsigned *) psrc, (unsigned *) tmp);
								*pdest++ = tmp[0];
								*pdest++ = tmp[1];
								*pdest++ = tmp[2];
								psrc += m;
							}
						}
						break;
					}
				case 4:
					{
						for ( int i = 0; i < height; ++i)
						{
							unsigned char * pdest = dstdata + (ty + i) * dst->_M_width * n + tx * n;
							unsigned char * psrc = srcdata + (srcy + i) * _M_width * m + srcx * m;

							for ( int j = 0; j < width; ++j)
							{
								ATOM_ConvertPixel(_M_format, dst->_M_format, (unsigned *) psrc, (unsigned *) tmp);
								*pdest++ = tmp[0];
								*pdest++ = tmp[1];
								*pdest++ = tmp[2];
								*pdest++ = tmp[3];
								psrc += m;
							}
						}
						break;
					}
				default:
					{
						break;
					}
				}
			}
		}
	}
}

void ATOM_BaseImage::flip (void) 
{
	ATOM_STACK_TRACE(ATOM_BaseImage::flip);
	if (_M_contents)
	{
		if (ATOM_PIXEL_FORMAT(_M_format)._is_compressed_format)
		{
			ATOM_LOGGER::error ("ATOM_BaseImage::flip S3TC compressed format flipping not implemented.\n");
			return;
		}

		unsigned pitch = _M_width * (ATOM_PIXEL_FORMAT(_M_format).bpp / 8);
		unsigned char * buffer = (unsigned char*)ATOM_STACK_ALLOC(pitch);

		unsigned char * top = _M_contents;
		unsigned char * bot = _M_contents + (_M_height - 1) * pitch;

		for ( unsigned i = 0; i < _M_height / 2; ++i)
		{
			memcpy(buffer, top, pitch);
			memcpy(top, bot, pitch);
			memcpy(bot, buffer, pitch);

			top += pitch;
			bot -= pitch;
		}

		ATOM_STACK_FREE(buffer, pitch);
	}
}

void ATOM_BaseImage::resize (unsigned widthout, unsigned heightout)
{
	ATOM_STACK_TRACE(ATOM_BaseImage::resize);
	if (widthout < 1 || heightout < 1)
	{
		ATOM_LOGGER::error ("ATOM_BaseImage::resize Bad argument\n");
		return;
	}

	if ( !_M_contents)
	{
		ATOM_LOGGER::error ("ATOM_BaseImage::resize Image is empty.\n");
		return;
	}

	if (ATOM_PIXEL_FORMAT(_M_format)._is_compressed_format)
	{
		ATOM_LOGGER::error ("ATOM_BaseImage::resize S3TC compressed format resizing not implemented.\n");
		return;
	}

	ATOM_PixelFormat savedFormat = _M_format;
	ATOM_PixelFormat newFormat = _M_format;
	int components;
	switch (_M_format)
	{
	case ATOM_PIXEL_FORMAT_A8:
	case ATOM_PIXEL_FORMAT_GREY8:
		{
			components = 1;
			break;
		}
	case ATOM_PIXEL_FORMAT_GREY8A8:
		{
			components = 2;
			break;
		}
	case ATOM_PIXEL_FORMAT_ABGR4444:
	case ATOM_PIXEL_FORMAT_ARGB1555:
	case ATOM_PIXEL_FORMAT_ARGB4444:
	case ATOM_PIXEL_FORMAT_BGRA4444:
	case ATOM_PIXEL_FORMAT_RGBA4444:
	case ATOM_PIXEL_FORMAT_RGBA5551:
		{
			convertFormat (ATOM_PIXEL_FORMAT_RGBA8888);
			newFormat = ATOM_PIXEL_FORMAT_RGBA8888;
			components = 4;
			break;
		}
	case ATOM_PIXEL_FORMAT_BGRX4444:
	case ATOM_PIXEL_FORMAT_BGR565:
	case ATOM_PIXEL_FORMAT_RGBX4444:
	case ATOM_PIXEL_FORMAT_RGBX5551:
	case ATOM_PIXEL_FORMAT_XBGR4444:
	case ATOM_PIXEL_FORMAT_XRGB1555:
	case ATOM_PIXEL_FORMAT_XRGB4444:
		{
			convertFormat (ATOM_PIXEL_FORMAT_RGB888);
			newFormat = ATOM_PIXEL_FORMAT_RGB888;
			components = 3;
			break;
		}
	default:
		{
			components = ATOM_PIXEL_FORMAT(_M_format).bpp / 8;
			break;
		}
	}

	float sx = float(_M_width - 1) / float((widthout > 1) ? widthout - 1 : 1);
	float sy = float(_M_height - 1) / float((heightout > 1) ? heightout - 1 : 1);
	unsigned char *newImage = ATOM_NEW_ARRAY(unsigned char, widthout * heightout * components);

	if (sx < 1.f && sy < 1.f)
	{
		for (unsigned i = 0; i < heightout; ++i)
		{
			int i0 = ATOM_ftol(i * sy);
			int i1 = i0 + 1;
			if (i1 >= int(_M_height))
			{
				i1 = _M_height - 1;
			}
			float alpha = i * sy - i0;

			i0 *= _M_width;
			i1 *= _M_width;

			for (unsigned j = 0; j < widthout; ++j)
			{
				int j0 = ATOM_ftol(j * sx);
				int j1 = j0 + 1;
				if (j1 >= int(_M_width))
				{
					j1 = _M_width - 1;
				}
				float beta = j * sx - j0;

				unsigned char *c00 = _M_contents + (i0 + j0) * components;
				unsigned char *c01 = _M_contents + (i0 + j1) * components;
				unsigned char *c10 = _M_contents + (i1 + j0) * components;
				unsigned char *c11 = _M_contents + (i1 + j1) * components;
				unsigned char *dst = newImage + (i * widthout + j) * components;

				for (int n = 0; n < components; ++n)
				{
					float s1 = *c00 + (*c01 - *c00) * beta;
					float s2 = *c10 + (*c11 - *c10) * beta;
					*dst++ = ATOM_ftol(s1 + (s2 - s1) * alpha);
					++c00;
					++c01;
					++c10;
					++c11;
				}
			}
		}
	}
	else
	{
		for (unsigned i = 0; i < heightout; ++i)
		{
			int i0 = ATOM_ftol(i * sy);
			int i1 = i0 + 1;
			if (i1 >= int(_M_height))
			{
				i1 = _M_height - 1;
			}

			for (unsigned j = 0; j < widthout; ++j)
			{
				int j0 = ATOM_ftol(j * sx);
				int j1 = j0 + 1;
				if (j1 >= int(_M_width))
				{
					j1 = _M_width - 1;
				}

				unsigned char *dst = newImage + (i * widthout + j) * components;

				for (int n = 0; n < components; ++n)
				{
					float sum = 0.f;
					for (int s = i0; s <= i1; ++s)
					{
						for (int t = j0; t <= j1; ++t)
						{
							sum += _M_contents[(s * _M_width + t) * components + n];
						}
					}

					sum /= (j1 - j0 + 1) * (i1 - i0 + 1);
					*dst++ = ATOM_ftol(sum);
				}
			}
		}
	}

	init (widthout, heightout, newFormat, newImage);
	convertFormat (savedFormat);

	ATOM_DELETE_ARRAY(newImage);
}

bool ATOM_BaseImage::save (ATOM_File *file, ATOM_PixelFormat format)
{
	ATOM_STACK_TRACE(ATOM_BaseImage::save);
	if ( file == 0)
	{
		ATOM_LOGGER::error ("[ATOM_BaseImage::save] Invalid arguments: 0x%08X.\n", file);
		return false;
	}

	if ( !_M_contents)
	{
		ATOM_LOGGER::error ("[ATOM_BaseImage::save] Image contains no data.\n");
		return false;
	}

	const char* name = file->getName();
	const char* ext = strrchr(name, '.');
	if ( ext)
	{
		++ext;
	}
	int type = ATOM_ImageCodec::guessTypeByExtension(ext);

	ATOM_ImageCodec *codec = 0;

	switch ( type)
	{
	case ATOM_ImageCodec::PNG:
		{
			codec = ATOM_NEW(ATOM_PNGCodec);
			break;
		}
	case ATOM_ImageCodec::DDS:
		{
			codec = ATOM_NEW(ATOM_DDSCodec);
			break;
		}
	default:
		{
			ATOM_LOGGER::error ("[ATOM_BaseImage::save] Unsupported image file format.\n");
			return false;
		}
	}

	if ( codec == 0)
	{
		ATOM_LOGGER::error ("[ATOM_BaseImage::save] Create image encoder failed.\n");
		return false;
	}

	ATOM_RWops* input = ATOM_RWops::createMemRWops(_M_contents, getBufferSize());
	ATOM_RWops* output = ATOM_RWops::createVFSRWops(file);

	ATOM_ImageCodec::CodecInfo ci;
	ci.format = _M_format;
	ci.width = _M_width;
	ci.height = _M_height;
	ci.buffersize = getBufferSize();

	if ( !codec->beginEncode(input, &ci))
	{
		ATOM_LOGGER::error ("[ATOM_BaseImage::save] Error encoding image.\n");
		ATOM_RWops::destroyRWops(input);
		ATOM_RWops::destroyRWops(output);
		ATOM_DELETE(codec);
		return false;
	}

	if (format != ATOM_PIXEL_FORMAT_END)
	{
		ci.format = format;
	}

	if ( !codec->encode(output, &ci))
	{
		ATOM_LOGGER::error ("[ATOM_BaseImage::save] Error encoding image.\n");
		ATOM_RWops::destroyRWops(input);
		ATOM_RWops::destroyRWops(output);
		ATOM_DELETE(codec);
		return false;
	}

	codec->endEncode(&ci);
	ATOM_RWops::destroyRWops(input);
	ATOM_RWops::destroyRWops(output);
	ATOM_DELETE(codec);

	return true;
}

bool ATOM_BaseImage::save (ATOM_RWops *rwops, int type, ATOM_PixelFormat format)
{
	ATOM_STACK_TRACE(ATOM_BaseImage::save);
	if ( rwops == 0)
	{
		ATOM_LOGGER::error ("[ATOM_BaseImage::save] Invalid arguments: 0x%08X.\n", rwops);
		return false;
	}

	if ( !_M_contents)
	{
		ATOM_LOGGER::error ("[ATOM_BaseImage::save] Image contains no data.\n");
		return false;
	}

	ATOM_ImageCodec *codec = 0;

	switch ( type)
	{
	case ATOM_ImageCodec::PNG:
		{
			codec = ATOM_NEW(ATOM_PNGCodec);
			break;
		}
	case ATOM_ImageCodec::DDS:
		{
			codec = ATOM_NEW(ATOM_DDSCodec);
			break;
		}
	default:
		{
			ATOM_LOGGER::error ("[ATOM_BaseImage::save] Unsupported image file format.\n");
			return false;
		}
	}

	if ( codec == 0)
	{
		ATOM_LOGGER::error ("[ATOM_BaseImage::save] Create image encoder failed.\n");
		return false;
	}

	ATOM_RWops* input = ATOM_RWops::createMemRWops(_M_contents, getBufferSize());

	ATOM_ImageCodec::CodecInfo ci;
	ci.format = _M_format;
	ci.width = _M_width;
	ci.height = _M_height;
	ci.buffersize = getBufferSize();
	ci.mipcount = _M_nummips;

	if ( !codec->beginEncode(input, &ci))
	{
		ATOM_LOGGER::error ("[ATOM_BaseImage::save] Error encoding image.\n");
		ATOM_RWops::destroyRWops(input);
		ATOM_DELETE(codec);
		return false;
	}

	if (format != ATOM_PIXEL_FORMAT_END)
	{
		ci.format = format;
	}

	if ( !codec->encode(rwops, &ci))
	{
		ATOM_LOGGER::error ("[ATOM_BaseImage::save] Error encoding image.\n");
		ATOM_RWops::destroyRWops(input);
		ATOM_DELETE(codec);
		return false;
	}

	codec->endEncode(&ci);
	ATOM_RWops::destroyRWops(input);
	ATOM_DELETE(codec);

	return true;
}

bool ATOM_BaseImage::load (ATOM_File *file, ATOM_PixelFormat format)
{
	ATOM_STACK_TRACE(ATOM_BaseImage::load);
	if ( file == 0)
	{
		ATOM_LOGGER::error ("[ATOM_BaseImage::load] Invalid arguments: 0x%08X.\n", file);
		return false;
	}

	const char* name = file->getName();
	const char* ext = strrchr(name, '.');
	if ( ext)	
	{
		++ext;
	}
	int type = ATOM_ImageCodec::guessTypeByExtension(ext);

	ATOM_RWops* input = ATOM_RWops::createVFSRWops(file);
	if ( input == 0)
	{
		return false;
	}

	bool ret = load(input, type, format);

	ATOM_RWops::destroyRWops(input);

	return ret;
}

bool ATOM_BaseImage::load (ATOM_RWops *rwops, int type, ATOM_PixelFormat format)
{
	ATOM_STACK_TRACE(ATOM_BaseImage::load);
	if (!rwops)
	{
		ATOM_LOGGER::error ("[ATOM_BaseImage::load] Invalid arguments: 0x%08X, %d\n", rwops, type);
		return false;
	}

	return rwops ? init (rwops, type, format) : false;
}

void ATOM_BaseImage::crop (int left, int top, unsigned width, unsigned height)
{
	ATOM_STACK_TRACE(ATOM_BaseImage::crop);
	if (width > _M_width - left)
	{
		width = _M_width - left;
	}

	if (height > _M_height - top)
	{
		height = _M_height - top;
	}

	ATOM_PixelFormat oldformat = _M_format;

	if (ATOM_PIXEL_FORMAT(_M_format).bpp == 0)
	{
		convertFormat (ATOM_PIXEL_FORMAT_RGBA8888);
	}

	int pixel_stride = ATOM_PIXEL_FORMAT(_M_format).bpp / 8;
	int line_stride = width * pixel_stride;
	int scanline_stride = _M_width * pixel_stride;
	unsigned char *src = _M_contents + top * scanline_stride + left * pixel_stride;
	unsigned char *newBits = ATOM_NEW_ARRAY(unsigned char, height * line_stride);
	for (unsigned i = 0; i < height; ++i)
	{
		memcpy (newBits + i * line_stride, src + i * scanline_stride, line_stride);
	}

	init (width, height, _M_format, newBits, 1);
	ATOM_DELETE_ARRAY(newBits);

	if (_M_format != oldformat)
	{
		convertFormat(oldformat);
	}
}

unsigned ATOM_BaseImage::getWidth (void) const 
{
	return _M_width;
}

unsigned ATOM_BaseImage::getHeight (void) const 
{
	return _M_height;
}

ATOM_PixelFormat ATOM_BaseImage::getFormat (void) const 
{
	return _M_format;
}

int ATOM_BaseImage::getNumMipmaps (void) const 
{
	return _M_nummips;
}

int ATOM_BaseImage::getBufferSize (void) const 
{
	return _M_buffersize;
}

unsigned char* ATOM_BaseImage::getData (void) const 
{
	return _M_contents;
}

void ATOM_BaseImage::swap (ATOM_BaseImage &rhs) 
{
	std::swap (_M_width, rhs._M_width);
	std::swap (_M_height, rhs._M_height);
	std::swap (_M_format, rhs._M_format);
	std::swap (_M_contents, rhs._M_contents);
	std::swap (_M_nummips, rhs._M_nummips);
	std::swap (_M_buffersize, rhs._M_buffersize);
	std::swap (_M_actual_size, rhs._M_actual_size);
}

