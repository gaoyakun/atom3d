INCLUDE_DIRECTORIES("${ATOM3D_INCLUDE_DIR}")
IF (BUILD_WIN64)
	LINK_DIRECTORIES("${THIRDPART_DIR}/lib/dxsdk/x64")
ELSE (BUILD_WIN64)
	LINK_DIRECTORIES("${THIRDPART_DIR}/lib/dxsdk/x86")
ENDIF (BUILD_WIN64)

AUX_SOURCE_DIRECTORY(. RunScriptSrc)
FILE(GLOB RunScriptPrivateHeaders *.h)
SOURCE_GROUP(headers FILES ${RunScriptPrivateHeaders})
SOURCE_GROUP(source FILES ${RunScriptSrc})

ADD_EXECUTABLE(ATOM_runscript ${RunScriptSrc} ${RunScriptPrivateHeaders})
ADD_DEPENDENCIES(ATOM_runscript ATOM_kernel ATOM_engine ATOM_commondlg ATOM_render ATOMX)
TARGET_LINK_LIBRARIES(ATOM_runscript ATOM_kernel ATOM_engine ATOM_commondlg ATOMX psapi d3dx9 dxerr)

INSTALL_TARGET(ATOM_runscript)
