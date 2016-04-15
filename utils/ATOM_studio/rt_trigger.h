#ifndef __ATOM3D_STUDIO_RT_TRIGGER_H
#define __ATOM3D_STUDIO_RT_TRIGGER_H

#if _MSC_VER > 1000
# pragma once
#endif

class EditorImpl;
class AS_CameraModal;

class RealtimeEventTrigger: public ATOM_EventTrigger
{
public:
	RealtimeEventTrigger (EditorImpl *editor);
	virtual ~RealtimeEventTrigger (void);

public:
	void onCommand (ATOM_WidgetCommandEvent *event);
	void onResize (ATOM_WidgetResizeEvent *event);
	void onPaint (ATOM_WidgetDrawClientEvent *event);
	void onMenuPopup (ATOM_MenuPopupEvent *event);
	void onUnhandled (ATOM_Event *event);

private:
	EditorImpl *_editorImpl;

	ATOM_DECLARE_EVENT_MAP(RealtimeEventTrigger, ATOM_EventTrigger)
};

#endif // __ATOM3D_STUDIO_RT_TRIGGER_H
