/**	\file animatedtexturechannel.h
 *	渲染底层相关的事件类的声明.
 *
 *	\author 高雅昆
 *	\addtogroup render
 *	@{
 */

#ifndef __ATOM_GLRENDER_EVENTS_H
#define __ATOM_GLRENDER_EVENTS_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_kernel.h"
#include "keysymbols.h"

class ATOM_RenderWindow;

//! 鼠标移动事件
class ATOM_MouseMoveEvent: public ATOM_Event
{
public:
  ATOM_MouseMoveEvent(void):win(0), x(0), y(0), xrel(0), yrel(0)
  { 
    states[0] = 0;
    states[1] = 0;
    states[2] = 0; 
  }
  ATOM_MouseMoveEvent(ATOM_RenderWindow *win_, int x_, int y_, int xrel_, int yrel_, char state0, char state1, char state2): win(win_), x(x_), y(y_), xrel(xrel_), yrel(yrel_) 
  { 
    states[0] = state0; 
    states[1] = state1; 
    states[2] = state2; 
  }

  ATOM_RenderWindow *win;
  int x, y;
  int xrel, yrel;
  char states[3];

  ATOM_DECLARE_EVENT(ATOM_MouseMoveEvent)
};

//! 鼠标滚轮事件
class ATOM_MouseWheelEvent: public ATOM_Event
{
public:
  ATOM_MouseWheelEvent(void): win(0), x(0), y(0), delta(0), keymod(0)
  {
    states[0] = 0;
    states[1] = 0;
    states[2] = 0;
  }
  ATOM_MouseWheelEvent(ATOM_RenderWindow *win_, int x_, int y_, int delta_, unsigned keymod_, char states0, char states1, char states2): win(win_), x(x_), y(y_), delta(delta_), keymod(keymod_)
  {
    states[0] = states0;
    states[1] = states1;
    states[2] = states2;
  }

  ATOM_RenderWindow *win;
  int x, y, delta;
  unsigned keymod;
  char states[3];

  ATOM_DECLARE_EVENT(ATOM_MouseWheelEvent)
};

//! 鼠标双击事件
class ATOM_MouseDblClickEvent: public ATOM_Event
{
public:
  ATOM_MouseDblClickEvent(void): win(0), x(0), y(0), button(BUTTON_UNKNOWN), keymod(0) {}
  ATOM_MouseDblClickEvent(ATOM_RenderWindow *win_, int x_, int y_, ATOM_MouseButton button_, unsigned keymod_): win(win_), x(x_), y(y_), button(button_), keymod(keymod_) {}

  ATOM_RenderWindow *win;
  int x, y;
  ATOM_MouseButton button;
  unsigned keymod;

  ATOM_DECLARE_EVENT(ATOM_MouseDblClickEvent)
};

//! 鼠标按钮按下事件
class ATOM_MouseButtonDownEvent: public ATOM_Event
{
public:
  ATOM_MouseButtonDownEvent(void): win(0), x(0), y(0), button(BUTTON_UNKNOWN), keymod(0) {}
  ATOM_MouseButtonDownEvent(ATOM_RenderWindow *win_, int x_, int y_, ATOM_MouseButton button_, unsigned keymod_): win(win_), x(x_), y(y_), button(button_), keymod(keymod_) {}

  ATOM_RenderWindow *win;
  int x, y;
  ATOM_MouseButton button;
  unsigned keymod;

  ATOM_DECLARE_EVENT(ATOM_MouseButtonDownEvent)
};

//! 鼠标按钮释放事件
class ATOM_MouseButtonUpEvent: public ATOM_Event
{
public:
  ATOM_MouseButtonUpEvent(void): win(0), x(0), y(0), button(BUTTON_UNKNOWN), keymod(0) {}
  ATOM_MouseButtonUpEvent(ATOM_RenderWindow *win_, int x_, int y_, ATOM_MouseButton button_, unsigned keymod_): win(win_), x(x_), y(y_), button(button_), keymod(keymod_) {}

  ATOM_RenderWindow *win;
  int x, y;
  ATOM_MouseButton button;
  unsigned keymod;

  ATOM_DECLARE_EVENT(ATOM_MouseButtonUpEvent)
};

//! 键盘按下事件
class ATOM_KeyDownEvent: public ATOM_Event
{
public:
  ATOM_KeyDownEvent(void): win(0), key(KEY_UNKNOWN), keymod(0) {}
  ATOM_KeyDownEvent(ATOM_RenderWindow *win_, ATOM_Key key_, unsigned keymod_): win(win_), key(key_), keymod(keymod_) {}

  ATOM_RenderWindow *win;
  ATOM_Key key;
  unsigned keymod;

  ATOM_DECLARE_EVENT(ATOM_KeyDownEvent)
};

//! 键盘释放事件
class ATOM_KeyUpEvent: public ATOM_Event
{
public:
  ATOM_KeyUpEvent(void): win(0), key(KEY_UNKNOWN), keymod(0) {}
  ATOM_KeyUpEvent(ATOM_RenderWindow *win_, ATOM_Key key_, unsigned keymod_): win(win_), key(key_), keymod(keymod_) {}

  ATOM_RenderWindow *win;
  ATOM_Key key;
  unsigned keymod;

  ATOM_DECLARE_EVENT(ATOM_KeyUpEvent)
};

//! 窗口大小改变事件
class ATOM_WindowResizeEvent: public ATOM_Event
{
public:
  ATOM_WindowResizeEvent(void): win(0), width(0), height(0) {}
  ATOM_WindowResizeEvent(ATOM_RenderWindow *win_, unsigned w, unsigned h): win(win_), width(w), height(h) {}

  ATOM_RenderWindow *win;
  unsigned width, height;

  ATOM_DECLARE_EVENT(ATOM_WindowResizeEvent)
};

//! 窗口激活事件
class ATOM_WindowActiveEvent: public ATOM_Event
{
public:
  ATOM_WindowActiveEvent(void): win(0), active(true) {}
  ATOM_WindowActiveEvent(ATOM_RenderWindow *win_, bool b): win(win_), active(b) {}

  ATOM_RenderWindow *win;
  bool active;

  ATOM_DECLARE_EVENT(ATOM_WindowActiveEvent)
};

//! 字符输入事件
class ATOM_CharEvent: public ATOM_Event
{
public:
  ATOM_CharEvent(void): win(0), mbcc(0), unicc(0) {}
  ATOM_CharEvent(ATOM_RenderWindow *win_, unsigned short mbcc_, unsigned short unicc_): win(win_), mbcc(mbcc_), unicc(unicc_) {}

  ATOM_RenderWindow *win;
  unsigned short mbcc, unicc;
  int codec;

  ATOM_DECLARE_EVENT(ATOM_CharEvent)
};

//! 窗口移动事件
class ATOM_WindowMoveEvent: public ATOM_Event
{
public:
  ATOM_WindowMoveEvent(void): win(0), x(0), y(0), xrel(0), yrel(0) {}
  ATOM_WindowMoveEvent(ATOM_RenderWindow *win_, int x_, int y_, int xrel_, int yrel_): win(win_), x(x_), y(y_), xrel(xrel_), yrel(yrel_) {}

  ATOM_RenderWindow *win;
  int x, y, xrel, yrel;

  ATOM_DECLARE_EVENT(ATOM_WindowMoveEvent)
};

//! 窗口重绘事件
class ATOM_WindowExposeEvent: public ATOM_Event
{
public:
  ATOM_WindowExposeEvent(void): win(0) {}
  ATOM_WindowExposeEvent(ATOM_RenderWindow* win_): win(win_) {}

  ATOM_RenderWindow *win;

  ATOM_DECLARE_EVENT(ATOM_WindowExposeEvent)
};

//! 窗口关闭事件
class ATOM_WindowCloseEvent: public ATOM_Event
{
public:
  ATOM_WindowCloseEvent(void): win(0) {}
  ATOM_WindowCloseEvent(ATOM_RenderWindow *win_): win(win_) {}

  ATOM_RenderWindow *win;

  ATOM_DECLARE_EVENT(ATOM_WindowCloseEvent)
};

//! 文件拖放事件
class ATOM_WindowFilesDroppedEvent: public ATOM_Event
{
public:
	ATOM_WindowFilesDroppedEvent(void): win(0) {}
	ATOM_WindowFilesDroppedEvent(ATOM_RenderWindow *win_): win(win_) {}

	ATOM_RenderWindow *win;
	ATOM_VECTOR<ATOM_STRING> filesDropped;
	int x;
	int y;

	ATOM_DECLARE_EVENT(ATOM_WindowFilesDroppedEvent)
};

//! 设备丢失事件
class ATOM_DeviceLostEvent: public ATOM_Event
{
  ATOM_DECLARE_EVENT(ATOM_DeviceLostEvent)
};

//! 设备复位事件
class ATOM_DeviceResetEvent: public ATOM_Event
{
public:
  ATOM_DeviceResetEvent (): presentParams(0) {}
  ATOM_DeviceResetEvent (void *pp): presentParams (pp) {}

  void *presentParams;

  ATOM_DECLARE_EVENT(ATOM_DeviceResetEvent)
};

#endif // __ATOM_GLRENDER_EVENTS_H
/*! @} */
