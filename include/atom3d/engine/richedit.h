/**	\file richedit.h
 *	富编辑框类.
 *
 *	\author 白大使
 *	\addtogroup engine
 *	@{
 */

#ifndef __ATOM3D_ENGINE_RICHEDIT_H
#define __ATOM3D_ENGINE_RICHEDIT_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_dbghlp.h"
#include "../ATOM_render.h"

#include "gui_font.h"
#include "widget.h"



class ATOM_RichEdit;

// 多行编辑器一行信息
struct RichEditLine
{
	typedef ATOM_LIST<ATOM_Widget*> WidgetList;

	WidgetList	_widgetList;	//!< 一行中的所有控件
	int			_top;			//!< 该行的Y轴位置 
	int			_height;		//!< 该行的高度

	inline RichEditLine(int t, int h) : _top(t), _height(h){}
};

typedef void (*CreateREWidgetFunc)(ATOM_TiXmlElement*, ATOM_RichEdit*);

//! 多行编辑框
class ATOM_ENGINE_API ATOM_RichEdit: public ATOM_Widget
{
public:
	typedef ATOM_LIST<RichEditLine> LineList;

	ATOM_RichEdit();
	ATOM_RichEdit (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, int lineHeight, ATOM_Widget::ShowState = ATOM_Widget::Hide);
	virtual ~ATOM_RichEdit (void);

public:
	virtual ATOM_WidgetType getType (void) const;

	//! 添加XML格式
	//! \note 内部会通过_createREWidgetFunc创建相应的WIDGET，并自动排布，如果有需要自定义格式，则调用setCreateFunc替换解析函数
	void addXml(const char* str);

	//! 设置XML格式
	//! \note 内部会通过_createREWidgetFunc创建相应的WIDGET，并自动排布，如果有需要自定义格式，则调用setCreateFunc替换解析函数
	void setXml(const char* str);

	//! 添加字符串
	void addString(const char* str, unsigned color, bool nb = true, bool pickable = true, ATOM_GUIFont::handle font = NULL, unsigned clrOutline = 0, unsigned clrUnderline = 0);

	//! 添加字符串
	void addStringEx(const char* str, unsigned color, int align, bool nb = true, bool pickable = true, ATOM_GUIFont::handle font = NULL, unsigned clrOutline = 0, unsigned clrUnderline = 0);

	//! 添加强制换行符
	void addNewline();

	//! 添加按钮
	void addButton(const char* str, int imageId, int w, int h, int id, bool nb = true, bool pickable = true);

	//! 添加按钮
	void addButton(const char* str, int imageId, int w, int h, int id, const char *param, bool nb = true, bool pickable = true);

	//! 添加按钮
	void addButton(const char* str, int imageId, int w, int h, int id, int align, const char *param, unsigned outlinecolor,unsigned textcolor,int alignText,bool nb = true, bool pickable = true);

	//! 添加图片
	void addImage(int imageId, int w, int h, int id, bool nb = true, bool pickable = true);

	//! 添加格子
	void addCell( int frameImageId,int w, int h, int id, int xSize = 1,int ySize = 1,int frameOffset = 0,bool pickable = true );

	//! 添加图片(有控件ID,有文字串的图片LABEL)
	void addImageEx(int imageId, int w, int h, const char* str, int align, unsigned color, ATOM_GUIFont::handle font = NULL, int id = ATOM_Widget::AnyId, bool nb = true, bool pickable = true);

	//! 添加超链接
	void addHyperLink(const char* str, unsigned normalColor, unsigned hoverColor, int id, const char* pParam,unsigned clrUnderline = 0, int clickSound = INVALID_AUDIOID, int hoverSound = INVALID_AUDIOID, bool nl = true, bool pickable = true);

	//! 添加超链接
	void addHyperLink(const char* str, unsigned normalColor, unsigned hoverColor, int id, int iParam, const char* pParam,unsigned clrUnderline = 0, int clickSound = INVALID_AUDIOID, int hoverSound = INVALID_AUDIOID, bool nl = true, bool pickable = true);

	//! 添加超链接
	void addHyperLink(const char* str, unsigned normalColor, unsigned hoverColor, int id = ATOM_Widget::AnyId, int clickSound = INVALID_AUDIOID, int hoverSound = INVALID_AUDIOID, bool nb = true, bool pickable = true, unsigned clrOutline = 0, unsigned clrUnderline = 0);

	//! 高度限制
	void setMaxHeight(int maxHeight);

	//! 取得高度限制
	int getMaxHeight() const;

	//! 设置自动适应大小
	void setAutoSize(bool autoSize);

	//! 是否自动适应大小
	bool getAutoSize() const;

	//! 清空数据
	void clear();

	//! 删除单条数据
	void deleteLine(int line);

	//! 设置描边颜色
	void setOutlineColor(ATOM_ColorARGB color);

	//! 取得描边颜色
	ATOM_ColorARGB getOutlineColor() const;

	//! 设定子成员半透
	void setChildAlpha(float fAlpha);

	//! 取得一行的高度
	int getLineHeight() const;

	//! 滚动到顶部
	void scroll2Top();

	//! 滚动到底部
	void scroll2Bottom();

	//! 设置解析函数
	//! \return 返回久的设置函数
	CreateREWidgetFunc setCreateFunc(CreateREWidgetFunc func);

	//!< 新格式解析
	void pasreRichEdit(ATOM_TiXmlElement * elem);

public:
	// 取得子控件数据
	LineList* getLineList();

public:
	void onPaint (ATOM_WidgetDrawClientEvent *event);
	void onCommand (ATOM_WidgetCommandEvent *event);
	static bool loadRichEditColor(const char * strRichEditColorFile);
protected:
	virtual bool supportIME (void) const;
	void addButtonImpl (const char* str, int imageId, int w, int h, int id, int yoff, const char *param,unsigned outlinecolor,unsigned textcolor,int alignText, bool nb, bool pickable);
	void addStringImpl (const char* str, unsigned color, int align, bool nl, bool pickable, ATOM_GUIFont::handle font, unsigned clrOutline, unsigned clrUnderline);

	ATOM_DECLARE_EVENT_MAP(ATOM_RichEdit, ATOM_Widget)

protected:
	LineList				_lineList;
	CreateREWidgetFunc		_createREWidgetFunc;

//	int						_cursorLine;
	int						_lineHeight;
	int						_maxHeight;

	//ATOM_ColorARGB _outlineColor;

	bool					_autoSize;
	unsigned _currentColor;
	static ATOM_MAP<ATOM_STRING,unsigned> _colorConfigMap;
};



#endif // __ATOM3D_ENGINE_RICHEDIT_H
/*! @} */
