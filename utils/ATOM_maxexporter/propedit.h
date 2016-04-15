#ifndef __ATOM_MAYAEXPORTER_PROPEDIT_H
#define __ATOM_MAYAEXPORTER_PROPEDIT_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <ATOM_utils.h>
#include <windows.h>
#include <string>
#include <vector>

class PropertyEditor
{
public:
	PropertyEditor (void);
	~PropertyEditor (void);

public:
	unsigned getNumProperties (void) const;
	const char *getPropertyName (unsigned index) const;
	const char *getPropertyValue (unsigned index) const;
	int getPropertyValueAsInt (unsigned index) const;
	float getPropertyValueAsFloat (unsigned index) const;
	ATOM_ColorARGB getPropertyValueAsColor (unsigned index) const;
	void setPropertyName (unsigned index, const char *name);
	void setPropertyValue (unsigned index, const char *value);
	void setPropertyValueAsInt (unsigned index, int value);
	void setPropertyValueAsFloat (unsigned index, float value);
	void setPropertyValueAsColor (unsigned index, ATOM_ColorARGB color);
	void newProperty (const char *name = 0, const char *value = 0);
	void removeProperty (unsigned index);
	void clear (void);
	bool validateProperties (void) const;

public:
	bool edit (HWND parentWnd);
	void trackMenu (HWND hWnd, int x, int y, bool selected);
	void syncUI (HWND hWnd);

	std::string float2str (float value);
	std::string int2str (int value);
	std::string color2str (ATOM_ColorARGB value);
	float str2float (const char *str) const;
	int str2int (const char *str) const;
	ATOM_ColorARGB str2color (const char *str) const;

	bool editColorValue (HWND hWnd, ATOM_ColorARGB initValue, std::string &result);
	bool editFileNameValue (HWND hWnd, std::string &result);
	bool editFolderValue (HWND hWnd, std::string &result);
	bool editStringValue (HWND hWnd, std::string &result);
	bool editIntValue (HWND hWnd, std::string &result);
	bool editFloatValue (HWND hWnd, std::string &result);

private:
	std::vector<std::pair<std::string, std::string> > _properties;
};

#endif // __ATOM_MAYAEXPORTER_PROPEDIT_H
