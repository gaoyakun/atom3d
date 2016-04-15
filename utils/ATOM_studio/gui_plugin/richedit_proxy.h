#ifndef __ATOM3D_STUDIO_GUIEDITOR_RICHEDIT_PROXY_H
#define __ATOM3D_STUDIO_GUIEDITOR_RICHEDIT_PROXY_H

#include "proxy.h"
#include "widgetparameters.h"

class RichEditParameters: public WidgetParameters
{
public:
	enum
	{
		PARAMID_RICHEDIT_MAXHEIGHT = WidgetParameters::PARAMID_LAST,
		PARAMID_RICHEDIT_AUTOSIZE,
		PARAMID_RICHEDIT_LINE_HEIGHT,
		
		PARAMID_RICHEDIT_SCROLLBAR_SLIDER_IMAGE,
		PARAMID_RICHEDIT_SCROLLBAR_UPBUTTON_IMAGE,
		PARAMID_RICHEDIT_SCROLLBAR_DOWNBUTTON_IMAGE,
		PARAMID_RICHEDIT_SCROLLBAR_BACKGROUND_IMAGE,
		PARAMID_RICHEDIT_SCROLLBAR_SLIDER_WIDTH,
		PARAMID_RICHEDIT_SCROLLBAR_SLIDER_LENGTH,
		PARAMID_RICHEDIT_SCROLLBAR_BUTTON_LENGTH,
		PARAMID_RICHEDIT_LAST
	};

public:
	RichEditParameters (ControlProxy *proxy);
	virtual void resetParameters (void);
	virtual WidgetParameters *clone (void) const;
	virtual void setupTweakBar (ATOMX_TweakBar *propBar);
	virtual void apply (ATOM_Widget *widget);
	virtual void saveToXML (ATOM_TiXmlElement *xml);
	virtual void loadFromXML (ATOM_TiXmlElement *xml);
	virtual bool handleBarValueChangedEvent (ATOMX_TWValueChangedEvent *event);
	virtual bool handleBarCommandEvent (ATOMX_TWCommandEvent *event);

protected:
	int _maxHeight;
	int _autoSize;
	int _lineHeight;
	//ATOM_ColorARGB _outlineColor;

	//! 滚动条的属性
	int _scrollBarSliderImageId;
	int _scrollBarBackgroundImageId;
	int _scrollBarUpButtonImageId;
	int _scrollBarDownButtonImageId;

	int _buttonLength; //! 滚动条按钮长度
	int _sliderLength; //! 滚动块的长度
	int _sliderWidth;  //! 滚动条的宽度

};

class RichEditProxy: public ControlProxy
{
public:
	RichEditProxy (PluginGUI *plugin);

protected:
	virtual ATOM_Widget *createWidgetImpl (ATOM_Widget *parent, WidgetParameters *parameters);
	virtual void changeFontImpl (const char *fontName, ATOM_GUIFont::handle handleAfterChanged);
	virtual WidgetParameters *createParametersImpl (ControlProxy *proxy) const;

	DECLARE_WIDGET_PROXY(RichEditProxy, richedit, ATOM_RichEdit)
};

#endif // __ATOM3D_STUDIO_GUIEDITOR_RICHEDIT_PROXY_H
