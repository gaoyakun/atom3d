#include "StdAfx.h"
#include "verticallayout.h"

ATOM_VerticalLayout::ATOM_VerticalLayout (void)
{
}

void ATOM_VerticalLayout::onCalcLayout (void)
{
	int total_left = _innerSpaceLeft;
	int total_top = _innerSpaceTop;
	int total_width = ATOM_max2 (0, _rect.size.w - _innerSpaceLeft - _innerSpaceRight);
	int total_height = ATOM_max2 (0, _rect.size.h - _innerSpaceTop - _innerSpaceBottom);

	int fixed_height_acc = 0;
	int total_percent = 0;
	int numUnits = 0;

	for (unsigned i = 0; i < _units.size(); ++i)
	{
		if (!_units[i]->widget || !_units[i]->widget->isLayoutable() || _units[i]->widget->getShowState() == ATOM_Widget::Hide)
		{
			continue;
		}

		++numUnits;

		if (_units[i]->desired_h >= 0)
		{
			_units[i]->actual_h = _units[i]->desired_h;
			fixed_height_acc += _units[i]->desired_h;
		}
		else
		{
			total_percent -= _units[i]->desired_h;
		}

		_units[i]->actual_x = _rect.point.x + _innerSpaceLeft;
		_units[i]->actual_w = _units[i]->desired_w < 0 ? (_units[i]->desired_w * -0.01f * total_width) : _units[i]->desired_w;
	}

	total_height -= (numUnits - 1) * _gap;
	total_percent = ATOM_max2(total_percent, 100);

	int var_height_acc = ATOM_max2 (0, total_height - fixed_height_acc);
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
			_units[i]->actual_y = _rect.point.y + _innerSpaceTop;
			first = false;
		}
		else
		{
			_units[i]->actual_y = _units[last]->actual_y + _units[last]->actual_h + _gap;
		}
		last = i;

		if (_units[i]->desired_h < 0)
		{
			float percent = -float(_units[i]->desired_h) / total_percent;
			_units[i]->actual_h = var_height_acc * percent;
		}
	}
}

ATOM_WidgetLayout::Type ATOM_VerticalLayout::getType (void) const
{
	return ATOM_WidgetLayout::Vertical;
}


