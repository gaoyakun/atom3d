#ifndef __ATOM_MAYAEXPORTER_QUERYNAME_H
#define __ATOM_MAYAEXPORTER_QUERYNAME_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <string>
#include <windows.h>

typedef bool (__cdecl * VerifyFunction) (const char *str, void *context);

bool QueryName (HWND hWndParent, const char *title, std::string &result, VerifyFunction verifyFunciton = 0, void *verifyData = 0);

#endif // __ATOM_MAYAEXPORTER_QUERYNAME_H