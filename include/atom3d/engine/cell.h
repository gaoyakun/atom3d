/**	\file cell.h
 *	������.
 *
 *	\author �״�ʹ
 *	\addtogroup engine
 *	@{
 */

#ifndef __ATOM3D_ENGINE_CELL_H
#define __ATOM3D_ENGINE_CELL_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_render.h"

#include "gui_font.h"
#include "celldata.h"



//! ���ӿؼ�
class ATOM_ENGINE_API ATOM_Cell: public ATOM_Widget
{
	friend class ATOM_CellData;

public:
	enum GrowMode
	{
		GM_NONE = 0,
		GM_UP,
		GM_DOWN
	};

public:
	ATOM_Cell();
	ATOM_Cell (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_Widget::ShowState showState = ATOM_Widget::Hide);
	virtual ~ATOM_Cell (void);

public:
	virtual ATOM_WidgetType getType (void) const;

	//! ���ø�������
	virtual void setCellSize(int x, int y);

	//! ȡ�ø�������
	void getCellSize(int& x, int& y) const;

	//! ���ø��Ӵ�С
	virtual void setCellWidth(int x, int y);

	//! ȡ�ø��Ӵ�С
	void getCellWidth(int& x, int& y) const;

	//! ȡ�ø��Ӵ�С
	ATOM_Size2Di getCellWidth (void) const;

	//! ����������ģʽ 
	void setGrowMode (GrowMode growMode);

	//! ��ȡ������ģʽ
	GrowMode getGrowMode (void) const;

	//! ׷����
	void appendRows (unsigned numRows);

	//! �Ƴ���
	void truncateRows (unsigned numRows);

	//! ���ø�������
	virtual ATOM_CellData* setCellData(int x, int y, int imageId, const char* str, unsigned cooldownMax = 0, unsigned cooldown = 0);

	//! ͨ��λ��ȡ�ø�������
	ATOM_CellData* getCell(int x, int y) const;

	//! ���ö���
	void setAlign(int align = AlignX_Middle|AlignY_Middle);

	//! ȡ�ö���
	int getAlign() const;

	//! ��������ƫ��
	void setOffset(const ATOM_Point2Di& offset);

	//! ȡ������ƫ��
	const ATOM_Point2Di& getOffset() const;

	//! ����CDͼƬ
	void setCooldownImageId(int id);

	//! ȡ��CDͼƬ
	int getCooldownImageId() const;

	//! ����ѡ��ͼƬ
	void setSelectImageId(int id);

	//! ȡ��ѡ��ͼƬ
	int getSelectImageId() const;

	//! ���ñ߿�ͼƬ
	void setFrameImageId (int id, bool reset = false);

	//! ȡ�ñ߿�ͼƬ
	int getFrameImageId (void) const;

	//! ���ñ߿�ƫ��
	void setFrameOffset(int offset, bool reset = false);

	//! ȡ�ñ߿�ƫ��
	int getFrameOffset (void) const;

	//! ����X���
	void setSpaceX(int val);

	//! ȡ��X���
	int getSpaceX() const;

	//! ����Y���
	void setSpaceY(int val);

	//! ȡ��Y���
	int getSpaceY() const;

	//! ѡ��
	void select(int x, int y);

	//! �Ƿ�����ѡ��
	void enableSelect(bool enable);

	//! ���������ɫ
	void setOutlineColor(ATOM_ColorARGB color);

	//! ȡ�������ɫ
	ATOM_ColorARGB getOutlineColor() const;

	//! �����Ƿ�������ӱ��Ϸ�
	void allowCellDragDrop (bool allow);

	//! ��ѯ�����Ƿ������Ϸ�
	bool isCellDragDropAllowed (void) const;

	//! ���ݵ�ǰ���ü�����ʵĿؼ���С
	ATOM_Size2Di calcClientSize (void) const;

	//! ���õ������ӵı߿�ͼ��
	void setCellFrameImageId(int x,int y,int imageId);

	//! ��ȡ�������ӵı߿�ͼ��
	int getCellFrameImageId(int x,int y) const;

	//! ���õ������ӵ��±�ͼƬ
	void setSubscriptImageId (int x,int y,int id);

	//! ȡ�õ������ӵ��±�ͼƬ
	int getSubscriptImageId(int x,int y)const;

	//! ���õ������ӵ��±�����
	void setSubscriptText (int x,int y,const char *str);

	//! ȡ�õ������ӵ��±�����
	const char *getSubscriptText (int x,int y) const;

	//! �����±�ͼƬ
	void setSubscriptImageId (int id);

	//! ȡ���±�ͼƬ
	int getSubscriptImageId()const;

	//! �����±�ͼƬλ��
	void setSubscriptImageRect(ATOM_Rect2Di& rect);

	//! ȡ���±�ͼƬλ��
	ATOM_Rect2Di getSubscriptImageRect()const;

	//! �����±�����λ��
	void setSubscriptTextPosition(ATOM_Point2Di & point);

	//! ��ȡ�±�����λ��
	ATOM_Point2Di getSubscriptTextPosition() const;

private:
	void unselect (void);
	void alignCells (void);

public:
	void onCellLButtonDown(ATOM_CellLButtonDownEvent *event);
	void onCellLButtonUp(ATOM_CellLButtonUpEvent *event);
	void onCellRButtonDown(ATOM_CellRButtonDownEvent *event);
	void onCellRButtonUp(ATOM_CellRButtonUpEvent *event);
	void onCellLClick (ATOM_CellLeftClickEvent *event);
	void onCellRClick (ATOM_CellRightClickEvent *event);
	void onCellDblClick (ATOM_CellDblClickEvent *event);
	void onCellMouseEnter (ATOM_CellMouseEnterEvent *event);
	void onCellMouseLeave (ATOM_CellMouseLeaveEvent *event);
	void onCellDragStart (ATOM_CellDragStartEvent *event);
	void onCellDragOver (ATOM_CellDragOverEvent *event);
	void onCellDragDrop (ATOM_CellDragDropEvent *event);
	void onPaint (ATOM_WidgetDrawClientEvent *event);

	ATOM_DECLARE_EVENT_MAP(ATOM_Cell, ATOM_Widget)

protected:

protected:
	int _cooldownImageId;
	int _selectImageId;
	int _xCellWidth, _yCellWidth;
	int _xSize, _ySize;
	int _align;
	GrowMode _growMode;
	ATOM_Point2Di _offset;
	int _xSpace;
	int _ySpace;
	int _selectId;
	int _frameImageId;
	int	_frameOffset;
	bool _enableSelect;
	bool _enableDragDrop;
	bool _updateCellDataMap;
	int						_subscriptImageId;		//!< �±�ͼƬ
	ATOM_Rect2Di			_subscriptRect;			//!< �±�ͼƬλ��
	ATOM_Point2Di			_subscriptTextPosition;	//!< �±�����λ��
	//ATOM_ColorARGB _outlineColor;
};

#endif // __ATOM3D_ENGINE_CELL_H
/*! @} */
