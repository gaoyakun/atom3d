INCLUDE_DIRECTORIES("${ATOM_INCLUDE_DIR}")
INCLUDE_DIRECTORIES("${ATOMX_INCLUDE_DIR}")
INCLUDE_DIRECTORIES("${THIRDPART_DIR}/include")
INCLUDE_DIRECTORIES("${THIRDPART_DIR}/include/tinyxml")

AUX_SOURCE_DIRECTORY(. ModelViewerSrc)
FILE(GLOB ModelViewerPrivateHeaders *.h)
SOURCE_GROUP(headers FILES ${ModelViewerPrivateHeaders})
SOURCE_GROUP(source FILES ${ModelViewerSrc})

ADD_EXECUTABLE(ATOM_modelviewer ${ModelViewerSrc} ${ModelViewerPrivateHeaders} ATOM_modelviewer.rc)
ADD_DEPENDENCIES(ATOM_modelviewer ATOM_kernel ATOM_engine ATOM_commondlg ATOM_render ATOM_archive ATOMX)
TARGET_LINK_LIBRARIES(ATOM_modelviewer ATOM_kernel ATOM_engine ATOM_commondlg ATOM_archive ATOMX psapi comdlg32)
SET_TARGET_PROPERTIES(ATOM_modelviewer PROPERTIES LINK_FLAGS "/MAP")

INSTALL(TARGETS ATOM_modelviewer RUNTIME DESTINATION ${DEPLOY_DIR})
