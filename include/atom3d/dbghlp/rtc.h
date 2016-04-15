#ifndef __DEBUGHELP_RTC_H__
#define __DEBUGHELP_RTC_H__

#include "basedefs.h"

#if ATOM3D_COMPILER_MSVC

#	include <crtdbg.h>
#	include <errno.h>

#if _MSC_VER > 1200
#	include <rtcapi.h>
#endif

#endif

class ATOM_RTCErrorHandler
{
public:
	virtual ~ATOM_RTCErrorHandler (void) {}
	virtual int report (int err, const char *filename, int line, const char *modulename, const char *desc);
};

extern"C" ATOM_DBGHLP_API int ATOM_MyRTCErrorFunc (int err, const char *filename, int lineno, const char *modulename, const char *format, ...);
extern"C" ATOM_DBGHLP_API ATOM_RTCErrorHandler *ATOM_SetRTCErrorHandler (ATOM_RTCErrorHandler *handler);

#if ATOM3D_COMPILER_MSVC && (_MSC_VER > 1200) && !defined(NDEBUG)
# define HOOK_RTC_ERROR_CHECK(enable) \
  if (enable) { \
    _RTC_SetErrorFunc (&ATOM_MyRTCErrorFunc); \
  } else { \
    _RTC_SetErrorFunc (&_CrtDbgReport); \
  }
#else
# define HOOK_RTC_ERROR_CHECK(enable)
#endif

class ATOM_DBGHLP_API ATOM_CRTErrorHandler
{
public:
	virtual ~ATOM_CRTErrorHandler (void) {}
	virtual int report (int err, const char *msg);
};

extern"C" ATOM_DBGHLP_API int __cdecl ATOM_MyCRTErrorFunc (int err, char *msg, int *ret);
extern"C" ATOM_DBGHLP_API ATOM_CRTErrorHandler *ATOM_SetCRTErrorHandler (ATOM_CRTErrorHandler *handler);

#if ATOM3D_COMPILER_MSVC
# define HOOK_CRT_ERROR_CHECK(enable) \
  if (enable) { \
    _CrtSetReportHook2(_CRT_RPTHOOK_INSTALL, &ATOM_MyCRTErrorFunc); \
  } else { \
    _CrtSetReportHook2(_CRT_RPTHOOK_REMOVE, &ATOM_MyCRTErrorFunc); \
  }
#else
#	define HOOK_CRT_ERROR_CHECK(enable)
#endif

#endif // __DEBUGHELP_RTC_H__

