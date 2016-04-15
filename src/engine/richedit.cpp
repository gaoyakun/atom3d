#include "stdafx.h"
#include "richedit.h"
#include "label.h"
#include "hyperlink.h"
#include "gui_canvas.h"
 ATOM_MAP<ATOM_STRING,unsigned> ATOM_RichEdit::_colorConfigMap;
// 默认的创建函数
void static DefaultCreateWidget(ATOM_TiXmlElement* elem, ATOM_RichEdit* parent)
{
	ATOM_STACK_TRACE(DefaultCreateWidget);

	if(NULL == elem)
	{
		return;
	}

	const char *a = elem->Attribute("valign");
	int align = ATOM_Widget::AlignY_Top;
	if (a)
	{
		if (!stricmp(a, "center")) 
			align = ATOM_Widget::AlignY_Middle;
		else if (!stricmp(a, "bottom")) 
			align = ATOM_Widget::AlignY_Bottom;
	}

	if(0 == strcmp(elem->Value(), "t") )
	{	// 创建文本数据
		const char* str = elem->Attribute("v");
		int val;
		unsigned color = elem->Attribute("color", &val) ? (unsigned)val : (unsigned)parent->getFontColor();
		color |= 0xFF000000;
		bool nl = (elem->Attribute("enable", &val) && !val) ? false : true;
		bool pickable = (elem->Attribute("pickable", &val) && !val) ? false : true;
		unsigned outline = elem->Attribute("outlinecolor", &val) ? val : 0;
		unsigned underline = elem->Attribute("underlinecolor", &val) ? val : 0;

		parent->addStringEx(str, color, align, nl, pickable, NULL, outline, underline);
	}
	else if(0 == strcmp(elem->Value(), "n") )
	{	// 创建换行
		parent->addNewline();
	}
	else if(0 == strcmp(elem->Value(), "i") )
	{	// 创建图片
		int imageId = 0;
		if( NULL == elem->Attribute("v", &imageId) )
		{
			return;
		}
		int val;
		int id = elem->Attribute("id", &id) ? id : -1;
		int w = elem->Attribute("w", &w) ? w : parent->getLineHeight();
		int h = elem->Attribute("h", &h) ? h : parent->getLineHeight();
		bool nl = (elem->Attribute("enable", &val) && !val) ? false : true;
		bool pickable = (elem->Attribute("pickable", &val) && !val) ? false : true;
		parent->addImage(imageId, w, h, id, nl, pickable);
	}
	else if(0 == strcmp(elem->Value(), "h") )
	{	// 创建超链
		const char* str = elem->Attribute("v");

		int id = elem->Attribute("id", &id) ? id : ATOM_Widget::AnyId;

		int val;
		unsigned normal = elem->Attribute("normal", &val) ? val : 0xFF000000;
		normal |= 0xFF000000;
		unsigned hover = elem->Attribute("hover", &val) ? val : 0xFF000000;
		hover |= 0xFF000000;
		int clickSound = elem->Attribute("clicksound", &val) ? val : INVALID_AUDIOID;
		int hoverSound = elem->Attribute("hoversound", &val) ? val : INVALID_AUDIOID;
		bool nl = (elem->Attribute("enable", &val) && !val) ? false : true;
		bool pickable = (elem->Attribute("pickable", &val) && !val) ? false : true;
		unsigned underline = elem->Attribute("underlinecolor", &val) ? val : 0;

		parent->addHyperLink(str, normal, hover, id, elem->Attribute("lua"),underline,clickSound, hoverSound, nl, pickable);
	}
	else if(0 == strcmp(elem->Value(), "b") )
	{ // 创建按钮
		const char *str = elem->Attribute("t");
		int imageId = 0;
		elem->Attribute("v", &imageId);
		int val;
		int id = elem->Attribute("id", &id) ? id : ATOM_Widget::AnyId;
		int w = elem->Attribute("w", &w) ? w : parent->getLineHeight();
		int h = elem->Attribute("h", &h) ? h : parent->getLineHeight();
		bool nl = (elem->Attribute("enable", &val) && !val) ? false : true;
		bool pickable = (elem->Attribute("pickable", &val) && !val) ? false : true;
		unsigned outlinecolor = elem->Attribute("outlinecolor", &val) ? val : 0;
		unsigned color = elem->Attribute("color", &val) ? (unsigned)val : (unsigned)parent->getFontColor();
		int alignText = elem->Attribute("align", &val) ? (unsigned)val : (ATOM_Widget::AlignX_Middle|ATOM_Widget::AlignY_Middle);
		parent->addButton(str, imageId, w, h, id, align, elem->Attribute("lua"),outlinecolor,color|0xFF000000,alignText,nl, pickable);
	}
	else if(0 == strcmp(elem->Value(), "c") )
	{ 
		// 创建格子控件
		int imageId = 0;
		elem->Attribute("v", &imageId);
		int val;
		int id = elem->Attribute("id", &id) ? id : ATOM_Widget::AnyId;
		int w = elem->Attribute("w", &w) ? w : parent->getLineHeight();
		int h = elem->Attribute("h", &h) ? h : parent->getLineHeight();
		int xSize = elem->Attribute("x", &xSize) ? xSize :1;
		int ySize = elem->Attribute("y", &ySize) ? ySize :1;
		int frameOffset = elem->Attribute("offset", &frameOffset) ? frameOffset :0;
		bool pickable = (elem->Attribute("pickable", &val) && !val) ? false : true;
		parent->addCell(imageId,w,h,id,xSize,ySize,frameOffset,pickable);
	}
}
//////////////////////////////////////////////////////////////////////////

bool ATOM_RichEdit::loadRichEditColor(const char * strRichEditColorFile)
{
	ATOM_AutoFile f(strRichEditColorFile, ATOM_VFS::read|ATOM_VFS::text);
	if (!f)
		return false;
	unsigned size = f->size ();
	char *p = ATOM_NEW_ARRAY(char, size);
	size = f->read (p, size);
	p[size] = '\0';
	ATOM_TiXmlDocument doc;
	doc.Parse (p);
	ATOM_DELETE_ARRAY(p);
	if (doc.Error())
		return false;
	ATOM_TiXmlElement *root = doc.RootElement ();
	if (!root)
		return false;
	for (ATOM_TiXmlElement *clr = root->FirstChildElement ("clr"); clr; clr = clr->NextSiblingElement("clr"))
	{
		const char *name = clr->Attribute ("name");
		if (!name)
			continue;
		int color = 0;
		clr->QueryIntAttribute ("color", &color);
		_colorConfigMap.insert(ATOM_MAP<ATOM_STRING,unsigned>::value_type(name,color));
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////

ATOM_BEGIN_EVENT_MAP(ATOM_RichEdit, ATOM_Widget)
	ATOM_EVENT_HANDLER(ATOM_RichEdit, ATOM_WidgetDrawClientEvent, onPaint)
	ATOM_EVENT_HANDLER(ATOM_RichEdit, ATOM_WidgetCommandEvent, onCommand)
ATOM_END_EVENT_MAP

ATOM_RichEdit::ATOM_RichEdit()
{
	ATOM_STACK_TRACE(ATOM_RichEdit::ATOM_RichEdit);

	_lineHeight = 0;
	_maxHeight = 0;
	_createREWidgetFunc = DefaultCreateWidget;
	_autoSize = false;
	_currentColor = getFontColor();
#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif
}

ATOM_RichEdit::ATOM_RichEdit (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, int lineHeight, ATOM_Widget::ShowState showState)
: ATOM_Widget (parent, rect, style|ATOM_Widget::Control|ATOM_Widget::ClipChildren|ATOM_Widget::NoFocus, id, showState)
{
	ATOM_STACK_TRACE(ATOM_RichEdit::ATOM_RichEdit);

	ATOM_ASSERT(parent);
	ATOM_ASSERT(getRenderer());

//	_cursorLine = 0;
	_lineHeight = lineHeight;
	_maxHeight = 0;
	_createREWidgetFunc = DefaultCreateWidget;
	_autoSize = false;
	_currentColor = getFontColor();
	resize (rect);

#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif
}

ATOM_RichEdit::~ATOM_RichEdit (void)
{
	ATOM_STACK_TRACE(ATOM_RichEdit::~ATOM_RichEdit);
	clear();
}

ATOM_WidgetType ATOM_RichEdit::getType (void) const
{
	return WT_RICHEDIT;
}

void ATOM_RichEdit::setOutlineColor(ATOM_ColorARGB color)
{
	ATOM_STACK_TRACE(ATOM_RichEdit::setOutlineColor);

	_widgetFontOutLineColor = color;
}

ATOM_ColorARGB ATOM_RichEdit::getOutlineColor() const
{
	ATOM_STACK_TRACE(ATOM_RichEdit::getOutlineColor);

	return _widgetFontOutLineColor;
}

int ATOM_RichEdit::getLineHeight() const
{
	ATOM_STACK_TRACE(ATOM_RichEdit::getLineHeight);

	return _lineHeight;
}

void ATOM_RichEdit::addXml(const char* str)
{
	ATOM_STACK_TRACE(ATOM_RichEdit::addXml);

	if (!str)
	{
		return;
	}

	// 添加元素
	ATOM_TiXmlDocument doc;
	doc.Parse(str);
	if (doc.Error ())
	{
		ATOM_LOGGER::error ("%s(%s) %s\n", __FUNCTION__, str, doc.ErrorDesc());
		return;
	}

	ATOM_TiXmlElement* elem = doc.FirstChildElement();
	if(!strcmp(elem->Value(),"x"))
	{
		elem = elem->NextSiblingElement();
		_currentColor = getFontColor();
		for(; elem; elem = elem->NextSiblingElement())
		{
			pasreRichEdit(elem);
		}
	}
	else
	{
		for(; elem; elem = elem->NextSiblingElement())
		{
			_createREWidgetFunc(elem, this);
		}
	}


	// 删除多余的行
	if(_maxHeight > 0)
	{
		RichEditLine* line = _lineList.empty() ? NULL : &_lineList.back();
		while(line && line->_top + line->_height >_maxHeight)
		{
			deleteLine(0);
			line = _lineList.empty() ? NULL : &_lineList.back();
		}
	}

	if(_autoSize)
	{
		// 重新计算canvas大小
		ATOM_Size2Di canvas(0, 0);
		if(!_lineList.empty())
		{
			canvas.h = _lineList.back()._top + _lineList.back()._height;
		}
		canvas.w = _widgetRect.size.w;
		resize(ATOM_Rect2Di(_widgetRect.point, canvas));
		setCanvasSize (canvas);
	}
	else
	{
		// 重新计算canvas大小
		ATOM_Size2Di canvas(0, 0);
		if(!_lineList.empty())
		{
			canvas.h = _lineList.back()._top + _lineList.back()._height;
		}
		canvas.w = ATOM_max2(_widgetRect.size.w, canvas.w);
		canvas.h = ATOM_max2(_widgetRect.size.h, canvas.h);
		setCanvasSize (canvas);
	}


	if (getShowState () != ATOM_Widget::Hide)
	{
		invalidate ();
	}
}

void ATOM_RichEdit::setXml(const char* str)
{
	ATOM_STACK_TRACE(ATOM_RichEdit::setXml);

	clear();
	addXml(str);
}

void ATOM_RichEdit::addStringImpl (const char* str, unsigned color, int align, bool nl, bool pickable, ATOM_GUIFont::handle font, unsigned clrOutline, unsigned clrUnderline)
{
	if(!str || !str[0])
	{
		return;
	}

	ATOM_RichEdit::LineList* lineList = getLineList();

	// 如果一行都没有，就放入一行
	if(lineList->empty())
	{
		lineList->push_back(RichEditLine(0, getLineHeight()));
	}
	RichEditLine* lastLine = &lineList->back();

	const char* startString = str;
	size_t strLen = strlen(str);

	int offsetX = (_verticalScrollBar && (_creationStyle & ATOM_Widget::LeftScroll)) ? _scrollBarWidth : 0;
	float scrollValueY = getScrollValue().y;
	ATOM_Widget* lastWidget = lastLine->_widgetList.empty() ? NULL : lastLine->_widgetList.back();
	int x = lastWidget ? lastWidget->getWidgetRect().point.x + lastWidget->getWidgetRect().size.w : offsetX;

	int widthMax = getClientRect().size.w-x;

	int l, t, w, h;
	if(NULL == font)
	{
		font = getFont();
	}
	ATOM_SetCharMargin (ATOM_GUIFont::getFontCharMargin (font));
	unsigned num = ATOM_ClipString(ATOM_GUIFont::getFontHandle(font), startString, widthMax, &l, &t, &w, &h);
	while(startString < str+strLen)
	{
		if(num > 0)
		{	// 如果还可以填一些字符串
			ATOM_CalcStringBounds(ATOM_GUIFont::getFontHandle(font), startString, num, &l, &t, &w, &h);
			ATOM_Label* label = ATOM_NEW(ATOM_Label, this, ATOM_Rect2Di(x, lastLine->_top-scrollValueY, w, getLineHeight()), int(ATOM_Widget::NoFocus|ATOM_Widget::NonLayoutable), int(ATOM_Widget::AnyId), ATOM_Widget::ShowNormal);
			ATOM_STRING s(startString, num);
			label->setWheelDeliver(true);
			label->setFont(font);
			label->setFontColor(color);
			label->setClientImageId(-1);
			label->setText(s.c_str());
			label->setEventTrigger(_eventTrigger.trigger);
			label->enable(nl);
			label->setPickable(pickable);
			label->setOutlineColor(clrOutline ? clrOutline : (unsigned)_widgetFontOutLineColor);
			label->setUnderlineColor(clrUnderline);
			//label->setAlign(ATOM_Widget::AlignX_Middle|align);
			lastLine->_widgetList.push_back(label);

			startString += num;
		}
		else if(0 == num && widthMax == getClientRect().size.w)
		{
			break;
		}

		// 如果都解析完毕了
		if(startString >= str+strLen)
		{
			break;
		}

		lineList->push_back(RichEditLine(lastLine->_top+lastLine->_height, getLineHeight()));
		lastLine = &lineList->back();
		x = offsetX;
		widthMax = getClientRect().size.w;

		num = ATOM_ClipString(ATOM_GUIFont::getFontHandle(font), startString, widthMax, &l, &t, &w, &h);
	}
}

void ATOM_RichEdit::addString(const char* str, unsigned color, bool nl, bool pickable, ATOM_GUIFont::handle font, unsigned clrOutline, unsigned clrUnderline)
{
	addStringImpl (str, color, ATOM_Widget::AlignY_Middle, nl, pickable, font, clrOutline, clrUnderline);
}

void ATOM_RichEdit::addStringEx(const char* str, unsigned color, int align, bool nb, bool pickable, ATOM_GUIFont::handle font, unsigned clrOutline, unsigned clrUnderline)
{
	addStringImpl (str, color, align, nb, pickable, font, clrOutline, clrUnderline);
}

void ATOM_RichEdit::addNewline()
{
	ATOM_STACK_TRACE(ATOM_RichEdit::addNewline);

	ATOM_RichEdit::LineList* lineList = getLineList();
	if(lineList->empty())
	{
		lineList->push_back(RichEditLine(0, getLineHeight()));
	}
	else
	{
		RichEditLine* lastLine = &lineList->back();
		lineList->push_back(RichEditLine(lastLine->_top + lastLine->_height, getLineHeight()));
	}
}

void ATOM_RichEdit::addImage(int imageId, int w, int h, int id, bool nl, bool pickable)
{
	ATOM_STACK_TRACE(ATOM_RichEdit::addImage);

	ATOM_RichEdit::LineList* lineList = getLineList();

	// 如果一行都没有，就放入一行
	if(lineList->empty())
	{
		lineList->push_back(RichEditLine(0, getLineHeight()));
	}
	RichEditLine* lastLine = &lineList->back();

	int offsetX = (_verticalScrollBar && (_creationStyle & ATOM_Widget::LeftScroll)) ? _scrollBarWidth : 0;
	float scrollValueY = getScrollValue().y;
	ATOM_Widget* lastWidget = lastLine->_widgetList.empty() ? NULL : lastLine->_widgetList.back();
	int x = lastWidget ? lastWidget->getWidgetRect().point.x + lastWidget->getWidgetRect().size.w : offsetX;
	int widthMax = getClientRect().size.w-x;

	// 如果这行显示不下
	if(widthMax < w)
	{
		lineList->push_back(RichEditLine(lastLine->_top+lastLine->_height, getLineHeight()));
		lastLine = &lineList->back();
		x = offsetX;
		widthMax = getClientRect().size.w;
	}

	//
	ATOM_Widget* widget = ATOM_NEW(ATOM_Widget, this, ATOM_Rect2Di(x, lastLine->_top-scrollValueY, w, h), ATOM_Widget::Control|ATOM_Widget::NoFocus|ATOM_Widget::NonLayoutable, id, ATOM_Widget::ShowNormal);
	widget->resize(ATOM_Rect2Di(x, lastLine->_top-scrollValueY, w, h));
	widget->setClientImageId(imageId);
	widget->setEventTrigger(_eventTrigger.trigger);
	widget->enable(nl);
	widget->setPickable(pickable);
	widget->setWheelDeliver(true);
	lastLine->_widgetList.push_back(widget);

	// 图片有可能很大
	if(lastLine->_height < h)
	{
		lastLine->_height = h;
	}
}

void ATOM_RichEdit::addImageEx(int imageId, int w, int h, const char* str, int align, unsigned color, ATOM_GUIFont::handle font /*= NULL*/, int id /*= ATOM_Widget::AnyId*/, bool nb /*= true*/, bool pickable /*= true*/)
{
	if(!str || !str[0])
	{
		return;
	}
	ATOM_RichEdit::LineList* lineList = getLineList();

	// 如果一行都没有，就放入一行
	if(lineList->empty())
	{
		lineList->push_back(RichEditLine(0, getLineHeight()));
	}
	RichEditLine* lastLine = &lineList->back();

	int offsetX = _verticalScrollBar && (_creationStyle & ATOM_Widget::LeftScroll) ? _scrollBarWidth : 0;
	float scrollValueY = getScrollValue().y;
	ATOM_Widget* lastWidget = lastLine->_widgetList.empty() ? NULL : lastLine->_widgetList.back();
	int x = lastWidget ? lastWidget->getWidgetRect().point.x + lastWidget->getWidgetRect().size.w : offsetX;
	int widthMax = getClientRect().size.w-x;	

	// 如果这行显示不下
	if(widthMax < w)
	{
		lineList->push_back(RichEditLine(lastLine->_top+lastLine->_height, getLineHeight()));
		lastLine = &lineList->back();
		x = offsetX;
		widthMax = getClientRect().size.w;
	}

	// 
	ATOM_Label* label = ATOM_NEW(ATOM_Label, this, ATOM_Rect2Di(x, lastLine->_top-scrollValueY, w, h), ATOM_Widget::Control|ATOM_Widget::NoFocus|ATOM_Widget::NonLayoutable, id, ATOM_Widget::ShowNormal);
	label->setClientImageId(imageId);
	label->setWheelDeliver(true);
	label->setText(str);
	label->setAlign(align);
	label->setFont(font);
	label->setFontColor(color);
	label->setEventTrigger(_eventTrigger.trigger);
	label->enable(nb);
	label->setPickable(pickable);
	lastLine->_widgetList.push_back(label);

	// 图片有可能很大
	if(lastLine->_height < h)
	{
		lastLine->_height = h;
	}
}

void ATOM_RichEdit::addButton(const char* str, int imageId, int w, int h, int id, const char *param, bool nb, bool pickable)
{
	addButtonImpl (str, imageId, w, h, id, 0, param,0,0,0,nb, pickable);
}

void ATOM_RichEdit::addButtonImpl (const char* str, int imageId, int w, int h, int id, int yoff, const char *param, unsigned outlinecolor,unsigned textcolor,int alignText, bool nb, bool pickable)
{
	ATOM_STACK_TRACE(ATOM_RichEdit::addImage);

	ATOM_RichEdit::LineList* lineList = getLineList();

	// 如果一行都没有，就放入一行
	if(lineList->empty())
	{
		lineList->push_back(RichEditLine(0, getLineHeight()));
	}
	RichEditLine* lastLine = &lineList->back();

	int offsetX = (_verticalScrollBar && (_creationStyle & ATOM_Widget::LeftScroll)) ? _scrollBarWidth : 0;
	float scrollValueY = getScrollValue().y;
	ATOM_Widget* lastWidget = lastLine->_widgetList.empty() ? NULL : lastLine->_widgetList.back();
	int x = lastWidget ? lastWidget->getWidgetRect().point.x + lastWidget->getWidgetRect().size.w : offsetX;
	int widthMax = getClientRect().size.w-x;

	// 如果这行显示不下
	if(widthMax < w)
	{
		lineList->push_back(RichEditLine(lastLine->_top+lastLine->_height, getLineHeight()));
		lastLine = &lineList->back();
		x = offsetX;
		widthMax = getClientRect().size.w;
	}

	//
	ATOM_Button* widget = ATOM_NEW(ATOM_Button, this, ATOM_Rect2Di(x, yoff+lastLine->_top-scrollValueY, w, h), ATOM_Widget::Control|ATOM_Widget::NoFocus|ATOM_Widget::NonLayoutable, id, ATOM_Widget::ShowNormal);
	widget->setClientImageId(imageId);
	widget->setEventTrigger(_eventTrigger.trigger);
	widget->setWheelDeliver(true);
	widget->setText(str);
	widget->setFontOutLineColor(outlinecolor);
	widget->setFontColor(textcolor);
	widget->setAlign(alignText);
	if (param)
	{
		widget->setLua (param);
	}
	widget->enable(nb);
	widget->setPickable(pickable);
	lastLine->_widgetList.push_back(widget);

	// 图片有可能很大
	if(lastLine->_height < h)
	{
		lastLine->_height = h;
	}
}

void ATOM_RichEdit::addButton(const char* str, int imageId, int w, int h, int id, int align, const char *param,  unsigned outlinecolor,unsigned textcolor,int alignText,bool nb, bool pickable)
{
	int yoff = 0;
	switch (align)
	{
	case ATOM_Widget::AlignY_Middle:
		yoff = (_lineHeight - h) / 2;
		break;
	case ATOM_Widget::AlignY_Bottom:
		yoff = _lineHeight - h;
		break;
	case ATOM_Widget::AlignY_Top:
	default:
		yoff = 0;
	}

	addButtonImpl (str, imageId, w, h, id, yoff, param,outlinecolor,textcolor,alignText, nb, pickable);
}

void ATOM_RichEdit::addButton(const char* str, int imageId, int w, int h, int id, bool nb, bool pickable)
{
	addButton (str, imageId, w, h, id, NULL, nb, pickable);
}

void ATOM_RichEdit::addHyperLink(const char* str, unsigned normalColor, unsigned hoverColor, int id, int clickSound, int hoverSound, bool nl, bool pickable, unsigned clrOutline, unsigned clrUnderline)
{
	ATOM_STACK_TRACE(ATOM_RichEdit::addHyperLink);

	if(!str || !str[0])
	{
		return;
	}

	// 计算宽度
	ATOM_GUIFont::handle hfont = ATOM_GUIFont::ensureValidFont (getFont(), 12, 0);
	ATOM_FontHandle font = ATOM_GUIFont::getFontHandle (hfont);
	int charmargin = ATOM_GUIFont::getFontCharMargin (hfont);
	int charset = ATOM_GUIFont::getFontCharSet (hfont);
	ATOM_SetCharMargin (charmargin);
	int l, t, w, h;
	ATOM_CalcStringBounds (font, str, strlen(str), &l, &t, &w, &h);
	h = getLineHeight();

	ATOM_RichEdit::LineList* lineList = getLineList();

	// 如果一行都没有，就放入一行
	if(lineList->empty())
	{
		lineList->push_back(RichEditLine(0, getLineHeight()));
	}
	RichEditLine* lastLine = &lineList->back();

	int offsetX = (_verticalScrollBar && (_creationStyle & ATOM_Widget::LeftScroll)) ? _scrollBarWidth : 0;
	float scrollValueY = getScrollValue().y;
	ATOM_Widget* lastWidget = lastLine->_widgetList.empty() ? NULL : lastLine->_widgetList.back();
	int x = lastWidget ? lastWidget->getWidgetRect().point.x + lastWidget->getWidgetRect().size.w : offsetX;
	int widthMax = getClientRect().size.w-x;

	// 如果这行显示不下
	if(widthMax < w)
	{
		lineList->push_back(RichEditLine(lastLine->_top+lastLine->_height, getLineHeight()));
		lastLine = &lineList->back();
		x = offsetX;
		widthMax = getClientRect().size.w;
	}

	// 创建控件
	ATOM_HyperLink* hyperlink = ATOM_NEW(ATOM_HyperLink, this, ATOM_Rect2Di(x, lastLine->_top-scrollValueY, w, h), ATOM_Widget::Control|ATOM_Widget::NoFocus|ATOM_Widget::NonLayoutable, id, ATOM_Widget::ShowNormal);
	hyperlink->setFont(getFont());
	hyperlink->setWheelDeliver(true);
	hyperlink->setFontColor(normalColor);
	hyperlink->setHoverFontColor(hoverColor);
	hyperlink->setClientImageId(-1);
	hyperlink->setText(str);
	hyperlink->setEventTrigger(_eventTrigger.trigger);
	hyperlink->setClickSound(clickSound);
	hyperlink->setHoverSound(hoverSound);
	hyperlink->enable(nl);
	hyperlink->setPickable(pickable);
	hyperlink->setOutlineColor(clrOutline ? clrOutline : (unsigned)_widgetFontOutLineColor);
	hyperlink->setUnderlineColor(clrUnderline);
	lastLine->_widgetList.push_back(hyperlink);
}


// 高度限制
void ATOM_RichEdit::setMaxHeight(int maxHeight)
{
	ATOM_STACK_TRACE(ATOM_RichEdit::setMaxHeight);

	_maxHeight = maxHeight;
}

// 取得高度限制
int ATOM_RichEdit::getMaxHeight() const
{
	ATOM_STACK_TRACE(ATOM_RichEdit::getMaxHeight);

	return _maxHeight;
}

// 设置自动适应大小
void ATOM_RichEdit::setAutoSize(bool autoSize)
{
	ATOM_STACK_TRACE(ATOM_RichEdit::setAutoSize);

	_autoSize = autoSize;
}

// 是否自动适应大小
bool ATOM_RichEdit::getAutoSize() const
{
	ATOM_STACK_TRACE(ATOM_RichEdit::getAutoSize);

	return _autoSize;
}

void ATOM_RichEdit::clear()
{
	ATOM_STACK_TRACE(ATOM_RichEdit::clear);

	for(LineList::iterator itLine = _lineList.begin(); itLine != _lineList.end(); ++itLine)
	{
		RichEditLine& line = *itLine;
		for(RichEditLine::WidgetList::iterator itWidget = line._widgetList.begin(); itWidget != line._widgetList.end(); ++itWidget)
		{
			ATOM_DELETE(*itWidget);
		}
	}
	_lineList.clear();
	scroll2Top();
	setCanvasSize(_widgetRect.size);
}

void ATOM_RichEdit::deleteLine(int lineIndex)
{
	ATOM_STACK_TRACE(ATOM_RichEdit::deleteLine);

	if(lineIndex < 0 ||  lineIndex >= _lineList.size())
	{
		return;
	}

	LineList::iterator iter = _lineList.begin();
	std::advance(iter, lineIndex);
	RichEditLine& line = *iter;
	int h = line._height;

	for(RichEditLine::WidgetList::iterator itWidget = line._widgetList.begin(); itWidget != line._widgetList.end(); ++itWidget)
	{
		ATOM_DELETE(*itWidget);
	}

	iter = _lineList.erase(iter);
	for(; iter != _lineList.end(); ++iter)
	{
		RichEditLine& line = *iter;
		line._top -= h;

		for(RichEditLine::WidgetList::iterator itWidget = line._widgetList.begin(); itWidget != line._widgetList.end(); ++itWidget)
		{
			ATOM_Widget* w = (*itWidget);
			ATOM_Rect2Di rc = w->getWidgetRect();
			rc.point.y -= h;
			w->resize(rc);
		}
	}
}

void ATOM_RichEdit::scroll2Top()
{
	ATOM_STACK_TRACE(ATOM_RichEdit::scroll2Top);

	setScrollValue(ATOM_Point2Di(0, 0));
}

void ATOM_RichEdit::scroll2Bottom()
{
	ATOM_STACK_TRACE(ATOM_RichEdit::scroll2Bottom);

	const ATOM_Size2Di& canvas = getCanvasSize();
	float y = canvas.h - _widgetRect.size.h;
	setScrollValue(ATOM_Point2Di(0, (y < 0) ? 0 : y));
}

CreateREWidgetFunc ATOM_RichEdit::setCreateFunc(CreateREWidgetFunc func)
{
	ATOM_STACK_TRACE(ATOM_RichEdit::setCreateFunc);

	CreateREWidgetFunc oldFunc = _createREWidgetFunc;
	_createREWidgetFunc = func;

	return oldFunc;
}

void ATOM_RichEdit::onPaint (ATOM_WidgetDrawClientEvent *event)
{
	ATOM_STACK_TRACE(ATOM_RichEdit::onPaint);
	if(_verticalScrollBar)
	{
		if(_lineList.empty()||_widgetRect.size.h > _lineList.back()._top+_lineList.back()._height)
			_verticalScrollBar->show(ATOM_Widget::Hide);
		else
			_verticalScrollBar->show(ATOM_Widget::ShowNormal);
	}

	
	callParentHandler (event);
}

void ATOM_RichEdit::onCommand (ATOM_WidgetCommandEvent *event)
{
	ATOM_Widget *widget = getChildById (event->id);
	if (widget && (widget->getType() == WT_HYPERLINK || widget->getType() == WT_BUTTON))
	{
		getParent()->queueEvent (ATOM_NEW(ATOM_RichEditCommandEvent, getId(), widget->getId()), ATOM_APP);
	}
}

ATOM_RichEdit::LineList* ATOM_RichEdit::getLineList()
{
	ATOM_STACK_TRACE(ATOM_RichEdit::getLineList);

	return &_lineList;
}

bool ATOM_RichEdit::supportIME (void) const
{
	ATOM_STACK_TRACE(ATOM_RichEdit::supportIME);

	return true;
}

void ATOM_RichEdit::addHyperLink(const char* str, unsigned normalColor, unsigned hoverColor, int id, const char* pParam,unsigned clrUnderline, int clickSound, int hoverSound, bool nl, bool pickable)
{
	if(!str || !str[0])
	{
		return;
	}

	// 计算宽度
	ATOM_GUIFont::handle hfont = ATOM_GUIFont::ensureValidFont (getFont(), 12, 0);
	ATOM_FontHandle font = ATOM_GUIFont::getFontHandle (hfont);
	int charmargin = ATOM_GUIFont::getFontCharMargin (hfont);
	ATOM_SetCharMargin (charmargin);
	int l, t, w, h;
	ATOM_CalcStringBounds (font, str, strlen(str), &l, &t, &w, &h);
	h = getLineHeight();

	ATOM_RichEdit::LineList* lineList = getLineList();

	// 如果一行都没有，就放入一行
	if(lineList->empty())
	{
		lineList->push_back(RichEditLine(0, getLineHeight()));
	}
	RichEditLine* lastLine = &lineList->back();

	int offsetX = _verticalScrollBar && (_creationStyle & ATOM_Widget::LeftScroll) ? _scrollBarWidth : 0;
	float scrollValueY = getScrollValue().y;
	ATOM_Widget* lastWidget = lastLine->_widgetList.empty() ? NULL : lastLine->_widgetList.back();
	int x = lastWidget ? lastWidget->getWidgetRect().point.x + lastWidget->getWidgetRect().size.w : offsetX;
	int widthMax = getClientRect().size.w-x;	

	// 如果这行显示不下
	if(widthMax < w)
	{
		lineList->push_back(RichEditLine(lastLine->_top+lastLine->_height, getLineHeight()));
		lastLine = &lineList->back();
		x = offsetX;
		widthMax = getClientRect().size.w;
	}

	// 创建控件
	int nOffsetY = h >= lastLine->_height ? 0 : ((lastLine->_height - h) / 2);	// 居中偏移
	ATOM_HyperLink* pWidget = ATOM_NEW(ATOM_HyperLink, this, ATOM_Rect2Di(x, lastLine->_top - scrollValueY + nOffsetY, w, h), ATOM_Widget::Control|ATOM_Widget::NoFocus|ATOM_Widget::NonLayoutable, id, ATOM_Widget::ShowNormal);

	pWidget->SetLua(pParam);
	pWidget->setFont(getFont());
	pWidget->setFontColor(normalColor);
	pWidget->setHoverFontColor(hoverColor);
	pWidget->setClientImageId(-1);
	pWidget->setText(str);
	pWidget->setEventTrigger(_eventTrigger.trigger);
	pWidget->setClickSound(clickSound);
	pWidget->setHoverSound(hoverSound);
	pWidget->enable(nl);
	pWidget->setWheelDeliver(true);
	pWidget->setPickable(pickable);
	pWidget->setOutlineColor(_widgetFontOutLineColor);
	pWidget->setUnderlineColor(clrUnderline);
	lastLine->_widgetList.push_back(pWidget);
}

/**
*@brief 设定子成员半透
*/
void ATOM_RichEdit::setChildAlpha(float fAlpha)
{
	ATOM_RichEdit::LineList* lineList = getLineList();
	RichEditLine::WidgetList::iterator iter_child;
	ATOM_Label* pWidget;
	DWORD dwColor, dwAlpha = DWORD(0xff * fAlpha)<<24;
	for (LineList::iterator iter = lineList->begin(); iter != lineList->end(); ++iter)
	{
		for (iter_child = iter->_widgetList.begin(); iter_child!= iter->_widgetList.end(); ++iter_child)
		{
			pWidget = dynamic_cast<ATOM_Label*>(*iter_child);
			if (pWidget)
			{
				dwColor = pWidget->getFontColor();
				pWidget->setFontColor((dwColor & 0xffffff) | dwAlpha);

				dwColor = pWidget->getOutlineColor();
				pWidget->setOutlineColor((dwColor & 0xffffff) | dwAlpha);

				dwColor = pWidget->getColor();
				pWidget->setColor((dwColor & 0xffffff) | dwAlpha);
			}
		}
	}
}

void ATOM_RichEdit::addHyperLink(const char* str, unsigned normalColor, unsigned hoverColor, int id, int iParam,const char* pParam, unsigned clrUnderline,int clickSound, int hoverSound, bool nl, bool pickable)
{
	if(!str || !str[0])
	{
		return;
	}

	// 计算宽度
	ATOM_GUIFont::handle hfont = ATOM_GUIFont::ensureValidFont (getFont(), 12, 0);
	ATOM_FontHandle font = ATOM_GUIFont::getFontHandle (hfont);
	int charmargin = ATOM_GUIFont::getFontCharMargin (hfont);
	ATOM_SetCharMargin (charmargin);
	int l, t, w, h;
	ATOM_CalcStringBounds (font, str, strlen(str), &l, &t, &w, &h);
	h = getLineHeight();

	ATOM_RichEdit::LineList* lineList = getLineList();

	// 如果一行都没有，就放入一行
	if(lineList->empty())
	{
		lineList->push_back(RichEditLine(0, getLineHeight()));
	}
	RichEditLine* lastLine = &lineList->back();

	int offsetX = _verticalScrollBar && (_creationStyle & ATOM_Widget::LeftScroll) ? _scrollBarWidth : 0;
	float scrollValueY = getScrollValue().y;
	ATOM_Widget* lastWidget = lastLine->_widgetList.empty() ? NULL : lastLine->_widgetList.back();
	int x = lastWidget ? lastWidget->getWidgetRect().point.x + lastWidget->getWidgetRect().size.w : offsetX;
	int widthMax = getClientRect().size.w-x;	

	// 如果这行显示不下
	if(widthMax < w)
	{
		lineList->push_back(RichEditLine(lastLine->_top+lastLine->_height, getLineHeight()));
		lastLine = &lineList->back();
		x = offsetX;
		widthMax = getClientRect().size.w;
	}

	// 创建控件
	ATOM_HyperLink* pWidget = ATOM_NEW(ATOM_HyperLink, this, ATOM_Rect2Di(x, lastLine->_top-scrollValueY, w, h), ATOM_Widget::Control|ATOM_Widget::NoFocus|ATOM_Widget::NonLayoutable, id, ATOM_Widget::ShowNormal);
	
	pWidget->SetParam(iParam);
	pWidget->SetLua(pParam);
	pWidget->setFont(getFont());
	pWidget->setFontColor(normalColor);
	pWidget->setHoverFontColor(hoverColor);
	pWidget->setClientImageId(-1);
	pWidget->setText(str);
	pWidget->setEventTrigger(_eventTrigger.trigger);
	pWidget->setClickSound(clickSound);
	pWidget->setHoverSound(hoverSound);
	pWidget->enable(nl);
	pWidget->setWheelDeliver(true);
	pWidget->setPickable(pickable);
	pWidget->setOutlineColor(_widgetFontOutLineColor);
	pWidget->setUnderlineColor(clrUnderline);
	lastLine->_widgetList.push_back(pWidget);
}

void ATOM_RichEdit::addCell( int frameImageId, int w, int h, int id, int xSize /*= 1*/,int ySize /*= 1*/,int frameOffset/* = 0*/,bool pickable /*= true */ )
{
	ATOM_STACK_TRACE(ATOM_RichEdit::addCell);

	ATOM_RichEdit::LineList* lineList = getLineList();

	if(lineList->empty())
	{
		lineList->push_back(RichEditLine(0, getLineHeight()));
	}
	RichEditLine* lastLine = &lineList->back();

	int offsetX = (_verticalScrollBar && (_creationStyle & ATOM_Widget::LeftScroll)) ? _scrollBarWidth : 0;
	float scrollValueY = getScrollValue().y;
	ATOM_Widget* lastWidget = lastLine->_widgetList.empty() ? NULL : lastLine->_widgetList.back();
	int x = lastWidget ? lastWidget->getWidgetRect().point.x + lastWidget->getWidgetRect().size.w : offsetX;
	int widthMax = getClientRect().size.w-x;

	// 如果这行显示不下
	if(widthMax < w)
	{
		lineList->push_back(RichEditLine(lastLine->_top+lastLine->_height, getLineHeight()));
		lastLine = &lineList->back();
		x = offsetX;
		widthMax = getClientRect().size.w;
	}

	//
	ATOM_Cell* cell = ATOM_NEW(ATOM_Cell, this, ATOM_Rect2Di(x, lastLine->_top-scrollValueY, w, h), ATOM_Widget::Control|ATOM_Widget::NoFocus|ATOM_Widget::NonLayoutable, id, ATOM_Widget::ShowNormal);
	cell->setFrameImageId(frameImageId);
	cell->setCellWidth(w,h);
	cell->setFrameOffset(frameOffset);
	cell->setCellSize(xSize,ySize);
	cell->setEventTrigger(_eventTrigger.trigger);
	cell->setWheelDeliver(true);
	cell->setPickable(pickable);
	lastLine->_widgetList.push_back(cell);
	if(lastLine->_height < h)
	{
		lastLine->_height = h;
	}
}

void ATOM_RichEdit::pasreRichEdit( ATOM_TiXmlElement * elem )
{
	ATOM_STACK_TRACE(DefaultCreateWidget);

	if(NULL == elem)
	{
		return;
	}
	const char *a = elem->Attribute("valign");
	ATOM_MAP<ATOM_STRING,unsigned>::iterator itr = _colorConfigMap.find(elem->Value());
	if(itr != _colorConfigMap.end())
	{
		_currentColor = itr->second;
		return;
	}
	int align = ATOM_Widget::AlignY_Top;
	if (a)
	{
		if (!stricmp(a, "center")) 
			align = ATOM_Widget::AlignY_Middle;
		else if (!stricmp(a, "bottom")) 
			align = ATOM_Widget::AlignY_Bottom;
	}

	if(0 == strcmp(elem->Value(), "n") )
	{	// 创建换行
		addNewline();
	}
	else if(0 == strcmp(elem->Value(), "i") )
	{	// 创建图片
		int imageId = 0;
		if( NULL == elem->Attribute("v", &imageId) )
		{
			return;
		}
		int val;
		int id = elem->Attribute("id", &id) ? id : -1;
		int w = elem->Attribute("w", &w) ? w : getLineHeight();
		int h = elem->Attribute("h", &h) ? h :getLineHeight();
		bool nl = (elem->Attribute("enable", &val) && !val) ? false : true;
		bool pickable = (elem->Attribute("pickable", &val) && !val) ? false : true;
		addImage(imageId, w, h, id, nl, pickable);
	}
	else if(0 == strcmp(elem->Value(), "h") )
	{	// 创建超链
		const char* str = elem->Attribute("v");
		int id = elem->Attribute("id", &id) ? id : ATOM_Widget::AnyId;
		int val;
		unsigned normal = elem->Attribute("normal", &val) ? val : 0xFF000000;
		normal |= 0xFF000000;
		unsigned hover = elem->Attribute("hover", &val) ? val : 0xFF000000;
		hover |= 0xFF000000;
		int clickSound = elem->Attribute("clicksound", &val) ? val : INVALID_AUDIOID;
		int hoverSound = elem->Attribute("hoversound", &val) ? val : INVALID_AUDIOID;
		bool nl = (elem->Attribute("enable", &val) && !val) ? false : true;
		bool pickable = (elem->Attribute("pickable", &val) && !val) ? false : true;
		unsigned underline = elem->Attribute("underlinecolor", &val) ? val : 0;
		addHyperLink(str, normal, hover, id, elem->Attribute("lua"),underline,clickSound, hoverSound, nl, pickable);
	}
	else if(0 == strcmp(elem->Value(), "b") )
	{ // 创建按钮
		const char *str = elem->Attribute("t");
		int imageId = 0;
		elem->Attribute("v", &imageId);
		int val;
		int id = elem->Attribute("id", &id) ? id : ATOM_Widget::AnyId;
		int w = elem->Attribute("w", &w) ? w : getLineHeight();
		int h = elem->Attribute("h", &h) ? h : getLineHeight();
		bool nl = (elem->Attribute("enable", &val) && !val) ? false : true;
		bool pickable = (elem->Attribute("pickable", &val) && !val) ? false : true;
		addButton(str, imageId, w, h, id, align, elem->Attribute("lua"),0,0,0, nl, pickable);
	}
	else if(0 == strcmp(elem->Value(), "c") )
	{ 
		// 创建格子控件
		int imageId = 0;
		elem->Attribute("v", &imageId);
		int val;
		int id = elem->Attribute("id", &id) ? id : ATOM_Widget::AnyId;
		int w = elem->Attribute("w", &w) ? w : getLineHeight();
		int h = elem->Attribute("h", &h) ? h : getLineHeight();
		int xSize = elem->Attribute("x", &xSize) ? xSize :1;
		int ySize = elem->Attribute("y", &ySize) ? ySize :1;
		int frameOffset = elem->Attribute("offset", &frameOffset) ? frameOffset :0;
		bool pickable = (elem->Attribute("pickable", &val) && !val) ? false : true;
		addCell(imageId,w,h,id,xSize,ySize,frameOffset,pickable);
	}
	else if(!strcmp(elem->Value(), "t"))
	{
		const char* str = elem->Attribute("v");
		if(str)
		{
			_currentColor |= 0xFF000000;
			addString(str, _currentColor);
		}
	}
}






