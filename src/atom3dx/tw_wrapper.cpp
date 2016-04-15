#include "StdAfx.h"
#include "tw_wrapper.h"

ATOMX_API bool ATOM_CALL ATOMX_LoadTweakBarLib (void)
{
	return true;
}

ATOMX_API void ATOM_CALL ATOMX_UnloadTweakBarLib (void)
{
	TwDeleteAllBars ();
	TwTerminate ();
}

