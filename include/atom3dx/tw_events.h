#ifndef __ATOM3DX_TW_EVENTS_H
#define __ATOM3DX_TW_EVENTS_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../atom3d/ATOM_kernel.h"
#include "twbar.h"

class ATOMX_TWAddVariableEvent: public ATOM_Event
{
public:
	enum VarType
	{
		VT_VAR,
		VT_BUTTON,
		VT_SEPARATOR,
		VT_TEXT
	};

	ATOMX_TWAddVariableEvent (void);
	ATOMX_TWAddVariableEvent (const char *name_, int id_, const char *group_, void *userdata_, ATOMX_TweakBar *bar_, VarType type_, bool allow_);

	int id;
	void *userdata;
	const char *name;
	const char *group;
	ATOMX_TweakBar *bar;
	VarType type;
	bool allow;

	ATOM_DECLARE_EVENT(ATOMX_TWAddVariableEvent)
};

class ATOMX_TWValueChangedEvent: public ATOM_Event
{
public:
	ATOMX_TWValueChangedEvent (void);
	ATOMX_TWValueChangedEvent (const char *name_, const ATOMX_TBValue &oldvar_, const ATOMX_TBValue &newval_, int id_, void *userdata_, ATOMX_TweakBar *bar_);
	int id;
	void *userdata;
	ATOM_STRING name;
	ATOMX_TBValue oldValue;
	ATOMX_TBValue newValue;
	ATOMX_TweakBar *bar;

	ATOM_DECLARE_EVENT(ATOMX_TWValueChangedEvent)
};

class ATOMX_TWCommandEvent: public ATOM_Event
{
public:
	ATOMX_TWCommandEvent (void);
	ATOMX_TWCommandEvent (const char *name_, int id_, void *userdata_, ATOMX_TweakBar *bar_);
	ATOM_STRING name;
	int id;
	void *userdata;
	ATOMX_TweakBar *bar;

	ATOM_DECLARE_EVENT(ATOMX_TWCommandEvent)
};

#endif // __ATOM3DX_TW_EVENTS_H