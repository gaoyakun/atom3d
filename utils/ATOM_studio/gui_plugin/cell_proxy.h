#ifndef __ATOM3D_STUDIO_GUIEDITOR_CELL_PROXY_H
#define __ATOM3D_STUDIO_GUIEDITOR_CELL_PROXY_H

#include "proxy.h"
#include "widgetparameters.h"

class CellParameters: public WidgetParameters
{
public:
	enum
	{
		PARAMID_CELL_SIZEX = WidgetParameters::PARAMID_LAST,
		PARAMID_CELL_SIZEY,
		PARAMID_CELL_CELLWIDTH,
		PARAMID_CELL_CELLHEIGHT,
		PARAMID_CELL_XALIGNMENT,
		PARAMID_CELL_YALIGNMENT,
		PARAMID_CELL_TEXTOFFSETX,
		PARAMID_CELL_TEXTOFFSETY,
		PARAMID_CELL_CD_IMAGE,
		PARAMID_CELL_SELECTED_IMAGE,
		PARAMID_CELL_FRAME_IMAGE,
		PARAMID_CELL_FRAME_OFFSET,
		PARAMID_CELL_SPACE_X,
		PARAMID_CELL_SPACE_Y,
		PARAMID_CELL_DRAGDROP,
		PARAMID_CELL_GROWMODE,
		PARAMID_CELL_SUBSCRIPT_IMAGE,
		PARAMID_CELL_SUBSCRIPT_IMAGEOFFSETX,
		PARAMID_CELL_SUBSCRIPT_IMAGEOFFSETY,
		PARAMID_CELL_SUBSCRIPT_IMAGEWIDTH,
		PARAMID_CELL_SUBSCRIPT_IMAGEHEIGHT,
		PARAMID_CELL_SUBSCRIPT_TEXTOFFSETX,
		PARAMID_CELL_SUBSCRIPT_TEXTOFFSETY,
		PARAMID_CELL_LAST
	};

public:
	CellParameters (ControlProxy *proxy);
	virtual void resetParameters (void);
	virtual WidgetParameters *clone (void) const;
	virtual void setupTweakBar (ATOMX_TweakBar *propBar);
	virtual void apply (ATOM_Widget *widget);
	virtual void saveToXML (ATOM_TiXmlElement *xml);
	virtual void loadFromXML (ATOM_TiXmlElement *xml);
	virtual bool handleBarValueChangedEvent (ATOMX_TWValueChangedEvent *event);
	virtual bool handleBarCommandEvent (ATOMX_TWCommandEvent *event);

protected:
	int _sizeX;
	int _sizeY;
	int _cellWidth;
	int _cellHeight;
	int _alignmentX;
	int _alignmentY;
	int _textOffsetX;
	int _textOffsetY;
	int _cooldownImage;
	int _selectedImage;
	int _frameImage;
	int _frameOffset;
	int _spaceX;
	int _spaceY;
	int _dragdrop;
	int _growmode;
	ATOM_Rect2Di _subscriptImageRect;
	int          _subscriptImageId;
	ATOM_Point2Di _subscriptTextPosition;
	//ATOM_ColorARGB _outlineColor;
};

class CellProxy: public ControlProxy
{
public:
	CellProxy (PluginGUI *plugin);

protected:
	virtual ATOM_Widget *createWidgetImpl (ATOM_Widget *parent, WidgetParameters *parameters);
	virtual void changeFontImpl (const char *fontName, ATOM_GUIFont::handle handleAfterChanged);
	virtual WidgetParameters *createParametersImpl (ControlProxy *proxy) const;

	DECLARE_WIDGET_PROXY(CellProxy, cell, ATOM_Cell)
};

#endif // __ATOM3D_STUDIO_GUIEDITOR_CELL_PROXY_H
