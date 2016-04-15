#include "StdAfx.h"
#include "progressbar_proxy.h"

ProgressBarParameters::ProgressBarParameters (ControlProxy *proxy): LabelParameters(proxy)
{
	style() |= ATOM_Widget::Border;
	borderMode() = ATOM_Widget::Drop;
	rect().size.w = 100;
	rect().size.h = 24;
	imageId() = ATOM_IMAGEID_WINDOW_BKGROUND;

	_alignmentX = 1;
	_frontImageId = ATOM_IMAGEID_PROGRESSBAR;
	_minValue = 0;
	_maxValue = 100;
	_currentValue = 50;
	_drawText = true;
}

void ProgressBarParameters::resetParameters (void)
{
	LabelParameters::resetParameters ();

	style() |= ATOM_Widget::Border;
	borderMode() = ATOM_Widget::Drop;
	rect().size.w = 100;
	rect().size.h = 24;
	imageId() = ATOM_IMAGEID_WINDOW_BKGROUND;

	_alignmentX = 1;
	_frontImageId = ATOM_IMAGEID_PROGRESSBAR;
	_minValue = 0;
	_maxValue = 100;
	_currentValue = 50;
	_drawText = true;
	_isHorizontal = true;
}

WidgetParameters *ProgressBarParameters::clone (void) const
{
	return ATOM_NEW(ProgressBarParameters, *this);
}

void ProgressBarParameters::setupTweakBar (ATOMX_TweakBar *propBar)
{
	LabelParameters::setupTweakBar (propBar);
	propBar->addBoolVar ("水平", PARAMID_PROGRESSBAR_HORIZONTAL, _isHorizontal, false, "进度条", 0);
	propBar->addIntVar ("进度条图片", PARAMID_PROGRESSBAR_FRONTIMAGE, _frontImageId, false, "进度条", 0);
	propBar->addIntVar ("最小值", PARAMID_PROGRESSBAR_MINVALUE, _minValue, false, "进度条", 0);
	propBar->addIntVar ("最大值", PARAMID_PROGRESSBAR_MAXVALUE, _maxValue, false, "进度条", 0);
	propBar->addIntVar ("当前值", PARAMID_PROGRESSBAR_CURRENTVALUE, _currentValue, false, "进度条", 0);
	propBar->addBoolVar ("显示当前值", PARAMID_PROGRESSBAR_DRAWTEXT, _drawText, false, "进度条", 0);
}

void ProgressBarParameters::apply (ATOM_Widget *widget)
{
	LabelParameters::apply (widget);

	ATOM_ProgressBar *progressBar = (ATOM_ProgressBar*)widget;
	progressBar->setFrontImageId (_frontImageId);
	progressBar->setMinMax (_minValue, _maxValue);
	progressBar->setCurrent (_currentValue);
	progressBar->enableDrawText (_drawText);
	progressBar->setIsHorizontal(_isHorizontal);
}

void ProgressBarParameters::saveToXML (ATOM_TiXmlElement *xml)
{
	LabelParameters::saveToXML (xml);

	xml->SetAttribute ("frontimage", _frontImageId);
	xml->SetAttribute ("min", _minValue);
	xml->SetAttribute ("max", _maxValue);
	xml->SetAttribute ("current", _currentValue);
	xml->SetAttribute ("drawtext", _drawText?1:0);
	xml->SetAttribute("horizontal",_isHorizontal?1:0);
}

void ProgressBarParameters::loadFromXML (ATOM_TiXmlElement *xml)
{
	LabelParameters::loadFromXML (xml);

	if (xml)
	{
		xml->QueryIntAttribute ("frontimage", &_frontImageId);
		xml->QueryIntAttribute ("min", &_minValue);
		xml->QueryIntAttribute ("max", &_maxValue);
		xml->QueryIntAttribute ("current", &_currentValue);
		int bDrawText = 1;
		xml->QueryIntAttribute ("drawtext", &bDrawText);
		_drawText = (bDrawText != 0);
		int bIsHorizontal = 1;
		xml->QueryIntAttribute ("horizontal", &bIsHorizontal);
		_isHorizontal = (bIsHorizontal != 0);
	}
}

bool ProgressBarParameters::handleBarValueChangedEvent (ATOMX_TWValueChangedEvent *event)
{
	switch (event->id)
	{
	case PARAMID_PROGRESSBAR_FRONTIMAGE:
		_frontImageId = event->newValue.getI();
		return true;
	case PARAMID_PROGRESSBAR_MINVALUE:
		_minValue = event->newValue.getI();
		return true;
	case PARAMID_PROGRESSBAR_MAXVALUE:
		_maxValue = event->newValue.getI();
		return true;
	case PARAMID_PROGRESSBAR_CURRENTVALUE:
		_currentValue = event->newValue.getI();
		return true;
	case PARAMID_PROGRESSBAR_DRAWTEXT:
		_drawText = event->newValue.getB();
		return true;
	case PARAMID_PROGRESSBAR_HORIZONTAL:
		_isHorizontal = event->newValue.getB();
		return true;
	default:
		return LabelParameters::handleBarValueChangedEvent (event);
	}
}

bool ProgressBarParameters::handleBarCommandEvent (ATOMX_TWCommandEvent *event)
{
	return LabelParameters::handleBarCommandEvent (event);
}

//////////////////////////////////////////////////////////////////////////////////////

ProgressBarProxy::ProgressBarProxy (PluginGUI *plugin)
	: LabelProxy (plugin)
{
}

ATOM_Widget *ProgressBarProxy::createWidgetImpl (ATOM_Widget *parent, WidgetParameters *parameters)
{
	ATOM_ProgressBar *progressBar = ATOM_NEW(ATOM_ProgressBar, parent, parameters->rect(), parameters->style(), parameters->getId(), ATOM_Widget::ShowNormal);
	parameters->apply (progressBar);
	return progressBar;
}

void ProgressBarProxy::changeFontImpl (const char *fontName, ATOM_GUIFont::handle handleAfterChanged)
{
}

WidgetParameters *ProgressBarProxy::createParametersImpl (ControlProxy *proxy) const
{
	return ATOM_NEW(ProgressBarParameters, proxy);
}

