#include "StdAfx.h"
#include "basiclayout.h"

void ATOM_BasicLayout::onCalcLayout (void)
{
	int total_width = ATOM_max2 (0, _rect.size.w - _innerSpaceLeft - _innerSpaceRight);
	int total_height = ATOM_max2 (0, _rect.size.h - _innerSpaceTop - _innerSpaceBottom);

	for (unsigned i = 0; i < _units.size(); ++i)
	{
		_units[i]->actual_x = _rect.point.x + _innerSpaceLeft + _units[i]->desired_x;
		_units[i]->actual_y = _rect.point.y + _innerSpaceTop + _units[i]->desired_y;
		_units[i]->actual_w = _units[i]->desired_w > 0 ? _units[i]->desired_w : _units[i]->desired_w * -0.01f * total_width;
		_units[i]->actual_h = _units[i]->desired_h > 0 ? _units[i]->desired_h : _units[i]->desired_h * -0.01f * total_height;
	}
}

ATOM_WidgetLayout::Type ATOM_BasicLayout::getType (void) const
{
	return ATOM_WidgetLayout::Basic;
}

