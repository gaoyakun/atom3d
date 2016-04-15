#include "stdafx.h"
#include "realtimectrl.h"
#include "scene.h"

ATOM_BEGIN_EVENT_MAP(ATOM_RealtimeCtrl, ATOM_Widget)
  ATOM_EVENT_HANDLER(ATOM_RealtimeCtrl, ATOM_WidgetResizeEvent, onResize)
  ATOM_EVENT_HANDLER(ATOM_RealtimeCtrl, ATOM_WidgetDrawClientEvent, onPaint)
ATOM_END_EVENT_MAP

ATOM_RealtimeCtrl::ATOM_RealtimeCtrl()
{
	ATOM_STACK_TRACE(ATOM_RealtimeCtrl::ATOM_RealtimeCtrl);

	_scene = 0;
	_transparent = false;
	_viewport.point.x = 0;
	_viewport.point.y = 0;
	_viewport.size.w = 0;
	_viewport.size.h = 0;

	setClientImageId (-1);

#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif
}

ATOM_RealtimeCtrl::ATOM_RealtimeCtrl (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_Widget::ShowState showState)
	:
ATOM_Widget (parent, rect, style, id, showState)
{
	ATOM_STACK_TRACE(ATOM_RealtimeCtrl::ATOM_RealtimeCtrl);

	_scene = 0;
	_transparent = false;
	_viewport.point.x = 0;
	_viewport.point.y = 0;
	_viewport.size.w = 0;
	_viewport.size.h = 0;

	resize (rect);

	setClientImageId (-1);

#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif
}

ATOM_RealtimeCtrl::~ATOM_RealtimeCtrl (void)
{
	ATOM_STACK_TRACE(ATOM_RealtimeCtrl::~ATOM_RealtimeCtrl);
}

ATOM_WidgetType ATOM_RealtimeCtrl::getType (void) const
{
	return WT_REALTIMECTRL;
}

void ATOM_RealtimeCtrl::drawClient (ATOM_GUICanvas *canvas, ATOM_WidgetState state)
{
	return;
}

void ATOM_RealtimeCtrl::setScene (ATOM_Scene *scene)
{
	ATOM_STACK_TRACE(ATOM_RealtimeCtrl::setScene);

	_scene = scene;
}

ATOM_Scene *ATOM_RealtimeCtrl::getScene (void) const
{
	ATOM_STACK_TRACE(ATOM_RealtimeCtrl::getScene);

	return _scene;
}

bool ATOM_RealtimeCtrl::isTransparent (void) const
{
	ATOM_STACK_TRACE(ATOM_RealtimeCtrl::isTransparent);

	return _transparent;
}

void ATOM_RealtimeCtrl::onResize (ATOM_WidgetResizeEvent *event)
{
	ATOM_STACK_TRACE(ATOM_RealtimeCtrl::onResize);

	callParentHandler (event);

	if (_scene)
	{
		if (_viewport.size.w == 0 || _viewport.size.h == 0)
		{
			int w = _viewport.size.w == 0 ? _clientRect.size.w : _viewport.size.w;
			int h = _viewport.size.h == 0 ? _clientRect.size.h : _viewport.size.h;
			_scene->resizeView (w, h);
		}
	}
}

void ATOM_RealtimeCtrl::setViewport (const ATOM_Rect2Di &rc)
{
	if (rc != _viewport)
	{
		_viewport = rc;

		if (_scene)
		{
			int w = _viewport.size.w == 0 ? _clientRect.size.w : _viewport.size.w;
			int h = _viewport.size.h == 0 ? _clientRect.size.h : _viewport.size.h;
			_scene->resizeView (w, h);
		}
	}
}

const ATOM_Rect2Di &ATOM_RealtimeCtrl::getViewport (void) const
{
	return _viewport;
}

bool ATOM_RealtimeCtrl::beginRender (ATOM_RenderDevice *device, bool clear)
{
	ATOM_STACK_TRACE(ATOM_RealtimeCtrl::beginRender);

	if (!device)
	{
		return false;
	}

	device->getViewport (device->getCurrentView(), &_savedViewPort.point.x, &_savedViewPort.point.y, &_savedViewPort.size.w, &_savedViewPort.size.h);

	ATOM_Point2Di pt(0, 0);
	clientToViewport (&pt);
	pt.x += _savedViewPort.point.x;
	pt.y += _savedViewPort.point.y;
	pt.x += _viewport.point.x;
	pt.y += _viewport.point.y;
	int w = _viewport.size.w ? _viewport.size.w : _clientRect.size.w;
	int h = _viewport.size.h ? _viewport.size.h : _clientRect.size.h;
	ATOM_Rect2Di rc(pt, ATOM_Size2Di(w, h));

	if (_scene)
	{
		_scene->getCamera()->setViewport (rc.point.x, rc.point.y, rc.size.w, rc.size.h);
	}

	device->setViewport (device->getCurrentView(), rc.point.x, rc.point.y, rc.size.w, rc.size.h);
	device->clear (clear, true, true);

	if (!_scene)
	{
		device->setViewport (device->getCurrentView(), _savedViewPort.point.x, _savedViewPort.point.y, _savedViewPort.size.w, _savedViewPort.size.h);
		return false;
	}

	return true;
}

void ATOM_RealtimeCtrl::render (ATOM_RenderDevice *device)
{
	ATOM_STACK_TRACE(ATOM_RealtimeCtrl::render);

	_scene->render (device, false);
}

void ATOM_RealtimeCtrl::endRender (ATOM_RenderDevice *device)
{
	ATOM_STACK_TRACE(ATOM_RealtimeCtrl::endRender);

	device->setViewport (device->getCurrentView(), _savedViewPort.point.x, _savedViewPort.point.y, _savedViewPort.size.w, _savedViewPort.size.h);
}

void ATOM_RealtimeCtrl::onPaint (ATOM_WidgetDrawClientEvent *event)
{
	ATOM_STACK_TRACE(ATOM_RealtimeCtrl::onPaint);
	ATOM_Widget::onPaint (event);

	ATOM_RenderDevice* device = ATOM_GetRenderDevice ();
	if (!device)
	{
		return;
	}

	if (beginRender (device, !_transparent))
	{
		render (device);
		endRender (device);
	}
	//if (_scene)
	//{
	//	int x, y, w, h;
	//	device->getViewport (device->getCurrentView(), &x, &y, &w, &h);

	//	ATOM_Point2Di pt(0,0);
	//	clientToViewport (&pt);
	//	pt.x += x;
	//	pt.y += y;
	//	ATOM_Rect2Di rc(pt, _clientRect.size);
	//	_scene->getCamera()->setViewport (rc.x, rc.y, rc.w, rc.h);
	//	device->setViewport (device->getCurrentView(), rc.x, rc.y, rc.w, rc.h);
	//	device->clear (!_transparent, true, true);
	//	//device->clear (_transparent ? false : true, true, true);

	//	bool isWireframe = device->isWireFrameMode ();
	//	device->enableWireFrameMode (_wireframe);
	//	_scene->render (device, !_transparent);
	//	device->enableWireFrameMode (isWireframe);

	//	device->setViewport (device->getCurrentView(), x, y, w, h);
	//}
	//else
	//{
	//	device->clear (true, true, true);
	//}
}

bool ATOM_RealtimeCtrl::constructRay (int x, int y, ATOM_Ray &ray)
{
	ATOM_STACK_TRACE(ATOM_RealtimeCtrl::constructRay);

	if (_scene)
	{
		int w = _viewport.size.w ? _viewport.size.w : _clientRect.size.w;
		int h = _viewport.size.h ? _viewport.size.h : _clientRect.size.h;

		float x0 = 2.f * float(x)/float(w) - 1.f;
		float y0 = 1.f - 2.f * float(y)/float(h);

		ATOM_Matrix4x4f mat;
		mat.invertFrom (_scene->getCamera()->getViewProjectionMatrix ());
		ATOM_Matrix4x4f matTest = mat >> _scene->getCamera()->getViewProjectionMatrix ();
		ATOM_Vector4f vTarget = mat >> ATOM_Vector4f(x0, y0, 0.5f, 1.f);
		vTarget /= vTarget.w;
		ATOM_Vector3f vEye (_scene->getCamera()->getViewMatrix().getRow3 (3));
		ATOM_Vector3f dir = vTarget.getVector3() - vEye;
		dir.normalize ();
		ray = ATOM_Ray (vEye, dir);

		return true;
	}

	return false;
}

void ATOM_RealtimeCtrl::pick (int x, int y, ATOM_PickVisitor &v)
{
	ATOM_STACK_TRACE(ATOM_RealtimeCtrl::pick);

	if (_scene)
	{
		ATOM_Node *node = _scene->getRootNode();
		if (node)
		{
			ATOM_Ray ray;
			constructRay (x, y, ray);
			v.setCamera (_scene->getCamera ());
			v.setScreenPos (x, y);
			v.setRay (ray);
			v.traverse (*node);
		}
	}
}

void ATOM_RealtimeCtrl::transparent(bool enable)
{
	ATOM_STACK_TRACE(ATOM_RealtimeCtrl::transparent);

	_transparent = enable;
}


