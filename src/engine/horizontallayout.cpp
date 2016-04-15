#include "StdAfx.h"
#include "horizontallayout.h"

ATOM_HorizontalLayout::ATOM_HorizontalLayout (void)
{
}

void ATOM_HorizontalLayout::onCalcLayout (void)
{
	int total_left = _innerSpaceLeft;
	int total_top = _innerSpaceTop;
	int total_width = ATOM_max2 (0, _rect.size.w - _innerSpaceLeft - _innerSpaceRight);
	int total_height = ATOM_max2 (0, _rect.size.h - _innerSpaceTop - _innerSpaceBottom);

	int fixed_width_acc = 0;
	int total_percent = 0;
	int numUnits = 0;

	for (unsigned i = 0; i < _units.size(); ++i)
	{
		if (!_units[i]->widget || !_units[i]->widget->isLayoutable() || _units[i]->widget->getShowState() == ATOM_Widget::Hide)
		{
			continue;
		}

		numUnits++;

		if (_units[i]->desired_w >= 0)
		{
			_units[i]->actual_w = _units[i]->desired_w;
			fixed_width_acc += _units[i]->desired_w;
		}
		else
		{
			total_percent -= _units[i]->desired_w;
		}

		_units[i]->actual_y = _rect.point.y + _innerSpaceTop;
		_units[i]->actual_h = _units[i]->desired_h < 0 ? (_units[i]->desired_h * -0.01f * total_height) : _units[i]->desired_h;
	}

	total_width -= (numUnits - 1) * _gap;
	int var_width_acc = ATOM_max2 (0, total_width - fixed_width_acc);
	bool first = true;
	int last = 0;
	for (unsigned i = 0; i < _units.size(); ++i)
	{
		if (!_units[i]->widget || !_units[i]->widget->isLayoutable() || _units[i]->widget->getShowState() == ATOM_Widget::Hide)
		{
			continue;
		}

		if (first)
		{
			_units[i]->actual_x = _rect.point.x + _innerSpaceLeft;
			first = false;
		}
		else
		{
			_units[i]->actual_x = _units[last]->actual_x + _units[last]->actual_w + _gap;
		}
		last = i;

		if (_units[i]->desired_w < 0)
		{
			float percent = -float(_units[i]->desired_w) / total_percent;
			_units[i]->actual_w = var_width_acc * percent;
		}
	}
}

ATOM_WidgetLayout::Type ATOM_HorizontalLayout::getType (void) const
{
	return ATOM_WidgetLayout::Horizontal;
}


