#include "StdAfx.h"
#include "dragdrop.h"

void ATOM_DragSource::setWidget (ATOM_Widget *widget)
{
	_widget = widget;
}

ATOM_Widget *ATOM_DragSource::getWidget (void) const
{
	return _widget;
}

void ATOM_DragSource::addProperty (const char *name, const ATOM_Variant &value)
{
	_props[name] = value;
}

void ATOM_DragSource::removeProperty (const char *name)
{
	ATOM_HASHMAP<ATOM_STRING, ATOM_Variant>::iterator it = _props.find (name);
	if (it != _props.end ())
	{
		_props.erase (it);
	}
}

const ATOM_Variant &ATOM_DragSource::getProperty (const char *name) const
{
	static const ATOM_Variant empty;
	ATOM_HASHMAP<ATOM_STRING, ATOM_Variant>::const_iterator it = _props.find (name);
	return it != _props.end () ? it->second : empty;
}

void ATOM_DragSource::removeAllProperties (void)
{
	_props.clear ();
}

void ATOM_DragSource::setIndicatorImageId (int id)
{
	_indicators.imageId = id;
}

int ATOM_DragSource::getIndicatorImageId (void) const
{
	return _indicators.imageId;
}

void ATOM_DragSource::setIndicatorRect (const ATOM_Rect2Di &rect)
{
	_indicators.rect = rect;
}

const ATOM_Rect2Di &ATOM_DragSource::getIndicatorRect (void) const
{
	return _indicators.rect;
}

void ATOM_DragSource::setDraggingPosition (const ATOM_Point2Di &pt)
{
	_dragPosition = pt;
}

const ATOM_Point2Di &ATOM_DragSource::getDraggingPosition (void) const
{
	return _dragPosition;
}

