#include "StdAfx.h"
#include "loaders.h"
#include "d3d9device.h"

extern ATOM_RenderDevice *GetGlobalDevice (void);

ATOM_Texture2DLoader::ATOM_Texture2DLoader (const char *filename, ATOM_Texture *texture, bool skipMipMap, ATOM_PixelFormat format): ATOM_BaseResourceLoader (filename)
{
	ATOM_ASSERT(texture);

	_texture = texture;
	_skipMipMap = skipMipMap || (texture->getFlags() & ATOM_Texture::NOMIPMAP) != 0;
	_format = format;
}

ATOM_Texture *ATOM_Texture2DLoader::getTexture (void) const
{
	return _texture.get();
}

ATOM_BaseResourceLoader::LOADRESULT ATOM_Texture2DLoader::loadFromDisk (void)
{
	ATOM_BaseResourceLoader::LOADRESULT result = ATOM_BaseResourceLoader::loadFromDisk ();
	if (result != ATOM_BaseResourceLoader::LOADERR_OK)
	{
		return result;
	}

	ATOM_ImageFileInfo info;
	if (!GetGlobalDevice()->getImageFileInfoInMemory (getData(), getDataSize(), &info))
	{
		return ATOM_BaseResourceLoader::LOADERR_FAILED;
	}

	if (GetGlobalDevice()->getCapabilities().texture_npo2_c && (!ATOM_ispo2(info.width) || !ATOM_ispo2(info.height)))
	{
		_skipMipMap = true;
	}

	_width = info.width;
	_height = info.height;
	_depth = info.depth;
	_numMips = _skipMipMap ? 1 : info.numMips;
	if (_format == ATOM_PIXEL_FORMAT_UNKNOWN)
	{
		_format = info.format;
	}
	_imagetype = info.type;
	_texturetype = info.textureType;

	switch (_format)
	{
	case ATOM_PIXEL_FORMAT_RGBA8888:
		_format = ATOM_PIXEL_FORMAT_BGRA8888;
		break;
	case ATOM_PIXEL_FORMAT_RGBX8888:
		_format = ATOM_PIXEL_FORMAT_BGRX8888;
		break;
	case ATOM_PIXEL_FORMAT_RGB888:
		_format = ATOM_PIXEL_FORMAT_BGR888;
		break;
	}

	return ATOM_BaseResourceLoader::LOADERR_OK;
}

bool ATOM_Texture2DLoader::lock (void)
{
	// TODO: support background loading 3d texture and cube texture.

	if (_texturetype != ATOM_Texture::TEXTURE2D)
	{
		if (!_texture->loadTexImageFromFile (_filename.c_str(), 0, 0, ATOM_PIXEL_FORMAT_UNKNOWN))
		{
			return false;
		}
	}
	else
	{
		if (!_texture->loadTexImageFromMemory (0, _width, _height, 1, _format, _numMips))
		{
			return false;
		}

		unsigned numMips = _texture->getNumLevels();
		if (_numMips > numMips)
		{
			_numMips = numMips;
		}
		_lockedRect.resize (_skipMipMap ? 1 : _numMips);

		for (unsigned i = 0; i < _lockedRect.size(); ++i)
		{
			if (!_texture->lock (i, &_lockedRect[i]))
			{
				return false;
			}
		}
	}

	return true;
}

int ATOM_Texture2DLoader::unlock (void)
{
	for (unsigned i = 0; i < _lockedRect.size(); ++i)
	{
		_texture->unlock (i);
	}

	return LOADINGSTAGE_FINISH;
}

extern void GetSurfaceInfo( UINT width, UINT height, int fmt, UINT* pNumBytes, UINT* pRowBytes, UINT* pNumRows );

bool ATOM_Texture2DLoader::realize (void)
{
	if (_texturetype != ATOM_Texture::TEXTURE2D)
	{
		return true;
	}

	unsigned numBytes, rowBytes, numRows;

	switch (_imagetype)
	{
	case ATOM_ImageCodec::DDS:
		{
			unsigned char *texData = getData() + sizeof(unsigned) + sizeof(DDSURFACEDESC2);

			for (unsigned i = 0; i < _lockedRect.size(); ++i)
			{
				GetSurfaceInfo (_width, _height, _format, &numBytes, &rowBytes, &numRows);

				unsigned char *destBits = (unsigned char*)_lockedRect[i].bits;
				
				for (unsigned h = 0; h < numRows; ++h)
				{
					memcpy (destBits, texData, rowBytes);
					destBits += _lockedRect[i].pitch;
					texData += rowBytes;
				}

				_width >>= 1;
				_height >>= 1;

				if (_width == 0)
				{
					_width = 1;
				}

				if (_height == 0)
				{
					_height = 1;
				}
			}
			break;
		}
	case ATOM_ImageCodec::PNG:
	case ATOM_ImageCodec::JPEG:
		{
			ATOM_RWops *input = ATOM_RWops::createMemRWops (getData(), getDataSize());
			ATOM_BaseImage image;
			bool succ = image.init (input, _imagetype, _format);
			ATOM_RWops::destroyRWops (input);
			if (!succ)
			{
				return false;
			}

			GetSurfaceInfo (_width, _height, _format, &numBytes, &rowBytes, &numRows);
			unsigned char *src = image.getData();
			unsigned char *dest = (unsigned char*)_lockedRect[0].bits;

			for (unsigned h = 0; h < numRows; ++h)
			{
				memcpy (dest, src, rowBytes);
				dest += _lockedRect[0].pitch;
				src += rowBytes;
			}
			break;
		}
	default:
		return false;
	}

	return true;
}

