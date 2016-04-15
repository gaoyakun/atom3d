INCLUDE_DIRECTORIES(
	"${CMAKE_CURRENT_BINARY_DIR}"
	"${CMAKE_CURRENT_SOURCE_DIR}"
)
INCLUDE_DIRECTORIES("${ATOM3D_INCLUDE_DIR}/kernel")
INCLUDE_DIRECTORIES("${THIRDPART_DIR}/include/dxsdk")

AUX_SOURCE_DIRECTORY(. KernelSrc)
FILE(GLOB KernelPrivateHeaders *.h)
FILE(GLOB KernelPublicHeaders ${ATOM3D_INCLUDE_DIR}/kernel/*.h)
SET(KernelSrcHeaders ${KernelSrc} ${KernelPrivateHeaders})
SOURCE_GROUP(headers FILES ${KernelPublicHeaders})
SOURCE_GROUP(source FILES ${KernelSrcHeaders})

ADD_DEFINITIONS(-DBUILD_ATOM_KERNEL)
FILE(GLOB PCH_Header stdafx.h)
FILE(GLOB PCH_Source stdafx.cpp)

ADD_LIBRARY(ATOM_kernel ${ATOM3D_SHARED} ${KernelSrcHeaders} ${KernelPublicHeaders})
ADD_DEPENDENCIES(ATOM_kernel ATOM_vfs)
ADD_PRECOMPILED_HEADER(ATOM_kernel ${PCH_Header} ${PCH_Source} KernelSrcHeaders)

if (WIN32)
TARGET_LINK_LIBRARIES(ATOM_kernel ATOM_vfs dxguid psapi kernel32 ole32 oleaut32)
else (WIN32)
TARGET_LINK_LIBRARIES(ATOM_kernel ATOM_vfs)
endif (WIN32)

INSTALL_TARGET(ATOM_kernel)
