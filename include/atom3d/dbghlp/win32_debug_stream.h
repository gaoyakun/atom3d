#ifndef __ATOM3D_DBGHLP_WIN32_DEBUG_STREAM_H
#define __ATOM3D_DBGHLP_WIN32_DEBUG_STREAM_H

#include "basedefs.h"

class ATOM_DBGHLP_API ATOM_Win32DebugStream
{
public:
	static bool isMonitoring (void);
	static void startMonitering (void);
	static void stopMonitering (void);
};

#endif // __ATOM3D_DBGHLP_WIN32_DEBUG_STREAM_H
