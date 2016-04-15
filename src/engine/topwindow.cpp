#include "stdafx.h"
#include "topwindow.h"
#include "gui_events.h"
#include "gui_canvas.h"

ATOM_BEGIN_EVENT_MAP(ATOM_TopWindow, ATOM_Widget)
	ATOM_EVENT_HANDLER(ATOM_TopWindow, ATOM_TopWindowCloseEvent, onClose)
	ATOM_EVENT_HANDLER(ATOM_TopWindow, ATOM_WidgetHitTestEvent, onHitTest)
	ATOM_EVENT_HANDLER(ATOM_TopWindow, ATOM_WidgetDrawClientEvent, onPaint)
	ATOM_EVENT_HANDLER(ATOM_TopWindow, ATOM_WidgetResizeEvent, onResize)
ATOM_END_EVENT_MAP

ATOM_TopWindow::ATOM_TopWindow()
{
	ATOM_STACK_TRACE(ATOM_TopWindow::ATOM_TopWindow);

	_displayInvalid = true;
	_enableWindowTexture = false;
	_clientImageId = ATOM_IMAGEID_WINDOW_BKGROUND;
	_clientDragging = false;

	_text = NULL;
	_align = AlignX_Middle|AlignY_Middle;
	_textDirty = false;
	_textPosition.x = 0;
	_textPosition.y = 0;
	_offset.x = 0;
	_offset.y = 0;

#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif
}

ATOM_TopWindow::ATOM_TopWindow (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_Widget::ShowState showState)
:
ATOM_Widget (parent, rect, (style & ~ATOM_Widget::Control), id, showState)
{
  ATOM_STACK_TRACE(ATOM_TopWindow::ATOM_TopWindow);

  _displayInvalid = true;
  _enableWindowTexture = false;
  _clientImageId = ATOM_IMAGEID_WINDOW_BKGROUND;
  _clientDragging = false;

  _text = ATOM_HARDREF(ATOM_Text) ();
  _text->setFont (ATOM_GUIFont::getFontHandle(getFont()));
  _text->setCharMargin (ATOM_GUIFont::getFontCharMargin(getFont()));
  _text->setZValue (1.f);
  _text->setDisplayMode (ATOM_Text::DISPLAYMODE_2D);
  _align = AlignX_Middle|AlignY_Middle;
  _textDirty = false;
  _textPosition.x = 0;
  _textPosition.y = 0;
  _offset.x = 0;
  _offset.y = 0;

  resize (rect);

#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif
}

ATOM_TopWindow::~ATOM_TopWindow ()
{
	ATOM_STACK_TRACE(ATOM_TopWindow::~ATOM_TopWindow);
}

bool ATOM_TopWindow::isTopWindow (void) const
{
	ATOM_STACK_TRACE(ATOM_TopWindow::isTopWindow);

	return true;
}

ATOM_WidgetType ATOM_TopWindow::getType (void) const
{
  return WT_TOPWINDOW;
}

void ATOM_TopWindow::invalidateDisplay (void)
{
  ATOM_STACK_TRACE(ATOM_TopWindow::invalidateDisplay);

  _displayInvalid = true;

  if (_guiRenderer)
  {
	  _guiRenderer->invalidate ();
  }
}

void ATOM_TopWindow::moveTo (int x, int y)
{
	ATOM_STACK_TRACE(ATOM_TopWindow::moveTo);

	ATOM_Widget::moveTo (x, y);

	if (_guiRenderer)
	{
		_guiRenderer->invalidate ();
	}
}


void ATOM_TopWindow::setText (const char *str)
{
	ATOM_STACK_TRACE(ATOM_TopWindow::setText);

	_caption = str ? str : "";
	_textDirty = true;
	invalidate ();
}

const char* ATOM_TopWindow::getText (void) const
{
	ATOM_STACK_TRACE(ATOM_TopWindow::getText);

	return _caption.c_str();
}

void ATOM_TopWindow::setFont (ATOM_GUIFont::handle font)
{
	ATOM_Widget::setFont (font);

	_text->setFont (ATOM_GUIFont::getFontHandle (getFont()));
	_text->setCharMargin (ATOM_GUIFont::getFontCharMargin (getFont()));
	_textDirty = true;
}

void ATOM_TopWindow::setAlign(int align)
{
	ATOM_STACK_TRACE(ATOM_TopWindow::setAlign);

	_align = align;
	_textDirty = true;
	invalidate ();
}

int ATOM_TopWindow::getAlign() const
{
	ATOM_STACK_TRACE(ATOM_TopWindow::getAlign);

	return _align;
}

// 设置文字偏移
void ATOM_TopWindow::setOffset(const ATOM_Point2Di& offset)
{
	ATOM_STACK_TRACE(ATOM_TopWindow::setOffset);

	_offset = offset;
}

// 取得文字偏移
const ATOM_Point2Di& ATOM_TopWindow::getOffset() const
{
	ATOM_STACK_TRACE(ATOM_TopWindow::getOffset);

	return _offset;
}


void ATOM_TopWindow::calcTextLayout (void)
{
	ATOM_STACK_TRACE(ATOM_TopWindow::calcTextLayout);

	ATOM_FontHandle font = ATOM_GUIFont::getFontHandle (getFont());
	int charmargin = ATOM_GUIFont::getFontCharMargin (getFont());
	ATOM_SetCharMargin (charmargin);
	int l, t, w, h;
	unsigned clip = ATOM_ClipString (font, _caption.c_str(), _clientRect.size.w, &l, &t, &w, &h);

	// 对齐
	if(_align & AlignX_Left)
	{
		_textPosition.x = _offset.x;
	}
	else if(_align & AlignX_Right)
	{
		_textPosition.x = _clientRect.size.w - w - _offset.x;
	}
	else
	{
		_textPosition.x = (_clientRect.size.w - w) / 2 + _offset.x;
	}

	const char *testStr = "gf";
	ATOM_CalcStringBounds (font, testStr, strlen(testStr), &l, &t, &w, &h);

	if(_align & AlignY_Top)
	{
		_textPosition.y = -t + _offset.y;
	}
	else if(_align & AlignY_Bottom)
	{
		_textPosition.y = _clientRect.size.h - h - t - _offset.y;
	}
	else
	{
		_textPosition.y = (_clientRect.size.h - h) / 2 - t + _offset.y;
	}


	_text->setString (_caption.substr(0, clip).c_str());
}

void ATOM_TopWindow::enableWindowTexture (bool enable)
{
	ATOM_STACK_TRACE(ATOM_TopWindow::enableWindowTexture);

	enable = false;

	if (enable && !_enableWindowTexture)
	{
		invalidate ();
	}
	_enableWindowTexture = enable;
}

bool ATOM_TopWindow::isWindowTextureEnabled (void) const
{
	ATOM_STACK_TRACE(ATOM_TopWindow::isWindowTextureEnabled);

	return _enableWindowTexture;
}

ATOM_Texture *ATOM_TopWindow::getWindowTexture (void) const
{
	ATOM_STACK_TRACE(ATOM_TopWindow::getWindowTexture);

	return _renderTarget.get();
}

void ATOM_TopWindow::drawSelf (void)
{
  ATOM_STACK_TRACE(ATOM_TopWindow::drawSelf);

  ATOM_RenderDevice *device = ATOM_GetRenderDevice();
  ATOM_ASSERT(device);

  bool rtt = _enableWindowTexture && !isControl ();

  if (rtt)
  {
    if (_displayInvalid)
    {
      _displayInvalid = false;

      if (!_renderTarget)
      {
        _renderTarget = device->allocTexture (0, 0, _widgetRect.size.w, _widgetRect.size.h, ATOM_PIXEL_FORMAT_RGBA8888,
          ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET|ATOM_Texture::NOCOMPRESS|ATOM_Texture::NOMIPMAP);
        ATOM_ASSERT(_renderTarget);
      }

	  ATOM_AUTOREF(ATOM_Texture) prevRT = device->getRenderTarget (0);

	  int vx, vy, vw, vh;
	  device->getViewport(device->getCurrentView(), &vx, &vy, &vw, &vh);

      device->setRenderTarget (0, _renderTarget.get());
      device->setViewport (device->getCurrentView(), 0, 0, _renderTarget->getWidth(), _renderTarget->getHeight());
	  _guiRenderer->pushScissor (ATOM_Rect2Di(0, 0, _renderTarget->getWidth(), _renderTarget->getHeight()), false);
	  _guiRenderer->setRenderTargetWindow (this);

      ATOM_Widget::drawSelf ();

	  _guiRenderer->setRenderTargetWindow (0);
      device->setRenderTarget (0, prevRT.get());
      device->setViewport (device->getCurrentView(), vx, vy, vw, vh);
	  _guiRenderer->popScissor ();
    }
  }
  else
  {
    ATOM_Widget::drawSelf ();
  }

  if (rtt)
  {
    ATOM_GUICanvas canvas(this);
	ATOM_Point2Di pt(-_clientRect.point.x, -_clientRect.point.y);
	clientToViewport (&pt);
    ATOM_Rect2Di rc(pt, _widgetRect.size);

    canvas.resize (rc.size);
    canvas.setOrigin (rc.point);
    rc.point.x = 0;
    rc.point.y = 0;

    canvas.drawTexturedRect (0, rc, ATOM_ColorARGB(1.f, 1.f, 1.f, 1.f), _renderTarget.get(), false);
  }
}

void ATOM_TopWindow::close (void)
{
	ATOM_STACK_TRACE(ATOM_TopWindow::close);

	ATOM_TopWindowCloseEvent event(getId());
	handleEvent (&event);
}

void ATOM_TopWindow::onClose (ATOM_TopWindowCloseEvent *event)
{
	ATOM_STACK_TRACE(ATOM_TopWindow::onClose);

	show (ATOM_Widget::Hide);
}

void ATOM_TopWindow::onHitTest (ATOM_WidgetHitTestEvent *event)
{
	ATOM_STACK_TRACE(ATOM_TopWindow::onHitTest);

	ATOM_Widget::onHitTest(event);

	if (event->hitTestResult.hitPoint == HitClient && _clientDragging)
	{
		event->hitTestResult.lug = true;
	}
}

void ATOM_TopWindow::onPaint (ATOM_WidgetDrawClientEvent *event)
{
	ATOM_STACK_TRACE(ATOM_TopWindow::onPaint);

	if (_clientImageId != ATOM_INVALID_IMAGEID)
	{
#if 1
		ATOM_GUIImage *image = getValidImage (_clientImageId);
#else
		const ATOM_GUIImageList *imagelist = getValidImageList ();
		ATOM_GUIImage *image = imagelist->getImage (_clientImageId);
#endif
		if (!image)
		{
			image = ATOM_GUIImageList::getDefaultImageList().getImage (ATOM_IMAGEID_WINDOW_BKGROUND);
			ATOM_ASSERT(image);
		}
		image->draw (event->state, event->canvas, ATOM_Rect2Di(ATOM_Point2Di(0, 0), event->canvas->getSize()));

	}

	if (!_caption.empty())
	{
		if (_textDirty)
		{
			calcTextLayout ();
			_textDirty = false;
		}

		event->canvas->drawText (_text.get(), _textPosition.x, _textPosition.y, getFontColor());
	}
}

void ATOM_TopWindow::onResize (ATOM_WidgetResizeEvent *event)
{
	ATOM_STACK_TRACE(ATOM_TopWindow::onResize);

	callParentHandler (event);

	_textDirty = true;
}

//  ATOM_Point2Di ATOM_TopWindow::getViewportOffset (void)
//  {
//    if (!_enableWindowTexture && !isControl ())
//    {
 //   ATOM_Point2Di pt(-_clientRect.point.x, -_clientRect.point.y);
	//clientToScreen (&pt);
//      ATOM_Rect2Di rc(pt, _widgetRect.size);

//      rc.x += getRenderer()->getViewport().x;
//      rc.y += getRenderer()->getViewport().y;
//      return rc.point;
//    }

//    return ATOM_Point2Di (0, 0);
//  }

void ATOM_TopWindow::allowClientDragging (bool allow)
{
	ATOM_STACK_TRACE(ATOM_TopWindow::allowClientDragging);

	_clientDragging = allow;
}

bool ATOM_TopWindow::isClientDraggingAllowed (void) const
{
	ATOM_STACK_TRACE(ATOM_TopWindow::isClientDraggingAllowed);

	return _clientDragging;
}

