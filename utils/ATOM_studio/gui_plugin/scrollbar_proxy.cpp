#include "StdAfx.h"
#include "scrollbar_proxy.h"

ScrollBarParameters::ScrollBarParameters (ControlProxy *proxy): WidgetParameters(proxy)
{
	imageId() = ATOM_AUTOMATIC_IMAGEID;
	_SliderImageId = ATOM_AUTOMATIC_IMAGEID;
	_UpButtonImageId = ATOM_AUTOMATIC_IMAGEID;
	_DownButtonImageId = ATOM_AUTOMATIC_IMAGEID;
	_placement = WP_VERTICAL;
	_buttonWidth = 10;
	_handleWidth = 6;
	_minValue = 0;
	_maxValue = 100;
	_stepValue = 1;
}

void ScrollBarParameters::resetParameters (void)
{
	WidgetParameters::resetParameters ();
	rect().size.w = 16;
	rect().size.h = 80;

	imageId() = ATOM_AUTOMATIC_IMAGEID;
	_SliderImageId = ATOM_AUTOMATIC_IMAGEID;
	_UpButtonImageId = ATOM_AUTOMATIC_IMAGEID;
	_DownButtonImageId = ATOM_AUTOMATIC_IMAGEID;
	_placement = WP_VERTICAL;
	_buttonWidth = 10;
	_handleWidth = 6;
	_minValue = 0;
	_maxValue = 100;
	_stepValue = 1;
}

WidgetParameters *ScrollBarParameters::clone (void) const
{
	return ATOM_NEW(ScrollBarParameters, *this);
}

void ScrollBarParameters::setupTweakBar (ATOMX_TweakBar *propBar)
{
	WidgetParameters::setupTweakBar (propBar);

	propBar->addIntVar ("手柄图片", PARAMID_SCROLLBAR_SLIDER_IMAGE, _SliderImageId, false, "滚动条", 0);
	propBar->addIntVar ("上按钮图片", PARAMID_SCROLLBAR_UPBUTTON_IMAGE, _UpButtonImageId, false, "滚动条", 0);
	propBar->addIntVar ("下按钮图片", PARAMID_SCROLLBAR_DOWNBUTTON_IMAGE, _DownButtonImageId, false, "滚动条", 0);

	ATOMX_TBEnum placementEnum;
	placementEnum.addEnum ("横向", WP_HORIZONTAL);
	placementEnum.addEnum ("纵向", WP_VERTICAL);
	propBar->addEnum ("位置", PARAMID_SCROLLBAR_PLACEMENT, _placement, placementEnum, false, "滚动条", 0);

	propBar->addUIntVar ("按钮宽度", PARAMID_SCROLLBAR_BUTTON_WIDTH, _buttonWidth, false,"滚动条", 0);
	propBar->addUIntVar ("手柄宽度", PARAMID_SCROLLBAR_HANDLE_WIDTH, _handleWidth, false, "滚动条", 0);
	propBar->addIntVar ("最小值", PARAMID_SCROLLBAR_MIN_VALUE, _minValue, false, "滚动条", 0);
	propBar->addIntVar ("最大值", PARAMID_SCROLLBAR_MAX_VALUE, _maxValue, false, "滚动条", 0);
	propBar->addIntVar ("步进值", PARAMID_SCROLLBAR_STEP_VALUE, _stepValue, false, "滚动条", 0);
}

void ScrollBarParameters::apply (ATOM_Widget *widget)
{
	WidgetParameters::apply (widget);

	ATOM_ScrollBar *sb = (ATOM_ScrollBar*)widget;
	sb->setPlacement ((ATOM_WidgetPlacement)_placement);
	if (_placement == WP_VERTICAL)
	{
		sb->setVerticalImageId (imageId(), _SliderImageId, _UpButtonImageId, _DownButtonImageId);
	}
	else
	{
		sb->setHorizontalImageId (imageId(), _SliderImageId, _UpButtonImageId, _DownButtonImageId);
	}
	sb->setButtonWidth (_buttonWidth);
	sb->setHandleWidth (_handleWidth);
	sb->setRange (_minValue, _maxValue);
	sb->setStep (_stepValue);
}

void ScrollBarParameters::saveToXML (ATOM_TiXmlElement *xml)
{
	WidgetParameters::saveToXML (xml);

	xml->SetAttribute ("placement", _placement);
	xml->SetAttribute ("handleimage", _SliderImageId);
	xml->SetAttribute ("ubimage", _UpButtonImageId);
	xml->SetAttribute ("dbimage", _DownButtonImageId);
	xml->SetAttribute ("buttonw", _buttonWidth);
	xml->SetAttribute ("handlew", _handleWidth);
	xml->SetAttribute ("min", _minValue);
	xml->SetAttribute ("max", _maxValue);
	xml->SetAttribute ("step", _stepValue);
}

void ScrollBarParameters::loadFromXML (ATOM_TiXmlElement *xml)
{
	WidgetParameters::loadFromXML (xml);

	if (xml)
	{
		xml->QueryIntAttribute ("placement", &_placement);
		xml->QueryIntAttribute ("handleimage", &_SliderImageId);
		xml->QueryIntAttribute ("ubimage", &_UpButtonImageId);
		xml->QueryIntAttribute ("dbimage", &_DownButtonImageId);
		xml->QueryIntAttribute ("buttonw", &_buttonWidth);
		xml->QueryIntAttribute ("handlew", &_handleWidth);
		xml->QueryIntAttribute ("min", &_minValue);
		xml->QueryIntAttribute ("max", &_maxValue);
		xml->QueryIntAttribute ("step", &_stepValue);
	}
}

bool ScrollBarParameters::handleBarValueChangedEvent (ATOMX_TWValueChangedEvent *event)
{
	switch (event->id)
	{
	case PARAMID_SCROLLBAR_SLIDER_IMAGE:
		_SliderImageId = event->newValue.getI();
		return true;
	case PARAMID_SCROLLBAR_UPBUTTON_IMAGE:
		_UpButtonImageId = event->newValue.getI();
		return true;
	case PARAMID_SCROLLBAR_DOWNBUTTON_IMAGE:
		_DownButtonImageId = event->newValue.getI();
		return true;
	case PARAMID_SCROLLBAR_PLACEMENT:
		{
			_placement = event->newValue.getI();
			int oldPlacement = event->oldValue.getI();
			if (_placement != oldPlacement)
			{
				ATOM_Rect2Di rc = getProxy()->getWidget()->getLayoutRect();
				ATOM_Rect2Di newRc(rc.point.x+rc.size.w/2-rc.size.h/2, rc.point.y+rc.size.h/2-rc.size.w/2, rc.size.h, rc.size.w);
				getProxy()->resize (newRc);
			}
			return true;
		}
	case PARAMID_SCROLLBAR_BUTTON_WIDTH:
		_buttonWidth = event->newValue.getI();
		return true;
	case PARAMID_SCROLLBAR_HANDLE_WIDTH:
		_handleWidth = event->newValue.getI();
		return true;
	case PARAMID_SCROLLBAR_MIN_VALUE:
		_minValue = event->newValue.getI();
		return true;
	case PARAMID_SCROLLBAR_MAX_VALUE:
		_maxValue = event->newValue.getI();
		return true;
	case PARAMID_SCROLLBAR_STEP_VALUE:
		_stepValue = event->newValue.getI();
		return true;
	default:
		return WidgetParameters::handleBarValueChangedEvent (event);
	}
}

bool ScrollBarParameters::handleBarCommandEvent (ATOMX_TWCommandEvent *event)
{
	return WidgetParameters::handleBarCommandEvent (event);
}

//////////////////////////////////////////////////////////////////////////////////////

ScrollBarProxy::ScrollBarProxy (PluginGUI *plugin)
	: ControlProxy (plugin)
{
}

ATOM_Widget *ScrollBarProxy::createWidgetImpl (ATOM_Widget *parent, WidgetParameters *parameters)
{
	ATOM_ScrollBar *sb = ATOM_NEW(ATOM_ScrollBar, parent, parameters->rect(), parameters->style(), parameters->getId(), ATOM_Widget::ShowNormal);
	parameters->apply (sb);
	return sb;
}

void ScrollBarProxy::changeFontImpl (const char *fontName, ATOM_GUIFont::handle handleAfterChanged)
{
}

WidgetParameters *ScrollBarProxy::createParametersImpl (ControlProxy *proxy) const
{
	return ATOM_NEW(ScrollBarParameters, proxy);
}

