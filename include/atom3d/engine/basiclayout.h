#ifndef __ATOM3D_ENGINE_BASICLAYOUT_H
#define __ATOM3D_ENGINE_BASICLAYOUT_H

#include "widgetlayout.h"

class ATOM_BasicLayout: public ATOM_WidgetLayout
{
public:
	virtual Type getType (void) const;

protected:
	virtual void onCalcLayout (void);
};

#endif // __ATOM3D_ENGINE_BASICLAYOUT_H
