#ifndef __ATOM_RENDER_TEXTRENDERER_H
#define __ATOM_RENDER_TEXTRENDERER_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <ATOM_utils.h>

#include "text.h"
#include "stateset.h"
#include "texture.h"

class ATOM_TextRenderer
{
public:
	struct TextUnit
	{
		ATOM_AUTOREF(ATOM_Texture) texture;
		ATOM_VECTOR<ATOM_Text::GlyphVertex> vertices;
		ATOM_VECTOR<ATOM_Text::GlyphVertex> vertices_outline;
	};
	typedef ATOM_VECTOR<TextUnit> UnitVector;
	typedef UnitVector::iterator UnitVectorIter;
	typedef UnitVector::const_iterator UnitVectorConstIter;
public:
	ATOM_TextRenderer (void);
	~ATOM_TextRenderer (void);
public:
	void AddText (ATOM_Texture *texture, const ATOM_Text::GlyphVertex *vertices, const ATOM_Text::GlyphVertex *vertices_outline, unsigned numVertices);
	void Clear (void);
	void Render (ATOM_RenderDevice *device
		, int displayMode
		, const ATOM_Vector4f &textColor
		, const ATOM_Vector4f &outlineColor
		, int x
		, int y
		, float zval
		, int viewportW
		, int viewportH
		, bool outline);
private:
	void InternalRender (ATOM_RenderDevice *device
		, const ATOM_Vector4f &textColor
		, bool outline);
	void CreateStateSet (ATOM_RenderDevice *device);

private:
	UnitVector _M_text_vector;
	bool _M_need_sort;
	ATOM_RadixSort _radixsort;
};

#endif // __ATOM_RENDER_TEXTRENDERER_H
