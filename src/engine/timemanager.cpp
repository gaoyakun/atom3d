#include "StdAfx.h"
#include "timemanager.h"

float ATOM_TimeManager::_currentTime = 0.f;

void ATOM_TimeManager::setCurrentTime (float time)
{
	if (time != _currentTime)
	{
		_currentTime = time;

		getTimeValueManager().setCurrentTime (time);
	}
}

float ATOM_TimeManager::getCurrentTime (void)
{
	return _currentTime;
}

void ATOM_TimeManager::registerTimeValue (ATOM_TimeValue *timeValue)
{
	getTimeValueManager().registerTimeValue (timeValue);
}

void ATOM_TimeManager::unregisterTimeValue (ATOM_TimeValue *timeValue)
{
	getTimeValueManager().unregisterTimeValue (timeValue);
}

ATOM_TimeManager::TimeValueManager::~TimeValueManager (void)
{
	for (unsigned i = 0; i < _timeValues.size(); ++i)
	{
		if (_timeValues[i])
		{
		    ATOM_ASSERT(_timeValues[i]->getSlot() == i);
			_timeValues[i]->setSlot (-1);
			_timeValues[i] = 0;
		}
	}
	_freeSlots.clear ();
}

void ATOM_TimeManager::TimeValueManager::registerTimeValue (ATOM_TimeValue *timeValue)
{
	if (timeValue && timeValue->getSlot() < 0)
	{
		int slot = -1;

		if (_freeSlots.empty ())
		{
			slot = _timeValues.size();
			_timeValues.resize (_timeValues.size() + 1);
		}
		else
		{
			slot = _freeSlots.back();
			_freeSlots.pop_back ();
		}
		_timeValues[slot] = timeValue;
		timeValue->setSlot (slot);
	}
}

void ATOM_TimeManager::TimeValueManager::unregisterTimeValue (ATOM_TimeValue *timeValue)
{
	if (timeValue && timeValue->getSlot () >= 0)
	{
		_timeValues[timeValue->getSlot()] = 0;
		_freeSlots.push_back (timeValue->getSlot());
		timeValue->setSlot (-1);
	}
}

void ATOM_TimeManager::TimeValueManager::setCurrentTime (float time)
{
	for (int i = 0; i < _timeValues.size(); ++i)
	{
		if (_timeValues[i])
		{
			_timeValues[i]->setCurrentTime (time);
		}
	}
}

ATOM_TimeManager::TimeValueManager &ATOM_TimeManager::getTimeValueManager (void)
{
	static ATOM_TimeManager::TimeValueManager manager;

	return manager;
}

