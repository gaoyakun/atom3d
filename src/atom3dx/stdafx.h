#ifndef __ATOM3DX_STDAFX_H
#define __ATOM3DX_STDAFX_H

#ifdef WIN32

#define WIN32_LEAN_AND_MEAN	
// 如果您必须使用下列所指定的平台之前的平台，则修改下面的定义。
// 有关不同平台的相应值的最新信息，请参考 MSDN。
#ifndef WINVER				// 允许使用 Windows 95 和 Windows NT 4 或更高版本的特定功能。
#define WINVER 0x0400		//为 Windows98 和 Windows 2000 及更新版本改变为适当的值。
#endif

#ifndef _WIN32_WINNT		// 允许使用 Windows NT 4 或更高版本的特定功能。
#define _WIN32_WINNT 0x0501		//为 Windows98 和 Windows 2000 及更新版本改变为适当的值。
#endif						

#ifndef _WIN32_WINDOWS		// 允许使用 Windows 98 或更高版本的特定功能。
#define _WIN32_WINDOWS 0x0410 //为 Windows Me 及更新版本改变为适当的值。
#endif

#ifndef _WIN32_IE			// 允许使用 IE 4.0 或更高版本的特定功能。
#define _WIN32_IE 0x0501	//为 IE 5.0 及更新版本改变为适当的值。
#endif

#include <windows.h>
#include <windowsx.h>
#include <commdlg.h>
#include <shlobj.h>
#include <tchar.h>
#include <initguid.h>


#endif



#include <string>
#include <cstdlib>
#include <stack>
#include <ctime>
#include <time.h>
#include <stdlib.h>
#include <cstdio>
#include <ctype.h>
#include <process.h>
#include <vector>
#include <map>
#include <stdio.h>
#include <malloc.h>
#include <algorithm>
#include <sstream>
#include <cmath>
#include <stdexcept>
#include <signal.h>

#include <ATOM_math.h>
#include <ATOM_utils.h>
#include <ATOM_dbghlp.h>
#include <ATOM_vfs.h>
#include <ATOM_kernel.h>
#include <ATOM_image.h>
#include <ATOM_render.h>
#include <ATOM_engine.h>
#include <ATOM_audio.h>
#include <ATOM_script.h>
#include <ATOM_ttfont.h>
#include <ATOM_commondlg.h>

#endif // __ATOM3DX_STDAFX_H
