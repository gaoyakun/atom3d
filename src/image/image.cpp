#include "StdAfx.h"
#ifdef __CODEGEARC__
# pragma hdrstop
#endif

ATOM_SCRIPT_INTERFACE_BEGIN(ATOM_Image)
  ATOM_ATTRIBUTES_BEGIN(ATOM_Image)
  ATOM_ATTRIBUTES_END()
ATOM_SCRIPT_INTERFACE_END(ATOM_Image, ATOM_Resource)

ATOM_Image::ATOM_Image (void) 
{
	ATOM_STACK_TRACE(ATOM_Image::ATOM_Image);
}

ATOM_Image::~ATOM_Image ()
{
	ATOM_STACK_TRACE(ATOM_Image::~ATOM_Image);
}

ATOM_BaseImage &ATOM_Image::getBaseImage (void)
{
  return _M_baseimage;
}

const ATOM_BaseImage &ATOM_Image::getBaseImage (void) const
{
  return _M_baseimage;
}

void ATOM_Image::fini (void)
{
	ATOM_STACK_TRACE(ATOM_Image::fini);
	_M_baseimage.fini ();
}

void ATOM_Image::init(unsigned width, unsigned height, ATOM_PixelFormat format, const void* data, int mipcount)
{
	ATOM_STACK_TRACE(ATOM_Image::init);
	_M_baseimage.init (width, height, format, data, mipcount);
}

bool ATOM_Image::init(ATOM_RWops* input, int type, ATOM_PixelFormat format)
{
	ATOM_STACK_TRACE(ATOM_Image::init);
	return _M_baseimage.init (input, type, format);
}

void ATOM_Image::convertFormat(ATOM_PixelFormat format)
{
	ATOM_STACK_TRACE(ATOM_Image::convertFormat);
	_M_baseimage.convertFormat (format);
}

void ATOM_Image::castFormat (ATOM_PixelFormat format)
{
	ATOM_STACK_TRACE(ATOM_Image::castFormat);
	_M_baseimage.castFormat (format);
}

void ATOM_Image::blitTo(ATOM_BaseImage *dst, int srcx, int srcy, int srcw, int srch, int dstx, int dsty)
{
	ATOM_STACK_TRACE(ATOM_Image::blitTo);
	_M_baseimage.blitTo (dst, srcx, srcy, srcw, srch, dstx, dsty);
}

void ATOM_Image::resize(unsigned width, unsigned height)
{
	ATOM_STACK_TRACE(ATOM_Image::resize);
	_M_baseimage.resize (width, height);
}

void ATOM_Image::flip (void)
{
	ATOM_STACK_TRACE(ATOM_Image::flip);
	_M_baseimage.flip ();
}

void ATOM_Image::crop (int left, int top, unsigned width, unsigned height)
{
	ATOM_STACK_TRACE(ATOM_Image::crop);
	_M_baseimage.crop (left, top, width, height);
}

bool ATOM_Image::save (ATOM_File *file, ATOM_PixelFormat format)
{
	ATOM_STACK_TRACE(ATOM_Image::save);
	return _M_baseimage.save (file, format);
}

bool ATOM_Image::save (ATOM_RWops *rwops, int type, ATOM_PixelFormat format)
{
	ATOM_STACK_TRACE(ATOM_Image::save);
	return _M_baseimage.save (rwops, type, format);
}

bool ATOM_Image::load (ATOM_File *file, ATOM_PixelFormat format)
{
	ATOM_STACK_TRACE(ATOM_Image::load);
	return _M_baseimage.load (file, format);
}

bool ATOM_Image::load (ATOM_RWops *rwops, int type, ATOM_PixelFormat format)
{
	ATOM_STACK_TRACE(ATOM_Image::load);
	return _M_baseimage.load (rwops, type, format);
}

unsigned ATOM_Image::getWidth (void) const
{
  return _M_baseimage.getWidth ();
}

unsigned ATOM_Image::getHeight (void) const
{
  return _M_baseimage.getHeight ();
}

ATOM_PixelFormat ATOM_Image::getFormat (void) const
{
  return _M_baseimage.getFormat ();
}

int ATOM_Image::getNumMipmaps (void) const
{
  return _M_baseimage.getNumMipmaps ();
}

int ATOM_Image::getBufferSize (void) const
{
  return _M_baseimage.getBufferSize ();
}

unsigned char* ATOM_Image::getData (void) const
{
  return _M_baseimage.getData ();
}

bool ATOM_Image::writeToFile (ATOM_File* file, ATOM_PixelFormat format) 
{
	ATOM_STACK_TRACE(ATOM_Image::writeToFile);
	return _M_baseimage.save (file, format);
}

bool ATOM_Image::readFromFile (ATOM_File* file, void *userdata) 
{
	ATOM_STACK_TRACE(ATOM_Image::readFromFile);
	return userdata ? _M_baseimage.load (file, *((ATOM_PixelFormat*)userdata)) : _M_baseimage.load (file, ATOM_PIXEL_FORMAT_END);
}

