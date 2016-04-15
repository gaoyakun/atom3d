#ifndef ATOM3D_FLASH_RENDERBUFFER_H
#define ATOM3D_FLASH_RENDERBUFFER_H

#include <windows.h>
#include "basedefs.h"

class RenderBuffer
{
public:
	unsigned char* buffer;
	int width, height, rowSpan;

	RenderBuffer(int width, int height);

	~RenderBuffer();

	void resize (int width, int height);
	void reserve(int width, int height);
	void copyFrom(unsigned char* srcBuffer, int srcRowSpan);
	void copyArea(RECT srcRect, unsigned char* srcBuffer, int srcRowSpan);
	void blitBGR(unsigned char* destBuffer, int destRowSpan, int destDepth);
	int getTransparency (int x, int y) const;
};

#endif