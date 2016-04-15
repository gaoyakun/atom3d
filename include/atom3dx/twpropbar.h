#ifndef __ATOMX_TWPROPBAR_H
#define __ATOMX_TWPROPBAR_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../atom3d/ATOM_kernel.h"

#include "twbar.h"

class ATOMX_API ATOMX_PropertyTweakBar: public ATOMX_TweakBar
{
public:
	ATOMX_PropertyTweakBar (const char *name);
	ATOMX_PropertyTweakBar (const char *name, ATOM_Object *object);

public:
	ATOM_Object *getObject (void) const;
	void setObject (ATOM_Object *object, bool clearBeforeSet = true);
	void pushObject (ATOM_Object *object);
	void popObject (void);

public:
	virtual void clear (void);

public:
	void onValueChanged (ATOMX_TWValueChangedEvent *event);
	void onCommand (ATOMX_TWCommandEvent *event);

private:
	void setupPropertyVars (bool addReturnButton);
	void setupPropertyVar (const char *prop, int index);
	ATOM_STRING askClassName (void) const;

private:
	struct VarDetail
	{
		ATOMX_TBType type;
		float minValue;
		float maxValue;
		float step;
		float precision;
		bool isObject;
		bool isEnum;
		bool hasMinMax;
		bool hasStep;
		bool hasPrecision;
		bool isVfsFileName;
		bool isFileName;
		bool isVfsDirName;
		bool isDirName;
		bool isReadonly;
		int attribIndex;
		ATOM_STRING filenameFilter;
		ATOM_STRING group;
		ATOM_STRING desc;
		ATOMX_TBValue propValue;
		ATOMX_TBEnum propEnumValue;
		ATOM_ScriptVar value;
		ATOM_VECTOR<ATOM_STRING> memberNames;
	};
	bool parsePropDetail (ATOMX_PropertyTweakBar *bar, ATOM_Object *object, const char *propName, const char *propComment, VarDetail *detail);

	ATOM_VECTOR<ATOM_AUTOREF(ATOM_Object)> _objects;
	ATOM_VECTOR<VarDetail> _varDetails;

	ATOM_DECLARE_EVENT_MAP(ATOMX_PropertyTweakBar, ATOMX_TweakBar)
};

#endif // __ATOMX_TWPROPBAR_H
