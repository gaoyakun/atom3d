#ifndef __ATOM3D_ENGINE_TIMEMANAGER_H
#define __ATOM3D_ENGINE_TIMEMANAGER_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_dbghlp.h"

#include "basedefs.h"

class ATOM_TimeValue;

class ATOM_ENGINE_API ATOM_TimeManager
{
	class TimeValueManager
	{
	public:
		~TimeValueManager (void);
		void registerTimeValue (ATOM_TimeValue *timeValue);
		void unregisterTimeValue (ATOM_TimeValue *timeValue);
		void setCurrentTime (float time);
	private:
		ATOM_VECTOR<ATOM_TimeValue*> _timeValues;
		ATOM_VECTOR<int> _freeSlots;
	};

public:
	static const int numTimeKeys = 8;

public:
	static void setCurrentTime (float time);
	static float getCurrentTime (void);
	static void registerTimeValue (ATOM_TimeValue *timeValue);
	static void unregisterTimeValue (ATOM_TimeValue *timeValue);

private:
	static float _currentTime;
	static TimeValueManager &getTimeValueManager (void);
};

#endif // __ATOM3D_ENGINE_TIMEMANAGER_H
