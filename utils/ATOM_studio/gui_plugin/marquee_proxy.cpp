#include "StdAfx.h"
#include "marquee_proxy.h"
#include "widgetparameters.h"

void MarqueeParameters::resetParameters (void)
{
	WidgetParameters::resetParameters ();

	rect().size.w = 100;
	rect().size.h = 24;

	_dir = MARQUEEDIR_RIGHT;
	_speed = 0.f;
	_offset = 10.f;
	_position = 0.f;
}

WidgetParameters *MarqueeParameters::clone (void) const
{
	return ATOM_NEW(MarqueeParameters, *this);
}

void MarqueeParameters::setupTweakBar (ATOMX_TweakBar *propBar)
{
	WidgetParameters::setupTweakBar (propBar);

	ATOMX_TBEnum eDir;
	eDir.addEnum ("左", MARQUEEDIR_LEFT);
	eDir.addEnum ("右", MARQUEEDIR_RIGHT);
	propBar->addEnum ("滚动方向", PARAMID_MARQUEE_DIRECTION, _dir, eDir, false, "跑马灯");
	propBar->addFloatVar ("滚动速度", PARAMID_MARQUEE_SPEED, _speed, false, "跑马灯");
	propBar->addFloatVar ("间距", PARAMID_MARQUEE_OFFSET, _offset, false, "跑马灯");
	propBar->addFloatVar ("位置", PARAMID_MARQUEE_POSITION, _position, false, "跑马灯");
	propBar->addStringVar ("文字", PARAMID_MARQUEE_TEXT, _text.c_str(), false, "跑马灯");
}

void MarqueeParameters::apply (ATOM_Widget *widget)
{
	WidgetParameters::apply (widget);

	ATOM_Marquee *marquee = (ATOM_Marquee*)widget;
	marquee->setDir (_dir);
	marquee->setPosition (_position);
	marquee->setOffset (_offset);
	marquee->setSpeed (_speed);
	marquee->setText (_text.c_str());
}

void MarqueeParameters::saveToXML (ATOM_TiXmlElement *xml)
{
	WidgetParameters::saveToXML (xml);

	xml->SetAttribute ("dir", (int)_dir);
	xml->SetDoubleAttribute ("position", _position);
	xml->SetDoubleAttribute ("offset", _offset);
	xml->SetDoubleAttribute ("speed", _speed);
	xml->SetAttribute ("text", _text.c_str());
}

void MarqueeParameters::loadFromXML (ATOM_TiXmlElement *xml)
{
	WidgetParameters::loadFromXML (xml);

	if (xml)
	{
		int dir = MARQUEEDIR_RIGHT;
		double pos = 0;
		double speed = 0;
		double offset = 0;

		xml->QueryIntAttribute("dir", &dir);
		xml->QueryDoubleAttribute("position", &pos);
		xml->QueryDoubleAttribute ("offset", &offset);
		xml->QueryDoubleAttribute ("speed", &speed);
		_dir = (MARQUEEDIR)dir;
		_position = pos;
		_offset = offset;
		_speed = speed;

		const char *s = xml->Attribute ("text");
		_text = s ? s : "";
	}
}

bool MarqueeParameters::handleBarValueChangedEvent (ATOMX_TWValueChangedEvent *event)
{
	switch (event->id)
	{
	case PARAMID_MARQUEE_DIRECTION:
		_dir = (MARQUEEDIR)event->newValue.getI();
		return true;
	case PARAMID_MARQUEE_SPEED:
		_speed = event->newValue.getF();
		return true;
	case PARAMID_MARQUEE_POSITION:
		_position = event->newValue.getF();
		return true;
	case PARAMID_MARQUEE_OFFSET:
		_offset = event->newValue.getF();
		return true;
	case PARAMID_MARQUEE_TEXT:
		_text = event->newValue.getS();
		return true;
	default:
		return WidgetParameters::handleBarValueChangedEvent (event);
	}
}

bool MarqueeParameters::handleBarCommandEvent (ATOMX_TWCommandEvent *event)
{
	return WidgetParameters::handleBarCommandEvent (event);
}

MarqueeProxy::MarqueeProxy (PluginGUI *plugin)
	: ControlProxy (plugin)
{
}

ATOM_Widget *MarqueeProxy::createWidgetImpl (ATOM_Widget *parent, WidgetParameters *parameters)
{
	ATOM_Marquee *marquee = ATOM_NEW(ATOM_Marquee, parent, parameters->rect(), parameters->style(), parameters->getId(), ATOM_Widget::ShowNormal);
	parameters->apply (marquee);
	return marquee;
}

void MarqueeProxy::changeFontImpl (const char *fontName, ATOM_GUIFont::handle handleAfterChanged)
{
}

WidgetParameters *MarqueeProxy::createParametersImpl (ControlProxy *proxy) const
{
	return ATOM_NEW(MarqueeParameters, proxy);
}

