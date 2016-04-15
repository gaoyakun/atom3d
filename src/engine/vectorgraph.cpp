#include "StdAfx.h"
#include "vectorgraph.h"
#include "gui_canvas.h"

struct VG_DrawRectOp: public VectorGraphOp
{
	float x, y, w, h;
	ATOM_ColorARGB rectColor;

	VG_DrawRectOp (void) {}
	VG_DrawRectOp (const ATOM_Vector4f &rc, ATOM_ColorARGB color): rectColor(color)
	{
		x = rc.x;
		y = rc.y;
		w = rc.z;
		h = rc.w;
	}

	virtual void draw (ATOM_GUICanvas *canvas, float scaleX, float scaleY, float translationX, float translationY)
	{
	    ATOM_Rect2Di rc;
	    rc.point.x = ATOM_ftol(x * scaleX + translationX);
	    rc.point.y = ATOM_ftol(y * scaleY + translationY);
	    rc.size.w = ATOM_ftol(w * scaleX);
	    rc.size.h = ATOM_ftol(h * scaleY);
		canvas->drawRect (rc, rectColor, 1);
	}

	virtual VectorGraphOp *clone (void) const
	{
		return ATOM_NEW(VG_DrawRectOp, *this);
	}
};

struct VG_TextureRectOp: public VectorGraphOp
{
	float x, y, w, h;
	ATOM_ColorARGB rectColor;
	ATOM_AUTOREF(ATOM_Texture) rectTexture;

	VG_TextureRectOp (void) {}
	VG_TextureRectOp (const ATOM_Vector4f &rc, ATOM_ColorARGB color, ATOM_Texture *texture): rectColor(color), rectTexture(texture)
	{
		x = rc.x;
		y = rc.y;
		w = rc.z;
		h = rc.w;
	}

	virtual void draw (ATOM_GUICanvas *canvas, float scaleX, float scaleY, float translationX, float translationY)
	{
	    ATOM_Rect2Di rc;
	    rc.point.x = ATOM_ftol(x * scaleX + translationX);
	    rc.point.y = ATOM_ftol(y * scaleY + translationY);
	    rc.size.w = ATOM_ftol(w * scaleX);
	    rc.size.h = ATOM_ftol(h * scaleY);
		canvas->drawTexturedRect (0, rc, rectColor, rectTexture.get(), false);
	}

	virtual VectorGraphOp *clone (void) const
	{
		return ATOM_NEW(VG_TextureRectOp, *this);
	}
};

struct VG_DrawTextOp: public VectorGraphOp
{
	ATOM_STRING text;
	ATOM_FontHandle font;
	float x;
	float y;
	ATOM_ColorARGB color;

	VG_DrawTextOp (void) {}
	VG_DrawTextOp (const char *str, ATOM_FontHandle f, float x_pos, float y_pos, ATOM_ColorARGB clr): text(str), font(f), x(x_pos), y(y_pos), color(clr) {}

	virtual void draw (ATOM_GUICanvas *canvas, float scaleX, float scaleY, float translationX, float translationY)
	{
		int ix = ATOM_ftol ((x * scaleX) + translationX);
		int iy = ATOM_ftol ((y * scaleY) + translationY);
		canvas->drawText (text.c_str(), font, ix, iy, color);
	}

	virtual VectorGraphOp *clone (void) const
	{
		return ATOM_NEW(VG_DrawTextOp, *this);
	}
};

struct VG_DrawTextOp2: public VectorGraphOp
{
	ATOM_AUTOREF(ATOM_Text) text;
	float x;
	float y;
	ATOM_ColorARGB color;

	VG_DrawTextOp2 (void) {}
	VG_DrawTextOp2 (ATOM_Text *str, float x_pos, float y_pos, ATOM_ColorARGB clr): text(str), x(x_pos), y(y_pos), color(clr) {}

	virtual void draw (ATOM_GUICanvas *canvas, float scaleX, float scaleY, float translationX, float translationY)
	{
		int ix = ATOM_ftol ((x * scaleX) + translationX);
		int iy = ATOM_ftol ((y * scaleY) + translationY);
		canvas->drawText (text.get(), ix, iy, color);
	}

	virtual VectorGraphOp *clone (void) const
	{
		return ATOM_NEW(VG_DrawTextOp2, *this);
	}
};

struct VG_DrawLineOp: public VectorGraphOp
{
	float x1, y1, x2, y2;
	ATOM_ColorARGB color;

	VG_DrawLineOp (void);
	VG_DrawLineOp (const ATOM_Vector2f &p1, const ATOM_Vector2f &p2, ATOM_ColorARGB clr): color(clr)
	{
		x1 = p1.x;
		y1 = p1.y;
		x2 = p2.x;
		y2 = p2.y;
	}

	virtual void draw (ATOM_GUICanvas *canvas, float scaleX, float scaleY, float translationX, float translationY)
	{
		int a = ATOM_ftol((x1 * scaleX) + translationX);
		int b = ATOM_ftol((y1 * scaleY) + translationY);
		int c = ATOM_ftol((x2 * scaleX) + translationX);
		int d = ATOM_ftol((y2 * scaleY) + translationY);
		canvas->drawLine (ATOM_Point2Di(a, b), ATOM_Point2Di(c, d), color);
	}

	virtual VectorGraphOp *clone (void) const
	{
		return ATOM_NEW(VG_DrawLineOp, *this);
	}
};

struct VG_DrawLineListOp: public VectorGraphOp
{
	ATOM_VECTOR<ATOM_Vector2f> points;
	ATOM_ColorARGB color;

	VG_DrawLineListOp (void);
	VG_DrawLineListOp (const ATOM_Vector2f *p, unsigned numLines, ATOM_ColorARGB clr): color(clr)
	{
		points.resize (numLines * 2);
		for (unsigned i = 0; i < numLines * 2; ++i)
		{
			points[i] = p[i];
		}
	}

	virtual void draw (ATOM_GUICanvas *canvas, float scaleX, float scaleY, float translationX, float translationY)
	{
		static ATOM_VECTOR<ATOM_Point2Di> pt;
		static ATOM_VECTOR<ATOM_ColorARGB> colors;

		if (points.size() > 0)
		{
			unsigned numPoints = points.size();
			unsigned numLines = numPoints / 2;
			pt.resize (numPoints);
			colors.resize (numLines);

			for (unsigned i = 0; i < numPoints; ++i)
			{
				const ATOM_Vector2f &v = points[i];
				pt[i].x = ATOM_ftol((v.x * scaleX) + translationX);
				pt[i].y = ATOM_ftol((v.y * scaleY) + translationY);
			}

			for (unsigned i = 0; i < numLines; ++i)
			{
				colors[i] = color;
			}

			canvas->drawLineList (numLines, &pt[0], &pt[numLines], &colors[0]);
		}
	}

	virtual VectorGraphOp *clone (void) const
	{
		return ATOM_NEW(VG_DrawLineListOp, *this);
	}
};

struct VG_DrawConvexPolygonOp: public VectorGraphOp
{
	ATOM_VECTOR<ATOM_Vector2f> points;
	ATOM_VECTOR<ATOM_Point2Di> ipoints;
	ATOM_ColorARGB color;

	VG_DrawConvexPolygonOp (void) {}
	VG_DrawConvexPolygonOp (unsigned num, const ATOM_Vector2f *pt, ATOM_ColorARGB clr)
	{
		points.resize (num);
		for (unsigned n = 0; n < num; ++n)
		{
			points[n] = pt[n];
		}
		color = clr;
	}

	virtual void draw (ATOM_GUICanvas *canvas, float scaleX, float scaleY, float translationX, float translationY)
	{
		if (points.size() > 0)
		{
			ipoints.resize (points.size());
			for (unsigned i = 0; i < ipoints.size(); ++i)
			{
				ipoints[i].x = ATOM_ftol((points[i].x * scaleX) + translationX);
				ipoints[i].y = ATOM_ftol((points[i].y * scaleX) + translationY);
			}

			canvas->drawConvexPolygon (ipoints.size(), &ipoints[0], color);
		}
	}

	virtual VectorGraphOp *clone (void) const
	{
		return ATOM_NEW(VG_DrawConvexPolygonOp, *this);
	}
};

struct VG_TexturedConvexPolygonOp: public VectorGraphOp
{
	ATOM_VECTOR<ATOM_Vector2f> points;
	ATOM_VECTOR<ATOM_Point2Di> ipoints;
	ATOM_VECTOR<ATOM_Vector2f> uv;
	ATOM_ColorARGB color;
	ATOM_AUTOREF(ATOM_Texture) texture;

	VG_TexturedConvexPolygonOp (void) {}
	VG_TexturedConvexPolygonOp (unsigned num, const ATOM_Vector2f *pt, const ATOM_Vector2f *tc, ATOM_ColorARGB clr, ATOM_Texture *tex)
	{
		color = clr;

		points.resize (num);
		for (unsigned n = 0; n < num; ++n)
		{
			points[n] = pt[n];
		}

		if (tc && tex)
		{
			uv.resize (num);
			for (unsigned n = 0; n < num; ++n)
			{
				uv[n] = tc[n];
			}

			texture = tex;
		}
	}

	virtual void draw (ATOM_GUICanvas *canvas, float scaleX, float scaleY, float translationX, float translationY)
	{
		if (points.size() > 0)
		{
			ipoints.resize (points.size());
			for (unsigned i = 0; i < ipoints.size(); ++i)
			{
				ipoints[i].x = ATOM_ftol((points[i].x * scaleX) + translationX);
				ipoints[i].y = ATOM_ftol((points[i].y * scaleY) + translationY);
			}

			canvas->drawTexturedConvexPolygon (0, ipoints.size(), &ipoints[0], uv.empty() ? 0 : &uv[0], color, texture.get(), false);
		}
	}

	virtual VectorGraphOp *clone (void) const
	{
		return ATOM_NEW(VG_TexturedConvexPolygonOp, *this);
	}
};

ATOM_VectorGraph::ATOM_VectorGraph (void)
{
	_scaleX = 1.f;
	_scaleY = 1.f;
	_translationX = 0.f;
	_translationY = 0.f;
}

ATOM_VectorGraph::~ATOM_VectorGraph (void)
{
	removeAll ();
}

ATOM_VectorGraph::ATOM_VectorGraph (const ATOM_VectorGraph &other)
{
	_opList.resize (other._opList.size());
	for (int i = 0; i < _opList.size(); ++i)
	{
		_opList[i] = other._opList[i]->clone ();
	}
	_scaleX = other._scaleX;
	_scaleY = other._scaleY;
	_translationX = other._translationX;
	_translationY = other._translationY;
}

ATOM_VectorGraph &ATOM_VectorGraph::operator = (const ATOM_VectorGraph &other)
{
	ATOM_VectorGraph tmp(other);
	swap (tmp);
	return *this;
}

void ATOM_VectorGraph::swap (ATOM_VectorGraph &other)
{
	std::swap (_opList, other._opList);
	std::swap (_scaleX, other._scaleX);
	std::swap (_scaleY, other._scaleY);
	std::swap (_translationX, other._translationX);
	std::swap (_translationY, other._translationY);
}

void ATOM_VectorGraph::addDrawRectOp (const ATOM_Vector4f &rect, ATOM_ColorARGB color)
{
	_opList.push_back (ATOM_NEW (VG_DrawRectOp, rect, color));
}

void ATOM_VectorGraph::addFillRectOp (const ATOM_Vector4f &rect, ATOM_ColorARGB color, ATOM_Texture *texture)
{
	_opList.push_back (ATOM_NEW (VG_TextureRectOp, rect, color, texture));
}

void ATOM_VectorGraph::addDrawTextOp (const char *text, ATOM_FontHandle font, float x, float y, ATOM_ColorARGB color)
{
	_opList.push_back (ATOM_NEW(VG_DrawTextOp, text, font, x, y, color));
}

void ATOM_VectorGraph::addDrawTextOp (ATOM_Text *text, float x, float y, ATOM_ColorARGB color)
{
	_opList.push_back (ATOM_NEW(VG_DrawTextOp2, text, x, y, color));
}

void ATOM_VectorGraph::addDrawLineOp (const ATOM_Vector2f &p1, const ATOM_Vector2f &p2, ATOM_ColorARGB color)
{
	_opList.push_back (ATOM_NEW(VG_DrawLineOp, p1, p2, color));
}

void ATOM_VectorGraph::addDrawLineListOp (const ATOM_Vector2f *points, unsigned numLines, ATOM_ColorARGB color)
{
	_opList.push_back (ATOM_NEW(VG_DrawLineListOp, points, numLines, color));
}

void ATOM_VectorGraph::addDrawConvexPolygonOp (unsigned num, const ATOM_Vector2f *points, ATOM_ColorARGB color)
{
	_opList.push_back (ATOM_NEW(VG_DrawConvexPolygonOp, num, points, color));
}

void ATOM_VectorGraph::addFillConvexPolygonOp (unsigned num, const ATOM_Vector2f *points, const ATOM_Vector2f *uv, ATOM_ColorARGB color, ATOM_Texture *texture)
{
	_opList.push_back (ATOM_NEW(VG_TexturedConvexPolygonOp, num, points, uv, color, texture));
}

void ATOM_VectorGraph::removeAll (void)
{
	for (unsigned i = 0; i < _opList.size(); ++i)
	{
		ATOM_DELETE(_opList[i]);
	}

	_opList.resize(0);
}

void ATOM_VectorGraph::draw (ATOM_GUICanvas *canvas)
{
	for (unsigned i = 0; i < _opList.size(); ++i)
	{
		_opList[i]->draw (canvas, _scaleX, _scaleY, _translationX, _translationY);
	}
}

void ATOM_VectorGraph::setScale (float x, float y)
{
	_scaleX = x;
	_scaleY = y;
}

void ATOM_VectorGraph::setTranslation (float x, float y)
{
	_translationX = x;
	_translationY = y;
}



