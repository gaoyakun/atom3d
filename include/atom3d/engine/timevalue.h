#ifndef __ATOM3D_ENGINE_TIMEVALUE_H
#define __ATOM3D_ENGINE_TIMEVALUE_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "basedefs.h"
#include "timemanager.h"

class ATOM_ENGINE_API ATOM_TimeValue
{
public:
	ATOM_TimeValue (void);
	ATOM_TimeValue (const ATOM_TimeValue &other);
	virtual ~ATOM_TimeValue (void);

public:
	ATOM_TimeValue &operator = (const ATOM_TimeValue &other);

public:
	void setCurrentTime (float time);
	void setSlot (int slot);
	int getSlot (void) const;

protected:
	virtual void _setCurrentTime (float time) = 0;

private:
	int _slot;
};

template <class T, int S = ATOM_TimeManager::numTimeKeys>
class ATOM_TimeValueT: public ATOM_TimeValue
{
public:
	ATOM_TimeValueT (void);
	ATOM_TimeValueT (const T &value);

public:
	void setKeyValue (int time, const T &value);
	void setKeyValues (const T &value);
	const T &getKeyValue (int time) const;
	const T &getCurrentValue (void) const;

protected:
	virtual void _setCurrentTime (float time);

private:
	T _keyValues[S];
	T _currentValue;
};

template <class T, int S>
ATOM_TimeValueT<T, S>::ATOM_TimeValueT (void)
{
	_setCurrentTime (ATOM_TimeManager::getCurrentTime ());
}

template <class T, int S>
ATOM_TimeValueT<T, S>::ATOM_TimeValueT (const T &value)
{
	setKeyValues (value);
}

template <class T, int S>
void ATOM_TimeValueT<T, S>::setKeyValues (const T &value)
{
	for (int i = 0; i < S; ++i)
	{
		_keyValues[i] = value;
	}
	_setCurrentTime (ATOM_TimeManager::getCurrentTime ());
}

template <class T, int S>
void ATOM_TimeValueT<T, S>::setKeyValue (int time, const T &value)
{
	_keyValues[time % S] = value;
	_setCurrentTime (ATOM_TimeManager::getCurrentTime ());
}

template <class T, int S>
const T &ATOM_TimeValueT<T, S>::getKeyValue (int time) const
{
	return _keyValues[time % S];
}

template <class T, int S>
const T &ATOM_TimeValueT<T, S>::getCurrentValue (void) const
{
	return _currentValue;
}

template <class T, int S>
void ATOM_TimeValueT<T, S>::_setCurrentTime (float time)
{
	int itime = ATOM_ftol (time);
	int t0 = itime % S;
	int t1 = (t0 + 1) % S;
	const T &value0 = getKeyValue (t0);
	const T &value1 = getKeyValue (t1);
	_currentValue = value0 + (value1 - value0) * (time - itime);
}

#endif // __ATOM3D_ENGINE_TIMEVALUE_H
