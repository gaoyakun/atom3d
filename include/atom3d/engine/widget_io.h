#ifndef __ATOM3D_ENGINE_WIDGET_IO_H
#define __ATOM3D_ENGINE_WIDGET_IO_H

#include "../ATOM_dbghlp.h"
#include "../ATOM_utils.h"
#include "basedefs.h"
#include "imagelist.h"

class ATOM_ENGINE_API ATOM_WidgetParameters
{
public:
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

		//--- wangjian added ---//
		// 加载优先级 默认构造时设置为未设定
		enum
		{
			LOAD_NOT_SPECIFIED = -2,
		};
		int asycnLoadFlag;
		ImageInfo():asycnLoadFlag(LOAD_NOT_SPECIFIED){}
		//----------------------//
	};

public:
	ATOM_WidgetParameters (void);
	virtual ~ATOM_WidgetParameters (void);

public:
	virtual void resetParameters (void);
	virtual void apply (ATOM_Widget *widget);
	virtual void saveToXML (ATOM_TiXmlElement *xml) const;
	virtual void loadFromXML (ATOM_TiXmlElement *xml);
	//--- wangjian modified ---//
	// 异步加载：添加加载优先级
	virtual ATOM_Widget *createWidget (ATOM_Widget *parent, int loadPriority) = 0;
	//-------------------------//
public:
	const ATOM_Rect2Di &rect (void) const;
	ATOM_Rect2Di &rect (void);
	int id (void) const;
	int &id (void);
	int imageId (void) const;
	int &imageId (void);
	int style (void) const;
	int &style (void);
	int borderMode (void) const;
	int &borderMode (void);
	int dragable (void) const;
	int &dragable (void);
	int dropable (void) const;
	int &dropable (void);
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
	ATOM_ColorARGB fontOutLineColor (void) const;
	ATOM_ColorARGB &fontOutLineColor (void);
	const ATOM_MAP<int,ImageInfo> &imageList (void) const;
	ATOM_MAP<int,ImageInfo> &imageList (void);

	// wangjian added
	int waitingCheck (void) const;
	int &waitingCheck (void);

protected:
	ATOM_Rect2Di _rect;
	int _id;
	int _imageId;
	int _style;
	int _borderMode;
	int _dragMode;
	ATOM_WidgetLayout::Type _layoutType;
	int _layoutSpaces[4];
	int _layoutGap;
	int _dragable;
	int _dropable;
	int _disabled;
	int _scrollBarBackgroundImageId;
	int _scrollBarDownButtonImageId;
	int _scrollBarSliderImageId;
	int _scrollBarUpButtonImageId;
	ATOM_STRING _font;
	ATOM_STRING _name;
	ATOM_ColorARGB _fontColor;
	ATOM_ColorARGB _fontOutLineColor;
	ATOM_MAP<int,ImageInfo> _imagelist;
	ATOM_HASHMAP<ATOM_STRING, ATOM_Variant> _properties;

	// wangjian added
	int	_waitingCheck;


};

class ATOM_ENGINE_API ATOM_WidgetPreloadedParameters
{
public:
	ATOM_WidgetPreloadedParameters (void);
	virtual ~ATOM_WidgetPreloadedParameters (void);

public:
	bool load (const char *filename);
	int getLoadPriority (void) const;
	ATOM_WidgetParameters *getParameters (void) const;
	int getNumChildParameters (void) const;
	ATOM_WidgetPreloadedParameters *getChildParameters (int n) const;

private:
	bool load_r (ATOM_WidgetPreloadedParameters *param, ATOM_TiXmlElement *root);

private:
	int asyncload;
	ATOM_WidgetParameters *_parameters;
	ATOM_VECTOR<ATOM_WidgetPreloadedParameters*> _childParameters;
};

////////////////////////////////////////////////////////////////////////////////////////

class ATOM_ENGINE_API ATOM_ScrollMapParameters: public ATOM_WidgetParameters
{
public:
	virtual ATOM_Widget *createWidget (ATOM_Widget *parent, int loadPriority);	// wangjian modified
};

/////////////////////////////////////////////////////////////////////////////////////////

class ATOM_ENGINE_API ATOM_CellParameters: public ATOM_WidgetParameters
{
public:
	ATOM_CellParameters (void);
	virtual void resetParameters (void);
	virtual void apply (ATOM_Widget *widget);
	virtual void saveToXML (ATOM_TiXmlElement *xml) const;
	virtual void loadFromXML (ATOM_TiXmlElement *xml);
	virtual ATOM_Widget *createWidget (ATOM_Widget *parent, int loadPriority);	// wangjian modified

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
	int						_subscriptImageId;		//!< 下标图片
	ATOM_Rect2Di			_subscriptImageRect;	//!< 下标图片位置
	ATOM_Point2Di			_subscriptTextPosition;	//!< 下标字体位置
//	ATOM_ColorARGB _outlineColor;
};

//////////////////////////////////////////////////////////////////////////////////////////

class ATOM_ENGINE_API ATOM_DialogParameters: public ATOM_WidgetParameters
{
public:
	ATOM_DialogParameters (void);
	virtual void resetParameters (void);
	virtual ATOM_Widget *createWidget (ATOM_Widget *parent, int loadPriority);	// wangjian modified
};

///////////////////////////////////////////////////////////////////////////////////////////

class ATOM_ENGINE_API ATOM_EditParameters: public ATOM_WidgetParameters
{
public:
	ATOM_EditParameters (void);
	virtual void resetParameters (void);
	virtual void apply (ATOM_Widget *widget);
	virtual void saveToXML (ATOM_TiXmlElement *xml) const;
	virtual void loadFromXML (ATOM_TiXmlElement *xml);
	virtual ATOM_Widget *createWidget (ATOM_Widget *parent, int loadPriority);	// wangjian modified

private:
	int _readonly;
	int _password;
	int _iNumber;
	int _fNumber;
	int _cursorImageId;
	int _cursorWidth;
	int _cursorOffset;
	int _maxChar;
	int _offsetX;
	int _frontImage;
	ATOM_STRING _text;
};

//////////////////////////////////////////////////////////////////////////////////////////

class ATOM_ComboBoxParameters: public ATOM_WidgetParameters
{
public:
	ATOM_ComboBoxParameters (void);
	virtual void resetParameters (void);
	virtual void apply (ATOM_Widget *widget);
	virtual void saveToXML (ATOM_TiXmlElement *xml);
	virtual void loadFromXML (ATOM_TiXmlElement *xml);
	virtual ATOM_Widget *createWidget (ATOM_Widget *parent, int loadPriority);	// wangjian modified

protected:
	int _buttonWidth;
	int _buttonImageId;
	int _buttonBorderMode;
	int _dropdownHeight;
	int _dropdownImageId;
	int _dropdownItemHeight;
	int _dropDownHilightImageId;
	int _dropDownBorderMode;
	int _editImageId;
	int _editBorderMode;
	int _offsetX;
	int _buttonLength;
	int _sliderLength;
	int _sliderWidth;
};

//////////////////////////////////////////////////////////////////////////////////////////

class ATOM_RealtimeCtrlParameters: public ATOM_WidgetParameters
{
public:
	ATOM_RealtimeCtrlParameters (void);
	virtual void resetParameters (void);
	virtual void apply (ATOM_Widget *widget);
	virtual void saveToXML (ATOM_TiXmlElement *xml);
	virtual void loadFromXML (ATOM_TiXmlElement *xml);
	virtual ATOM_Widget *createWidget (ATOM_Widget *parent, int loadPriority);	// wangjian modified

protected:
	int _transparent;
};

//////////////////////////////////////////////////////////////////////////////////////////

class ATOM_ENGINE_API ATOM_ListBoxParameters: public ATOM_WidgetParameters
{
public:
	ATOM_ListBoxParameters (void);
	virtual void resetParameters (void);
	virtual void apply (ATOM_Widget *widget);
	virtual void saveToXML (ATOM_TiXmlElement *xml) const;
	virtual void loadFromXML (ATOM_TiXmlElement *xml);
	virtual ATOM_Widget *createWidget (ATOM_Widget *parent, int loadPriority);	// wangjian modified

protected:
	int _lineHeight;
	int _selectedImageId;
	int _hoverImageId;
};

//////////////////////////////////////////////////////////////////////////////////////////

class ATOM_ENGINE_API ATOM_TreeCtrlParameters: public ATOM_WidgetParameters
{
public:
	ATOM_TreeCtrlParameters (void);
	virtual void resetParameters (void);
	virtual void apply (ATOM_Widget *widget);
	virtual void saveToXML (ATOM_TiXmlElement *xml) const;
	virtual void loadFromXML (ATOM_TiXmlElement *xml);
	virtual ATOM_Widget *createWidget (ATOM_Widget *parent, int loadPriority);	// wangjian modified

protected:
	int _imageSize;
	int _lineHeight;
	int _indent;
	int _textImageId;
//	ATOM_ColorARGB _outlineColor;
};

//////////////////////////////////////////////////////////////////////////////////////////

class ATOM_ENGINE_API ATOM_ListViewParameters: public ATOM_WidgetParameters
{
public:
	ATOM_ListViewParameters (void);
	virtual void resetParameters (void);
	virtual void apply (ATOM_Widget *widget);
	virtual void saveToXML (ATOM_TiXmlElement *xml) const;
	virtual void loadFromXML (ATOM_TiXmlElement *xml);
	virtual ATOM_Widget *createWidget (ATOM_Widget *parent, int loadPriority);	// wangjian modified

protected:
	int _columnCount;
	int _rowCount;
	int _columnWidth;
	int _itemHeight;
	int _selectedImage;
	int _hoverImage;
	int _singleBackGroundImage;

	int _buttonLength; //! 滚动条按钮长度
	int _sliderLength; //! 滚动块长度
	int _sliderWidth;  //! 滚动条宽度
};

///////////////////////////////////////////////////////////////////////////////////////////////

class ATOM_ENGINE_API ATOM_MultieditParameters: public ATOM_WidgetParameters
{
public:
	ATOM_MultieditParameters (void);
	virtual void resetParameters (void);
	virtual void apply (ATOM_Widget *widget);
	virtual void saveToXML (ATOM_TiXmlElement *xml) const;
	virtual void loadFromXML (ATOM_TiXmlElement *xml);
	virtual ATOM_Widget *createWidget (ATOM_Widget *parent, int loadPriority);	// wangjian modified

private:
	int _readonly;
	int _lineHeight;
	int _cursorImageId;
	int _cursorWidth;
	int _cursorOffset;
	int _maxChar;
	ATOM_STRING _text;
	int _frontimage;
	int _buttonLength; //! 滚动条按钮长度
	int _sliderLength; //! 滚动块长度
	int _sliderWidth;  //! 滚动条宽度
};

//////////////////////////////////////////////////////////////////////////////////////////////

class ATOM_ENGINE_API ATOM_PanelParameters: public ATOM_WidgetParameters
{
public:
	ATOM_PanelParameters (void);
	virtual void resetParameters (void);
	virtual void apply (ATOM_Widget *widget);
	virtual void saveToXML (ATOM_TiXmlElement *xml) const;
	virtual void loadFromXML (ATOM_TiXmlElement *xml);
	virtual ATOM_Widget *createWidget (ATOM_Widget *parent, int loadPriority);	// wangjian modified

private:
	bool _eventTransition;
};

//////////////////////////////////////////////////////////////////////////////////////////////

class ATOM_ENGINE_API ATOM_MarqueeParameters: public ATOM_WidgetParameters
{
public:
	ATOM_MarqueeParameters (void);
	virtual void resetParameters (void);
	virtual void apply (ATOM_Widget *widget);
	virtual void saveToXML (ATOM_TiXmlElement *xml) const;
	virtual void loadFromXML (ATOM_TiXmlElement *xml);
	virtual ATOM_Widget *createWidget (ATOM_Widget *parent, int loadPriority);	// wangjian modified

private:
	MARQUEEDIR _dir;
	float _speed;
	float _offset;
	float _position;
	ATOM_STRING _text;
};

//////////////////////////////////////////////////////////////////////////////////////////////

class ATOM_ENGINE_API ATOM_LabelParameters: public ATOM_PanelParameters
{
public:
	ATOM_LabelParameters (void);
	virtual void resetParameters (void);
	virtual void apply (ATOM_Widget *widget);
	virtual void saveToXML (ATOM_TiXmlElement *xml) const;
	virtual void loadFromXML (ATOM_TiXmlElement *xml);
	virtual ATOM_Widget *createWidget (ATOM_Widget *parent, int loadPriority);	// wangjian modified

protected:
	ATOM_STRING _text;
	int _alignmentX;
	int _alignmentY;
	int _textOffsetX;
	int _textOffsetY;
	ATOM_ColorARGB _color;
	//	ATOM_ColorARGB _outlineColor;
	ATOM_ColorARGB _underlineColor;
	float _rotation;
};

////////////////////////////////////////////////////////////////////////////////////////

class ATOM_ENGINE_API ATOM_ButtonParameters: public ATOM_LabelParameters
{
public:
	ATOM_ButtonParameters (void);
	virtual void resetParameters (void);
	virtual void apply (ATOM_Widget *widget);
	virtual void saveToXML (ATOM_TiXmlElement *xml) const;
	virtual void loadFromXML (ATOM_TiXmlElement *xml);
	virtual ATOM_Widget *createWidget (ATOM_Widget *parent, int loadPriority);	// wangjian modified

private:
	int _clickSound;
	int _hoverSound;
};

//////////////////////////////////////////////////////////////////////////////////////////

class ATOM_ENGINE_API ATOM_CheckBoxParameters: public ATOM_LabelParameters
{
public:
	ATOM_CheckBoxParameters (void);
	virtual void resetParameters (void);
	virtual void apply (ATOM_Widget *widget);
	virtual void saveToXML (ATOM_TiXmlElement *xml) const;
	virtual void loadFromXML (ATOM_TiXmlElement *xml);
	virtual ATOM_Widget *createWidget (ATOM_Widget *parent, int loadPriority);	// wangjian modified

private:
	int _clickSound;
	int _hoverSound;
	int _checkedImageId;
	int _checked;
};

/////////////////////////////////////////////////////////////////////////////////////////

class ATOM_ENGINE_API ATOM_SpacerParameters: public ATOM_WidgetParameters
{
public:
	ATOM_SpacerParameters (void);
	virtual void resetParameters (void);
	virtual void apply (ATOM_Widget *widget);
	virtual void saveToXML (ATOM_TiXmlElement *xml) const;
	virtual void loadFromXML (ATOM_TiXmlElement *xml);
	virtual ATOM_Widget *createWidget (ATOM_Widget *parent, int loadPriority);	// wangjian modified
};

//////////////////////////////////////////////////////////////////////////////////////////////

class ATOM_ENGINE_API ATOM_ProgressBarParameters: public ATOM_LabelParameters
{
public:
	ATOM_ProgressBarParameters (void);
	virtual void resetParameters (void);
	virtual void apply (ATOM_Widget *widget);
	virtual void saveToXML (ATOM_TiXmlElement *xml) const;
	virtual void loadFromXML (ATOM_TiXmlElement *xml);
	virtual ATOM_Widget *createWidget (ATOM_Widget *parent, int loadPriority);	// wangjian modified

private:
	int _frontImageId;
	int _minValue;
	int _maxValue;
	int _currentValue;
	bool _drawText;
	bool _isHorizontal;
};

//////////////////////////////////////////////////////////////////////////////////////////////

class ATOM_ENGINE_API ATOM_RichEditParameters: public ATOM_WidgetParameters
{
public:
	ATOM_RichEditParameters (void);
	virtual void resetParameters (void);
	virtual void apply (ATOM_Widget *widget);
	virtual void saveToXML (ATOM_TiXmlElement *xml) const;
	virtual void loadFromXML (ATOM_TiXmlElement *xml);
	virtual ATOM_Widget *createWidget (ATOM_Widget *parent, int loadPriority);	// wangjian modified

protected:
	int _maxHeight;
	int _autoSize;
	int _lineHeight;
	int _buttonLength;
	int _sliderLength;
	int _sliderWidth;
	//ATOM_ColorARGB _outlineColor;
};

//////////////////////////////////////////////////////////////////////////////////////////////

class ATOM_ENGINE_API ATOM_ScrollBarParameters: public ATOM_WidgetParameters
{
public:
	ATOM_ScrollBarParameters (void);
	virtual void resetParameters (void);
	virtual void apply (ATOM_Widget *widget);
	virtual void saveToXML (ATOM_TiXmlElement *xml) const;
	virtual void loadFromXML (ATOM_TiXmlElement *xml);
	virtual ATOM_Widget *createWidget (ATOM_Widget *parent, int loadPriority);	// wangjian modified

protected:
	int _SliderImageId;
	int _UpButtonImageId;
	int _DownButtonImageId;
	int _placement;
	int _buttonWidth;
	int _handleWidth;
	int _minValue;
	int _maxValue;
	int _stepValue;
};

//////////////////////////////////////////////////////////////////////////////////////////////

class ATOM_ENGINE_API ATOM_WindowParameters: public ATOM_WidgetParameters
{
public:
	ATOM_WindowParameters (void);
	virtual void resetParameters (void);
	virtual ATOM_Widget *createWidget (ATOM_Widget *parent, int loadPriority);	// wangjian modified
};

///////////////////////////////////////////////////////////////////////////////////////////////

class ATOM_ENGINE_API ATOM_ViewStackParameters: public ATOM_PanelParameters
{
public:
	ATOM_ViewStackParameters (void);
	virtual void resetParameters (void);
	virtual void apply (ATOM_Widget *widget);
	virtual void saveToXML (ATOM_TiXmlElement *xml);
	virtual void loadFromXML (ATOM_TiXmlElement *xml);
	virtual ATOM_Widget *createWidget (ATOM_Widget *parent, int loadPriority);	// wangjian modified

private:
	int _activePage;
};

///////////////////////////////////////////////////////////////////////////////////////////////

class ATOM_ENGINE_API ATOM_ComponentParameters: public ATOM_WidgetParameters
{
public:
	ATOM_ComponentParameters (void);
	virtual void resetParameters (void);
	virtual void apply (ATOM_Widget *widget);
	virtual void saveToXML (ATOM_TiXmlElement *xml);
	virtual void loadFromXML (ATOM_TiXmlElement *xml);
	virtual ATOM_Widget *createWidget (ATOM_Widget *parent, int loadPriority);	// wangjian modified

private:
	ATOM_STRING _filename;
};

//////////////////////////////////////////////////////////////////////////////////////////

class ATOM_ENGINE_API ATOM_FlashParameters: public ATOM_WidgetParameters
{
public:
	ATOM_FlashParameters (void);
	virtual void resetParameters (void);
	virtual void apply (ATOM_Widget *widget);
	virtual void saveToXML (ATOM_TiXmlElement *xml) const;
	virtual void loadFromXML (ATOM_TiXmlElement *xml);
	virtual ATOM_Widget *createWidget (ATOM_Widget *parent, int loadPriority);	// wangjian modified

private:
	ATOM_STRING _filename;
	ATOM_STRING _initFunc;
	ATOM_STRING _clearFunc;
	ATOM_STRING _activateFunc;
	ATOM_STRING _deactivateFunc;
	unsigned _FPS;
};

//////////////////////////////////////////////////////////////////////////////////////////

class ATOM_ENGINE_API ATOM_CurveEditorParameters: public ATOM_WidgetParameters 
{
public:
	ATOM_CurveEditorParameters (void);
	virtual void resetParameters (void);
	virtual void apply (ATOM_Widget *widget);
	virtual void saveToXML (ATOM_TiXmlElement *xml) const;
	virtual void loadFromXML (ATOM_TiXmlElement *xml);
	//--- wangjian modified ---//
	// 添加一个加载优先级
	virtual ATOM_Widget *createWidget (ATOM_Widget *parent, int loadPriority);
	//-------------------------//

private:
	unsigned _timeAxisUnitLength;
	float _timeAxisUnitResolution;
	unsigned _valueAxisUnitLength;
	float _valueAxisUnitResolution;
	unsigned _curveResolution;
	float _originTimeAxis;
	float _originValueAxis;
	bool _enableScroll;
	ATOM_ColorARGB _curveColor;
};


#endif // __ATOM3D_ENGINE_WIDGET_IO_H
