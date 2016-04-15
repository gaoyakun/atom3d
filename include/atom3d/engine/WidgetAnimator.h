#ifndef __ATOM3D_ENGINE_WIDGETANIMATOR_H
#define __ATOM3D_ENGINE_WIDGETANIMATOR_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_dbghlp.h"
#include "gui_events.h"



enum
{
	WANIMATORTYPE_NULL = 0,
	WANIMATORTYPE_EXPEND,
	WANIMATORTYPE_LIST,
	WANIMATORTYPE_MOVE,
};

//////////////////////////////////////////////////////////////////////////

class ATOM_ENGINE_API ATOM_WidgetAnimator : public ATOM_EventTrigger
{
public:
	inline virtual ~ATOM_WidgetAnimator(){}

	virtual bool load(ATOM_Widget* widget, ATOM_TiXmlElement* elem) = 0;

	void onUnhandled (ATOM_Event *event);

	ATOM_DECLARE_EVENT_MAP(ATOM_WidgetAnimator, ATOM_EventTrigger)
};

//////////////////////////////////////////////////////////////////////////

class ATOM_ENGINE_API ExpendWidgetAnimator : public ATOM_WidgetAnimator
{
public:
	ExpendWidgetAnimator();

	bool init(ATOM_Widget* widget, int expendX, int expendY, float duration);
	virtual bool load(ATOM_Widget* widget, ATOM_TiXmlElement* elem);

	void onStart (ATOM_WidgetAnimatorStartEvent *event);
	void onIdle(ATOM_WidgetAnimatorUpdateEvent *event);

	ATOM_DECLARE_EVENT_MAP(ExpendWidgetAnimator, ATOM_WidgetAnimator)
public:
	float _elapsed;
	ATOM_Widget* _widget;
	ATOM_Rect2Di _srcRect;
	ATOM_Rect2Di _dstRect;
	float _duration;
};

//////////////////////////////////////////////////////////////////////////

class ATOM_ENGINE_API MoveWidgetAnimator : public ATOM_WidgetAnimator
{
public:
	MoveWidgetAnimator();

	bool init(ATOM_Widget* widget, int offsetX, int offsetY, float duration);
	virtual bool load(ATOM_Widget* widget, ATOM_TiXmlElement* elem);

	void onStart (ATOM_WidgetAnimatorStartEvent *event);
	void onIdle(ATOM_WidgetAnimatorUpdateEvent *event);

	ATOM_DECLARE_EVENT_MAP(MoveWidgetAnimator, ATOM_WidgetAnimator)
public:
	float _elapsed;
	ATOM_Widget* _widget;
	ATOM_Rect2Di _srcRect;
	ATOM_Rect2Di _dstRect;
	float _duration;
};

//////////////////////////////////////////////////////////////////////////

class ATOM_ENGINE_API ListWidgetAnimator : public ATOM_WidgetAnimator
{
public:
	ListWidgetAnimator();
	virtual ~ListWidgetAnimator();

	virtual bool load(ATOM_Widget* widget, ATOM_TiXmlElement* elem);
	void fini();
	void add(ATOM_WidgetAnimator* animator);
	
	void onStart (ATOM_WidgetAnimatorStartEvent *event);
	void onIdle(ATOM_WidgetAnimatorUpdateEvent *event);

	ATOM_DECLARE_EVENT_MAP(ListWidgetAnimator, ATOM_WidgetAnimator)
protected:
	ATOM_VECTOR<ATOM_WidgetAnimator*> _animatorList;
	int _currentAnimator;
};

//////////////////////////////////////////////////////////////////////////

class ATOM_ENGINE_API WidgetAnimatorFactory
{
public:
	static ATOM_WidgetAnimator* create(ATOM_Widget* widget, ATOM_TiXmlElement* elem);
	static void destroy(ATOM_WidgetAnimator* animator);
};




#endif // __ATOM3D_ENGINE_WIDGETANIMATOR_H
