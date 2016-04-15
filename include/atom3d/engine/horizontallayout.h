#ifndef __ATOM3D_ENGINE_HORIZONTALLAYOUT_H
#define __ATOM3D_ENGINE_HORIZONTALLAYOUT_H

#include "widgetlayout.h"

class ATOM_HorizontalLayout: public ATOM_WidgetLayout
{
public:
	ATOM_HorizontalLayout (void);

public:
	virtual Type getType (void) const;

protected:
	virtual void onCalcLayout (void);
};

#endif // __ATOM3D_ENGINE_HORIZONTALLAYOUT_H
