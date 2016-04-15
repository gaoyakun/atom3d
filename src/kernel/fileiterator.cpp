#include "StdAfx.h"

ATOM_FileIterator::ATOM_FileIterator (void)
{
	_findData = nullptr;
	_valid = false;
}

ATOM_FileIterator::ATOM_FileIterator (const char *dir)
{
	open (dir);
}

ATOM_FileIterator::~ATOM_FileIterator (void)
{
	close ();
}

ATOM_VFS::finddata_t *ATOM_FileIterator::getData (void) const
{
	return _valid ? _findData : nullptr;
}

void ATOM_FileIterator::open (const char *dir)
{
	_findData = ATOM_FindFirst (dir);
	_valid = (_findData != 0);
}

void ATOM_FileIterator::close (void)
{
	if (_valid)
	{
		ATOM_CloseFind (_findData);
		_findData = nullptr;
		_valid = false;
	}
}

void ATOM_FileIterator::next (void)
{
	if (_valid)
	{
		_valid = ATOM_FindNext (_findData);
	}
}

bool ATOM_FileIterator::isValid (void) const
{
	return _valid;
}
