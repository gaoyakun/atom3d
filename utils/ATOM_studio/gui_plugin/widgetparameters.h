#ifndef __ATOM3D_STUDIO_GUIEDITOR_WIDGETPARAMETERS_H
#define __ATOM3D_STUDIO_GUIEDITOR_WIDGETPARAMETERS_H

#include <ATOM_engine.h>
#include <ATOM3DX.h>
#include "editor.h"

class ControlProxy;

class WidgetParameters
{
public:
	enum
	{
		PARAMID_SELECTIMAGE_OK = PLUGIN_ID_START + 50,
		PARAMID_SELECTIMAGE_CANCEL,
		PARAMID_SELECTIMAGE_NEW_IMAGE,
		PARAMID_SELECTIMAGE_DELETE_IMAGE,
		PARAMID_SELECTIMAGE_IMAGELIST,
		PARAMID_SELECTIMAGE_LABEL_NORMAL,
		PARAMID_SELECTIMAGE_LABEL_HOVER,
		PARAMID_SELECTIMAGE_LABEL_HOLD,
		PARAMID_SELECTIMAGE_LABEL_DISABLED,
		PARAMID_WIDGET_ID,
		PARAMID_WIDGET_NAME,
		PARAMID_WIDGET_DESC,
		PARAMID_IMAGE_ID,
		PARAMID_BORDERSTYLE,
		PARAMID_CLIENTDRAG,
		PARAMID_TITLEBAR,
		PARAMID_CLOSEBUTTON,
		PARAMID_CONTROL,
		PARAMID_NOCLIP,
		PARAMID_NOFOCUS,
		PARAMID_VSCROLL,
		PARAMID_HSCROLL,
		PARAMID_DISABLED,
		PARAMID_NONSCROLLABLE,
		PARAMID_CLIPCHILDREN,
		PARAMID_LEFTSCROLL,
		PARAMID_TOPSCROLL,
		PARAMID_ENABLEDROP,
		PARAMID_ENABLEDRAG,
		PARAMID_ENABLELAYOUT,
		PARAMID_LAYOUTTYPE,
		PARAMID_LAYOUTSPACELEFT,
		PARAMID_LAYOUTSPACETOP,
		PARAMID_LAYOUTSPACERIGHT,
		PARAMID_LAYOUTSPACEBOTTOM,
		PARAMID_LAYOUTGAP,
		PARAMID_RECT_X,
		PARAMID_RECT_Y,
		PARAMID_RECT_W,
		PARAMID_RECT_H,
		PARAMID_FONT,
		PARAMID_FONT_COLOR,
		PARAMID_FONT_OUTLINE_COLOR,

		PARAMID_WAITING_CHECK,			// wangjian added

		PARAMID_LAST
	};

	struct ImageStateInfo
	{
		ATOM_STRING filename;
		ATOM_STRING material_filename;
		ATOM_Rect2Di region;
		ATOM_ColorARGB color;

	};

	struct ImageInfo
	{
		ATOM_GUIImage::Type imageType;
		ImageStateInfo states[4];
	};

public:
	WidgetParameters (ControlProxy *proxy);
	WidgetParameters (const WidgetParameters &other);
	virtual ~WidgetParameters (void);

public:
	virtual bool interactive (void);
	virtual WidgetParameters *clone (void) const = 0;
	virtual void resetParameters (void);
	virtual void setupTweakBar (ATOMX_TweakBar *propBar);
	virtual void apply (ATOM_Widget *widget);
	virtual void saveToXML (ATOM_TiXmlElement *xml);
	virtual void loadFromXML (ATOM_TiXmlElement *xml);
	virtual bool handleBarValueChangedEvent (ATOMX_TWValueChangedEvent *event);
	virtual bool handleBarCommandEvent (ATOMX_TWCommandEvent *event);

public:
	const ATOM_Rect2Di &rect (void) const;
	ATOM_Rect2Di &rect (void);
	int getId (void) const;
	bool setId (int id);
	int imageId (void) const;
	int &imageId (void);
	int style (void) const;
	int &style (void);
	int borderMode (void) const;
	int &borderMode (void);
	int dragMode (void) const;
	int &dragMode (void);
	int enableDrop (void) const;
	int &enableDrop (void);
	int enableDrag (void) const;
	int &enableDrag (void);
	int disabled (void) const;
	int &disabled (void);
	ATOM_WidgetLayout::Type layoutType (void) const;
	ATOM_WidgetLayout::Type &layoutType (void);
	int layoutInnerSpaceLeft (void) const;
	int &layoutInnerSpaceLeft (void);
	int layoutInnerSpaceTop (void) const;
	int &layoutInnerSpaceTop (void);
	int layoutInnerSpaceRight (void) const;
	int &layoutInnerSpaceRight (void);
	int layoutInnerSpaceBottom (void) const;
	int &layoutInnerSpaceBottom (void);
	int layoutGap (void) const;
	int &layoutGap (void);
	ATOM_STRING font (void) const;
	ATOM_STRING &font (void);
	ATOM_ColorARGB fontColor (void) const;
	ATOM_ColorARGB &fontColor (void);
	ATOM_ColorARGB fontOutLineColor(void) const;
	ATOM_ColorARGB &fontOutLineColor(void);
	const ATOM_MAP<int,ImageInfo> &imageList (void) const;
	ATOM_MAP<int,ImageInfo> &imageList (void);
	ControlProxy *getProxy (void) const;
	void setProxy (ControlProxy *proxy);

	// wangjian added
	int waitingCheck (void) const;
	int &waitingCheck (void);

private:
	int selectImage (void);

protected:
	ControlProxy *_proxy;

	ATOM_Rect2Di _rect;
	int _id;
	int _imageId;
	int _style;
	int _borderMode;
	int _dragMode;
	int _enableDrop;
	int _enableDrag;
	int _disabled;
	ATOM_WidgetLayout::Type _layoutType;
	int _layoutSpaces[4];
	int _layoutGap;
	ATOM_STRING _desc;
	ATOM_STRING _font;
	ATOM_ColorARGB _fontColor;
	ATOM_ColorARGB _fontOutLineColor;
	ATOM_MAP<int,ImageInfo> _imagelist;

	// wangjian added
	// 异步加载时，是否需要等待WIDGET中资源加载完成才显示整个UI
	int	_waitingCheck;
};

#endif // __ATOM3D_STUDIO_GUIEDITOR_WIDGETPARAMETERS_H
