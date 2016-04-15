/**	\file richedit.h
 *	���༭����.
 *
 *	\author �״�ʹ
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

// ���б༭��һ����Ϣ
struct RichEditLine
{
	typedef ATOM_LIST<ATOM_Widget*> WidgetList;

	WidgetList	_widgetList;	//!< һ���е����пؼ�
	int			_top;			//!< ���е�Y��λ�� 
	int			_height;		//!< ���еĸ߶�

	inline RichEditLine(int t, int h) : _top(t), _height(h){}
};

typedef void (*CreateREWidgetFunc)(ATOM_TiXmlElement*, ATOM_RichEdit*);

//! ���б༭��
class ATOM_ENGINE_API ATOM_RichEdit: public ATOM_Widget
{
public:
	typedef ATOM_LIST<RichEditLine> LineList;

	ATOM_RichEdit();
	ATOM_RichEdit (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, int lineHeight, ATOM_Widget::ShowState = ATOM_Widget::Hide);
	virtual ~ATOM_RichEdit (void);

public:
	virtual ATOM_WidgetType getType (void) const;

	//! ���XML��ʽ
	//! \note �ڲ���ͨ��_createREWidgetFunc������Ӧ��WIDGET�����Զ��Ų����������Ҫ�Զ����ʽ�������setCreateFunc�滻��������
	void addXml(const char* str);

	//! ����XML��ʽ
	//! \note �ڲ���ͨ��_createREWidgetFunc������Ӧ��WIDGET�����Զ��Ų����������Ҫ�Զ����ʽ�������setCreateFunc�滻��������
	void setXml(const char* str);

	//! ����ַ���
	void addString(const char* str, unsigned color, bool nb = true, bool pickable = true, ATOM_GUIFont::handle font = NULL, unsigned clrOutline = 0, unsigned clrUnderline = 0);

	//! ����ַ���
	void addStringEx(const char* str, unsigned color, int align, bool nb = true, bool pickable = true, ATOM_GUIFont::handle font = NULL, unsigned clrOutline = 0, unsigned clrUnderline = 0);

	//! ���ǿ�ƻ��з�
	void addNewline();

	//! ��Ӱ�ť
	void addButton(const char* str, int imageId, int w, int h, int id, bool nb = true, bool pickable = true);

	//! ��Ӱ�ť
	void addButton(const char* str, int imageId, int w, int h, int id, const char *param, bool nb = true, bool pickable = true);

	//! ��Ӱ�ť
	void addButton(const char* str, int imageId, int w, int h, int id, int align, const char *param, unsigned outlinecolor,unsigned textcolor,int alignText,bool nb = true, bool pickable = true);

	//! ���ͼƬ
	void addImage(int imageId, int w, int h, int id, bool nb = true, bool pickable = true);

	//! ��Ӹ���
	void addCell( int frameImageId,int w, int h, int id, int xSize = 1,int ySize = 1,int frameOffset = 0,bool pickable = true );

	//! ���ͼƬ(�пؼ�ID,�����ִ���ͼƬLABEL)
	void addImageEx(int imageId, int w, int h, const char* str, int align, unsigned color, ATOM_GUIFont::handle font = NULL, int id = ATOM_Widget::AnyId, bool nb = true, bool pickable = true);

	//! ��ӳ�����
	void addHyperLink(const char* str, unsigned normalColor, unsigned hoverColor, int id, const char* pParam,unsigned clrUnderline = 0, int clickSound = INVALID_AUDIOID, int hoverSound = INVALID_AUDIOID, bool nl = true, bool pickable = true);

	//! ��ӳ�����
	void addHyperLink(const char* str, unsigned normalColor, unsigned hoverColor, int id, int iParam, const char* pParam,unsigned clrUnderline = 0, int clickSound = INVALID_AUDIOID, int hoverSound = INVALID_AUDIOID, bool nl = true, bool pickable = true);

	//! ��ӳ�����
	void addHyperLink(const char* str, unsigned normalColor, unsigned hoverColor, int id = ATOM_Widget::AnyId, int clickSound = INVALID_AUDIOID, int hoverSound = INVALID_AUDIOID, bool nb = true, bool pickable = true, unsigned clrOutline = 0, unsigned clrUnderline = 0);

	//! �߶�����
	void setMaxHeight(int maxHeight);

	//! ȡ�ø߶�����
	int getMaxHeight() const;

	//! �����Զ���Ӧ��С
	void setAutoSize(bool autoSize);

	//! �Ƿ��Զ���Ӧ��С
	bool getAutoSize() const;

	//! �������
	void clear();

	//! ɾ����������
	void deleteLine(int line);

	//! ���������ɫ
	void setOutlineColor(ATOM_ColorARGB color);

	//! ȡ�������ɫ
	ATOM_ColorARGB getOutlineColor() const;

	//! �趨�ӳ�Ա��͸
	void setChildAlpha(float fAlpha);

	//! ȡ��һ�еĸ߶�
	int getLineHeight() const;

	//! ����������
	void scroll2Top();

	//! �������ײ�
	void scroll2Bottom();

	//! ���ý�������
	//! \return ���ؾõ����ú���
	CreateREWidgetFunc setCreateFunc(CreateREWidgetFunc func);

	//!< �¸�ʽ����
	void pasreRichEdit(ATOM_TiXmlElement * elem);

public:
	// ȡ���ӿؼ�����
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
