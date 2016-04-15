INCLUDE_DIRECTORIES(
	"${CMAKE_CURRENT_BINARY_DIR}"
	"${CMAKE_CURRENT_SOURCE_DIR}"
)
INCLUDE_DIRECTORIES("${ATOM3D_INCLUDE_DIR}/gfxdriver")
INCLUDE_DIRECTORIES(".")
INCLUDE_DIRECTORIES("${THIRDPART_DIR}/include/dxsdk")

AUX_SOURCE_DIRECTORY(. GfxDriverD3D9Src)
AUX_SOURCE_DIRECTORY(../win32 Win32CommonSrc)
AUX_SOURCE_DIRECTORY(.. GfxDriverCommonSrc)
FILE(GLOB GfxDriverD3D9PrivateHeaders *.h)
FILE(GLOB GfxDriverWin32CommonHeaders ../win32/*.h)
FILE(GLOB GfxDriverCommonHeaders ../*.h)
SET(GfxDriverD3D9SrcHeaders ${GfxDriverD3D9Src} ${GfxDriverD3D9PrivateHeaders})
SET(GfxDriverWin32CommonSrcHeaders ${Win32CommonSrc} ${GfxDriverWin32CommonHeaders}) 
SET(GfxDriverCommonSrcHeaders ${GfxDriverCommonSrc} ${GfxDriverCommonHeaders})
SET(GfxDriverSrcHeaders ${GfxDriverD3D9SrcHeaders} ${GfxDriverWin32CommonSrcHeaders} ${GfxDriverCommonSrcHeaders})
SOURCE_GROUP(headers FILES ${GfxDriverPublicHeaders})
SOURCE_GROUP(sources FILES ${GfxDriverSrcHeaders})

ADD_DEFINITIONS(-DBUILD_ATOM_GFXDRIVER -DGFXDRIVER_D3D)

ADD_LIBRARY(ATOM_gfxdriverd3d9 ${ATOM3D_SHARED} ${GfxDriverSrcHeaders} ${GfxDriverPublicHeaders})
ADD_DEPENDENCIES(ATOM_gfxdriverd3d9 ATOM_kernel ATOM_image)

if (MSVC)
	TARGET_LINK_LIBRARIES(ATOM_gfxdriverd3d9 ATOM_kernel ATOM_image imm32 d3dx9 dxerr)
else (MSVC)
	TARGET_LINK_LIBRARIES(ATOM_gfxdriverd3d9 ATOM_kernel ATOM_image imm32 d3dx9)
endif (MSVC)

INSTALL_TARGET(ATOM_gfxdriverd3d9)
