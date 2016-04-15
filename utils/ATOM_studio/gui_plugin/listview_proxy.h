#ifndef __ATOM3D_STUDIO_GUIEDITOR_LISTVIEW_PROXY_H
#define __ATOM3D_STUDIO_GUIEDITOR_LISTVIEW_PROXY_H

#include "proxy.h"
#include "widgetparameters.h"

class ListViewParameters: public WidgetParameters
{
public:
	enum
	{
		PARAMID_LISTVIEW_COLUMN_COUNT = WidgetParameters::PARAMID_LAST,
		PARAMID_LISTVIEW_ROW_COUNT,
		PARAMID_LISTVIEW_COLUMN_WIDTH,
		PARAMID_LISTVIEW_ITEM_HEIGHT,
		PARAMID_LISTVIEW_SELECTED_IMAGE,
		PARAMID_LISTVIEW_SCROLLBAR_BACKGROUND_IMAGE,
		PARAMID_LISTVIEW_SCROLLBAR_UPBUTTON_IMAGE,
		PARAMID_LISTVIEW_SCROLLBAR_DOWNBUTTON_IMAGE,
		PARAMID_LISTVIEW_SCROLLBAR_SLIDER_IMAGE,
		RARAMID_LISTVIEW_BKGROUND_IMGAE,
		RARAMID_LISTVIEW_HOVER_IMAGE,
		PARAMID_LISTVIEW_SCROLLBAR_SLIDER_WIDTH,
		PARAMID_LISTVIEW_SCROLLBAR_SLIDER_LENGTH,
		PARAMID_LISTVIEW_SCROLLBAR_BUTTON_LENGTH,
		PARAMID_LISTVIEW_LAST
	};

public:
	ListViewParameters (ControlProxy *proxy);
	virtual void resetParameters (void);
	virtual WidgetParameters *clone (void) const;
	virtual void setupTweakBar (ATOMX_TweakBar *propBar);
	virtual void apply (ATOM_Widget *widget);
	virtual void saveToXML (ATOM_TiXmlElement *xml);
	virtual void loadFromXML (ATOM_TiXmlElement *xml);
	virtual bool handleBarValueChangedEvent (ATOMX_TWValueChangedEvent *event);
	virtual bool handleBarCommandEvent (ATOMX_TWCommandEvent *event);

protected:
	int _columnCount;
	int _rowCount;
	int _columnWidth;
	int _itemHeight;
	int _selectedImage;
	int _scrollBarBackgroundImageId;
	int _scrollBarUpButtonImageId;
	int _scrollBarDownButtonImageId;
	int _scrollBarSliderImageId;
	int _hoverImage;
	int _singleBackGroundImage;

	int _buttonLength; //! 滚动条按钮长度
	int _sliderLength; //! 滚动块的长度
	int _sliderWidth;  //! 滚动条的宽度
};

class ListViewProxy: public ControlProxy
{
public:
	ListViewProxy (PluginGUI *plugin);

protected:
	virtual ATOM_Widget *createWidgetImpl (ATOM_Widget *parent, WidgetParameters *parameters);
	virtual void changeFontImpl (const char *fontName, ATOM_GUIFont::handle handleAfterChanged);
	virtual WidgetParameters *createParametersImpl (ControlProxy *proxy) const;

	DECLARE_WIDGET_PROXY(ListViewProxy, listview, ATOM_ListView)
};

#endif // __ATOM3D_STUDIO_GUIEDITOR_LISTVIEW_PROXY_H
