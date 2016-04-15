#ifndef __ATOM_MAYAEXPORTER_SELECTPROP_H
#define __ATOM_MAYAEXPORTER_SELECTPROP_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <windows.h>
#include <windowsx.h>

class SelectProp
{
public:
	SelectProp (void): _selectedProp (0) {}
	const char *select (HWND parentWnd);

public:
	void setSelection (const char *s) { _selectedProp = s; }

private:
	const char *_selectedProp;
};

#endif // __ATOM_MAYAEXPORTER_SELECTPROP_H
