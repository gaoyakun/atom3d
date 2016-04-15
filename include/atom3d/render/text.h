/**	\file animatedtexturechannel.h
 *	渲染字体类的声明.
 *
 *	\author 高雅昆
 *	\addtogroup render
 *	@{
 */

#ifndef __ATOM_RENDER_TEXT_H
#define __ATOM_RENDER_TEXT_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <string>
#include <vector>
#include "../ATOM_ttfont.h"

#include "textglyph.h"
#include "texture.h"

class ATOM_RenderDatasGlyph;
class ATOM_RenderDevice;
class ATOM_TextRenderer;

class ATOM_Text: public ATOM_Object
{
	ATOM_CLASS(render, ATOM_Text, ATOM_Text)

	friend class ATOM_RenderDatasGlyph;
	friend class ATOM_SegmentRenderCallback;

public:
	enum 
	{
		DISPLAYMODE_UNKNOWN = -1,
		DISPLAYMODE_2D = 0,
		DISPLAYMODE_2D_ZTEST,
		DISPLAYMODE_3D
	};

	struct GlyphVertex
	{
		float x, y, z;
		float u;
		float v;
	};

public:
	ATOM_Text (void);
	virtual ~ATOM_Text (void);

public:
	virtual void render(int x, int y, ATOM_ColorARGB color);
	virtual void renderOutline(int x, int y, ATOM_ColorARGB color, ATOM_ColorARGB colorOutline);
	virtual void renderShadow(int x, int y, int offsetx, int offsety, ATOM_ColorARGB color, ATOM_ColorARGB colorShadow);
	virtual const char *getString(void) const;
	virtual void setString (const char *str);
	virtual float getZValue(void) const;
	virtual void setZValue(float z);
	virtual ATOM_FontHandle getFont () const;
	virtual void setFont (ATOM_FontHandle font);
	virtual void setCharMargin (int margin);
	virtual int getCharMargin (void) const;
	virtual void getBounds (int *left, int *top, int *width, int *height);
	virtual void setRenderMode (int mode);
	virtual int getRenderMode (void) const;
	virtual void setDisplayMode (int mode);
	virtual int  getDisplayMode (void) const;
	virtual bool realize (void);
	virtual void setLimit (int limit);

protected:
	void internalRender (int vw, int vh);

private:
	ATOM_RenderDatasGlyph *createRenderDatasGlyph (ATOM_Text *segment) const;
	void clearBuffers ();

protected:
    ATOM_STRING _M_string;
    ATOM_FontHandle _M_font;
    float _M_zvalue;
    bool _M_bounds_dirty;
    bool _M_dirty;
    int _M_margin;
    int _M_left;
    int _M_top;
    int _M_width;
    int _M_height;
    int _M_render_mode;
    int _M_saved_viewport[2];
	int _M_display_mode;
	int _M_limit;
	float _M_outline_width;
    ATOM_VECTOR<ATOM_RenderDatasGlyph*> _M_renderdatas;
    ATOM_VECTOR< ATOM_AUTOREF(ATOM_Texture) > _M_texmaps;
    ATOM_TextRenderer *_M_renderer;
  };

#endif // __ATOM_RENDER_TEXT_H
/*! @} */
