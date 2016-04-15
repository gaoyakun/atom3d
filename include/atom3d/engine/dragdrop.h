#ifndef __ATOM3D_ENGINE_DRAGDROP_H
#define __ATOM3D_ENGINE_DRAGDROP_H

#include "../ATOM_kernel.h"
#include "../ATOM_utils.h"
#include "basedefs.h"

class ATOM_Widget;

class ATOM_ENGINE_API ATOM_DragSource: public ATOM_ReferenceObj
{
public:
	void setWidget (ATOM_Widget *widget);
	ATOM_Widget *getWidget (void) const;
	void addProperty (const char *name, const ATOM_Variant &value);
	void removeProperty (const char *name);
	const ATOM_Variant &getProperty (const char *name) const;
	void removeAllProperties (void);
	void setIndicatorImageId (int id);
	int getIndicatorImageId (void) const;
	void setIndicatorRect (const ATOM_Rect2Di &rect);
	const ATOM_Rect2Di &getIndicatorRect (void) const;
	void setDraggingPosition (const ATOM_Point2Di &pt);
	const ATOM_Point2Di &getDraggingPosition (void) const;

private:
	struct Indicator
	{
		int imageId;
		ATOM_Rect2Di rect;
	};

	ATOM_HASHMAP<ATOM_STRING, ATOM_Variant> _props;
	Indicator _indicators;
	ATOM_Widget *_widget;
	ATOM_Point2Di _dragPosition;
};


#endif // __ATOM3D_ENGINE_DRAGDROP_H
