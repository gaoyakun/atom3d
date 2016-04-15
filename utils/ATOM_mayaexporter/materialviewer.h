#ifndef __ATOM_MAYAEXPORTER_MATERIALVIEWER_H
#define __ATOM_MAYAEXPORTER_MATERIALVIEWER_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <string>
#include <vector>
#include "propedit.h"

class MaterialViewer
{
public:
	void addMaterials (const MObjectArray &shaders);
	unsigned getNumMaterials (void) const;
	const char *getMaterialName (unsigned index) const;
	PropertyEditor *getProperty (unsigned index);
	const PropertyEditor *getProperty (unsigned index) const;
	PropertyEditor *getPropertyByName (const char *name);
	const PropertyEditor *getPropertyByName (const char *name) const;
	bool edit (HWND parentWnd);
	void clear (void);

private:
	std::vector<PropertyEditor> _shaderProps;
	std::vector<std::string> _materialNames;
};

#endif // __ATOM_MAYAEXPORTER_MATERIALVIEWER_H
