#ifndef __ATOM3D_STUDIO_GUIEDITOR_EDIT_PROXY_H
#define __ATOM3D_STUDIO_GUIEDITOR_EDIT_PROXY_H

#include "proxy.h"
#include "widgetparameters.h"

class EditParameters: public WidgetParameters
{
public:
	enum
	{
		PARAMID_EDIT_READONLY = WidgetParameters::PARAMID_LAST,
		PARAMID_EDIT_CONTENT,
		PARAMID_EDIT_NUMBERTYPE, // 0:无 1:整数 2:浮点
		PARAMID_EDIT_PASSWORD,
		PARAMID_EDIT_CURSOR_IMAGEID,
		PARAMID_EDIT_CURSOR_WIDTH,
		PARAMID_EDIT_CURSOR_OFFSET,
		PARAMID_EDIT_MAX_CHAR,
		PARAMID_EDIT_OFFSET_X,
		PARAMID_EDIT_FRONTIMG,
		PARAMID_EDIT_LAST
	};

public:
	EditParameters (ControlProxy *proxy);
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
	int _password;
	int _numberType;
	int _cursorImageId;
	int _cursorWidth;
	int _cursorOffset;
	int _maxChar;
	int _offsetX;
	int _frontImage;
	ATOM_STRING _text;
};

class EditProxy: public ControlProxy
{
public:
	EditProxy (PluginGUI *plugin);

protected:
	virtual ATOM_Widget *createWidgetImpl (ATOM_Widget *parent, WidgetParameters *parameters);
	virtual void changeFontImpl (const char *fontName, ATOM_GUIFont::handle handleAfterChanged);
	virtual WidgetParameters *createParametersImpl (ControlProxy *proxy) const;

	DECLARE_WIDGET_PROXY(EditProxy, edit, ATOM_Edit)
};

#endif // __ATOM3D_STUDIO_GUIEDITOR_EDIT_PROXY_H
