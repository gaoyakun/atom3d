#include "StdAfx.h"
#include "widgetlayout.h"
#include "basiclayout.h"
#include "horizontallayout.h"
#include "verticallayout.h"

ATOM_WidgetLayout::ATOM_WidgetLayout (void)
{
	_innerSpaceLeft = 0;
	_innerSpaceTop = 0;
	_innerSpaceRight = 0;
	_innerSpaceBottom = 0;
	_gap = 4;
	_rect.point.x = 0;
	_rect.point.y = 0;
	_rect.size.w = 0;
	_rect.size.h = 0;
	_layoutDirty = false;
}

ATOM_WidgetLayout::~ATOM_WidgetLayout (void)
{
	for (unsigned i = 0; i < _units.size(); ++i)
	{
		ATOM_DELETE (_units[i]);
	}
	_units.clear ();
}

ATOM_WidgetLayout::Unit *ATOM_WidgetLayout::allocUnit (void)
{
	Unit *unit = ATOM_NEW(Unit);
	memset (unit, 0, sizeof(Unit));
	_units.push_back (unit);
	invalidateLayout ();

	return unit;
}

void ATOM_WidgetLayout::freeUnit (Unit *unit)
{
	for (unsigned i = 0; i < _units.size(); ++i)
	{
		if (_units[i] == unit)
		{
			ATOM_DELETE(_units[i]);
			_units.erase (_units.begin() + i);
			invalidateLayout ();
			return;
		}
	}
}

unsigned ATOM_WidgetLayout::getNumUnits (void) const
{
	return _units.size();
}

ATOM_WidgetLayout::Unit *ATOM_WidgetLayout::getUnit (unsigned index) const
{
	return _units[index];
}

void ATOM_WidgetLayout::invalidateLayout (void)
{
	_layoutDirty = true;
}

void ATOM_WidgetLayout::calcLayout (void)
{
	if (_layoutDirty)
	{
		_layoutDirty = false;

		if (_units.size() > 0)
		{
			onCalcLayout ();
		}
	}
}

void ATOM_WidgetLayout::setGap (int gap)
{
	if (_gap != gap)
	{
		_gap = gap;
		invalidateLayout ();
	}
}

int ATOM_WidgetLayout::getGap (void) const
{
	return _gap;
}


void ATOM_WidgetLayout::setRect (const ATOM_Rect2Di &rect)
{
	if (_rect != rect)
	{
		_rect = rect;
		invalidateLayout ();
	}
}

const ATOM_Rect2Di &ATOM_WidgetLayout::getRect (void) const
{
	return _rect;
}

void ATOM_WidgetLayout::setInnerSpaceLeft (int val)
{
	if (_innerSpaceLeft != val)
	{
		_innerSpaceLeft = val;
		invalidateLayout ();
	}
}

int ATOM_WidgetLayout::getInnerSpaceLeft (void) const
{
	return _innerSpaceLeft;
}

void ATOM_WidgetLayout::setInnerSpaceTop (int val)
{
	if (_innerSpaceTop != val)
	{
		_innerSpaceTop = val;
		invalidateLayout ();
	}
}

int ATOM_WidgetLayout::getInnerSpaceTop (void) const
{
	return _innerSpaceTop;
}

void ATOM_WidgetLayout::setInnerSpaceRight (int val)
{
	if (_innerSpaceRight != val)
	{
		_innerSpaceRight = val;
		invalidateLayout ();
	}
}

int ATOM_WidgetLayout::getInnerSpaceRight (void) const
{
	return _innerSpaceRight;
}

void ATOM_WidgetLayout::setInnerSpaceBottom (int val)
{
	if (_innerSpaceBottom != val)
	{
		_innerSpaceBottom = val;
		invalidateLayout ();
	}
}

int ATOM_WidgetLayout::getInnerSpaceBottom (void) const
{
	return _innerSpaceBottom;
}

ATOM_WidgetLayout *ATOM_WidgetLayout::createLayout (Type type)
{
	switch (type)
	{
	case ATOM_WidgetLayout::Basic:
		return ATOM_NEW(ATOM_BasicLayout);

	case ATOM_WidgetLayout::Horizontal:
		return ATOM_NEW(ATOM_HorizontalLayout);

	case ATOM_WidgetLayout::Vertical:
		return ATOM_NEW(ATOM_VerticalLayout);

	default:
		return 0;
	}
}

void ATOM_WidgetLayout::deleteLayout (ATOM_WidgetLayout *layout)
{
	ATOM_DELETE(layout);
}

bool ATOM_WidgetLayout::isDirty (void) const
{
	return _layoutDirty;
}

bool ATOM_WidgetLayout::moveUnitUp (Unit *unit)
{
	for (int i = 0; i < _units.size(); ++i)
	{
		if (_units[i] == unit)
		{
			int newPos = i - 1;
			while (newPos >= 0)
			{
				if (_units[newPos]->widget->isLayoutable())
				{
					break;
				}
				newPos--;
			}

			if (newPos < 0)
			{
				return false;
			}

			_units.erase (_units.begin() + i);
			_units.insert (_units.begin() + newPos, unit);
			invalidateLayout ();
			return true;
		}
	}

	return false;
}

bool ATOM_WidgetLayout::moveUnitDown (Unit *unit)
{
	for (int i = 0; i < _units.size(); ++i)
	{
		if (_units[i] == unit)
		{
			int newPos = i + 1;
			while (newPos < _units.size())
			{
				if (_units[newPos]->widget->isLayoutable())
				{
					break;
				}
				newPos++;
			}

			if (newPos == _units.size())
			{
				return false;
			}

			_units.erase (_units.begin() + i);
			_units.insert (_units.begin() + newPos, unit);
			invalidateLayout ();
			return true;
		}
	}

	return false;
}

ATOM_Rect2Di ATOM_WidgetLayout::computeRect (void) const
{
	int b = false;
	int x_min = INT_MAX;
	int y_min = INT_MAX;
	int x_max = -INT_MAX;
	int y_max = -INT_MAX;

	if (_units.size() > 0)
	{
		for (unsigned i = 0; i < _units.size(); ++i)
		{
			if (_units[i]->widget && _units[i]->widget->getShowState() != ATOM_Widget::Hide)
			{
				b = true;

				int ax = _units[i]->actual_x;
				int ay = _units[i]->actual_y;
				int aw = _units[i]->actual_w;
				int ah = _units[i]->actual_h;

				if (ax < x_min) x_min = ax;
				if (ax+aw > x_max) x_max = ax+aw;
				if (ay < y_min) y_min = ay;
				if (ay+ah > y_max) y_max = ay+ah;
			}
		}
	}

	return b ? ATOM_Rect2Di(x_min-_innerSpaceLeft, y_min-_innerSpaceTop, x_max-x_min+_innerSpaceLeft+_innerSpaceRight, y_max-y_min+_innerSpaceTop+_innerSpaceBottom) : ATOM_Rect2Di(0,0,0,0);
}


