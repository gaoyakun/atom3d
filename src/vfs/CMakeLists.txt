INCLUDE_DIRECTORIES("${ATOM3D_INCLUDE_DIR}/vfs")
INCLUDE_DIRECTORIES("../zlib")

AUX_SOURCE_DIRECTORY(. VFSSrc)
FILE(GLOB VFSPrivateHeaders *.h)
FILE(GLOB VFSPublicHeaders ${ATOM3D_INCLUDE_DIR}/vfs/*.h)
SET(VFSSrcHeaders ${VFSSrc} ${VFSPrivateHeaders})
SOURCE_GROUP(headers FILES ${VFSPublicHeaders})
SOURCE_GROUP(source FILES ${VFSSrcHeaders})

IF(DEFINED PC_LINT_ENABLE)
	set(PC_LINT_OPT_DIR ${TOPLEVEL_DIR}/)
	set(PC_LINT_USER_FLAGS "-b")
	if (COMMAND add_pc_lint)
	  add_pc_lint(ATOM_vfs ${VFSSrcHeaders} ${VFSPublicHeaders})
	endif (COMMAND add_pc_lint)
ENDIF(DEFINED PC_LINT_ENABLE)

ADD_DEFINITIONS(-DBUILD_ATOM_VFS)
ADD_LIBRARY(ATOM_vfs ${ATOM3D_SHARED} ${VFSSrcHeaders} ${VFSPublicHeaders})
ADD_DEPENDENCIES(ATOM_vfs ATOM_dbghlp zlib_s)
TARGET_LINK_LIBRARIES(ATOM_vfs ATOM_dbghlp ATOM_lzmapkg zlib_s)

INSTALL_TARGET(ATOM_vfs)
