INCLUDE_DIRECTORIES(
	"${CMAKE_CURRENT_BINARY_DIR}"
	"${CMAKE_CURRENT_SOURCE_DIR}"
)
INCLUDE_DIRECTORIES("${TOPLEVEL_INCLUDE_DIR}/atom3dx")
INCLUDE_DIRECTORIES("${THIRDPART_DIR}/src/anttweakbar/include")
INCLUDE_DIRECTORIES("${THIRDPART_DIR}/include/dxsdk")
LINK_DIRECTORIES("${THIRDPART_DIR}/lib")

AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR} Atom3DXSrc)
FILE(GLOB Atom3DXPrivateHeaders *.h)
FILE(GLOB Atom3DXPublicHeaders ${TOPLEVEL_INCLUDE_DIR}/atom3dx/*.h)
SET(Atom3DXSrcHeaders ${Atom3DXSrc} ${Atom3DXPrivateHeaders})
SOURCE_GROUP(headers FILES ${Atom3DXPublicHeaders})
SOURCE_GROUP(source FILES ${Atom3DXSrcHeaders})

ADD_DEFINITIONS(-DBUILD_ATOMX)
FILE(GLOB PCH_Header stdafx.h)
FILE(GLOB PCH_Source stdafx.cpp)

ADD_LIBRARY(ATOMX ${ATOM3D_SHARED} ${Atom3DXSrcHeaders} ${Atom3DXPublicHeaders})
ADD_PRECOMPILED_HEADER(ATOMX ${PCH_Header} ${PCH_Source} Atom3DXSrc)
ADD_DEPENDENCIES(ATOMX ATOM_vfs ATOM_kernel ATOM_image ATOM_dbghlp ATOM_engine ATOM_audio ATOM_script ATOM_ttfont ATOM_commondlg AntTweakBar)
TARGET_LINK_LIBRARIES(ATOMX ATOM_vfs ATOM_kernel ATOM_image ATOM_dbghlp ATOM_engine ATOM_audio ATOM_script ATOM_ttfont ATOM_commondlg AntTweakBar)

INSTALL_TARGET(ATOMX)
