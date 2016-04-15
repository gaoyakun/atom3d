#include "StdAfx.h"
#include "tw_events.h"

ATOMX_TWAddVariableEvent::ATOMX_TWAddVariableEvent (void)
{
}

ATOMX_TWAddVariableEvent::ATOMX_TWAddVariableEvent (const char *name_, int id_, const char *group_, void *userdata_, ATOMX_TweakBar *bar_, VarType type_, bool allow_)
	: name(name_)
	, group(group_)
	, id(id_)
	, userdata(userdata_)
	, bar(bar_)
	, type(type_)
	, allow(allow_)
{
}

ATOMX_TWValueChangedEvent::ATOMX_TWValueChangedEvent (void)
{
}

ATOMX_TWValueChangedEvent::ATOMX_TWValueChangedEvent (const char *name_, const ATOMX_TBValue &oldvar_, const ATOMX_TBValue &newvar_, int id_, void *userdata_, ATOMX_TweakBar *bar_)
	: name(name_)
	, id(id_)
	, userdata(userdata_)
	, oldValue(oldvar_)
	, newValue(newvar_)
	, bar(bar_)
{
}

ATOMX_TWCommandEvent::ATOMX_TWCommandEvent (void)
{
}

ATOMX_TWCommandEvent::ATOMX_TWCommandEvent (const char *name_, int id_, void *userdata_, ATOMX_TweakBar *bar_)
	: name(name_)
	, id (id_)
	, userdata (userdata_)
	, bar(bar_)
{
}

