#include "StdAfx.h"
#include "treectrl_proxy.h"

TreeCtrlParameters::TreeCtrlParameters (ControlProxy *proxy): WidgetParameters(proxy)
{
	rect().size.w = 60;
	rect().size.h = 100;
	imageId() = ATOM_IMAGEID_WINDOW_BKGROUND;

	_imageSize = 0;
	_lineHeight = 0;
	_indent = 0;
	_textImageId = -1;
}

void TreeCtrlParameters::resetParameters (void)
{
	WidgetParameters::resetParameters ();

	rect().size.w = 60;
	rect().size.h = 100;
	imageId() = ATOM_IMAGEID_WINDOW_BKGROUND;
	style() = style()|ATOM_Widget::Border;
	borderMode() = ATOM_Widget::Drop;

	_imageSize = 0;
	_lineHeight = 0;
	_indent = 0;
	_textImageId = -1;
}

WidgetParameters *TreeCtrlParameters::clone (void) const
{
	return ATOM_NEW(TreeCtrlParameters, *this);
}

void TreeCtrlParameters::setupTweakBar (ATOMX_TweakBar *propBar)
{
	WidgetParameters::setupTweakBar (propBar);

	propBar->addIntVar ("Í¼±ê´óÐ¡", PARAMID_TREECTRL_IMAGESIZE, _imageSize, false, "Ê÷¿Ø¼þ", 0);
	propBar->addIntVar ("ÐÐ¸ß", PARAMID_TREECTRL_LINEHEIGHT, _lineHeight, false, "Ê÷¿Ø¼þ", 0);
	propBar->addIntVar ("Ëõ½ø¾àÀë", PARAMID_TREECTRL_INDENT, _indent, false, "Ê÷¿Ø¼þ", 0);
	propBar->addIntVar ("ÎÄ×Ö±³¾°", PARAMID_TREECTRL_TEXT_IMAGE, _textImageId, false, "Ê÷¿Ø¼þ", 0);
//	propBar->addRGBAVar ("Ãè±ßÑÕÉ«", PARAMID_TREECTRL_OUTLINE_COLOR, _outlineColor.getFloatR(), _outlineColor.getFloatG(), _outlineColor.getFloatB(), _outlineColor.getFloatA(), false, "Ê÷¿Ø¼þ", 0);
}

void TreeCtrlParameters::apply (ATOM_Widget *widget)
{
	WidgetParameters::apply (widget);

	ATOM_TreeCtrl *tc = (ATOM_TreeCtrl*)widget;
	tc->setTextImageId (_textImageId);
}

void TreeCtrlParameters::saveToXML (ATOM_TiXmlElement *xml)
{
	WidgetParameters::saveToXML (xml);

	xml->SetAttribute ("imagesize", _imageSize);
	xml->SetAttribute ("lineheight", _lineHeight);
	xml->SetAttribute ("indent", _indent);
	xml->SetAttribute ("textimage", _textImageId);
}

void TreeCtrlParameters::loadFromXML (ATOM_TiXmlElement *xml)
{
	WidgetParameters::loadFromXML (xml);

	if (xml)
	{
		xml->QueryIntAttribute ("imagesize", &_imageSize);
		xml->QueryIntAttribute ("lineheight", &_lineHeight);
		xml->QueryIntAttribute ("indent", &_indent);
		xml->QueryIntAttribute ("textimage", &_textImageId);
	}
}

bool TreeCtrlParameters::handleBarValueChangedEvent (ATOMX_TWValueChangedEvent *event)
{
	switch (event->id)
	{
	case PARAMID_TREECTRL_IMAGESIZE:
		_imageSize = event->newValue.getI();
		return true;
	case PARAMID_TREECTRL_LINEHEIGHT:
		_lineHeight = event->newValue.getI();
		return true;
	case PARAMID_TREECTRL_INDENT:
		_indent = event->newValue.getI();
		return true;
	case PARAMID_TREECTRL_TEXT_IMAGE:
		_textImageId = event->newValue.getI();
		return true;
	default:
		return WidgetParameters::handleBarValueChangedEvent (event);
	}
}

bool TreeCtrlParameters::handleBarCommandEvent (ATOMX_TWCommandEvent *event)
{
	return WidgetParameters::handleBarCommandEvent (event);
}

//////////////////////////////////////////////////////////////////////////////////////

TreeCtrlProxy::TreeCtrlProxy (PluginGUI *plugin)
	: ControlProxy (plugin)
{
}

ATOM_Widget *TreeCtrlProxy::createWidgetImpl (ATOM_Widget *parent, WidgetParameters *parameters)
{
	TreeCtrlParameters *params = (TreeCtrlParameters*)parameters;
	ATOM_TreeCtrl *tc = ATOM_NEW(ATOM_TreeCtrl, parent, parameters->rect(), parameters->style(), parameters->getId(), params->_imageSize, params->_lineHeight, params->_indent, ATOM_Widget::ShowNormal);
	parameters->apply (tc);
	return tc;
}

void TreeCtrlProxy::changeFontImpl (const char *fontName, ATOM_GUIFont::handle handleAfterChanged)
{
}

WidgetParameters *TreeCtrlProxy::createParametersImpl (ControlProxy *proxy) const
{
	return ATOM_NEW(TreeCtrlParameters, proxy);
}

