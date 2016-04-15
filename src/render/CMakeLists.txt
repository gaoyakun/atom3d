INCLUDE_DIRECTORIES(
	"${CMAKE_CURRENT_BINARY_DIR}"
	"${CMAKE_CURRENT_SOURCE_DIR}"
)
INCLUDE_DIRECTORIES("${ATOM3D_INCLUDE_DIR}/render")
INCLUDE_DIRECTORIES(".")
INCLUDE_DIRECTORIES("d3d")
INCLUDE_DIRECTORIES("${THIRDPART_DIR}/include/dxsdk")
LINK_DIRECTORIES("${THIRDPART_DIR}/lib/dxsdk/x86")

AUX_SOURCE_DIRECTORY(. RenderSrc)
AUX_SOURCE_DIRECTORY(d3d D3DSrc)
FILE(GLOB RenderPrivateHeaders *.h)
FILE(GLOB D3DPrivateHeaders d3d/*.h)
FILE(GLOB RenderPublicHeaders ${ATOM3D_INCLUDE_DIR}/render/*.h)
SET(RenderSrcHeaders ${RenderSrc} ${RenderPrivateHeaders})
SET(D3DSrcHeaders ${D3DSrc} ${D3DPrivateHeaders}) 
SOURCE_GROUP(headers FILES ${RenderPublicHeaders})
SOURCE_GROUP(source FILES ${RenderSrcHeaders})
SOURCE_GROUP(d3d FILES ${D3DSrcHeaders})

ADD_DEFINITIONS(-DBUILD_ATOM_RENDER -DN3RENDER_D3D)
FILE(GLOB PCH_Header stdafx.h)
FILE(GLOB PCH_Source stdafx.cpp)

SET(RenderSource ${RenderSrcHeaders} ${D3DSrcHeaders})
ADD_LIBRARY(ATOM_render ${ATOM3D_SHARED} ${RenderSource} ${RenderPublicHeaders})
ADD_PRECOMPILED_HEADER(ATOM_render ${PCH_Header} ${PCH_Source} RenderSource)
ADD_DEPENDENCIES(ATOM_render ATOM_kernel ATOM_image ATOM_ttfont ATOM_script)

if (MSVC)
	TARGET_LINK_LIBRARIES(ATOM_render ATOM_kernel ATOM_image ATOM_ttfont ATOM_script ATOM_net imm32 d3dx9 dxerr)
else (MSVC)
	TARGET_LINK_LIBRARIES(ATOM_render ATOM_kernel ATOM_image ATOM_ttfont ATOM_script ATOM_net imm32 d3dx9)
endif (MSVC)

INSTALL_TARGET(ATOM_render)
