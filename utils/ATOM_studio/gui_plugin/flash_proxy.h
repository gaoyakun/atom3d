#ifndef __ATOM3D_STUDIO_GUIEDITOR_FLASH_PROXY_H
#define __ATOM3D_STUDIO_GUIEDITOR_FLASH_PROXY_H

#include "proxy.h"
#include "widgetparameters.h"

class FlashParameters: public WidgetParameters
{
public:
	enum
	{
		PARAMID_FLASH_FILENAME = WidgetParameters::PARAMID_LAST,
		PARAMID_INIT_FUNC,
		PARAMID_CLEAR_FUNC,
		PARAMID_ACTIVATE_FUNC,
		PARAMID_DEACTIVATE_FUNC,
		PARAMID_FPS,
		PARAMID_FLASH_LAST
	};

public:
	FlashParameters (ControlProxy *proxy): WidgetParameters(proxy)
	{
	}

	const ATOM_STRING &SWFFileName (void) const;
	ATOM_STRING &SWFFileName (void);
	const ATOM_STRING &initFunc (void) const;
	ATOM_STRING &initFunc (void);
	const ATOM_STRING &clearFunc (void) const;
	ATOM_STRING &clearFunc (void);
	const ATOM_STRING &activateFunc (void) const;
	ATOM_STRING &activateFunc (void);
	const ATOM_STRING &deactivateFunc (void) const;
	ATOM_STRING &deactivateFunc (void);
	int FPS (void) const;
	int &FPS (void);

	virtual void resetParameters (void);
	virtual WidgetParameters *clone (void) const;
	virtual void setupTweakBar (ATOMX_TweakBar *propBar);
	virtual void apply (ATOM_Widget *widget);
	virtual void saveToXML (ATOM_TiXmlElement *xml);
	virtual void loadFromXML (ATOM_TiXmlElement *xml);
	virtual bool handleBarValueChangedEvent (ATOMX_TWValueChangedEvent *event);
	virtual bool handleBarCommandEvent (ATOMX_TWCommandEvent *event);

private:
	ATOM_STRING _fileName;
	ATOM_STRING _initFunc;
	ATOM_STRING _activateFunc;
	ATOM_STRING _deactivateFunc;
	ATOM_STRING _clearFunc;
	int _FPS;
};

class FlashProxy: public ControlProxy
{
public:
	FlashProxy (PluginGUI *plugin);

protected:
	virtual ATOM_Widget *createWidgetImpl (ATOM_Widget *parent, WidgetParameters *parameters);
	virtual void changeFontImpl (const char *fontName, ATOM_GUIFont::handle handleAfterChanged);
	virtual WidgetParameters *createParametersImpl (ControlProxy *proxy) const;

	DECLARE_WIDGET_PROXY(FlashProxy, flash, ATOM_FlashCtrl)
};

#endif // __ATOM3D_STUDIO_GUIEDITOR_FLASH_PROXY_H
