INCLUDE_DIRECTORIES("${ATOM3D_INCLUDE_DIR}/commondlg")

AUX_SOURCE_DIRECTORY(. CommonDlgSrc)
AUX_SOURCE_DIRECTORY(win CommonDlgWinSrc)
FILE(GLOB CommonDlgPrivateHeaders *.h win/*.h)
FILE(GLOB CommonDlgPublicHeaders ${ATOM3D_INCLUDE_DIR}/commondlg/*.h)
SET(CommonDlgSrcHeaders ${CommonDlgSrc} ${CommonDlgWinSrc} ${CommonDlgPrivateHeaders})
SOURCE_GROUP(headers FILES ${CommonDlgPublicHeaders})
SOURCE_GROUP(source FILES ${CommonDlgSrcHeaders})
SOURCE_GROUP(resources FILES ${CommonDlgResources})

SET(CommonDlgAllSrc ${CommonDlgSrcHeaders} ${CommonDlgPublicHeaders} ${CommonDlgResources})
ADD_DEFINITIONS(-DBUILD_ATOM_COMMONDLG)
ADD_LIBRARY(ATOM_commondlg ${ATOM3D_SHARED} ${CommonDlgSrcHeaders} ${CommonDlgPublicHeaders} ${CommonDlgResources})
ADD_DEPENDENCIES(ATOM_commondlg ATOM_kernel)
TARGET_LINK_LIBRARIES(ATOM_commondlg ATOM_kernel comctl32)

INSTALL_TARGET(ATOM_commondlg)
