#ifndef __ATOM3D_ENGINE_GUI_RENDERER_H
#define __ATOM3D_ENGINE_GUI_RENDERER_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_script.h"
#include "../ATOM_audio.h"

#include "basedefs.h"
#include "gui_misc.h"
#include "dragdrop.h"

class ATOM_Desktop;
class ATOM_Dialog;
class ATOM_GUIImageList;
class ScriptDialogEventTrigger;

class ATOM_ENGINE_API ATOM_GUIRenderer
{
  friend class ATOM_Widget;
  friend class ATOM_Dialog;

public:
  ATOM_GUIRenderer (bool asMain=true);
  virtual ~ATOM_GUIRenderer (void);

protected:
  virtual void doRender (void);

public:
  void render (void);
  void setViewport (const ATOM_Rect2Di &rect);
  const ATOM_Rect2Di & getViewport (void) const;
  void setAudioDevice(ATOM_AudioDeviceWrapper *device);
  ATOM_AudioDeviceWrapper *getAudioDevice();
  ATOM_Desktop *getDesktop (void);
  void setFocus (ATOM_Widget *widget);
  ATOM_Widget *getFocus (void) const;
  void setCapture (ATOM_Widget *widget);
  ATOM_Widget *getCapture (void) const;
  ATOM_Widget *getHover (void) const;
  ATOM_Point2Di getMousePosition (void) const;
  bool isLeftButtonDown (void) const;
  bool isMiddleButtonDown (void) const;
  bool isRightButtonDown (void) const;
  const ATOM_Rect2Di *getScissor (void) const;
  bool pushScissor (const ATOM_Rect2Di &region, bool keepLast);
  void popScissor (void);
  bool belongsTo (ATOM_Widget *child, ATOM_Widget *parent) const;
  void invalidate (void);
  void setRenderTargetWindow (ATOM_TopWindow *window);
  ATOM_TopWindow *getRenderTargetWindow (void) const;
  void set3DMode (bool b);
  bool is3DMode (void) const;
  void setZValue (float z);
  float getZValue (void) const;
  void ignoreFog (bool b);
  bool isIgnoreFog (void) const;
  ATOM_TopWindow *getCurrentModalDialog (void);
  void showScriptDialog (const char *title, const ATOM_Rect2Di &rect);
  ATOM_Script *getScript (void) const;
  void closeScriptDialog (void);
  void setAsMainGUI (bool b);
  bool isModalLooping (void);
  ATOM_Widget *hitTest (int x, int y, ATOM_GUIHitTestResult *result, ATOM_Point2Di *position, ATOM_Point2Di *clientPosition, bool fullTest = false) const;
  void startDragging (ATOM_Widget *widget, int x, int y);
  void stopDragging (void);
  ATOM_Widget *getDraggingWidget (void) const;
  void doDrag (ATOM_DragSource *dragSource, ATOM_Widget *sourceWidget);
  void cancelDrag (void);
  void drawDark (ATOM_ColorARGB color);

public:
  void onMouseMove (ATOM_MouseMoveEvent *event);
  void onMouseWheel (ATOM_MouseWheelEvent *event);
  void onMouseButtonDown (ATOM_MouseButtonDownEvent *event);
  void onMouseButtonUp (ATOM_MouseButtonUpEvent *event);
  void onMouseDblClick (ATOM_MouseDblClickEvent *event);
  void onChar (ATOM_CharEvent *event);
  void onKeyDown (ATOM_KeyDownEvent *event);
  void onKeyUp (ATOM_KeyUpEvent *event);
  void onIdle (ATOM_AppIdleEvent *event);
  void onFilesDropped (ATOM_WindowFilesDroppedEvent *event);
  void onWinMessage (ATOM_AppWMEvent *event);

private:
  void widgetStateUpdateR (ATOM_Widget *widget);
  void widgetFrameUpdateR (ATOM_Widget *widget, ATOM_AppIdleEvent *event);
  void setModalDialog (ATOM_TopWindow *dialog);
  void removeModalDialog (ATOM_TopWindow *dialog);
  void notifyWidgetDeleted (ATOM_Widget *widget);
  void drawDragIndicator (void);

public:
	static void enableMultiThreading (bool enable);
	static bool isMultiThreadingEnabled (void);

protected:
  ATOM_Rect2Di _viewport;
  ATOM_Desktop *_desktop;
  ATOM_Widget *_hoverWidget;
  ATOM_Widget *_draggingWidget;
  ATOM_Widget *_focusWidget;
  ATOM_Widget *_captureWidget;
  ATOM_Widget *_dropTarget;
  ATOM_TopWindow *_scriptDialog;
  ATOM_Script *_script;
  ATOM_ScriptManager *_scriptManager;
  ATOM_AUTOPTR(ATOM_GUIImageList) _scriptDialogImageList;
  ScriptDialogEventTrigger *_scriptDialogTrigger;
  ATOM_AudioDeviceWrapper *_audioDevice;
  
  int _draggingX;
  int _draggingY;
  int _mouseX;
  int _mouseY;
  bool _leftDown;
  bool _middleDown;
  bool _rightDown;
  bool _displayDirty;
  bool _3dMode;
  bool _nofog;
  bool _isMainGUI;
  bool _cancelUpdate;
  float _zValue;
  bool _manualChange;
  ATOM_Widget *_preDragWidget;
  ATOM_Point2Di _preDragPosition;
  HKL _keyboardLayout;
  ATOM_TopWindow *_renderTargetWindow;
  ATOM_VECTOR<ATOM_TopWindow*> _modalDialogsStack;

  ATOM_AUTOPTR(ATOM_DragSource) _dragSource;
  static ATOM_VECTOR<ATOM_Rect2Di> _scissorStack;
  static bool _mtEnabled;

  ATOM_DECLARE_EVENT_MAP_NOPARENT(ATOM_GUIRenderer)
};

#endif // __ATOM3D_ENGINE_GUI_RENDERER_H
