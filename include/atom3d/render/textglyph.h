#ifndef __ATOM_RENDER_TEXTGLYPH_H
#define __ATOM_RENDER_TEXTGLYPH_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_ttfont.h"
#include "texture.h"

struct ATOM_TextGlyph
{
	ATOM_UNICC unicode;
	ATOM_FontHandle font;
	bool mono;
	float u1, v1;
	float u2, v2;
	ATOM_AUTOREF(ATOM_Texture) texmap;
};

#endif // __ATOM_RENDER_TEXTGLYPH_H
/*! @} */
