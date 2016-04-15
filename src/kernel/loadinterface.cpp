#include "StdAfx.h"
#include "loadinterface.h"

ATOM_LoadInterface::ATOM_LoadInterface (void)
{
	_state = LS_NOTLOADED;
}

ATOM_LoadInterface::~ATOM_LoadInterface (void)
{
}

const char *ATOM_LoadInterface::getFileName (void) const
{
	return _filename.c_str ();
}

void ATOM_LoadInterface::setFileName (const char *filename)
{
	_filename = filename;
}

ATOM_LoadInterface::LoadingState ATOM_LoadInterface::getLoadingState (void) const
{
	return _state;
}

void ATOM_LoadInterface::setLoadingState (ATOM_LoadInterface::LoadingState state)
{
	_state = state;
}

void ATOM_LoadInterface::insureLoadDone (void)
{
	if (_state == LS_LOADING)
	{
		ATOM_ContentStream::waitForInterfaceDone (this);
	}
}

