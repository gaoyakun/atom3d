#ifndef __ATOM_MAYAEXPORTER_MESHVIEWER_H
#define __ATOM_MAYAEXPORTER_MESHVIEWER_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <string>
#include <vector>
#include "propedit.h"

class MeshViewer
{
public:
	void addMesh (const char *name, PropertyEditor *props);
	void clear (void);
	unsigned getNumMeshes (void) const;
	const char *getMeshName (unsigned index) const;
	PropertyEditor *getProperty (unsigned index);
	bool edit (HWND parentWnd);

private:
	std::vector<PropertyEditor*> _meshProps;
	std::vector<std::string> _meshNames;
};

#endif // __ATOM_MAYAEXPORTER_MESHVIEWER_H
