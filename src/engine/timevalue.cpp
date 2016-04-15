#include "StdAfx.h"
#include "timevalue.h"

ATOM_TimeValue::ATOM_TimeValue (void)
{
	_slot = -1;
	ATOM_TimeManager::registerTimeValue (this);
}

ATOM_TimeValue::ATOM_TimeValue (const ATOM_TimeValue &other)
{
	ATOM_TimeManager::registerTimeValue (this);
}

ATOM_TimeValue::~ATOM_TimeValue (void)
{
	if (_slot >= 0)
	{
		ATOM_TimeManager::unregisterTimeValue (this);
	}
}

ATOM_TimeValue & ATOM_TimeValue::operator = (const ATOM_TimeValue &other)
{
	return *this;
}

void ATOM_TimeValue::setCurrentTime (float time)
{
	_setCurrentTime (time);
}

void ATOM_TimeValue::setSlot (int slot)
{
	_slot = slot;
}

int ATOM_TimeValue::getSlot (void) const
{
	return _slot;
}

