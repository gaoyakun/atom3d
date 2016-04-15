#include "texture.h"

ATOM_Texture::ATOM_Texture(void) {
  _flags = 0;
  _width = 0;
  _height = 0;
  _depth = 0;
  _contentLost = false;
}

ATOM_Texture::~ATOM_Texture (void)
{
}

unsigned ATOM_Texture::getFlags (void) const 
{
  return _flags;
}

unsigned ATOM_Texture::getWidth (void) const 
{
  return _width;
}

unsigned ATOM_Texture::getHeight (void) const 
{
  return _height;
}

bool ATOM_Texture::isContentLost (void) const
{
	return _contentLost;
}

void ATOM_Texture::setContentLost (bool b)
{
	_contentLost = b;
}

