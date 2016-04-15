#ifndef __ATOM3D_ENGINE_WIDGETLAYOUT_H
#define __ATOM3D_ENGINE_WIDGETLAYOUT_H

#include "basedefs.h"

class ATOM_Widget;

class ATOM_WidgetLayout
{
public:
	enum Type
	{
		Basic,
		Horizontal,
		Vertical
	};

	struct Unit
	{
		int desired_x;
		int desired_y;
		int desired_w;	// 负数表示百分数
		int desired_h;  // 负数表示百分数

		int actual_x;
		int actual_y;
		int actual_w;
		int actual_h;

		ATOM_Widget *widget;
	};

public:
	ATOM_WidgetLayout (void);
	virtual ~ATOM_WidgetLayout (void);

public:
	Unit *allocUnit (void);
	void freeUnit (Unit *unit);
	unsigned getNumUnits (void) const;
	Unit *getUnit (unsigned index) const;
	void invalidateLayout (void);
	void calcLayout (void);
	void setRect (const ATOM_Rect2Di &rect);
	const ATOM_Rect2Di &getRect (void) const;
	void setInnerSpaceLeft (int val);
	int getInnerSpaceLeft (void) const;
	void setInnerSpaceTop (int val);
	int getInnerSpaceTop (void) const;
	void setInnerSpaceRight (int val);
	int getInnerSpaceRight (void) const;
	void setInnerSpaceBottom (int val);
	int getInnerSpaceBottom (void) const;
	void setGap (int gap);
	int getGap (void) const;
	bool isDirty (void) const;
	bool moveUnitUp (Unit *unit);
	bool moveUnitDown (Unit *unit);

public:
	virtual Type getType (void) const = 0;
	virtual ATOM_Rect2Di computeRect (void) const;

protected:
	virtual void onCalcLayout (void) = 0;

public:
	static ATOM_WidgetLayout *createLayout (Type type);
	static void deleteLayout (ATOM_WidgetLayout *layout);

protected:
	int _innerSpaceLeft;
	int _innerSpaceTop;
	int _innerSpaceRight;
	int _innerSpaceBottom;
	int _gap;
	bool _layoutDirty;
	ATOM_Rect2Di _rect;
	ATOM_VECTOR<Unit*> _units;
};

#endif // __ATOM3D_ENGINE_WIDGETLAYOUT_H
