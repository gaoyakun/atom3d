#ifndef __ATOM3D_ENGINE_VECTORGRAPH_H
#define __ATOM3D_ENGINE_VECTORGRAPH_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_utils.h"
#include "../ATOM_render.h"

#include "basedefs.h"



class ATOM_GUICanvas;

struct VectorGraphOp
{
	virtual ~VectorGraphOp (void) {}
	virtual void draw (ATOM_GUICanvas *canvas, float scaleX, float scaleY, float translationX, float translationY) = 0;
	virtual VectorGraphOp *clone (void) const = 0;
};

class ATOM_ENGINE_API ATOM_VectorGraph
{
public:
	ATOM_VectorGraph (void);
	ATOM_VectorGraph (const ATOM_VectorGraph &other);
	virtual ~ATOM_VectorGraph (void);
	ATOM_VectorGraph &operator = (const ATOM_VectorGraph &other);
	void swap (ATOM_VectorGraph &other);

public:
	void addDrawRectOp (const ATOM_Vector4f &rect, ATOM_ColorARGB color);
	void addFillRectOp (const ATOM_Vector4f &rect, ATOM_ColorARGB color, ATOM_Texture *texture);
	void addFillRectOp (const ATOM_Vector4f &rect, ATOM_Material *material);
	void addDrawTextOp (const char *text, ATOM_FontHandle font, float x, float y, ATOM_ColorARGB color);
	void addDrawTextOp (ATOM_Text *text, float x, float y, ATOM_ColorARGB color);
	void addDrawLineOp (const ATOM_Vector2f &p1, const ATOM_Vector2f &p2, ATOM_ColorARGB color);
	void addDrawLineListOp (const ATOM_Vector2f *points, unsigned numLines, ATOM_ColorARGB color);
	void addDrawConvexPolygonOp (unsigned num, const ATOM_Vector2f *points, ATOM_ColorARGB color);
	void addFillConvexPolygonOp (unsigned num, const ATOM_Vector2f *points, const ATOM_Vector2f *uv, ATOM_ColorARGB color, ATOM_Texture *texture);
	void removeAll (void);
	void setScale (float x, float y);
	void setTranslation (float x, float y);
	void draw (ATOM_GUICanvas *canvas);

private:
	ATOM_VECTOR<VectorGraphOp*> _opList;
	float _scaleX;
	float _scaleY;
	float _translationX;
	float _translationY;
};


#endif // __ATOM3D_ENGINE_VECTORGRAPH_H
