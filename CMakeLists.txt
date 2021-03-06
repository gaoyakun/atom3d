CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
SET(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
	cmake_policy(SET CMP0005 NEW)
	cmake_policy(SET CMP0008 OLD)
endif (COMMAND cmake_policy)

PROJECT(atom3d)

set(ATOM3D_BUILD_PLATFORM_WIN32 0)
set(ATOM3D_BUILD_PLATFORM_APPLE 0)
set(ATOM3D_BUILD_PLATFORM_APPLE_IPHONE 0)
set(ATOM3D_BUILD_PLATFORM_APPLE_IPHONE_SIMULATOR 0)
set(ATOM3D_BUILD_PLATFORM_CYGWIN 0)
set(ATOM3D_BUILD_PLATFORM_MINGW 0)
set(ATOM3D_BUILD_PLATFORM_LINUX 0)
set(ATOM3D_SHARED_LIBS 0)

if (MSVC)
	set(ATOM3D_TEST_BIG_ENDIAN FALSE)
else (MSVC)
	include(TestBigEndian)
	TEST_BIG_ENDIAN(ATOM3D_TEST_BIG_ENDIAN)
endif (MSVC)

if (ATOM3D_TEST_BIG_ENDIAN)
	set(ATOM3D_BIG_ENDIAN 1)
	set(ATOM3D_LITTLE_ENDIAN 0)
else (ATOM3D_TEST_BIG_ENDIAN)
	set(ATOM3D_BIG_ENDIAN 0)
	set(ATOM3D_LITTLE_ENDIAN 1)
endif (ATOM3D_TEST_BIG_ENDIAN)

# detecting platform
set(ATOM3D_BUILD_PLATFORM_WIN32 0)
set(ATOM3D_BUILD_PLATFORM_APPLE 0)
set(ATOM3D_BUILD_PLATFORM_APPLE_IPHONE 0)
set(ATOM3D_BUILD_PLATFORM_APPLE_IPHONE_SIMULATOR 0)
set(ATOM3D_BUILD_PLATFORM_CYGWIN 0)
set(ATOM3D_BUILD_PLATFORM_MINGW 0)
set(ATOM3D_BUILD_PLATFORM_LINUX 0)

if (WIN32)
	set(ATOM3D_BUILD_PLATFORM_WIN32 1)
endif (WIN32)

if (APPLE)
	set(ATOM3D_BUILD_PLATFORM_APPLE 1)
	option (ATOM3D_BUILD_PLATFORM_IPHONE "Build for iphone device support" OFF)
	option (ATOM3D_BUILD_PLATFORM_IPHONE_SIMULATOR "Build for iphone simulator support" OFF)
	if (ATOM3D_BUILD_PLATFORM_IPHONE)
		set(ATOM3D_BUILD_PLATFORM_APPLE_IPHONE 1)
	elseif (ATOM3D_BUILD_PLATFORM_IPHONE_SIMULATOR)
		set(ATOM3D_BUILD_PLATFORM_APPLE_IPHONE_SIMULATOR 1)
	endif ()
endif (APPLE)

if (CYGWIN)
	set(ATOM3D_BUILD_PLATFORM_CYGWIN 1)
endif (CYGWIN)

if (MINGW)
	set(ATOM3D_BUILD_PLATFORM_MINGW 1)
endif (MINGW)

if (LINUX)
	set(ATOM3D_BUILD_PLATFORM_LINUX 1)
endif (LINUX)

# detecting compiler
set(ATOM3D_COMPILER_MSVC 0)
set(ATOM3D_COMPILER_GCC	0)

if (MSVC)
	set(ATOM3D_COMPILER_MSVC 1)
endif (MSVC)

if (CMAKE_COMPILER_IS_GNUCC)
	set(ATOM3D_COMPILER_GCC 1)
endif (CMAKE_COMPILER_IS_GNUCC)

if (CMAKE_COMPILER_IS_GNUCXX)
	set(ATOM3D_COMPILER_GCC 1)
endif (CMAKE_COMPILER_IS_GNUCXX)

# atom3d version definition
SET(ATOM3D_MAJOR_VERSION 0)
SET(ATOM3D_MINOR_VERSION 7)
SET(ATOM3D_PATCH_VERSION 0)
SET(ATOM3D_VERSION ${ATOM3D_MAJOR_VERSION}.${ATOM3D_MINOR_VERSION}.${ATOM3D_PATCH_VERSION})

option(ATOM3D_BUILD_SHARED_LIBS "Enable build shared library if not iOS" OFF)
if (ATOM3D_BUILD_SHARED_LIBS)
	set(ATOM3D_SHARED "SHARED")
	set(ATOM3D_SHARED_LIBS 1)
else (ATOM3D_BUILD_SHARED_LIBS)
	set(ATOM3D_SHARED "STATIC")
	set(ATOM3D_SHARED_LIBS 0)
endif (ATOM3D_BUILD_SHARED_LIBS)

option(ATOM3D_MINIMAL_BUILD "Minimal build for smallest size" OFF)
if (ATOM3D_MINIMAL_BUILD)
	set(ATOM3D_MINIMAL 1)
else (ATOM3D_MINIMAL_BUILD)
	set(ATOM3D_MINIMAL 0)
endif(ATOM3D_MINIMAL_BUILD)

SET(TOPLEVEL_DIR ${CMAKE_SOURCE_DIR})
SET(TOPLEVEL_INCLUDE_DIR ${TOPLEVEL_DIR}/include)
SET(ATOM3D_INCLUDE_DIR ${TOPLEVEL_INCLUDE_DIR}/atom3d)
SET(ATOM3DX_INCLUDE_DIR ${TOPLEVEL_INCLUDE_DIR}/atom3dx)
IF (ATOM3D_BUILD_PLATFORM_IPHONE)
	SET(OUT_DIR ${TOPLEVEL_DIR}/bin/ios)
	SET(LIB_DIR ${TOPLEVEL_DIR}/lib/ios)
ELSEIF (ATOM3D_BUILD_PLATFORM_IPHONE_SIMULATOR)
	SET(OUT_DIR ${TOPLEVEL_DIR}/bin/ios_sim)
	SET(LIB_DIR ${TOPLEVEL_DIR}/lib/ios_sim)
ELSEIF (ATOM3D_BUILD_PLATFORM_APPLE)
	SET(OUT_DIR ${TOPLEVEL_DIR}/bin/macosx)
	SET(LIB_DIR ${TOPLEVEL_DIR}/lib/macosx)
ELSEIF(ATOM3D_BUILD_PLATFORM_MINGW)
	SET(OUT_DIR ${TOPLEVEL_DIR}/bin/mingw32)
	SET(LIB_DIR ${TOPLEVEL_DIR}/lib/mingw32)
ELSEIF (ATOM3D_BUILD_PLATFORM_WIN32)
	IF (BUILD_WIN64)
		SET(OUT_DIR ${TOPLEVEL_DIR}/bin/x64)
		SET(LIB_DIR ${TOPLEVEL_DIR}/lib/x64)
	ELSE (BUILD_WIN64)
		SET(OUT_DIR ${TOPLEVEL_DIR}/bin/x86)
		SET(LIB_DIR ${TOPLEVEL_DIR}/lib/x86)
	ENDIF (BUILD_WIN64)
ENDIF (ATOM3D_BUILD_PLATFORM_IPHONE)
SET(DEPLOY_DIR ${OUT_DIR})
SET(LIBRARY_DIR ${LIB_DIR})

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE "RelWithDebInfo")
ENDIF(NOT CMAKE_BUILD_TYPE)

MACRO(INSTALL_TARGET)
	INSTALL(TARGETS ${ARGV} 
		RUNTIME DESTINATION ${DEPLOY_DIR}/Debug CONFIGURATIONS Debug 
		ARCHIVE DESTINATION ${LIBRARY_DIR}/Debug CONFIGURATIONS Debug)
	INSTALL(TARGETS ${ARGV} 
		RUNTIME DESTINATION ${DEPLOY_DIR}/Release CONFIGURATIONS Release 
		ARCHIVE DESTINATION ${LIBRARY_DIR}/Release CONFIGURATIONS Release)
	INSTALL(TARGETS ${ARGV} 
		RUNTIME DESTINATION ${DEPLOY_DIR}/RelWithDebInfo CONFIGURATIONS RelWithDebInfo
		ARCHIVE DESTINATION ${LIBRARY_DIR}/RelWithDebInfo CONFIGURATIONS RelWithDebInfo)
	INSTALL(TARGETS ${ARGV} 
		RUNTIME DESTINATION ${DEPLOY_DIR}/MinSizeRel CONFIGURATIONS MinSizeRel
		ARCHIVE DESTINATION ${LIBRARY_DIR}/MinSizeRel CONFIGURATIONS MinSizeRel)
	INSTALL(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Debug DESTINATION ${LIBRARY_DIR} FILES_MATCHING PATTERN *.pdb)
	INSTALL(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Release DESTINATION ${LIBRARY_DIR} FILES_MATCHING PATTERN *.pdb)
	INSTALL(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/RelWithDebInfo DESTINATION ${LIBRARY_DIR} FILES_MATCHING PATTERN *.pdb)
	INSTALL(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/MinSizeRel DESTINATION ${LIBRARY_DIR} FILES_MATCHING PATTERN *.pdb)
ENDMACRO(INSTALL_TARGET)

SET(THIRDPART_DIR "${TOPLEVEL_DIR}/3rd")

IF (BUILD_WIN64)
	LINK_DIRECTORIES("${THIRDPART_DIR}/lib64")
ELSE (BUILD_WIN64)
	LINK_DIRECTORIES("${THIRDPART_DIR}/lib")
ENDIF (BUILD_WIN64)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${TOPLEVEL_DIR}/CMake)

# build options
if (APPLE)
	if (ATOM3D_BUILD_PLATFORM_IPHONE OR ATOM3D_BUILD_PLATFORM_IPHONE_SIMULATOR)
		set (ATOM3D_SHARED "STATIC")
		set (ATOM3D_SHARED_LIBS 0)
		set (IPHONE_SDKVER "4.2")
		include (CMakeForceCompiler)
		CMAKE_FORCE_C_COMPILER(gcc-4.2 GNU)
		CMAKE_FORCE_CXX_COMPILER(gcc-4.2 GNU)
		set (GCC_THUMB_SUPPORT NO)
		
		if (ATOM3D_BUILD_PLATFORM_IPHONE)
			set (IPHONE_DEVROOT "/Developer/Platforms/iPhoneOS.platform/Developer")
			set (IPHONE_SDKROOT "${IPHONE_DEVROOT}/SDKs/iPhoneOS${IPHONE_SDKVER}.sdk")
		else (ATOM3D_BUILD_PLATFORM_IPHONE)
			set (IPHONE_DEVROOT "/Developer/Platforms/iPhoneSimulator.platform/Developer")
			set (IPHONE_SDKROOT "${IPHONE_DEVROOT}/SDKs/iPhoneSimulator${IPHONE_SDKVER}.sdk")
		endif (ATOM3D_BUILD_PLATFORM_IPHONE)
		
		set (OPENGLES_LIBRARY "${IPHONE_SDKROOT}/System/Library/Frameworks/OpenGLES.framework")
		MESSAGE(STATUS "${OPENGLES_LIBRARY}")
		set (OPENAL_LIBRARY "${IPHONE_SDKROOT}/System/Library/Frameworks/OpenAL.framework")
		MESSAGE(STATUS "${OPENAL_LIBRARY}")
		find_library(COCOA_LIBRARY Cocoa)
		
		set (CMAKE_OSX_DEPLOYMENT_TARGET "")
		set (CMAKE_EXE_LINKER_FLAGS "-framework Foundation -framework CoreGraphics -framework QuartzCore -framework UIKit -framework OpenAL -framework AudioToolBox -framework AVFoundation")
		set (MACOSX_BUNDLE_GUI_IDENTIFIER "com.atom3d.\${PRODUCT_NAME:rfc1034identifier}")
		set (CMAKE_OSX_ARCHITECTURES $(ARCHS_STANDARD_32_BIT))
		remove_definitions (-msse)
	else (ATOM3D_BUILD_PLATFORM_IPHONE OR ATOM3D_BUILD_PLATFORM_IPHONE_SIMULATOR)
		find_library(COCOA_LIBRARY Cocoa)
		find_package(OpenGL REQUIRED)
		MESSAGE(STATUS "OpenGL include path found: ${OpenGL_INCLUDE_PATH}")
		MESSAGE(STATUS "OpenGL library found: ${OpenGL_LIBRARIES}")
		find_package(OpenAL REQUIRED)
		MESSAGE(STATUS "OpenAL include path found: ${OpenAL_INCLUDE_PATH}")
		MESSAGE(STATUS "OpenAL library found: ${OPENAL_LIBRARIES}")
		find_library(AGL_LIBRARY AGL)
		set (CMAKE_EXE_LINKER_FLAGS "-framework CoreAudio -framework OpenAL -framework AudioToolBox")
	endif (ATOM3D_BUILD_PLATFORM_IPHONE OR ATOM3D_BUILD_PLATFORM_IPHONE_SIMULATOR)
else (APPLE)
	find_package(OpenGL)
endif (APPLE)

configure_file("${TOPLEVEL_DIR}/CMake/templates/ATOM_config.h.in" "${TOPLEVEL_INCLUDE_DIR}/atom3d/ATOM_config.h")

IF(MSVC)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4996 /wd4786 /wd4819 /wd4251 /Zc:wchar_t /Oi /fp:fast /arch:SSE2 /Zm200")
ENDIF(MSVC)

IF (BUILD_WIN64)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
ENDIF (BUILD_WIN64)

INCLUDE_DIRECTORIES("${ATOM3D_INCLUDE_DIR}")

include(PrecompiledHeader)

option (ATOM3D_BUILD_ANTTWEAKBAR_STATIC "Build AntTweakBar as static library" OFF)

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(utils)
ADD_SUBDIRECTORY(3rd)

if (APPLE)
	if (ATOM3D_BUILD_PLATFORM_IPHONE OR ATOM3D_BUILD_PLATFORM_IPHONE_SIMULATOR)
		if (ATOM3D_BUILD_PLATFORM_IPHONE)
			MESSAGE(STATUS "Platform iphone")
			SET(CMAKE_OSX_ARCHITECTURES "armv6;armv7" CACHE STRING "Build architectures for iOS" FORCE)
			SET(CMAKE_CXX_FLAGS "-miphoneos-version-min=3.1 -mno-thumb -arch armv6 -pipe -no-cpp-precomp" CACHE STRING "Flags used by the compiler during all build types." FORCE)
		else (ATOM3D_BUILD_PLATFORM_IPHONE)
			MESSAGE(STATUS "Platform iphone simulator")
			SET(CMAKE_OSX_ARCHITECTURES "i386" CACHE STRING "Build architectures for iOS simulator" FORCE)
			SET(CMAKE_CXX_FLAGS "-mno-thumb -arch i386  -pipe -no-cpp-precomp" CACHE STRING "Flags used by the compiler during all build types." FORCE)
		endif (ATOM3D_BUILD_PLATFORM_IPHONE)
		set (CMAKE_OSX_SYSROOT "${IPHONE_SDKROOT}" CACHE STRING "System root for iOS" FORCE)
		set (CMAKE_EXE_LINKER_FLAGS "-framework Foundation -framework OpenGLES")
	else (ATOM3D_BUILD_PLATFORM_IPHONE OR ATOM3D_BUILD_PLATFORM_IPHONE_SIMULATOR)
		if (EXISTS /Developer/SDKs/MacOSX10.5.sdk)
			set (CMAKE_OSX_ARCHITECTURES "ppc;i386" CACHE STRING "Build architectures for OSX" FORCE)
			set (CMAKE_CXX_FLAGS "-ftree-vectorize -fvisibility-inlines-hidden" CACHE STRING "Flags used by the compiler during all build types." FORCE)
		elseif (EXISTS /Developer/SDKs/MacOSX10.4u.sdk)
			set (CMAKE_OSX_ARCHITECTURES "ppc;i386" CACHE STRING "Build architectures for OSX" FORCE)
			set (CMAKE_CXX_FLAGS "-ftree-vectorize -fvisibility-inlines-hidden" CACHE STRING "Flags used by the compiler during all build types." FORCE)
		endif (EXISTS /Developer/SDKs/MacOSX10.5.sdk)
	endif (ATOM3D_BUILD_PLATFORM_IPHONE OR ATOM3D_BUILD_PLATFORM_IPHONE_SIMULATOR)
endif (APPLE)
