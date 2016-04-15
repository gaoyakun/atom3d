INCLUDE_DIRECTORIES("${ATOM3D_INCLUDE_DIR}")
INCLUDE_DIRECTORIES("${ATOM3DX_INCLUDE_DIR}")
INCLUDE_DIRECTORIES("${THIRDPART_DIR}/include")

IF (BUILD_WIN64)
	LINK_DIRECTORIES("${THIRDPART_DIR}/lib/dxsdk/x64")
ELSE (BUILD_WIN64)
	LINK_DIRECTORIES("${THIRDPART_DIR}/lib/dxsdk/x86")
ENDIF (BUILD_WIN64)

AUX_SOURCE_DIRECTORY(. ChgSkeletonSrc)
FILE(GLOB ChgSkeletonPrivateHeaders *.h)
SOURCE_GROUP(headers FILES ${ChgSkeletonPrivateHeaders})
SOURCE_GROUP(source FILES ${ChgSkeletonSrc})

ADD_EXECUTABLE(dumpskeleton ${ChgSkeletonSrc} ${ChgSkeletonPrivateHeaders})
ADD_DEPENDENCIES(dumpskeleton ATOM_dbghlp ATOM_kernel ATOM_geometry ATOM_engine ATOM_image)
TARGET_LINK_LIBRARIES(dumpskeleton ATOM_dbghlp ATOM_kernel ATOM_geometry ATOM_engine ATOM_image)

INSTALL_TARGET(dumpskeleton)
