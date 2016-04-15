#include "stdafx.h"
#include "engineapi.h"

/* DeviceID list
{ 0x00F1, "GeForce 6600 GT" },
{ 0x00F2, "GeForce 6600" },
{ 0x00F3, "GeForce 6200" },
{ 0x00F4, "GeForce 6600 LE" },
{ 0x00F5, "GeForce 7800 GS" },
{ 0x00F6, "GeForce 6800 GS/XT" },
{ 0x00F8, "Quadro FX 3400/4400" },
{ 0x00F9, "GeForce 6800 Series GPU" },
{ 0x02E0, "GeForce 7600 GT" },
{ 0x02E1, "GeForce 7600 GS" },
{ 0x02E2, "GeForce 7300 GT" },
{ 0x02E3, "GeForce 7900 GS" },
{ 0x02E4, "GeForce 7950 GT" },
{ 0x0090, "GeForce 7800 GTX" },
{ 0x0091, "GeForce 7800 GTX " },
{ 0x0092, "GeForce 7800 GT" },
{ 0x0093, "GeForce 7800 GS " },
{ 0x0095, "GeForce 7800 SLI" },
{ 0x009D, "Quadro FX 4500" },
{ 0x0290, "GeForce 7900 GTX" },
{ 0x0291, "GeForce 7900 GT/GTO" },
{ 0x0292, "GeForce 7900 GS " },
{ 0x0293, "GeForce 7950 GX2" },
{ 0x0294, "GeForce 7950 GX2 " },
{ 0x0295, "GeForce 7950 GT " },
{ 0x029C, "Quadro FX 5500" },
{ 0x029D, "Quadro FX 3500" },
{ 0x029E, "Quadro FX 1500" },
{ 0x029F, "Quadro FX 4500 X2" },
{ 0x01D0, "GeForce 7350 LE" },
{ 0x01D1, "GeForce 7300 LE" },
{ 0x01D3, "GeForce 7300 SE/7200 GS" },
{ 0x01DB, "Quadro NVS 120M" },
{ 0x01DD, "GeForce 7500 LE" },
{ 0x01DE, "Quadro FX 350" },
{ 0x01DF, "GeForce 7300 GS" },
{ 0x0390, "GeForce 7650 GS" },
{ 0x0391, "GeForce 7600 GT " },
{ 0x0392, "GeForce 7600 GS " },
{ 0x0393, "GeForce 7300 GT " },
{ 0x0394, "GeForce 7600 LE" },
{ 0x0395, "GeForce 7300 GT  " },
{ 0x039E, "Quadro FX 560" },
{ 0x0191, "GeForce 8800 GTX" },
{ 0x0193, "GeForce 8800 GTS" },
{ 0x0194, "GeForce 8800 Ultra" },
{ 0x0197, "Tesla C870" },
{ 0x019D, "Quadro FX 5600" },
{ 0x019E, "Quadro FX 4600" },
{ 0x0400, "GeForce 8600 GTS" },
{ 0x0401, "GeForce 8600 GT" },
{ 0x0402, "GeForce 8600 GT " },
{ 0x0403, "GeForce 8600GS" },
{ 0x0404, "GeForce 8400 GS" },
{ 0x0406, "GeForce 8300 GS" },
{ 0x040A, "Quadro FX 370" },
{ 0x040E, "Quadro FX 570" },
{ 0x040F, "Quadro FX 1700" },
{ 0x0420, "GeForce 8400 SE" },
{ 0x0421, "GeForce 8500 GT" },
{ 0x0422, "GeForce 8400 GS " },
{ 0x0423, "GeForce 8300 GS " },
{ 0x0424, "GeForce 8400 GS  " },
{ 0x042C, "GeForce 9400 GT" },
{ 0x042F, "Quadro NVS 290" },
{ 0x0600, "GeForce 8800 GTS 512" },
{ 0x0601, "GeForce 9800 GT" },
{ 0x0602, "GeForce 8800 GT" },
{ 0x0604, "GeForce 9800 GX2" },
{ 0x0605, "GeForce 9800 GT " },
{ 0x0606, "GeForce 8800 GS" },
{ 0x0610, "GeForce 9600 GSO" },
{ 0x0611, "GeForce 8800 GT " },
{ 0x0612, "GeForce 9800 GTX/9800 GTX+" },
{ 0x0613, "GeForce 9800 GTX+" },
{ 0x0614, "GeForce 9800 GT  " },
{ 0x061A, "Quadro FX 3700" },
{ 0x0622, "GeForce 9600 GT" },
{ 0x0623, "GeForce 9600 GS" },
{ 0x0640, "GeForce 9500 GT" },
{ 0x0641, "GeForce 9400 GT " },
{ 0x0643, "GeForce 9500 GT " },
{ 0x0644, "GeForce 9500 GS" },
{ 0x0645, "GeForce 9500 GS " },
{ 0x06E0, "GeForce 9300 GE" },
{ 0x06E1, "GeForce 9300 GS" },
{ 0x06E2, "GeForce 8400" },
{ 0x06E3, "GeForce 8400 SE " },
{ 0x06E4, "GeForce 8400 GS   " },
{ 0x06F9, "Quadro FX 370 LP" },
{ 0x05E1, "GeForce GTX 280" },
{ 0x05E2, "GeForce GTX 260" },
{ 0x05FD, "Quadro FX 5800" },
{ 0x05FE, "Quadro FX 4800" },
{ 0x0040, "GeForce 6800 Ultra" },
{ 0x0041, "GeForce 6800" },
{ 0x0042, "GeForce 6800 LE" },
{ 0x0043, "GeForce 6800 XE" },
{ 0x0044, "GeForce 6800 XT" },
{ 0x0045, "GeForce 6800 GT" },
{ 0x0047, "GeForce 6800 GS" },
{ 0x0048, "GeForce 6800 XT " },
{ 0x004E, "Quadro FX 4000" },
{ 0x00C0, "GeForce 6800 GS " },
{ 0x00C1, "GeForce 6800 " },
{ 0x00C2, "GeForce 6800 LE " },
{ 0x00C3, "GeForce 6800 XT  " },
{ 0x00CD, "Quadro FX 3450/4000 SDI" },
{ 0x00CE, "Quadro FX 1400" },
{ 0x0140, "GeForce 6600 GT " },
{ 0x0141, "GeForce 6600 " },
{ 0x0142, "GeForce 6600 LE " },
{ 0x0143, "GeForce 6600 VE" },
{ 0x0145, "GeForce 6610 XL" },
{ 0x0147, "GeForce 6700 XL" },
{ 0x014A, "Quadro NVS 440" },
{ 0x014C, "Quadro FX 540M" },
{ 0x014D, "Quadro FX 550" },
{ 0x014E, "Quadro FX 540" },
{ 0x014F, "GeForce 6200 " },
{ 0x0160, "GeForce 6500" },
{ 0x0161, "GeForce 6200 TurboCache(TM)" },
{ 0x0162, "GeForce 6200SE TurboCache(TM)" },
{ 0x0163, "GeForce 6200 LE" },
{ 0x0165, "Quadro NVS 285" },
{ 0x0169, "GeForce 6250" },
{ 0x016A, "GeForce 7100 GS" },
{ 0x0221, "GeForce 6200  " },
{ 0x0222, "GeForce 6200 A-LE" },
{ 0x0046, "GeForce 6800 GT " },
{ 0x0211, "GeForce 6800  " },
{ 0x0212, "GeForce 6800 LE  " },
{ 0x0215, "GeForce 6800 GT  " },
{ 0x0218, "GeForce 6800 XT   " },
{ 0x0240, "GeForce 6150" },
{ 0x0241, "GeForce 6150 LE" },
{ 0x0242, "GeForce 6100" },
{ 0x0245, "Quadro NVS 210S / NVIDIA GeForce 6150LE" },
{ 0x03D0, "GeForce 6150SE nForce 430" },
{ 0x03D1, "GeForce 6100 nForce 405" },
{ 0x03D2, "GeForce 6100 nForce 400" },
{ 0x03D5, "GeForce 6100 nForce 420" },
{ 0x053A, "GeForce 7050 PV / NVIDIA nForce 630a" },
{ 0x053B, "GeForce 7050 PV / NVIDIA nForce 630a " },
{ 0x053E, "GeForce 7025 / NVIDIA nForce 630a" },
{ 0x07E0, "GeForce 7150 / NVIDIA nForce 630i" },
{ 0x07E1, "GeForce 7100 / NVIDIA nForce 630i" },
{ 0x07E2, "GeForce 7050 / NVIDIA nForce 630i" },
{ 0x07E3, "GeForce 7050 / NVIDIA nForce 610i" },
{ 0x07E5, "GeForce 7050 / NVIDIA nForce 620i" },
{ 0x0848, "GeForce 8300" },
{ 0x0849, "GeForce 8200" },
{ 0x084A, "nForce 730a" },
{ 0x084B, "GeForce 8200 " },
{ 0x084C, "nForce 780a SLI" },
{ 0x084D, "nForce 750a SLI" },
{ 0x084F, "GeForce 8100 / nForce 720a" },
*/

bool _S_deviceIdGot = false;
ATOM_STRING _S_deviceId;
ATOM_STRING _S_vendorId;

ATOM_AUTOREF(ATOM_Renderer) _S_renderer;
ATOM_STRING _S_texture_share_directory = "/";
ATOM_STRING _S_model_share_directory = "/";
ATOM_HASHMAP<unsigned, ATOM_AUTOREF(ATOM_Texture)> _S_color_textures;

static PCHAR*
cmdlineToArgvA(
	PCHAR CmdLine,
	int* _argc
	)
{
	PCHAR* argv;
	PCHAR  _argv;
	ULONG   len;
	ULONG   argc;
	CHAR   a;
	ULONG   i, j;

	BOOLEAN  in_QM;
	BOOLEAN  in_TEXT;
	BOOLEAN  in_SPACE;

	len = strlen(CmdLine);
	i = ((len+2)/2)*sizeof(PVOID) + sizeof(PVOID);

	argv = (PCHAR*)GlobalAlloc(GMEM_FIXED,
		i + (len+2)*sizeof(CHAR));

	_argv = (PCHAR)(((PUCHAR)argv)+i);

	argc = 0;
	argv[argc] = _argv;
	in_QM = FALSE;
	in_TEXT = FALSE;
	in_SPACE = TRUE;
	i = 0;
	j = 0;

	while( a = CmdLine[i] ) {
		if(in_QM) {
			if(a == '\"') {
				in_QM = FALSE;
			} else {
				_argv[j] = a;
				j++;
			}
		} else {
			switch(a) {
			case '\"':
				in_QM = TRUE;
				in_TEXT = TRUE;
				if(in_SPACE) {
					argv[argc] = _argv+j;
					argc++;
				}
				in_SPACE = FALSE;
				break;
			case ' ':
			case '\t':
			case '\n':
			case '\r':
				if(in_TEXT) {
					_argv[j] = '\0';
					j++;
				}
				in_TEXT = FALSE;
				in_SPACE = TRUE;
				break;
			default:
				in_TEXT = TRUE;
				if(in_SPACE) {
					argv[argc] = _argv+j;
					argc++;
				}
				_argv[j] = a;
				j++;
				in_SPACE = FALSE;
				break;
			}
		}
		i++;
	}
	_argv[j] = '\0';
	argv[argc] = NULL;

	(*_argc) = argc;
	return argv;
}

ATOM_ENGINE_API ATOM_RenderDevice * ATOM_CALL ATOM_GetRenderDevice (void)
{
  return _S_renderer ? _S_renderer->getRenderDevice() : 0;
}

static void logSystemInformations (void)
{
	/*
	ATOM_STACK_TRACE(logSystemInformations);

	char regKeyBuffer[512];
	char valueNameBuffer[256];
	char cpuName[512];

	int c = 0;
	for (;;)
	{
		sprintf (regKeyBuffer, "Hardware\\Description\\System\\CentralProcessor\\%d", c);
		HKEY reg;
		if (ERROR_SUCCESS != ::RegOpenKeyExA (HKEY_LOCAL_MACHINE, regKeyBuffer, 0, KEY_QUERY_VALUE, &reg))
		{
			break;
		}
		int v = 0;
		for (;;)
		{
			DWORD numLen = 256;
			DWORD numName = 512;
			if (ERROR_SUCCESS != ::RegEnumValueA (reg, v, valueNameBuffer, &numLen, 0, 0, (LPBYTE)cpuName, &numName))
			{
				break;
			}
			if (!_stricmp (valueNameBuffer, "ProcessorNameString"))
			{
				ATOM_LOGGER::log ("CPU%d: %s\n", c, cpuName);
				break;
			}
			v++;
		}
		c++;
	}

	ATOM_MemoryInfo meminfo;
	ATOM_LOGGER::log ("MEM:  %s\n", meminfo.getTotalRamStr());

	const ATOM_DisplayDeviceInfo &dispinfo = ATOM_GetDisplayDeviceInfo (0);
	ATOM_LOGGER::log ("Device 0: %s\n", dispinfo.desc.c_str());
	ATOM_LOGGER::log ("Video memory: %s\n", dispinfo.memorySize.c_str());
	ATOM_LOGGER::log ("Display mode: %s\n", dispinfo.displayMode.c_str());
	ATOM_LOGGER::log ("Driver name: %s\n", dispinfo.driverName.c_str());
	ATOM_LOGGER::log ("Driver version: %s\n", dispinfo.driverVersion.c_str());
	ATOM_LOGGER::log ("Driver date: %s\n", dispinfo.driverDate.c_str());
	ATOM_LOGGER::log ("Device vendor Id: 0x%08X\n", dispinfo.vendorId);
	ATOM_LOGGER::log ("Device Id: 0x%08X\n", dispinfo.deviceId);
	ATOM_LOGGER::log ("D3D acceleration %s\n", dispinfo.d3dAccelEnabled ? "enabled" : "not enabled");
	ATOM_LOGGER::log ("AGP memory %s\n", dispinfo.AGPEnabled ? "enabled" : "not enabled");

	ATOM_SoundInfo soundinfo;
	if (soundinfo.getNumDevices() == 0)
	{
		ATOM_LOGGER::log ("SND:  Not detected\n");
	}
	else
	{
		for (unsigned i = 0; i < soundinfo.getNumDevices(); ++i)
		{
			ATOM_LOGGER::log ("SND:  %s (%s)\n", soundinfo.getDeviceDescription(i), soundinfo.getDeviceDriverName(i));
		}
	}

	ATOM_OSInfo osinfo;
	ATOM_LOGGER::log ("OS:   %s\n", osinfo.getOSName ());
	ATOM_LOGGER::log ("D3D:  %s\n\n", dispinfo.directXVersion.c_str());
	*/
}

ATOM_RenderWindow *__initializeGfx (ATOM_DisplayMode *mode, const ATOM_RenderWindowDesc &primaryWindowDesc, ATOM_RenderWindow::ShowMode showMode, bool contentBackup, void *instance)
{
	ATOM_STACK_TRACE(ATOM_InitializeGfx);

	if (!_S_renderer)
	{
		_S_renderer = ATOM_HARDREF(ATOM_Renderer)();
		ATOM_ASSERT(_S_renderer);

		unsigned w = primaryWindowDesc.width;
		unsigned h = primaryWindowDesc.height;
		DWORD WindowStyle = 0;

		if (mode == 0 && primaryWindowDesc.windowid == 0)
		{
			// window mode, check the window size

			// first calculate the actual window size
			if (primaryWindowDesc.naked)
			{
				WindowStyle = WS_POPUP;
			}
			else
			{
				WindowStyle |= WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX; 
				if (primaryWindowDesc.resizable)
				{
					WindowStyle |= WS_THICKFRAME | WS_MAXIMIZEBOX;
				}
			}

			RECT rc;
			rc.left = 100;
			rc.right = 200;
			rc.top = 100;
			rc.bottom = 200;
			::AdjustWindowRect (&rc, WindowStyle, FALSE);

			unsigned deltaW = rc.right - rc.left - 100;
			unsigned deltaH = rc.bottom - rc.top - 100;

			unsigned windowWidth = w + deltaW;
			unsigned windowHeight = h + deltaH;

			unsigned screenWidth = 0;
			unsigned screenHeight = 0;

			DEVMODE desktopMode;
			EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &desktopMode);
			screenWidth = desktopMode.dmPelsWidth;
			screenHeight = desktopMode.dmPelsHeight;

			if (!primaryWindowDesc.fullscreen)
			{
				RECT rc;
				if (::SystemParametersInfoA(SPI_GETWORKAREA, 0, &rc, 0))
				{
					screenWidth = rc.right - rc.left;
					screenHeight = rc.bottom - rc.top;
				}
			}

			if (windowWidth > screenWidth || windowHeight > screenHeight)
			{
				// Required size exceeds the screen size, need be adjusted

				ATOM_LOGGER::warning ("Required window size exceeds the screen size and will be truncated.\n");

				unsigned newWidth = ATOM_min2 (windowWidth, screenWidth);
				unsigned newHeight = ATOM_min2 (windowHeight, screenHeight);
				w -= (windowWidth - newWidth);
				h -= (windowHeight - newHeight);
			}
		}

		void *icon = primaryWindowDesc.iconid;
		if (!icon)
		{
			char szExePath[MAX_PATH];
			GetModuleFileNameA( NULL, szExePath, MAX_PATH );
			icon = ::ExtractIconA ( ::GetModuleHandleA(NULL), szExePath, 0 );
		}

		if (!_S_renderer->initialize (mode, w, h, showMode, primaryWindowDesc.naked, primaryWindowDesc.resizable, 0/*primaryWindowDesc.multisample*/, primaryWindowDesc.title, primaryWindowDesc.windowid, primaryWindowDesc.parentWindowId, primaryWindowDesc.iconid, contentBackup, instance))
		{
			_S_renderer = 0;
			return 0;
		}

		int argc = 0;
		PCHAR *argv = cmdlineToArgvA (::GetCommandLineA (), &argc);
		ATOM_RenderSettings::initialize (_S_renderer->getRenderDevice(), false, argc, argv);
		::GlobalFree (argv);
	}

	logSystemInformations ();

	return _S_renderer->getRenderWindow();
}

ATOM_ENGINE_API ATOM_AUTOREF(ATOM_RenderWindow) ATOM_CALL ATOM_InitializeGfx (const ATOM_RenderWindowDesc &primaryWindowDesc, ATOM_RenderWindow::ShowMode showMode, bool contentBackup, void *instance)
{
	ATOM_DisplayMode disp;
	ATOM_DisplayMode *pDisp = 0;

	if (primaryWindowDesc.fullscreen)
	{
		disp.width = primaryWindowDesc.width;
		disp.height = primaryWindowDesc.height;
		disp.depth = 24;
		disp.freq = 0;
		pDisp = &disp;
	}

	return __initializeGfx (pDisp, primaryWindowDesc, showMode, contentBackup, instance);
}

ATOM_ENGINE_API bool ATOM_CALL ATOM_ToggleFullScreen (void)
{
	if (_S_renderer)
	{
		ATOM_RenderWindow *w = _S_renderer->getRenderWindow();
		if (w)
		{
			return w->toggleFullScreen ();
		}
	}
	return false;
}

ATOM_ENGINE_API bool ATOM_CALL ATOM_IsInitialized ()
{
  return _S_renderer != 0;
}

ATOM_ENGINE_API void ATOM_CALL ATOM_DoneGfx ()
{
	ATOM_STACK_TRACE(ATOM_DoneGfx);

	//--- wangjian modifieid ---//
	// 去掉原来的异步加载方法调用
	//ATOM_ContentStream::waitForAllDone ();
	//--------------------------//

	//--- wangjian added ---//
	// 异步加载相关 销毁所有对象 并且停止运行
	//ATOM_AsyncLoader::AbandonAllTask(true);
	//----------------------//

	if (_S_renderer)
	{
		_S_renderer->finalize();
		_S_renderer = 0;
	}
}

ATOM_ENGINE_API ATOM_Vector3f CalcTangentVector(
  const ATOM_Vector3f &x,
  const ATOM_Vector3f &y,
  const ATOM_Vector3f &z,
  const ATOM_Vector2f &uv_x,
  const ATOM_Vector2f &uv_y,
  const ATOM_Vector2f &uv_z)
{
  //side0 is the vector along one side of the triangle of vertices passed in, 
  //and side1 is the vector along another side. Taking the cross product of these returns the normal.
  ATOM_Vector3f side0 = x - y;
  ATOM_Vector3f side1 = z - x;
  //Calculate face normal
  ATOM_Vector3f normal = crossProduct (side1, side0);
  normal.normalize();
  //Now we use a formula to calculate the tangent. 
  float deltaV0 = uv_x.y - uv_y.y;
  float deltaV1 = uv_z.y - uv_x.y;
  ATOM_Vector3f tangent = deltaV1 * side0 - deltaV0 * side1;
  tangent.normalize();
  //Calculate binormal
  float deltaU0 = uv_x.x - uv_y.x;
  float deltaU1 = uv_z.x - uv_x.x;
  ATOM_Vector3f binormal = deltaU1 * side0 - deltaU0 * side1;
  binormal.normalize ();
  //Now, we take the cross product of the tangents to get a vector which 
  //should point in the same direction as our normal calculated above. 
  //If it points in the opposite direction (the dot product between the normals is less than zero), 
  //then we need to reverse the s and t tangents. 
  //This is because the triangle has been mirrored when going from tangent space to object space.
  //reverse tangents if necessary
  ATOM_Vector3f tangentCross = crossProduct(tangent, binormal);
  if (dotProduct (tangentCross, normal) < 0.0f)
  {
    tangent = -tangent;
    binormal = -binormal;
  }

  return tangent;    
}

ATOM_ENGINE_API void ATOM_CALL ATOM_SetTextureShareDirectory (const char *dir)
{
	ATOM_ASSERT(dir && dir[0]);
	_S_texture_share_directory = dir;
	if (_S_texture_share_directory[_S_texture_share_directory.length()-1] != '/')
		_S_texture_share_directory += "/";
}

ATOM_ENGINE_API const char * ATOM_CALL ATOM_GetTextureShareDirectory (void)
{
	return _S_texture_share_directory.c_str();
}

ATOM_ENGINE_API void ATOM_CALL ATOM_SetModelShareDirectory (const char *dir)
{
	ATOM_ASSERT(dir && dir[0]);
	_S_model_share_directory = dir;
	if (_S_model_share_directory[_S_model_share_directory.length()-1] != '/')
		_S_model_share_directory += "/";
}

ATOM_ENGINE_API const char * ATOM_CALL ATOM_GetModelShareDirectory (void)
{
	return _S_model_share_directory.c_str();
}

ATOM_ENGINE_API ATOM_Texture *ATOM_GetColorTexture (ATOM_ColorARGB color)
{
	ATOM_STACK_TRACE(ATOM_GetColorTexture);

	ATOM_RenderDevice *device = ATOM_GetRenderDevice();
	if (device)
	{
		ATOM_HASHMAP<unsigned, ATOM_AUTOREF(ATOM_Texture)>::iterator it = _S_color_textures.find (unsigned(color));
		if (it == _S_color_textures.end ())
		{
			ATOM_AUTOREF(ATOM_Texture) texture = device->allocTexture(0, &color, 1, 1, ATOM_PIXEL_FORMAT_BGRA8888, ATOM_Texture::TEXTURE2D|ATOM_Texture::NOCOMPRESS|ATOM_Texture::NOMIPMAP);
			if (texture)
			{
				_S_color_textures[color] = texture;
				texture->getLoadInterface()->setLoadingState (ATOM_LoadInterface::LS_LOADED);
			}
			return texture.get ();
		}
		else
		{
			return it->second.get ();
		}
	}
	return 0;
}

//--- wangjian MODIFIED ---//
// 异步加载 ： 添加加载优先级
ATOM_ENGINE_API ATOM_AUTOREF(ATOM_Texture) ATOM_CreateTextureResource (const char *filename, ATOM_PixelFormat format, int loadPriority)
{
	ATOM_STACK_TRACE(ATOM_CreateTextureResource);

	char buffer[ATOM_VFS::max_filename_length];
	if (!filename || !ATOM_CompletePath (filename, buffer))
	{
		return 0;
	}

	_strlwr (buffer);
	//--- wangjian MODIFIED ---//
	// 异步加载 ： 添加加载优先级
	return ATOM_GetRenderDevice()->createTextureResource (	buffer, 
															ATOM_RenderSettings::getMaxTextureSizeLimit(), 
															ATOM_RenderSettings::getMaxTextureSizeLimit(), 
															format,
															loadPriority	);		// wangjian added
}

ATOM_ENGINE_API ATOM_AUTOREF(ATOM_Texture) ATOM_FindTextureResource (const char *filename)
{
	ATOM_STACK_TRACE(ATOM_FindTextureResource);

	char buffer[ATOM_VFS::max_filename_length];
	if (!filename || !ATOM_CompletePath (filename, buffer))
	{
		return 0;
	}

	_strlwr (buffer);
	return ATOM_GetRenderDevice()->findTextureResource (buffer);
}

ATOM_ENGINE_API ATOM_AUTOREF(ATOM_Texture) ATOM_FindOrCreateEmptyTextureResource (const char *filename)
{
	ATOM_STACK_TRACE(ATOM_FindOrCreateEmptyTextureResource);

	char buffer[ATOM_VFS::max_filename_length];
	if (!filename || !ATOM_CompletePath (filename, buffer))
	{
		return 0;
	}

	_strlwr (buffer);
	ATOM_AUTOREF(ATOM_Texture) texture = ATOM_GetRenderDevice()->findTextureResource (buffer);
	
	if (!texture)
	{
		texture = ATOM_GetRenderDevice()->allocTexture (buffer, ATOM_Texture::TEXTURE2D);
		texture->setRenderDevice (ATOM_GetRenderDevice());
	}

	return texture;
}

ATOM_ENGINE_API ATOM_AUTOREF(ATOM_Texture) ATOM_CreateEmptyTextureResource (const char *filename)
{
	ATOM_STACK_TRACE(ATOM_CreateEmptyTextureResource);

	char buffer[ATOM_VFS::max_filename_length];
	if (!filename || !ATOM_CompletePath (filename, buffer))
	{
		return 0;
	}
	_strlwr (buffer);

	ATOM_AUTOREF(ATOM_Texture) texture = ATOM_GetRenderDevice()->allocTexture (buffer, ATOM_Texture::TEXTURE2D|ATOM_Texture::NOCOMPRESS);
	texture->setRenderDevice(ATOM_GetRenderDevice());

	return texture;
}

extern unsigned sceneCount;
extern unsigned GUIrendererCount;
extern unsigned widgetCount;
extern unsigned meshCount;
extern unsigned modelCount;
extern unsigned terrainCount;
extern unsigned particlesysCount;
extern unsigned trackCount;
extern unsigned hudCount;

ATOM_ENGINE_API void ATOM_DumpEngineObjects (const char *filename)
{
	if (filename && filename[0])
	{
		ATOM_AutoFile f(filename, ATOM_VFS::write|ATOM_VFS::text);
		if (f)
		{
			f->printf ("-- Dump engine objects --\n");
			f->printf ("Scene: %u\n", sceneCount);
			f->printf ("GuiRenderer: %u\n", GUIrendererCount);
			f->printf ("Widget: %u\n", widgetCount);
			f->printf ("Mesh: %u\n", meshCount);
			f->printf ("Model: %u\n", modelCount);
			f->printf ("Terrain: %u\n", terrainCount);
			f->printf ("ParticleSystem: %u\n", particlesysCount);
			f->printf ("AnimationTrack: %u\n", trackCount);
			f->printf ("Hud: %u\n", hudCount);
		}
	}
	else
	{
		ATOM_LOGGER::log ("-- Dump engine objects --\n");
		ATOM_LOGGER::log ("Scene: %u\n", sceneCount);
		ATOM_LOGGER::log ("GuiRenderer: %u\n", GUIrendererCount);
		ATOM_LOGGER::log ("Widget: %u\n", widgetCount);
		ATOM_LOGGER::log ("Mesh: %u\n", meshCount);
		ATOM_LOGGER::log ("Model: %u\n", modelCount);
		ATOM_LOGGER::log ("Terrain: %u\n", terrainCount);
		ATOM_LOGGER::log ("ParticleSystem: %u\n", particlesysCount);
		ATOM_LOGGER::log ("AnimationTrack: %u\n", trackCount);
		ATOM_LOGGER::log ("Hud: %u\n", hudCount);
	}
}

static bool GetDeviceId (void)
{
	DISPLAY_DEVICE dd;
	dd.cb = sizeof(DISPLAY_DEVICE);

	int i = 0;
	ATOM_STRING id;

	// locate primary display device
	while (::EnumDisplayDevices(NULL, i, &dd, 0))
	{
		if (dd.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)
		{
			id = dd.DeviceID;
			break;
		}

		i++;
	}

	if (id == "") return false;

	// get vendor ID
	_S_vendorId = id.substr(8, 4);

	// get device ID
	_S_deviceId = id.substr(17, 4);

	return true;
}

ATOM_ENGINE_API const char *ATOM_GetPrimaryDeviceId (void)
{
	if (!_S_deviceIdGot)
	{
		GetDeviceId ();
		_S_deviceIdGot = true;
	}

	return _S_deviceId.c_str();
}

ATOM_ENGINE_API const char *ATOM_GetPrimaryVendorId (void)
{
	if (!_S_deviceIdGot)
	{
		GetDeviceId ();
		_S_deviceIdGot = true;
	}

	return _S_vendorId.c_str();
}

static ATOM_VECTOR<ATOM_DisplayDeviceInfo> _deviceInfos;

static void GetDeviceInfos(void)
{
	ATOM_DisplayInfo dispInfo;

	ATOM_STRING s1;
	ATOM_STRING s2 = s1;
	ATOM_DisplayDeviceInfo devInfo;
	ATOM_DisplayDeviceInfo devInfo2 = devInfo;

	_deviceInfos.resize (dispInfo.getNumDevices ());

	for (unsigned i = 0; i < _deviceInfos.size(); ++i)
	{
		_deviceInfos[i].desc = dispInfo.getDeviceDescription (i);
		_deviceInfos[i].manufacturer = dispInfo.getDeviceManufacturer (i);
		_deviceInfos[i].chipType = dispInfo.getDeviceChipType (i);
		_deviceInfos[i].memorySize = dispInfo.getDeviceMemory (i);
		_deviceInfos[i].displayMode = dispInfo.getDeviceDisplayMode (i);
		_deviceInfos[i].driverName = dispInfo.getDeviceDriverName (i);
		_deviceInfos[i].driverVersion = dispInfo.getDeviceDriverVersion (i);
		_deviceInfos[i].driverDate = dispInfo.getDeviceDriverDate (i);
		_deviceInfos[i].directXVersion = dispInfo.getDXVersionString ();
		_deviceInfos[i].vendorId = dispInfo.getDeviceVendorId (i);
		_deviceInfos[i].deviceId = dispInfo.getDeviceId (i);
		_deviceInfos[i].subsysId = dispInfo.getDeviceSubSystemId (i);
		_deviceInfos[i].revisionId = dispInfo.getDeviceRevisionId (i);
		_deviceInfos[i].ddrawAccelEnabled = dispInfo.isDDrawAccelerationEnabled (i);
		_deviceInfos[i].d3dAccelEnabled = dispInfo.is3DAccelerationEnabled (i);
		_deviceInfos[i].AGPEnabled = dispInfo.isAGPEnabled (i);
		_deviceInfos[i].d3dAccelExists = dispInfo.is3DAccelerationExists (i);
		_deviceInfos[i].AGPExists = dispInfo.isAGPExists (i);
	}
}

ATOM_ENGINE_API unsigned ATOM_GetNumDisplayDevices (void)
{
	ATOM_STACK_TRACE(ATOM_GetNumDisplayDevices);

	if (_deviceInfos.empty ())
	{
		GetDeviceInfos ();
	}

	return _deviceInfos.size ();
}

ATOM_ENGINE_API const ATOM_DisplayDeviceInfo &ATOM_GetDisplayDeviceInfo (unsigned device)
{
	ATOM_STACK_TRACE(ATOM_GetDisplayDeviceInfo);

	if (_deviceInfos.empty ())
	{
		GetDeviceInfos ();
	}

	if (_deviceInfos.empty ())
	{
		throw "No display device detected";
	}

	return _deviceInfos[device];
}

