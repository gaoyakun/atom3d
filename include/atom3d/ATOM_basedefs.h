#ifndef __ATOM_BASEDEFS_H
#define __ATOM_BASEDEFS_H

#include "ATOM_platform.h"
#include "ATOM_config.h"

#if ATOM3D_PLATFORM_WIN32
# define ATOM_EXPORT __declspec(dllexport)
# define ATOM_IMPORT __declspec(dllimport)
# define ATOM_CALL   __cdecl
#else
# define ATOM_EXPORT
# define ATOM_IMPORT
# define ATOM_CALL
#endif

#if ATOM3D_COMPILER_MSVC

# pragma warning(disable:4786)
# pragma warning(disable:4251)
# pragma warning(disable:4267)
# pragma warning(disable:4244)
# pragma warning(disable:4018)
# pragma warning(disable:4312)

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

# undef  strdup
# define strdup _strdup

# if (_MSC_VER < 1500)
#	undef  vsnprintf
#	define vsnprintf  _vsnprintf
# endif

# if _MSC_VER <= 1200
  typedef __int64 ATOM_LongLong;
  typedef unsigned __int64 ATOM_ULongLong;
# else
  typedef long long ATOM_LongLong;
  typedef unsigned long long ATOM_ULongLong;
#endif

# define ATOM_ALIGN(alignment) __declspec(align(alignment))

#else // not _MSC_VER

  typedef long long ATOM_LongLong;
  typedef unsigned long long ATOM_ULongLong;

# define ATOM_ALIGN(alignment) __attribute__((aligned(alignment)))

#endif // _MSC_VER

#endif // __ATOM_BASEDEFS_H
