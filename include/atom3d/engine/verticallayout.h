#ifndef __ATOM3D_ENGINE_VERTICALTALLAYOUT_H
#define __ATOM3D_ENGINE_VERTICALTALLAYOUT_H

#include "widgetlayout.h"

class ATOM_VerticalLayout: public ATOM_WidgetLayout
{
public:
	ATOM_VerticalLayout (void);

public:
	virtual Type getType (void) const;

protected:
	virtual void onCalcLayout (void);
};

#endif // __ATOM3D_ENGINE_VERTICALTALLAYOUT_H
