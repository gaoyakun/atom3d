#include "StdAfx.h"
#include "label_proxy.h"
#include "widgetparameters.h"

LabelParameters::LabelParameters (ControlProxy *proxy): PanelParameters(proxy)
{
	_alignmentX = 0;
	_alignmentY = 1;
	_textOffsetX = 0;
	_textOffsetY = 0;
	_color = 0;
	_underlineColor = 0;
	_rotation = 0.f;
}

void LabelParameters::resetParameters (void)
{
	PanelParameters::resetParameters ();

	rect().size.w = 100;
	rect().size.h = 24;

	_alignmentX = 0;
	_alignmentY = 1;
	_textOffsetX = 0;
	_textOffsetY = 0;
	_color = 0;
	_underlineColor = 0;
	_rotation = 0.f;
	_text = "标签";
}

WidgetParameters *LabelParameters::clone (void) const
{
	return ATOM_NEW(LabelParameters, *this);
}

void LabelParameters::setupTweakBar (ATOMX_TweakBar *propBar)
{
	PanelParameters::setupTweakBar (propBar);

	propBar->addStringVar ("文字", PARAMID_LABEL_TEXT, _text.c_str(), false, "标签", 0);
	ATOMX_TBEnum alignEnum;
	alignEnum.addEnum ("左", 0);
	alignEnum.addEnum ("中", 1);
	alignEnum.addEnum ("右", 2);
	propBar->addEnum ("横向对齐", PARAMID_LABEL_XALIGNMENT, _alignmentX, alignEnum, false, "标签", 0);
	propBar->addEnum ("纵向对齐", PARAMID_LABEL_YALIGNMENT, _alignmentY, alignEnum, false, "标签", 0);
	propBar->addIntVar ("文字横向偏移", PARAMID_LABEL_TEXTOFFSETX, _textOffsetX, false, "标签", 0);
	propBar->addIntVar ("文字纵向偏移", PARAMID_LABEL_TEXTOFFSETY, _textOffsetY, false, "标签", 0);
	propBar->addRGBAVar ("颜色", PARAMID_LABEL_COLOR, _color.getFloatR(), _color.getFloatG(), _color.getFloatB(), _color.getFloatA(), false, "标签", 0);
//	propBar->addRGBAVar ("描边颜色", PARAMID_LABEL_OUTLINE_COLOR, _outlineColor.getFloatR(), _outlineColor.getFloatG(), _outlineColor.getFloatB(), _outlineColor.getFloatA(), false, "标签", 0);
	propBar->addRGBAVar ("下划线颜色", PARAMID_LABEL_UNDERLINE_COLOR, _underlineColor.getFloatR(), _underlineColor.getFloatG(), _underlineColor.getFloatB(), _underlineColor.getFloatA(), false, "标签", 0);
	propBar->addFloatVar ("旋转角度", PARAMID_LABEL_ROTATION, _rotation, false, "标签", 0);
}

void LabelParameters::apply (ATOM_Widget *widget)
{
	PanelParameters::apply (widget);

	ATOM_Label *label = (ATOM_Label*)widget;
	int alignment = 0;
	switch (_alignmentX)
	{
	case 0: alignment |= ATOM_Widget::AlignX_Left; break;
	case 1: alignment |= ATOM_Widget::AlignX_Middle; break;
	case 2: alignment |= ATOM_Widget::AlignX_Right; break;
	}
	switch (_alignmentY)
	{
	case 0: alignment |= ATOM_Widget::AlignY_Top; break;
	case 1: alignment |= ATOM_Widget::AlignY_Middle; break;
	case 2: alignment |= ATOM_Widget::AlignY_Bottom; break;
	}
	label->setText (_text.c_str());
	label->setAlign (alignment);
	label->setOffset (ATOM_Point2Di(_textOffsetX, _textOffsetY));
	label->setColor (_color);
	label->setUnderlineColor (_underlineColor);
	label->setRotation (_rotation);
}

void LabelParameters::saveToXML (ATOM_TiXmlElement *xml)
{
	PanelParameters::saveToXML (xml);

	if (!_text.empty ())
	{
		xml->SetAttribute ("text", _text.c_str());
	}
	xml->SetAttribute ("alignx", _alignmentX);
	xml->SetAttribute ("aligny", _alignmentY);
	xml->SetAttribute ("offsetx", _textOffsetX);
	xml->SetAttribute ("offsetY", _textOffsetY);
	xml->SetAttribute ("color", int(_color));
	xml->SetAttribute ("underlinecolor", int(_underlineColor));
	xml->SetDoubleAttribute ("rotation", _rotation);
}

void LabelParameters::loadFromXML (ATOM_TiXmlElement *xml)
{
	PanelParameters::loadFromXML (xml);

	if (xml)
	{
		const char *text = xml->Attribute ("text");
		_text = text ? text : "";
		xml->QueryIntAttribute ("alignx", &_alignmentX);
		xml->QueryIntAttribute ("aligny", &_alignmentY);
		xml->QueryIntAttribute ("offsetx", &_textOffsetX);
		xml->QueryIntAttribute ("offsetY", &_textOffsetY);
		xml->QueryIntAttribute ("color", (int*)&_color);
		xml->QueryIntAttribute ("underlinecolor", (int*)&_underlineColor);
		xml->QueryFloatAttribute ("rotation", &_rotation);
	}
}

bool LabelParameters::handleBarValueChangedEvent (ATOMX_TWValueChangedEvent *event)
{
	switch (event->id)
	{
	case PARAMID_LABEL_XALIGNMENT:
		_alignmentX = event->newValue.getI();
		return true;
	case PARAMID_LABEL_YALIGNMENT:
		_alignmentY = event->newValue.getI();
		return true;
	case PARAMID_LABEL_TEXT:
		_text = event->newValue.getS();
		return true;
	case PARAMID_LABEL_TEXTOFFSETX:
		_textOffsetX = event->newValue.getI();
		return true;
	case PARAMID_LABEL_TEXTOFFSETY:
		_textOffsetY = event->newValue.getI();
		return true;
	case PARAMID_LABEL_COLOR:
		_color.setFloats (event->newValue.get4F()[0], event->newValue.get4F()[1], event->newValue.get4F()[2], event->newValue.get4F()[3]);
		return true;
		/*case PARAMID_LABEL_OUTLINE_COLOR:
		_outlineColor.setFloats (event->newValue.get4F()[0], event->newValue.get4F()[1], event->newValue.get4F()[2], event->newValue.get4F()[3]);
		return true;*/
	case PARAMID_LABEL_UNDERLINE_COLOR:
		_underlineColor.setFloats (event->newValue.get4F()[0], event->newValue.get4F()[1], event->newValue.get4F()[2], event->newValue.get4F()[3]);
		return true;
	case PARAMID_LABEL_ROTATION:
		_rotation = event->newValue.getF();
		return true;
	default:
		return PanelParameters::handleBarValueChangedEvent (event);
	}
}

bool LabelParameters::handleBarCommandEvent (ATOMX_TWCommandEvent *event)
{
	return PanelParameters::handleBarCommandEvent (event);
}

//////////////////////////////////////////////////////////////////////////////////////

LabelProxy::LabelProxy (PluginGUI *plugin)
	: PanelProxy (plugin)
{
}

ATOM_Widget *LabelProxy::createWidgetImpl (ATOM_Widget *parent, WidgetParameters *parameters)
{
	ATOM_Label *label = ATOM_NEW(ATOM_Label, parent, parameters->rect(), parameters->style(), parameters->getId(), ATOM_Widget::ShowNormal);
	parameters->apply (label);
	label->enableEventTransition (false);
	return label;
}

void LabelProxy::changeFontImpl (const char *fontName, ATOM_GUIFont::handle handleAfterChanged)
{
}

WidgetParameters *LabelProxy::createParametersImpl (ControlProxy *proxy) const
{
	return ATOM_NEW(LabelParameters, proxy);
}

