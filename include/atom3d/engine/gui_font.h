/**	\file gui_font.h
 *	GUI×ÖÌåÀà.
 *
 *	\author ¸ßÑÅÀ¥
 *	\addtogroup engine
 *	@{
 */

#ifndef __ATOM3D_ENGINE_GUI_FONT_H
#define __ATOM3D_ENGINE_GUI_FONT_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_ttfont.h"
#include "../ATOM_render.h"

#include "basedefs.h"



class ATOM_ENGINE_API ATOM_GUIFont
{
public:
	struct _handle
	{
		int charset;
		int charmargin;
		ATOM_FontHandle handle;
	};
	typedef _handle * handle;

	struct GUIFontInfo
	{
		ATOM_STRING name;
		ATOM_STRING filename;
		int size;
		int charset;
		int margin;
		ATOM_GUIFont::handle handle;
	};


	static const handle invalid_handle;
	static const char *default_font_name;
	static ATOM_GUIFont::handle createFont (const char *name, int size, int charset, int charmargin);
	static handle getDefaultFont (int size, int charmargin);
	static ATOM_FontHandle getFontHandle (ATOM_GUIFont::handle font);
	static int getFontCharSet (ATOM_GUIFont::handle font);
	static int getFontCharMargin (ATOM_GUIFont::handle font);
	static void releaseFont (ATOM_GUIFont::handle font);
	static ATOM_GUIFont::handle ensureValidFont (ATOM_GUIFont::handle font, int size, int charmargin);
	static void loadFontConfig (const char *configFileName);
	static handle lookUpFont (const char *fontName);
	static const ATOM_HASHMAP<ATOM_STRING, ATOM_GUIFont::GUIFontInfo> &getFontInfoMap(void);
};


static inline ATOM_AUTOREF(ATOM_Text) GUI_createText (const char* str, ATOM_GUIFont::handle font)
{
	ATOM_AUTOREF(ATOM_Text) text = ATOM_HARDREF(ATOM_Text) ();
	text->setFont (ATOM_GUIFont::getFontHandle(font));
	text->setCharMargin (ATOM_GUIFont::getFontCharMargin (font));
	text->setZValue (1.f);
	text->setRenderMode (ATOM_Text::DISPLAYMODE_2D);
	text->setString (str);
	text->realize ();
	return text;
}

static inline bool GUI_calcStringBounds(const char* str, int len, ATOM_GUIFont::handle font, int& l, int& t, int& w, int& h)
{
	ATOM_SetCharMargin (ATOM_GUIFont::getFontCharMargin (font));
	return ATOM_CalcStringBounds (ATOM_GUIFont::getFontHandle(font), str, len, &l, &t, &w, &h);
}


#endif // __ATOM3D_ENGINE_GUI_FONT_H
/*! @} */
