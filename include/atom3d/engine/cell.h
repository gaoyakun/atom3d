/**	\file cell.h
 *	格子类.
 *
 *	\author 白大使
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



//! 格子控件
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

	//! 设置格子数量
	virtual void setCellSize(int x, int y);

	//! 取得格子数量
	void getCellSize(int& x, int& y) const;

	//! 设置格子大小
	virtual void setCellWidth(int x, int y);

	//! 取得格子大小
	void getCellWidth(int& x, int& y) const;

	//! 取得格子大小
	ATOM_Size2Di getCellWidth (void) const;

	//! 设置行增长模式 
	void setGrowMode (GrowMode growMode);

	//! 获取行增长模式
	GrowMode getGrowMode (void) const;

	//! 追加行
	void appendRows (unsigned numRows);

	//! 移除行
	void truncateRows (unsigned numRows);

	//! 设置格子数据
	virtual ATOM_CellData* setCellData(int x, int y, int imageId, const char* str, unsigned cooldownMax = 0, unsigned cooldown = 0);

	//! 通过位置取得格子数据
	ATOM_CellData* getCell(int x, int y) const;

	//! 设置对齐
	void setAlign(int align = AlignX_Middle|AlignY_Middle);

	//! 取得对齐
	int getAlign() const;

	//! 设置文字偏移
	void setOffset(const ATOM_Point2Di& offset);

	//! 取得文字偏移
	const ATOM_Point2Di& getOffset() const;

	//! 设置CD图片
	void setCooldownImageId(int id);

	//! 取得CD图片
	int getCooldownImageId() const;

	//! 设置选择图片
	void setSelectImageId(int id);

	//! 取得选择图片
	int getSelectImageId() const;

	//! 设置边框图片
	void setFrameImageId (int id, bool reset = false);

	//! 取得边框图片
	int getFrameImageId (void) const;

	//! 设置边框偏移
	void setFrameOffset(int offset, bool reset = false);

	//! 取得边框偏移
	int getFrameOffset (void) const;

	//! 设置X间隔
	void setSpaceX(int val);

	//! 取得X间隔
	int getSpaceX() const;

	//! 设置Y间隔
	void setSpaceY(int val);

	//! 取得Y间隔
	int getSpaceY() const;

	//! 选择
	void select(int x, int y);

	//! 是否允许选择
	void enableSelect(bool enable);

	//! 设置描边颜色
	void setOutlineColor(ATOM_ColorARGB color);

	//! 取得描边颜色
	ATOM_ColorARGB getOutlineColor() const;

	//! 设置是否允许格子被拖放
	void allowCellDragDrop (bool allow);

	//! 查询格子是否允许拖放
	bool isCellDragDropAllowed (void) const;

	//! 根据当前设置计算合适的控件大小
	ATOM_Size2Di calcClientSize (void) const;

	//! 设置单个格子的边框图像
	void setCellFrameImageId(int x,int y,int imageId);

	//! 获取单个格子的边框图像
	int getCellFrameImageId(int x,int y) const;

	//! 设置单个格子的下标图片
	void setSubscriptImageId (int x,int y,int id);

	//! 取得单个格子的下标图片
	int getSubscriptImageId(int x,int y)const;

	//! 设置单个格子的下标文字
	void setSubscriptText (int x,int y,const char *str);

	//! 取得单个格子的下标文字
	const char *getSubscriptText (int x,int y) const;

	//! 设置下标图片
	void setSubscriptImageId (int id);

	//! 取得下标图片
	int getSubscriptImageId()const;

	//! 设置下标图片位置
	void setSubscriptImageRect(ATOM_Rect2Di& rect);

	//! 取得下标图片位置
	ATOM_Rect2Di getSubscriptImageRect()const;

	//! 设置下标字体位置
	void setSubscriptTextPosition(ATOM_Point2Di & point);

	//! 获取下标字体位置
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
	int						_subscriptImageId;		//!< 下标图片
	ATOM_Rect2Di			_subscriptRect;			//!< 下标图片位置
	ATOM_Point2Di			_subscriptTextPosition;	//!< 下标字体位置
	//ATOM_ColorARGB _outlineColor;
};

#endif // __ATOM3D_ENGINE_CELL_H
/*! @} */
