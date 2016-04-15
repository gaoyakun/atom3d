#ifndef __ATOM3D_ENGINE_ENGINEAPI_H
#define __ATOM3D_ENGINE_ENGINEAPI_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_render.h"
#include "../ATOM_script.h"

struct ATOM_RenderWindowDesc
{
	unsigned width;
	unsigned height;
	bool fullscreen;
	bool naked;
	bool resizable;
	int multisample;
	const char *title;
	void *iconid;
	void *windowid;
	void *parentWindowId;
};

struct ATOM_DisplayDeviceInfo
{
	ATOM_STRING desc;
	ATOM_STRING manufacturer;
	ATOM_STRING chipType;
	ATOM_STRING memorySize;
	ATOM_STRING displayMode;
	ATOM_STRING driverName;
	ATOM_STRING driverVersion;
	ATOM_STRING driverDate;
	ATOM_STRING directXVersion;
	unsigned vendorId;
	unsigned deviceId;
	unsigned subsysId;
	unsigned revisionId;
	bool ddrawAccelEnabled;
	bool d3dAccelEnabled;
	bool AGPEnabled;
	bool d3dAccelExists;
	bool AGPExists;
};

ATOM_ENGINE_API bool ATOM_CALL ATOM_IsInitialized (void);
ATOM_ENGINE_API ATOM_AUTOREF(ATOM_RenderWindow) ATOM_CALL ATOM_InitializeGfx (const ATOM_RenderWindowDesc &primaryWindowDesc, ATOM_RenderWindow::ShowMode showMode, bool contentBackup, void *instance);
ATOM_ENGINE_API bool ATOM_CALL ATOM_ToggleFullScreen (void);
ATOM_ENGINE_API void ATOM_CALL ATOM_DoneGfx (void);
ATOM_ENGINE_API ATOM_RenderDevice * ATOM_CALL ATOM_GetRenderDevice (void);
ATOM_ENGINE_API const ATOM_FrameStamp & ATOM_CALL ATOM_GetFrameStamp (void);
ATOM_ENGINE_API void ATOM_CALL ATOM_SetTextureShareDirectory (const char *dir);
ATOM_ENGINE_API const char * ATOM_CALL ATOM_GetTextureShareDirectory (void);
ATOM_ENGINE_API void ATOM_CALL ATOM_SetModelShareDirectory (const char *dir);
ATOM_ENGINE_API const char * ATOM_CALL ATOM_GetModelShareDirectory (void);
ATOM_ENGINE_API ATOM_Vector3f CalcTangentVector(
  const ATOM_Vector3f &x,
  const ATOM_Vector3f &y,
  const ATOM_Vector3f &z,
  const ATOM_Vector2f &uv_x,
  const ATOM_Vector2f &uv_y,
  const ATOM_Vector2f &uv_z
  );
ATOM_ENGINE_API ATOM_Texture *ATOM_GetColorTexture (ATOM_ColorARGB color);
//--- wangjian modified ---//
// 异步加载：增加了一个异步加载优先级（默认异步加载）
ATOM_ENGINE_API ATOM_AUTOREF(ATOM_Texture) ATOM_CreateTextureResource (	const char *filename, ATOM_PixelFormat format = ATOM_PIXEL_FORMAT_UNKNOWN, 
																		int loadPriority = ATOM_LoadPriority_ASYNCBASE );
//-------------------------//
ATOM_ENGINE_API ATOM_AUTOREF(ATOM_Texture) ATOM_FindTextureResource (const char *filename);
ATOM_ENGINE_API ATOM_AUTOREF(ATOM_Texture) ATOM_FindOrCreateEmptyTextureResource (const char *filename);
ATOM_ENGINE_API ATOM_AUTOREF(ATOM_Texture) ATOM_CreateEmptyTextureResource (const char *filename);
ATOM_ENGINE_API const char *ATOM_GetPrimaryDeviceId (void);
ATOM_ENGINE_API const char *ATOM_GetPrimaryVendorId (void);
ATOM_ENGINE_API unsigned ATOM_GetNumDisplayDevices (void);
ATOM_ENGINE_API const ATOM_DisplayDeviceInfo &ATOM_GetDisplayDeviceInfo (unsigned device);
ATOM_ENGINE_API void ATOM_DumpEngineObjects (const char *filename);
ATOM_ENGINE_API void ATOM_BindToScript (ATOM_Script *script);
ATOM_ENGINE_API void ATOM_BindSystemToScript (ATOM_Script *script);
ATOM_ENGINE_API void ATOM_BindMiscToScript (ATOM_Script *script);
ATOM_ENGINE_API void ATOM_BindDbgHlpToScript (ATOM_Script *script);
ATOM_ENGINE_API void ATOM_BindKernelToScript (ATOM_Script *script);
ATOM_ENGINE_API void ATOM_BindRenderToScript (ATOM_Script *script);
ATOM_ENGINE_API void ATOM_BindEngineToScript (ATOM_Script *script);

#endif // __ATOM3D_ENGINE_ENGINEAPI_H
