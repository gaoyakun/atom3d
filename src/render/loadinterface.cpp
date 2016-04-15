#include "StdAfx.h"
#include "loadinterface.h"
#include "loaders.h"

ATOM_TextureLoadInterface::ATOM_TextureLoadInterface (ATOM_Texture *owner)
{
	_loader = 0;
	_texture = owner;
	_skipMipMap = false;
	_width = 0;
	_height = 0;
	_format = ATOM_PIXEL_FORMAT_UNKNOWN;
}

ATOM_TextureLoadInterface::~ATOM_TextureLoadInterface (void)
{
	ATOM_DELETE(_loader);
}

void ATOM_TextureLoadInterface::setSkipMipMap (bool skip)
{
	_skipMipMap = skip;
}

ATOM_Texture *ATOM_TextureLoadInterface::getTexture (void) const
{
	return _texture;
}

void ATOM_TextureLoadInterface::setTexture (ATOM_Texture *texture)
{
	_texture = texture;
}

bool ATOM_TextureLoadInterface::isSkipMipMap (void) const
{
	return _skipMipMap;
}

void ATOM_TextureLoadInterface::setWidth (unsigned width)
{
	_width = width;
}

unsigned ATOM_TextureLoadInterface::getWidth (void) const
{
	return _width;
}

void ATOM_TextureLoadInterface::setHeight (unsigned height)
{
	_height = height;
}

unsigned ATOM_TextureLoadInterface::getHeight (void) const
{
	return _height;
}

void ATOM_TextureLoadInterface::setFormat (ATOM_PixelFormat format)
{
	_format = format;
}

ATOM_PixelFormat ATOM_TextureLoadInterface::getFormat (void) const
{
	return _format;
}

ATOM_LoadInterface::LoadingState ATOM_TextureLoadInterface::load (void)
{
	ATOM_ASSERT(_texture);

	_state = _texture->loadTexImageFromFile (getFileName(), _width, _height, _format) ? ATOM_LoadInterface::LS_LOADED : ATOM_LoadInterface::LS_LOADFAILED;

	return _state;
}

ATOM_LoadInterface::LoadingState ATOM_TextureLoadInterface::mtload (unsigned flags, long group, ATOM_LoadingRequestCallback callback, void *userData)
{
	ATOM_ASSERT(_texture);

	if (!_loader)
	{
		_loader = ATOM_NEW(ATOM_Texture2DLoader, getFileName(), _texture, _skipMipMap, _format);
	}

	ATOM_LoadingRequest *request = ATOM_NEW(ATOM_LoadingRequest);
	request->loadInterface = this;
	request->flags = flags;
	request->userData = userData;
	request->callback = callback;
	request->groupId = group;

	_texture->addRef ();

	ATOM_ContentStream::appendRequest (request);

	return _state;
}

void ATOM_TextureLoadInterface::unload (void)
{
	ATOM_ASSERT (_texture);

	_texture->invalidate (false);
	_width = 0;
	_height = 0;
}

void ATOM_TextureLoadInterface::finishLoad (void)
{
	_width = 0;
	_height = 0;

	ATOM_AUTOREF(ATOM_Texture) refHolder = _texture;

	_texture->decRef ();

	if (_loader)
	{
		ATOM_DELETE(_loader);
		_loader = 0;
	}
}

ATOM_BaseResourceLoader *ATOM_TextureLoadInterface::getLoader (void)
{
	return _loader;
}

