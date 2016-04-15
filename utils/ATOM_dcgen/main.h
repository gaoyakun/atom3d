#ifndef __ATOM_TOOLS_DCGEN_MAIN_H
#define __ATOM_TOOLS_DCGEN_MAIN_H

#include <ATOM.h>

class DCGenApp: public ATOM_Application
{
public:
  ATOM_AUTOREF(ATOM_RenderWindow) view;

public:
	void onAppInit (ATOM_AppInitEvent *event);
	void onAppExit (ATOM_AppExitEvent *event);
	void usage (void);

	ATOM_DECLARE_EVENT_MAP(DCGenApp, ATOM_Application)
};

#endif // __ATOM_TOOLS_DCGEN_MAIN_H
