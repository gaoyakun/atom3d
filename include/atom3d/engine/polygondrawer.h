#ifndef __ATOM3D_ENGINE_POLYGONDRAWER_H
#define __ATOM3D_ENGINE_POLYGONDRAWER_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_render.h"

#include "gui_font.h"
#include "widget.h"



class ATOM_CircleDrawer
{
public:
	ATOM_CircleDrawer(ATOM_GUICanvas* canvas, ATOM_Texture* texture);

	void DrawAdd(const ATOM_Rect2Di& rect, float ratio);
	void InvertDrawAdd(const ATOM_Rect2Di& rect, float ratio);
	void DrawRemove(const ATOM_Rect2Di& rect, float ratio);
	void InvertDrawRemove(const ATOM_Rect2Di& rect, float ratio);

private:
	void DrawTriangle(const ATOM_Point2Di& offset, const ATOM_Point2Di& pt1, const ATOM_Point2Di& pt2, const ATOM_Point2Di& pt3
		, const ATOM_Vector2f& uv1, const ATOM_Vector2f& uv2, const ATOM_Vector2f& uv3);

private:
	ATOM_GUICanvas* _canvas;
	ATOM_Texture* _texture;
};



#endif // __ATOM3D_ENGINE_POLYGONDRAWER_H
