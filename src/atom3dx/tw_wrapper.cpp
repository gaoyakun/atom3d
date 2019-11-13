#include "StdAfx.h"
#include "tw_wrapper.h"

#define TwDllName _T("AntTweakBar.dll")

HMODULE hTwDll = NULL;
Fn_TwNewBar TwNewBar = NULL;
Fn_TwDeleteBar TwDeleteBar = NULL;
Fn_TwDeleteAllBars TwDeleteAllBars = NULL;
Fn_TwSetTopBar TwSetTopBar = NULL;
Fn_TwGetTopBar TwGetTopBar = NULL;
Fn_TwSetBottomBar TwSetBottomBar = NULL;
Fn_TwGetBottomBar TwGetBottomBar = NULL;
Fn_TwGetBarName TwGetBarName = NULL;
Fn_TwGetBarCount TwGetBarCount = NULL;
Fn_TwGetBarByIndex TwGetBarByIndex = NULL;
Fn_TwGetBarByName TwGetBarByName = NULL;
Fn_TwRefreshBar TwRefreshBar = NULL;
Fn_TwAddVarRW TwAddVarRW = NULL;
Fn_TwAddVarRO TwAddVarRO = NULL;
Fn_TwAddVarCB TwAddVarCB = NULL;
Fn_TwAddButton TwAddButton = NULL;
Fn_TwAddSeparator TwAddSeparator = NULL;
Fn_TwRemoveVar TwRemoveVar = NULL;
Fn_TwRemoveAllVars TwRemoveAllVars = NULL;
Fn_TwDefine TwDefine = NULL;
Fn_TwDefineEnum TwDefineEnum = NULL;
Fn_TwDefineStruct TwDefineStruct = NULL;
Fn_TwCopyCDStringToClientFunc TwCopyCDStringToClientFunc = NULL;
Fn_TwCopyCDStringToLibrary TwCopyCDStringToLibrary = NULL;
Fn_TwCopyStdStringToClientFunc TwCopyStdStringToClientFunc = NULL;
Fn_TwCopyStdStringToLibrary TwCopyStdStringToLibrary = NULL;
Fn_TwCursorAccessFunc TwCursorAccessFunc = NULL;
Fn_TwGetParam TwGetParam = NULL;
Fn_TwSetParam TwSetParam = NULL;
Fn_TwInit TwInit = NULL;
Fn_TwTerminate TwTerminate = NULL;
Fn_TwGraphCreateFunc TwGraphCreateFunc = NULL;
Fn_TwDraw TwDraw = NULL;
Fn_TwWindowSize TwWindowSize = NULL;
Fn_TwKeyPressed TwKeyPressed = NULL;
Fn_TwCharEntered TwCharEntered = NULL;
Fn_TwMouseButton TwMouseButton = NULL;
Fn_TwMouseMotion TwMouseMotion = NULL;
Fn_TwMouseWheel TwMouseWheel = NULL;
Fn_TwGetLastError TwGetLastError = NULL;
Fn_TwHandleErrors TwHandleErrors = NULL;
Fn_TwEventWin TwEventWin = NULL;

#define LOAD_FUNC(func) do { func = (Fn_##func)::GetProcAddress(hTwDll, #func); if (!func) { ::FreeLibrary(hTwDll); hTwDll = NULL; return false; } } while (0);
ATOMX_API bool ATOM_CALL ATOMX_LoadTweakBarLib (void)
{
	if (!hTwDll) 
	{
		hTwDll = ::LoadLibrary(TwDllName);
		if (!hTwDll) 
		{
			return false;
		}
		LOAD_FUNC(TwNewBar);
		LOAD_FUNC(TwDeleteBar);
		LOAD_FUNC(TwDeleteAllBars);
		LOAD_FUNC(TwSetTopBar);
		LOAD_FUNC(TwGetTopBar);
		LOAD_FUNC(TwSetBottomBar);
		LOAD_FUNC(TwGetBottomBar);
		LOAD_FUNC(TwGetBarName);
		LOAD_FUNC(TwGetBarCount);
		LOAD_FUNC(TwGetBarByIndex);
		LOAD_FUNC(TwGetBarByName);
		LOAD_FUNC(TwRefreshBar);
		LOAD_FUNC(TwAddVarRW);
		LOAD_FUNC(TwAddVarRO);
		LOAD_FUNC(TwAddVarCB);
		LOAD_FUNC(TwAddButton);
		LOAD_FUNC(TwAddSeparator);
		LOAD_FUNC(TwRemoveVar);
		LOAD_FUNC(TwRemoveAllVars);
		LOAD_FUNC(TwDefine);
		LOAD_FUNC(TwDefineEnum);
		LOAD_FUNC(TwDefineStruct);
		LOAD_FUNC(TwCopyCDStringToClientFunc);
		LOAD_FUNC(TwCopyCDStringToLibrary);
		LOAD_FUNC(TwCopyStdStringToClientFunc);
		LOAD_FUNC(TwCopyStdStringToLibrary);
		LOAD_FUNC(TwCursorAccessFunc);
		LOAD_FUNC(TwGetParam);
		LOAD_FUNC(TwSetParam);
		LOAD_FUNC(TwInit);
		LOAD_FUNC(TwTerminate);
		LOAD_FUNC(TwGraphCreateFunc);
		LOAD_FUNC(TwDraw);
		LOAD_FUNC(TwWindowSize);
		LOAD_FUNC(TwKeyPressed);
		LOAD_FUNC(TwCharEntered);
		LOAD_FUNC(TwMouseButton);
		LOAD_FUNC(TwMouseMotion);
		LOAD_FUNC(TwMouseWheel);
		LOAD_FUNC(TwGetLastError);
		LOAD_FUNC(TwHandleErrors);
		LOAD_FUNC(TwEventWin);
	}
	return true;
}

ATOMX_API void ATOM_CALL ATOMX_UnloadTweakBarLib (void)
{
	if (hTwDll) {
		TwDeleteAllBars();
		TwTerminate();
	}
}

