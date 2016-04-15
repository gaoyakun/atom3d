#ifndef __ATOM3D_PLATFORM_H
#define __ATOM3D_PLATFORM_H

#if defined(__linux__)||defined(__linux)||defined(linux)
#	undef ATOM3D_PLATFORM_LINUX
#	define ATOM3D_PLATFORM_LINUX 1
#endif

#if defined(__APPLE__)
#	include "AvailabilityMacros.h"
#	include "TargetConditionals.h"
#	if TARGET_OS_IPHONE
#		undef ATOM3D_PLATFORM_IPHONE
#		define ATOM3D_PLATFORM_IPHONE 1
#	elif TARGET_IPHONE_SIMULATOR
#		undef ATOM3D_PLATFORM_IPHONE_SIMULATOR
#		define ATOM3D_PLATFORM_IPHONE_SIMULATOR 1
#	endif
#	undef ATOM3D_PLATFORM_APPLE
#	define ATOM3D_PLATFORM_APPLE 1
#endif

#if defined(WIN32)||defined(_WIN32)
#	undef ATOM3D_PLATFORM_WIN32
#	define ATOM3D_PLATFORM_WIN32 1
#endif

#if defined(__MINGW32__)||defined(__MINGW__)
#	undef ATOM3D_PLATFORM_MINGW
#	define ATOM3D_PLATFORM_MINGW 1
#endif

#if defined(_MSC_VER)
#	undef ATOM3D_COMPILER_MSVC
#	define ATOM3D_COMPILER_MSVC 1
#endif

#if defined(__GNUC__)||defined(__GNUG__)
#	undef ATOM3D_COMPILER_GCC
#	define ATOM3D_COMPILER_GCC 1
#endif

#endif // __ATOM3D_PLATFORM_H
