#include "stdafx.h"
#include "WidgetAnimator.h"



ATOM_BEGIN_EVENT_MAP(ATOM_WidgetAnimator, ATOM_EventTrigger)
ATOM_END_EVENT_MAP

//////////////////////////////////////////////////////////////////////////

ATOM_BEGIN_EVENT_MAP(ExpendWidgetAnimator, ATOM_WidgetAnimator)
	ATOM_EVENT_HANDLER(ExpendWidgetAnimator, ATOM_WidgetAnimatorStartEvent, onStart)
	ATOM_EVENT_HANDLER(ExpendWidgetAnimator, ATOM_WidgetAnimatorUpdateEvent, onIdle)
ATOM_END_EVENT_MAP

ExpendWidgetAnimator::ExpendWidgetAnimator()
{
	_widget = NULL;
	_elapsed = 0.f;
	_duration = 0.f;
}

bool ExpendWidgetAnimator::load(ATOM_Widget* widget, ATOM_TiXmlElement* elem)
{
	int intValue;
	int expendX = 0;
	int expendY = 0;
	double doubleValue;
	if(elem->Attribute("expendxf", &doubleValue))
	{
		float expendXF = (float)doubleValue;
		float expendYF = elem->Attribute("expendyf", &doubleValue) ? (float)doubleValue : 0;
		expendX = expendXF*widget->getWidgetRect().size.w;
		expendY = expendYF*widget->getWidgetRect().size.h;
	}
	else
	{
		expendX = elem->Attribute("expendx", &intValue) ? intValue : 0;
		expendY = elem->Attribute("expendy", &intValue) ? intValue : 0;
	}
	float duration = elem->Attribute("duration", &doubleValue) ? (float)doubleValue : 0.f;

	return init(widget, expendX, expendY, duration);
};

bool ExpendWidgetAnimator::init(ATOM_Widget* widget, int expendX, int expendY, float duration)
{
	_srcRect = widget->getWidgetRect();
	_dstRect = widget->getWidgetRect();
	_dstRect.point.x -= expendX/2;
	_dstRect.point.y -= expendY/2;
	_dstRect.size.w += expendX;
	_dstRect.size.h += expendY;
	_elapsed = 0;
	_duration = duration;
	_widget = widget;

	return true;
}

void ExpendWidgetAnimator::onStart(ATOM_WidgetAnimatorStartEvent *event)
{
	_srcRect = _widget->getWidgetRect();
	_elapsed = 0;
}

void ExpendWidgetAnimator::onIdle(ATOM_WidgetAnimatorUpdateEvent *event)
{
	float fDelta = ATOM_APP->getFrameStamp().elapsedTick*0.001;
	_elapsed += fDelta;
	if(_elapsed >= _duration)
	{
		_elapsed = _duration;
		_widget->resize(_dstRect);
		event->done = true;
	}
	else
	{
		ATOM_Rect2Di rc = _srcRect;
		int w = _dstRect.size.w - _srcRect.size.w;
		w *= _elapsed/_duration;
		rc.point.x -= w/2;
		rc.size.w += w;
		int h = _dstRect.size.h - _srcRect.size.h;
		h *= _elapsed/_duration;
		rc.point.y -= h/2;
		rc.size.h += h;
		_widget->resize(rc);
		event->done = false;
	}
}


//////////////////////////////////////////////////////////////////////////

ATOM_BEGIN_EVENT_MAP(MoveWidgetAnimator, ATOM_WidgetAnimator)
ATOM_EVENT_HANDLER(MoveWidgetAnimator, ATOM_WidgetAnimatorStartEvent, onStart)
ATOM_EVENT_HANDLER(MoveWidgetAnimator, ATOM_WidgetAnimatorUpdateEvent, onIdle)
ATOM_END_EVENT_MAP

MoveWidgetAnimator::MoveWidgetAnimator()
{
	_widget = NULL;
	_elapsed = 0.f;
	_duration = 0.f;
}

bool MoveWidgetAnimator::load(ATOM_Widget* widget, ATOM_TiXmlElement* elem)
{
	int intValue;
	int offsetX = 0;
	int offsetY = 0;
	double doubleValue;
	if(elem->Attribute("offsetxf", &doubleValue))
	{
		float offsetXF = (float)doubleValue;
		float offsetYF = elem->Attribute("offsetyf", &doubleValue) ? (float)doubleValue : 0;
		offsetX = offsetXF*widget->getWidgetRect().size.w;
		offsetY = offsetYF*widget->getWidgetRect().size.h;
	}
	else
	{
		offsetX = elem->Attribute("offsetx", &intValue) ? intValue : 0;
		offsetY = elem->Attribute("offsety", &intValue) ? intValue : 0;
	}
	float duration = elem->Attribute("duration", &doubleValue) ? (float)doubleValue : 0.f;

	return init(widget, offsetX, offsetY, duration);
};

bool MoveWidgetAnimator::init(ATOM_Widget* widget, int offsetX, int offsetY, float duration)
{
	_srcRect = widget->getWidgetRect();
	_dstRect = widget->getWidgetRect();
	_dstRect.point.x += offsetX;
	_dstRect.point.y += offsetY;
	_elapsed = 0;
	_duration = duration;
	_widget = widget;

	return true;
}

void MoveWidgetAnimator::onStart(ATOM_WidgetAnimatorStartEvent *event)
{
	_srcRect = _widget->getWidgetRect();
	_elapsed = 0;
}

void MoveWidgetAnimator::onIdle(ATOM_WidgetAnimatorUpdateEvent *event)
{
	float fDelta = ATOM_APP->getFrameStamp().elapsedTick*0.001;
	_elapsed += fDelta;
	if(_elapsed >= _duration)
	{
		_elapsed = _duration;
		_widget->resize(_dstRect);
		event->done = true;
	}
	else
	{
		ATOM_Rect2Di rc = _srcRect;
		int x = _dstRect.point.x - _srcRect.point.x;
		x *= _elapsed/_duration;
		rc.point.x += x;
		int y = _dstRect.point.y - _srcRect.point.y;
		y *= _elapsed/_duration;
		rc.point.y += y;
		_widget->resize(rc);
		event->done = false;
	}
}

//////////////////////////////////////////////////////////////////////////

ATOM_BEGIN_EVENT_MAP(ListWidgetAnimator, ATOM_WidgetAnimator)
ATOM_EVENT_HANDLER(ListWidgetAnimator, ATOM_WidgetAnimatorStartEvent, onStart)
ATOM_EVENT_HANDLER(ListWidgetAnimator, ATOM_WidgetAnimatorUpdateEvent, onIdle)
ATOM_END_EVENT_MAP

ListWidgetAnimator::ListWidgetAnimator()
{

}

ListWidgetAnimator::~ListWidgetAnimator()
{
	fini();
}

bool ListWidgetAnimator::load(ATOM_Widget* widget, ATOM_TiXmlElement* elem)
{
	fini();

	if(NULL == elem)
	{
		return false;
	}

	ATOM_WidgetAnimator* animator = NULL;
	ATOM_TiXmlElement* subElem = elem->FirstChildElement();
	for(; subElem; subElem = subElem->NextSiblingElement())
	{
		animator = WidgetAnimatorFactory::create(widget, subElem);
		if(animator)
		{
			add(animator);
		}
	}

	_currentAnimator = -1;

	return true;
}

void ListWidgetAnimator::fini()
{
	for(size_t i=0; i<_animatorList.size(); ++i)
	{
		WidgetAnimatorFactory::destroy(_animatorList[i]);
	}
	_animatorList.clear();
}

void ListWidgetAnimator::onStart(ATOM_WidgetAnimatorStartEvent *event)
{
	if(!_animatorList.empty())
	{
		_currentAnimator = 0;
		ATOM_WidgetAnimatorStartEvent startEvent;
		_animatorList[0]->handleEvent(&startEvent);
	}
}

void ListWidgetAnimator::onIdle(ATOM_WidgetAnimatorUpdateEvent *event)
{
	if(_currentAnimator < 0 || _currentAnimator >= _animatorList.size())
	{
		event->done = true;
		return;
	}

	// 更新
	ATOM_WidgetAnimatorUpdateEvent updateEvent;
	_animatorList[_currentAnimator]->handleEvent(&updateEvent);

	// 如果当前的已经处理完，则处理下一个
	if( updateEvent.done )
	{
		// 停止当前
		ATOM_WidgetAnimatorStopEvent stopEvent;
		_animatorList[_currentAnimator]->handleEvent(&stopEvent);

		// 开始下一个
		++_currentAnimator;
		if(_currentAnimator < _animatorList.size())
		{
			ATOM_WidgetAnimatorStartEvent startEvent;
			_animatorList[_currentAnimator]->handleEvent(&startEvent);
		}
	}
}

void ListWidgetAnimator::add(ATOM_WidgetAnimator* animator)
{
	_animatorList.push_back(animator);
}


//////////////////////////////////////////////////////////////////////////

ATOM_WidgetAnimator* WidgetAnimatorFactory::create(ATOM_Widget* widget, ATOM_TiXmlElement* elem)
{
	int type = WANIMATORTYPE_NULL;
	if( elem->Attribute("type", &type) )
	{
		ATOM_WidgetAnimator* animator = NULL;
		switch (type)
		{
		case WANIMATORTYPE_EXPEND:
			animator = ATOM_NEW(ExpendWidgetAnimator);
			break;

		case WANIMATORTYPE_LIST:
			animator = ATOM_NEW(ListWidgetAnimator);
			break;

		case WANIMATORTYPE_MOVE:
			animator = ATOM_NEW(MoveWidgetAnimator);
			break;
		}

		if(false == animator->load(widget, elem) )
		{
			ATOM_DELETE(animator);
			animator = NULL;
		}
		return animator;
	}

	return NULL;
}

void WidgetAnimatorFactory::destroy(ATOM_WidgetAnimator* animator)
{
	if(animator)
	{
		ATOM_DELETE(animator);
		animator = NULL;
	}
}



