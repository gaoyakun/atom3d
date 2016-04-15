#ifndef __ATOM3D_ENGINE_REALTIMECTRL_H
#define __ATOM3D_ENGINE_REALTIMECTRL_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_kernel.h"
#include "node.h"
#include "topwindow.h"

class ATOM_Scene;
class ATOM_Camera;
class ATOM_Node;
class ATOM_PickVisitor;

class ATOM_ENGINE_API ATOM_RealtimeCtrl: public ATOM_Widget
{
public:
	ATOM_RealtimeCtrl();
	ATOM_RealtimeCtrl (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id = ATOM_Widget::AnyId, ATOM_Widget::ShowState showState = ATOM_Widget::ShowNormal);
	virtual ~ATOM_RealtimeCtrl (void);

public:
	virtual ATOM_WidgetType getType (void) const;

public:
	void setScene (ATOM_Scene *scene);
	ATOM_Scene *getScene (void) const;
	bool constructRay (int x, int y, ATOM_Ray &ray);
	void pick (int x, int y, ATOM_PickVisitor &v);
	void transparent(bool enable);
	bool isTransparent (void) const;
	void setViewport (const ATOM_Rect2Di &rc);
	const ATOM_Rect2Di &getViewport (void) const;
	bool beginRender (ATOM_RenderDevice *device, bool clear);
	void render (ATOM_RenderDevice *device);
	void endRender (ATOM_RenderDevice *device);

protected:
	virtual void drawClient (ATOM_GUICanvas *canvas, ATOM_WidgetState state);

public:
	void onResize (ATOM_WidgetResizeEvent *event);
	void onPaint (ATOM_WidgetDrawClientEvent *event);

protected:
	ATOM_Scene *_scene;
	float _w;
	float _h;
	bool _transparent;
	ATOM_Rect2Di _viewport;
	ATOM_Rect2Di _savedViewPort;

	ATOM_DECLARE_EVENT_MAP(ATOM_RealtimeCtrl, ATOM_TopWindow)
};


#endif // __ATOM3D_ENGINE_REALTIMECTRL_H
