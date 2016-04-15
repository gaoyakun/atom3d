#include "StdAfx.h"
#include "idmanager.h"

ATOM_SET<int> IdManager::_idSet;

bool IdManager::findId (int id)
{
	return _idSet.find (id) != _idSet.end ();
}

bool IdManager::addId (int id)
{
	if (findId (id))
	{
		return false;
	}
	_idSet.insert (id);
	return true;
}

bool IdManager::removeId (int id)
{
	ATOM_SET<int>::iterator it = _idSet.find (id);
	if (it != _idSet.end ())
	{
		_idSet.erase (it);
		return true;
	}
	return false;
}

void IdManager::clear (void)
{
	_idSet.clear ();
}

int IdManager::aquireId (int minValue)
{
	int n = minValue;

	while (findId (n))
	{
		++n;
	}

	addId (n);
	return n;
}

int IdManager::peekId (int minValue)
{
	int n = minValue;

	while (findId (n))
	{
		++n;
	}

	return n;
}

