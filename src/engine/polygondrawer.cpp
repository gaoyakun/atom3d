#include "stdafx.h"
#include "polygondrawer.h"
#include "gui_canvas.h"


ATOM_CircleDrawer::ATOM_CircleDrawer(ATOM_GUICanvas* canvas, ATOM_Texture* texture)
{
	ATOM_STACK_TRACE(ATOM_CircleDrawer::ATOM_CircleDrawer);

	_canvas = canvas;
	_texture = texture;
}


void ATOM_CircleDrawer::DrawAdd(const ATOM_Rect2Di& rect, float ratio)
{
	ATOM_STACK_TRACE(ATOM_CircleDrawer::DrawAdd);

	if(ratio <= 0.f)
	{
		return;
	}

	if(ratio >= 1.f)
	{
		_canvas->drawTexturedRectEx (0, rect, 0xFFFFFFFF, _texture, ATOM_Rect2Di(0,0,0,0), false);
		return;
	}

	int w = rect.size.w;
	int h = rect.size.h;
	int wh = rect.size.w/2;
	int hh = rect.size.h/2;
	float tg = tan(ratio*3.1415926*2);
	if(ratio <= 0.125f)
	{
		DrawTriangle(rect.point, ATOM_Point2Di(wh, 0), ATOM_Point2Di(wh, hh), ATOM_Point2Di(tg*hh+wh, 0), ATOM_Vector2f(0.5f, 0), ATOM_Vector2f(0.5f, 0.5f), ATOM_Vector2f(tg*0.5f+0.5, 0));
	}
	else if(ratio <= 0.375f)
	{
		DrawTriangle(rect.point, ATOM_Point2Di(wh, 0), ATOM_Point2Di(wh, hh), ATOM_Point2Di(w, 0), ATOM_Vector2f(0.5f, 0), ATOM_Vector2f(0.5f, 0.5f), ATOM_Vector2f(1.f, 0));
		DrawTriangle(rect.point, ATOM_Point2Di(w, 0), ATOM_Point2Di(wh, hh), ATOM_Point2Di(w, hh-wh/tg), ATOM_Vector2f(1.f, 0), ATOM_Vector2f(0.5f, 0.5f), ATOM_Vector2f(1.f, 0.5-0.5/tg) );
	}
	else if(ratio <= 0.625f)
	{
		DrawTriangle(rect.point, ATOM_Point2Di(wh, 0), ATOM_Point2Di(wh, hh), ATOM_Point2Di(w, 0), ATOM_Vector2f(0.5f, 0), ATOM_Vector2f(0.5f, 0.5f), ATOM_Vector2f(1.f, 0) );
		DrawTriangle(rect.point, ATOM_Point2Di(w, 0), ATOM_Point2Di(wh, hh), ATOM_Point2Di(w, h), ATOM_Vector2f(1.f, 0), ATOM_Vector2f(0.5f, 0.5f), ATOM_Vector2f(1.f, 1.f) );
		DrawTriangle(rect.point, ATOM_Point2Di(w, h), ATOM_Point2Di(wh, hh), ATOM_Point2Di(wh-hh*tg, h), ATOM_Vector2f(1.f, 1.f), ATOM_Vector2f(0.5f, 0.5f), ATOM_Vector2f(0.5f-tg/2, 1.f) );
	}
	else if(ratio <= 0.875f)
	{
		DrawTriangle(rect.point, ATOM_Point2Di(wh, 0), ATOM_Point2Di(wh, hh), ATOM_Point2Di(w, 0), ATOM_Vector2f(0.5f, 0), ATOM_Vector2f(0.5f, 0.5f), ATOM_Vector2f(1.f, 0) );
		DrawTriangle(rect.point, ATOM_Point2Di(w, 0), ATOM_Point2Di(wh, hh), ATOM_Point2Di(w, h), ATOM_Vector2f(1.f, 0), ATOM_Vector2f(0.5f, 0.5f), ATOM_Vector2f(1.f, 1.f) );
		DrawTriangle(rect.point, ATOM_Point2Di(w, h), ATOM_Point2Di(wh, hh), ATOM_Point2Di(0, h), ATOM_Vector2f(1.f, 1.f), ATOM_Vector2f(0.5f, 0.5f), ATOM_Vector2f(0, 1.f) );
		DrawTriangle(rect.point, ATOM_Point2Di(0, h), ATOM_Point2Di(wh, hh), ATOM_Point2Di(0, hh+wh/tg), ATOM_Vector2f(0.f, 1.f), ATOM_Vector2f(0.5f, 0.5f), ATOM_Vector2f(0, 0.5f+0.5f/tg) );
	}
	else
	{
		DrawTriangle(rect.point, ATOM_Point2Di(wh, 0), ATOM_Point2Di(wh, hh), ATOM_Point2Di(w, 0), ATOM_Vector2f(0.5f, 0), ATOM_Vector2f(0.5f, 0.5f), ATOM_Vector2f(1.f, 0) );
		DrawTriangle(rect.point, ATOM_Point2Di(w, 0), ATOM_Point2Di(wh, hh), ATOM_Point2Di(w, h), ATOM_Vector2f(1.f, 0), ATOM_Vector2f(0.5f, 0.5f), ATOM_Vector2f(1.f, 1.f) );
		DrawTriangle(rect.point, ATOM_Point2Di(w, h), ATOM_Point2Di(wh, hh), ATOM_Point2Di(0, h), ATOM_Vector2f(1.f, 1.f), ATOM_Vector2f(0.5f, 0.5f), ATOM_Vector2f(0, 1.f) );
		DrawTriangle(rect.point, ATOM_Point2Di(0, h), ATOM_Point2Di(wh, hh), ATOM_Point2Di(0, 0), ATOM_Vector2f(0.f, 1.f), ATOM_Vector2f(0.5f, 0.5f), ATOM_Vector2f(0, 0) );
		DrawTriangle(rect.point, ATOM_Point2Di(0, 0), ATOM_Point2Di(wh, hh), ATOM_Point2Di(wh+tg*hh, 0), ATOM_Vector2f(0.f, 0.f), ATOM_Vector2f(0.5f, 0.5f), ATOM_Vector2f(0.5+tg*0.5, 0) );
	}

}

void ATOM_CircleDrawer::InvertDrawAdd(const ATOM_Rect2Di& rect, float ratio)
{
	ATOM_STACK_TRACE(ATOM_CircleDrawer::InvertDrawAdd);

	if(ratio <= 0.f)
	{
		return;
	}

	if(ratio >= 1.f)
	{
		_canvas->drawTexturedRectEx (0, rect, 0xFFFFFFFF, _texture, ATOM_Rect2Di(0,0,0,0), false);
		return;
	}

	int w = rect.size.w;
	int h = rect.size.h;
	int wh = rect.size.w/2;
	int hh = rect.size.h/2;
	float tg = tan(ratio*3.1415926*2);
	if(ratio <= 0.125f)
	{
		DrawTriangle(rect.point, ATOM_Point2Di(wh, hh), ATOM_Point2Di(wh, h), ATOM_Point2Di(wh-tg*hh, h), ATOM_Vector2f(0.5f, 0.5), ATOM_Vector2f(0.5f, 1.f), ATOM_Vector2f(0.5-tg*0.5f, 1.f));
	}
	else if(ratio <= 0.375f)
	{
		DrawTriangle(rect.point, ATOM_Point2Di(wh, hh), ATOM_Point2Di(wh, h), ATOM_Point2Di(0, h), ATOM_Vector2f(0.5f, 0.5f), ATOM_Vector2f(0.5f, 1.f), ATOM_Vector2f(0, 1.f));
		DrawTriangle(rect.point, ATOM_Point2Di(wh, hh), ATOM_Point2Di(0, h), ATOM_Point2Di(0, hh+wh/tg), ATOM_Vector2f(0.5f, 0.5f), ATOM_Vector2f(0, 1.f), ATOM_Vector2f(0.f, 0.5+0.5/tg) );
	}
	else if(ratio <= 0.625f)
	{
		DrawTriangle(rect.point, ATOM_Point2Di(wh, hh), ATOM_Point2Di(wh, h), ATOM_Point2Di(0, h), ATOM_Vector2f(0.5f, 0.5f), ATOM_Vector2f(0.5f, 1.f), ATOM_Vector2f(0, 1.f) );
		DrawTriangle(rect.point, ATOM_Point2Di(wh, hh), ATOM_Point2Di(0, h), ATOM_Point2Di(0, 0), ATOM_Vector2f(0.5f, 0.5f), ATOM_Vector2f(0, 1.f), ATOM_Vector2f(0.f, 0.f) );
		DrawTriangle(rect.point, ATOM_Point2Di(wh, hh), ATOM_Point2Di(0, 0), ATOM_Point2Di(wh+hh*tg, 0), ATOM_Vector2f(0.5f, 0.5f), ATOM_Vector2f(0, 0), ATOM_Vector2f(0.5f+0.5f*tg, 0) );
	}
	else if(ratio <= 0.875f)
	{
		DrawTriangle(rect.point, ATOM_Point2Di(wh, hh), ATOM_Point2Di(wh, h), ATOM_Point2Di(0, h), ATOM_Vector2f(0.5f, 0.5), ATOM_Vector2f(0.5f, 1.f), ATOM_Vector2f(0, 1.f) );
		DrawTriangle(rect.point, ATOM_Point2Di(wh, hh), ATOM_Point2Di(0, h), ATOM_Point2Di(0, 0), ATOM_Vector2f(0.5f, 0.5f), ATOM_Vector2f(0, 1.f), ATOM_Vector2f(0.f, 0.f) );
		DrawTriangle(rect.point, ATOM_Point2Di(wh, hh), ATOM_Point2Di(0, 0), ATOM_Point2Di(w, 0), ATOM_Vector2f(0.5f, 0.5f), ATOM_Vector2f(0, 0), ATOM_Vector2f(1.f, 0) );
		DrawTriangle(rect.point, ATOM_Point2Di(wh, hh), ATOM_Point2Di(w, 0), ATOM_Point2Di(w, hh-wh/tg), ATOM_Vector2f(0.5f, 0.5f), ATOM_Vector2f(1.f, 0), ATOM_Vector2f(1, 0.5f-0.5f/tg) );
	}
	else
	{
		DrawTriangle(rect.point, ATOM_Point2Di(wh, hh), ATOM_Point2Di(wh, h), ATOM_Point2Di(0, h), ATOM_Vector2f(0.5f, 0.5), ATOM_Vector2f(0.5f, 1.f), ATOM_Vector2f(0, 1.f) );
		DrawTriangle(rect.point, ATOM_Point2Di(wh, hh), ATOM_Point2Di(0, h), ATOM_Point2Di(0, 0), ATOM_Vector2f(0.5f, 0.5f), ATOM_Vector2f(0, 1.f), ATOM_Vector2f(0, 0) );
		DrawTriangle(rect.point, ATOM_Point2Di(wh, hh), ATOM_Point2Di(0, 0), ATOM_Point2Di(w, 0), ATOM_Vector2f(0.5f, 0.5f), ATOM_Vector2f(0, 0), ATOM_Vector2f(1.f, 0) );
		DrawTriangle(rect.point, ATOM_Point2Di(wh, hh), ATOM_Point2Di(w, 0), ATOM_Point2Di(w, h), ATOM_Vector2f(0.5f, 0.5f), ATOM_Vector2f(1.f, 0), ATOM_Vector2f(1.f, 1.f) );
		DrawTriangle(rect.point, ATOM_Point2Di(wh, hh), ATOM_Point2Di(w, h), ATOM_Point2Di(wh-tg*hh, h), ATOM_Vector2f(0.5f, 0.5f), ATOM_Vector2f(1.f, 1.f), ATOM_Vector2f(0.5f-tg*0.5f, 1.f) );
	}

}

void ATOM_CircleDrawer::DrawRemove(const ATOM_Rect2Di& rect, float ratio)
{
	ATOM_STACK_TRACE(ATOM_CircleDrawer::DrawRemove);

	if(ratio <= 0.f)
	{
		_canvas->drawTexturedRectEx (0, rect, 0xFFFFFFFF, _texture, ATOM_Rect2Di(0,0,0,0), false);
		return;
	}

	if(ratio >= 1.f)
	{
		return;
	}

	int w = rect.size.w;
	int h = rect.size.h;
	int wh = rect.size.w/2;
	int hh = rect.size.h/2;
	float tg = tan(ratio*3.1415926*2);
	if(ratio <= 0.125f)
	{
		DrawTriangle(rect.point, ATOM_Point2Di(wh+tg*hh, 0), ATOM_Point2Di(wh, hh), ATOM_Point2Di(w, 0), ATOM_Vector2f(0.5f+tg*0.5f, 0), ATOM_Vector2f(0.5f, 0.5f), ATOM_Vector2f(1.f, 0) );
		DrawTriangle(rect.point, ATOM_Point2Di(w, 0), ATOM_Point2Di(wh, hh), ATOM_Point2Di(w, h), ATOM_Vector2f(1.f, 0), ATOM_Vector2f(0.5f, 0.5f), ATOM_Vector2f(1.f, 1.f) );
		DrawTriangle(rect.point, ATOM_Point2Di(w, h), ATOM_Point2Di(wh, hh), ATOM_Point2Di(0, h), ATOM_Vector2f(1.f, 1.f), ATOM_Vector2f(0.5f, 0.5f), ATOM_Vector2f(0, 1.f) );
		DrawTriangle(rect.point, ATOM_Point2Di(0, h), ATOM_Point2Di(wh, hh), ATOM_Point2Di(0, 0), ATOM_Vector2f(0.f, 1.f), ATOM_Vector2f(0.5f, 0.5f), ATOM_Vector2f(0, 0) );
		DrawTriangle(rect.point, ATOM_Point2Di(0, 0), ATOM_Point2Di(wh, hh), ATOM_Point2Di(wh, 0), ATOM_Vector2f(0.f, 0.f), ATOM_Vector2f(0.5f, 0.5f), ATOM_Vector2f(0.5, 0) );

	}
	else if(ratio <= 0.375f)
	{
		DrawTriangle(rect.point, ATOM_Point2Di(w, hh-wh/tg), ATOM_Point2Di(wh, hh), ATOM_Point2Di(w, h), ATOM_Vector2f(1.f, 0.5-0.5/tg), ATOM_Vector2f(0.5f, 0.5f), ATOM_Vector2f(1.f, 1.f) );
		DrawTriangle(rect.point, ATOM_Point2Di(w, h), ATOM_Point2Di(wh, hh), ATOM_Point2Di(0, h), ATOM_Vector2f(1.f, 1.f), ATOM_Vector2f(0.5f, 0.5f), ATOM_Vector2f(0, 1.f) );
		DrawTriangle(rect.point, ATOM_Point2Di(0, h), ATOM_Point2Di(wh, hh), ATOM_Point2Di(0, 0), ATOM_Vector2f(0.f, 1.f), ATOM_Vector2f(0.5f, 0.5f), ATOM_Vector2f(0, 0) );
		DrawTriangle(rect.point, ATOM_Point2Di(0, 0), ATOM_Point2Di(wh, hh), ATOM_Point2Di(wh, 0), ATOM_Vector2f(0.f, 0.f), ATOM_Vector2f(0.5f, 0.5f), ATOM_Vector2f(0.5, 0) );
	}
	else if(ratio <= 0.625f)
	{
		DrawTriangle(rect.point, ATOM_Point2Di(0, h), ATOM_Point2Di(wh, hh), ATOM_Point2Di(wh-hh*tg, h), ATOM_Vector2f(0, 1), ATOM_Vector2f(0.5f, 0.5f), ATOM_Vector2f(0.5f-0.5f*tg, 1.f) );
		DrawTriangle(rect.point, ATOM_Point2Di(0, h), ATOM_Point2Di(wh, hh), ATOM_Point2Di(0, 0), ATOM_Vector2f(0.f, 1.f), ATOM_Vector2f(0.5f, 0.5f), ATOM_Vector2f(0, 0) );
		DrawTriangle(rect.point, ATOM_Point2Di(0, 0), ATOM_Point2Di(wh, hh), ATOM_Point2Di(wh, 0), ATOM_Vector2f(0.f, 0.f), ATOM_Vector2f(0.5f, 0.5f), ATOM_Vector2f(0.5, 0) );
	}
	else if(ratio <= 0.875f)
	{
		DrawTriangle(rect.point, ATOM_Point2Di(0, 0), ATOM_Point2Di(wh, hh), ATOM_Point2Di(0, hh+wh/tg), ATOM_Vector2f(0.f, 0), ATOM_Vector2f(0.5f, 0.5f), ATOM_Vector2f(0, 0.5+0.5/tg) );
		DrawTriangle(rect.point, ATOM_Point2Di(0, 0), ATOM_Point2Di(wh, hh), ATOM_Point2Di(wh, 0), ATOM_Vector2f(0.f, 0.f), ATOM_Vector2f(0.5f, 0.5f), ATOM_Vector2f(0.5, 0) );
	}
	else
	{
		DrawTriangle(rect.point, ATOM_Point2Di(wh, 0), ATOM_Point2Di(wh, hh), ATOM_Point2Di(wh+wh*tg, 0), ATOM_Vector2f(0.5f, 0), ATOM_Vector2f(0.5f, 0.5f), ATOM_Vector2f(0.5f+0.5f*tg, 0));
	}

}

void ATOM_CircleDrawer::DrawTriangle(const ATOM_Point2Di& offset, const ATOM_Point2Di& pt1, const ATOM_Point2Di& pt2, const ATOM_Point2Di& pt3
								, const ATOM_Vector2f& uv1, const ATOM_Vector2f& uv2, const ATOM_Vector2f& uv3)
{
	ATOM_STACK_TRACE(ATOM_CircleDrawer::DrawTriangle);

	ATOM_Point2Di pt[3] = {pt1+offset, pt2+offset, pt3+offset};
	ATOM_Vector2f uv[3] = {uv1, uv2, uv3};
	_canvas->drawTexturedConvexPolygon(0, 3, pt, uv, 0xFFFFFFFF, _texture, false);
}




