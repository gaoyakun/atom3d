INCLUDE_DIRECTORIES("${ATOM3D_INCLUDE_DIR}")
INCLUDE_DIRECTORIES("${ATOM3DX_INCLUDE_DIR}")

IF (BUILD_WIN64)
	LINK_DIRECTORIES("${THIRDPART_DIR}/lib/dxsdk/x64")
ELSE (BUILD_WIN64)
	LINK_DIRECTORIES("${THIRDPART_DIR}/lib/dxsdk/x86")
ENDIF (BUILD_WIN64)

AUX_SOURCE_DIRECTORY(. ShaderCompilerSrc)
FILE(GLOB ShaderCompilerPrivateHeaders *.h)

ADD_EXECUTABLE(shadercompiler ${ShaderCompilerSrc} ${ShaderCompilerPrivateHeaders})
ADD_DEPENDENCIES(shadercompiler ATOMX)
TARGET_LINK_LIBRARIES(shadercompiler ATOMX)

INSTALL_TARGET(shadercompiler)
