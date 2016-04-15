#ifndef __ATOM3D_STUDIO_GUIEDITOR_MULTIEDIT_PROXY_H
#define __ATOM3D_STUDIO_GUIEDITOR_MULTIEDIT_PROXY_H

#include "proxy.h"
#include "widgetparameters.h"

class MultieditParameters: public WidgetParameters
{
public:
	enum
	{
		PARAMID_MULTIEDIT_READONLY = WidgetParameters::PARAMID_LAST,
		PARAMID_MULTIEDIT_LINEHEIGHT,
		PARAMID_MULTIEDIT_CONTENT,
		PARAMID_MULTIEDIT_CURSOR_IMAGEID,
		PARAMID_MULTIEDIT_CURSOR_WIDTH,
		PARAMID_MULTIEDIT_CURSOR_OFFSET,
		PARAMID_MULTIEDIT_MAX_CHAR,

		PARAMID_MULTIEDIT_SCROLLBAR_SLIDER_IMAGE,
		PARAMID_MULTIEDIT_SCROLLBAR_UPBUTTON_IMAGE,
		PARAMID_MULTIEDIT_SCROLLBAR_DOWNBUTTON_IMAGE,
		PARAMID_MULTIEDIT_SCROLLBAR_BACKGROUND_IMAGE,
		PARAMID_MULTIEDIT_SCROLLBAR_SLIDER_WIDTH,
		PARAMID_MULTIEDIT_SCROLLBAR_SLIDER_LENGTH,
		PARAMID_MULTIEDIT_SCROLLBAR_BUTTON_LENGTH,
		PARAMID_MULTIEDIT_FRONTIMG,
		PARAMID_MULTIEDIT_LAST
	};

public:
	MultieditParameters (ControlProxy *proxy);
	virtual void resetParameters (void);
	virtual WidgetParameters *clone (void) const;
	virtual void setupTweakBar (ATOMX_TweakBar *propBar);
	virtual void apply (ATOM_Widget *widget);
	virtual void saveToXML (ATOM_TiXmlElement *xml);
	virtual void loadFromXML (ATOM_TiXmlElement *xml);
	virtual bool handleBarValueChangedEvent (ATOMX_TWValueChangedEvent *event);
	virtual bool handleBarCommandEvent (ATOMX_TWCommandEvent *event);

private:
	int _readonly;
	int _lineHeight;
	int _cursorImageId;
	int _cursorWidth;
	int _cursorOffset;
	int _maxChar;
	ATOM_STRING _text;
	int _frontimage;

	//! ������������
	int _scrollBarSliderImageId;
	int _scrollBarBackgroundImageId;
	int _scrollBarUpButtonImageId;
	int _scrollBarDownButtonImageId;

	int _buttonLength; //! ��������ť����
	int _sliderLength; //! ������ĳ���
	int _sliderWidth;  //! �������Ŀ��
};

class MultieditProxy: public ControlProxy
{
public:
	MultieditProxy (PluginGUI *plugin);

protected:
	virtual ATOM_Widget *createWidgetImpl (ATOM_Widget *parent, WidgetParameters *parameters);
	virtual void changeFontImpl (const char *fontName, ATOM_GUIFont::handle handleAfterChanged);
	virtual WidgetParameters *createParametersImpl (ControlProxy *proxy) const;

	DECLARE_WIDGET_PROXY(MultieditProxy, multiedit, ATOM_MultiEdit)
};

#endif // __ATOM3D_STUDIO_GUIEDITOR_MULTIEDIT_PROXY_H
