INCLUDE_DIRECTORIES(
	"${CMAKE_CURRENT_BINARY_DIR}"
	"${CMAKE_CURRENT_SOURCE_DIR}"
	"${ATOM3D_INCLUDE_DIR}/dbghlp"
)

AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR} DbgHlpSrc)
AUX_SOURCE_DIRECTORY(nedmalloc NedMallocSrc)
FILE(GLOB DbgHlpPrivateHeaders *.h)
FILE(GLOB DbgHlpPublicHeaders ${ATOM3D_INCLUDE_DIR}/dbghlp/*.h)
SET(DbgHlpSrcHeaders ${DbgHlpSrc} ${DbgHlpPrivateHeaders} ${DbgHlpResource})
SOURCE_GROUP(headers FILES ${DbgHlpPublicHeaders})
SOURCE_GROUP(source FILES ${DbgHlpSrcHeaders})

ADD_DEFINITIONS(-DBUILD_ATOM_DEBUGHELP)
FILE(GLOB PCH_Header stdafx.h)
FILE(GLOB PCH_Source stdafx.cpp)

ADD_LIBRARY(ATOM_dbghlp ${ATOM3D_SHARED} ${DbgHlpSrcHeaders} ${DbgHlpPublicHeaders})
ADD_PRECOMPILED_HEADER(ATOM_dbghlp ${PCH_Header} ${PCH_Source} DbgHlpSrcHeaders)
ADD_DEPENDENCIES(ATOM_dbghlp ATOM_thread)
TARGET_LINK_LIBRARIES(ATOM_dbghlp ATOM_thread version comctl32 wininet comdlg32 gdi32)

INSTALL_TARGET(ATOM_dbghlp)
