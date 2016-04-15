#ifndef __ATOM3D_STUDIO_GUIEDITOR_EDITORFORM_H
#define __ATOM3D_STUDIO_GUIEDITOR_EDITORFORM_H

#include <ATOM_engine.h>

class ControlProxy;
class PluginGUI;

#define CTLPOINT_LEFT_TOP		0
#define CTLPOINT_MIDDLE_TOP		1
#define CTLPOINT_RIGHT_TOP		2
#define CTLPOINT_MIDDLE_LEFT	3
#define CTLPOINT_MIDDLE_RIGHT	4
#define CTLPOINT_LEFT_BOTTOM	5
#define CTLPOINT_MIDDLE_BOTTOM	6
#define CTLPOINT_RIGHT_BOTTOM	7

class EditorForm: public ATOM_Widget
{
public:
	EditorForm (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_Widget::ShowState showState = ATOM_Widget::Hide);
	virtual ~EditorForm (void);

public:
	virtual ATOM_WidgetType getType (void) const;
	virtual void hitTest (int x, int y, ATOM_GUIHitTestResult *result, bool fullTest);
	virtual void draw (void);

public:
	void beginCreateWidget (int widgetType, int x, int y);
	void endCreateWidget (void);
	void setWidgetPropertyBar (ATOMX_TweakBar *bar);
	void setCustomPropertyBar (ATOMX_TweakBar *bar);
	void setPlugin (PluginGUI *plugin);
	void setActiveProxy (ControlProxy *proxy);
	ControlProxy *getActiveProxy (void) const;
	bool isPreviewMode (void) const;
	void setPreviewMode (bool preview);
	void deleteActiveProxy (void);
	void cloneActiveProxy (void);

public:
	void onLButtonDown (ATOM_WidgetLButtonDownEvent *event);
	void onLButtonUp (ATOM_WidgetLButtonUpEvent *event);
	void onRButtonUp (ATOM_WidgetRButtonUpEvent *event);
	void onMouseMove (ATOM_WidgetMouseMoveEvent *event);
	void onKeyDown (ATOM_WidgetKeyDownEvent *event);
	void onKeyUp (ATOM_WidgetKeyUpEvent *event);
	void onResize (ATOM_WidgetResizeEvent *event);
	void onIdle (ATOM_WidgetIdleEvent *event);
	void onPaint (ATOM_WidgetDrawClientEvent *event);
	void onTWCommandEvent (ATOMX_TWCommandEvent *event);
	void onTWValueChangedEvent (ATOMX_TWValueChangedEvent *event);

private:
	void drawController (void);
	void drawIndicator (void);
	void drawParentProxy (void);
	ATOM_Widget *getHoverWidget (void);
	void calcControllerPoints (void);

private:
	ControlProxy *_mainProxy;
	ControlProxy *_promptParentProxy;
	ATOMX_TweakBar *_propBar;
	ATOMX_TweakBar *_customPropBar;

	bool _preview;
	bool _dragging;
	int _lastDragMouseX;
	int _lastDragMouseY;

	bool _enablePick;
	bool _promptCreate;
	int _promptCreateWidgetType;
	int _promptMouseX;
	int _promptMouseY;

	int _draggingCtlPoint;
	ATOM_Point2Di _lastCtlPoint;
	ATOM_Point2Di _currentCtlPoint;
	ATOM_Rect2Di _ctlPoints[8];

	ATOM_Point2Di _movingVel;

	PluginGUI *_plugin;

	ATOM_DECLARE_EVENT_MAP(EditorForm, ATOM_Widget)
};

#endif // __ATOM3D_STUDIO_GUIEDITOR_EDITORFORM_H
