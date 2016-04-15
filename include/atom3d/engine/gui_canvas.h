/**	\file gui_canvas.h
 *	GUI���廭����.
 *
 *	\author ������
 *	\addtogroup engine
 *	@{
 */

#ifndef __ATOM3D_ENGINE_GUI_CANVAS_H
#define __ATOM3D_ENGINE_GUI_CANVAS_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_utils.h"
#include "../ATOM_render.h"

#include "basedefs.h"
#include "gui_misc.h"

class ATOM_GUIRenderer;

class ATOM_ENGINE_API ATOM_GUICanvas
{
public:
	enum
	{
		DRAWTEXT_OUTLINE = (1<<0),	// ������壬�����Ժ�DRAWTEXT_SHADOWһ����
		DRAWTEXT_SHADOW = (1<<1),	// ��Ӱ���壬�����Ժ�DRAWTEXT_OUTLINEһ����
		DRAWTEXT_UNDERLINE = (1<<2),	// ��Ⱦ�»���
		DRAWTEXT_TEXTSTRING = (1<<3),	// ��Ⱦ�ַ�����DrawTextInfo�ṹ��textString��Ա�����ã������Ժ�DRAWTEXT_TEXTOBJECTһ����
		DRAWTEXT_TEXTOBJECT = (1<<5)	// ��Ⱦ���ֶ���DrawTextInfo�ṹ��textObject��Ա�����ã������Ժ�DRAWTEXT_TEXTSTRINGһ����
	};

	struct DrawTextInfo
	{
		ATOM_Text *textObject;
		const char *textString;
		ATOM_FontHandle font;
		int x;
		int y;
		int shadowOffsetX;
		int shadowOffsetY;
		ATOM_ColorARGB textColor;
		ATOM_ColorARGB outlineColor;
		ATOM_ColorARGB shadowColor;
		ATOM_ColorARGB underlineColor;
		unsigned flags;
	};

public:
	ATOM_GUICanvas (ATOM_Widget *widget);
	virtual ~ATOM_GUICanvas (void);

public:
	void fillRect (const ATOM_Rect2Di &rect, ATOM_ColorARGB color, float rotation = 0.f);
	void fillRectList (unsigned num, const ATOM_Rect2Di *rects, const ATOM_ColorARGB *colors, const float *rotations = 0);
	void drawRect (const ATOM_Rect2Di &rect, ATOM_ColorARGB color, int lineWidth);
	void drawTexturedRect (ATOM_Material *material, const ATOM_Rect2Di &rect, ATOM_ColorARGB color, ATOM_Texture *texture, bool bilinearFilter, float rotation = 0.f);
	void drawTexturedRectList (ATOM_Material *material, unsigned num, const ATOM_Rect2Di *rects, const ATOM_ColorARGB *colors, ATOM_Texture *texture, bool bilinearFilter, const float *rotations = 0);
	void drawTexturedRectEx (ATOM_Material *material, const ATOM_Rect2Di &rect, ATOM_ColorARGB color, ATOM_Texture *texture, const ATOM_Rect2Di &region, bool bilinearFilter, float rotation = 0);
	void drawTexturedRectListEx (ATOM_Material *material, unsigned num, const ATOM_Rect2Di *rects, const ATOM_ColorARGB *colors, ATOM_Texture *texture, const ATOM_Rect2Di *regions, bool bilinearFilter, const float *rotations = 0);
	void drawText (const char *text, ATOM_FontHandle font, int x, int y, ATOM_ColorARGB color);
	void drawTextOutline (const char *text, ATOM_FontHandle font, int x, int y, ATOM_ColorARGB color, ATOM_ColorARGB colorOutline);
	void drawTextShadow (const char *text, ATOM_FontHandle font, int x, int y, int offsetx, int offsety, ATOM_ColorARGB color, ATOM_ColorARGB colorShadow);
	void drawText (ATOM_Text *text, int x, int y, ATOM_ColorARGB color);
	void drawTextOutline (ATOM_Text *text, int x, int y, ATOM_ColorARGB color, ATOM_ColorARGB colorOutline);
	void drawTextShadow (ATOM_Text *text, int x, int y, int offsetx, int offsety, ATOM_ColorARGB color, ATOM_ColorARGB colorShadow);
	void drawTextEx (const DrawTextInfo *info);
	void drawLine (const ATOM_Point2Di &p1, const ATOM_Point2Di &p2, ATOM_ColorARGB color);
	void drawLineList (unsigned num, const ATOM_Point2Di *p1, const ATOM_Point2Di *p2, const ATOM_ColorARGB *colors);
	void drawConvexPolygon (unsigned numPoints, const ATOM_Point2Di *points, ATOM_ColorARGB color);
	void fillConvexPolygon (unsigned numPoints, const ATOM_Point2Di *points, ATOM_ColorARGB color);
	void drawTexturedConvexPolygon (ATOM_Material *material, unsigned numPoints, const ATOM_Point2Di *points, const ATOM_Vector2f *uv, ATOM_ColorARGB color, ATOM_Texture *texture, bool bilinearFilter);

public:
	void resize (const ATOM_Size2Di &size);
	void setOrigin (const ATOM_Point2Di &origin);
	void setTargetRect (const ATOM_Rect2Di &rect);
	const ATOM_Point2Di & getOrigin (void) const;
	const ATOM_Size2Di & getSize (void) const;
	const ATOM_Rect2Di & getTargetRect (void) const;

protected:
	ATOM_Size2Di _size;
	ATOM_Point2Di _origin;
	ATOM_Rect2Di _targetRect;
	ATOM_Widget *_widget;
};

#endif // __ATOM3D_ENGINE_GUI_CANVAS_H
/*! @} */
